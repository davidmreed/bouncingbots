//
//  BBBoard.m
//  BB-Cocoa
//
//  Created by David Reed on 6/10/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import "BBBoard.h"
#import "BBMove.h"
#import "BBMoveSequence.h"
#import "BBCocoaPrivate.h"
#import "../strings.h"
#import "../fifo.h"
#import "../array.h"
#import "../solver.h"


@implementation BBBoard

@dynamic width, height;

- copyWithZone:(NSZone *)zone
{
	bb_board *nb = bb_board_copy(_board);
	
	return [[[BBBoard allocWithZone:zone] initWithbb_board:nb] autorelease];
}

+ boardWithSize:(NSSize)size
{
	return [[[BBBoard alloc] initWithSize:size] autorelease];
}

+ boardWithString:(NSString *)string
{
	return [[[BBBoard alloc] initWithString:string] autorelease];
}

+ boardWithContentsOfURL:(NSURL *)url
{
	return [[[BBBoard alloc] initWithContentsOfURL:url] autorelease];
}

- initWithSize:(NSSize)size
{
	if ((size.width < BB_MAX_DIMENSION) && (size.height < BB_MAX_DIMENSION)) {
		self = [super init];
		
		if (self) {
			_board = bb_board_alloc(floor(size.width), floor(size.height));
			bb_init_pawn_state(_ps);
			_cells = [[NSMutableDictionary alloc] initWithCapacity:self.width * self.height];
		}
		
		return self;
	}
	
	return NULL;
}

- initWithbb_board:(bb_board *)board
{
	self = [super init];
	
	if (self != NULL) {
		_board = board;
		_cells = [[NSMutableDictionary alloc] initWithCapacity:self.width * self.height];
	}
	
	return self;
}

- initWithString:(NSString *)string
{
	self = [super init];
	
	if (self) {
		bb_create_board_from_string([string cStringUsingEncoding:NSASCIIStringEncoding], &_board, _ps);
		
		if (_board == NULL) {
			[self dealloc];
			return nil;
		}
		
		_cells = [[NSMutableDictionary alloc] initWithCapacity:self.width * self.height];
	}
	
	return self;
}

- initWithContentsOfURL:(NSURL *)url
{
	return [self initWithString:[NSString stringWithContentsOfURL:url encoding:NSASCIIStringEncoding error:nil]];
}

- (BOOL)writeToURL:(NSURL *)url
{
	NSString *string;
	char *buffer;
	
	bb_create_string_from_board(_board, _ps, &buffer);
	string = [[[NSString alloc] initWithCString:buffer encoding:NSASCIIStringEncoding] autorelease];
	free(buffer);

	return [string writeToURL:url atomically:YES encoding:NSASCIIStringEncoding error:nil];
}

- (bb_dimension)width
{
	return _board->width;
}

- (bb_dimension)height
{
	return _board->height;
}

- (BBCell *)cellAtRow:(bb_dimension)row column:(bb_dimension)col
{
	BBCell *cell;
	bb_cell *bbcell = bb_get_cell(_board, row, col);
	
	if ((cell = [_cells objectForKey:[NSValue valueWithPointer:bbcell]]) == nil) {
		cell = [BBCell cellWithbb_cell:bbcell];
		[_cells setObject:cell forKey:[NSValue valueWithPointer:bbcell]];
	}
	
	return cell;
}

- (bb_dimension)rowForCell:(BBCell *)cell
{
	bb_dimension row;
	
	bb_get_cell_location(_board, cell.cell, &row, NULL);
	
	return row;
}

- (bb_dimension)colForCell:(BBCell *)cell
{
	bb_dimension col;
	
	bb_get_cell_location(_board, cell.cell, NULL, &col);
	
	return col;
}

- (BBCell *)cellForPawn:(bb_pawn)pawn
{
	return [self cellAtRow:[self rowForPawn:pawn] column:[self colForPawn:pawn]];
}

- (bb_pawn)pawnForCell:(BBCell *)cell
{
	bb_dimension row, col;
	
	bb_get_cell_location(_board, cell.cell, &row, &col);
	
	return bb_pawn_at_location(_ps, row, col);
}

- (bb_dimension)rowForPawn:(bb_pawn)pawn
{
	bb_dimension row;
	
	bb_get_pawn_location(_ps, pawn, &row, NULL);
	
	return row;
}

- (bb_dimension)colForPawn:(bb_pawn)pawn
{
	bb_dimension col;
	
	bb_get_pawn_location(_ps, pawn, NULL, &col);
	
	return col;
}

- (void)movePawn:(bb_pawn)pawn toRow:(bb_dimension)row col:(bb_dimension)column
{
	bb_move_pawn_to_location(_ps, pawn, row, column);
}

- (void)applyMoveSequence:(BBMoveSequence *)moves
{
	for (BBMove *move in moves) {
		[self applyMove:move];
	}
}

- (void)applyMove:(BBMove *)move
{
	bb_move aMove = { move.pawn, move.dir };
	
	bb_apply_move(_board, _ps, aMove);
}

- (NSArray *)solutionsForPawn:(bb_pawn)pawn token:(bb_token)token
{
	NSMutableArray *array;
	bb_array *solutions;
	bb_fifo *fifo;

	fifo = bb_find_solutions(_board, _ps, pawn, token, -1);
	solutions = bb_winnow_solutions(fifo);
	bb_fifo_dealloc(fifo);
	
	array = [[NSMutableArray alloc] initWithCapacity:bb_array_length(solutions)];
	
	for (bb_index i = 0; i < bb_array_length(solutions); i++) {
		bb_move_set *set = bb_array_get_item(solutions, i);
		
		[array addObject:[[[BBMoveSequence alloc] initWithbb_move_set:set] autorelease]];
	}
	
	bb_array_dealloc(solutions);
	
	return [array autorelease];
}

- (void)dealloc
{
	[_cells release];
	bb_board_dealloc(_board);
	[super dealloc];
}

@end
