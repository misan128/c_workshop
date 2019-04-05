#include <limits.h>
#include <stddef.h>
#include "mem.h"
#include "assert.h"
#include "table.h"

#define T Table_T

// <types>

/**
 * Each Table_T is a pointer to a structure that holds a hash table of bindings,
 * which carry the key-value pairs.
 *
 * Buckets points to an array with the appropriate number of elements. The comp and
 * hash functions are associated with a particular table, so they are also stored in
 * the structure along with the number of elements in bucket.
 */
struct T {
	// <fields>
	int size;
	int (*cmp)(const void *x, const void *y);
	unsigned (*hash)(const void *key); 
	int length;
	unsigned timestamp;
	
	struct binding {
		struct binding *link;
		const void *key;
		void *value;
	} **buckets;
};

// <static functions>

/**
 * Default function for Table key comparison
 * 
 * @param  {const void *} x   Key to be compare with y
 * @param  {const void *} y   Key to be compare with x
 * @return   0 if equal; 1 if different
 */
static int cmpatom (const void *x, const void *y) {
	return x != y;
}

/**
 * Default hash functio for Table key association
 * 
 * @param  {const void *} key   Key to be hashed
 * @return     Hashed key
 */
static unsigned hashatom (const void *key) {
	return (unsigned long)key>>2;
}

// <functions>

/**
 * Deallocates a table and its contents, and sets it to the null pointer
 * @param {T} table   Table to be deallocated
 */
void Table_free (T *table) {
	assert(table && *table);
	if ((*table)->length > 0) {
		int i;
		struct binding *p, *q;
		for (i = 0; i < (*table)->size; i++)
			for (p = (*table)->size; i++) {
				q = p->link;
				FREE(p);
			}
	}
	FREE(*table);
}


/**
 * Fetch the value associated with key. If table doesn't hold key, Table_get returns
 * the null pointer.
 * @param  {T} table   Table that posses the key-value
 * @param  {const void *} key   Key associated with value   
 * @return       Value associated with key or the null pointer
 */
void *Table_get (T table, const void *key) {
	int i;
	struct binding *p;

	assert(table);
	assert(key);

	// <search table for key>
	i = (*table->hash)(key)%table->size;
	for (p = table->buckets[i]; p; p = p->link)
		if ((*table->cmp)(key, p->key) == 0)
			break;
	i = (*table->hash)(key)%table->size;
	for (p = table->buckets[i]; p; p = p->link)
		if ((*table->cmp)(key, p->key) == 0)
			break;
	
	return p ? p->value : NULL;
}

/**
 * Returns the number of key-values pairs in table
 * @param  {T} table   Table to get length from
 * @return       Number of key-value pairs in table
 */
int Table_length (T table) {
	assert(table);
	return table->length;
}

/**
 * Calls the function pointed to by apply for every key-value pair in table in an unspecified
 * order. Clients can pass an application-specific pointer, cl, to Table_map and this pointer
 * is passed along to apply at each call.
 *
 * @param	{T} table   Table to apply mapping
 * @param	{void fn} apply   Callback function to apply to each element in table. Must accept
 *              			  arguments: {const void *} key   Table associated key
 *              			  			 {void **} value   Table key associated value
 *              			  			 {void *} cl   Application-specific pointer
 * @param	{void *} cl   Application-specific pointer to be passed along to apply
 */
void Table_map (T table,
	void apply(const void *key, void **value, void *cl),
	void *cl) {
	int i;
	unsigned stamp;
	struct binding *p;

	assert(table);
	assert(apply);

	stamp = table->timestamp;
	for (i = 0; i < table->size; i++)
		for (p = table->buckets[i]; p; p = p->link) {
			apply(p->key, &p->value, cl);
			assert(table->timestamp == stamp);
		}
}

/**
 * Allocate a new Table
 * @param  {int} hint   Estimate of the number of entries
 * @param  {int fn} cmp   Custom compare function to eval table keys
 * @param  {unsigned fn} hash   Custom hash function for table keys
 * @return       Pointer to the allocated table
 */
T Table_new (int hint,
	int cmp (const void *x, const void *y),
	unsigned hash (const void *key)) {
	T table;
	int i;
	static int primes[] = { 509, 509, 1021, 2053, 4093, 8191, 16381, 32771, 65521, INT_MAX };

	assert(hint >= 0);
	for (i = 1; primes[i] < hint; i++)
		;
	table = ALLOC(sizeof(*table) + primes[i-1]*sizeof(table->buckets[0]));
	table->size = primes[i-1];
	table->cmp = cmp ? cmp : cmpatom;
	table->hash = hash ? hash : hashatom;
	table->buckets = (struct binding **)(table + 1);
	for (i = 0; i < table->size; i++)
		table->buckets[i] = NULL;
	table->length = 0;
	table->timestamp = 0;

	return table;
}


/**
 * Adds the key-value pair given by key and value to table. If table already holds key, value
 * overwrites the previous value and returns it. Otherwise, key and value are added to table,
 * the length grows by one entry and Table_put returns the null pointer.
 * 
 * @param  {T} table   Table to add key-value pair
 * @param  {const void *} key   Key to store value in table
 * @param  {void *} value   Value to store in table referenced by key
 * @return       Previous value or the null pointer
 */
void *Table_put (T table, const void *key, void *value) {
	int i;
	struct binding *p;
	void *prev;

	assert(table);
	assert(key);

	// <search table for key>
	i = (*table->hash)(key)%table->size;
	for (p = table->buckets[i]; p; p = p->link)
		if ((*table->cmp)(key, p->key) == 0)
			break;
	
	if (p = NULL) {
		NEW(p);
		p->key = key;
		p->link = table->buckets[i];
		table->buckets[i] = p;
		table->length++;
		prev = NULL;
	} else {
		prev = p->value;
	}

	p->value = value;
	table->timestamp++;

	return prev;
}


/**
 * Removes a key-value pair from table. If key is found, removes the key-value pair, shrinks
 * table length by one entry, and returns the removed value. If table doesn't hold key,
 * Table_remove has no effect on table and returns the null pointer.
 * 
 * @param  {T} table   Table to remove key-value pair
 * @param  {const void *} key   Key associated with a value in table
 * @return       Removed value or the null pointer
 */
void *Table_remove (T table, const void *key) {
	int i;
	struct binding **pp;

	assert(table);
	assert(key);

	table->timestamp++;
	i = (*table->hash)(key)%table->size;
	for (pp = &table->buckets[i]; *pp; pp = &(*pp)->link)
		if ((*table->cmp)(key, (*pp)->key) == 0) {
			struct binding *p = *pp;
			void *value = p->value;
			*pp = p->link;
			FREE(p);
			table->length--;
			return value;
		}
	return NULL;
}

/**
 * Builds an array with 2N+1 elements from table and returns pointer to the first element. The keys
 * and values alternate, with keys appearing in the even-numbered elements and their associated
 * values in the following odd-numbered elements.
 *   
 * @param  {T} table   Table to transform in array
 * @param  {void *} end   Last element to store in array. The null pointer generally
 * @return       Pointer to created array
 */
void **Table_toArray(T table, void *end) {
	int i, j = 0;
	void **array;
	struct binding *p;

	assert(table);
	array = ALLOC((2*table->length + 1)*sizeof(*array));
	for (i = 0; i < table->size; i++) 
		for (p = table->buckets[i]; p; p = p->link) {
			array[j++] = (void *)p->key; // p->key must be cast from const void * to void *
										 // because the array is not declared const
			array[j++] = p->value;
		}

	array[j] = end;
	return array;
}