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

bb_array *bb_array_alloc(bb_index length, bb_index itemsize)
{
	bb_array *array = malloc(sizeof(bb_array));
	
	if (array != NULL) {
		bb_index len = ((length == 0) ? 10 : length);
		
		array->items = malloc(itemsize * len);
		
		if (array->items != NULL) {
			array->length = 0; 
			array->allocated = len;
			array->itemsize = itemsize;
			
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
	bb_array *ns = bb_array_alloc(bb_array_length(set), set->itemsize);
	
	memcpy(ns->items, set->items, set->length * set->itemsize);
	ns->length = set->length;
		
	return ns;
}

bb_bool bb_array_equal(bb_array *set, bb_array *other)
{
	if ((bb_array_length(set) == bb_array_length(other)) &&
		(set->itemsize == other->itemsize)) {
		
		return (memcmp(set->items, other->items, set->itemsize * set->length) == 0 ? BB_TRUE : BB_FALSE);
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

void bb_array_get_item(bb_array *set, bb_index index, void *out_item)
{
	if (out_item != NULL) {
		if (index < set->length)
			memcpy(out_item, (void *)(set->items + index * set->itemsize), set->itemsize);
		else 
			memset(out_item, 0, set->itemsize);
	}
	
}

void *bb_array_get_item_p(bb_array *arr, bb_index index)
{
	void *item = NULL;

	if (arr->itemsize == sizeof(void *)) 
		bb_array_get_item(arr, index, &item);

	return item;
}

void bb_array_add_item(bb_array *set, void *item)
{	
	if (set->allocated < (set->length + 1)) {
		set->items = realloc(set->items, set->itemsize * (set->allocated + 10));
		set->allocated += 10;
	}
	
	memcpy(set->items + set->length * set->itemsize, item, set->itemsize);
	set->length += 1;
}

void bb_array_add_item_p(bb_array *set, void *item)
{	
	bb_array_add_item(set, &item);
}

void bb_array_remove_item(bb_array *array, bb_index index)
{
	if (index < array->length) {
		if (index < (array->length - 1)) {
			memmove(array->items + ((index + 1) * array->itemsize), 
					array->items + (index * array->itemsize), 
					(array->length - (index + 1)) * array->itemsize);
		}
		
		array->length -= 1;
	}
}