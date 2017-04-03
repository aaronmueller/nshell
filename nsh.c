#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "built_in.h"

#define MAXLEN 1024		//max # of chars from line of user input (flexible)
#define MAXTOKENS 128	//max # of tokens in cmd (flexible)

char* read_line() {
	int pos = 0;
	int size = MAXLEN;
	char *buf = malloc(size * sizeof(char));
	char c;

	if (!buf) {
		fprintf(stderr, "nsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1) {
		c = getchar();
		if (c == EOF || c == '\n' || c == '%') {
			buf[pos] = '\0';
			return buf;
		}
		else {
			buf[pos] = c;
		}
		pos++;

		if (pos >= size) {
			size += MAXLEN;
			buf = realloc(buf, size * sizeof(char));
			if (!buf) {
				fprintf(stderr, "nsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

char** tokenize(char* line) {
	int size = MAXTOKENS;
	int pos = 0;
	char** tokens = malloc(MAXTOKENS * sizeof(char*));
	char* token;

	if (!tokens) {
		fprintf(stderr, "nsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, " \t\r\n\a");
	while (token != NULL) {
		tokens[pos] = token;
		pos++;

		if (pos >= size) {
			size += MAXTOKENS;
			tokens = realloc(tokens, size * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "nsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, " \t\r\n\a");
	}

	tokens[pos] = NULL;
	return tokens;
}

int main() {
	char* user_prompt = "nsh > ";
	char* line;
	char** tokens;	// initialize first entry in 
					// `tokens` to blank string
	
	pid_t pid = 0;
	while(1) {
		printf("%s", user_prompt);
		// read user input into `line`;
		// tokenize user input (by spaces) into `tokens`
		line = read_line();
		tokens = tokenize(line);

		// handle commands
		// - TODO: set, prompt, procs, back, tovar
		// - if invalid, print error message to stderr
		// - check and use parameters as appropriate
		
		// Program-control Commnds
		// SUG: Change to SWITCH statement
		// SUG: Make statement flexible; no need for 3
		
		if (*tokens) {
			if (strcmp(tokens[0], "done") == 0) {
				break;
			}
	
			// do
			if (strcmp(tokens[0], "do") == 0) {
				if ((pid = fork())) {
					// parent
					// Why is WNOHANG used here?
					// Do we want child exit status? If yes, why NULL?
					waitpid(pid, NULL, WNOHANG);
				} else {
					// child
					if (execv(tokens[1], tokens+1)) {
						perror(tokens[1]);
						exit(1);
					}
				}
			}

			// back
			else if (strcmp(tokens[0], "back") == 0) {
				if (!(pid = fork())) {
					// child
					if (execv(tokens[1], tokens+1)) {
						perror(tokens[1]);
						exit(1);
					}
				} else {
					// parent
					waitpid(pid, NULL, WNOHANG);
				} 
			}

			// tovar
			else if (strcmp(tokens[0], "tovar") == 0) {
				if ((pid = fork())) {
					// parent
					waitpid(pid, NULL, WNOHANG);
				} else {
					// child
					if (execv(tokens[1], tokens+1)) {
						perror(tokens[1]);
						exit(1);
					}
				}
			}

			else if (strcmp(tokens[0], "set") == 0) {
				set(tokens);
			}

			else if (strcmp(tokens[0], "prompt") == 0) {
				prompt(tokens, &user_prompt);
			}

			else if (strcmp(tokens[0], "dir") == 0) {
				dir(tokens);
			}

			else if (strcmp(tokens[0], "procs") == 0) {
				procs(tokens);
			}
			
			else {
				fprintf(stderr, "invalid command: %s\n", tokens[0]);
			}

			free(line);
			free(tokens);
		} else {
			continue;
		}
		
	} 

	return 0;
}
