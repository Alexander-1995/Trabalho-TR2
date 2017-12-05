#ifndef CACHE_H_
#define CACHE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <iostream>
#include <list>

#define MAX_CACHE_SIZE (1<<20) /* 1MB */


struct cache_list {
    char * data; /* HTML code */
	int size; /* size of the data */
	long int lru_time_track; /* least recently used */
	char * url;	/* identify cache objects by url */
	struct cache_list * next;
};
typedef struct cache_list cache_object;

pthread_rwlock_t lock;

cache_object * head;
int cache_size;
long int global_time;

void cacheAdd(char * buffer, int site_size, char* url);
void cacheRemove();
cache_object* cacheFind( char * url );

#endif
