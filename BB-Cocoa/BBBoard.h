//
//  BBBoard.h
//  BB-Cocoa
//
//  Created by David Reed on 6/10/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "../board.h"
#import "../types.h"

@class BBBoard, BBCell, BBMove, BBMoveSequence;

@protocol BBBoardSolverDelegate

- (void)board:(BBBoard *)board foundSolutions:(NSArray *)solutions forPawn:(bb_pawn)pawn token:(bb_token)token;
- (void)board:(BBBoard *)board didEncounterError:(NSString *)error;
- (void)boardDidCancelSolution:(BBBoard *)board;

@end


@interface BBBoard : NSObject {
	bb_board *_board;
	bb_pawn_state _ps;
	NSMutableDictionary *_cells;
	pthread_mutex_t *_mutex;
	BOOL canceled;
}

@property (readonly) bb_dimension width;
@property (readonly) bb_dimension height;
@property (readonly) NSArray *tokens;
@property (readonly) BOOL asynchronousSolutionInProgress;

+ boardWithSize:(NSSize)size;
+ boardWithString:(NSString *)string;
+ boardWithContentsOfURL:(NSURL *)url;

- initWithSize:(NSSize)size;
- initWithString:(NSString *)string;
- initWithContentsOfURL:(NSURL *)url;

- (BOOL)writeToURL:(NSURL *)url;

- (NSArray *)pawns;
- (NSArray *)tokens;

- (BBCell *)cellAtRow:(bb_dimension)row column:(bb_dimension)col;
- (bb_dimension)rowForCell:(BBCell *)cell;
- (bb_dimension)colForCell:(BBCell *)cell;

- (BBCell *)cellForPawn:(bb_pawn)pawn;
- (bb_pawn)pawnForCell:(BBCell *)cell;

- (bb_dimension)rowForPawn:(bb_pawn)pawn;
- (bb_dimension)colForPawn:(bb_pawn)pawn;

- (bb_dimension)rowForToken:(bb_token)token;
- (bb_dimension)colForToken:(bb_token)token;

- (void)movePawn:(bb_pawn)pawn toRow:(bb_dimension)row col:(bb_dimension)column;
- (void)applyMoveSequence:(BBMoveSequence *)moves;
- (void)applyMove:(BBMove *)move;

- (NSArray *)solutionsForPawn:(bb_pawn)pawn token:(bb_token)token;

- (void)beginAsynchronousSolveWithDelegate:(id <BBBoardSolverDelegate>)delegate forPawn:(bb_pawn)pawn token:(bb_token)token;
- (BOOL)asynchronousSolutionInProgress;
- (void)cancelAsynchronousSolution;

@end
