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

struct T 	{
	int length;
	unsigned timestamp;
	int (*cmp )(const void *x, const void *y);
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

/**
 * Default function for Set member comparison
 * 
 * @param  {const void *} x   Member to be compare with y
 * @param  {const void *} y   Member to be compare with x
 * @return   0 if equal; 1 if different
 */
static int cmpatom (const void *x, const void *y) {
	return x != y;
}


static T copy (T t, int hint) {
	T set;

	assert(t);
	set = Set_new(hint, t->cmp, t->hash);
	{
		// <for each member q in t>
		int i;
		struct member *q;
		for (i = 0; i < t->size; i++)
			for (q = t->buckets[i]; q; q = q->link) { 
				// <add q->member to set>
				struct member *p;
				const void *member = q->member;
				int i = (*set->hash)(member)%set->size;

				// <add member to set>
				NEW(p);
				p->member = member;
				p->link = set->buckets[i];
				set->buckets[i] = p;
				set->length++;

			}
	}
	return set;
}


/**
 * Default hash function for Set hash association
 * 
 * @param  {const void *} key   Key to be hashed
 * @return     Hashed key
 */
static unsigned hashatom (const void *x) {
	return (unsigned long)x>>2;
}


///////////////
// functions //
///////////////


T Set_diff (T s, T t) {
	if (s == NULL) {
		assert(t);
		return copy(t, t->size);
	} else if (t == NULL) {
		return copy(s, s->size);
	} else {
		T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
		assert(s->cmp == t->cmp && s->hash == t->hash);
		{ //<for each member q in t>
			int i;
			struct member *q;
			for (i = 0; i < t->size; i++)
				for (q = t->buckets[i]; q; q = q->link)
					if(!Set_member(s, q->member)) // <add q->member to set>
						{
							struct member *p;
							const void *member = q->member;
							int i = (*set->hash)(member)%set->size;
							// <add member to set>
							NEW(p);
							p->member = member;
							p->link = set->buckets[i];
							set->buckets[i] = p;
							set->length++;
						}
		}
		{ T u = t; t = s; s = u; } //changes s and t so it can repeat
		{ //<for each member q in t>
			int i;
			struct member *q;
			for (i = 0; i < t->size; i++)
				for (q = t->buckets[i]; q; q = q->link)
					if(!Set_member(s, q->member)) // <add q->member to set>
						{
							struct member *p;
							const void *member = q->member;
							int i = (*set->hash)(member)%set->size;
							// <add member to set>
							NEW(p);
							p->member = member;
							p->link = set->buckets[i];
							set->buckets[i] = p;
							set->length++;
						}
		}
		return set;
	}
}


/**
 * Deallocates Set pointed by *set and assigns the null pointer. Set_free does not
 * deallocate the members
 * 
 * @param {T} set   Non-null Set pointer 
 */
void Set_free (T *set) {
	assert(set && *set);
	if ((*set)->length > 0) {
		int i;
		struct member *p, *q;
		for (i = 0; i < (*set)->size; i++)
			for (p = (*set)->buckets[i]; p; p = q) {
				q = p->link;
				FREE(p);
			}
	}
	FREE(*set);
}


T Set_inter (T s, T t) {
	if (s == NULL) {
		assert(t);
		return Set_new(t->size, t->cmp, t->hash);
	} else if (t == NULL) {
		return Set_new(s->size, s->cmp, s->hash);
	} else if (s->length < t->length) {
		return Set_inter(t, s);
	} else {
		T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
		assert(s->cmp == t->cmp && s->hash == t->hash);
		{ // <for each member q in t>
			int i;
			struct member *q;
			for (i = 0; i < t->size; i++)
				for (q = t->buckets[i]; q; q = q->link)
					if (!Set_member(s, q->member)) // <add q->member to set>
						{
							struct member *p;
							const void *member = q->member;
							int i = (*set->hash)(member)%set->size;
							// <add member to set>
							NEW(p);
							p->member = member;
							p->link = set->buckets[i];
							set->buckets[i] = p;
							set->length++;

						}

		}
		return set;
	}
}


/**
 * Returns Set's cardinality, or the number of members it contains.
 * 
 * @param  {T} set   Set
 * @return     Set's length
 */
int Set_length (T set) {
	assert(set);
	return set->length;
}

/**
 * Calls apply for each member of set. It passes the member and the client-specific pointer
 * cl to apply. It does not otherwise inspect cl.
 * 
 * @param {T} set   Set to map members
 * @param {void fn} apply   Fn to call on each member of set
 * @param {void *} cl   Client-specific pointer to pass to apply
 */
void Set_map (T set,
	void apply (const void *member, void *cl), void *cl) {
	int i;
	unsigned stamp;
	struct member *p;

	assert(set);
	assert(apply);
	stamp = set->timestamp;
	for (i = 0; i < set->size; i++)
		for (p = set->buckets[i]; p; p = p->link) {
			apply(p->member, cl);
			assert(set->timestamp == stamp); 
		}
}

/**
 * Returns 1 if member is in Set and 0 if it is not.
 * 
 * @param  {T} set   Set to test membership
 * @param  {const void *} member   Member to find in Set
 * @return        1 if member; 0 if not
 */
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


T Set_minus (T t, T s) {
	if (t == NULL) {
		assert(s);
		return Set_new(s->size, s->cmp, s->hash);
	} else if (s == NULL)
		return copy(t, t->size);
	else {
		T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
		assert(s->cmp == t->cmp && s->hash == t->hash);
		{	
			// <for each member q in t>
			int i;
			struct member *q;
			for (i = 0; i < t->size; i++)
				for (q = t->buckets[i]; q; q = q->link)
					if (!Set_member(s, q->member)) // <add q->member to set>
						{
							struct member *p;
							const void *member = q->member;
							int i = (*set->hash)(member)%set->size;
							// <add member to set>
							NEW(p);
							p->member = member;
							p->link = set->buckets[i];
							set->buckets[i] = p;
							set->length++;
						}

		}
		return set;
	}
}


/**
 * Allocates, initialize, and returns a new Set.
 * 
 * @param  {int} hint   Estimate of the number of members expected
 * @param  {int fn} cmp   Compare members function
 * @param  {unsigned fn} hash   Map onto unsigned integers members function
 * @return        New allocated Set
 */
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


/**
 * Adds member to set, unless is already there
 * 
 * @param {T} set   Set to add member
 * @param {void const *} member Pointer to const member to add to set
 */
void Set_put (T set, const void *member) {
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


/**
 * Removes member from set if set contains member, and returns the member removed.
 * Otherwise, Set_remove does nothing and returns null.
 * 
 * @param  {T} set   Set to remove member
 * @param  {const void *}  member   Member to remove from set
 * @return      Removed member pointer or null
 */
void *Set_remove (T set, const void *member) {
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


/**
 * Returns a pointer to an N+1 element array that holds the N elements of set in an
 * arbitrary order. The value of end, which is often the null pointer, is assigned
 * to the N+1st element of the array.
 * 
 * @param  {T} set   Set of elements to transform into array 
 * @param  {void *} end   Value to assign to last element of array
 * @return     Pointer to the first element to resulting array
 */
void **Set_toArray (T set, void *end) {
	int i, j = 0;
	void **array;
	struct member *p;

	assert(set);
	array = ALLOC((set->length + 1) * sizeof(*array));
	for (i = 0; i < set->size; i++)
		for (p = set->buckets[i]; p; p = p->link)
			array[j++] = (void *)p->member; // p->member must be cast from const void * to void *
											// because the array is not declared const

	array[j] = end;
	return array;
}


T Set_union (T s, T t) {
	if (s == NULL) {
		assert(t);
		return copy(t, t->size);
	} else if (t == NULL)
		return copy(s, s->size);
	else {
		T set = copy(s, Arith_max(s->size, t->size));
		assert(s->cmp == t->cmp && s->hash == t->hash);
		{  
			// <for each member q in t>
			int i;
			struct member *q;
			for (i = 0; i < t->size; i++)
				for (q = t->buckets[i]; q; q = q->link)
					Set_put(set, q->member);
		}
		return set;
	}
}

