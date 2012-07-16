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

- (id)init
{
    self = [super init];
    if (self) {
		board = [[BBBoard alloc] initWithSize:NSMakeSize(16, 16)];
    }
    return self;
}

- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"BBDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    [super windowControllerDidLoadNib:aController];

	view.board = board;
}

- (IBAction)showCellInspector:(id)sender
{
	[cellInspector orderFront:sender];
}

- (BOOL)writeToURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{
	return [board writeToURL:absoluteURL];
}

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{
	[board release];
	board = [[BBBoard alloc] initWithContentsOfURL:absoluteURL];
	
	view.board = board;
	
	return (board != nil);
}

- (void)dealloc
{
	[board release];
	[super dealloc];
}

@end
