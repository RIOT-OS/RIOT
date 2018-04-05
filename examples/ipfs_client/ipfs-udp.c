#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

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
#define BUFFER_SIZE (256)
#define WORKERS (10)

enum operation_t { GET, PUT, POST };

void ipfs_udp_send(char *addr_str, uint16_t src_port, uint16_t dst_port, char *data, size_t data_len, unsigned int num, unsigned int delay)
{
    ipv6_addr_t addr;

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return;
    }
    if (src_port <= 0 || src_port >= 65535) {
        puts("Error: unable to parse destination port");
        return;
    }
    if (dst_port <= 0 || dst_port >= 65535) {
        puts("Error: unable to parse destination port");
        return;
    }

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
        udp = gnrc_udp_hdr_build(payload, src_port, dst_port);
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
        printf("Success: sent %u byte(s) to [%s]:%u\n", payload_size, addr_str, dst_port);
        xtimer_usleep(delay);
    }
}

struct worker_t {
    int used;
    gnrc_netreg_entry_t server;
    char server_stack[SERVER_STACKSIZE];
    msg_t server_queue[SERVER_MSG_QUEUE_SIZE];
    kernel_pid_t server_pid;
    uint16_t src_port, dst_port;
    char ip[40];
    enum operation_t operation;
    char object[BUFFER_SIZE];
    char data[BUFFER_SIZE];
};
static struct worker_t workers[WORKERS];

void ipfs_udp_init_workers(void)
{
    for (int i = 0; i < WORKERS; i++) {
//  workers[i].server = GNRC_NETREG_ENTRY_INIT_PID(0, KERNEL_PID_UNDEF);
        memset(&(workers[i].server), 0, sizeof(workers[i].server));
        workers[i].server_pid = KERNEL_PID_UNDEF;
        workers[i].used = 0;
    }
}

static int ipfs_udp_analyze(msg_t *msg, struct worker_t *worker)
{
    char src_addr_str[IPV6_ADDR_MAX_STR_LEN];
    int src_port;
    char data[BUFFER_SIZE];
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
                memset(data, 0, BUFFER_SIZE);
                memcpy(data, ((gnrc_pktsnip_t *) msg->content.ptr)->data, ((gnrc_pktsnip_t *) msg->content.ptr)->size);
                data[((gnrc_pktsnip_t *) msg->content.ptr)->size] = '\0';
                printf("source port: %d\n", src_port );
                printf("destination port: %d\n", byteorder_ntohs(((udp_hdr_t *) snip->data)->dst_port));
                printf("length: %d\n", byteorder_ntohs(((udp_hdr_t *) snip->data)->length));
                printf("data: %s\n", data);
                if (src_port == worker->dst_port) {
                    seen++;
                }
                break;
            default: break;
        }
        if (seen == 2) {
            return 1;
        }
        ++snips;
        size += snip->size;
        snip = snip->next;
    }
    return 0;
}

static void *ipfs_udp_handler(void *arg)
{
    struct worker_t *worker = (struct worker_t *) arg;
    char buffer[BUFFER_SIZE];
    size_t buffer_len;
    msg_t msg, reply;

    /* setup the message queue */
    msg_init_queue(worker->server_queue, SERVER_MSG_QUEUE_SIZE);
    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    gnrc_netreg_register(GNRC_NETTYPE_UDP, &(worker->server));

    if (PUT == worker->operation) {
        buffer_len = snprintf(buffer, BUFFER_SIZE, "put %s %s", worker->object, worker->data);
    }
    else if (GET == worker->operation) {
        buffer_len = snprintf(buffer, BUFFER_SIZE, "get %s", worker->object);
    }
    else {
        buffer_len = 0;
    }
    ipfs_udp_send(worker->ip, worker->src_port, worker->dst_port, buffer, buffer_len, 1, 1000000LU);

    while (1) {
        printf("waiting for reply on port %d\n", worker->src_port);
        msg_receive(&msg);
        puts("message received");
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                if (1 == ipfs_udp_analyze(&msg, worker)) {
                    puts("IPFS commit");
                    gnrc_pktbuf_release(msg.content.ptr);
                    worker->used = 0;
                    gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &(worker->server));
                    worker->server_pid = KERNEL_PID_UNDEF;
                    memset(&(workers->server), 0, sizeof(worker->server));
                    return NULL;
                }
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            case SERVER_RESET:
                break;
            default:
                break;
        }
    }
    return NULL;
}


void ipfs_udp_request(char *ip, enum operation_t operation, char *object, char *data)
{
    static uint16_t port = 2000;

    port++;
    int found;
    struct worker_t *worker;

    found = 0;
    while (found == 0) {
        for (worker = workers; worker < workers + WORKERS; worker++) {
            if (worker->used == 0) {
                found = 1;
                break;
            }
        }
        if (found == 0) {
            puts("no worker available.");
            continue;
        }
        xtimer_usleep(3 * 1000000);
    }

    worker->used = 1;
    worker->src_port = port;
    worker->dst_port = 5001;
    memcpy(worker->ip, ip, strlen(ip));

    worker->operation = operation;
    memcpy(worker->object, object, strlen(object));
    if (data != NULL) {
        memcpy(worker->data, data, strlen(data));
    }

    if (worker->server_pid <= KERNEL_PID_UNDEF) {
        worker->server_pid = thread_create(worker->server_stack, sizeof(worker->server_stack), SERVER_PRIO, THREAD_CREATE_STACKTEST, ipfs_udp_handler, worker, "ipfs");
        if (worker->server_pid <= KERNEL_PID_UNDEF) {
            puts("Error: cannot start thread");
            worker->server_pid = KERNEL_PID_UNDEF;
            worker->used = 0;
            return;
        }
    }
    gnrc_netreg_entry_init_pid(&(worker->server), worker->src_port, worker->server_pid);
}



int ipfs_udp_cmd(int argc, char **argv)
{
    /* check if a port was given */
    if (argc < 4) {
        puts("usage: ipfs ip <put|get> object data");
        return 1;
    }
    if (argc == 5) {
        ipfs_udp_request(argv[1], (0 == strcmp(argv[2], "get")) ? GET : PUT, argv[3], argv[4]);
    }
    else {
        ipfs_udp_request(argv[1], (0 == strcmp(argv[2], "get")) ? GET : PUT, argv[3], NULL);
    }
    return 0;
}
