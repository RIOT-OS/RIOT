#ifdef MODULE_OPENTHREAD_CLI
#include "ot-cli.h"
#else
#include <openthread.h>
#endif

#include "shell.h"
#include "shell_commands.h"
#include <stdio.h>

#ifndef MODULE_OPENTHREAD_CLI
int state_cmd(int argc, char **argv)
{
    switch(otGetDeviceRole())
    {
        case kDeviceRoleDisabled:
            printf("disabled\n");
            break;
        case kDeviceRoleDetached:
            printf("detached\n");
            break;
        case kDeviceRoleChild:
            printf("child\n");
            break;
        case kDeviceRoleRouter:
            printf("router\n");
            break;
        case kDeviceRoleLeader:
            printf("leader\n");
            break;
    }
    return 0;
}
static const shell_command_t shell_commands[] = {
        { "state", "get OpenThread state", state_cmd },
            { NULL, NULL, NULL }
};

void cli_run(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
#endif

int main(void)
{
#ifdef MODULE_OPENTHREAD_CLI
    openthread_cli_run();
#else
    cli_run();
#endif

	return 0;
}
