#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "arena.h"

#define T Arena_T


const Except_T Arena_NewFailed = { "Arena Creation Failed" };
const Except_T Arena_Failed = { "Arena Allocation Failed" };

// <macros>

// <types>
struct T {
	T prev; // Points to the head of the chunk
	char *avail; // Points to the chunk's first free location
	char *limit; // The space between avail and limit is available for allocation
}

// <data>

// <functions>

/**
 * @param
 */
void Arena_dispose(int o) {
	assert(ap && *ap);
	Arena_free(*ap);
	free(*ap);
	*ap = NULL;
}


/**
 * Allocates and returns an arena structure with it's fields set to null pointers
 * which denotes an empty arena
 * 
 * @return 		New allocated arena structure
 */
T Arena_new(void){
	T arena = malloc(sizeof(*arena));

	if (arena == NULL)
		RAISE(Arena_NewFailed);

	arena->prev = NULL;
	arena->limit = arena->avail = NULL;
	return arena;
}


