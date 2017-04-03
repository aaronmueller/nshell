# nsh-P4
CS485G: Shell Project
Authors: Aaron Mueller, Connor VanMeter

# ABOUT

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
