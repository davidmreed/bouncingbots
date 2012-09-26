/*
 *  main.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/2/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */


#include "types.h"
#include "board.h"
#include "move.h"
#include "solver.h"
#include "strings.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define CHECK_PAWN(tBoard, tPawn, tRow, tCol) { unsigned mRow, mCol; (void)locate_pawn(tBoard, tPawn, &mRow, &mCol); if ((mRow == tRow) && (mCol == tCol)) printf("Found pawn %d where expected\n", tPawn); else printf("Found pawn %d at unexpected location (%d, %d)\n", tPawn, mRow, mCol);}

int main (int argc, char **argv);

void iferror(bb_bool error, const char *errstr, ...) 
{
	if (error) {
		va_list list;
		
		va_start(list, errstr);
		vprintf(errstr, list);
		va_end(list);
		
		exit(-1);
	}
}

int main (int argc, char **argv)
{
	/* Initial setup of 5x5 board: (* = token 2)
	   R _ _ _ *
	   _ _ _ _ _
	   _ _ _ _ _
	   _ / _ _ _
	   B G|_ _ S */
	/* We move Red right, Silver left, and Green up, in that order. */
	/* Expected result:
	   _ _ _ _ R
	   _ _ _ _ _
	   _ _ _ _ _
	   _ / _ _ G
	   B _|S _ _*/

	bb_board *board;// = alloc_board(5, 5);
	bb_move_set *set;
	bb_cell *cell;
	
	/*cell = get_cell(board, 0, 0);
	cell->pawn = BB_PAWN_RED;
	
	cell = get_cell(board, 4, 4);
	cell->pawn = BB_PAWN_SILVER;

	cell = get_cell(board, 4, 1);
	cell->wall_right = BB_WALL;

	cell = get_cell(board, 4, 0);
	cell->pawn = BB_PAWN_BLUE;

	cell = get_cell(board, 3, 1);
	cell->reflector = BB_REFLECTOR_RED;
	cell->reflector_direction = BB_45_DEGREES;
	
	cell = get_cell(board, 4, 1);
	cell->pawn = BB_PAWN_GREEN;
	
	cell = get_cell(board, 0, 3);
	cell->pawn = BB_PAWN_YELLOW;
	
	cell = get_cell(board, 0, 4);
	cell->token = 2;*/
	
	int fd;
	off_t size;
	char *b;
	
	fd = open("/Users/davidreed/Documents/Projects/bouncingbots/Board.bb", O_RDONLY);	
	iferror (fd == -1, "Could not open file %s\n", argv[2]);
	
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	b = malloc(size + 1);
	iferror(b == NULL, "Could not allocate memory\n");
	memset(b, 0, size + 1);
	
	iferror(read(fd, b, size) != size, "Could not read file.\n");
	
	board = create_board_from_string(b);
	iferror(board == NULL, "Could not parse file.\n");
	
	
	bb_fifo *fifo = bb_find_solutions(board, BB_PAWN_GREEN, 2, 5);
	bb_solution_state *state;
	
	state = (bb_solution_state *)bb_fifo_pop(fifo);
	
	if (state != NULL) {
		unsigned char *setstr;

		set = state->move_sequence;
		
		if (set != NULL) {
			create_string_from_move_set(set, &setstr);
			printf("Solution: %s, length %d\n", setstr, move_set_length(set));
			free(setstr);
		} else {
			printf("NULL move sequence\n");
		}

		dealloc_solution_state(state);
	} else {
		printf("No solutions found\n");
	}
	
	set = create_move_set_from_string("rrbrslgu", 8);
	if (set != NULL) {
		bb_apply_move_set(board, set);
		print_move_set(set);
		printf("\n");
		dealloc_move_set(set);
	} else {
		printf("could not parse move set\n");
	}

	CHECK_PAWN(board, BB_PAWN_RED, 0, 4)
	CHECK_PAWN(board, BB_PAWN_SILVER, 4, 2)
	CHECK_PAWN(board, BB_PAWN_BLUE, 4, 0)
	CHECK_PAWN(board, BB_PAWN_GREEN, 3, 4)
	
	exit(0);
}