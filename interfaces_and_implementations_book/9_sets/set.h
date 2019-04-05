/**
 * A set is an unordered collection of distinct members. The basic operations on a set
 * are testing for membership, adding members, and removing members.
 *
 * Applications use sets much the way they use tables. Indeed, the sets provided by Set
 * are like tables: set members are the keys and the values associated with the keys are
 * ignored.
 */

#ifndef SET_INCLUDED
#define SET_INCLUDED

#define T Set_T
typedef struct T T*;

// <exported functions>

/////////////////////////////////
// Allocation and deallocation //
/////////////////////////////////
extern T    Set_new  (int hint,
	int cmp (const void *x, const void *y),
	unsigned hash (const void *x));
extern void Set_free (T *set);

//////////////////////////
// Basic set operations //
//////////////////////////
extern int 	 Set_length	(T set);
extern int 	 Set_member	(T set, const void *member);
extern void  Set_put 	(T set, const void *member);
extern void *Set_remove (T set, const void *member);

/////////////////////
// Set transversal //
/////////////////////
extern void   Set_map     (T set,
	void apply (const void *member, void *cl), void *cl);
extern void **Set_toArray (T set, void *end);

////////////////////
// Set operations //
////////////////////
extern T Set_union (T s, T t);
extern T Set_inter (T s, T t);
extern T Set_minus (T s, T t);
extern T Set_diff  (T s, T t);

#undef T
#endif