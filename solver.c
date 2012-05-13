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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void add_states_to_fifo(bb_fifo *fifo, bb_solution_state *state);
void print_move_set (bb_move_set *set);

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
		
		if (move_sets_equal(state->move_sequence, create_move_set_from_string("rrgugugl", 8))) {
			printf("Found the right solution");
		}
		
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
				if (move_set_length(state->move_sequence) < depth) 
					add_states_to_fifo(fifo, state);
			}
			dealloc_solution_state(state);
		} else {
			/* Found a solution. If it's the first we've found, it's also the shortest, but that doesn't mean it is the only solution of a given length. */
			printf("We think we found a solution: ");
			print_move_set(state->move_sequence);
			printf("\n");
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
								
				//printf("Adding solution state with move sequence: ");
				//print_move_set(ns->move_sequence);
				//printf("\n");
								
				bb_fifo_append(fifo, ns);
			}
		}
	}
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