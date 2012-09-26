//
//  BBCocoaPrivate.h
//  BB-Cocoa
//
//  Created by David Reed on 7/15/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "BBBoard.h"
#import "BBCell.h"
#import "BBMoveSequence.h"
#import "../board.h"
#import "../move.h"

@interface BBCell (Private)

+ cellWithbb_cell:(bb_cell *)cell;
- initWithbb_cell:(bb_cell *)cell;

@property (readonly) bb_cell *cell;

@end

@interface BBBoard (Private)

- initWithbb_board:(bb_board *)board;

@end

@interface BBMoveSequence (Private)

- initWithbb_move_set:(bb_move_set *)set;

@end
