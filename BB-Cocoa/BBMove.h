//
//  BBMove.h
//  BB-Cocoa
//
//  Created by David Reed on 7/8/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "../types.h"

@interface BBMove : NSObject {
	bb_pawn pawn;
	bb_direction dir;
}

@property (readwrite) bb_pawn pawn;
@property (readwrite) bb_direction dir;

@end
