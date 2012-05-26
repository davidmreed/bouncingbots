/*
 *  board.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/1/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include "board.h"
#include <string.h>
#include <stdlib.h>

void get_collision_point(bb_board *board, bb_pawn pawn, bb_direction dir, unsigned *out_row, unsigned *out_col);
unsigned get_collision_point_vertical(bb_board *board, bb_pawn pawn, unsigned col, unsigned row, bb_direction dir);
unsigned get_collision_point_horizontal(bb_board *board, bb_pawn pawn, unsigned row, unsigned col, bb_direction dir);

bb_direction reflect(bb_cell *c, bb_direction dir);
bb_bool can_leave_cell_in_direction(bb_cell *cell, bb_direction dir);
bb_bool can_enter_cell_in_direction(bb_cell *cell, bb_direction dir);

bb_board *bb_board_alloc(unsigned width, unsigned height)
{
	if ((width < BB_MAX_DIMENSION) && (height < BB_MAX_DIMENSION)) {
		bb_board *board = malloc(sizeof(bb_board));
		
		if (board != NULL) {
			board->c = malloc(sizeof(bb_cell) * width * height);
			
			if (board->c != NULL) {
				unsigned i;
				
				memset(board->c, 0, sizeof(bb_cell) * width * height);
				board->width = width;
				board->height = height;
				
				for (i = 0; i < 5; i++) {
					board->pawns[i].cell = NULL;
					board->pawns[i].col = BB_NOT_FOUND;
					board->pawns[i].row = BB_NOT_FOUND;
				}
				
				return board;
			} else {
				free(board);
			}
		}
	}
	
	return NULL;
}

bb_board *bb_board_copy(bb_board *board)
{
	bb_board *nb = bb_board_alloc(board->width, board->height);
	unsigned i;
	
	memcpy(nb->c, board->c, sizeof(bb_cell) * board->width * board->height);
	
	for (i = 0; i < 5; i++) {
		nb->pawns[i].row = board->pawns[i].row;
		nb->pawns[i].col = board->pawns[i].col;
		nb->pawns[i].cell = bb_get_cell(nb, nb->pawns[i].row, nb->pawns[i].col);
	}
	
	return nb;
}

void bb_board_dealloc(bb_board *board)
{
	free(board->c);
	free(board);
}

bb_cell *bb_get_cell(bb_board *board, unsigned row, unsigned col)
{
	if ((row > board->height) || (col > board->width)) return NULL;
	
	return &(board->c[row * board->width + col]);
}

bb_direction reflect(bb_cell *c, bb_direction dir)
{
	if (c->reflector_direction == BB_45_DEGREES) {
		/* Reflector oriented like this: / */
		if (dir == BB_DIRECTION_UP)
			return BB_DIRECTION_RIGHT;
		else if (dir == BB_DIRECTION_RIGHT)
			return BB_DIRECTION_UP;
		else if (dir == BB_DIRECTION_LEFT)
			return BB_DIRECTION_DOWN;
		else if (dir == BB_DIRECTION_DOWN)
			return BB_DIRECTION_LEFT;
	} else {
		/* Reflector oriented like this : \ */
		if (dir == BB_DIRECTION_UP) 
			return BB_DIRECTION_LEFT;
		else if (dir == BB_DIRECTION_LEFT)
			return BB_DIRECTION_UP;
		else if (dir == BB_DIRECTION_DOWN)
			return BB_DIRECTION_RIGHT;
		else if (dir == BB_DIRECTION_RIGHT) 
			return BB_DIRECTION_DOWN;
	}
	
	return dir;
}

bb_bool can_leave_cell_in_direction(bb_cell *cell, bb_direction dir)
{
	if (((dir == BB_DIRECTION_UP) && (cell->wall_top == BB_WALL)) ||
		((dir == BB_DIRECTION_RIGHT) && (cell->wall_right == BB_WALL)) ||
		((dir == BB_DIRECTION_DOWN) && (cell->wall_bottom == BB_WALL)) ||
		((dir == BB_DIRECTION_LEFT) && (cell->wall_left == BB_WALL)))
		return BB_FALSE;

	return BB_TRUE;
}

bb_bool can_enter_cell_in_direction(bb_cell *cell, bb_direction dir)
{
	if ((cell->pawn != 0) ||
		(cell->block == BB_WALL))
		return BB_FALSE;
	
	if (((dir == BB_DIRECTION_UP) && (cell->wall_bottom == BB_WALL)) ||
		((dir == BB_DIRECTION_RIGHT) && (cell->wall_left == BB_WALL)) ||
		((dir == BB_DIRECTION_DOWN) && (cell->wall_top == BB_WALL)) ||
		((dir == BB_DIRECTION_LEFT) && (cell->wall_right == BB_WALL)))
		return BB_FALSE;
	
	return BB_TRUE;
	
}

void bb_get_landing_point(bb_board *board, bb_pawn pawn, bb_direction dir, unsigned *out_row, unsigned *out_col)
{
	unsigned row, col;
	bb_cell *c;
	bb_cell *cur = bb_locate_pawn(board, pawn, NULL, NULL);
	
	get_collision_point(board, pawn, dir, &row, &col);
	
	c = bb_get_cell(board, row, col);
	
	if (c == cur) {
		*out_col = col;
		*out_row = row;
		return;
	}
	
	if ((c->reflector != 0) && (c->reflector != pawn)) {
		bb_board *nb = bb_board_copy(board);
		bb_cell *nc;
		
		nc = bb_get_cell(nb, row, col);
		
		/* Go ahead and move the pawn to the reflector location */
		bb_move_pawn_to_location(nb, pawn, row, col);
		
		bb_get_landing_point(nb, pawn, reflect(nc, dir), &row, &col);
		bb_board_dealloc(nb);
	}
	
	*out_col = col;
	*out_row = row;
}

