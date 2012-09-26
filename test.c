/*
 *  test.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/26/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "board.h"
#include "solver.h"
#include "fifo.h"
#include "strings.h"
#include "position_trie.h"

void test_boards();
void test_trie();
void generate_random_position(bb_pawn_state ps);
void test_solver();
void test_strings();

void test_boards()
{
	bb_board *board = bb_board_alloc(5, 5);
	bb_pawn_state ps;
	bb_cell *c;
	bb_dimension row, col;
	
	/* Testing board layout:
	   B _ / _ / (reflectors are Red and Green respectively)
	   _ _ / _ _ (reflector is Blue)
	   _ _ _ _ _ 
	   _ _ _ _ _ 
	   R _ _ _ _ 
	 */
	bb_init_pawn_state(ps);
	
	bb_move_pawn_to_location(ps, BB_PAWN_RED, 4, 0);
	bb_move_pawn_to_location(ps, BB_PAWN_BLUE, 0, 0);
	
	c = bb_get_cell(board, 0, 2);
	c->reflector = BB_PAWN_RED;
	c->reflector_direction = BB_45_DEGREES;
	
	c = bb_get_cell(board, 0, 4);
	c->reflector = BB_PAWN_GREEN;
	c->reflector_direction = BB_45_DEGREES;
	
	c = bb_get_cell(board, 1, 2);
	c->reflector = BB_PAWN_BLUE;
	c->reflector_direction = BB_45_DEGREES;
	
	bb_apply_move(board, ps, bb_create_move(BB_PAWN_RED, BB_DIRECTION_UP));
	bb_get_pawn_location(ps, BB_PAWN_RED, &row, &col);
	assert((row == 1) && (col == 0));
	
	bb_apply_move(board, ps, bb_create_move(BB_PAWN_BLUE, BB_DIRECTION_DOWN));
	bb_get_pawn_location(ps, BB_PAWN_RED, &row, &col);
	assert((row == 1) && (col == 0));
	bb_get_pawn_location(ps, BB_PAWN_BLUE, &row, &col);
	assert((row == 0) && (col == 0));
	
	bb_apply_move(board, ps, bb_create_move(BB_PAWN_RED, BB_DIRECTION_RIGHT));
	bb_get_pawn_location(ps, BB_PAWN_RED, &row, &col);
	assert((row == 0) && (col == 2));
	
	bb_apply_move(board, ps, bb_create_move(BB_PAWN_RED, BB_DIRECTION_RIGHT));
	bb_get_pawn_location(ps, BB_PAWN_RED, &row, &col);
	assert((row == 0) && (col == 4));
	
	bb_board_dealloc(board);
}

void generate_random_position(bb_pawn_state ps)
{
	bb_dimension r;
	unsigned i;
	
	for (i = 0; i < 5; i++) {
		r = random() & 0x01FF;
		ps[i].row = (r >= BB_MAX_DIMENSION) ? BB_NOT_FOUND : r;
		r = random() & 0x01FF;
		ps[i].col = (r >= BB_MAX_DIMENSION) ? BB_NOT_FOUND : r;
	}
}

void test_trie()
{
	bb_position_trie *trie = bb_position_trie_alloc();
	bb_array *array = bb_array_alloc(100000);
	bb_index index;
	bb_pawn_state *ps;
	
	/* Generate 100,000 random positions */
	for (index = 0; index <= 100000; index++) {
		ps = malloc(sizeof(bb_pawn_state));
		generate_random_position(*ps);
		bb_array_add_item(array, ps);
		bb_position_trie_add(trie, *ps);
	}
	
	/* Check that each of the known positions returns BB_TRUE */
	for (index = 0; index <= 100000; index++) {
		ps = (bb_pawn_state *)bb_array_get_item(array, index);
		
		assert(bb_position_trie_contains(trie, *ps));
	}
	
	/* Check that 100,000 unknown random positions return BB_FALSE */
	for (index = 0; index <= 100000; index++) {
		generate_random_position(*ps);
		
		if (bb_position_trie_contains(trie, *ps)) {
			bb_index k;
			bb_bool found = BB_FALSE;
			for (k = 0; k < bb_array_length(array); k++) {
				bb_pawn_state *nps = (bb_pawn_state *)bb_array_get_item(array, index);
				
				found = bb_pawn_states_equal(*ps, *nps);
				if (found) break;
			}
			
			assert(!found);
		}
	}
	
	/* Deallocate the positions */
	for (index = 0; index < bb_array_length(array); index++) {
		ps = (bb_pawn_state *)bb_array_get_item(array, index);
		free(ps);
	}
	
	bb_array_dealloc(array);
	bb_position_trie_dealloc(trie);
}

