#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include "assert.h"
#include "mem.h"
#include "thread.h"
#include "sem.h"

void _MONITOR(void) {}
extern void _ENDMONITOR(void);


#define T Thread_T

////////////
// macros //
////////////

#define isempty(q) ((q) == NULL)


///////////
// types //
///////////

struct T {
	unsigned long *sp; /* must be first */
	// fields
	T link;
	T *inqueue;
	T handle;
	Except_Frame *estack;
	int code;
	T join;
	T next;
	int alerted;
};


//////////
// data //
//////////

const Except_T Thread_Alerted = { "Thread alerted" };
const Except_T Thread_Failed = { "Thread failed" };

static T current;
static T freelist;
static T join0;
static T ready = NULL;
static int critical;
static int nthreads;
static struct Thread_T root;


////////////////
// prototypes //
////////////////

extern void _swtch (T from, T to); // machine dependent code


//////////////////////
// static functions //
//////////////////////

static void delete (T t, T *q) {
	T p;

	assert(t->link && t->inqueue == q);
	assert(!isempty(*q));
	for (p = *q; p->link != t; p = p->link)
		;

	if(p == t)
		*q = NULL;
	else {
		p->link = t->link;
		if (*q == t)
			*q = p;
	}
	t->link = NULL;
	t->inqueue = NULL;
}


static T get(T *q) {
	T t;

	assert(!isempty(*q));
	t = (*q)->link;
	if (t == *q)
		*q = NULL;
	else
		(*q)->link = t->link;

	assert(t->inqueue == q);
	t->link = NULL;
	t->inqueue = NULL;
	return t;
}


static int interrupt (int sig, int code, struct sigcontext *scp) {
	if (critical || scp->sc_pc >= (unsigned long)_MONITOR && scp->sc_pc <= (unsigned long)_ENDMONITOR)
		return 0;

	put(current, &ready);
	sigsetmask(scp->sc_mask);
	run();
	return 0;
}


static void put (T t, T *q) {
	assert(t);
	assert(t->inqueue == NULL && t->link == NULL);
	if (*q) {
		t->link = (*q)->link;
		(*q)->link = t;
	} else
		t->link = t;

	*q = t;
	t->inqueue = q;
}


static void release (void) {
	T t;
	// begin critical region
	do { critical++;
		while((t = freelist) != NULL) {
			freelist = t->next;
			FREE(t);
		}
	// end critical region
	critical--; } while (0);
}


static void run (void) {
	T t = current;

	current = get(&ready);
	t->estack = Except_stack;
	Except_stack = current->stack;
	_swtch(t, current);
}


static void testalert (void) {
	if (current->alerted) {
		current->alerted = 0;
		RAISE(Thread_Alerted);
	}
}


//////////////////////
// thread functions //
//////////////////////

void Thread_alert(T t) {
	assert(current);
	assert(t && t->handle == t);
	t->alerted = 1;
	if (t->inqueue) {
		delete(t, t->inqueue);
		put(t, &ready);
	}
}


void Thread_exit (int code) {
	assert(current);
	release();
	if (current != &root) {
		current->next = freelist;
		freelist = current;
	}
	current->handle = NULL;
	// resume threads waiting for current's termination
	while (!isempty(current->join)) {
		T t = get(&current->join);
		t->code = code;
		put(t, &ready);
	}

	if (!isempty(join0) && nthreads == 2) {
		assert(isempty(ready));
		put(get(&join0), &ready);
	}

	// run another thread or exit
	if (--nthreads == 0)
		exit(code);
	else
		run();
}


int Thread_init (int preempt, ...) {
	assert(preempt == 0 || preempt == 1);
	assert(current == NULL);
	root.handle = &root;
	current = &root;
	nthreads = 1;
	if (preempt) {
		// initialize preemptive scheduling
		{
			struct sigaction sa;
			memset(&sa, '\0', sizeof sa);
			sa.sa_handler = (void (*)())interrupt;
			if (sigaction(SIFVTALRM, &sa, NULL) < 0)
				return 0;
		}
		{
			struct itimerval it;
			it.it_value.tv_sec = 0;
			it.it_value.tv_usec = 50;
			it.it_interval.tv_sec = 0;
			it.it_interval.tv_usec = 50;
			if (setitimer(ITIMER_VIRTUAL, &it, NULL) < 0)
				return 0;
		}

	}
	return 1;
}


