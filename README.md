# nsh-P4
CS485G: Shell Project
Authors: Aaron Mueller, Connor VanMeter

# LIST OF FILES
* nsh.c: contains all code for the shell project
	- nsh: executable shell binary (run this file)
* Makefile: file containing directives used with `make`
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
	- set _variable value_: _set_ a _shell_ variable to _value_.
	- prompt _newPrompt_: sets the shell _prompt_ to _newPrompt_.
	- dir _directoryName_: change current directory to _directoryName_.
	- procs: list all processes running in the background.
	- done: nsh exits with exit status 0. nsh also accepts control-D.
	- pwd: print working directory.
	- dshv: display all shell variables.
* Built-in Commands ignore additional tokens.
* Program Commands
	- do _cmd param*_: execute _cmd_. By default, _cmd_ is searched in a list of directories indivated by PATH. If _cmd_ starts with /, it is a full path name sstarting at root. If _cmd_ starts with ./, it is a path name starting in the current directory.
	- back _cmd param*_: Same as do, except the program runs in the background.
	- tovar _variable cmd param*_: Same as do, except the output is assigned to _variable_.
* If none of the above commands are given, an error message is displayed.

# OTHER FEATURES
* Shell Variables have split storage. Name are stored in usrVarName. Value are stored in usrVarValue.
* Index 0 is reserved for PATH. Index 1 is reserved for ShowTokens.

# LIMITATIONS/BUGS
* Built-in Commands ignore additional tokens. The user is not warned.
* control-D termintes on a blank line or when hit twice in a row.
* `back` makes the next command print without a proper prompt.
* Manually set prompts can't contain spaces, when the default prompt does contain spaces.
