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

bb_move_set *bb_move_set_alloc()
{	
	return bb_array_alloc(10, sizeof(bb_move));
}

bb_move_set *bb_move_set_copy(bb_move_set *set) 
{
	return bb_array_copy(set);
}

bb_bool bb_move_sets_equal(bb_move_set *set, bb_move_set *other)
{
	return bb_array_equal(set, other);
}

void bb_move_set_dealloc(bb_move_set *set)
{
	bb_array_dealloc(set);
}

bb_index bb_move_set_length(bb_move_set *set)
{
	return bb_array_length(set);
}

bb_move bb_move_set_get_move(bb_move_set *set, bb_index move)
{
	bb_move mv;
	
	bb_array_get_item(set, move, &mv);
		
	return mv;
}

void bb_move_set_add_move(bb_move_set *set, bb_move move)
{
	bb_array_add_item(set, &move);
}

bb_move bb_create_move(bb_pawn pawn, bb_direction dir)
{
	bb_move move;
	
	move.pawn = pawn;
	move.direction = dir;
	
	return move;
}
