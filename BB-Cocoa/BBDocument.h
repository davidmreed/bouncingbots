//
//  MyDocument.h
//  BB-Cocoa
//
//  Created by David Reed on 6/10/12.
//  Copyright 2012 David Reed. All rights reserved.
//


#import <Cocoa/Cocoa.h>

@class BBBoardView, BBBoard;

@interface BBDocument : NSDocument
{
	BBBoard *board;
	IBOutlet BBBoardView *view;
	IBOutlet NSPanel *cellInspector;
}

- (IBAction)showCellInspector:(id)sender;

@end
