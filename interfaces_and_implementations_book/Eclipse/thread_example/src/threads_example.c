/*
 ============================================================================
 Name        : threads_example.c
 Author      : misan128
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
/**
 * Any data that can be accessed by more than one thread in a preemptive system must be protected.
 * Access must be limited to a critical region in which only one thread at a time is permitted.
 * spin is a simple example of the right way and wrong way to access shared data.
 */

#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "fmt.h"
#include "thread.h"
#include "sem.h"

#define NBUMP 30000

// spin types

struct args {
	Sem_T *mutex;
	int *ip;
};


// spin functions

int safe (void *cl) {
	struct args *p = cl;
	int i;

	for (i = 0; i < NBUMP; i++)
		LOCK(*p->mutex)
			*p->ip = *p->ip + 1;
		END_LOCK;

	return EXIT_SUCCESS;
}


int unsafe (void *cl) {
	int i, *ip = cl;

	for (i = 0; i < NBUMP; i++)
		*ip = *ip + 1;

	return EXIT_SUCCESS;
}


int n;

int main (int argc, char *argv[]) {
	int m = 5, preempt;

	preempt = Thread_init(1, NULL);
	assert(preempt == 1);
	if (argc >= 2)
		m = atoi(argv[1]);

	n = 0;
	// increment n unsafely
	{
		int i;
		for (i = 0; i < m; i ++)
			Thread_new(unsafe, &n, 0, NULL);

		Thread_join(NULL);
	}

	Fmt_print("%d == %d\n", n, NBUMP * m);
	n = 0;

	// increment n safely
	{
		int i;
		struct args args;
		Sem_T mutex;
		Sem_init(&mutex, 1);
		args.mutex = &mutex;
		args.ip = &n;
		for (i = 0; i < m; i++)
			Thread_new(safe, &args, sizeof args, NULL);

		Thread_join(NULL);
	}

	Fmt_print("%d == %d\n", n, NBUMP * m);
	Thread_exit(EXIT_SUCCESS);

	return EXIT_SUCCESS;
}








