//
//  BBBoardView.m
//  BB-Cocoa
//
//  Created by David Reed on 7/8/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import "BBBoardView.h"
#import "BBBoard.h"
#import "BBCell.h"
#import "../types.h"

@implementation BBBoardView

@synthesize board;
@dynamic selectedCellPawn, selectedCell;

/*+ (NSSet *)keyPathsForValuesAffectingSelectedCellPawn
{
	return [NSSet setWithObjects:@"selectedCell", @"board", nil];
}*/

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if (context == selectedCell) {
		[[[[self window] windowController] document] updateChangeCount:NSChangeDone];
		[self setNeedsDisplayInRect:[self rectForCellAtRow:[board rowForCell:selectedCell] col:[board colForCell:selectedCell]]];
	} else {
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
	}
}

- (BBCell *)selectedCell
{
	return selectedCell;
}

- (void)setSelectedCell:(BBCell *)cell
{
	[selectedCell removeObserver:self forKeyPath:@"wallTop"];
	[selectedCell removeObserver:self forKeyPath:@"wallBottom"];
	[selectedCell removeObserver:self forKeyPath:@"wallRight"];
	[selectedCell removeObserver:self forKeyPath:@"wallLeft"];
	[selectedCell removeObserver:self forKeyPath:@"block"];
	[selectedCell removeObserver:self forKeyPath:@"reflector"];
	[selectedCell removeObserver:self forKeyPath:@"reflectorDirection"];
	[selectedCell removeObserver:self forKeyPath:@"token"];

	[self willChangeValueForKey:@"selectedCellPawn"];
	selectedCell = cell;
	[self didChangeValueForKey:@"selectedCellPawn"];
	
	[selectedCell addObserver:self forKeyPath:@"wallTop" options:0 context:selectedCell];
	[selectedCell addObserver:self forKeyPath:@"wallBottom" options:0 context:selectedCell];
	[selectedCell addObserver:self forKeyPath:@"wallRight" options:0 context:selectedCell];
	[selectedCell addObserver:self forKeyPath:@"wallLeft" options:0 context:selectedCell];
	[selectedCell addObserver:self forKeyPath:@"block" options:0 context:selectedCell];
	[selectedCell addObserver:self forKeyPath:@"reflector" options:0 context:selectedCell];
	[selectedCell addObserver:self forKeyPath:@"reflectorDirection" options:0 context:selectedCell];
	[selectedCell addObserver:self forKeyPath:@"token" options:0 context:selectedCell];
}

- (bb_pawn)selectedCellPawn
{
	if (board == nil)
		return 0;
	
	return [board pawnForCell:self.selectedCell];
}

- (void)setSelectedCellPawn:(bb_pawn)pawn
{
	if (self.selectedCellPawn != 0)
		[board movePawn:self.selectedCellPawn toRow:BB_NOT_FOUND col:BB_NOT_FOUND];
	
	if (pawn != 0) {
		[self setNeedsDisplayInRect:[self rectForCellAtRow:[board rowForPawn:pawn] col:[board colForPawn:pawn]]];
		[board movePawn:pawn toRow:[board rowForCell:self.selectedCell] col:[board colForCell:self.selectedCell]];
	}

	[self setNeedsDisplayInRect:[self rectForCellAtRow:[board rowForCell:self.selectedCell] col:[board colForCell:self.selectedCell]]];
	[[[[self window] windowController] document] updateChangeCount:NSChangeDone];
}

- (NSColor *)backgroundColor
{
	return [NSColor darkGrayColor];
}

- (NSColor *)cellBorderColor
{
	return [NSColor lightGrayColor];
}

- (NSColor *)colorForWalls
{
	return [NSColor blackColor];
}

- (NSColor *)colorForToken:(bb_token)token
{
	// FIXME: implement.
	return [NSColor yellowColor];
}

- (void)mouseDown:(NSEvent *)event
{
	NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
	bb_dimension i, j;
	
	if (self.board != nil) {
		for (i = 0; i < board.width; i++) {
			for (j = 0; j < board.height; j++) {
				NSRect cellRect = [self rectForCellAtRow:j col:i];
				
				if (NSPointInRect(loc, cellRect)) {
					self.selectedCell = [board cellAtRow:j column:i];
					return;
				}
			}
		}
	}
}

- (void)drawRect:(NSRect)dirtyRect 
{
	bb_dimension i, j;
	
	[[self backgroundColor] set];
	NSRectFill(dirtyRect);
	
	if (self.board != nil) {
		for (i = 0; i < board.width; i++) {
			for (j = 0; j < board.height; j++) {
				NSRect cellRect = [self rectForCellAtRow:j col:i];
			
				if (NSIntersectsRect(dirtyRect, cellRect)) {
					[[self backgroundColor] set];
					NSRectFill(cellRect);
					[[self cellBorderColor] set];
					NSFrameRectWithWidth(cellRect, ceil(MAX(NSWidth(cellRect), NSHeight(cellRect)) / 32));
					[self drawCellAtRow:j col:i inRect:cellRect];
				}
			}
		}
	}
}

