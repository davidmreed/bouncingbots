/*
 *  solver.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/1/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include "solver.h"
#include "move.h"
#include "strings.h"
#include "array.h"
#include "position_trie.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void add_states_to_fifo(bb_fifo *fifo, bb_board *board, bb_solution_state *state, bb_position_trie *knownpositions);
bb_bool is_trivial_variant(bb_move_set *set, bb_move_set *comparand);
bb_bool is_unique_solution(bb_move_set *set, bb_array *uniques);

bb_solution_state *bb_solution_state_alloc()
{
	bb_solution_state *state = malloc(sizeof(bb_solution_state));

	memset(state, 0, sizeof(bb_solution_state));
	bb_init_pawn_state(state->ps);
	
	return state;
}

bb_solution_state *bb_solution_state_copy(bb_solution_state *state)
{
	bb_solution_state *ns = bb_solution_state_alloc();
	
	bb_copy_pawn_state(state->ps, ns->ps);
	ns->move_sequence = bb_move_set_copy(state->move_sequence);
	
	return ns;
}

void bb_solution_state_dealloc(bb_solution_state *state)
{
	bb_move_set_dealloc(state->move_sequence);
	free(state);
}

bb_fifo *bb_find_solutions(bb_board *in_board, bb_pawn_state ps, bb_pawn pawn, bb_token token, int depth)
{
	/* Perform a breadth-first search for the shortest possible solution */
	bb_fifo *fifo = bb_fifo_alloc();
	bb_fifo *solutions = bb_fifo_alloc();
	bb_solution_state *state = bb_solution_state_alloc();
	bb_position_trie *knownpositions = bb_position_trie_alloc();
	bb_board *board = bb_board_copy(in_board); /* Copy the board for thread safety */
	int min_solution = -1;
	
	bb_copy_pawn_state(ps, state->ps);
	state->move_sequence = bb_move_set_alloc(10);
	
	/* Add the initial state to the known positions list */
	bb_position_trie_add(knownpositions, state->ps);
	
	/* Add to the fifo a board for each potential move for each pawn */
	add_states_to_fifo(fifo, board, state, knownpositions);
	
	bb_solution_state_dealloc(state);
	
	state = (bb_solution_state *)bb_fifo_pop(fifo);
	while (state != NULL) {
		if (!bb_is_board_target(board, state->ps, pawn, token)) {
			if (depth <= 0) {
				/* If we are searching for the shortest solution, we do not want to descend another ply if we've already found a shorter solution than is possible */
				if (min_solution > 0) {
					if (bb_move_set_length(state->move_sequence) < min_solution)
						add_states_to_fifo(fifo, board, state, knownpositions);
				} else {
#ifdef DEBUG
					printf("Current depth = %d, max = %d, %lu states.\n", bb_move_set_length(state->move_sequence), depth, (unsigned long)bb_fifo_length(fifo));
#endif
					add_states_to_fifo(fifo, board, state, knownpositions);
				}
			} else {
				/* If we are searching to a set depth, continue if appropriate */
				if (bb_move_set_length(state->move_sequence) < depth) {
#ifdef DEBUG
					printf("Current depth = %d, max = %d, %lu states.\n", bb_move_set_length(state->move_sequence), depth, (unsigned long)bb_fifo_length(fifo));
#endif
					add_states_to_fifo(fifo, board, state, knownpositions);
				}
			}
			bb_solution_state_dealloc(state);
		} else {
			/* Found a solution. If it's the first we've found, it's also the shortest, but that doesn't mean it is the only solution of a given length. */
			if (depth <= 0) {
				/* Depth <= 0 means find the shortest solution(s) */
				if (min_solution == -1) {
					min_solution = bb_move_set_length(state->move_sequence);
					bb_fifo_append(solutions, state);
				} else {
					if (bb_move_set_length(state->move_sequence) == min_solution) {
						bb_fifo_append(solutions, state);
					} else {
						bb_solution_state_dealloc(state);
					}
				}
			} else {
				/* A positive depth means find all solutions up to and including depth in length */
				bb_fifo_append(solutions, state);
			}
		}
		
		state = (bb_solution_state *)bb_fifo_pop(fifo);
	}
	
	bb_fifo_dealloc(fifo);
	bb_board_dealloc(board);
	bb_position_trie_dealloc(knownpositions);

	return solutions;
}

