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

bb_move_set *create_move_set_from_string(char *str, unsigned length);
void create_string_from_move_set(bb_move_set *set, unsigned char **out_str);

bb_board *create_board_from_string(char *str);
//void create_string_from_board(bb_board *board, unsigned char **out_str);

#endif