#include "stdio.h"
#include "ot.h"

#include "shell.h"
#include "shell_commands.h"
#include "openthread/thread.h"

int _state(int argc, char **argv)
{
    begin_mutex();
    int status;
    status = otThreadGetDeviceRole(ot_get_instance());
    printf("Status is: %i\n", status);
    end_mutex();
    return 0; 
}

static const shell_command_t shell_commands[] = {
    {"ot_state","Read OpenThread state",_state},
    {NULL, NULL, NULL}
};
int main(void)
{
#if defined(MODULE_OPENTHREAD_CLI) || defined(MODULE_OPENTHREAD_NCP)
    openthread_uart_run();
#else
    char line_buf[3 * SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
#endif

    return 0;
}