void add_states_to_fifo(bb_fifo *fifo, bb_board *board, bb_solution_state *state, bb_position_trie *knownpositions)
{
	bb_pawn p;
	bb_direction dir;
	bb_dimension cur_row, cur_col;
	
	for (p = BB_PAWN_RED; p <= BB_PAWN_SILVER; p++) {
		bb_get_pawn_location(state->ps, p, &cur_row, &cur_col);
		
		if (cur_row == BB_NOT_FOUND) continue; /* skip any pawn not in use this game */
		
		for (dir = BB_DIRECTION_UP; dir <= BB_DIRECTION_LEFT; dir++) {
			bb_dimension final_row, final_col;
			
			bb_get_landing_point(board, state->ps, p, dir, &final_row, &final_col);
			
			if ((final_col != cur_col) || (final_row != cur_row)) {
				/* This move has an effect on the board */
				bb_solution_state *ns = bb_solution_state_copy(state);
				
				bb_apply_move(board, ns->ps, bb_create_move(p, dir));
				bb_move_set_add_move(ns->move_sequence, bb_create_move(p, dir));
				
				/* Before we add this to the state list, check to make sure we have not reached this position by other lines */
				if (bb_position_trie_contains(knownpositions, ns->ps)) {
					bb_solution_state_dealloc(ns);
				} else {
					bb_fifo_append(fifo, ns);
					bb_position_trie_add(knownpositions, ns->ps);
				}
			}
		}
	}
}

bb_array *bb_winnow_solutions(bb_fifo *solutions)
{
	bb_array *unique_solutions = bb_array_alloc(10);
	bb_solution_state *state;
	
	state = bb_fifo_pop(solutions);
	while (state != NULL) {
		/* Choose each solution and eliminate all solutions longer than it which share the same skeleton (the same moves in the same order). These are trivial variations. */
		/* This relies on the FIFO containing solutions in ascending order of length */
		if (is_unique_solution(state->move_sequence, unique_solutions)) {
			bb_move_set *set = bb_move_set_copy(state->move_sequence);
			bb_array_add_item(unique_solutions, set);
		}
		bb_solution_state_dealloc(state);
		state = bb_fifo_pop(solutions);
	}
	
	return unique_solutions;
}

bb_bool is_unique_solution(bb_move_set *set, bb_array *uniques)
{
	bb_index i;
		
	for (i = 0; i < bb_array_length(uniques); i++) {
		if (is_trivial_variant(set, bb_array_get_item(uniques, i))) {
			return BB_FALSE;
		}
	}
	
	return BB_TRUE;
	
}

bb_bool is_trivial_variant(bb_move_set *set, bb_move_set *comparand)
{
	/* If set contains the same moves in the same sequence as comparand (regardless of intervening moves), it is considered a trivial variant and will be eliminated from the list */
	/* length(comparand) <= length (set) */
	
	unsigned i, position = 0;
	
	for (i = 0; i < bb_move_set_length(comparand); i++) {
		bb_move move = bb_move_set_get_move(comparand, i);
		bb_bool found = BB_FALSE;
		
		/* iterate from the current position until the end of set looking for this move */
		for (; position < bb_move_set_length(set); position++) {
			bb_move other = bb_move_set_get_move(set, position);
			
			if ((other.pawn == move.pawn) && (other.direction == move.direction)) {
				found = BB_TRUE;
				break;
			}
		}
		
		if (!found) return BB_FALSE;
	}
	
	return BB_TRUE;
}
