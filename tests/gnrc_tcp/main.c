/*
 * Copyright (C) 2018   Simon Brummer <simon.brummer@posteo.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "msg.h"
#include "net/af.h"
#include "net/gnrc/tcp.h"

#define MAIN_QUEUE_SIZE (8)
#define BUFFER_SIZE (2049)

static msg_t main_msg_queue[MAIN_QUEUE_SIZE];
static gnrc_tcp_tcb_t tcb;
static char buffer[BUFFER_SIZE];

void dump_args(int argc, char **argv)
{
    printf("%s: ", argv[0]);
    printf("argc=%d", argc);
    for (int i = 0; i < argc; ++i) {
        printf(", argv[%d] = %s", i, argv[i]);
    }
    printf("\n");
}

int get_af_family(char *family)
{
    if (memcmp(family, "AF_INET6", sizeof("AF_INET6")) == 0) {
        return AF_INET6;
    }
    else if (memcmp(family, "AF_INET", sizeof("AF_INET")) == 0) {
        return AF_INET;
    }
    return AF_UNSPEC;
}

/* API Export for test script */
int buffer_init_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    memset(buffer, '\0', sizeof(buffer));
    return 0;
}

int buffer_get_max_size_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    printf("%s: returns %d\n", argv[0], BUFFER_SIZE - 1);
    return 0;
}

int buffer_write_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    size_t offset = atol(argv[1]);
    char *src = argv[2];

    size_t src_len = strlen(src);
    char *dst = buffer + offset;

    memcpy(dst, src, src_len);
    return 0;
}

int buffer_read_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    size_t offset = atol(argv[1]);
    size_t size = atol(argv[2]);

    /* Calculate Start and End of readout */
    char *begin = buffer + offset;
    char *end = begin + size;

    /* Place temporary endmarker in buffer and print */
    char tmp = *end;
    *end = '\0';

    printf("%s: <begin>%s<end>\n", argv[0], begin);

    *end = tmp;

    return 0;
}

int gnrc_tcp_tcb_init_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    gnrc_tcp_tcb_init(&tcb);
    return 0;
}

int gnrc_tcp_open_active_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    int af_family = get_af_family(argv[1]);
    char *target_addr = argv[2];
    uint16_t target_port = atol(argv[3]);
    uint16_t local_port = atol(argv[4]);

    int err = gnrc_tcp_open_active(&tcb, af_family, target_addr, target_port,
                                   local_port);
    switch (err) {
        case -EAFNOSUPPORT:
            printf("%s: returns -EAFNOSUPPORT\n", argv[0]);
            break;

        case -EINVAL:
            printf("%s: returns -EINVAL\n", argv[0]);
            break;

        case -EISCONN:
            printf("%s: returns -EISCONN\n", argv[0]);
            break;

        case -ENOMEM:
            printf("%s: returns -ENOMEM\n", argv[0]);
            break;

        case -EADDRINUSE:
            printf("%s: returns -EADDRINUSE\n", argv[0]);
            break;

        case -ETIMEDOUT:
            printf("%s: returns -ETIMEOUT\n", argv[0]);
            break;

        case -ECONNREFUSED:
            printf("%s: returns -ECONNREFUSED\n", argv[0]);
            break;

        default:
            printf("%s: returns %d\n", argv[0], err);
    }
    return err;
}

int gnrc_tcp_open_passive_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    int af_family = get_af_family(argv[1]);
    char *local_addr = NULL;
    uint16_t local_port = 0;

    if (argc == 3) {
        local_port = atol(argv[2]);
    }
    else if (argc == 4) {
        local_addr = argv[2];
        local_port = atol(argv[3]);
    }

    int err = gnrc_tcp_open_passive(&tcb, af_family, local_addr, local_port);
    switch (err) {
        case -EAFNOSUPPORT:
            printf("%s: returns -EAFNOSUPPORT\n", argv[0]);
            break;

        case -EINVAL:
            printf("%s: returns -EINVAL\n", argv[0]);
            break;

        case -EISCONN:
            printf("%s: returns -EISCONN\n", argv[0]);
            break;

        case -ENOMEM:
            printf("%s: returns -ENOMEM\n", argv[0]);
            break;

        default:
            printf("%s: returns %d\n", argv[0], err);
    }
    return err;
}

