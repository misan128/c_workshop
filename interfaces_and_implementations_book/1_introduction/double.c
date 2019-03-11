/* doubl: A program that detects adjacent identical words in its input,
 *	such as “the the.”
 */

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

// Defines
#define BUFF_SIZE 128

// Prototypes
int getword(FILE *, char *, int);
void doubleword(char *, FILE *);

// data
int linenum;
int word_rep;


// Main implementation
int main(int argc, char *argv[]) {
	int i;
	for (i = 1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL) {
			fprintf(stderr, "%s: can't open '%s' (%s)\n",
				argv[0], argv[i], strerror(errno));
			return EXIT_FAILURE;
		} else {
			doubleword(argv[i], fp);
			fclose(fp);
		}
	}
	if (argc == 1) doubleword(NULL, stdin);
	return EXIT_SUCCESS;
}

// get word fn
int getword(FILE *fp, char *buf, int size){
	int c;

	c = getc(fp);

	//<scan forward to a nonspace character or EOF 6>
	for ( ; c != EOF && isspace(c); c = getc(fp))
		if (c == '\n')
			linenum++;

	// <copy the word into buf[0..size-1] 7>
	{
		int i = 0;
		for ( ; c != EOF && !isspace(c); c = getc(fp)){
			if (i < size - 1){
				buf[i++] = tolower(c);
			}
		}
		if (i < size){
			buf[i] = '\0';
		}
	}

	if (c != EOF)
		ungetc(c, fp);

	// <found a word? 7>
	return buf[0] != '\0';
}

// duplicate word fn
void doubleword(char *name, FILE *fp){
	char prev[BUFF_SIZE], word[BUFF_SIZE];

	linenum = 1;
	word_rep = 0;
	prev[0] = '\0';
	while(getword(fp, word, sizeof word)){

		// <word is a duplicate 8≡>
		// Custom aproach
		if(isalpha(word[0])){
			if(strcmp(prev, word) == 0){
				word_rep++;
			}else if(word_rep > 0){
				if(name){
					printf("%s:", name);
				}
				printf("%d: %s (%d times rep)\n", linenum, prev, word_rep);
				word_rep = 0;
			}
		}
		// End custom aproach

		// Book code
		/*if(isalpha(word[0]) && strcmp(prev, word) == 0){
			{
				if (name){
					printf("%s:", name);
				}
				printf("%d: %s\n", linenum, word);
			}
		}*/
		// End book code
		strcpy(prev, word);
	}


	/* As we are evaluating only the last word to only print the duplicate word one
	 * time with it's number of ocurrences there's the case where the last word
	 * is a duplicate thus we eval if we have a repetition counter and if the prev
	 * word is an alphanumeric to print it
	 */
	if(word_rep > 0 && isalpha(prev[0])){
		if(name){
			printf("%s:", name);
		}
		printf("%d: %s (%d times rep)\n", linenum - 1, prev, word_rep);
		//word_rep = 0;
	}
}