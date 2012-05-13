/*
 *  move.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/1/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include "move.h"
#include <stdlib.h>

bb_move_set *alloc_move_set(unsigned length)
{
	bb_move_set *move_set = malloc(sizeof(bb_move_set));
	
	if (move_set != NULL) {
		unsigned len = ((length == 0) ? 10 : length);
		move_set->moves = malloc(sizeof(bb_move) * len);
		
		if (move_set->moves != NULL) {
			move_set->length = 0; 
			move_set->allocated = len;
			return move_set;
		} else {
			free(move_set);
			return NULL;
		}
	}
	
	return NULL;
}

bb_move_set *copy_move_set(bb_move_set *set) 
{
	bb_move_set *ns = alloc_move_set(set->length);
	int i;
	
	for (i = 0; i < move_set_length(set); i++) 
		move_set_add_move(ns, move_set_get_move(set, i));
		
	return ns;
}

bb_bool move_sets_equal(bb_move_set *set, bb_move_set *other)
{
	if (move_set_length(set) == move_set_length(other)) {
		int i;
		for (i = 0; i < move_set_length(set); i++) {
			bb_move one, two;
			
			one = move_set_get_move(set, i);
			two = move_set_get_move(other, i);
			
			if ((one.pawn != two.pawn) || (one.direction != two.direction))
				return BB_FALSE;
		}
		
		return BB_TRUE;
	}
	
	return BB_FALSE;
}

void dealloc_move_set(bb_move_set *set)
{
	free(set->moves);
	free(set);
}

unsigned move_set_length(bb_move_set *set)
{
	return set->length;
}

bb_move move_set_get_move(bb_move_set *set, unsigned move)
{
	if (move < set->length)
		return set->moves[move];
	
	bb_move none = {0, 0};
	return none;
}

void move_set_add_move(bb_move_set *set, bb_move move)
{
	if (set->allocated >= set->length + 1) {
		set->moves[set->length] = move;
		set->length += 1;
	} else {
		set->moves = realloc(set->moves, sizeof(bb_move) * set->length + 10);
		set->allocated += 10;
		set->moves[set->length] = move;
		set->length += 1;
	}
}

bb_move bb_create_move(bb_pawn pawn, bb_direction dir)
{
	bb_move move;
	
	move.pawn = pawn;
	move.direction = dir;
	
	return move;
}
