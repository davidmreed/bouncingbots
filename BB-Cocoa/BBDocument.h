//
//  MyDocument.h
//  BB-Cocoa
//
//  Created by David Reed on 6/10/12.
//  Copyright 2012 David Reed. All rights reserved.
//


#import <Cocoa/Cocoa.h>
#import "BBBoard.h"

@class BBBoardView;

@interface BBDocument : NSDocument <BBBoardSolverDelegate>
{
	BBBoard *board;
	NSArray *solutions;
	bb_pawn pawn;
	bb_token token;
	IBOutlet BBBoardView *view;
	IBOutlet NSDrawer *cellDrawer;
	IBOutlet NSArrayController *solutionsArrayController;
}

@property (readwrite, copy) BBBoard *board;
@property (readwrite) bb_pawn pawn;
@property (readwrite) bb_token token;
@property (readwrite, copy) NSArray *solutions;

- (IBAction)showCellInspector:(id)sender;
- (IBAction)showSolution:(id)sender;
- (IBAction)applySolution:(id)sender;
- (IBAction)solve:(id)sender;

@end
