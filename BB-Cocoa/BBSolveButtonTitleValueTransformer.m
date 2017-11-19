//
//  BBSolveButtonTitleValueTransformer.m
//  BB-Cocoa
//
//  Created by David Reed on 9/26/12.
//  Copyright 2012 David Reed. All rights reserved.
//

#import "BBSolveButtonTitleValueTransformer.h"


@implementation BBSolveButtonTitleValueTransformer

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
	if ([value isKindOfClass:[NSValue class]]) {
		return [(NSValue *)value boolValue] ? NSLocalizedString(@"Cancel", nil) : NSLocalizedString(@"Solve", nil);
	}
	
	return @"";
}

@end
