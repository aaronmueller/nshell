#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXLEN 1024	// max # of chars from line of user input (flexible)
#define MAXTOKENS 128	// max # of tokens in cmd (flexible)
#define MAXPROMPT 256	// max length of prompt
#define MAXTOKENLEN 256	// max length of tokens 
#define MAXPROCS 2048	// max # of processes

int usrVarSize = 10;	// defult size of usr var array (flexible)
char **usrVarName;
char **usrVarValue;
int sizeVar = 0; 	// index of last set variable value
int *processes;
int numProcs;
int procsSize = MAXPROCS;
int status;

char* read_line() {
	int pos = 0;
	int skip = 0;
	int size = MAXLEN;
	char *buf = malloc(size * sizeof(char));
	char c;

	if (!buf) {
		fprintf(stderr, "nsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1) {
		c = getchar();

		//if ctrl-d, quit immediately
		if (c == '\377') {
			strcpy(buf, "done\n");
			return buf;
		}
		//end buffer at newline
		else if (c == '\n') {
			buf[pos] = '\0';
			return buf;
		}
		//ignore everything after comment marker (%); stop reading
		else if(c == '%'){
			buf[pos] = '\0';
			skip = 1;
		}
		//read character as normal; keep going
		else if (!skip) {
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

// Returns index of token in usrVarName. Returns -1 if not found.
int varIndex(char* token) {
	int index;
	for (index = 0; index < sizeVar; ++index) {
		if (strcmp(token, usrVarName[index]) == 0)
			return index;
	}
	return -1;
}

// tokenizes user input. tokens can be a single word or a string
// surrounded by double quotes. Extra spaces are discounted.
char** tokenize(char* line) {
	int size = MAXTOKENS;
	int pos = 0, c;
	char** tokens = malloc(MAXTOKENS * sizeof(char*));
	char *token, *token_start;
	enum states { BASE, WORD, STRING} state = BASE;
	
	// State machine for parsing tokens
	for(token = line; *token != '\0'; token++) {
		if (pos >= size) {
			size += MAXTOKENS;
			tokens = realloc(tokens, size * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "nsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		c = (unsigned char) *token;

		// Different states
		switch (state) {
			// Base Case (whitespace)
			case BASE:
				if (isspace(c)) {
					continue;
				}
				if (c == '"') {
					state = STRING;
					token_start = token+1;
					continue;
				}
				state = WORD;
				token_start = token;
				continue;
			// TOKEN has no "
			case WORD:
				if (isspace(c)) {
					*token = 0;
					tokens[pos++] = token_start;
					state = BASE;
				}
				continue;
			// TOKEN has "
			case STRING:	
				if (c == '"') {
					*token = 0;
					tokens[pos++] = token_start;
					state = BASE;
				}
				continue;
		}
	}

	// Allocate more space if needed
	if (pos >= size) {
		size += MAXTOKENS;
		tokens = realloc(tokens, size * sizeof(char*));
		if (!tokens) {
			fprintf(stderr, "nsh: allocation error\n");
			exit(EXIT_FAILURE);
		}
	}
	// Check for last token
	if (state != BASE)
		tokens[pos++] = token_start;
	tokens[pos] = NULL;

	return tokens;
}

// set variable value
void set(char** tokens) {
	int i = 0;
	// Check for proper format
	if ( !tokens[0] || !tokens[1]) {
		fprintf(stderr, "usage: set <variable> <value>\n");
		return;
	}

	// Check if token starts with a non-alphabet character
	if ( !isalpha(tokens[0][0]) ) {
		fprintf(stderr, "variable must start with an alphabet character\n");
		return;
	}
	
	// Check if variable contains non-alphanumeric character
	while ( isalnum(tokens[0][i] )) {i++;};
	if(i != strlen(tokens[0])) {
		fprintf(stderr, "variable contains non-alphanumeric character\n");
		return;
	}

	if (tokens[0] )

	// Check if more space is needed
	if (usrVarSize < sizeVar) {
		usrVarSize += 10;
		usrVarName = realloc(usrVarName, usrVarSize * MAXTOKENLEN);
		usrVarValue = realloc(usrVarValue, usrVarSize * MAXTOKENLEN);	
	}

	// Find index of var name (if it exists)
	int index = varIndex(tokens[0]);
	// Truncate var name/value if > MAXTOKENLEN
	if (sizeof(tokens[0]) > MAXTOKENLEN) {
		tokens[0][MAXTOKENLEN-1] = '\0';
		printf("Truncated variable to: %s", tokens[0]);
	}
	if (sizeof(tokens[1]) > MAXTOKENLEN) {
		tokens[1][MAXTOKENLEN-1] = '\0';
		printf("Truncated value to: %s", tokens[1]);
	}
	// Increment size if var name is new
	if (index == -1) {
		index = sizeVar;
		sizeVar++;
	}

	// Set var name and value
	strcpy(usrVarName[index], tokens[0]);
	strcpy(usrVarValue[index], tokens[1]);
	
	// Display results of set
	printf("%d: %s = %s\n", index, usrVarName[index], usrVarValue[index]);
}

// Prints a list of set shell variables
void displayShellVariables(){
	int index;
	for (index = 0; index < sizeVar; ++index)
		printf("%d: %s = %s\n", index, usrVarName[index], usrVarValue[index]);
}

// Program Command function (do, back, tovar)
// do: run in foreground
// back: run in background
// tovar: store output in variable
void doCmd(char** tokens, int type) {
	pid_t pid;
	char* buf = malloc(MAXLEN);
	char* tovarbuf = malloc(MAXLEN);
	int pipefd[2];
	int chld_status;

	// Set up pipe for tovar
	if (type == 2) {
		strcpy(buf, (tokens-1)[0]);
		strcat(buf, " ");
		if (pipe(pipefd) == -1) {
			perror("Problem with pipe.");
			exit(EXIT_FAILURE);
		}
	}

	if ((pid = fork())) {
		// parent
		if (type != 1) {
			waitpid(pid, &chld_status, 0);
		}
		else {
			processes[numProcs] = (int) pid;
			numProcs++;
			if (numProcs >= procsSize) {
				procsSize += MAXPROCS;
				processes = realloc(processes, procsSize * sizeof(int));
				if (!processes) {
					fprintf(stderr, "nsh: allocation error\n");
					exit(EXIT_FAILURE);
				}
			}
		}
		if (type == 2) {
			close(pipefd[1]); // close write
			while( read(pipefd[0], tovarbuf, MAXLEN) ){};
			strcat(buf, tovarbuf);
			set(tokenize(buf));
			free(buf);
		}
	} else {
		// child
		if (type == 2) {
			close(pipefd[0]); //close unused read
			dup2(pipefd[1], 1); //stdout to pipe
			dup2(pipefd[1], 2); //stderr to pipe
			close(pipefd[1]); //close write
		}
		// path starts with '/' (from root)
		if (tokens[0][0] == '/') {
			if (execv(tokens[0], tokens)) {
				perror(tokens[0]);
				exit(EXIT_FAILURE);
			}
		}
		// path starts with './' (from current dir)
		else if (tokens[0][0] == '.' && tokens[0][1] == '/') {
			getcwd(buf, 100);
			strcat(buf, tokens[0]+1);
			if (execv(buf, tokens)) {
				perror(tokens[0]);
				exit(EXIT_FAILURE);
			}
		}
		// path is $PATH
		else {
			int path_worked = 0;
			char* path;
			path = strtok(usrVarValue[0], ":");
			// loop through all paths, separated by colons
			while (path != NULL) {
				strcpy(buf, path);
				// append path-final '/' if not already existent
				if (path[strlen(path) - 1] != '/') {
					strcat(buf, "/");
				}
				strcat(buf, tokens[0]);
				if (!execv(buf, tokens)) {
					path_worked = 1;
					break;
				}

				path = strtok(NULL, ":");
			}
			// cmd not found
			if (!path_worked) {
				perror(tokens[0]);
				exit(EXIT_FAILURE);
			}

			free(path);
		} // else
	} // else
} // doCmd

int main() {
	char* user_prompt = malloc(MAXPROMPT);
 	usrVarName = malloc(usrVarSize * MAXTOKENLEN);
	usrVarValue = malloc(usrVarSize * MAXTOKENLEN);
	strncpy(user_prompt, "nsh > ", MAXPROMPT);
	char* line;
	char** tokens;	
	int i, j, m;
	processes = malloc(procsSize * sizeof(int));

	// allocate usr variables
	for (i = 0; i < usrVarSize; ++i) {
		usrVarName[i] = (char*)malloc(MAXTOKENLEN+1);
		usrVarValue[i] = (char*)malloc(MAXTOKENLEN+1);
	}
	// set default PATH @ index 0
	strcpy(usrVarName[sizeVar], "PATH");
	strcpy(usrVarValue[sizeVar], "/bin:/usr/bin");
	sizeVar++;
	// set ShowTokens to 0 @ index 1
	strcpy(usrVarName[sizeVar],"ShowTokens");
	strcpy(usrVarValue[sizeVar], "0");
	sizeVar++;

	while(1) {
		printf("%s", user_prompt);
		// read user input into `line`;
		// tokenize user input (by spaces) into `tokens`
		line = read_line();
		tokens = tokenize(line);
		
		//check for variables and perform substitutions
		for (j = 1; tokens[j] != NULL; j++) {
			// increment until first non-quote character
			int k = 0;
			while (tokens[j][k] == '\'' || tokens[j][k] == '\"') {
				k++;
			}
			if (tokens[j][k] == '$' && tokens[j][k+1] != '\0') {
				char* str = malloc(MAXTOKENLEN+1);
				strcpy(str, tokens[j]+(k+1));
				char* to_cat = malloc(MAXTOKENLEN+1);
				while (str[strlen(str) - 1] == '\"' || str[strlen(str) - 1] == '\'') {
					strcat(to_cat, str+(strlen(str) - 1));
					str[strlen(str) - 1] = '\0';
				}
				for (m = 0; m < sizeVar; m++) {
					if (strcmp(str, usrVarName[m]) == 0) {
						strcpy(tokens[j]+k, usrVarValue[m]);
						strcat(tokens[j], to_cat);
					}
				}
				free(str);
				free(to_cat);
			}
		}

		// handle commands
		if (*tokens) {
			if (strcmp(tokens[0], "done") == 0) {
				break;
			}
			
			// show tokens
			if (strcmp(usrVarValue[1], "1") == 0) {
				i = 0;
				while (tokens[i]) {
				printf("Token = %s\n", tokens[i]);
				i++;
				}	
			}

			// do
			if (strcmp(tokens[0], "do") == 0) {
				doCmd(tokens+1, 0);
			}

			// back
			else if (strcmp(tokens[0], "back") == 0) {
				doCmd(tokens+1, 1);
			}

			// tovar
			else if (strcmp(tokens[0], "tovar") == 0) {
				doCmd(tokens+2, 2);
			}
			
			// set
			else if (strcmp(tokens[0], "set") == 0) {
				set(tokens+1);
			}

			// prompt
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
			} // if

			// dir
			else if (strcmp(tokens[0], "dir") == 0) {
				// if no argument, print error message to stderr
				if (!tokens[1]) {
					fprintf(stderr, "\'dir\' usage: dir <directory>\n");
				}
				
				// if chdir does not return 0, print error message using perror()
				else if (chdir(tokens[1]) != 0) {
					perror(tokens[1]);
				}
			}

			// procs
			else if (strcmp(tokens[0], "procs") == 0) {
				if (numProcs == 0) {
					printf("No background processes.\n");
				}
				else {
					printf("Background processes: \n");
					for (i = 0; i < numProcs; i++) {
						printf("\t%i", processes[i]);
						status = waitpid(processes[i], &status, WNOHANG);
						if (status){
							printf(" (finished)");
						}
						printf("\n");
					}
				}
			}
		
			// pwd
			else if (strcmp(tokens[0], "pwd") == 0) {
				getcwd(line, 100);
				printf("%s \n", line);
			}

			// dshv (display shell vars)
			else if (strcmp(tokens[0], "dshv") == 0) {
				displayShellVariables();
			}
	
			else {
				fprintf(stderr, "invalid command: %s\n", tokens[0]);
			}
			
			free(line);
			free(tokens);
		} else {
			continue;
		}
		
	} // while

	return 0;
} //main
