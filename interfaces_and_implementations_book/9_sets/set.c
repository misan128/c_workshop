#include <limits.h>
#include <stddef.h>
#include "mem.h"
#include "assert.h"
#include "arith.h"
#include "set.h"


#define T Set_T


///////////
// types //
///////////

struct T {
	int length;
	unsigned timestamp;
	int (*cmp)(const void *x, const void *y);
	unsigned (*hash)(const void *x);
	int size;
	struct member {
		struct member *link;
		const void *member;
	} **buckets;
};


//////////////////////
// static functions //
//////////////////////

static int cmpatom (const void *x, const void *y) {
	return x != y;
}

static unsigned hashatom (const void *x) {
	return (unsigned long)x>>2;
}


///////////////
// functions //
///////////////

int Set_member (T set, const void *member) {
	int i;
	struct member *p;

	assert(set);
	assert(member);
	// <search set for member>
	i = (*set->hash)(member)%set->size;
	for (p = set->buckets[i]; p; p = p->link)
		if ((*set->cmp)(member, p->member) == 0)
			break;
	
	return p != NULL; 
}


T Set_new (int hint,
	int cmp (const void *x, const void *y),
	unsigned hash (const void *x)) {
	T set;
	int i;
	static int primes[] = {509, 509, 1021, 2053, 4093, 8191, 16381, 32771, 65521, INT_MAX };

	assert(hint > 0);
	for (i = 1; primes[i] < hint; i++)
		;

	set = ALLOC(sizeof(*set) + primes[i-1] * sizeof(set->buckets[0]));
	set->size = primes[i-1];
	set->cmp = cmp ? cmp : cmpatom;
	set->hash = hash ? hash : hashatom;
	set->buckets = (struct member **)(set + 1);

	for (i = 0; i < set->size; i++)
		set->buckets[i] = NULL;

	set->length = 0;
	set->timestamp = 0;

	return set;
}


void Set_put(T set, const void *member) {
	int i;
	struct member *p;

	assert(set);
	assert(member);
	// <search set for member>
	i = (*set->hash)(member)%set->size;
	for (p = set->buckets[i]; p; p = p->link)
		if ((*set->cmp)(member, p->member) == 0)
			break;

	if (p == NULL) {
		// <add member to set>
		NEW(p);
		p->member = member;
		p->link = set->buckets[i];
		set->buckets[i] = p;
		set->length++;

	} else {
		p->member = member;
	}

	set->timestamp++;
}


void *Set_remove(T set, const void *member) {
	int i;
	struct member **pp;

	assert(set);
	assert(member);
	set->timestamp++;
	i = (*set->hash)(member)%set->size;
	for (pp = &set->buckets[i]; *pp; pp = &(*pp)->link)
		if ((*set->cmp)(member, (*pp)->member) == 0) {
			struct member *p = *pp;
			*pp = p->link;
			member = p->member;
			FREE(p);
			set->length--;
			return (void *)member;
		}
		return NULL;
}






