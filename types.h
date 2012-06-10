/*
 *  types.h
 *  bouncingbots
 *
 *  Created by David Reed on 5/2/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#ifndef BB_TYPES_H
#define BB_TYPES_H

#include <sys/types.h>

#ifndef NULL
#define NULL 0
#endif

#define BB_INLINE        inline
#define BB_NOT_FOUND     0xFF

#define BB_MAX_DIMENSION 128
#define BB_MAX_TOKEN     0xFF

typedef u_int8_t bb_dimension;
typedef u_int32_t bb_index;

enum {
	BB_TRUE = 1,
	BB_FALSE = 0
};
typedef unsigned bb_bool;

enum {
	BB_PAWN_RED = 1,
	BB_PAWN_BLUE = 2,
	BB_PAWN_GREEN = 3,
	BB_PAWN_YELLOW = 4,
	BB_PAWN_SILVER = 5
};
typedef unsigned bb_pawn;

typedef unsigned bb_token;

enum {
	BB_REFLECTOR_RED = BB_PAWN_RED,
	BB_REFLECTOR_BLUE = BB_PAWN_BLUE,
	BB_REFLECTOR_GREEN = BB_PAWN_GREEN,
	BB_REFLECTOR_YELLOW = BB_PAWN_YELLOW,
	BB_REFLECTOR_SILVER = BB_PAWN_SILVER
};

enum {
	BB_WALL = 1
};

enum {
	BB_45_DEGREES = 0,
	BB_135_DEGREES = 1
};

enum {
	BB_DIRECTION_UP,
	BB_DIRECTION_RIGHT,
	BB_DIRECTION_DOWN,
	BB_DIRECTION_LEFT
};
typedef unsigned bb_direction;

#endif