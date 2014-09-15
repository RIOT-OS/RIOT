/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip6.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/if_tun.h>

#include "sixlowdriver.h"
#include "multiplex.h"
#include "flowcontrol.h"
#include "serialnumber.h"
#include "control_2xxx.h"

#define TUNDEV              "/dev/net/tun"
#define MAXIMUM_CONTEXTS    16

char tun_if_name[IF_NAME_LEN];

uint8_t tun_out_buf[BUFFER_SIZE];
uint8_t tun_in_buf[BUFFER_SIZE];

/* Cell i is defined as empty if context_cache[i].cid != i */
border_context_t context_cache[MAXIMUM_CONTEXTS];

int tun_fd;
pthread_t serial_reader, tun_reader;
uint16_t abro_version = 0;

uint16_t get_abro_version()
{
    return abro_version;
}

uint16_t get_next_abro_version()
{
    abro_version = serial_add16(abro_version, 1);
    return abro_version;
}

int get_tun_fd(void)
{
    return tun_fd;
}

void *serial_reader_f(void *arg)
{
    unsigned char buf[BUFFER_SIZE];
    border_packet_t *packet_buf;

    while (1) {
        int n = readpacket(buf, BUFFER_SIZE);

        if (n > 0) {
            if (buf[0] == 0) {
                packet_buf = (border_packet_t *)buf;
                flowcontrol_deliver_from_tty(packet_buf, n);
                continue;
            }

            printf("\033[00;33m[via serial interface] %s\033[00m\n", buf);
        }
    }
}

int tun_to_serial_packet(uint8_t *serial_packet, uint8_t *tun_packet, size_t packet_size)
{
    struct tun_pi *tun_hdr = (struct tun_pi *)tun_packet;
    border_l3_header_t *l3_hdr = (border_l3_header_t *)serial_packet;

    l3_hdr->empty = 0;
    l3_hdr->type = BORDER_PACKET_L3_TYPE;
    l3_hdr->ethertype = ntohs(tun_hdr->proto);

    memcpy(
        serial_packet + sizeof(border_l3_header_t),
        tun_packet + sizeof(struct tun_pi),
        packet_size - sizeof(struct tun_pi)
    );
    return (sizeof(border_l3_header_t) + (packet_size - sizeof(struct tun_pi)));
}

void *tun_reader_f(void *args)
{
    unsigned char data[BUFFER_SIZE];

    while (1) {
        size_t bytes = read(tun_fd, (void *)data, BUFFER_SIZE);

        if (bytes > 0) {
            bytes = tun_to_serial_packet(tun_in_buf, (uint8_t *)data, bytes);
            flowcontrol_send_over_tty((border_packet_t *)tun_in_buf, bytes);
        }
    }
}

void border_send_ipv6_over_tun(int fd, const struct ip6_hdr *packet)
{
    uint8_t tun_packet[BUFFER_SIZE];
    int packet_size = packet->ip6_plen + sizeof(struct ip6_hdr);

    struct tun_pi *tun_hdr = (struct tun_pi *)tun_packet;

    tun_hdr->flags = 0;
    tun_hdr->proto = htons(ETHERTYPE_IPV6);

    memcpy(tun_packet + sizeof(struct tun_pi), (uint8_t *)packet, packet_size);
    write(tun_fd, tun_packet, packet_size + sizeof(struct tun_pi));
}

int tun_set_owner(int fd, const uid_t *uid, const gid_t *gid)
{
    if (uid != NULL) {
        if (*uid != -1 && ioctl(fd, TUNSETOWNER, *uid)) {
            return -1;
        }
    }

    if (gid != NULL) {
        if (*gid != -1 && ioctl(fd, TUNSETGROUP, *gid)) {
            return -1;
        }
    }

    return 0;
}

int tun_add_addr(const char *ip_addr)
{
    char command[21 + IPV6_ADDR_LEN + IF_NAME_LEN];

    printf("INFO: ip addr add %s dev %s\n", ip_addr, tun_if_name);
    sprintf(command, "ip addr add %s dev %s", ip_addr, tun_if_name);

    if (system(command) != 0) {
        return -1;
    }

    return 0;
}

/* Source: http://backreference.org/2010/03/26/tuntap-interface-tutorial/
 */
