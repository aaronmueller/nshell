#include <unistd.h>
#include <stdio.h>

void set(char* tokens[]) {}

void prompt(char* tokens[], char** user_prompt) {
	if (!tokens[1]) {
		fprintf(stderr, "\'prompt\' usage: prompt <new_prompt>\n");
		return;
	}

	*user_prompt = tokens[1];
}

// use `chdir()` to change directory; handle errors here
void dir(char* tokens[]) {
	// if no argument, print error message to stderr
	if (!tokens[1]) {
		fprintf(stderr, "\'dir\' usage: dir <directory>\n");
		return;
	}
	
	// if chdir does not return 0, print error message using perror()
	if (chdir(tokens[1]) != 0) {
		perror(tokens[1]);
	}
	return;
}

void procs(char* tokens[]) {}
