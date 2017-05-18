#include <stdio.h>

#include "net/ipv6/addr.h"
#include "openthread/ip6.h"
#include "openthread/thread.h"
#include "openthread/udp.h"
#include "ot.h"
#include "shell.h"
#include "shell_commands.h"

otUdpSocket mSocket;

static OT_JOB _set_panid(otInstance *ot_instance, void *context)
{
    uint16_t panid = *((uint16_t *) context);

    otLinkSetPanId(ot_instance, panid);
}

static OT_JOB _get_panid(otInstance *ot_instance, void *context)
{
    *((uint16_t *) context) = otLinkGetPanId(ot_instance);
    printf("PanID: %04x\n", *((uint16_t *) context));
}

static OT_JOB _thread_start(otInstance *ot_instance, void *context)
{
    printf("Starting OpenThread\n");
    otIp6SetEnabled(ot_instance, true);
    otThreadSetEnabled(ot_instance, true);
}

static OT_JOB _read_state(otInstance *ot_instance, void *context)
{
    uint8_t state = otThreadGetDeviceRole(ot_instance);

    switch (state) {
        case kDeviceRoleOffline:
            puts("offline");
            break;
        case kDeviceRoleDisabled:
            puts("disabled");
            break;
        case kDeviceRoleDetached:
            puts("detached");
            break;
        case kDeviceRoleChild:
            puts("child");
            break;
        case kDeviceRoleRouter:
            puts("router");
            break;
        case kDeviceRoleLeader:
            puts("leader");
            break;
    }
}

static OT_JOB _get_ip_addresses(otInstance *ot_instance, void *context)
{
    for (const otNetifAddress *addr = otIp6GetUnicastAddresses(ot_instance); addr; addr = addr->mNext) {
        char addrstr[IPV6_ADDR_MAX_STR_LEN];
        ipv6_addr_to_str(addrstr, (ipv6_addr_t *) &addr->mAddress.mFields, sizeof(addrstr));
        printf("inet6 %s\n", addrstr);
    }
}

void _handle_receive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    size_t payload_len = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);

    char buf[100];

    otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, payload_len);

    printf("Message: ");
    for (int i = 0; i < payload_len; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

static OT_JOB _create_udp_socket(otInstance *ot_instance, void *context)
{
    otSockAddr sockaddr;

    memset(&sockaddr, 0, sizeof(otSockAddr));
    sockaddr.mPort = *((uint16_t *) context);

    otUdpOpen(ot_instance, &mSocket, _handle_receive, NULL);
    otUdpBind(&mSocket, &sockaddr);
}

typedef struct {
    ipv6_addr_t ip_addr;
    uint16_t port;
    void *payload;
    size_t len;
} udp_pkt_t;

static OT_JOB _send_udp_pkt(otInstance *ot_instance, void *context)
{
    udp_pkt_t *pkt = (udp_pkt_t *) context;
    otMessage *message;

    otUdpSocket socket;

    memset(&socket, 0, sizeof(otUdpSocket));

    otUdpOpen(ot_instance, &socket, _handle_receive, NULL);

    message = otUdpNewMessage(ot_instance, true);
     otMessageSetLength(message, pkt->len);
    otMessageWrite(message, 0, pkt->payload, pkt->len);

    otMessageInfo mPeer;

    //Set dest address
    memcpy(&mPeer.mPeerAddr.mFields, &(pkt->ip_addr), sizeof(ipv6_addr_t));

    //Set dest port
    mPeer.mPeerPort = pkt->port;

    otUdpSend(&socket, message, &mPeer);
    otUdpClose(&socket);
}

int _udp(int argc, char **argv)
{
    if (argc < 3) {
        return 1;
    }
    else if (strcmp(argv[1], "server") == 0) {
        uint16_t port = atoi(argv[2]);
        ot_exec_job(_create_udp_socket, &port);
    }
    else if (argc >= 2 && strcmp(argv[1], "send") == 0) {
        /* send packet */
        udp_pkt_t pkt;
        ipv6_addr_from_str(&pkt.ip_addr, argv[2]);
        pkt.port = atoi(argv[3]);
        pkt.payload = argv[4];
        pkt.len = strlen(argv[4]);
        ot_exec_job(_send_udp_pkt, &pkt);
    }
    return 0;
}

static void _usage(char *cmd)
{
    printf("Usage: %s \n", cmd);
    puts("        List Thread IPv6 addresses");
    printf("    %s <get|set> <panid> \n", cmd);
    puts("        Get/Set PAN ID");
    printf("    %s thread start \n", cmd);
    puts("        Start the thread network");
    printf("    %s thread state \n", cmd);
    puts("        Asks for the state of a thread network.");
}

int _ifconfig(int argc, char **argv)
{
    uint16_t panid;

    if (argc >= 2) {
        if (strcmp(argv[1], "get") == 0 && argc >= 3) {
            if (strcmp(argv[2], "panid") == 0) {
                ot_exec_job(_get_panid, &panid);
            }
        }
        else if (strcmp(argv[1], "set") == 0 && argc >= 4) {
            if (strcmp(argv[2], "panid") == 0) {
                panid = strtol(argv[3], NULL, 0);
                ot_exec_job(_set_panid, &panid);
            }
        }
        else if (strcmp(argv[1], "thread") == 0) {
            if (strcmp(argv[2], "start") == 0) {
                ot_exec_job(_thread_start, NULL);
            }
            else if (strcmp(argv[2], "state") == 0) {
                uint8_t state;
                ot_exec_job(_read_state, &state);
            }
        }
        else {
            _usage(argv[0]);
        }
    }
    else {
        ot_exec_job(_get_ip_addresses, NULL);
    }
    return 0;
}

#if !defined(MODULE_OPENTHREAD_CLI) && !defined(MODULE_OPENTHREAD_NCP)
static const shell_command_t shell_commands[] = {
    { "ifconfig", "Get or set panid", _ifconfig },
    { "udp", "Test udp", _udp },
    { NULL, NULL, NULL }
};
#endif

int main(void)
{
#if defined(MODULE_OPENTHREAD_CLI) || defined(MODULE_OPENTHREAD_NCP)
    openthread_uart_run();
#else
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
#endif

    return 0;
}
