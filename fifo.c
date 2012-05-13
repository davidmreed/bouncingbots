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
	if (fifo->next != NULL)
		bb_dealloc_fifo(fifo->next);
	free(fifo);
}

void *bb_fifo_pop(bb_fifo *fifo)
{
	bb_fifo *top = fifo->next;
	void *item = NULL;
	
	if (top != NULL) {
		item = top->item;
		fifo->next = top->next;
		top->next = NULL;
		bb_dealloc_fifo(top);
	}
	
	return item;
}

void bb_fifo_append(bb_fifo *fifo, void *item)
{
	bb_fifo *last;
	bb_fifo *new_last = bb_create_fifo();
	
	new_last->item = item;
	new_last->next = NULL;
	
	last = fifo->next;
	if (last != NULL) {
		while (last->next != NULL)
			last = last->next;
	
		last->next = new_last;
	} else {
		fifo->next = new_last;
	}

}