int open_tun(char *if_name, int flags)
{
    struct ifreq ifr;
    int fd, err;

    /* Arguments taken by the function:
     *
     * char *if_name: the name of an interface (or '\0'). MUST have enough
     *   space to hold the interface name if '\0' is passed
     * int flags: interface flags (eg, IFF_TUN etc.)
     */

    /* open the clone device */
    if ((fd = open(TUNDEV, O_RDWR)) < 0) {
        return fd;
    }

    /* preparation of the struct ifr, of type "struct ifreq" */
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

    if (*if_name) {
        /* if a device name was specified, put it in the structure; otherwise,
         * the kernel will try to allocate the "next" device of the
         * specified type */
        strncpy(ifr.ifr_name, if_name, IFNAMSIZ);
    }

    /* try to create the device */
    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        close(fd);
        return err;
    }

    /* if the operation was successful, write back the name of the
     * interface to the variable "if_name", so the caller can know
     * it. Note that the caller MUST reserve space in *if_name (see calling
     * code below) */
    strcpy(if_name, ifr.ifr_name);

    /* this is the special file descriptor that the caller will use to talk
     * with the virtual interface */
    return fd;
}

int context_empty(uint8_t cid)
{
    return context_cache[cid].cid != cid;
}

int border_update_context(uint8_t cid, const struct in6_addr *prefix,
                          uint8_t len, uint8_t comp,
                          uint16_t lifetime)
{
    if (cid >= MAXIMUM_CONTEXTS) {
        return -1;
    }

    len = (len <= 128) ? len : 128;

    if (context_empty(cid)) {
        context_cache[cid].version = get_abro_version();
    }
    else {
        context_cache[cid].version = get_next_abro_version();
    }

    context_cache[cid].cid = cid;
    memcpy(&(context_cache[cid].prefix), prefix, len);
    context_cache[cid].len = len;
    context_cache[cid].comp = comp;
    context_cache[cid].lifetime = lifetime;

    multiplex_send_context_over_tty(&context_cache[cid]);

    return 0;
}

int border_renew_existing_context(uint8_t cid)
{
    if (cid >= MAXIMUM_CONTEXTS) {
        return -1;
    }

    if (context_empty(cid)) {
        return -1;
    }

    multiplex_send_context_over_tty(&context_cache[cid]);

    return 0;
}

void border_remove_context(uint8_t cid)
{
    if (cid >= MAXIMUM_CONTEXTS) {
        return;
    }

    if (context_empty(cid)) {
        return;
    }

    context_cache[cid].version = get_next_abro_version();
    context_cache[cid].lifetime = 0;

    multiplex_send_context_over_tty(&context_cache[cid]);

    context_cache[cid].cid = 0xFF;
}

int border_add_addr(const char *ip_addr)
{
    struct in6_addr parsed_addr;

    if (inet_pton(AF_INET6, ip_addr, &parsed_addr) != 1) {
        return -1;
    }

    tun_add_addr(ip_addr);

    multiplex_send_addr_over_tty(&parsed_addr);
    return 0;
}

int border_initialize(char *if_name, const char *ip_addr, const char *tty_dev)
{
    int res, i;
    char command[21 + IPV6_ADDR_LEN + IF_NAME_LEN];
    char ip_addr_cpy[IPV6_ADDR_LEN];
    struct in6_addr parsed_addr;

    strncpy(ip_addr_cpy, ip_addr, IPV6_ADDR_LEN);

    strtok(ip_addr_cpy, "/");

    if ((res = inet_pton(AF_INET6, ip_addr_cpy, &parsed_addr)) != 1) {
        return res;
    }

    if ((res = init_multiplex(tty_dev)) != 0) {
        return res;
    }

    tun_fd = open_tun(if_name, IFF_TUN);

    printf("INFO: ip link set %s up\n", if_name);
    sprintf(command, "ip link set %s up", if_name);

    strncpy(tun_if_name, if_name, IF_NAME_LEN);

    if ((res = system(command)) != 0) {
        return res;
    }

    if ((res = tun_add_addr(ip_addr)) != 0) {
        return res;
    }

    // initialize context cache as empty.
    for (i = 0; i < MAXIMUM_CONTEXTS; i++) {
        context_cache[i].cid = 0xFF;
    }

    pthread_create(&serial_reader, NULL, serial_reader_f, NULL);

    hard_reset_to_user_code();
    flowcontrol_init(&parsed_addr);
    pthread_create(&tun_reader, NULL, tun_reader_f, NULL);

    return 0;
}
