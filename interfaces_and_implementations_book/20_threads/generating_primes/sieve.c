/**
 * The last example illustrates a pipeline implemented by communication channels. sieve
 * N computes and prints the prime numbers less than or equal to N. For example:
 *
 * 		% sieve 100
 * 		2 3 5 7 9 11 13 17 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97
 *
 * sieve is an implementation of the well-known Sieve of Eratosthenes for computing
 * primes, in which each "sieve" is a thread that discards multiples of its primes.
 * Channels connect these threads to form a pipeline as depicted in Figure 20.1. The
 * source thread (the white box) generates two followed by the odd integers, and fires
 * them down the pipe. The filters (the light gray boxes) between the source and the sink
 * (the dark gray box) discard numbers that are multiples of their primes, and pass the
 * others down the pipe. The sink also filters out its primes, but if a number gets by the
 * sink's filter, it is a prime. Each box in Figure 20.1 is a thread; the numbers in each
 * box are the primes associated with that thread, and the lines between the boxes that
 * form the pipeline are channels.
 *
 * There are n primes attached to the sink and to each filter. When the sink has accumulated
 * n primes - 5 in figure 20.1 - it spawns a fresh copy of itself and turns itself into a
 * filter. Figure 20.2. shows how the sieve expands as it computes the primes up to including
 * 100.
 *
 * After sieve initializes the thread system, it creates threads for the source and for
 * the sink, connects them with a new channel, and exits.
 */

#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "fmt.h"
#include "thread.h"
#include "chan.h"


struct args {
	Chan_T c;
	int t, last;
};

// sieve functions

void filter (int primes[], Chan_T input, Chan_T output) {
	int j, x;

	for (;;) {
		Chan_receive(input, &x, sizeof x);
		// x is a multiple of primes [0...]
		for (j = 0; primes[j] != 0 && x % primes[j] != 0; j++)
			;

		if (primes[j] == 0)
			if(Chan_send(output, &x, sizeof x) == 0)
				break;
	}
	Chan_receive(input, &x, 0);
}


int sink (void *cl) {
	struct args *p = cl;
	Chan_T input = p->c;
	int i = 0, j, x, primes[256];

	primes[0] = 0;
	for (;;) {
		Chan_receive(input, &x, sizeof x);
		// x is a multiple of primes[0...]

		if (primes[j] == 0) {
			// x is prime
			if (x > p->last)
				break;

			Fmt_print(" %d", x);
			primes[i++] = x;
			primes[i] = 0;
			if (i == p->n) // spawn a new sink and call filter
			{
				p->c = Chan_new();
				Thread_new(sink, p, sizeof *p, NULL);
				filter(primes, input, p->c);
				return EXIT_SUCCESS;
			}

		}
	}
	Fmt_print("\n");
	Chan_receive(input, &x, 0);
	return EXIT_SUCCESS;
}


int source (void *cl) {
	struct args *p = cl;
	int i = 2;

	if (Chan_send(p->c, &i, sizeof i))
		for (i = 3; Chan_send(p->c, &i, sizeof i); )
			i += 2;

	return EXIT_SUCCESS;
}


//////////
// MAIN //
//////////

int main (int argc, char *argv[]) {
	struct args args;

	Thread_init(1, NULL);
	args.c = Chan_new();
	Thread_new(source, &args, sizeof args, NULL);
	args.n = argc > 2 ? atoi(argv[2]) : 5;
	args.last = argc > 1 ? atoi(argv[1]) : 1000;
	Thread_new(sink, &args, sizeof args, NULL);
	Thread_exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}