int Thread_join (T t) {
	assert(current && t != current);
	testalert();
	if (t) {
		// wait for thread t to terminate
		if (t->handle == t) {
			put(current, &t->join);
			run();
			testalert();
			return current->code;
		} else
			return -1;

	} else {
		// wait for all threads to terminate
		assert(isempty(join0));
		if (nthreads > 1) {
			put(current, &join0);
			run();
			testalert();
		}

		return 0;
	}
}


T Thread_new (int apply (void *), void *args, int nbytes, ...) {
	T t;

	assert(current);
	assert(apply);
	assert(args && nbytes >= 0 || args == NULL);
	if (args == NULL)
		nbytes = 0;

	// allocate resources for a new thread
	{
		/* Initializes the stack pointer aligned on a 16-byte boundary*/
		int stacksize = (16 * 1024 + sizeof(*t) + nbytes + 15)&~15;
		release();
		// begin critical region
		do { critical++;

			TRY
				t = ALLOC(stacksize);
				memset(t, '\0', sizeof *t);
			EXCEPT(Mem_Failed)
				t = NULL;
			END_TRY;

		// end critical region
		critical--; } while (0);

		if (t == NULL)
			RAISE(Thread_Failed);

		// initialize t's stack pointer
		t->sp = (void *)((char *)t + stacksize);
		while (((unsigned long)t->sp)&15) 
			t->sp--;
		
	}

	t->handle = t;
	// initialize t's state
	if (nbytes > 0) {
		t->sp -= ((nbytes + 15U)&~15)/sizeof (*t->sp);
		// begin critical region
		memcpy(t->sp, args, nbytes);
		// end critical region
		args = t->sp;
	}
	#if alpha
	{
		// initialize an ALPHA stack
		extern void _start(void);
		t->sp -= 112/8;
		t->sp[(48+24)/8] = (unsigned long)Thread_exit;
		t->sp[(48+16)/8] = (unsigned long)args;
		t->sp[(48+8)/8] = (unsigned long)apply;
		t->sp[(48+0)/8] = (unsigned long)_start;
	} 
	#elif mips
	{
		// initialize a MIPS stack
		extern void _start(void);
		t->sp -= 16/4;
		t->sp -= 88/4;
		t->sp[(48+20)/4] = (unsigned long)Thread_exit;
		t->sp[(48+28)/4] = (unsigned long)args;
		t->sp[(48+32)/4] = (unsigned long)apply;
		t->sp[(48+36)/4] = (unsigned long)_start;
	}
	#elif sparc
	{
		// initilize a SPARC stack
		int i; void *fp; extern void_start(void);
		for (i = 0; i < 8; i++)
			*--t->sp = 0;
		*--t->sp = (unsigned long)args;
		*--t->sp = (unsigned long)apply;
		t->sp -= 64/4;
		fp = t->sp;
		*--t->sp = (unsigned long)_start - 8;
		*--t->sp = (unsigned long)fp;
		t->sp -= 64/4;
	}
	#else
	// Unsuported platform
	#endif
	
	nthreads++;
	put(t, &ready);
	return t;
}


void Thread_pause (void) {
	assert(current);
	put(current, &ready);
	run();
}


T Thread_self (void) {
	assert(current);
	return current;
}


#undef T




#define T Sem_T

///////////////////
// sem functions //
///////////////////

void Sem_init (T *s, int count) {
	assert(current);
	assert(s);
	s->count = count;
	s->queue = NULL;
}


T *Sem_new (int count) {
	T *s;

	NEW(s);
	Sem_init(s, count);
	return s;
}


void Sem_signal (T *s) {
	assert(current);
	assert(s);
	if (s->count == 0 && !isempty(s->queue)) {
		Thread_T t = get((Thread_T *)&s->queue);
		assert(!t->alerted);
		put(t, &ready);
	} else
		++s->count;
}


void Sem_wait (T *s) {
	assert(current);
	assert(s);
	testalert();
	if (s->count <= 0){
		put(current, (Thread_T *)&s->queue);
		run();
		testalert();
	} else {
		--s->count
	}
}



#undef T












