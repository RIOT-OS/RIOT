#include "stdio.h"
#include "ot.h"

#include "shell.h"
#include "shell_commands.h"
#include "openthread/thread.h"
#include "openthread/ip6.h"

void _send_cmd(ot_command_t *cmd, int type)
{
    msg_t msg, reply;
    msg.type = type;
    msg.content.ptr=cmd;
    msg_send_receive(&msg, &reply, openthread_get_pid());
}
int _ifconfig(int argc, char **argv)
{
    ot_command_t cmd;
    if(argc >= 2)
    {
	    printf("here");
	if(strcmp(argv[1], "get") == 0 && argc >= 3)
	{
		printf("and here");
            if(strcmp(argv[2], "panid") == 0)
	    {
		cmd.command = OT_CMD_PANID;
		_send_cmd(&cmd, OPENTHREAD_CMD_GET_MSG_TYPE_EVENT);
		printf("Panid: %04x", cmd.content.panid);
	    }	    
	}
	else if(strcmp(argv[1], "set") == 0 && argc >= 4)
	{
            if(strcmp(argv[2], "panid") == 0)
	    {
		cmd.command = OT_CMD_PANID;
		cmd.content.panid = strtol(argv[3], NULL, 0);
		_send_cmd(&cmd, OPENTHREAD_CMD_SET_MSG_TYPE_EVENT);
		printf("PANID: set to %02x\n", cmd.content.panid);
	    }	    
	
	}
	else if(strcmp(argv[1], "up") == 0)
	{
	    cmd.command = OT_CMD_IF;
	    cmd.content.enable = true;
	    _send_cmd(&cmd, OPENTHREAD_CMD_SET_MSG_TYPE_EVENT);
	    printf("IPv6 enabled\n");
	}
	else if(strcmp(argv[1], "thread") == 0)
	{
	    if(strcmp(argv[2], "start") == 0)
	    {
		printf("Starting OpenThread\n");
	        cmd.command = OT_CMD_THREAD;
	        cmd.content.enable = true;
	        _send_cmd(&cmd, OPENTHREAD_CMD_SET_MSG_TYPE_EVENT);
	    }
	    else if(strcmp(argv[2], "state") == 0)
            {
	        cmd.command = OT_CMD_STATE;
	        _send_cmd(&cmd, OPENTHREAD_CMD_GET_MSG_TYPE_EVENT);
		printf("State is: %i\n", cmd.content.state);
	    }
	}
	else
	{
	    printf("Usage...");
	}
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
    {"ifconfig", "Get or set panid", _ifconfig},
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
