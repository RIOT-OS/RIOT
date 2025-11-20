---
title: Shell Commands
description: This tutorial explains how to use the RIOT shell.
code_folder: examples/guides/shell/
---

The shell is a powerful tool in RIOT that allows you to interact
with your application at runtime.
You can use the shell to inspect the state of your application,
change configuration parameters, and even run tests.

In this tutorial, we will take a look at how to use the shell in RIOT
and how to create custom shell commands.

## Step 1: Using the Shell

We need to include the shell module in our application to use the shell.
As we did in the previous tutorials, we will have to go into the `Makefile`
of our application and add the following line:

```make
USEMODULE += shell
```

This line tells the build system to include the shell module in our application.
Now we can use the shell in our application.

![The Makefile in Visual Studio Code](img/shell/01_makefile.png)

Now we can start the shell in our main function by calling the `shell_init` function.
Go into your `main.c` file and include the following code:

```c title="The include so we can use the shell"
#include "shell.h"
```

```c title="The main function"
int main(void)
{
    /* Buffer to store command line input */
    char buffer[SHELL_DEFAULT_BUFSIZE];

    /* Start the shell */
    shell_run(NULL, buffer, SHELL_DEFAULT_BUFSIZE);

    /* Return 0 to indicate that the program has finished successfully */
    return 0;
}
```

There are two importants things to note here:
First, we have to provide a buffer to the shell,
the shell will then use this buffer to store your input.
For this example,
we use the `SHELL_DEFAULT_BUFSIZE` macro to define the size of the buffer
but you can use any size you want (as long as it is big enough to store your input).

The second thing to note is that we pass `NULL` as the first argument to the `shell_run` function.
This argument is used to pass a list of shell commands to the shell,
but we will take a look at that in the next step.

Now we can build and run our program.
Compile the program and flash it to your board using `make flash`.

If you look into the terminal via `make term` you should see the shell prompt.
You can now type shell commands and interact with your application.
For example, you can type `help` to get a list of available commands
... which is not very exciting at the moment because we don't have any custom commands.

![The shell prompt](img/shell/02_shell.png)

## Step 2: Creating Custom Shell Commands

Creating custom shell commands is quite simple.
You just have to define a function that implements the command and register it with the shell.
RIOT provides a set of macros to make this process easier.

Let's create a simple shell command that echoes back the input.
Go into your `main.c` file and include the following code:

```c title="The include so we can print to the console"
#include <stdio.h>
```

```c title="The function that implements the echo command"
/* Our command we want to register to the shell */
int echo_command(int argc, char **argv)
{
    /* We take the arguments passed to the command */
    for (int i = 1; i < argc; i++) {
        /* We print each argument followed by a space */
        printf("%s ", argv[i]);
    }
    /* Finally, we print a newline character to end the line */
    printf("\n");

    /* Return 0 to indicate success */
    return 0;
}
```

This function takes two arguments:
`argc` and `argv`. `argc` is the number of arguments passed to the command
and `argv` is an array of strings containing the arguments.

Now all that is left is to register the command with the shell.
Go back to your `main.c` file and include the following code _outside_ of the `main` function:

```c title="Register the command with the shell"
/* This little macro registers the command so we can use it in the shell */
SHELL_COMMAND(echo, "Echo a message", echo_command);
```

This macro takes three arguments: the name of the command,
a short description of the command, and the function that implements the command.

And that's it! You have created a custom shell command.
Now you can build and run your program again and type
`echo hello world` in the shell to see the command in action.

![The echo command in action](img/shell/03_echo.png)

## Conclusion

In this tutorial, we have learned how to use the shell in RIOT
and how to create custom shell commands.

:::note
The source code for this tutorial can be found
[HERE](https://github.com/RIOT-OS/RIOT/tree/master/examples/guides/shell).

If your project is not working as expected,
you can compare your code with the code in this repository to see if you missed anything.
:::
