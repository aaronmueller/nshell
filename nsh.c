#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXLEN 1024		//max no. of characters from a line of user input
#define MAXTOKENS 128	//max no. of tokens in a command

void readAndTokenize(char line[], char* tokens[]) {
	//read user input into `line`
	fgets(line, MAXLEN, stdin);

	//tokenize input into `tokens`
	int i = 0;

	//increment `i` until first non-space character is found
	for(; i < MAXLEN; i++) {
		if (line[i] == ' ') {
			continue;
		}
		else {
			break;
		}
	}
	
	//go through input char-by-char, tokenize by space divisions
	//handles multiple spaces between tokens and spaces after end of meaningful input
	int was_space = 0;		// 1 if previous char was a space, 0 otherwise
	int token_start = i;	// index in `line` of start of current token
	int token_no = 0;		// index in `tokens` of current token
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

	while(strcmp(tokens[0], "done") != 0) {
		printf("> ");
		// read user input into `line`;
		// tokenize user input (by spaces) into `tokens`
		readAndTokenize(line, tokens);

		// handle command
		//	  - check first command
		//		# set, prompt, dir, procs, done, do, back, tobvar
		//		# if none of the above, print error message to stderr
		//	  - check and use parameters as appropriate
		
	}

	return 0;
}
