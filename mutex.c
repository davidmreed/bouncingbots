/*
 *  mutex.c
 *  bouncingbots
 *
 *  Created by David Reed on 10/12/12.
 *  Copyright 2012 David Reed. All rights reserved.
 *
 */

#include "mutex.h"

#ifdef WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifdef WINDOWS

struct _bb_mutex {
	HANDLE m;
};

bb_mutex bb_mutex_create()
{
	bb_mutex m = malloc(sizeof(struct _bb_mutex));
	
	if (m != NULL) {
		m->m = CreateMutex(NULL, BB_FALSE, NULL);
		
		if (m->m == NULL) {
			free(m);
			m = NULL;
		}
	}
	
	return m;
}

void bb_mutex_dealloc(bb_mutex mutex)
{
	CloseHandle(mutex->m);
	free(mutex);
}

bb_bool bb_mutex_lock(bb_mutex mutex)
{
	return WaitForSingleObject(mutex->m, INFINITE) == WAIT_OBJECT_0;
}

bb_bool bb_mutex_try_lock(bb_mutex mutex)
{
	return WaitForSingleObject(mutex->m, 0) == WAIT_OBJECT_0;
}

void bb_mutex_unlock(bb_mutex mutex)
{
	ReleaseMutex(mutex->m);
}

#else

#include <stdlib.h>
#include <pthread.h>

/* We will assume for the time being that anything that isn't Windows supports pthread mutexes */

struct _bb_mutex {
	pthread_mutex_t *m;
};

bb_mutex bb_mutex_create()
{
	bb_mutex m = malloc(sizeof(struct _bb_mutex));
	
	if (m != NULL) {
		m->m = malloc(sizeof(pthread_mutex_t));
		
		if (m->m != NULL) {
			if (pthread_mutex_init(m->m, NULL) != 0) {
				free(m->m);
				free(m);
				m = NULL;
			}
		} else {
			free(m);
			m = NULL;
		}
	}
	
	return m;
}

void bb_mutex_dealloc(bb_mutex mutex)
{
	pthread_mutex_destroy(mutex->m);
	free(mutex->m);
	free(mutex);
}

bb_bool bb_mutex_lock(bb_mutex mutex)
{
	return pthread_mutex_lock(mutex->m) == 0;
}

bb_bool bb_mutex_try_lock(bb_mutex mutex)
{
	return pthread_mutex_trylock(mutex->m) == 0;
}

void bb_mutex_unlock(bb_mutex mutex)
{
	pthread_mutex_unlock(mutex->m);
}

#endif