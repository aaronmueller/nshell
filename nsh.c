#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "built_in.h"

#define MAXLEN 1024	// max # of chars from line of user input (flexible)
#define MAXTOKENS 128	// max # of tokens in cmd (flexible)
#define MAXPROMPT 256	// max length of prompt
#define MAXTOKENLEN 32	// max length of tokens (flexible)

int usrVarSize = 10;	// defult size of usr var array (flexible)
char **usrVarName;
char **usrVarValue;
int lastVar = 0; 	// index of last set variable value

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

int varIndex(char* token) {
	int i;
	for (i = 0; i < lastVar; ++i) {
		if (strcmp(token, usrVarName[i]) == 0)
			return i;
	};
	return -1;
}

// set variable value
void set(char* tokens[]) {
	// need to check for token #
	if ( !isalpha(tokens[0][1]) ) {
		perror("variable starts with a non-alphabet character");
		return;
	}
	usrVarName[lastVar] = tokens[0];
	usrVarValue[lastVar] = tokens[1];
	lastVar++;
}

int main() {
	char* user_prompt = malloc(MAXPROMPT);
 	usrVarName = malloc(usrVarSize * MAXTOKENLEN);
	usrVarValue = malloc(usrVarSize * MAXTOKENLEN);
	strncpy(user_prompt, "nsh > ", MAXPROMPT);
	char* line;
	char** tokens;	
	pid_t pid = 0;
	int i;

	// allocate usr variables
	for (i = 0; i < usrVarSize; ++i) {
		usrVarName[i] = (char*)malloc(MAXTOKENLEN+1);
		usrVarValue[i] = (char*)malloc(MAXTOKENLEN+1);
	}
	// set default PATH @ index 0
	usrVarName[lastVar] = "PATH";
	usrVarValue[lastVar] = "/bin:/usr/bin";
	lastVar++;

	while(1) {
		printf("%s", user_prompt);
		// read user input into `line`;
		// tokenize user input (by spaces) into `tokens`
		line = read_line();
		tokens = tokenize(line);

		// handle commands
		// - TODO: set, procs
		// - if invalid, print error message to stderr
		// - check and use parameters as appropriate
		
		// Program-control Commnds
		
		if (*tokens) {
			if (strcmp(tokens[0], "done") == 0) {
				break;
			}
	
			// do
			if (strcmp(tokens[0], "do") == 0) {
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
				if (!tokens[1]) {
					fprintf(stderr, "\'prompt\' usage: prompt <new_prompt>\n");
				}
				else {
					if (strlen(tokens[1]) > 255) {
						fprintf(stderr, 
								"warning: prompt too long. truncating to 256 characters.\n");
					}
					strncpy(user_prompt, tokens[1], MAXPROMPT);
				}
			}

			else if (strcmp(tokens[0], "dir") == 0) {
				dir(tokens);
			}

			else if (strcmp(tokens[0], "procs") == 0) {
				procs();
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
