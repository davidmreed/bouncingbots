/*
 *  fifo.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/1/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include "fifo.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>

bb_fifo *bb_create_fifo()
{
	bb_fifo *fifo = malloc(sizeof(bb_fifo));
	
	memset(fifo, 0, sizeof(bb_fifo));
	
	return fifo;
}

void bb_dealloc_fifo(bb_fifo *fifo)
{
	bb_fifo_item *item = bb_fifo_pop(fifo);
	
	while (item != NULL) {
		free(item);
		item = bb_fifo_pop(fifo);
	}
	
	free(fifo);
}

void *bb_fifo_pop(bb_fifo *fifo)
{
	bb_fifo_item *top = fifo->next;
	void *item = NULL;
	
	if (top != NULL) {
		item = top->item;
		fifo->next = top->next;
		top->next = NULL;
		free(top);
		fifo->length--;
	}
	
	return item;
}

bb_index bb_fifo_length(bb_fifo *fifo)
{
	return fifo->length;
}

void bb_fifo_append(bb_fifo *fifo, void *item)
{
	bb_fifo_item *new_last = malloc(sizeof(bb_fifo_item));
	
	new_last->item = item;
	new_last->next = NULL;
	
	if (fifo->next == NULL) {
		fifo->next = new_last;
		fifo->last = new_last;
	} else {
		fifo->last->next = new_last;
		fifo->last = new_last;
	}
	fifo->length++;
}