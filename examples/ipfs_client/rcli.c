#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "msg.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/pktdump.h"
#include "timex.h"
#include "xtimer.h"
#include "thread.h"
#define SERVER_MSG_QUEUE_SIZE   (8U)
#define SERVER_PRIO (THREAD_PRIORITY_MAIN - 1)
#define SERVER_STACKSIZE  (THREAD_STACKSIZE_MAIN)
#define SERVER_RESET   (0x8fae)

extern int _icmpv6_ping(int argc, char **argv);
extern int ipfs_cmd(int argc, char **argv);


static gnrc_netreg_entry_t server = GNRC_NETREG_ENTRY_INIT_PID(0, KERNEL_PID_UNDEF);
static char server_stack[SERVER_STACKSIZE];
static msg_t server_queue[SERVER_MSG_QUEUE_SIZE];
static kernel_pid_t server_pid = KERNEL_PID_UNDEF;

static int listen_port;

void rcli_send(char *addr_str, uint16_t port, char *data, size_t data_len, unsigned int num, unsigned int delay)
{
    ipv6_addr_t addr;

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return;
    }
    if (port <= 0 || port >= 65535) {
        puts("Error: unable to parse destination port");
        return;
    }
    puts("go");

    for (unsigned int i = 0; i < num; i++) {
        gnrc_pktsnip_t *payload, *udp, *ip;
        unsigned payload_size;
        /* allocate payload */
        payload = gnrc_pktbuf_add(NULL, data, data_len, GNRC_NETTYPE_UNDEF);
        if (payload == NULL) {
            puts("Error: unable to copy data to packet buffer");
            return;
        }
        /* store size for output */
        payload_size = (unsigned)payload->size;
        /* allocate UDP header, set source port := destination port */
        udp = gnrc_udp_hdr_build(payload, listen_port, port);
        if (udp == NULL) {
            puts("Error: unable to allocate UDP header");
            gnrc_pktbuf_release(payload);
            return;
        }
        /* allocate IPv6 header */
        ip = gnrc_ipv6_hdr_build(udp, NULL, &addr);
        if (ip == NULL) {
            puts("Error: unable to allocate IPv6 header");
            gnrc_pktbuf_release(udp);
            return;
        }
        /* send packet */
        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
            puts("Error: unable to locate UDP thread");
            gnrc_pktbuf_release(ip);
            return;
        }
        /* access to `payload` was implicitly given up with the send operation above
         * => use temporary variable for output */
        printf("Success: sent %u byte(s) to [%s]:%u\n", payload_size, addr_str, port);
        xtimer_usleep(delay);
    }
}



static void rcli_exec_command(char *src_addr_str, int src_port, char *data)
{
#define NB_ARGS (3)
    char *arg[NB_ARGS + 1];
    char *current_pos, *tmp;
    int argc = 1;
    arg[0] = data;
    current_pos = data;
    if (NULL != (tmp = strstr(current_pos, "\n"))) {
        *tmp = '\0';
    }

    for (int i = 0; i < NB_ARGS; i++) {
        if (NULL == (tmp = strstr(current_pos, " "))) {
            arg[i + 1] = NULL;
            current_pos = data + strlen(data);
        }
        else {
            *tmp = '\0';
            arg[i + 1] = tmp + 1;
            current_pos = tmp + 1;
            argc += 1;
        }
    }

    /* exec command */
    for (int i = 0; i < NB_ARGS + 1; i++) {
        printf(" %s ", arg[i] == NULL ? "NULL" : arg[i]);
    }
    puts("");

    int res;
    res = 0;
    if (0 == strcmp(arg[0], "ping6")) {
        res = _icmpv6_ping(argc, arg);
    }
    else if (0 == strcmp(arg[0], "ipfs")) {
        res = ipfs_cmd(argc, arg);
    }

    /* return packet */
    char reply_data[256];
    size_t reply_data_len;
    reply_data_len = snprintf(reply_data, 256, "%s\n", (res == 0) ? "OK" : "KO");
    rcli_send(src_addr_str, (uint16_t) src_port, reply_data, reply_data_len, 1, 1000000LU);
}

static void rcli_analyze(msg_t *msg)
{
    char src_addr_str[IPV6_ADDR_MAX_STR_LEN];
    int src_port;
    char data[256];
    int snips, size, seen;

    gnrc_pktsnip_t *snip = (gnrc_pktsnip_t *) msg->content.ptr;

    snips = size = seen = 0;
    src_port = 0;
    while (NULL != snip) {
        switch (snip->type) {
            case GNRC_NETTYPE_IPV6:
                if (!ipv6_hdr_is((ipv6_hdr_t *) snip->data)) {
                    puts("invalid header");
                    break;
                }
                ipv6_addr_to_str(src_addr_str, &((ipv6_hdr_t *) snip->data)->src, sizeof(src_addr_str));
                printf("source address: %s\n", src_addr_str);
// printf("length: %d\n", snip->size);
                seen++;
                break;
            case GNRC_NETTYPE_UDP:
                src_port = byteorder_ntohs(((udp_hdr_t *) snip->data)->src_port);
                memset(data, 0, 256);
                memcpy(data, ((gnrc_pktsnip_t *) msg->content.ptr)->data, ((gnrc_pktsnip_t *) msg->content.ptr)->size);
                data[((gnrc_pktsnip_t *) msg->content.ptr)->size] = '\0';
                printf("source port: %d\n", src_port );
                printf("destination port: %d\n", byteorder_ntohs(((udp_hdr_t *) snip->data)->dst_port));
                printf("length: %d\n", byteorder_ntohs(((udp_hdr_t *) snip->data)->length));
                printf("data: %s\n", data);
                seen++;
                break;
            default: break;
        }
        if (seen == 2) {
            rcli_exec_command(src_addr_str, src_port, data);
            break;
        }
        ++snips;
        size += snip->size;
        snip = snip->next;
    }
}

static void *rcli_handler(void *arg)
{
    printf("listen on port %d\n", listen_port);
    (void)arg;
    msg_t msg, reply;
    unsigned int rcv_count = 0;

    /* setup the message queue */
    msg_init_queue(server_queue, SERVER_MSG_QUEUE_SIZE);
    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_receive(&msg);
        puts("message received");
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                printf("Packets received: %d\n", ++rcv_count);
                rcli_analyze(&msg);
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            case SERVER_RESET:
                rcv_count = 0;
                break;
            default:
                break;
        }
    }
    return NULL;
}


void rcli_server_start(char *port_str)
{
    /* check if server is already running */
    if (server.target.pid != KERNEL_PID_UNDEF) {
        printf("Error: server already running on port %" PRIu32 "\n", server.demux_ctx);
        return;
    }
    /* extract port */
    listen_port = atoi(port_str);
    if (listen_port <= 0 || listen_port > 65535) {
        puts("Error: invalid port specified");
        return;
    }
    /* start server thread */
    if (server_pid <= KERNEL_PID_UNDEF) {
        server_pid = thread_create(server_stack, sizeof(server_stack), SERVER_PRIO, THREAD_CREATE_STACKTEST, rcli_handler, NULL, "rcli");
        if (server_pid <= KERNEL_PID_UNDEF) {
            puts("Error: cannot start server thread");
            return;
        }
    }
    gnrc_netreg_entry_init_pid(&server, listen_port, server_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &server);
    printf("Success: started UDP server on port %" PRIu16 "\n", listen_port);
    return;
}

void rcli_cmd(int argc, char **argv)
{
    /* check if a port was given */
    if (argc < 2) {
        puts("usage: rcli <port>");
        return;
    }
    rcli_server_start(argv[1]);
    return;
}
