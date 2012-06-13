/*
 *  board.h
 *  bouncingbots
 *
 *  Created by David Reed on 5/1/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#ifndef BB_BOARD_H
#define BB_BOARD_H

#include "move.h"
#include "types.h"

typedef struct {
	unsigned wall_top:1;
	unsigned wall_bottom:1;
	unsigned wall_right:1;
	unsigned wall_left:1;
	unsigned block:1;
	unsigned reflector:3;
	unsigned reflector_direction:2;
	unsigned token:6;
} bb_cell;

typedef struct {
	bb_dimension width, height;
	bb_cell *c;
} bb_board;

typedef struct { bb_dimension row, col; } bb_pawn_state[5];

bb_board *bb_board_alloc(bb_dimension width, bb_dimension height);
bb_board *bb_board_copy(bb_board *board);
void bb_board_dealloc(bb_board *board);

void bb_init_pawn_state(bb_pawn_state ps);
void bb_copy_pawn_state(bb_pawn_state ps, bb_pawn_state nps);
bb_bool bb_pawn_states_equal(bb_pawn_state ps, bb_pawn_state nps);

void bb_get_landing_point(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_direction dir, bb_dimension *out_row, bb_dimension *out_col);
void bb_move_pawn_to_location(bb_pawn_state ps, bb_pawn pawn, bb_dimension row, bb_dimension col);

void bb_apply_move_set(bb_board *board, bb_pawn_state ps, bb_move_set *set);
void bb_apply_move(bb_board *board, bb_pawn_state ps, bb_move move);

bb_cell *bb_get_cell(bb_board *board, bb_dimension row, bb_dimension col);
void bb_get_cell_location(bb_board *board, bb_cell *cell, bb_dimension *row, bb_dimension *col);
bb_cell *bb_get_cell_for_pawn(bb_board *board, bb_pawn_state ps, bb_pawn pawn);
void bb_get_pawn_location(bb_pawn_state ps, bb_pawn pawn, bb_dimension *out_row, bb_dimension *out_col);
bb_bool bb_is_pawn_at_location(bb_pawn_state ps, bb_dimension row, bb_dimension col);

bb_bool bb_is_board_target(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_token token);

#endif