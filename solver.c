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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void add_states_to_fifo(bb_fifo *fifo, bb_solution_state *state, bb_array *knownpositions);
bb_bool is_trivial_variant(bb_move_set *set, bb_move_set *comparand);
bb_bool is_unique_solution(bb_move_set *set, bb_array *uniques);
bb_bool is_known_position(bb_solution_state *state, bb_array *knownpositions);
typedef unsigned char *bb_pawn_state;

bb_pawn_state pawn_state_for_solution_state(bb_solution_state *board);

bb_pawn_state pawn_state_for_solution_state(bb_solution_state *board)
{
	bb_pawn_state ps = malloc(10);
	unsigned row, col;
	bb_pawn pawn;

	if (ps != NULL) {
		memset(ps, 0, 10);
	
		for (pawn = BB_PAWN_RED; pawn <= BB_PAWN_SILVER; pawn++) {
			bb_locate_pawn(board->board, pawn, &row, &col);
			ps[(pawn - 1) * 2] = (unsigned char)(row & 0xFF);
			ps[(pawn - 1) * 2 + 1] = (unsigned char)(col & 0xFF);
		}
	}
	
	return ps;
}

bb_bool pawn_states_equal(bb_pawn_state one, bb_pawn_state other)
{
	return memcmp(one, other, 10) == 0 ? BB_TRUE : BB_FALSE;
}

bb_solution_state *bb_solution_state_alloc()
{
	bb_solution_state *state = malloc(sizeof(bb_solution_state));

	memset(state, 0, sizeof(bb_solution_state));
	
	return state;
}

bb_solution_state *bb_solution_state_copy(bb_solution_state *state)
{
	bb_solution_state *ns = bb_solution_state_alloc();
	
	ns->board = bb_board_copy(state->board);
	ns->move_sequence = bb_move_set_copy(state->move_sequence);
	
	return ns;
}

void bb_solution_state_dealloc(bb_solution_state *state)
{
	bb_board_dealloc(state->board);
	bb_move_set_dealloc(state->move_sequence);
	free(state);
}

bb_fifo *bb_find_solutions(bb_board *board, bb_pawn pawn, bb_token token, int depth)
{
	/* Perform a breadth-first search for the shortest possible solution */
	bb_fifo *fifo = bb_fifo_alloc();
	bb_array *knownpositions = bb_array_alloc(512);
	bb_fifo *solutions = bb_fifo_alloc();
	bb_solution_state *state = bb_solution_state_alloc();
	int min_solution = -1;
	bb_index i;
	
	state->board = bb_board_copy(board);
	state->move_sequence = bb_move_set_alloc(10);
	
	/* Add the initial state to the known positions list */
	bb_array_add_item(knownpositions, pawn_state_for_solution_state(state));
	
	/* Add to the fifo a board for each potential move for each pawn */
	add_states_to_fifo(fifo, state, knownpositions);
	
	bb_solution_state_dealloc(state);
	
	state = (bb_solution_state *)bb_fifo_pop(fifo);
	while (state != NULL) {
		if (!bb_is_board_target(state->board, pawn, token)) {
			if (depth <= 0) {
				/* If we are searching for the shortest solution, we do not want to descend another ply if we've already found a shorter solution than is possible */
				if (min_solution > 0) {
					if (bb_move_set_length(state->move_sequence) < min_solution)
						add_states_to_fifo(fifo, state, knownpositions);
				} else {
#ifdef DEBUG
					printf("Current depth = %d, max = %d, %lu states, %lu known positions (%lu allocated).\n", bb_move_set_length(state->move_sequence), depth, (unsigned long)bb_fifo_length(fifo), (unsigned long)bb_array_length(knownpositions), (unsigned long)knownpositions->allocated);
#endif
					add_states_to_fifo(fifo, state, knownpositions);
				}
			} else {
				/* If we are searching to a set depth, continue if appropriate */
				if (bb_move_set_length(state->move_sequence) < depth) {
#ifdef DEBUG
					printf("Current depth = %d, max = %d, %lu states, %lu known positions (%lu allocated).\n", bb_move_set_length(state->move_sequence), depth, (unsigned long)bb_fifo_length(fifo), (unsigned long)bb_array_length(knownpositions), (unsigned long)knownpositions->allocated);
#endif
					add_states_to_fifo(fifo, state, knownpositions);
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
	
	/* Deallocate the knownpositions array */
	
	for (i = 0; i < bb_array_length(knownpositions); i++)
		free(bb_array_get_item(knownpositions, i));
	
	bb_array_dealloc(knownpositions);

	return solutions;
}

void add_states_to_fifo(bb_fifo *fifo, bb_solution_state *state, bb_array *knownpositions)
{
	bb_pawn p;
	bb_direction dir;
	bb_cell *cur_cell;
	unsigned cur_row, cur_col;
	
	for (p = BB_PAWN_RED; p <= BB_PAWN_SILVER; p++) {
		cur_cell = bb_locate_pawn(state->board, p, &cur_row, &cur_col);
		
		if (cur_cell == NULL) continue; /* skip any pawn not in use this game */
		
		for (dir = BB_DIRECTION_UP; dir <= BB_DIRECTION_LEFT; dir++) {
			unsigned final_row, final_col;
			
			/* Somehow we are getting a crash here when the silver pawn's data gets corrupted */
			/* And we end up thinking there is a silver pawn and trying to dereference a null pointer */
			bb_get_landing_point(state->board, p, dir, &final_row, &final_col);
			
			if ((final_col != cur_col) || (final_row != cur_row)) {
				/* This move has an effect on the board */
				bb_solution_state *ns = bb_solution_state_copy(state);
				
				bb_apply_move(ns->board, bb_create_move(p, dir));
				bb_move_set_add_move(ns->move_sequence, bb_create_move(p, dir));
				
				/* Before we add this to the state list, check to make sure we have not reached this position by other lines */
				if (is_known_position(ns, knownpositions)) {
					bb_solution_state_dealloc(ns);
				} else {
					bb_fifo_append(fifo, ns);
					bb_array_add_item(knownpositions, pawn_state_for_solution_state(ns));
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

bb_bool is_known_position(bb_solution_state *ss, bb_array *knownpositions)
{
	bb_pawn_state state = pawn_state_for_solution_state(ss);
	bb_index i;

	for (i = 0; i < bb_array_length(knownpositions); i++) {
		bb_pawn_state ps = (bb_pawn_state)bb_array_get_item(knownpositions, i);

		if (pawn_states_equal(ps, state)) {	
			free(state);
			return BB_TRUE;
		}
	}
	
	free(state);
	
	return BB_FALSE;
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
