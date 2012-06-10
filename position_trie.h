/*
 *  position_trie.h
 *  bouncingbots
 *
 *  Created by David Reed on 5/26/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#ifndef POSITION_TRIE_H
#define POSITION_TRIE_H

#include "types.h"
#include "array.h"
#include "board.h"

typedef struct {
	unsigned char value;
	bb_array *trie;
} bb_position_trie;

bb_position_trie *bb_position_trie_alloc();
bb_bool bb_position_trie_contains(bb_position_trie *trie, bb_pawn_state ps);
void bb_position_trie_add(bb_position_trie *trie, bb_pawn_state ps);
void bb_position_trie_dealloc(bb_position_trie *trie);

#endif