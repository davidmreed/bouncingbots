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
#import <pthread.h>


@implementation BBBoard

@dynamic width, height, tokens;

- copyWithZone:(NSZone *)zone
{
	bb_board *nb = bb_board_copy(_board);
	
	return [[[BBBoard allocWithZone:zone] initWithbb_board:nb pawnState:_ps] autorelease];
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
			_mutex = NULL;
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
		bb_init_pawn_state(_ps);
		_cells = [[NSMutableDictionary alloc] initWithCapacity:self.width * self.height];
		_mutex = NULL;
	}
	
	return self;
}

- initWithbb_board:(bb_board *)board pawnState:(bb_pawn_state)ps
{
	self = [self initWithbb_board:board];
	
	if (self != NULL) {
		bb_copy_pawn_state(ps, _ps);
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
		_mutex = NULL;
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

- (NSArray *)pawns
{
	NSMutableArray *array = [[NSMutableArray alloc] initWithCapacity:5];
	int i;
	
	for (i = 0; i < 5; i++) {
		if ((_ps[i].row != BB_NOT_FOUND) && (_ps[i].col != BB_NOT_FOUND))
			[array addObject:[NSNumber numberWithInt:i+1]];
	}
	
	return [NSArray arrayWithArray:[array autorelease]];
}

- (NSArray *)tokens
{
	bb_dimension i, j;
	NSMutableSet *set = [NSMutableSet set];
	
	for (i = 0; i < self.width; i++) {
		for (j = 0; j < self.height; j++) {
			BBCell *cell = [self cellAtRow:j column:i];
			
			if (cell.token != 0)
				[set addObject:[NSNumber numberWithChar:cell.token]];
		}
	}
	
	return [set allObjects];
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

- (bb_dimension)rowForToken:(bb_token)token
{
	bb_dimension row;
	
	bb_get_token_location(_board, token, &row, NULL);
	
	return row;
}

- (bb_dimension)colForToken:(bb_token)token
{
	bb_dimension col;
	
	bb_get_token_location(_board, token, NULL, &col);
	
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

- (void)beginAsynchronousSolveWithDelegate:(id <BBBoardSolverDelegate>)delegate forPawn:(bb_pawn)pawn token:(bb_token)token
{
	if (_mutex == NULL) {
		[self willChangeValueForKey:@"asynchronousSolutionInProgress"];
		_mutex = malloc(sizeof(pthread_mutex_t));
		[self didChangeValueForKey:@"asynchronousSolutionInProgress"];
		if ((_mutex == NULL) || (pthread_mutex_init(_mutex, NULL) != 0)) {
			[delegate board:self didEncounterError:@"Could not create mutex."];
		} else {
			canceled = NO;
			pthread_mutex_lock(_mutex);
			[NSThread detachNewThreadSelector:@selector(_beginAsyncSolve:) toTarget:self withObject:[[NSMutableDictionary alloc] initWithObjectsAndKeys:delegate, @"delegate", [NSNumber numberWithChar:pawn], @"pawn", [NSNumber numberWithChar:token], @"token", nil]];
		}
	} else {
		[delegate board:self didEncounterError:@"Solution already in progress."];
	}

}

- (void)_beginAsyncSolve:(NSMutableDictionary *)params
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSMutableArray *array;
	bb_array *solutions;
	bb_fifo *fifo;
		
	fifo = bb_find_solutions_mutex(_board, _ps, [[params objectForKey:@"pawn"] charValue], [[params objectForKey:@"token"] charValue], -1, _mutex);
	if (fifo != NULL) {
		solutions = bb_winnow_solutions(fifo);
		bb_fifo_dealloc(fifo);
		
		array = [[NSMutableArray alloc] initWithCapacity:bb_array_length(solutions)];
		
		for (bb_index i = 0; i < bb_array_length(solutions); i++) {
			bb_move_set *set = bb_array_get_item(solutions, i);
			
			[array addObject:[[[BBMoveSequence alloc] initWithbb_move_set:set] autorelease]];
		}
		
		bb_array_dealloc(solutions);
		[params setObject:array forKey:@"solutions"];
		[array release];
	
	}

	[self performSelectorOnMainThread:@selector(_finishAsyncSolve:) withObject:params waitUntilDone:NO];
	
	[pool release];
}

- (void)_finishAsyncSolve:(NSMutableDictionary *)params
{
	id <BBBoardSolverDelegate> delegate = [params objectForKey:@"delegate"];
	NSArray *solutions = [params objectForKey:@"solutions"];
	
	if (canceled) {
		[delegate boardDidCancelSolution:self];
	} else {
		[delegate board:self foundSolutions:solutions forPawn:[[params objectForKey:@"pawn"] charValue] token:[[params objectForKey:@"token"] charValue]];
	}
	
	[params autorelease];
	pthread_mutex_destroy(_mutex);
	[self willChangeValueForKey:@"asynchronousSolutionInProgress"];
	free(_mutex);
	_mutex = NULL;
	[self didChangeValueForKey:@"asynchronousSolutionInProgress"];
}

- (BOOL)asynchronousSolutionInProgress
{
	return (_mutex != NULL);
}

- (void)cancelAsynchronousSolution
{
	if (_mutex != NULL) {
		canceled = YES;
		pthread_mutex_unlock(_mutex);
	}
}

- (void)dealloc
{
	[_cells release];
	bb_board_dealloc(_board);
	[super dealloc];
}

@end
