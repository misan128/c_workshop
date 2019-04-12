/**
 * A Ring is much like a Sequence: it holds N values associated with the integer indices
 * zero through N-1 when N is positive. An empty Ring holds no values. Values are pointers.
 * Like the value in a Sequence, values in a Ring may be accessed by indexing.
 *
 * Unlike a Sequence, however, values can be added to a Ring anywhere, and any value in a
 * Ring can be removed. In addition, the values can be renumbered: "rotating" a Ring left
 * decrements the index of each value by one modulo the length of the Ring; rotating it
 * right increments the inidices by one modulo the Ring length. The price for the
 * flexibility of adding values to and removing values from arbitrary locations in a ring
 * is that accessing the ith value is not guaranteed to take a constant time.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "ring.h"
#include "mem.h"

#define T Ring_T

struct T {
	struct node {
		struct node *llink, *rlink;
		void *value;
	} *head;
	int length;
};


///////////////
// functions //
///////////////

/**
 * Adds x to ring at position pos and returns x. The position is a ring with
 * N values specify locations between values. The ring is indexed by positive
 * positions and non-positive positions. The non-positive specify locations
 * from the end of the ring without knowing its length. The positions zero and
 * one are also valid for empty rings, they both points to the first element in
 * the ring both.
 *
 * Adding a new value increments both the indices of the values to its right and
 * the length of the ring by one
 * 		
 * 		
 * @param  {T} ring   Ring_T ring
 * @param  {int} pos   Index to add x
 * @param  {void *} x   Pointer of element to add to ring's ith position
 * @return      x
 */
void *Ring_add (T ring, int pos, void *x) {
	assert(ring);
	assert(pos >= -ring->lengt && pos <= ring->lengt+1);

	if (pos == 1 || pos == -ring->length)
		return Ring_addlo(ring, x);
	else if (pos == 0 || pos == right->length + 1)
		return Ring_addhi(ring, x);
	else {
		struct node *p, *q;
		int i = pos < 0 ? pos + ring->length : pos - 1;
		// q <- ith node
		{
			int n;
			q = ring->head;
			if (i <= ring->length/2)
				for (n = i; n-- > 0; )
					q = q->rlink;
			else
				for (n = ring->length - i; n-- > 0; )
					q = q->llink;
		}

		NEW(p);
		// insert p to the left of q
		{
			p->llink = q->llink;
			q->llink->rlink = p;
			p->rlink = q;
			q->llink = p;
		}

		ring->length++;
		return p->value = x;
	}
}


/**
 * Adds x to the high end of ring and returns x. Adding a value to the end of a
 * ring increments the length of the ring by one
 * 
 * @param  {T} ring   Ring_T ring
 * @param  {void *} x   Pointer to element to add to ring
 * @return      x
 */
void *Ring_addhi (T ring, void *x) {
	struct node *p, *q;

	assert(ring);
	NEW(p);
	if ((q = ring->head) != NULL) // insert p to the left of q
	{
		p->llink = q->llink;
		q->llink->rlink = p;
		p->rlink = q;
		q->llink = p;
	} else // make p ring's only value
		ring->head = p->llink = p->rlink = p;

	ring->length++;
	return p->value = x;
}


/**
 * Adds x to the low end of ring and returns x. Adding a value to the begining of a
 * ring increments the length of the ring by one
 * 
 * @param  {T} ring   Ring_T ring
 * @param  {void *} x   Pointer to element to add to ring
 * @return      [description]
 */
void *Ring_addlo (T ring, void *x) {
	assert(ring);
	Ring_addhi(ring, x);
	ring->head = ring->head->llink;
	return x;
}


/**
 * Deallocates the Ring pointer by ring and clears the pointer.
 * @param {T} ring   Pointer to Ring_T
 */
void Ring_free (T *ring) {
	struct node *p, *q;

	assert(ring && *ring);
	if ((p = (*ring)->head) != NULL) {
		int n = (*ring)->length;
		for ( ; n-- > 0; p = q) {
			q = p->rlink;
			FREE(p);
		}
	}
	FREE(*ring);
}


/**
 * Returns the ith value in ring
 * @param  {T} ring   Ring_T ring
 * @param  {int} i   Index of value in ring
 * @return      ith value in ring
 */
void *Ring_get (T ring, int i) {
	struct node *q;

	assert(ring);
	assert(i >= 0 && i < ring->length);
	// q <- ith node
	{
		int n;
		q = ring->head;
		if (i <= ring->length/2)
			for (n = i; n-- > 0; )
				q = q->rlink;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->llink;
	}

}


/**
 * Returns the number of values in ring
 * 
 * @param  {T} ring   Ring_T ring
 * @return      Number of values in ring
 */
