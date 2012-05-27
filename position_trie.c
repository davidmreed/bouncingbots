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

/* A position trie is composed of five levels of two-dimensional grids, each of whose elements are pointers to the next level down. The first level (trie->trie) represents the positions of the red pawn. If an element is non-NULL, we know a board state has been encountered with the red pawn in that position. The value of the element is a pointer to a table of blue pawn positions. Etc, five levels down. On the fifth level (silver pawn), elements are simply set to BB_TRUE to indicate the silver pawn's presence. 
 Each level contains one extra element for positions in which the given pawn does not exist. It is the final element in each level.
 */

void _bb_position_trie_dealloc_level(void **level, unsigned width, unsigned height);

bb_position_trie *bb_position_trie_alloc(unsigned width, unsigned height)
{
	bb_position_trie *trie = calloc(1, sizeof(bb_position_trie));
	
	if (trie != NULL) {
		trie->width = width;
		trie->height = height;
		trie->trie = calloc(width * height + 1, sizeof(void *));
		
		if (trie->trie == NULL) {
			free(trie);
			return NULL;
		}
	}
	
	return trie;
}

bb_bool bb_position_trie_contains(bb_position_trie *trie, bb_board *board)
{
	unsigned row, col;
	bb_pawn pawn;
	void **this_level = trie->trie;
		
	if ((board->width != trie->width) || (board->height != trie->height)) return BB_FALSE;
	
	for (pawn = BB_PAWN_RED; pawn <= BB_PAWN_SILVER; pawn++) {
		bb_locate_pawn(board, pawn, &row, &col);
		
		if (row == BB_NOT_FOUND) 
			this_level = this_level[trie->width * trie->height];
		else 
			this_level = this_level[row * trie->width + col];
		
		if (this_level == NULL) return BB_FALSE;
	}
	
	return BB_TRUE;
}

void bb_position_trie_add(bb_position_trie *trie, bb_board *board)
{
	unsigned row, col;
	bb_pawn pawn;
	void **this_level = trie->trie;
	void **next_level;
	
	if ((board->width != trie->width) || (board->height != trie->height)) return;
	
	for (pawn = BB_PAWN_RED; pawn <= BB_PAWN_SILVER; pawn++) {
		bb_locate_pawn(board, pawn, &row, &col);
		
		if (row == BB_NOT_FOUND) 
			next_level = this_level[trie->width * trie->height];
		else 
			next_level = this_level[row * trie->width + col];
		
		if (next_level == NULL) {
			if (pawn != BB_PAWN_SILVER) {
				next_level = calloc(trie->width * trie->height + 1, sizeof(void *));
				
				if (row == BB_NOT_FOUND) 
					this_level[trie->width * trie->height] = next_level;
				else 
					this_level[row * trie->width + col] = next_level;
			} else {
				/* For the silver pawn, we set one element to BB_TRUE to signify its position */
				/* If the silver pawn is not in use, we set the not found element to BB_TRUE */
				if (row == BB_NOT_FOUND) 
					this_level[trie->height * trie->width] = (void *)BB_TRUE;
				else 
					this_level[row * trie->width + col] = (void *)BB_TRUE;
				
				next_level = NULL;
			}
			
		}
		
		this_level = next_level;
	}
}

void _bb_position_trie_dealloc_level(void **level, unsigned width, unsigned height)
{
	unsigned i;
	
	for (i = 0; i < width * height + 1; i++)
		if ((level[i] != NULL) && (level[i] != (void *)BB_TRUE))
			_bb_position_trie_dealloc_level(level[i], width, height);
	
	free(level);
}

void bb_position_trie_dealloc(bb_position_trie *trie)
{
	_bb_position_trie_dealloc_level(trie->trie, trie->width, trie->height);
	free(trie);
}
