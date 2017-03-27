#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXLEN 1024		//max no. of characters from a line of user input
#define MAXTOKENS 128	//max no. of tokens in a command

int main() {
	char line[MAXLEN];
	char* tokens[MAXTOKENS];

	while(1) {
		// read user input into `line`
		// tokenize user input (by spaces) into `tokens`
		//    - ensure that multiple spaces between tokens are handled correctly
		// read first token
		//    - set, prompt, dir, procs, done, do, back, tobvar
		//    - if none of the above, print error message to stderr
		
		//if (tokens[0] == "done") {
			break;
		//}
	}

	return 0;
}
