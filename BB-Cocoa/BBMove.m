//
//  BBMove.m
//  BB-Cocoa
//
//  Created by David Reed on 7/8/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import "BBMove.h"


@implementation BBMove

@synthesize dir, pawn;

- (NSString *)description
{
	char *dirs = "urdl";
	char *pawns = " rbgys";
	
	return [NSString stringWithFormat:@"%c%c", (self.pawn >= BB_PAWN_RED && self.pawn <= BB_PAWN_SILVER) ? pawns[self.pawn] : '?', (self.dir >= BB_DIRECTION_UP && self.dir <= BB_DIRECTION_LEFT) ? dirs[self.dir] : '?'];
}

@end
