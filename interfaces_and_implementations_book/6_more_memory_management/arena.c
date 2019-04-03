#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "arena.h"

#define T Arena_T


const Except_T Arena_NewFailed = { "Arena Creation Failed" };
const Except_T Arena_Failed = { "Arena Allocation Failed" };

//////////////
// <macros> //
//////////////
#define THRESHOLD 10

/////////////
// <types> //
/////////////

/**
 * The size of the union give the minimun alignment on the host machine. Its fields
 * are those thar are most likely to have the strictest alignment requirements, and
 * it is used to round nbytes.
 */
union align {
	int i;
	long l;
	long *lp;
	void *p;
	void (*fp)(void);
	float f;
	double d;
	long double ld;
};

/**
 * Ensures that new arena->avail is set to a properly aligned addres for the
 * first allocationn of this chunk.
 */
union header {
	struct T b;
	union align a;
};

/**
 * Arena struct
 */
struct T {
	T prev; // Points to the head of the chunk
	char *avail; // Points to the chunk's first free location
	char *limit; // The space between avail and limit is available for allocation
};


////////////
// <data> //
////////////

static T freechunks;
static int nfree;


/////////////////
// <functions> //
/////////////////

/**
 * Allocates a space chunk in an already initialized arena structure. Rounds the requested
 * amount up to the proper alignment boundary, increments the avail pointer by the amount
 * of the rounded request, and returns the previous value.
 * 
 * @param  {T} 				arena   Arena structure
 * @param  {long} 			nbytes  Number of bytes to allocate in arena
 * @param  {const char *} 	file   	Execution file name fot exception
 * @param  {int} 			line   	Execution line number for exception
 * @return        			Pointer to allocated memory space
 */
void *Arena_alloc(T arena, long nbytes, const char *file, int line){
	assert(arena);
	assert(nbytes > 0);

	// <round nbytes up to an alignment boundary>
	nbytes = ((nbytes + sizeof(union align) - 1) / (sizeof(union align))) * (sizeof(union align));
	
	while(nbytes > arena->limit - arena->avail){
		// <get a new chunk>
		T ptr;
		char *limit;

		// <ptr <- a new chunk>
		if ((ptr = freechunks) != NULL){
			freechunks = freechunks->prev;
			nfree--;
			limit = ptr->limit;
		} else {
			long m = sizeof(union header) + nbytes + 10*1024;
			ptr = malloc(m);
			if (ptr == NULL){
				// <raise Arena_Failed>
				if (file == NULL){ RAISE(Arena_Failed); }
				else { Except_raise(&Arena_Failed, file, line); }

			}
			limit = (char *)ptr + m;
		}
		
		*ptr = *arena;
		arena->avail = (char *)((union header *)ptr + 1);
		arena->limit = limit;
		arena->prev = ptr;
	}

	arena->avail += nbytes;
	return arena->avail - nbytes;
}

/**
 * Allocates a contiguous number of bytes. Uses Arena_alloc
 * @param  {T} 				arena   Arena structure
 * @param  {long}			count	Number of blocks of nbytes size to allocate
 * @param  {long} 			nbytes  Number of bytes of each count block
 * @param  {const char *} 	file   	Execution file name fot exception
 * @param  {int} 			line   	Execution line number for exception
 * @return        			Pointer to allocated memory space
 */
void *Arena_calloc(T arena, long count, long nbytes, const char *file, int line){
	void *ptr;

	assert(count > 0);
	ptr = Arena_alloc(arena, count * nbytes, file, line);
	memset(ptr, '\0', count * nbytes);
	return ptr;
}

/**
 * Deallocates the chunks in the arena; then frees the arena structure itself and clears
 * the pointer to arena
 * 
 * @param {T} ap Arena structure pointer
 */
void Arena_dispose(T *ap) {
	assert(ap && *ap);
	Arena_free(*ap);
	free(*ap);
	*ap = NULL;
}

/**
 * Frees teh allocated chunks space of an arena. If the number of free arenas is lower than
 * THRESHOLD the pointer to the begining of the memory block, otherwise it'll calls free()
 * 
 * @param {T} arena Arena structure to deallocate
 */
void Arena_free(T arena) {
	assert(arena);
	while (arena->prev) {
		struct T tmp = *arena->prev;

		// <free the chunk described by arena>
		if (nfree < THRESHOLD) {
			arena->prev->prev = freechunks;
			freechunks = arena->prev;
			nfree++;
			freechunks->limit = arena->limit;
		} else {
			free(arena->prev);
		}
		
		*arena = tmp;
	}
	assert(arena->limit == NULL);
	assert(arena->avail == NULL);
}


/**
 * Allocates and returns an arena structure with it's fields set to null pointers
 * which denotes an empty arena
 * 
 * @return New allocated arena structure
 */
T Arena_new(void){
	T arena = malloc(sizeof(*arena));

	if (arena == NULL)
		RAISE(Arena_NewFailed);

	arena->prev = NULL;
	arena->limit = arena->avail = NULL;
	return arena;
}


