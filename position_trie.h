/*
 *  position_trie.h
 *  bouncingbots
 *
 *  Created by David Reed on 5/26/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */


#ifndef BB_POSITION_TRIE_H
#define BB_POSITION_TRIE_H

#include "types.h"
#include "board.h"

typedef struct {
	unsigned width, height;
	void **trie;
} bb_position_trie;


bb_position_trie *bb_position_trie_alloc(unsigned width, unsigned height);
bb_bool bb_position_trie_contains(bb_position_trie *trie, bb_board *board);
void bb_position_trie_add(bb_position_trie *trie, bb_board *board);
void bb_position_trie_dealloc(bb_position_trie *trie);

#endif