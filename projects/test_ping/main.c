#include <stdio.h>
#include <string.h>
#include <vtimer.h>
#include <thread.h>

#include <posix_io.h>
#include <shell.h>
#include <shell_commands.h>
#include <board_uart0.h>

// Prototypes
void help(char * arg);
void init(char * arg);
void ping(char * arg);
void stop(char * arg);

// Shell commands for this application
const shell_command_t shell_commands[] =
{ { "help", "Prints the help", help },
        { "init", "Initializes this node with an address and a channel.", init },
        { "ping", "Makes this node a pinging node", ping },
        { "stop", "Stops the current node's pings and prints a summary", stop },
        { NULL, NULL, NULL }
};

/**
 * Initializes this node with a given radio address and on a given channel or
 * on a default channel, if no channel is given. A radio address must be given
 * in any case.
 */
void init(char* arg) {
//TODO implement
}

/**
 * Prints the shellcommands that are usable in this application for reference.
 */
void help(char* unused) {
    printf("These are the usable commands:\n");
    printf("\thelp (commandname)\n");
    printf("\tinit [address] (channel)\n");
    printf("\tping [address] (time)\n");
    printf("\tstop\n");
    printf("\n");
    printf("[] = mandatory, () = optional");
}

/**
 * Sends out pings from this node to another node in a continuous manner, until
 * the stop command is used.
 */
void ping(char* unused) {
//TODO implement
}

/**
 * Stops any current pinging-action by this node and prints a summary of how
 * many pings got returned.
 * If the node was not pinging at the time of the method-call, this method does
 * nothing.
 */
void stop(char* unused) {
//TODO implement
}

/**
 * The main function for this application, starts the shell and waits for
 * commands.
 */
int main(void) {
    puts("Ping Test Application\n");
    puts("For commands type 'help'!\n");

    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

    shell_run(&shell);

    return 0;
}
