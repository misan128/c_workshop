#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "assert.h"
#include "getword.h"

int getword (FILE *fp, char *buf, int size,
	int first (int c), int rest (int c)) {

	int i = 0, c;

	assert(fp && buf && size > 1 && first && rest);
	c = getc(fp);
	for ( ; c != EOF; c = getc(fp)) {
		if (first(c)) {
			// <store c in buf if it fits>
			if (i < size - 1)
				buf[i++] = c;
			
			c = getc(fp);
			break;
		}
	}

	for ( ; c != EOF && rest(c); c = getc(fp)) {
		// <store c in buf if it fits>
		if (i < size - 1)
			buf[i++] = c;
	}

	if (i < size) {
		buf[i] = '\0';
	} else {
		buf[size-1] = '\0';
	}

	if (c != EOF) {
		ungetc(c, fp);
	}

	return i > 0;
}


int main (int argc, char *argv[]){
	int i;

	for (i = 1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL) {
			fprint(stderr, "%s: cant't open '%s' (%s)\n", argv[0], argv[i], strerror(errno));
			return EXIT_FAILURE;
		} else {
			wf(argv[i], fp);
			fclose(fp);
		}
	}
	if (argc == 1) wf(NULL, stdin);

	return EXIT_SUCCESS;
}