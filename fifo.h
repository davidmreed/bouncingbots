/*
 *  fifo.h
 *  bouncingbots
 *
 *  Created by David Reed on 5/1/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#ifndef BB_FIFO_H
#define BB_FIFO_H

#include "types.h"

struct bb_fifo_item {
	struct bb_fifo_item *next;
	void *item;
};
typedef struct bb_fifo_item bb_fifo_item;

struct bb_fifo {
	bb_fifo_item *next;
	bb_fifo_item *last;
	bb_index length;
};
typedef struct bb_fifo bb_fifo;

bb_fifo *bb_fifo_alloc();
void bb_fifo_dealloc(bb_fifo *fifo);

void *bb_fifo_pop(bb_fifo *fifo);
void bb_fifo_append(bb_fifo *fifo, void *item);
bb_index bb_fifo_length(bb_fifo *fifo);


#endif