//
//  BBCell.h
//  BB-Cocoa
//
//  Created by David Reed on 6/10/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "../board.h"
#import "../types.h"

@interface BBCell : NSObject {
	bb_cell *_cell;
}

@property (readwrite) BOOL wallTop;
@property (readwrite) BOOL wallBottom;
@property (readwrite) BOOL wallRight;
@property (readwrite) BOOL wallLeft;
@property (readwrite) BOOL block;

@property (readwrite) bb_pawn reflector;
@property (readwrite) bb_direction reflectorDirection;

@property (readwrite) bb_token token;

@end
