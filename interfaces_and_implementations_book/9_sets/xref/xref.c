/**
 * xref prints cross-reference lists of the identifiers in its input files, which
 * helps, for example, to find all of the uses of specific identifiers in a program's
 * source files. For example:
 *
 * 		% xred xref.c getword.c
 * 	 	...
 * 	  	FILE 	getword.c: 6
 * 			 	xref.c: 18 43 72
 * 	    ...
 * 	    c 		getword.c: 7 8 9 10 11 16 19 22 27 34 35
 * 			  	xref.c: 141 142 144 147 148
 * 	    ...
 *
 * Says that FILE is used on line 6 in getword.c and on lines 18, 43, and 72 in xref.c
 * etcetera...
 *
 * If there are no program arguments, xref emits a cross-reference list of the identifiers
 * in the standard input, omiting the file names shown in the sample output above:
 * 
 * 		% cat xref.c getword.c | xref
 * 		...
 * 		FILE 18 43 72 157
 * 		...
 * 		c 141 142 144 147 148 158 159 160 161 162 167 170 173 178 185 186 ... 
 *
 * xref's implementation shows how sets and tables can be used together. It builds a table
 * indexed by identifiers in which each associated value is another table indexed by
 * file name. The values in this table are sets of pointers to integers, which holds the
 * line numbers. 
 */


//////////////
// includes //
//////////////

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "table.h"
#include "atom.h"
#include "set.h"
#include "mem.h"
#include "getword.h"


////////////////
// prototypes //
////////////////

int cmpint (const void *x, const void *y);
int compare (const void *x, const void *y);
int first (int c);
int intcmp (const void *x, const void *y);
unsigned inthash(const void *x);
void print (Table_T);
int rest (int c);
void xref (const char *, FILE *, Table_T);


//////////
// data //
//////////

int linenum;


//////////
// main //
//////////
///
int main (int argc, char *argv[]) {
	int i;
	Table_T identifiers = Table_new(0, NULL, NULL);

	for (i = 1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL) {
			fprintf(stderr, "%s: can't open '%s' (%s)\n", argv[0], argv[i], strerror(errno));
			return EXIT_FAILURE;
		} else {
			xref(argv[i], fp, identifiers);
			fclose(fp);
		}
	}
	if (argc == 1) xref(NULL, stdin, identifiers);

	// <print the identifiers>
	{
		int i;
		void **array = Table_toArray(identifiers, NULL);
		qsort(array, Table_length(identifiers), 2*sizeof(*array), compare);

		for (i = 0; array[i]; i += 2) {
			printf("%s", (char *)array[i]);
			print(array[i+1]);
		}
		FREE(array);
	}

	return EXIT_SUCCESS;
}


///////////////
// functions //
///////////////

int cmpint (const void *x, const void *y) {
	if (**(int **)x < **(int **)y)
		return -1;
	else if (**(int **)x > **(int **)y)
		return +1;
	else
		return 0;
}


int compare (const void *x, const void *y) {
	return strcmp(*(char **)x, *(char **)y);
}


int first (int c) {
	if (c == '\n')
		linenum++;

	return isalpha(c) || c == '_';
}


int intcmp (const void *x, const void *y) {
	return cmpint(&x, &y);
}


unsigned inthash (const void *x) {
	return *(int *)x;
}


void print (Table_T files) {
	int i;
	void **array = Table_toArray(files, NULL);

	qsort(array, Table_length(files), 2*sizeof(*array), compare);

	for (i = 0; array[i]; i += 2) {
		if (*(char *)array[i] != '\0')
			printf("\t%s:", (char *)array[i]);

		// <print the line numbers in the set array[i+1]>
		{
			int j;
			void **lines = Set_toArray(array[i+1], NULL);
			qsort(lines, Set_length(array[i+1]), sizeof(*lines), cmpint);

			for (j = 0; lines[j]; j++)
				printf(" %d", *(int *)lines[j]);

			FREE(lines);
		}

		printf("\n");
	}
	FREE(array);
}


int rest (int c) {
	return isalpha(c) || c == '_' || isdigit(c);
}


void xref (const char *name, FILE *fp, Table_T identifiers) {
	char buf[128];

	if (name == NULL)
		name = "";

	name = Atom_string(name);
	linenum = 1;

	while (getword(fp, buf, sizeof buf, first, rest)) {
		Set_T set;
		Table_T files;
		const char *id = Atom_string(buf);
		// <files <- file table identifiers associated with id>
		files = Table_get(identifiers, id);
		if (files == NULL) {
			files = Table_new(0, NULL, NULL);
			Table_put(identifiers, id, files);
		}
		
		// <set <- set in files associated with name>
		set = Table_get(files, name);
		if (set == NULL) {
			set = Set_new(0, intcmp, inthash);
			Table_put(files, name, set);
		}

		// <add linenum to set, if necessary>
		int *p = &linenum;
		if (!Set_member(set, p)) {
			NEW(p);
			*p = linenum;
			Set_put(set, p);
		}
	}
}






