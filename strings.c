/*
 *  strings.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/2/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include "strings.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

char *read_token(const char *str, bb_cell *out_cell, bb_bool *out_bool);
char *read_cell(const char *str, bb_cell *out_cell, bb_bool *out_bool, bb_pawn *out_pawn);
char *skip_whitespace (const char *chrptr);
char *append_char(char *instr, char aChar, size_t *size, size_t *position);
char *append_integer(char *instr, int anInt, size_t *size, size_t *position);

bb_move_set *bb_create_move_set_from_string(const char *str, unsigned length)
{
	unsigned len = (length % 2 == 0) ? length : length - 1;
	unsigned i;
	bb_move_set *set = bb_move_set_alloc(len / 2);
	
	for (i = 0; i < len; i+=2) {
		char c = str[i];
		bb_move move = {0, 0};
		
		if (c == 'r')
			move.pawn = BB_PAWN_RED;
		else if (c == 'b')
			move.pawn = BB_PAWN_BLUE;
		else if (c == 'g')
			move.pawn = BB_PAWN_GREEN;
		else if (c == 'y')
			move.pawn = BB_PAWN_YELLOW;
		else if (c == 's')
			move.pawn = BB_PAWN_SILVER;
		else {
			bb_move_set_dealloc(set);
			return NULL;
		}
		
		c = str[i + 1];
		if (c == 'u')
			move.direction = BB_DIRECTION_UP;
		else if (c == 'r')
			move.direction = BB_DIRECTION_RIGHT;
		else if (c == 'd')
			move.direction = BB_DIRECTION_DOWN;
		else if (c == 'l')
			move.direction = BB_DIRECTION_LEFT;
		else {
			bb_move_set_dealloc(set);
			return NULL;
		}
		
		bb_move_set_add_move(set, move);
	}
	
	return set;
}

void bb_create_string_from_move_set(bb_move_set *set, unsigned char **out_str)
{
	unsigned length = bb_move_set_length(set), i;
	char *pawns = " rbgys";
	char *dirs = "urdl";
	
	*out_str = malloc(length * 2 + 1);
	memset(*out_str, 0, length * 2 + 1);
	
	for (i = 0; i < length; i++) {
		bb_move move = bb_move_set_get_move(set, i);

		*(*out_str + i * 2) = pawns[move.pawn]; 
		*(*out_str + (i * 2) + 1) = dirs[move.direction];
	}
	*(*out_str + length * 2) = '\0';
}

void bb_print_move_set (bb_move_set *set)
{
	unsigned char *str;
	
	bb_create_string_from_move_set(set, &str);
	
	if (str != NULL) {
		printf("%s", str);
		free(str);
	} else {
		printf("Move set %p yielded a NULL string representation.\n", (void *)set);
	}
}

char *skip_whitespace (const char *chrptr) 
{
	char *aptr = (char *)chrptr;
	
	while ((*aptr != '\0') && isspace(*aptr)) aptr++;

	return aptr;
}

char *read_token(const char *str, bb_cell *out_cell, bb_bool *out_bool)
{
	long token;
	char *cur;
	
	token = strtol(str, &cur, 0);
	
	if ((token < 0) || (token > BB_MAX_TOKEN)) {
		*out_bool = BB_FALSE;
		return cur;
	}

	out_cell->token = token;
	*out_bool = BB_TRUE;
	
	return cur;
}

char *read_cell(const char *str, bb_cell *out_cell, bb_bool *out_bool, bb_pawn *out_pawn)
{
	char *cur = skip_whitespace(str);
	bb_bool success = BB_TRUE;
	
	if (*cur == '{') {
		/* The opening character of a cell */
		cur++;
		while ((*cur != 0x00) && (*cur != '}') && success) {
			if      (isspace(*cur)) ;
			else if (*cur == 'R') *out_pawn = BB_PAWN_RED;
			else if (*cur == 'B') *out_pawn = BB_PAWN_BLUE;
			else if (*cur == 'G') *out_pawn = BB_PAWN_GREEN;
			else if (*cur == 'Y') *out_pawn = BB_PAWN_YELLOW;
			else if (*cur == 'S') *out_pawn = BB_PAWN_SILVER;
			else if (*cur == '*') out_cell->block       = BB_WALL;
			else if (*cur == '[') out_cell->wall_left   = BB_WALL;
			else if (*cur == ']') out_cell->wall_right  = BB_WALL;
			else if (*cur == '^') out_cell->wall_top    = BB_WALL;
			else if (*cur == '_') out_cell->wall_bottom = BB_WALL;
			else if (isdigit(*cur)) { cur = read_token(cur, out_cell, &success); cur--; }
			else if (*cur == '/') out_cell->reflector_direction = BB_45_DEGREES;
			else if (*cur == '\\') out_cell->reflector_direction = BB_135_DEGREES;
			else if (*cur == 'r') out_cell->reflector = BB_REFLECTOR_RED;
			else if (*cur == 'b') out_cell->reflector = BB_REFLECTOR_BLUE;
			else if (*cur == 'g') out_cell->reflector = BB_REFLECTOR_GREEN;
			else if (*cur == 'y') out_cell->reflector = BB_REFLECTOR_YELLOW;
			else if (*cur == 's') out_cell->reflector = BB_REFLECTOR_SILVER;
			else success = BB_FALSE;
			
			cur++;
		}
		
		cur = skip_whitespace(cur);
		if (*cur != '}') {
			success = BB_FALSE;
		} else {
			cur++;
		}
	} else {
		success = BB_FALSE;
	}
	
	*out_bool = success;
	return cur;
}

