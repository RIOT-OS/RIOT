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

static otUdpSocket mSocket;
static otMessageInfo mPeer;

void HandleUdpReceived(void *aContext, otMessage aMessage, const otMessageInfo *aMessageInfo)
{
    printf("Hello\n");
}

int udp_cmd(int argc, char **argv)
{
    otSockAddr sockaddr = {};
    sockaddr.mPort = 7335;

    otOpenUdpSocket(&mSocket, &HandleUdpReceived, NULL);
    otBindUdpSocket(&mSocket, &sockaddr);
    return 0;
}

int udp_send_cmd(int argc, char **argv)
{
    char buf[] = "hola";
    otMessage message;

    ipv6_addr_t addr;
    char *addr_str = argv[1];
    ipv6_addr_from_str(&addr, addr_str);
    printf("Ip address: %s\n", addr_str);

    otSockAddr peer = {};
    peer.mPort = 7335;
    printf("\n");
    for(int i=0;i<16;i++)
    {
        peer.mAddress.mFields.m8[i] = addr.u8[i];
    }
    printf("\n");
    mSocket.mPeerName = peer;

    message = otNewUdpMessage();
    printf("message %i\n", (int) message);
    printf("otSetMessageLength %i\n",otSetMessageLength(message, 4));
    printf("otWriteMessage %i\n",otWriteMessage(message, 0, buf, 4));
    mPeer.mPeerAddr = peer.mAddress;
    mPeer.mPeerPort = peer.mPort;
    otSendUdp(&mSocket, message, &mPeer);
    return 0;
}

static const shell_command_t shell_commands[] = {
        { "state", "get OpenThread state", state_cmd },
        { "udp", "start udp server", udp_cmd },
        { "send_udp", "send udp to address", udp_send_cmd },
        { "ipaddr", "get ip addrs", ipaddr_cmd },
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
