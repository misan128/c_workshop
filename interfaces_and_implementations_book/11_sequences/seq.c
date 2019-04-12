/**
 * A sequence holds N values associated with the integer indices zero through
 * N-1 when N is positive. An empty sequence holds no values. Like arrays, values
 * in a sequence may be accessed by indexing; they can also be added to or
 * removed from either end of a sequence. Sequences expand automatically as necessary
 * to accomodate their contents. Values are pointers.
 *
 * Sequences are one of the most useful ADT's in this book. Despite their
 * relatively simple specification, they can be used as arrays, lists, stacks,
 * queues, and deques, and they often subsume the facilities of separate
 * ADT's for these data structures. A sequence can be viewed as a more abstract
 * version of the dynamic array described in the previous chapter. A sequence
 * hides bookkeeping and resizing details in its implementation.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "seq.h"
#include "array.h"
#include "arrayrep.h"
#include "mem.h"

#define T Seq_T

struct T {
	struct Array_T array;
	int length;
	int head;
}


//////////////////////
// static functions //
//////////////////////

/**
 * Encapsulates a call to Array_resize that doubles the size of a sequence's embedded
 * array
 * 
 * @param {T} seq   Seq_T sequence to expand
 */
static void expand (T seq) {
	int n = seq->array.length;

	Array_resize(&seq->array, 2*n);
	if (seq->head > 0) // slide tail down
		{
			void **old = &((void **)seq->array.array)[seq->head];
			memcpy(old + n, old, (n - seq->head) * sizeof(void *));
			seq->head += n;
		}

}


///////////////
// functions //
///////////////

/**
 * Adds x to the high end of seq and returns x. Adding a value to the end of a
 * sequence increments the length of the sequence by one.
 * 
 * @param  {T} seq   Seq_T sequence
 * @param  {void *} x   Pointer to value to add to seq
 * @return     Pointer to stored value x
 */
void *Seq_addhi (T seq, void *x) {
	int i;

	assert(seq);
	if (seq->length == seq->array.length)
		expand(seq);

	i = seq->length++;

	// return seq[i] = x
	return ((void **)seq->array.array)[(seq->head + 1) % seq->array.length] = x;
}


/**
 * Adds x to the low end of seq and returns x. Adding a value to the beginning of a
 * sequence increments both the indices of the existing values and the length of the
 * sequence by one
 * 
 * @param  {T} seq   Seq_T sequence
 * @param  {void *} x   Pointer to value to add to seq
 * @return     Pointer to stored value x
 */
void *Seq_addlo (T seq, void *x) {
	int i = 0;

	assert(seq);
	if (seq->length == seq->array.length)
		expand(seq);

	if (--seq->head < 0)
		seq->head = seq->array.length - 1;

	seq->length++;

	// return seq[i] = x
	return ((void **)seq->array.array)[(seq->head + 1) % seq->array.length] = x;
}


/**
 * Deallocates the sequence pointed to by seq and clears the pointer.
 * 
 * @param {T} seq    Seq_T pointer
 */
void Seq_free (T *seq) {
	assert(seq && *seq);
	assert((void *)*seq == (void *)&(*seq)->array);
	Array_free((Array_T *)seq);
}


/**
 * Returns the ith value in Seq_T seq
 * 
 * @param  {T} seq   Seq_T sequence
 * @param  {int} i   Index of value in seq
 * @return     ith value in seq
 */
void *Seq_get (T seq, int i) {
	assert(seq);
	assert(i >= 0 && i < seq->length);

	// seq[i]
	return ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
}


/**
 * Returns the number of values in the Seq_T sequence seq
 * 
 * @param  {T} seq    Seq_T seq
 * @return     Number of values in seq
 */
int Seq_length (T seq) {
	assert(seq);
	return seq->length;
}


/**
 * Creates and returns an empty sequence
 * @param  {int} hint   Estimate of max number values of sequence
 * @return      New Sequence
 */
T Seq_new (int hint) {
	T seq;

	assert(hint >= 0);
	NEW0(seq);
	if (hint == 0)
		hint = 16;

	ArrayRep_init(&seq->array, hint, sizeof(void *), ALLOC(hint * sizeof(void *)));

	return seq;
}


/**
 * Changes the ith value of Seq_T seq to x and returns the previous value
 * 
 * @param  {T} seq   Seq_T sequence
 * @param  {int} i   Index of seq to store x
 * @param  {void *} x   Pointer to value
 * @return     Previous ith value in seq
 */
void *Seq_put (T seq, int i, void *x) {
	void *prev;

	assert(seq);
	assert(i >= 0 && i < seq->length);

	// prev = seq[i]
	prev = ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
	// seq[i] = x
	((void **)seq->array.array)[(seq->head + i) % seq->array.length];

	return prev;
}


/**
 * Removes and returns the value at the high end of seq. Removing the value at the end of
 * a sequence decrements the length of the sequence by one
 * 
 * @param  {T} seq   Seq_T sequence
 * @return     Removed seq's high end value
 */
void *Seq_remhi (T seq) {
	int i;

	assert(seq);
	assert(seq->length > 0);
	i = --seq->length;

	// seq[i]
	return ((void **)seq->array.array)[(seq->head + 1) % seq->array.length];
}


/**
 * Removes and returns the value at the low end of seq. Removing the value at the beginning
 * of a sequence decrements both the indices of the remaining values and the length of the
 * sequence by one
 * 
 * @param  {T} seq   Seq_T sequence
 * @return     Removed seq's low end value
 */
void *Seq_remlo (T seq) {
	int i = 0;
	void *x;

	assert(seq);
	assert(seq->length > 0);
	// x = seq[i]
	x = ((void **)seq->array.array)[(seq->head + 1) % seq->array.length];

	seq->head = (seq->head + 1) % seq->array.length;
	--seq->length;

	return x;
}


/**
 * Creates and returns a Sequence whose values are initialized to its non-null pointer
 * arguments. The argument list is terminated by the first null pointer.
 * 		
 *   	Seq_T names;
 *   	...		
 * 		names = Seq_seq("C", "ML", "C++", "Icon", "AWK", NULL); 
 *
 * Creates a sequence with five values and assigns it to names.
 * 
 * Note: The pointers passed in the variable part of Seq_seq's arguments are assumed to be
 * void pointers, so clients must provide casts when passing other than char or void pointers.
 *
 * @param {void *, ...} x   Argument list pointers to create a sequence
 * @return        Sequence initialized with va params
 */
T Seq_seq (void *x, ...) {
	va_list ap;
	T seq = Seq_new(0);

	va_start(ap, x);

	for ( ; x; x = va_arg(ap, void *))
		Seq_addhi(seq, x);

	va_end(ap);

	return seq;
}