- (NSRect)rectForCellAtRow:(bb_dimension)row col:(bb_dimension)col
{
	float cellSize;
	NSPoint topLeftPoint;
	
	if (([self bounds].size.width / board.width) > ([self bounds].size.height / board.height)) {		
		cellSize = floor([self bounds].size.height / board.height);
	} else {
		cellSize = floor([self bounds].size.width / board.width);
	}

	topLeftPoint = NSMakePoint(floor((NSWidth(self.bounds) - cellSize * board.width) / 2), 
							   floor((NSHeight(self.bounds) - cellSize * board.height) / 2));
		
	return NSMakeRect(topLeftPoint.x + cellSize * col, 
					  topLeftPoint.y + cellSize * row, 
					  cellSize, cellSize);
}

- (BOOL)isFlipped
{
	return YES;
}

- (NSColor *)colorForPawn:(bb_pawn)pawn
{
	switch (pawn) {
		case BB_REFLECTOR_RED:
			return [NSColor redColor];
		case BB_REFLECTOR_BLUE:
			return [NSColor blueColor];
		case BB_REFLECTOR_GREEN:
			return [NSColor greenColor];
		case BB_REFLECTOR_YELLOW:
			return [NSColor yellowColor];
		case BB_REFLECTOR_SILVER:
			return [NSColor lightGrayColor];
	}
	
	return nil;
}	

- (void)drawCellAtRow:(bb_dimension)row col:(bb_dimension)col inRect:(NSRect)rect
{
	CGFloat wallWidth = ceil(MAX(NSWidth(rect), NSHeight(rect)) / 8);
	BBCell *cell = [board cellAtRow:row column:col];
	
	[NSBezierPath setDefaultLineCapStyle:NSButtLineCapStyle];
	[NSBezierPath setDefaultLineWidth:wallWidth];
	
	if (cell.block == BB_WALL) {
		[[self colorForWalls] set];
		[[NSBezierPath bezierPathWithRoundedRect:rect xRadius:wallWidth yRadius:wallWidth] fill];
		return;
	}
	
	if (cell.token != 0) {
		NSRect tokenRect = NSInsetRect(rect, wallWidth, wallWidth);
		[[self colorForToken:cell.token] set];
		[[NSBezierPath bezierPathWithRoundedRect:tokenRect xRadius:wallWidth yRadius:wallWidth] fill];
	}
	
	[[self colorForWalls] set];
	
	if (cell.wallTop) {
		[NSBezierPath strokeLineFromPoint:NSMakePoint(NSMinX(rect), NSMinY(rect)) 
								  toPoint:NSMakePoint(NSMaxX(rect), NSMinY(rect))];
	}
	if (cell.wallRight) {
		[NSBezierPath strokeLineFromPoint:NSMakePoint(NSMaxX(rect), NSMinY(rect)) 
								  toPoint:NSMakePoint(NSMaxX(rect), NSMaxY(rect))];
	}
	if (cell.wallBottom) {
		[NSBezierPath strokeLineFromPoint:NSMakePoint(NSMinX(rect), NSMaxY(rect)) 
								  toPoint:NSMakePoint(NSMaxX(rect), NSMaxY(rect))];
	}
	if (cell.wallLeft) {
		[NSBezierPath strokeLineFromPoint:NSMakePoint(NSMinX(rect), NSMinY(rect)) 
								  toPoint:NSMakePoint(NSMinX(rect), NSMaxY(rect))];
	}
	
	if (cell.reflector != 0) {
		[[self colorForPawn:cell.reflector] set];
		if (cell.reflectorDirection == BB_45_DEGREES) {
			[NSBezierPath strokeLineFromPoint:NSMakePoint(NSMinX(rect) + 2 * wallWidth, NSMaxY(rect) - 2 * wallWidth) 
									  toPoint:NSMakePoint(NSMaxX(rect) - 2 * wallWidth, NSMinY(rect) + 2 * wallWidth)];
		} else {
			[NSBezierPath strokeLineFromPoint:NSMakePoint(NSMinX(rect) + 2 * wallWidth, NSMinY(rect) + 2 * wallWidth) 
									  toPoint:NSMakePoint(NSMaxX(rect) - 2 * wallWidth, NSMaxY(rect) - 2 * wallWidth)];
		}
		
	}

	if ([board pawnForCell:cell] != 0) {
		[[self colorForPawn:[board pawnForCell:cell]] set];
		[[NSBezierPath bezierPathWithOvalInRect:NSInsetRect(rect, wallWidth * 2, wallWidth * 2)] fill];
	}
}
			
- (void)dealloc
{
	self.selectedCell = nil;
	[board release];
	[super dealloc];
}

@end
