/* Arena-based algorithms, which allocate memory
 * from an arena and deallocate entire arenas at once.
 * 
 * The arena-based allocator encourages simple applicative
 * algorithms in place of algorithms that might be more space-efficient but
 * are always more complex because they must remember when to call
 * free.
 *
 * There are two disadvantages of the arena-based scheme: It can use
 * more memory, and it can create dangling pointers. If an object is allocated
 * in the wrong arena and that arena is deallocated before the program
 * is done with the object, the program will reference either
 * unallocated memory or memory that has been reused for another, perhaps
 * unrelated, arena. It’s also possible to allocate objects in an arena
 * that isn’t deallocated as early as expected, which creates a storage leak.
 */


#ifndef ARENA_INCLUDED
#define ARENA_INCLUDED

#include "except.h"

#define T Arena_T
typedef struct T *T;

extern const Except_T Arena_NewFailed;
extern const Except_T Arena_Failed;

// <exported functions>
extern T Arena_new(void);
extern void Arena_dispose(T *ap);

extern void *Arena_alloc(T arena, long nbytes, const char *file, int line);
extern void *Arena_calloc(T arena, long count, long nbytes, const char *file, int line);
extern void Arena_free(T arena);

#undef T
#endif