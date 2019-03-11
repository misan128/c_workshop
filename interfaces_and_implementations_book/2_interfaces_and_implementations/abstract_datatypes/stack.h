#ifndef STACK_INCLUDED
#define STACK_INCLUDED

// By convention, an interface X that specifies an ADT defines it as a type
// named X_T. The interfaces in this book carry this convention one step
// further by using a macro to abbreviate X_T to just T within the interface.

#define T Stack_T

typedef struct T *T;

extern T 	Stack_new 	(void);
extern int 		Stack_empty (T stk);
extern void		Stack_push	(T stk, void *x);
extern void	   *Stack_pop	(T stk);
extern void		Stack_free	(T *stk);

#undef T
#endif