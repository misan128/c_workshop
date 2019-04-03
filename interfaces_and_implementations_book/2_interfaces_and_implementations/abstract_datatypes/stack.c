#include <stddef.h>
#include "assert.h"
#include "mem.h"
#include "stack.h"

#define T Stack_T

// <types>

struct T {
	int count;
	struct elem {
		void *x;
		struct elem *link;
	} *head;
}

// <functions>

// Stack_new allocates and initializes a new T
T Stack_new(void){
	T stk;
	NEW(stk); // NEW -> allocation macro from Mem interface
	stk->count = 0;
	stk->head = NULL;
	return stk;
}

int Stack_empty(T stk){
	assert(stk); // assert implements the checked runtime error that forbids a null T to be passed to any
				 // function in Stack. assert(e) is an assertion that e is nonzero for any expression e.
				 // It does nothing if e is nonzero, and halts program execution otherwise
	return stk->count == 0;
}

// Stack_push and Stack_pop add and remove elements from the head
// of the linked list emanating from stk->head
void Stack_push(T stk, void *x){
	struct elem *t;

	assert(stk);
	NEW(t);
	t->x = x;
	t->link = stk->head;
	stk->head = t;
	stk->count ++;
}

void *Stack_pop(T stk){
	void *x;
	struct elem *t;
	assert(stk);
	assert(stk->count > 0);
	t = stk->head;
	stk->head = t->link;
	stk->count--;
	x = t->x;
	FREE(t); // deallocates space pointed to by its pointer argument, then sets the argument to the null
			 // pointer
	return x;
}


void Stack_free(T *stk){
	struct elem *t, *u;

	assert(stk && *stk);
	for(t = (*stk)->head; t; t = u){
		u = t->link;
		FREE(t);
	}
	FREE(*stk);
}