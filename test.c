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
#include "types.h"
#include "board.h"
#include "solver.h"
#include "fifo.h"
#include "strings.h"
#include "position_trie.h"

void test_boards();
void test_trie();
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

void test_trie()
{
	bb_position_trie *trie = bb_position_trie_alloc();
	bb_pawn_state ps;
	
	bb_init_pawn_state(ps);
	
	bb_move_pawn_to_location(ps, BB_PAWN_RED, 2, 3);
	bb_move_pawn_to_location(ps, BB_PAWN_BLUE, 4, 3);
	bb_move_pawn_to_location(ps, BB_PAWN_YELLOW, 1, 0);
	bb_position_trie_add(trie, ps);
	assert(bb_position_trie_contains(trie, ps));
	bb_move_pawn_to_location(ps, BB_PAWN_RED, 0, 0);
	assert(!bb_position_trie_contains(trie, ps));
	
	bb_move_pawn_to_location(ps, BB_PAWN_SILVER, 4, 4);
	assert(!bb_position_trie_contains(trie, ps));
	bb_position_trie_add(trie, ps);
	assert(bb_position_trie_contains(trie, ps));
	
	bb_move_pawn_to_location(ps, BB_PAWN_GREEN, 1, 1);
	bb_position_trie_add(trie, ps);
	assert(bb_position_trie_contains(trie, ps));
	bb_position_trie_add(trie, ps);
	assert(bb_position_trie_contains(trie, ps));

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
}

int main (int argc, char **argv)
{
	test_boards();
	test_trie();
	test_solver();
	test_strings();

	return 0;
}