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
	unsigned pawn:6;
	unsigned token:8;
} bb_cell;

typedef struct {
	unsigned width, height;
	struct _pawnloc { bb_cell *cell; unsigned row, col; } pawns[5];
	bb_cell *c;
} bb_board;

bb_board *bb_board_alloc(unsigned width, unsigned height);
bb_board *bb_board_copy(bb_board *board);
void bb_board_dealloc(bb_board *board);

void bb_get_landing_point(bb_board *board, bb_pawn pawn, bb_direction dir, unsigned *out_row, unsigned *out_col);
bb_cell *bb_get_cell(bb_board *board, unsigned row, unsigned col);

bb_cell *bb_locate_pawn(bb_board *board, bb_pawn pawn, unsigned *out_row, unsigned *out_col);
void bb_move_pawn(bb_board *board, bb_pawn pawn, unsigned row, unsigned col, bb_cell *cell);
void bb_move_pawn_to_cell(bb_board *board, bb_pawn pawn, bb_cell *cell);
void bb_move_pawn_to_location(bb_board *board, bb_pawn pawn, unsigned row, unsigned col);
void bb_get_cell_location(bb_board *board, bb_cell *cell, unsigned *row, unsigned *col);

void bb_apply_move_set(bb_board *board, bb_move_set *set);
void bb_apply_move(bb_board *board, bb_move move);

bb_bool bb_is_board_target(bb_board *board, bb_pawn pawn, bb_token token);

#endif