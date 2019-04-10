/**
 * An array is a homogeneous sequence of values in which the elements in the sequence
 * are associated one-to-one with indices in a continous range. Arrays in some form
 * appear as built-in data types in virtually all programming languages. In some
 * languages, like C, all arrays indices have the same lower bounds, and in other
 * languages, like Modula-3, each array, can have its own bounds. In C, all arrays
 * have indices that start at zero.
 *
 * Array sizes are specified at either compile time or runtime. The sizes of static
 * arrays are known at compile time. In C, for example, declared arrays must have sizes
 * known at compile time; that is, in the declaration
 * 
 * 		int a[n], n must be a constant expression
 *
 * A static array may be allocated at runtime; for example, local arrays are allocated
 * at runtime when the function in which they appear is called, but their sizes are known
 * at compile time.
 *
 * ArrayRep interface reveals the representation for dynamic arrays for those few clients
 * that need more efficient access to the array elements. Together, Array and ArrayRep
 * illustrate a two-level interface ot a layered interface.
 *
 * Array specifies a high-level view of the ADT at a lower level. The advantage of this
 * organization is that importing ArrayRep clearly identifies those clients that 
 * depend on the representation of dynamic arrays. Changes on the representation thus
 * affects only them, not the presumably much larger population of clients that import
 * Array.
 */

#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "array.h"
#include "arrayrep.h"
#include "mem.h"


#define T Array_T

///////////////
// functions //
///////////////

/**
 * Creates a new Array_T of length elements and copies the first length elements
 * that array holds. If length exceeds the number of elements in array, the
 * excess elements in the copy are initialized to zero.
 * 
 * @param  {T} array   Array_T array to be copied
 * @param  {int} length   Length of the new copied Array_T array
 * @return        Copied array of length elements
 */
T Array_copy (T array, int length) {
	T copy;

	assert(array);
	assert(length >= 0);

	copy = Array_new(length, array->size);
	if (copy->length >= array->length && array->length > 0)
		memcpy(copy->array, array->array, array->length*array->size);
	else if (array->length > copy->length && copy->length > 0)
		memcpy(copy->array, array->array, copy->length * array->size);

	return copy;
}


/**
 * Deallocates and clears *array
 * 
 * @param {T *} array   Pointer to Array_T array to deallocate
 */
void Array_free (T *array) {
	assert(array && *array);
	FREE((*array)->array);
	FREE(*array);
}


/**
 * Returns a pointer to element number i in array
 * @param  {T} array   Array_T array
 * @param  {int} i   Index of element i in array
 * @return       Pointer to ith elementof array
 */
void *Array_get (T array, int i) {
	assert(array);
	assert(i >= 0 && i < array->length);
	return array->array + i*array->size;
}


/**
 * Returns the max number of elements in array
 * 
 * @param  {T} array   Array_T array
 * @return       Length of array
 */
int Array_length (T array) {
	assert(array);
	return array->length;
}


/**
 * Allocates, initializes, and returns a new array of length elements with bounds
 * zero through length-1, unless length is zero, in which case the array has no
 * elements. Each element occupies size bytes. The bytes in each element are
 * initialized to zero.
 * 
 * @param  {int} length   Max number of elements expected in array
 * @param  {int} size   Size in bytes of each element to be stored in array
 * @return        Array of length elements of size bytes each
 */
T Array_new (int length, int size) {
	T array;

	NEW(array);
	if (length > 0)
		ArrayRep_init(array, length, size, CALLOC(length, size));
	else
		ArrayRep_init(array, length, size, NULL);

	return array;
}


/**
 * Overwrites the value of element i with the new element pointed to by elem
 * 
 * @param  {T} array   Array_T array
 * @param  {int} i   Index to store elem in array
 * @param  {void *} elem   Pointer to element to store in array
 * @return       Pointer of elem
 */
void *Array_put (T array, int i, void *elem) {
	assert(array);
	assert(i >= 0 && i < array->length);
	assert(elem);
	memcpy(array->array + i*array->size, elem, array->size);

	return elem;
}


/**
 * Changes the size of array so that it hold length elements, expanding or
 * contracting it as necessary. If length exceeds the current length of the
 * array, the new elements are initialized to zero. Calling Array_resize
 * invalidates any values returned by previous calls to Array_get
 * 
 * @param {T} array   Array_T array
 * @param {int} length   New length of array 
 */
void Array_resize (T array, int length) {
	assert(array);
	assert(length >= 0);
	if (length == 0)
		FREE(array->array);
	else if (array->length == 0)
		array->array = ALLOC(length * array->size);
	else
		RESIZE(array->array, length * array->size);

	array->length = length;
}

/**
 * Returns the size of each element that can be stored in array
 * 
 * @param  {T} array   Array_T array
 * @return       Size of elements of array
 */
int Array_size (T array) {
	assert(array);
	return array->size;
}


/**
 * Initializes the fields in the Array_T structure pointed to by array to the
 * values of the arguments length, size, and ary.
 * 
 * @param {T} array   Pointer to Array_T structure
 * @param {int} length   Length of elements in array
 * @param {int} size   Size of each element of array
 * @param {void *} ary   Pointer to allocated space to store length elements of size bytes 
 */
void ArrayRep_init (T array, int length, int size, void *ary) {
	assert(array);
	assert(ary && length>0 || lenght==0 && ary==NULL);
	assert(size > 0);
	array->length = length;
	array->size = size;

	if (length > 0)
		array->array = ary;
	else
		array->array = NULL;
}
