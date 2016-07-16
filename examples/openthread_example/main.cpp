#ifdef MODULE_OPENTHREAD_CLI
#include "ot-cli.h"
#else
#include <openthread.h>
#endif

#include "shell.h"
#include "shell_commands.h"
#include <stdio.h>

#include <cli/cli.hpp>
#include <net/ipv6/addr.h>
#include "net/conn/udp.h"
#include "conn.h"
#include "periph_conf.h"
#include "periph/i2c.h"

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

int ipaddr_cmd(int argc, char **argv)
{
    for (const otNetifAddress *addr = otGetUnicastAddresses(); addr; addr = addr->mNext)
        {
            printf("%x:%x:%x:%x:%x:%x:%x:%x\r\n",
                                  HostSwap16(addr->mAddress.mFields.m16[0]),
                                  HostSwap16(addr->mAddress.mFields.m16[1]),
                                  HostSwap16(addr->mAddress.mFields.m16[2]),
                                  HostSwap16(addr->mAddress.mFields.m16[3]),
                                  HostSwap16(addr->mAddress.mFields.m16[4]),
                                  HostSwap16(addr->mAddress.mFields.m16[5]),
                                  HostSwap16(addr->mAddress.mFields.m16[6]),
                                  HostSwap16(addr->mAddress.mFields.m16[7]));
        }
    return 0;
}
static conn_udp_t conn_obj;

int udp_cmd(int argc, char **argv)
{
    char *addr_str = argv[1];
    ipv6_addr_t ad;
    ipv6_addr_from_str(&ad, addr_str);
    conn_udp_create(&conn_obj, &ad, 0, 0, 7335);
    //conn_udp_getlocaladdr(&conn_obj, NULL, 0);
    /*char data[10];
    uint16_t port;
    char addr[16];
    size_t addr_len;*/
    /*conn_udp_recvfrom(&conn_obj, data, 16, addr, &addr_len, &port);
    printf("Port was: %i\n", port);
    printf("And address was: \n");
    for(int i=0;i<16;i++)
    {
        printf("%02x ",addr[i]);
    }
    printf("\n");
    printf("And the message was: \n");;
    for(int i=0;i<4;i++)
    {
        printf("%i ", data[i]);
    }
    printf("\n");*/
    return 0;
}

int udp_send_cmd(int argc, char **argv)
{
    char *addr_str = argv[1];
    uint16_t port = atoi(argv[2]);
    size_t payload_size = strlen(argv[3]);

    ipv6_addr_t addr;
    ipv6_addr_from_str(&addr, addr_str);

    char srcaddr[16];
    srcaddr[0] = 69;

    conn_udp_sendto(argv[3], payload_size, srcaddr, 16, &addr, 16, 0, 0, port);
    printf("Sent.\n");
    return 0;
}

static const shell_command_t shell_commands[] = {
        { "state", "get OpenThread state", state_cmd },
        //{ "udp", "start udp server", udp_cmd },
        { "udp", "send udp to given address and port", udp_send_cmd },
        { "ipaddr", "get ip addrs", ipaddr_cmd },
            { NULL, NULL, NULL }
};

void cli_run(void)
{
    char addr_str[16];
    memset(addr_str, 0, 16);
    ipv6_addr_t ad;
    ipv6_addr_from_str(&ad, addr_str);
    conn_udp_create(&conn_obj, &ad, 0, 0, 7335);

    char line_buf[3*SHELL_DEFAULT_BUFSIZE];
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
