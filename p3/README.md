
# Overview



First thing the shell needs to take different execution options. So 
checking execution arguments is needed. This will also determines
how the shell is run. Handled in `main()`

Secondly the shell should be able to print to and get line in 
terminal or a file, depending on the mode of execution, in a loop 
unless specifically asked to exit. Only the first 512 characters in 
a line are received at each given time, and the shell functionally
ignore the rest, meaning anything after 512 is purposefully ignored.
`main()` takes care of the io loop.

Getting the line allows it to be parsed into a serie of command
arguments tokens, with the first argument the main argument. This is 
under the presumption that every line has one command to execute; 
else we never know what to put into the first parameter of 
`execvp()`. Also new line characters are removed from the last 
token, and last token only since by definition a new line character 
leads to a new line. If the last token is a new line character the
last token is removed. It could lead to no token if the only 
thing in a line is a new line character which should be checked. 
`parse_line()` takes the line and parse it into `char **cmd`, as 
well as `redirect_fd` for redirection file.

Redirection character are parsed as a single token, and once they 
are detected the tokens after it, if there is any, are checked 
for both proper usage and file validity in `parse_line()`. 
And before executing anything with`execvp()` if the `redirect_fd`
is checked to be proper `dup2()` is used to set output file
descriptor from `stdout` to file pointed by `redirect_fd`.


After we get all the command arg tokens, we check if they are
built in commands. If they are we pipe all of them into `execvp()`, 
after `fork()` so only the child process executes them. The parent 
loop `wait()`for the child to finish so it doesn't accidentally 
print anything until it can do so. `execvp()` also checks if a 
command is found.If the commands are built in commands they 
don't go to `execvp()` and are  detected and sorted to different
system calls using `sort_built_in()`.

Aliases has to be stored as shell variables using built in 
command `alias`. The function called after `alias` is parsed
is `func_alias`, and should parsed the tokenized args after
the main alias arg to determine what to do with the program
variables. All commands that are not built ins are also parsed
to see if their main arg is an alias before being piped to 
`execvp()`. If they are the args, including the main command arg,
is replaced with the replacement command arg stored.

`func_unset()` and `func_export()` are called depending on which
command is passed to shell,`export()` or `unset()`. They are acting
as wrappers for the `getenv()` , `setenv()`and `unsetenv()` system
call functions. Also now non-built-in commands have to be parsed
if they have environment variables as arguments. If they are the
enviroment variable arguments are swapped with their value before
piping them into `execvp()`.












