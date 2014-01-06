#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "kernel.h"
#include "thread.h"

#include "destiny/socket.h"

#include "net_help.h"

#include "demo.h"

#define UDP_BUFFER_SIZE     (128)
#define SERVER_PORT     (0xFF01)

char udp_server_stack_buffer[KERNEL_CONF_STACKSIZE_MAIN];
char addr_str[IPV6_MAX_ADDR_STR_LEN];

void init_udp_server(void);

/* UDP server thread */
void udp_server(char *unused)
{
    (void) unused;
    int udp_server_thread_pid = thread_create(udp_server_stack_buffer, KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN, CREATE_STACKTEST, init_udp_server, "init_udp_server");
    printf("UDP SERVER THREAD PID: %i\n", udp_server_thread_pid);
}

void init_udp_server(void)
{
    sockaddr6_t sa;
    char buffer_main[UDP_BUFFER_SIZE];
    int32_t recsize;
    uint32_t fromlen;
    int sock = destiny_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    memset(&sa, 0, sizeof(sa));

    sa.sin6_family = AF_INET;
    sa.sin6_port = HTONS(SERVER_PORT);

    fromlen = sizeof(sa);

    if (-1 == destiny_socket_bind(sock, &sa, sizeof(sa))) {
        printf("Error bind failed!\n");
        destiny_socket_close(sock);
    }

    for(;;) {
        recsize = destiny_socket_recvfrom(sock, (void *)buffer_main, UDP_BUFFER_SIZE, 0, 
                                          &sa, &fromlen);

        if(recsize < 0) {
            printf("ERROR: recsize < 0!\n");
        }

        printf("UDP packet received, payload: %s\n", buffer_main);
    }

    destiny_socket_close(sock);
}

/* UDP send command */
void udp_send(char *str)
{
    int sock;
    sockaddr6_t sa;
    ipv6_addr_t ipaddr;
    int bytes_sent;
    int address;
    char text[5];
    if (sscanf(str, "send %i %s", &address, text) < 2) {
        printf("usage: send <addr> <text>\n");
        return;
    }

    sock = destiny_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if(-1 == sock) {
        printf("Error Creating Socket!");
        return;
    }

    memset(&sa, 0, sizeof sa);

    ipv6_addr_init(&ipaddr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00, (uint16_t)address);

    sa.sin6_family = AF_INET;
    memcpy(&sa.sin6_addr, &ipaddr, 16);
    sa.sin6_port = HTONS(SERVER_PORT);

    bytes_sent = destiny_socket_sendto(sock, (char *)text,
            strlen(text) + 1, 0, &sa,
            sizeof sa);

    if(bytes_sent < 0) {
        printf("Error sending packet!\n");
    }
    else {
        printf("Successful deliverd %i bytes over UDP to %s to 6LoWPAN\n", bytes_sent, ipv6_addr_to_str(addr_str, &ipaddr));
    }

    destiny_socket_close(sock);
}


