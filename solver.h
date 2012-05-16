/*
 *  solver.h
 *  bouncingbots
 *
 *  Created by David Reed on 5/1/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#ifndef BB_SOLVER_H
#define BB_SOLVER_H

#include "board.h"
#include "types.h"
#include "fifo.h"
#include "array.h"

typedef struct {
	bb_board *board;
	bb_move_set *move_sequence;
} bb_solution_state;

bb_solution_state *alloc_solution_state();
bb_solution_state *copy_solution_state(bb_solution_state *state);
void dealloc_solution_state(bb_solution_state *state);

bb_fifo *bb_find_solutions(bb_board *board, bb_pawn pawn, bb_token token, int depth);
bb_array *winnow_solutions(bb_fifo *solutions);
void print_move_set (bb_move_set *set);

#endif