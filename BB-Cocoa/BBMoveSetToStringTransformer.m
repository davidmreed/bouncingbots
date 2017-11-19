//
//  BBMoveSetToStringTransformer.m
//  BB-Cocoa
//
//  Created by David Reed on 7/21/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import "BBMoveSetToStringTransformer.h"
#import "BBMoveSequence.h"

@implementation BBMoveSetToStringTransformer

+ (Class)transformedValueClass
{
	return [NSString class];
}

+ (BOOL)allowsReverseTransformation
{
	return NO;
}

- (id)transformedValue:(id)value
{
	if ([value isKindOfClass:[BBMoveSequence class]]) {
		return [(BBMoveSequence *)value stringValue];
	}
	
	return value;
}

@end
