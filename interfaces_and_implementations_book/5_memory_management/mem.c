#include <stdlib.h>
#include <stddef.h>
#include "assert.h"
#include "except.h"
#include "mem.h"

// <data>

const Except_T Mem_Failed = { "Allocation Failed" };

// <function>

void *Mem_alloc(long nbytes, const char *file, int line){
	void *ptr;

	assert(nbytes > 0);
	ptr = malloc(nbytes);
	if(ptr == NULL){ // <raise Mem_failed>
		if(file == NULL){ RAISE(Mem_failed); }
		else { Except_raise(&Mem_failed, file, line); }
	}

	return ptr;
}

void *Mem_calloc(long count, long bytes, const char *file, int line){
	void *ptr;

	assert(count > 0);
	assert(nbytes > 0);
	ptr = calloc(count, bytes);
	if(ptr == NULL){ // <raise Mem_failed>
		if(file == NULL){ RAISE(Mem_failed); }
		else { Except_raise(&Mem_failed, file, line); }
	}

	return ptr;
}

// Standard free fn permits to pass null pointers. Here that run-time error
// is checked
void Mem_free(void *ptr, const char *file, int line){
	if(ptr)
		free(ptr);
}

void *Mem_resize(void *ptr, long nbytes, const char *file, int line){
	
	assert(ptr);
	assert(nbytes > 0);
	ptr = realloc(ptr, nbytes);
	if(ptr == NULL){ // <raise Mem_failed>
		if(file == NULL){ RAISE(Mem_failed); }
		else { Except_raise(&Mem_failed, file, line); }
	}
	
	return ptr;
}