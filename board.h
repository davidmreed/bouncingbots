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
	bb_cell *c;
} bb_board;

bb_board *alloc_board(unsigned width, unsigned height);
bb_board *copy_board(bb_board *board);
void dealloc_board(bb_board *board);

void get_landing_point(bb_board *board, bb_pawn pawn, bb_direction dir, unsigned *out_row, unsigned *out_col);
bb_cell *get_cell(bb_board *board, unsigned row, unsigned col);
void get_collision_point(bb_board *board, bb_pawn pawn, bb_direction dir, unsigned *out_row, unsigned *out_col);
unsigned get_collision_point_vertical(bb_board *board, bb_pawn pawn, unsigned col, unsigned row, bb_direction dir);
unsigned get_collision_point_horizontal(bb_board *board, bb_pawn pawn, unsigned row, unsigned col, bb_direction dir);

bb_cell *locate_pawn(bb_board *board, bb_pawn pawn, unsigned *out_row, unsigned *out_col);

void bb_apply_move_set(bb_board *board, bb_move_set *set);
void bb_apply_move(bb_board *board, bb_move move);

bb_bool is_board_target(bb_board *board, bb_pawn pawn, bb_token token);

#endif