/*
 *  strings.h
 *  bouncingbots
 *
 *  Created by David Reed on 5/2/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#ifndef BB_STRINGS_H
#define BB_STRINGS_H

#include "board.h"
#include "move.h"

bb_move_set *bb_create_move_set_from_string(char *str, unsigned length);
void bb_create_string_from_move_set(bb_move_set *set, unsigned char **out_str);
void bb_print_move_set (bb_move_set *set);

void bb_create_board_from_string(char *str, bb_board **b, bb_pawn_state ps);
//void create_string_from_board(bb_board *board, unsigned char **out_str);

#endif