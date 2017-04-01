#include <unistd.h>
#include <stdio.h>

void set(char* tokens[]) {}

void prompt(char* tokens[]) {}

void dir(char* tokens[]) {
	if (!tokens[1]) {
		fprintf(stderr, "\'dir\' usage: dir <directory>");
		return;
	}
	
	if (chdir(tokens[1]) != 0) {
		perror(tokens[1]);
	}
	return;
}

void procs(char* tokens[]) {}