void get_collision_point(bb_board *board, bb_pawn pawn, bb_direction dir, unsigned *out_row, unsigned *out_col)
{
	unsigned row, col;
	bb_cell *cur;
	
	bb_locate_pawn(board, pawn, &row, &col);
	cur = bb_get_cell(board, row, col);

	if (cur == NULL) {
		*out_row = *out_col = BB_NOT_FOUND;
		return;
	}
	
	/* Check if the pawn can leave this cell in the given direction */
	if (!can_leave_cell_in_direction(cur, dir)) {
		*out_row = row;
		*out_col = col;
		return;
	}
	
	if ((dir == BB_DIRECTION_UP) || (dir == BB_DIRECTION_DOWN)) {
		*out_row = get_collision_point_vertical(board, pawn, col, row, dir);
		*out_col = col;
	} else {
		*out_col = get_collision_point_horizontal(board, pawn, row, col, dir);
		*out_row = row;
	}
}

unsigned get_collision_point_vertical(bb_board *board, bb_pawn pawn, unsigned col, unsigned row, bb_direction dir)
{
	int i, max = (dir == BB_DIRECTION_UP) ? 0 : board->height, inc = (dir == BB_DIRECTION_UP) ? -1 : 1;
	
	for (i = row + inc; ((dir == BB_DIRECTION_UP) ? i >= max : i < max); i+= inc) {
		bb_cell *c = bb_get_cell(board, i, col);
		
		if ((c->reflector != 0) && (c->reflector != pawn)) {
			return i;
		}
				
		if (!can_enter_cell_in_direction(c, dir))
			return i + -inc;

		if (!can_leave_cell_in_direction(c, dir))
			return i;
	}
	
	return (max != 0) ? max - 1 : 0;
}


unsigned get_collision_point_horizontal(bb_board *board, bb_pawn pawn, unsigned row, unsigned col, bb_direction dir)
{
	int i, max = (dir == BB_DIRECTION_LEFT) ? 0 : board->width;
	int inc = (dir == BB_DIRECTION_LEFT) ? -1 : 1;
	
	for (i = col + inc; ((dir == BB_DIRECTION_LEFT) ? i >= max : i < max); i+= inc) {
		bb_cell *c = bb_get_cell(board, row, i);
		
		if ((c->reflector != 0) && (c->reflector != pawn)) {
			return i;
		}

		if (!can_enter_cell_in_direction(c, dir))
			return i + -inc;		

		if (!can_leave_cell_in_direction(c, dir))
			return i;
	}
	
	return (max != 0) ? max - 1 : 0;
}


bb_cell *bb_locate_pawn(bb_board *board, bb_pawn pawn, unsigned *out_row, unsigned *out_col)
{
	if (out_row != NULL) *out_row = board->pawns[pawn - 1].row;
	if (out_col != NULL) *out_col = board->pawns[pawn - 1].col;

	return board->pawns[pawn - 1].cell;
}

void bb_get_cell_location(bb_board *board, bb_cell *cell, unsigned *row, unsigned *col)
{
	unsigned i, j;
	
	for (i = 0; i < board->width; i++) {
		for (j = 0; j < board->height; j++) {
			bb_cell *c = bb_get_cell(board, j, i);
			
			if (c == cell) {
				if (row != NULL) *row = j;
				if (col != NULL) *col = i;
				return;
			}
		}
	}
	
	if (row != NULL) *row = BB_NOT_FOUND;
	if (col != NULL) *col = BB_NOT_FOUND;
}	

void bb_move_pawn(bb_board *board, bb_pawn pawn, unsigned row, unsigned col, bb_cell *cell)
{
	bb_cell *cur = board->pawns[pawn - 1].cell;
	
	if (cur != NULL) cur->pawn = 0;
	
	cell->pawn = pawn;
	board->pawns[pawn - 1].cell = cell;
	board->pawns[pawn - 1].row = row;
	board->pawns[pawn - 1].col = col;
}	

void bb_move_pawn_to_location(bb_board *board, bb_pawn pawn, unsigned row, unsigned col)
{
	bb_move_pawn(board, pawn, row, col, bb_get_cell(board, row, col));
}

void bb_move_pawn_to_cell(bb_board *board, bb_pawn pawn, bb_cell *cell)
{
	unsigned row, col;
	
	bb_get_cell_location(board, cell, &row, &col);
	bb_move_pawn(board, pawn, row, col, cell);
}

bb_bool bb_is_board_target(bb_board *board, bb_pawn pawn, bb_token token) 
{
	bb_cell *cell = bb_locate_pawn(board, pawn, NULL, NULL);
	
	if ((cell != NULL) && (cell->token == token))
		return BB_TRUE;
	
	return BB_FALSE;
}

void bb_apply_move_set(bb_board *board, bb_move_set *set)
{
	unsigned i;
	
	for (i = 0; i < bb_move_set_length(set); i++) 
		bb_apply_move(board, bb_move_set_get_move(set, i));
}

void bb_apply_move(bb_board *board, bb_move move)
{
	unsigned final_row, final_col;
	
	bb_get_landing_point(board, move.pawn, move.direction, &final_row, &final_col);
	bb_move_pawn_to_location(board, move.pawn, final_row, final_col);
}
