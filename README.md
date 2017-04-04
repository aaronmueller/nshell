# nsh-P4
CS485G: Shell Project
Authors: Aaron Mueller, Connor VanMeter

# LIST OF FILES
* nsh.c: contains all code for the shell project
	- nsh: executable shell binary (run this file)
* workingProgram: a well-functioning version of the shell; Rafi's nsh
* README.txt: contains description of files, project, limitations, and special features
* testfile.txt: for testing the project. run with 'nsh < testfile.txt'
* goodoutput.txt: the output of 'workingProgram < testfile.txt'

# HOW TO RUN/EXIT THIS PROJECT
1. Use `make` to compile nsh.
2. Run `./nsh` to enter the shell.
3. When you are finished, type `done` into the shell to exit.

# COMMANDS
* The nsh shell has the following commands:
* Built-in Commands
	- set variable value
	- prompt newPrompt
	- dir directoryName
	- procs
	- done
	- pwd (print working directory)
	- dshv (display shell variable)
* Built-in Commands ignore additional tokens.
* Program Commands
	- do
	- back
	- tovar
* If none of the above commands are given, an error message is displayed.

# OTHER FEATURES
* Shell Variables have split storage. Name are stored in usrVarName. Value are stored in usrVarValue.
* Index 0 is reserved for PATH. Index 1 is reserved for ShowTokens.
