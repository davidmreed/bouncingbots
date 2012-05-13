/*
 *  move.h
 *  bouncingbots
 *
 *  Created by David Reed on 5/1/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#ifndef BB_MOVE_H
#define BB_MOVE_H

#include "types.h"

typedef struct {
	unsigned pawn:6;
	unsigned direction:2;
} bb_move;

typedef struct {
	unsigned length, allocated;
	bb_move *moves;
} bb_move_set;

bb_move_set *alloc_move_set(unsigned length);
bb_move_set *copy_move_set(bb_move_set *set);
bb_bool move_sets_equal(bb_move_set *set, bb_move_set *other);
void dealloc_move_set(bb_move_set *set);

unsigned move_set_length(bb_move_set *set);
bb_move move_set_get_move(bb_move_set *set, unsigned move);
void move_set_add_move(bb_move_set *set, bb_move move);

bb_move bb_create_move(bb_pawn pawn, bb_direction dir);

#endif