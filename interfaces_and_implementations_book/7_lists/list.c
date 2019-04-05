#include <stdarg.h>
#include <stddef.h>
#include "assert.h"
#include "mem.h"
#include "list.h"

#define T List_T

// <funtions>

/**
 * Appends one list to another. Assigns to the last rest field in list
 * @param  {T} list List to append
 * @param  {T} tail List to be appended
 * @return     List with tail appended to list
 */
T List_append (T list, T tail){
	T *p = &list;

	while(*p)
		p = &(*p)->rest;
	*p = tail;

	return list;
}

/**
 * Makes and returns a copy of list
 * @param  {T} list   List to be copied
 * @return     Copy of list
 */
T List_copy (T list) {
	T head, *p = &head;

	for ( ; list; list = list->rest) {
		NEW(*p);
		(*p)->first = list->first;
		p = &(*p)->rest;
	}
	*p = NULL;

	return head;
}

/**
 * If list is nonnull deallocates all of the nodes on list and sets ir to the null
 * pointer
 * 
 * @param {T} list   Pointer to a List
 */
void List_free (T *list) {
	T next;

	assert(list);
	for ( ; *list; *list = next) {
		next = (*list)->rest;
		FREE(*list);
	}
}

/**
 * Creates and returns a list. It's called with N nonnull pointers followed by one
 * null pointer, and it creates a list with N nodes whose first fields hold the N
 * nonnull pointers and whose Nth rest field is null.
 * Any pointer different to void passed in the variable part of its argument needs
 * to be casted.
 *
 * @param {void *} x   Pointer to element to include in list
 * @param {va_arg} ... Variable argument pointers to be included in list
 */
T List_list (void *x, ...) {
	va_list ap;
	T list, *p = &list;

	va_start(ap, x);
	for ( ; x; x = va_arg(ap, void *)) {
		NEW(*p);
		(*p)->first = x;
		p = &(*p)->rest;
	}
	*p = NULL;
	va_end(ap);
	return list;
}

/**
 * Returns the number of nodes in list
 * @param  {T} list   List to count nodes
 * @return     Number of nodes in list
 */
int List_length(T list) {
	int n;

	for (n = 0; list; list = list->rest)
		n++
	return n;
}

/**
 * Calls the function pointed to by apply for every node in list. Clients can pass
 * an application-specific pointer, cl, to List_map and this pointer is passed
 * along to apply as its second argument.
 *
 * @param {T} list   List to map with apply
 * @param {function} apply   Funtion that takes a pointer to a pointer and an
 *                   		 application-specific pointer
 * @param {void *} cl   Application-specific pointer to pass to apply as second param	
 */
void List_map(T list, void apply(void **x, void *cl), void *cl) {
	assert(apply);
	for ( ; list; list = list->rest)
		apply(&list->first, cl);
}

/**
 * Removes the first node in a nonempty list and returns the new list, or returns
 * an empty list. If x is nonnull, *x is assigned the contents of the first field
 * of the first node
 * 
 * @param  {T} list   List to remove first node
 * @param  x    [description]
 * @return      [description]
 */
T List_pop (T list, void **x) {
	if (list) {
		T head = list->rest;
		if (x)
			*x = list->first;
		FREE(list);
		return head;
	} else
		return list;

}

/**
 * Adds a new node that holds x to the beginning of list, and returns the new list
 * 
 * @param  list List pointer to push x
 * @param  x    Value to push into list
 * @return      New list that holds x at the beginning
 */
T List_push (T list, void *x) {
	T p;

	NEW(p);
	p->first = x;
	p->rest = list;
	return p;
}

/**
 * Reverses the order of the nodes in its list argument and returns the resulting list
 * @param  {T} list   List to be reversed
 * @return     Reversed list
 */
T List_reverse (T list) {
	T head = NULL, next;

	for ( ; list; list = next) {
		next = list->rest;
		list->rest = head;
		head = list;
	}

	return head;
}

/**
 * Creates an array in which elements zero through N-1 hold the N values from the first
 * field of the list and the Nth element holds the value of end, which is often the null
 * pointer
 * 
 * @param  {T} 		list   	List to create array from
 * @param  {void *} end   	Pointer to the value of end
 * @return      	Pointer to the first element of the array created
 */
void **List_toArray(T list, void *end) {
	int i, n = List_length(list);
	void **array = ALLOC((n + 1) * sizeof(*array));

	for (i = 0; i < n; i++) {
		array[i] = list->first;
		list = list->rest;
	}
	array[i] = end;
	return array;
}