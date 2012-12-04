#include <stdio.h>

// Shell commands for this application
const shell_command_t shell_commands[] = {
    {"help", "Prints a list of commands", help},
    {"init", "Initializes this node for a pingtest.", init},
    {"ping", "Makes this node a pinging node", ping},
    {"stop", "Stops the current node's ping-action and prints a summary", ping},
    {NULL, NULL, NULL}
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
//TODO implement
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

	shell_t shell;
	shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

	shell_run(&shell);

	return 0;
	while (1)
		;
}
