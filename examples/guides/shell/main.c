#include <stdio.h>
#include "shell.h"

int echo_command(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    return 0;
}

SHELL_COMMAND(echo,"Echo a message", echo_command);

int main(void)
{
    // Buffer to store command line input
    char buffer[SHELL_DEFAULT_BUFSIZE];

    // Start the shell
    shell_run(NULL, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
