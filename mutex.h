/*
 *  mutex.h
 *  bouncingbots
 *
 *  Created by David Reed on 10/12/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#ifndef BB_MUTEX_H
#define BB_MUTEX_H

#include "types.h"

typedef struct _bb_mutex *bb_mutex;

bb_mutex bb_mutex_create();
void bb_mutex_dealloc(bb_mutex mutex);

bb_bool bb_mutex_lock(bb_mutex mutex);
bb_bool bb_mutex_try_lock(bb_mutex mutex);
void bb_mutex_unlock(bb_mutex mutex);

#endif