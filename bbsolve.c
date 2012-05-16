/*
 *  bbsolve.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/12/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include "types.h"
#include "board.h"
#include "solver.h"
#include "strings.h"
#include "array.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
	int fd;
	int depth;
	off_t size;
	char *b;
	bb_board *board;
	bb_token token;
	bb_pawn pawn;
	bb_array *solutions;
	bb_fifo *fifo;
	unsigned i;
	
	iferror (argc < 5, "bbsolve usage: bbsolve (depth) (pawn) (token) file\n");
	
	fd = open(argv[4], O_RDONLY);	
	iferror (fd == -1, "Could not open file %s\n", argv[2]);
	
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	b = malloc(size + 1);
	iferror(b == NULL, "Could not allocate memory\n");
	memset(b, 0, size + 1);
	
	iferror(read(fd, b, size) != size, "Could not read file.\n");
	
	board = create_board_from_string(b);
	iferror(board == NULL, "Could not parse file.\n");
	
	depth = strtol(argv[1], NULL, 0);
	
	pawn = strtol(argv[2], NULL, 0);
	iferror((pawn < BB_PAWN_RED) || (pawn > BB_PAWN_SILVER), "Pawn value %d is out of range", pawn);
	
	token = strtol(argv[3], NULL, 0);
	iferror((token <= 0) || (token > BB_MAX_TOKEN), "Token value %d is out of range", token);
	
	fifo = bb_find_solutions(board, pawn, token, depth);
	solutions = winnow_solutions(fifo);
	bb_dealloc_fifo(fifo);
	
	for (i = 0; i < bb_array_length(solutions); i++) {
		bb_move_set *set = bb_array_get_item(solutions, i);
		
		printf("Found solution ");
		print_move_set(set);
		printf("\n");
		dealloc_move_set(set);
	}
	
	bb_array_dealloc(solutions);
	
	return 0;
	
}