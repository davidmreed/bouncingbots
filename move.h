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
#include "array.h"

typedef struct {
	unsigned pawn:6;
	unsigned direction:2;
} bb_move;

typedef bb_array bb_move_set;

bb_move_set *bb_move_set_alloc();
bb_move_set *bb_move_set_copy(bb_move_set *set);
bb_bool bb_move_sets_equal(bb_move_set *set, bb_move_set *other);
void bb_move_set_dealloc(bb_move_set *set);

bb_index bb_move_set_length(bb_move_set *set);
bb_move bb_move_set_get_move(bb_move_set *set, bb_index move);
void bb_move_set_add_move(bb_move_set *set, bb_move move);

bb_move bb_create_move(bb_pawn pawn, bb_direction dir);

#endif