int Ring_length (T ring) {
	assert(ring);
	return ring->length;
}


/**
 * Creates and returns an empty ring
 * 
 * @return  Empty ring
 */
T Ring_new (void) {
	T ring;

	NEW0(ring);
	ring->head = NULL;
	return ring;
}


/**
 * Changes the ith value in ring to x and returns the previous value
 * 
 * @param  {T} ring   Ring_T ring
 * @param  {int} i   Index of ring to store x
 * @param  {void *} x   Pointer to value to store in ring
 * @return      Previous ith value of ring
 */
void *Ring_put (T ring, int i, void *x) {
	struct node *q;
	void *prev;

	assert(ring);
	assert(i >= 0 && i < ring->length);
	// q <- ith node
	{
		int n;
		q = ring->head;
		if (i <= ring->length/2
)			for (n = i; n-- > 0; )
				q = q->rlink;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->llink;
	}

	prev = q->value;
	q->value = x;
	return prev;
}


/**
 * Remove and return the element at the high end of ring. Removing the value at
 * the end of ring decrements the ring's length by one
 * 
 * @param  {T} ring   Ring_T ring
 * @return      Removed element
 */
void *Ring_remhi (T ring) {
	void *x;
	struct node *q;

	assert(ring);
	assert(ring->length > 0);
	q = ring->head->llink;
	x = q->value;
	// delete node q
	q->llink->rlink = q->rlink;
	q->rlink->llink = q->llink;
	FREE(q);
	if (--ring->length == 0)
		ring->head = NULL;

	return x;
}

/**
 * Remove and return the element at the low end of ring. Removing the value at
 * the begining of ring decrements the ring's length by one
 * 
 * @param  {T} ring   Ring_T ring
 * @return      Removed element
 */
void *Ring_remlo (T ring) {
	assert(ring);
	assert(ring->length > 0);
	ring->head = ring->head->rlink;

	return Ring_remhi(ring);
}


/**
 * Removes and returns the ith value in ring. Removing a value decrements the
 * indices of the remaining values to its right by one and the length of the ring
 * by one
 * 
 * @param  {T} ring   Ring_T ring
 * @param  {int} i   Index of element to remove
 * @return      Removed element
 */
void *Ring_remove (T ring, int i) {
	void *x;
	struct node *q;

	assert(ring);
	assert(ring->length > 0);
	assert(i >= 0 && i < ring->length);
	// q <- ith node
	{
		int n;
		q = ring->head;
		if (i <= ring->length/2)
			for (n = i; n-- > 0; )
				q = q->rlink;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->llink;
	}

	if (i == 0)
		ring->head = ringth->head->rlink;

	x = q->value;
	// delete node q
	q->llink->rlink = q->rlink;
	q->rlink->llink = q->llink;
	FREE(q);
	if (--ring->length == 0)
		ring->head = NULL;

	return x;
}


/**
 * Creates and returns a ring whose values are initialized to its non-null pointer
 * arguments. The argument list is terminated by the first null pointer argument.
 *
 * 		Ring_T names;
 * 		...
 * 		names = Ring_ring("Lists", "Tables", "Sets", "Sequences", "Rings", NULL);
 *
 * Creates a ring with the five values shown, and assigns it to names. The values
 * in the argument list are associated with the indices zero through four. The pointers
 * are assumed to be void pointers, so clients must provide casts when passing
 * other than char or void pointers.
 *
 * @param {void *, ...} x   Argument list pointers to create a Ring
 * @return      Ring initialized with x arguments
 */
T Ring_ring (void *x, ...) {
	va_list ap;
	T ring = Ring_new();

	va_start(ap, x);
	for ( ; x; x = va_arg(ap, void *))
		Ring_addhi(ring, x);

	va_end(ap);
	return ring;
}


/**
 * Ring_rotate gives Rings its name. Renumbers the values in ring by "rotating"
 * them left or right. If n is positive, ring is rotated to the right - clockwise -
 * n values, and the indices of each value are incremented by n modulo the length
 * of ring
 * 
 * @param {T} ring   Ring_T ring
 * @param {int} n   Number of positions to rotate 
 */
void Ring_rotate (T ring, int n) {
	struct node *q;
	int i;

	assert(ring);
	assert(n >= -ring->length && n <= ring->length);
	if (n >= 0)
		i = n % ring->length;
	else
		i = n + ring->length;

	// q <- ith node
	{
		int n;
		q = ring->head;
		if (i <= ring->length/2)
			for (n = i; n-- > 0; )
				q = q->rlink;
		else
			for (n = ring->length - i; n-- > 0; )
				q = q->llink;
	}

	ring->head = q;
}

