//
//  BBMoveSequence.h
//  BB-Cocoa
//
//  Created by David Reed on 7/8/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "../move.h"

@interface BBMoveSequence : NSMutableArray {
	bb_move_set *_ms;
}

- (NSString *)stringValue;

@end
