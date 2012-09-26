/*
 *  position_trie.c
 *  bouncingbots
 *
 *  Created by David Reed on 5/26/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include "position_trie.h"
#include <stdlib.h>

bb_position_trie *_bb_position_trie_get_element(bb_position_trie *trie, u_int8_t value);

bb_position_trie *bb_position_trie_alloc()
{
	bb_position_trie *trie = calloc(sizeof(bb_position_trie), 1);
	
	if (trie != NULL) {
		trie->trie = bb_array_alloc(1);
		
		if (trie->trie == NULL) {
			free(trie);
			trie = NULL;
		}
	}
	
	return trie;
}

bb_position_trie *_bb_position_trie_get_element(bb_position_trie *trie, u_int8_t value) 
{
	bb_index i;
	
	for (i = 0; i < bb_array_length(trie->trie); i++) {
		bb_position_trie *t = (bb_position_trie *)bb_array_get_item(trie->trie, i);
		
		if (t->value == value) {
			return t;
		}
	}
	
	return NULL;
}	

bb_bool bb_position_trie_contains(bb_position_trie *trie, bb_pawn_state ps)
{
	bb_dimension row, col;
	bb_pawn pawn;
	bb_position_trie *current = trie, *next;
	
	for (pawn = BB_PAWN_RED; pawn <= BB_PAWN_SILVER; pawn++) {
		next = NULL;
		
		bb_get_pawn_location(ps, pawn, &row, &col);
				
		next = _bb_position_trie_get_element(current, row);
				
		if (next == NULL) {
			return BB_FALSE;
		} else {
			current = next; next = NULL;
			/* We found the trie entry for the row (== current). Now search for the column. */

			next = _bb_position_trie_get_element(current, col);
			
			if (next == NULL) {
				return BB_FALSE;
			} else {
				current = next;
			}
		} 
	}
	
	return BB_TRUE;
}

void bb_position_trie_add(bb_position_trie *trie, bb_pawn_state ps)
{
	bb_dimension row, col;
	bb_pawn pawn;
	bb_position_trie *current = trie, *next = NULL;
	
	for (pawn = BB_PAWN_RED; pawn <= BB_PAWN_SILVER; pawn++) {		
		bb_get_pawn_location(ps, pawn, &row, &col);
		
		next = _bb_position_trie_get_element(current, row);
		
		if (next == NULL) {
			next = bb_position_trie_alloc();

			next->value = row;
			bb_array_add_item(current->trie, next);
		}
		
		current = next; next = NULL;
		
		/* We found the trie entry for the row (== current). Now search for the column. */

		next = _bb_position_trie_get_element(current, col);
		
		if (next == NULL) {
			next = bb_position_trie_alloc();

			next->value = col;
			bb_array_add_item(current->trie, next);
		}
		current = next; next = NULL;
	}
}

void bb_position_trie_dealloc(bb_position_trie *trie)
{
	unsigned i;
	
	for (i = 0; i < bb_array_length(trie->trie); i++) {
		bb_position_trie_dealloc((bb_position_trie *)bb_array_get_item(trie->trie, i));
	}
	
	bb_array_dealloc(trie->trie);
	free(trie);
}
