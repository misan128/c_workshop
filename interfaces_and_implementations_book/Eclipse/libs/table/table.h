/**
 * An associative table is a set of key-value pairs. It's like an array except that the indices
 * can be values of any type. Many applications uses tables and tables have many uses.
 *
 * The Table interface is designed so that it can be used for many of these uses. Table represents
 * an associative table with an opaque pointer type.
 */

#ifndef TABLE_INCLUDED
#define TABLE_INCLUDED

#define T Table_T
typedef struct T *T;

// <exported functions>

extern T Table_new (int hint,
	int cmp (const void *x, const void *y),
	unsigned hash (const void *key));

extern void Table_free (T *table);

extern int 	 Table_length (T table);
extern void *Table_put (T table, const void *key, void *value);
extern void *Table_get (T table, const void *key);
extern void *Table_remove (T table, const void *key);

extern void  Table_map (T table,
	void apply (const void *key, void **value, void *cl),
	void *cl);
extern void **Table_toArray (T table, void *end);

#undef T
#endif
