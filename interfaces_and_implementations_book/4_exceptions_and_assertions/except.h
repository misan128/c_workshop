/* The Except interface wraps the setjmp/longjmp facility in a set of macros
 * and functions that collaborate to provide a structured exception
 * facility.
 */

#ifndef EXCEP_INCLUDED
#define EXCEPT_INCLUDED

// includes
#include <setjmp.h>

// defines
#define T Except_T

typedef struct T {
	const char *reason;
} T;


//<exported types>
typedef struct Except_Frame Except_Frame;
struct Except_Frame {
	Except_Frame *prev;
	jmp_buf env;
	const char *file;
	const T *exception;
}

// There are four states within a TRY statement, as suggested by the following enumeration identifiers.
enum {
	Except_entered = 0,
	Except_raised,
	Except_handled,
	Except_finalized
};

//<exported variables>
extern Except_Frame *Except_stack;

//<exported functions>
void Except_raise(const T *e, const char *file, int line);

//<exported macros>
#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)
#define RERAISE Except_raise(Except_frame.exception, Except_frame.file, Except_frame.line)
#define RETURN  switch(/*<pop>*/ Except_stack = Except_stack->prev, 0) default: return

#define TRY do { \
	volatile int Except_flag; \
	Except_Frame Except_frame; \
	//<push> \
	Except_frame.prev = Except_stack; \
	Except_stack = &Except_frame; \
	\
	Except_flag = setjmp(Except_frame.env); \
	if (Except_flag == Except_entered) {

#define EXCEPT(e) \
		//<pop if this chunk follows S> \
		if (Except_flag == Except_entered) /*<pop>*/ \
			Except_stack = Except_stack->prev; \
	} else if(Except_frame.exception == &(e)) { \
		Except_flag = Except_handled;

#define ELSE(e) \
		//<pop if this chunk follows S> \
		if (Except_flag == Except_entered) /*<pop>*/ \
			Except_stack = Except_stack->prev; \
	} else { \
		Except_flag = Except_handled;

#define FINALLY \
		//<pop if this chunk follows S> \
		if (Except_flag == Except_entered) /*<pop>*/ \
			Except_stack = Except_stack->prev; \
	} { \
		if (Except_flag == Except_enetered) \
			Except_flag = Except_finalized;

#define END_TRY \
		//<pop if this chunk follows S> \
		if (Except_flag == Except_entered) /*<pop>*/ \
			Except_stack = Except_stack->prev; \
	} if(Except_flag == Except_raised) RERAISE; \
} while (0)


#undef T
#endif