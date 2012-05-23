/*
 *  array.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/13/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include "array.h"
#include <stdlib.h>
#include <string.h>

bb_array *bb_array_alloc(bb_index length)
{
	bb_array *array = malloc(sizeof(bb_array));
	
	if (array != NULL) {
		bb_index len = ((length == 0) ? 10 : length);
		
		array->items = malloc(sizeof(void *) * len);
		
		if (array->items != NULL) {
			array->length = 0; 
			array->allocated = len;
			return array;
		} else {
			free(array);
			return NULL;
		}
	}
	
	return NULL;
}

bb_array *bb_array_copy(bb_array *set) 
{
	bb_array *ns = bb_array_alloc(bb_array_length(set));
	int i;
	
	for (i = 0; i < bb_array_length(set); i++) 
		bb_array_add_item(ns, bb_array_get_item(set, i));
	
	return ns;
}

bb_bool bb_array_equal(bb_array *set, bb_array *other)
{
	if (bb_array_length(set) == bb_array_length(other)) {
		int i;
		for (i = 0; i < bb_array_length(set); i++) {
			void *one, *two;
			
			one = bb_array_get_item(set, i);
			two = bb_array_get_item(other, i);
			
			if (one != two)
				return BB_FALSE;
		}
		
		return BB_TRUE;
	}
	
	return BB_FALSE;
}

void bb_array_dealloc(bb_array *set)
{
	free(set->items);
	free(set);
}

bb_index bb_array_length(bb_array *set)
{
	return set->length;
}

void *bb_array_get_item(bb_array *set, bb_index index)
{
	if (index < set->length)
		return set->items[index];
		
	return NULL;
}

void bb_array_add_item(bb_array *set, void *item)
{	
	if (set->allocated < (set->length + 1)) {
		set->items = realloc(set->items, sizeof(void *) * (set->allocated + 10));
		set->allocated += 10;
	}

	set->items[set->length] = item;
	set->length += 1;
}

void array_remove_item(bb_array *array, bb_index index)
{
	if (index < array->length) {
		if (index < (array->length - 1)) {
			memmove(array->items + index + 1, array->items + index, (array->length - (index + 1)) * sizeof(void *));
		}
		
		array->length -= 1;
	}
}