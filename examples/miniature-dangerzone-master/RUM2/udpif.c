/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cebit_demo
 * @{
 *
 * @file        udp.c
 * @brief       CeBIT 2014 demo application - router node
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "kernel.h"
#include "thread.h"
#include "tl_socket/socket.h"
#include "net_help.h"

#include "udpif.h"
#include "demo.h"
#include "viz.h"

#define SHELL_PORT          APPLICATION_PORT

#define UDP_BUFFER_SIZE     (128)

static int socket = -1;
static sockaddr6_t socket_addr;

static char server_stack[KERNEL_CONF_STACKSIZE_MAIN];
static int server_socket = -1;
static sockaddr6_t server_addr;
static void(*server_on_data)(uint16_t src_addr, char* data, int length);


void init_send_socket(void);
void server_loop(void);
void default_data_handler(uint16_t src, char *data, int length);



/* UDP server thread */
void udpif_shell_server(int argc, char **argv)
{
    uint16_t port;

    // check command line arguments
    if (argc < 2) {
        port = SHELL_PORT;
    } else {
        port = (uint16_t)atoi(argv[1]);
    }

    udpif_start_server(port, default_data_handler);
}

/* send UDP datagram from shell */
void udpif_shell_send(int argc, char **argv)
{
    uint8_t dst_addr;
    size_t length;

    // check parameters
    if (argc != 3) {
        printf("usage: send <addr> <text>\n");
        return;
    }

    // parse dst address and data length
    dst_addr = (uint8_t)atoi(argv[1]);
    length = strlen(argv[2]);

    // send packet
    udpif_send(dst_addr, SHELL_PORT, argv[2], length);
}

/* send data via UDP */
int udpif_send(uint16_t dst_addr, uint16_t port, char *data, int length)
{
    ipv6_addr_t dst;
    int bytes_send;

    if (socket < 0) {
        init_send_socket();
        if (socket < 0) {           // UGLY, something better here?!
            puts("Error sending data - unable to open sending socket");
        }
    }

    // set receiver address
    udpif_get_ipv6_address(&dst, dst_addr);
    // write address and port to socket address
    memcpy(&socket_addr.sin6_addr, &dst, sizeof(ipv6_addr_t));
    socket_addr.sin6_port = HTONS(port);

    // send data
    bytes_send = tl_socket_sendto(socket, data, length, 0, &socket_addr, sizeof(sockaddr6_t));

    if (bytes_send < 0) {
        printf("Error: Sending data to %i failed\n", dst_addr);
    } else {
        printf("Successful delivered %i bytes over UDP to %i, port %i\n", bytes_send, dst_addr, port);
    }
    return bytes_send;
}

/* start a new UDP server on given port */
void udpif_start_server(uint16_t port, void(*ondata)(uint16_t src, char *data, int length))
{
    // allow only one server running at the same time - TODO this sucks, enable more then one!
    if (server_socket > 0) {
        printf("Error: UDP server already running\n");
        return;
    }

    // open server socket
    server_socket = tl_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket < 0) {
        printf("Error: Unable to open UDP server socket\n");
    }

    // set server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET;
    server_addr.sin6_port = HTONS(port);

    // bind server socket
    if (-1 == tl_socket_bind(server_socket, &server_addr, sizeof(server_addr))) {
        printf("Error: Binding the server socket failed!\n");
        tl_socket_close(server_socket);
    }

    // set on data callback
    server_on_data = ondata;

    // start server thread
    int udpif_server_thread_pid = thread_create(server_stack,
                                              KERNEL_CONF_STACKSIZE_MAIN, 
                                              PRIORITY_MAIN, 
                                              CREATE_STACKTEST, 
                                              server_loop, 
                                              "udp_server");
    printf("UDP server started on port %d (Thread PID: %d)\n", port, udpif_server_thread_pid);
}


void server_loop(void)
{
    int bytes_received;
    char receive_buffer[UDP_BUFFER_SIZE];
    sockaddr6_t src_addr;
    socklen_t fromlen = sizeof(src_addr);
    uint16_t src_local_addr;

    // listen for data
    while (1) {
        bytes_received = tl_socket_recvfrom(server_socket,
                                            (void *)receive_buffer, 
                                            UDP_BUFFER_SIZE, 
                                            0,
                                            &src_addr, 
                                            &fromlen);
        if (bytes_received < 0) {      // receive error
            printf("ERROR: UDP server bytes_received < 0!\n");
        } else {                // handle received data
            src_local_addr = src_addr.sin6_addr.uint8[15];
            server_on_data(src_local_addr, receive_buffer, bytes_received);
            printf("UDP: received %i bytes from %i\n", bytes_received, src_local_addr);
        }
    }
}

void default_data_handler(uint16_t src, char *data, int length)
{
    printf("UDP packet received - from: [%i] - length: [%i bytes]\n", src, length);
    printf("Data: ");
    for (int i = 0; i < length; i++) {
        printf("%X ", data[i]);
    }
    printf("\n");

    viz_udp_pkt(src);
}

void init_send_socket(void)
{
    // open a new UDP socket
    socket = tl_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if (socket == -1) {
        puts("Error creating sending socket!");
        return;
    }

    // pre-configure the socket's address
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin6_family = AF_INET;

    printf("Successfully opened sending socket\n");
}

void udpif_get_ipv6_address(ipv6_addr_t *addr, uint16_t local_addr)
{
    ipv6_addr_init(addr, 0xfe80, 0x0, 0x0, 0x0, 0x0, 0x00ff, 0xfe00, local_addr);
}
