#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXLEN 1024	//max # of chars from line of user input
#define MAXTOKENS 128	//max # of tokens in cmd

// PRE:
// POST:
void readAndTokenize(char line[], char* tokens[]) {
	// Read user input into `line`
	fgets(line, MAXLEN, stdin);

	// Tokenize input into `tokens`
	int i = 0;

	// Increment `i` until first non-space character is found
	for(; i < MAXLEN; i++) {
		if (line[i] == ' ') {
			continue;
		}
		else {
			break;
		}
	}
	
	// Go through input char-by-char, tokenize by space divisions
	// Handles multiple spaces between tokens and spaces after end of meaningful input
	int was_space = 0;	// 1 if previous char was a space, 0 otherwise
	int token_start = i;	// index in `line` of start of current token
	int token_no = 0;	// index in `tokens` of current token
	// What does this loop do?
	for (; i < MAXLEN; i++) {
		if (line[i] == ' ' && !was_space) {
			tokens[token_no] = (char*) malloc(i - token_start + 1);
			memcpy(tokens[token_no], (line+token_start), (i - token_start));
			was_space = 1;
			token_no++;
		}

		else if (line[i] == '\0' || line[i] == '\n'){
			if (!was_space) {
				tokens[token_no] = (char*) malloc(i - token_start + 1);
				memcpy(tokens[token_no], (line+token_start), (i - token_start));
			}
			break;
		} 

		else if (line[i] != ' ' && was_space) {
			token_start = i;
			was_space = 0;
		}
	}

	// print all tokens on separate lines (for debugging purposes)
	/*
	i = 0;
	while(tokens[i] != NULL){
		printf("%s\n", tokens[i]);
		i++;
	}
	*/

	return;
}

/*
void freeTokens(char* tokens[]) {

}
*/

int main() {
	char line[MAXLEN];
	char* tokens[MAXTOKENS] = {"\0"};	// initialize first entry in 
						// `tokens` to blank string
	// What does this loop do?
	while(strcmp(tokens[0], "done") != 0) {
		printf("> ");
		// read user input into `line`;
		// tokenize user input (by spaces) into `tokens`
		readAndTokenize(line, tokens);

		// handle commands
		// - TODO: set, prompt, dir, procs, back, tobvar
		// - if invalid, print error message to stderr
		// - check and use parameters as appropriate
		
		// handle `do`; prints two command prompts and does nothing as of now
		if (strcmp(tokens[0], "do") == 0) {
			pid_t pid;

			if ((pid = fork())) {
				// parent
				waitpid(pid, NULL, WNOHANG);
			} else {
				// child
				if (execv(tokens[1], tokens+1)) {
					perror(tokens[1]);
					exit(1);
				}
			} //esle
		} //fi
	} //elihw
	return 0;
}
