//
//  BBCell.m
//  BB-Cocoa
//
//  Created by David Reed on 6/10/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import "BBCell.h"
#import "BBCocoaPrivate.h"

@implementation BBCell

@dynamic wallTop, wallBottom, wallRight, wallLeft, block, reflector, reflectorDirection, token;

+ cellWithbb_cell:(bb_cell *)cell
{
	return [[[BBCell alloc] initWithbb_cell:cell] autorelease];
}

- initWithbb_cell:(bb_cell *)cell
{
	self = [super init];
	
	if (self != NULL)
		_cell = cell;
	
	return self;
}

- (BOOL)wallTop
{
	return _cell->wall_top == BB_WALL;
}

- (void)setWallTop:(BOOL)wall
{
	_cell->wall_top = (wall ? BB_WALL : 0);
}

- (BOOL)wallBottom
{
	return _cell->wall_bottom == BB_WALL;
}

- (void)setWallBottom:(BOOL)wall
{
	_cell->wall_bottom = (wall ? BB_WALL : 0);
}

- (BOOL)wallRight
{
	return _cell->wall_right == BB_WALL;
}

- (void)setWallRight:(BOOL)wall
{
	_cell->wall_right = (wall ? BB_WALL : 0);
}

- (BOOL)wallLeft
{
	return _cell->wall_left == BB_WALL;
}

- (void)setWallLeft:(BOOL)wall
{
	_cell->wall_left = (wall ? BB_WALL : 0);
}

- (BOOL)block
{
	return _cell->block == BB_WALL;
}

- (void)setBlock:(BOOL)wall
{
	_cell->block = (wall ? BB_WALL : 0);
}

- (bb_pawn)reflector
{
	return _cell->reflector;
}

- (void)setReflector:(bb_pawn)reflector
{
	_cell->reflector = reflector;
}

- (bb_direction)reflectorDirection
{
	return _cell->reflector_direction;
}

- (void)setReflectorDirection:(bb_direction)dir
{
	_cell->reflector_direction = dir;
}

- (bb_token)token
{
	return _cell->token;
}

- (void)setToken:(bb_token)token
{
	if (token <= BB_MAX_TOKEN)
		_cell->token = token;
}

- (bb_cell *)cell
{
	return _cell;
}

@end
