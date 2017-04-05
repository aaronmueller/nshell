# nsh-P4
CS485G-004: Shell Project
Authors: Aaron Mueller, Connor VanMeter

# DESCRIPTION
`nsh` is a project for CS485G-004; it is a shell environment wherein the user may run commands
(a list of which may be found below). This project was coded in C.

The code works as follows:
1. The user inputs a line of text. This is read into a C string.
2. Upon a new line, the line of text is tokenized by spaces or quotes using a state machine.
	- Three different states: BASE, WORD, STRING
	- BASE: check for extra white space before and after `token`
	- WORD: `token` was not preceded by "
	- STRING: `token` was preceded by "
3. The first token in the token list is used to determine which command to perform (see list below).
	- `do`, `back`, and `tovar` work using fork() and execv(). See `doCmd()` in `nsh.c`.
	- `prompt`, `dir`, `procs`, and `pwd` are handled within `main()` in `nsh.c`.
	- `set` checks for valid variable name before storing variable and value in two different arrays using the same index
	- `dshv` loops through set shell variables and prints them
	- `done` breaks the main loop upon an `Enter` press. `Ctrl-D` ends `nsh` immediately.

# LIST OF FILES
* nsh.c: contains all code for the shell project
* Makefile: file containing directives used with `make`
* workingProgram: a well-functioning version of the shell; Rafi's nsh
* README.txt: contains description of files, project, limitations, and special features
* testfile.txt: for testing the project. run with './nsh < testfile.txt'
* goodoutput.txt: the output of './workingProgram < testfile.txt'
* output.txt: the output of './nsh < testfile.txt'

# HOW TO RUN/EXIT THIS PROJECT
1. Use `make` to compile nsh.
2. Run `./nsh` to enter the shell.
3. When you are finished, type `done` into the shell or `ctrl-D` at prompt to exit.

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
* Built-in commands ignore additional tokens. The user is not warned.
* Control-D terminates on a blank line or when hit twice in a row (workingProgram does this).
* `back` prints a prompt on a new line, followed by the output of the command on that same line.
	- The next command is entered on a new line after that, meaning that it is entered after no prompt.
	- workingProgram does this.
* Manually-set prompts can't contain spaces, whereas the default prompt does contain spaces (workingProgram does this).
* The state machine has interesting quotes behavior.
	- "token" -> token
	- ""token"" -> token""
	- "t"oken" -> token"
	- to"k"en -> to"k"en
* `procs` does not remove finished processes from the display list, but it does mark them as finished.
	- The questions page on the course site states that this is acceptable.
