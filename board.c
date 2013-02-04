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

void get_collision_point(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_direction dir, bb_dimension *out_row, bb_dimension *out_col);
bb_dimension get_collision_point_vertical(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_dimension col, bb_dimension row, bb_direction dir);
bb_dimension get_collision_point_horizontal(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_dimension row, bb_dimension col, bb_direction dir);

bb_direction reflect(bb_cell *c, bb_direction dir);
bb_bool can_leave_cell_in_direction(bb_cell *cell, bb_direction dir);
bb_bool can_enter_cell_in_direction(bb_cell *cell, bb_direction dir);

bb_board *bb_board_alloc(bb_dimension width, bb_dimension height)
{
	if ((width <= BB_MAX_DIMENSION) && (height <= BB_MAX_DIMENSION)) {
		bb_board *board = malloc(sizeof(bb_board));
		
		if (board != NULL) {
			board->c = calloc(width * height, sizeof(bb_cell));
			
			if (board->c != NULL) {				
				board->width = width;
				board->height = height;
								
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
	
	memcpy(nb->c, board->c, sizeof(bb_cell) * board->width * board->height);
		
	return nb;
}

void bb_board_dealloc(bb_board *board)
{
	free(board->c);
	free(board);
}

void bb_init_pawn_state(bb_pawn_state ps)
{
	unsigned i;
	
	for (i = 0; i < 5; i++) {
		ps[i].row = BB_NOT_FOUND;
		ps[i].col = BB_NOT_FOUND;
	}
}

bb_bool bb_pawn_states_equal(bb_pawn_state ps, bb_pawn_state nps)
{
	return (memcmp(ps, nps, sizeof(bb_pawn_state)) == 0) ? BB_TRUE : BB_FALSE;
}
			
void bb_copy_pawn_state(bb_pawn_state ps, bb_pawn_state nps)
{
	memcpy(nps, ps, sizeof(bb_pawn_state));
}

bb_path *bb_path_alloc()
{
	return bb_array_alloc(5, sizeof(bb_dimension) * 2);
}

bb_path *bb_path_copy(bb_path *path)
{
	return (bb_path *)bb_array_copy(path);
}

void bb_path_dealloc(bb_path *path)
{
	bb_array_dealloc(path);
}

void bb_path_add_position(bb_path *path, bb_dimension row, bb_dimension col)
{
	bb_dimension pos[2];
	
	pos[0] = row; pos[1] = col;
	
	bb_array_add_item(path, pos);
}
	
void bb_path_get_position(bb_path *path, bb_index position, bb_dimension *row, bb_dimension *col)
{
	bb_dimension pos[2];
	
	bb_array_get_item(path, position, (void *)pos);
	
	if (row != NULL) *row = pos[0];
	if (col != NULL) *col = pos[1];
}

bb_cell *bb_get_cell(bb_board *board, bb_dimension row, bb_dimension col)
{
	if ((row > board->height) || (col > board->width)) return NULL;
	
	return &(board->c[row * board->width + col]);
}

void bb_get_pawn_location(bb_pawn_state ps, bb_pawn pawn, bb_dimension *out_row, bb_dimension *out_col)
{
	if ((pawn >= BB_PAWN_RED) && (pawn <= BB_PAWN_SILVER)) {
		if (out_row != NULL) *out_row = ps[pawn - 1].row;
		if (out_col != NULL) *out_col = ps[pawn - 1].col;
	} else {
		if (out_row != NULL) *out_row = BB_NOT_FOUND;
		if (out_col != NULL) *out_col = BB_NOT_FOUND;
	}
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
	if (cell->block == BB_WALL)
		return BB_FALSE;
	
	if (((dir == BB_DIRECTION_UP) && (cell->wall_bottom == BB_WALL)) ||
		((dir == BB_DIRECTION_RIGHT) && (cell->wall_left == BB_WALL)) ||
		((dir == BB_DIRECTION_DOWN) && (cell->wall_top == BB_WALL)) ||
		((dir == BB_DIRECTION_LEFT) && (cell->wall_right == BB_WALL)))
		return BB_FALSE;
	
	return BB_TRUE;
	
}

void bb_get_landing_point(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_direction dir, bb_dimension *out_row, bb_dimension *out_col, bb_path *path)
{
	bb_dimension row, col;
	bb_cell *c;
	bb_cell *cur = bb_get_cell_for_pawn(board, ps, pawn);
	
	get_collision_point(board, ps, pawn, dir, &row, &col);
	
	c = bb_get_cell(board, row, col);
	
	if (c == cur) {
		*out_col = col;
		*out_row = row;
		return;
	}
	
	if ((c->reflector != 0) && (c->reflector != pawn)) {
		bb_pawn_state nps;
		
		bb_copy_pawn_state(ps, nps);
		
		/* Go ahead and move the pawn to the reflector location */
		bb_move_pawn_to_location(nps, pawn, row, col);
		if (path != NULL) bb_path_add_position(path, row, col);
		
		bb_get_landing_point(board, nps, pawn, reflect(c, dir), &row, &col, path);
	} else {
		if (path != NULL) bb_path_add_position(path, row, col);

	}

	*out_col = col;
	*out_row = row;
}

void get_collision_point(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_direction dir, bb_dimension *out_row, bb_dimension *out_col)
{
	bb_dimension row, col;
	bb_cell *cur;
	
	bb_get_pawn_location(ps, pawn, &row, &col);
	cur = bb_get_cell_for_pawn(board, ps, pawn);

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
		*out_row = get_collision_point_vertical(board, ps, pawn, col, row, dir);
		*out_col = col;
	} else {
		*out_col = get_collision_point_horizontal(board, ps, pawn, row, col, dir);
		*out_row = row;
	}
}

bb_dimension get_collision_point_vertical(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_dimension col, bb_dimension row, bb_direction dir)
{
	int i, max = (dir == BB_DIRECTION_UP) ? 0 : board->height, inc = (dir == BB_DIRECTION_UP) ? -1 : 1;
	
	for (i = row + inc; ((dir == BB_DIRECTION_UP) ? i >= max : i < max); i+= inc) {
		bb_cell *c = bb_get_cell(board, i, col);
		
		if ((c->reflector != 0) && (c->reflector != pawn)) {
			return i;
		}
				
		if (!can_enter_cell_in_direction(c, dir) || bb_is_pawn_at_location(ps, i, col))
			return i + -inc;

		if (!can_leave_cell_in_direction(c, dir))
			return i;
	}
	
	return (max != 0) ? max - 1 : 0;
}


bb_dimension get_collision_point_horizontal(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_dimension row, bb_dimension col, bb_direction dir)
{
	int i, max = (dir == BB_DIRECTION_LEFT) ? 0 : board->width;
	int inc = (dir == BB_DIRECTION_LEFT) ? -1 : 1;
	
	for (i = col + inc; ((dir == BB_DIRECTION_LEFT) ? i >= max : i < max); i+= inc) {
		bb_cell *c = bb_get_cell(board, row, i);
		
		if ((c->reflector != 0) && (c->reflector != pawn)) {
			return i;
		}

		if (!can_enter_cell_in_direction(c, dir) || bb_is_pawn_at_location(ps, row, i))
			return i + -inc;		

		if (!can_leave_cell_in_direction(c, dir))
			return i;
	}
	
	return (max != 0) ? max - 1 : 0;
}


bb_bool bb_is_pawn_at_location(bb_pawn_state ps, bb_dimension row, bb_dimension col)
{
	unsigned i;
	
	for (i = 0; i < 5; i++)
		if ((ps[i].row == row) && (ps[i].col == col))
			return BB_TRUE;
	
	return BB_FALSE;
}

bb_pawn bb_pawn_at_location(bb_pawn_state ps, bb_dimension row, bb_dimension col)
{
	unsigned i;
	
	for (i = 0; i < 5; i++)
		if ((ps[i].row == row) && (ps[i].col == col))
			return i + 1;
	
	return 0;
}

void bb_get_cell_location(bb_board *board, bb_cell *cell, bb_dimension *row, bb_dimension *col)
{
	bb_dimension i, j;
	
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

bb_cell *bb_get_cell_for_pawn(bb_board *board, bb_pawn_state ps, bb_pawn pawn)
{
	bb_dimension row, col;
	
	bb_get_pawn_location(ps, pawn, &row, &col);
	if ((row != BB_NOT_FOUND) && (col != BB_NOT_FOUND))
		return bb_get_cell(board, row, col);
	
	return NULL;
}

bb_cell *bb_get_cell_for_token(bb_board *board, bb_token token)
{
	bb_dimension row, col;
	
	bb_get_token_location(board, token, &row, &col);
	
	if ((row != BB_NOT_FOUND) && (col != BB_NOT_FOUND))
		return bb_get_cell(board, row, col);
	
	return NULL;
}

void bb_get_token_location(bb_board *board, bb_token token, bb_dimension *out_row, bb_dimension *out_col)
{
	bb_dimension i, j;
	
	if (token != 0) {
		for (i = 0; i < board->width; i++) {
			for (j = 0; j < board->height; j++) {
				bb_cell *c = bb_get_cell(board, j, i);
				
				if (c->token == token) {
					if (out_row != NULL) *out_row = j;
					if (out_col != NULL) *out_col = i;
					return;
				}
			}
		}
	}
	
	if (out_row != NULL) *out_row = BB_NOT_FOUND;
	if (out_col != NULL) *out_col = BB_NOT_FOUND;
}

void bb_move_pawn_to_location(bb_pawn_state ps, bb_pawn pawn, bb_dimension row, bb_dimension col)
{
	if ((pawn > 0) && (pawn < 6)) {
		ps[pawn - 1].row = row;
		ps[pawn - 1].col = col;
	}
}

bb_bool bb_is_board_target(bb_board *board, bb_pawn_state ps, bb_pawn pawn, bb_token token) 
{
	bb_dimension row, col;
	bb_cell *cell;
	
	bb_get_pawn_location(ps, pawn, &row, &col);
	cell = bb_get_cell(board, row, col);
	
	if ((cell != NULL) && (cell->token == token))
		return BB_TRUE;
	
	return BB_FALSE;
}

void bb_apply_move_set(bb_board *board, bb_pawn_state ps, bb_move_set *set)
{
	unsigned i;
	
	for (i = 0; i < bb_move_set_length(set); i++) 
		bb_apply_move(board, ps, bb_move_set_get_move(set, i));
}

void bb_apply_move(bb_board *board, bb_pawn_state ps, bb_move move)
{
	bb_dimension final_row, final_col;
	
	bb_get_landing_point(board, ps, move.pawn, move.direction, &final_row, &final_col, NULL);
	bb_move_pawn_to_location(ps, move.pawn, final_row, final_col);
}