void bb_create_board_from_string(const char *str, bb_board **b, bb_pawn_state ps)
{
	bb_board *board;
	long width, height, i, j;
	char *cur;
	
	width = strtol(str, &cur, 0);
	height = strtol(cur, &cur, 0);
	
	if ((width < 1) || (height < 1) || (width > BB_MAX_DIMENSION) || (height > BB_MAX_DIMENSION)) {
		*b = NULL;
		return;
	}
	
	board = bb_board_alloc(width, height);
	if (board == NULL) {
		*b = NULL;
		return;
	}
	bb_init_pawn_state(ps);
	
	cur = skip_whitespace(cur);
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {	
			bb_bool success;
			bb_pawn pawn;
			bb_cell *cell = bb_get_cell(board, i, j);
			
			pawn = 0;
			cur = read_cell(cur, cell, &success, &pawn);
						
			if (!success) {
				bb_board_dealloc(board);
				*b = NULL;
				return;
			}
			
			/* Make sure we record where the pawn is */
			if (pawn != 0) {
				ps[pawn - 1].row = i;
				ps[pawn - 1].col = j;
			}
		}
	}
	
	*b = board;
}

char *append_char(char *instr, char aChar, size_t *size, size_t *position)
{
	char *ret = instr;
	
	if (instr == NULL) return NULL;
	
	if ((*size - *position) < 10) {
		ret = realloc(instr, *size * 2);
		*size *= 2;
	}
	
	ret[*position] = aChar;
	*position += 1;
	
	return ret;
}

char *append_integer(char *instr, int anInt, size_t *size, size_t *position)
{
	char *ret = instr;
	int retval;
	
	if (instr == NULL) return NULL;
	
	if ((*size - *position) < 10) {
		ret = realloc(instr, *size * 2);
		*size *= 2;
	}
	
	retval = snprintf(ret + *position, *size - *position, "%d", anInt);
	
	if (retval > *size - *position) {
		ret = realloc(ret, *size * 2);
		*size *= 2;
		
		retval = snprintf(ret + *position, *size - *position, "%d", anInt);

		if (retval > *size - *position) {
			return NULL;
		}		
	}
	
	*position += retval;
	
	return ret;
}

void bb_create_string_from_board(bb_board *board, bb_pawn_state ps, char **out_str)
{
	size_t size = 3 + 1 + 3 + 1 + board->width * board->height * 13 + board->height;
	unsigned i, j;
	size_t position = 0;
	char *const pawns = " RBGYS";
	char *const reflectors = " rbgys";
	char *const reflector_directions = "/\\";
	char *b;

	if (out_str == NULL) return;

	b = malloc(size);
	if (b == NULL) {
		*out_str = NULL;
		return;
	}
	memset(b, 0, size);
	
	b = append_integer(b, board->width, &size, &position);
	b = append_char(b, ' ', &size, &position);
	b = append_integer(b, board->height, &size, &position);
	b = append_char(b, '\n', &size, &position);
		
	for (i = 0; i < board->height; i++) {
		for (j = 0; j < board->width; j++) {
			bb_cell *cell = bb_get_cell(board, i, j);
			
			b = append_char(b, '{', &size, &position);
			
			if (cell->block == BB_WALL)	      b = append_char(b, '*', &size, &position);
			if (cell->wall_left == BB_WALL)   b = append_char(b, '[', &size, &position);
			if (cell->wall_top == BB_WALL)	  b = append_char(b, '^', &size, &position);
			if (bb_pawn_at_location(ps, i, j) != 0)			
										      b = append_char(b, pawns[bb_pawn_at_location(ps, i, j)], &size, &position);
			if (cell->token != 0)			  b = append_integer(b, cell->token, &size, &position);
			if (cell->reflector != 0)		  b = append_char(b, reflectors[cell->reflector], &size, &position);
			if (cell->reflector != 0)		  b = append_char(b, reflector_directions[cell->reflector_direction], &size, &position);
			if (cell->wall_bottom == BB_WALL) b = append_char(b, '_', &size, &position);
			if (cell->wall_right == BB_WALL)  b = append_char(b, ']', &size, &position);
			
			b = append_char(b, '}', &size, &position);
		}
		
		b = append_char(b, '\n', &size, &position);
	}
	
	*out_str = b;
}
					 
