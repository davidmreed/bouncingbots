//
//  BBMoveSequence.m
//  BB-Cocoa
//
//  Created by David Reed on 7/8/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import "BBMoveSequence.h"
#import "BBMove.h"
#import "BBCocoaPrivate.h"
#import "../move.h"

@implementation BBMoveSequence

- init
{
	self = [super init];
	
	if (self != nil) {
		_ms = bb_move_set_alloc(10);
	
		if (_ms == NULL) {
			[self dealloc];
			return nil;
		}
	}
	
	return self;
}

- initWithbb_move_set:(bb_move_set *)set
{
	self = [super init];
	
	if (self != nil) {		
		_ms = set;
	}
	
	return self;
}

- copyWithZone:(NSZone *)zone
{
	bb_move_set *ms = bb_move_set_copy(_ms);
	
	if (ms == NULL) {
		return nil;
	}
	
	return [[[BBMoveSequence allocWithZone:zone] initWithbb_move_set:ms] autorelease];
}

- (NSUInteger)count
{
	return bb_move_set_length(_ms);
}

- (id)objectAtIndex:(NSUInteger)index
{
	if (index < [self count]) {
		bb_move move = bb_move_set_get_move(_ms, index);
		BBMove *m = [[BBMove alloc] init];
		
		m.pawn = move.pawn;
		m.dir = move.direction;
		
		return [m autorelease];
	}
	
	[NSException raise:NSRangeException format:@""];
	
	return nil;
}

- (void)addObject:(id)anObject
{
	if ([anObject isKindOfClass:[BBMove class]]) {
		bb_move move;
		
		move.pawn = ((BBMove *)anObject).pawn;
		move.direction = ((BBMove *)anObject).dir;
		
		bb_move_set_add_move(_ms, move);
	} else {
		[NSException raise:NSInvalidArgumentException format:@"BBMoveSequence can only contain BBMove objects."];
	}
}

- (void)insertObject:(id)anObject atIndex:(NSUInteger)index
{
	[NSException raise:NSInternalInconsistencyException format:@"BBMoveSequences do not support insertion."];
}

- (void)removeLastObject
{
	[NSException raise:NSInternalInconsistencyException format:@"BBMoveSequences do not support deletion."];
}

- (void)removeObjectAtIndex:(NSUInteger)index
{
	[NSException raise:NSInternalInconsistencyException format:@"BBMoveSequences do not support deletion."];

}

- (void)replaceObjectAtIndex:(NSUInteger)index withObject:(id)anObject
{
	[NSException raise:NSInternalInconsistencyException format:@"BBMoveSequences do not support mutation."];
}

- (void)dealloc
{
	bb_move_set_dealloc(_ms);
	[super dealloc];
}

@end