int gnrc_tcp_send_cmd(int argc, char **argv)
{
    dump_args(argc, argv);

    int timeout = atol(argv[1]);
    size_t to_send = strlen(buffer);
    size_t sent = 0;

    while (sent < to_send) {
        int ret = gnrc_tcp_send(&tcb, buffer + sent, to_send - sent, timeout);
        switch (ret) {
            case -ENOTCONN:
                printf("%s: returns -ENOTCONN\n", argv[0]);
                return ret;

            case -ECONNRESET:
                printf("%s: returns -ECONNRESET\n", argv[0]);
                return ret;

            case -ECONNABORTED:
                printf("%s: returns -ECONNABORTED\n", argv[0]);
                return ret;

            case -ETIMEDOUT:
                printf("%s: returns -ETIMEDOUT\n", argv[0]);
                return ret;
        }
        sent += ret;
    }

    printf("%s: sent %u\n", argv[0], (unsigned)sent);
    return sent;
}

int gnrc_tcp_recv_cmd(int argc, char **argv)
{
    dump_args(argc, argv);

    int timeout = atol(argv[1]);
    size_t to_receive = atol(argv[2]);
    size_t rcvd = 0;

    while (rcvd < to_receive) {
        int ret = gnrc_tcp_recv(&tcb, buffer + rcvd, to_receive - rcvd,
                                timeout);
        switch (ret) {
            case -EAGAIN:
                printf("%s: returns -EAGAIN\n", argv[0]);
                continue;

            case -ETIMEDOUT:
                printf("%s: returns -ETIMEDOUT\n", argv[0]);
                continue;

            case -ENOTCONN:
                printf("%s: returns -ENOTCONN\n", argv[0]);
                return ret;

            case -ECONNRESET:
                printf("%s: returns -ECONNRESET\n", argv[0]);
                return ret;

            case -ECONNABORTED:
                printf("%s: returns -ECONNABORTED\n", argv[0]);
                return ret;
        }
        rcvd += ret;
    }

    printf("%s: received %u\n", argv[0], (unsigned)rcvd);
    return 0;
}

int gnrc_tcp_close_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    gnrc_tcp_close(&tcb);
    return 0;
}

int gnrc_tcp_abort_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    gnrc_tcp_abort(&tcb);
    return 0;
}

/* Exporting GNRC TCP Api to for shell usage */
static const shell_command_t shell_commands[] = {
    { "gnrc_tcp_tcb_init", "gnrc_tcp: init tcb", gnrc_tcp_tcb_init_cmd },
    { "gnrc_tcp_open_active", "gnrc_tcp: open active connection",
      gnrc_tcp_open_active_cmd },
    { "gnrc_tcp_open_passive", "gnrc_tcp: open passive connection",
      gnrc_tcp_open_passive_cmd },
    { "gnrc_tcp_send", "gnrc_tcp: send data to connected peer",
      gnrc_tcp_send_cmd },
    { "gnrc_tcp_recv", "gnrc_tcp: recv data from connected peer",
      gnrc_tcp_recv_cmd },
    { "gnrc_tcp_close", "gnrc_tcp: close connection gracefully",
      gnrc_tcp_close_cmd },
    { "gnrc_tcp_abort", "gnrc_tcp: close connection forcefully",
      gnrc_tcp_abort_cmd },
    { "buffer_init", "init internal buffer", buffer_init_cmd },
    { "buffer_get_max_size", "get max size of internal buffer",
      buffer_get_max_size_cmd },
    { "buffer_write", "write data into internal buffer", buffer_write_cmd },
    { "buffer_read", "read data from internal buffer", buffer_read_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(main_msg_queue, MAIN_QUEUE_SIZE);
    printf("RIOT GNRC_TCP test application\n");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
