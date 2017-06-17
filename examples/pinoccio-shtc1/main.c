
#include "shtc1.h"
#include "shell.h"

extern int shtc1_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
	{ "shtc", "read values from a shtc1 sensor", shtc1_cmd },
    { NULL, NULL, NULL }
};

int main (void)
{

	char line_buf[SHELL_DEFAULT_BUFSIZE];

	shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("Error: END of main().");
	/* should be never reached */
	return 0;
}
