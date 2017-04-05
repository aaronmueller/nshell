#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main(){
	char* thing = "this is a \"test\"";
	char* brk = strtok(thing, "\"");
	printf("%s", brk);
	return 0;
}
