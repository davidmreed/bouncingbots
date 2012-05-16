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

void add_states_to_fifo(bb_fifo *fifo, bb_solution_state *state);
bb_bool is_trivial_variant(bb_move_set *set, bb_move_set *comparand);
bb_bool is_unique_solution(bb_move_set *set, bb_array *uniques);

bb_solution_state *alloc_solution_state()
{
	bb_solution_state *state = malloc(sizeof(bb_solution_state));

	memset(state, 0, sizeof(bb_solution_state));
	
	return state;
}

bb_solution_state *copy_solution_state(bb_solution_state *state)
{
	bb_solution_state *ns = alloc_solution_state();
	
	ns->board = copy_board(state->board);
	ns->move_sequence = copy_move_set(state->move_sequence);
	
	return ns;
}

void dealloc_solution_state(bb_solution_state *state)
{
	dealloc_board(state->board);
	dealloc_move_set(state->move_sequence);
	free(state);
}

bb_fifo *bb_find_solutions(bb_board *board, bb_pawn pawn, bb_token token, int depth)
{
	/* Perform a breadth-first search for the shortest possible solution */
	bb_fifo *fifo = bb_create_fifo();
	bb_fifo *solutions = bb_create_fifo();
	bb_solution_state *state = alloc_solution_state();
	int min_solution = -1;
	
	state->board = copy_board(board);
	state->move_sequence = alloc_move_set(10);
	
	/* Add to the fifo a board for each potential move for each pawn */
	add_states_to_fifo(fifo, state);
	
	dealloc_solution_state(state);
	
	state = (bb_solution_state *)bb_fifo_pop(fifo);
	while (state != NULL) {
		if (!is_board_target(state->board, pawn, token)) {
			if (depth <= 0) {
				/* If we are searching for the shortest solution, we do not want to descend another ply if we've already found a shorter solution than is possible */
				if (min_solution > 0) {
					if (move_set_length(state->move_sequence) < min_solution)
						add_states_to_fifo(fifo, state);
				} else {
					add_states_to_fifo(fifo, state);
				}
			} else {
				/* If we are searching to a set depth, continue if appropriate */
				if (move_set_length(state->move_sequence) < depth) {
					printf("Current depth = %d, max = %d, %lu states\n", move_set_length(state->move_sequence), depth, bb_fifo_length(fifo));
					add_states_to_fifo(fifo, state);
				}
			}
			dealloc_solution_state(state);
		} else {
			/* Found a solution. If it's the first we've found, it's also the shortest, but that doesn't mean it is the only solution of a given length. */
			if (depth <= 0) {
				/* Depth <= 0 means find the shortest solution(s) */
				if (min_solution == -1) {
					min_solution = move_set_length(state->move_sequence);
					bb_fifo_append(solutions, state);
				} else {
					if (move_set_length(state->move_sequence) == min_solution) {
						bb_fifo_append(solutions, state);
					}
				}
			} else {
				/* A positive depth means find all solutions up to and including depth in length */
				bb_fifo_append(solutions, state);
			}
		}
		
		state = (bb_solution_state *)bb_fifo_pop(fifo);
	}

	return solutions;
}

void add_states_to_fifo(bb_fifo *fifo, bb_solution_state *state)
{
	bb_pawn p;
	bb_direction dir;
	bb_cell *cur_cell;
	unsigned cur_row, cur_col;
	
	for (p = BB_PAWN_RED; p <= BB_PAWN_SILVER; p++) {
		cur_cell = locate_pawn(state->board, p, &cur_row, &cur_col);
		if (cur_cell == NULL) continue; /* skip any pawn not in use this game */
		
		for (dir = BB_DIRECTION_UP; dir <= BB_DIRECTION_LEFT; dir++) {
			unsigned final_row, final_col;
			
			get_landing_point(state->board, p, dir, &final_row, &final_col);
			
			if ((final_col != cur_col) || (final_row != cur_row)) {
				/* This move has an effect on the board */
				bb_solution_state *ns = copy_solution_state(state);
				
				bb_apply_move(ns->board, bb_create_move(p, dir));
				move_set_add_move(ns->move_sequence, bb_create_move(p, dir));
																
				bb_fifo_append(fifo, ns);
			}
		}
	}
}

bb_array *winnow_solutions(bb_fifo *solutions)
{
	bb_array *unique_solutions = bb_array_alloc(10);
	bb_solution_state *state;
	
	state = bb_fifo_pop(solutions);
	while (state != NULL) {
		/* Choose each solution and eliminate all solutions longer than it which share the same skeleton (the same moves in the same order). These are trivial variations. */
		/* This relies on the FIFO containing solutions in ascending order of length */
		if (is_unique_solution(state->move_sequence, unique_solutions)) {
			bb_move_set *set = copy_move_set(state->move_sequence);
			bb_array_add_item(unique_solutions, set);
		}
		dealloc_solution_state(state);
		state = bb_fifo_pop(solutions);
	}
	
	return unique_solutions;
}

bb_bool is_unique_solution(bb_move_set *set, bb_array *uniques)
{
	unsigned i;
		
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
	
	for (i = 0; i < move_set_length(comparand); i++) {
		bb_move move = move_set_get_move(comparand, i);
		bb_bool found = BB_FALSE;
		
		/* iterate from the current position until the end of set looking for this move */
		for (; position < move_set_length(set); position++) {
			bb_move other = move_set_get_move(set, position);
			
			if ((other.pawn == move.pawn) && (other.direction == move.direction)) {
				found = BB_TRUE;
				break;
			}
		}
		
		if (!found) return BB_FALSE;
	}
	
	return BB_TRUE;
}

void print_move_set (bb_move_set *set)
{
	unsigned char *str;
	
	create_string_from_move_set(set, &str);
	
	if (str != NULL) {
		printf("%s", str);
		free(str);
	} else {
		printf("Move set %p yielded a NULL string representation.\n", set);
	}
}