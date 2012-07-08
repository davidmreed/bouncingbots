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
	bb_pawn_state ps;
	bb_move_set *move_sequence;
} bb_solution_state;

bb_solution_state *bb_solution_state_alloc();
bb_solution_state *bb_solution_state_copy(bb_solution_state *state);
void bb_solution_state_dealloc(bb_solution_state *state);

bb_fifo *bb_find_solutions(bb_board *in_board, bb_pawn_state ps, bb_pawn pawn, bb_token token, int depth);
bb_array *bb_winnow_solutions(bb_fifo *solutions);

#endif