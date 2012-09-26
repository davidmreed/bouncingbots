//
//  BBBoardView.h
//  BB-Cocoa
//
//  Created by David Reed on 7/8/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "../types.h"

@class BBBoard, BBCell;

@interface BBBoardView : NSView {
	BBBoard *board;
	BBCell *selectedCell;
}

@property (readwrite, assign) BBCell *selectedCell;
@property (readwrite, retain) BBBoard *board;
@property (readwrite) bb_pawn selectedCellPawn;

- (NSRect)rectForCellAtRow:(bb_dimension)row col:(bb_dimension)col;

- (NSColor *)colorForPawn:(bb_pawn)pawn;
- (NSColor *)colorForToken:(bb_token)token;
- (NSColor *)colorForWalls;
- (NSColor *)backgroundColor;
- (NSColor *)cellBorderColor;
- (void)drawCellAtRow:(bb_dimension)row col:(bb_dimension)col inRect:(NSRect)rect;


@end