void test_solver()
{
	bb_board *board = bb_board_alloc(5, 5);
	bb_pawn_state ps;
	bb_cell *c;
	bb_fifo *fifo;
	bb_array *solutions;
	unsigned i;

	/* Testing board layout:
	 B _ / _ / (reflectors are Red and Green respectively)
	 _ _ / _ _ (reflector is Blue)
	 _ _ _ _ _ 
	 _ _ G _ 2 
	 R _ _ _ _ 
	 */
	
	bb_init_pawn_state(ps);
	bb_move_pawn_to_location(ps, BB_PAWN_RED, 4, 0);
	bb_move_pawn_to_location(ps, BB_PAWN_BLUE, 0, 0);
	bb_move_pawn_to_location(ps, BB_PAWN_GREEN, 3, 2);
	
	c = bb_get_cell(board, 0, 2);
	c->reflector = BB_PAWN_RED;
	c->reflector_direction = BB_45_DEGREES;
	
	c = bb_get_cell(board, 0, 4);
	c->reflector = BB_PAWN_GREEN;
	c->reflector_direction = BB_45_DEGREES;
	
	c = bb_get_cell(board, 1, 2);
	c->reflector = BB_PAWN_BLUE;
	c->reflector_direction = BB_45_DEGREES;
	
	c = bb_get_cell(board, 3, 4);
	c->token = 2;
	
	fifo = bb_find_solutions(board, ps, BB_PAWN_RED, 2, -1);
	solutions = bb_winnow_solutions(fifo);
	
	for (i = 0; i < bb_array_length(solutions); i++) {
		bb_move_set *set = bb_array_get_item(solutions, i);
		
		printf("Found solution ");
		bb_print_move_set(set);
		printf(", length %d\n", bb_move_set_length(set));
		bb_move_set_dealloc(set);
	}
	
	bb_array_dealloc(solutions);
	bb_fifo_dealloc(fifo);
}

void test_strings()
{
	char *board = "5 5\n{R}{}{}{}{2}\n{ }{}{}{}{ }\n{ }{}{}{}{ }\n{ }{/r}{}{}{ }\n{B}{G}{[}{}{S}";
	char *newboard;
	bb_board *b;
	bb_pawn_state ps;
	bb_pawn_state nps;
	bb_board *nb;
	
	bb_create_board_from_string(board, &b, ps);
	assert(b != NULL);
	
	bb_create_string_from_board(b, ps, &newboard);
	assert(newboard != NULL);
	
	bb_create_board_from_string(newboard, &nb, nps);
	assert(nb != NULL);
	
	/* Compare the board created from the inital string and the board created from the generated string */
	assert(b->width == nb->width);
	assert(b->height == nb->height);
	assert(memcmp(b->c, nb->c, b->width * b->height * sizeof(bb_cell)) == 0);
	assert(memcmp(ps, nps, sizeof(bb_pawn_state)) == 0);
	
	bb_board_dealloc(b);
	bb_board_dealloc(nb);
	free(newboard);
}

int main (int argc, char **argv)
{
	test_boards();
	test_strings();
	test_trie();
	test_solver();

	return 0;
}