#include <stdio.h>

#include "net/ipv6/addr.h"
#include "net/sock/udp.h"
#include "openthread/ip6.h"
#include "openthread/thread.h"
#include "openthread/udp.h"
#include "ot.h"
#include "shell.h"
#include "shell_commands.h"

static sock_udp_t sock;
static char _udp_buf[100];

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

int _listen(int argc, char **argv)
{
	int bytes = sock_udp_recv(&sock, (char *)_udp_buf, sizeof(_udp_buf), SOCK_NO_TIMEOUT, NULL);
	for(int i=0;i<bytes;i++)
	{
		printf("%c", _udp_buf[i]);
	}
	puts("\n");
	return 0;
}

int _udp(int argc, char **argv)
{
    if (argc < 3)
    {
        return 1;
    }
    else if (strcmp(argv[1],"server")==0)
    {
        sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
        local.port = atoi(argv[2]);
        if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
    }
    }
    else if(argc >= 2 && strcmp(argv[1],"send")==0)
    {
    sock_udp_ep_t remote = { .family = AF_INET6 };
    remote.port = atoi(argv[3]);
    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr.ipv6, argv[2]);
    if (sock_udp_send(&sock, argv[4], strlen(argv[4]), &remote) < 0) {
        puts("Error sending message");
        sock_udp_close(&sock);
        return 1;
    }
    }
    else if(argc >= 2 && strcmp(argv[1],"echo")==0)
    {
    }
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
    {"udp", "Test udp", _udp},
    {"listen", "Listen in given server port", _listen},
    {NULL, NULL, NULL}
};
#endif

int main(void)
{
#if defined(MODULE_OPENTHREAD_CLI) || defined(MODULE_OPENTHREAD_NCP)
    openthread_uart_run();
#else
    uint16_t panid=0x1234;
    ot_exec_job(_set_panid, &panid);
    ot_exec_job(_thread_start, NULL);
    char line_buf[3 * SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
#endif

    return 0;
}
