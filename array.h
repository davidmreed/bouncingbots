/*
 *  array.h
 *  bouncingbots
 *
 *  Created by David Reed on 5/13/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#ifndef BB_ARRAY_H
#define BB_ARRAY_H

#include "types.h"

typedef struct {
	bb_index length, allocated;
	void **items;
} bb_array;

bb_array *bb_array_alloc(bb_index length);
bb_array *bb_array_copy(bb_array *set);
bb_bool bb_array_equal(bb_array *set, bb_array *other);
void bb_array_dealloc(bb_array *set);

bb_index bb_array_length(bb_array *set);
void *bb_array_get_item(bb_array *set, bb_index index);
void bb_array_add_item(bb_array *set, void *move);

#endif