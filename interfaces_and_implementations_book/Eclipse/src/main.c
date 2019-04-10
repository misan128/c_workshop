/*
 * main.c
 *
 *  Created on: 9 abr. 2019
 *      Author: mssandoval
 */


// <includes>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "atom.h"
#include "table.h"
#include "mem.h"
#include "getword.h"
#include "string.h"

// <prototypes>
int compare (const void *x, const void *y);
int first (int c);
int rest (int c);
void vfree (const void *, void **, void *);
void wf (char *, FILE *);

// <functions>

/**
 * Main function processes its arguments, which name files. Opens each file and
 * calls wf with the file pointer and the file name.
 *
 * @param  argc   Number of arguments
 * @param  argv   Argument values array
 * @return        Exit status code
 */
int main (int argc, char *argv[]){
	int i;

	for (i = 1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL) {
			fprintf(stderr, "%s: cant't open '%s' (%s)\n", argv[0], argv[i], strerror(errno));
			return EXIT_FAILURE;
		} else {
			wf(argv[i], fp);
			fclose(fp);
		}
	}
	if (argc == 1) wf(NULL, stdin);

	return EXIT_SUCCESS;
}

/**
 * Compare 2 strings in the way wf maps them and uses C's string lib function to compare them
 * @param  {const void *} x   String to be compared to y
 * @param  {const void *} y   String to be compared to x
 * @return   1 if equals; 0 otherwise
 */
int compare (const void *x, const void *y) {
	return strcmp(*(char **)x, *(char **)y);
}

/**
 * Tests if its argument is alphanumeric
 * @param  {int} c   character
 * @return       1 if alphanumeric; 0 if not
 */
int first (int c) {
	return isalpha(c);
}

/**
 * Tests if its argument is alphanumeric or "_"
 * @param  {int} c   character
 * @return       1 if alphanumeric or "_"; 0 if not
 */
int rest (int c) {
	return isalpha(c) || c == '_';
}

/**
 * Callback function to deallocate Table entries
 *
 * @param key   [description]
 * @param count [description]
 * @param cl    [description]
 */
void vfree (const void *key, void **count, void *cl) {
	FREE(*count);
}

/**
 * Uses a Table to store the words and their counts. Each word is folded to lowercase, converted to an
 * atom, and used as a key. Values are pointers, but wf needs to associate an integer count with each key.
 * It thus allocates space for a counter and stores a pointer to this space in the table.
 *
 * Note: As wf is called for each file-name argument, in order to save space, it should deallocate the table
 * 		 and the count before it returns.
 *
 * @param name [description]
 * @param fp   [description]
 */
void wf (char *name, FILE *fp) {
	Table_T table = Table_new(0, NULL, NULL);
	char buf[128];

	while (getword(fp, buf, sizeof buf, first, rest)) {
		const char *word;
		int i, *count;

		for (i = 0; buf[i] != '\0'; i++) {
			buf[i] = tolower(buf[i]);
		}

		word = Atom_string(buf);
		count = Table_get(table, word);

		if (count)
			(*count)++; // This expression is different to *count++, which would increment count
						// instead of the integer it points to.
		else {
			NEW(count);
			*count = 1;
			Table_put(table, word, count);
		}
	}

	if (name)
		printf("%s: \n", name);

	{
		// <print the words>
		int i;
		void **array = Table_toArray(table, NULL);
		qsort(array, Table_length(table), 2*sizeof(*array), compare);

		for (i = 0; array[i]; i += 2) {
			printf("%d\t%s\n", *(int *)array[i+1], (char *)array[i]);
		}
		FREE(array);
	}

	// <deallocates the entries and tables>
	Table_map(table, vfree, NULL);
	Table_free(&table);
}
