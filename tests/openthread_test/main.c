#include "stdio.h"
#include "ot.h"

#include "shell.h"
#include "shell_commands.h"
#include "openthread/thread.h"
#include "openthread/ip6.h"
#include "net/ipv6/addr.h"

static OT_JOB _set_panid(otInstance *ot_instance, void *data)
{
    uint16_t panid = *((uint16_t*) data);
    otLinkSetPanId(ot_instance, panid);
}

static OT_JOB _get_panid(otInstance *ot_instance, void *data)
{
    *((uint16_t*) data) = otLinkGetPanId(ot_instance);
    printf("PanID: %04x\n", *((uint16_t*) data));
}

static OT_JOB _thread_start(otInstance *ot_instance, void *data)
{
    printf("Starting OpenThread\n");
    otIp6SetEnabled(ot_instance, true);
    otThreadSetEnabled(ot_instance, true);
}

static OT_JOB _read_state(otInstance *ot_instance, void *data)
{
    uint8_t state = otThreadGetDeviceRole(ot_instance);
    printf("State is: %i\n", state);
}

static OT_JOB _get_ip_addresses(otInstance *ot_instance, void *data)
{
    for(const otNetifAddress *addr=otIp6GetUnicastAddresses(ot_instance); addr; addr=addr->mNext)
    {
        char addrstr[IPV6_ADDR_MAX_STR_LEN];
        printf("inet6 %s\n", ipv6_addr_to_str(addrstr, (ipv6_addr_t*) &addr->mAddress.mFields, sizeof(addrstr)));
    }
}

int _test(int argc, char **argv)
{
    uint16_t panid;
    ot_exec_job(_get_panid, &panid);
    return 0;
}

int _ifconfig(int argc, char **argv)
{
    uint16_t panid;
    if(argc >= 2)
    {
	if(strcmp(argv[1], "get") == 0 && argc >= 3)
	{
            if(strcmp(argv[2], "panid") == 0)
	    {
		ot_exec_job(_get_panid, &panid);
	    }	    
	}
	else if(strcmp(argv[1], "set") == 0 && argc >= 4)
	{
            if(strcmp(argv[2], "panid") == 0)
	    {
		panid = strtol(argv[3], NULL, 0);
		ot_exec_job(_set_panid, &panid);
	    }	    
	
	}
	else if(strcmp(argv[1], "thread") == 0)
	{
	    if(strcmp(argv[2], "start") == 0)
	    {
		ot_exec_job(_thread_start, NULL);
	    }
	    else if(strcmp(argv[2], "state") == 0)
            {
		uint8_t state;
		ot_exec_job(_read_state, &state);
	    }
	}
	else
	{
	    printf("Usage...");
	}
    }
    else
    {
	ot_exec_job(_get_ip_addresses, NULL);
    }
    return 0;
}

#if !defined(MODULE_OPENTHREAD_CLI) && !defined(MODULE_OPENTHREAD_NCP)
static const shell_command_t shell_commands[] = {
    {"ifconfig", "Get or set panid", _ifconfig},
    {"test", "Test function ptr", _test},
    {NULL, NULL, NULL}
};
#endif

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
