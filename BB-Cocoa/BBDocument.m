//
//  MyDocument.m
//  BB-Cocoa
//
//  Created by David Reed on 6/10/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import "BBDocument.h"
#import "BBBoard.h"
#import "BBBoardView.h"

@implementation BBDocument

@synthesize pawn, token, solutions, board;

- (id)init
{
    self = [super init];
    if (self) {
		board = [[BBBoard alloc] initWithSize:NSMakeSize(16, 16)];
		pawn = BB_PAWN_RED;
		token = 0;
    }
    return self;
}

- (NSString *)windowNibName
{
    return @"BBDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    [super windowControllerDidLoadNib:aController];

	view.board = board;
}

- (IBAction)showCellInspector:(id)sender
{
	[cellDrawer open];
}

- (IBAction)showSolution:(id)sender
{
}

- (IBAction)applySolution:(id)sender
{
}

- (IBAction)solve:(id)sender
{
	self.solutions = nil;

	// if a solution is already in progress, cancel.
	if (self.board.asynchronousSolutionInProgress)
		[board cancelAsynchronousSolution];
	else 
		[board beginAsynchronousSolveWithDelegate:self forPawn:self.pawn token:self.token];
}

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)anItem
{
	if ([anItem action] == @selector(solve:)) {
		return (self.token != 0);
	}
	
	return [super validateUserInterfaceItem:anItem];
}

- (void)board:(BBBoard *)board foundSolutions:(NSArray *)newSolutions forPawn:(bb_pawn)pawn token:(bb_token)token
{
	self.solutions = newSolutions;
}

- (void)board:(BBBoard *)board didEncounterError:(NSString *)error
{
	NSBeginAlertSheet(NSLocalizedString(@"Error occured during solution.", nil), NSLocalizedString(@"OK", nil), nil, nil, [self windowForSheet], nil, NULL, NULL, NULL, NSLocalizedString(@"While attempting to solve the board, BouncingBots encountered the following error:\n%@", nil), error);
}

- (void)boardDidCancelSolution:(BBBoard *)board
{
}

- (BOOL)writeToURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{
	return [board writeToURL:absoluteURL];
}

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{
	self.board = [[[BBBoard alloc] initWithContentsOfURL:absoluteURL] autorelease];
	
	view.board = board;
	
	return (board != nil);
}

- (void)dealloc
{
	self.solutions = nil;
	self.board = nil;
	[super dealloc];
}

@end
