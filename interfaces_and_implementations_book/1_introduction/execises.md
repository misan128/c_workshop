#Exercises

1. getword increments linenum in `<scan forward to a nonspace or EOF 6>` but not after `<copy the word into buf[0..size-1] 7>` when a word ends at a new-line character. Explain why. What would happen if linenum were incremented in this case?

2. What does double print when it sees three or more identical words in its input? Change double to fix this “feature.”

3. Many experienced C programmers would include an explicit comparison in strcpy’s loop:

		char *strcpy(char *dst, const char *src) {
			char *s = dst;
			while ((*dst++ = *src++) != '\0')
			;
			return s;
		}

The explicit comparison makes it clear that the assignment isn’t a typographical error. C compilers and related tools, like Gimpel Software’s PC-Lint and LCLint (Evans 1996),  a warning when the result of an assignment is used as a conditional, because such usage is a common source of errors. If you have PCLint or LCLint, experiment with it on some  programs.

## Solution

1. Since getword will be "copying" the found word into the buffer passed and the condition loops until EOF or an space character, if we incremented linenum there, anytime a word is found the line number will be incremented and the printed word duplication line occurrence will be wrong.

2. Right now it will be printing couplw of word occurrences so it will print number_of_repetitions - 1 times.
What can be done is a counter of the word repetitions and only one print with this value

3. can't