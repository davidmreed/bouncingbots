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
#include <strings.h>
#include <getopt.h>

static struct option opts[] = {
	{ "depth", required_argument, NULL, 'd' },
	{ "pawn", required_argument, NULL, 'p' },
	{ "token", required_argument, NULL, 't' },
	{ "any-depth", no_argument, NULL, 'a' },
	{ NULL, 0, NULL, 0 }
};

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

void usage()
{
	iferror(BB_TRUE, "bbsolve usage: bbsolve [--depth [-d] depth | --any-depth [-a]] --pawn [-p] pawn --token [-t] token BOARD\n");
}

int main (int argc, char **argv)
{
	int fd;
	int depth = -1;
	off_t size;
	char *b;
	bb_board *board;
	bb_pawn_state ps;
	bb_token token;
	bb_pawn pawn = 0;
	bb_array *solutions;
	bb_fifo *fifo;
	unsigned i;
	char opt;
	
	while ((opt = getopt_long(argc, argv, "ad:p:t:", opts, NULL)) != -1) {
		if (opt == 'd') {
			depth = strtol(optarg, NULL, 0);
		} else if (opt == 'a') {
			depth = -1;
		} else if (opt == 'p') {
			if (strncasecmp(optarg, "red", 3) == 0) {
				pawn = BB_PAWN_RED;
			} else if (strncasecmp(optarg, "blue", 4) == 0) {
				pawn = BB_PAWN_BLUE;
			} else if (strncasecmp(optarg, "green", 5) == 0) {
				pawn = BB_PAWN_GREEN;
			} else if (strncasecmp(optarg, "yellow", 6) == 0) {
				pawn = BB_PAWN_YELLOW;
			} else if (strncasecmp(optarg, "silver", 6) == 0) {
				pawn = BB_PAWN_SILVER;
			} else {
				usage();
			}
		} else if (opt == 't') {
			token = strtol(optarg, NULL, 0);
			iferror((token <= 0) || (token > BB_MAX_TOKEN), "Token value %d is out of range", token);
		} else {
			usage();
		}
	}
	
	argc -= optind;
	argv += optind;
	
	if (argc >= 1) {
		fd = open(argv[0], O_RDONLY);	
		iferror (fd == -1, "Could not open file %s\n", argv[2]);
	} else {
		usage();
	}

	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	b = malloc(size + 1);
	iferror(b == NULL, "Could not allocate memory\n");
	memset(b, 0, size + 1);
	
	iferror(read(fd, b, size) != size, "Could not read file.\n");
	
	bb_create_board_from_string(b, &board, ps);
	free(b);
	iferror(board == NULL, "Could not parse file.\n");
	
	fifo = bb_find_solutions(board, ps, pawn, token, depth);
	solutions = bb_winnow_solutions(fifo);
	bb_fifo_dealloc(fifo);
	
	for (i = 0; i < bb_array_length(solutions); i++) {
		bb_move_set *set = (bb_move_set *)bb_array_get_item_p(solutions, i);
		
		printf("Found solution ");
		bb_print_move_set(set);
		printf(", length %d\n", bb_move_set_length(set));
		bb_move_set_dealloc(set);
	}
	
	bb_array_dealloc(solutions);
	bb_board_dealloc(board);
	
	return 0;
	
}
