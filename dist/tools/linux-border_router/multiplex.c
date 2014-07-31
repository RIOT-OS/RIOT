/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include "flowcontrol.h"
#include "multiplex.h"
#include "serial.h"
#include "sixlowdriver.h"

#define END         0xC0
#define ESC         0xDB
#define END_ESC     0xDC
#define ESC_ESC     0xDD

uint8_t serial_out_buf[BUFFER_SIZE];
uint8_t serial_in_buf[BUFFER_SIZE];

uint8_t *get_serial_out_buffer(int offset)
{
    if (offset > BUFFER_SIZE) {
        return NULL;
    }

    return &(serial_out_buf[offset]);
}

uint8_t *get_serial_in_buffer(int offset)
{
    if (offset > BUFFER_SIZE) {
        return NULL;
    }

    return &(serial_in_buf[offset]);
}

int init_multiplex(const char *tty_dev)
{
    return open_serial_port(tty_dev);
}

uint8_t serial_read_byte()
{
    unsigned char c;

    read_serial_port(&c, 1);

    return (uint8_t)c;
}

int readpacket(uint8_t *packet_buf, size_t size)
{
    uint8_t *line_buf_ptr = packet_buf;
    uint8_t byte = END + 1;
    uint8_t esc = 0;
    uint8_t translate = 1;

    while ((line_buf_ptr - packet_buf) < size - 1) {
        byte = serial_read_byte();

        if (translate && byte == END) {
            break;
        }

        if (line_buf_ptr == packet_buf && byte != 0) {
            translate = 0;
        }

        if (line_buf_ptr > packet_buf && !translate && byte == '\n') {
            *line_buf_ptr++ = '\0';
            return line_buf_ptr - packet_buf;
        }

        if (translate) {
            if (esc) {
                esc = 0;

                switch (byte) {
                    case (END_ESC): {
                        *line_buf_ptr++ = END;
                        continue;
                    }

                    case (ESC_ESC): {
                        *line_buf_ptr++ = ESC;
                        continue;
                    }

                    default:
                        continue;
                }
            }

            if (byte == ESC) {
                esc = 1;
                continue;
            }
        }

        *line_buf_ptr++ = byte;
    }

    return (line_buf_ptr - packet_buf);
}



int writepacket(uint8_t *packet_buf, size_t size)
{
    uint8_t packet_tmp[2 * BUFFER_SIZE];
    uint8_t *byte_ptr = packet_buf;
    uint8_t *tmp_ptr = packet_tmp;

    if (2 * size + 1 > BUFFER_SIZE) {
        return -1;
    }

    while ((byte_ptr - packet_buf) < size) {
        switch (*byte_ptr) {
            case (END): {
                *byte_ptr = END_ESC;
                *tmp_ptr = ESC;
                tmp_ptr++;
                break;
            }

            case (ESC): {
                *byte_ptr = ESC_ESC;
                *tmp_ptr = ESC;
                tmp_ptr++;
                break;
            }

            default: {
                break;
            }
        }

        *tmp_ptr = *byte_ptr;
        byte_ptr++;
        tmp_ptr++;
    }

    *tmp_ptr++ = END;
    write_serial_port(packet_tmp, tmp_ptr - packet_tmp);

    return 0;
}

void demultiplex(const border_packet_t *packet, int len)
{
    switch (packet->type) {
        case (BORDER_PACKET_RAW_TYPE): {
            printf("\033[00;33m[via serial interface] %s\033[00m\n",
                   ((unsigned char *)packet) + sizeof(border_packet_t)
                  );
            break;
        }

        case (BORDER_PACKET_L3_TYPE): {
            border_l3_header_t *l3_header_buf = (border_l3_header_t *)packet;

            switch (l3_header_buf->ethertype) {
                case (ETHERTYPE_IPV6): {
                    printf("INFO: IPv6-Packet %d received\n", l3_header_buf->seq_num);
                    struct ip6_hdr *ip6_buf = (struct ip6_hdr *)(((unsigned char *)packet) + sizeof(border_l3_header_t));
                    border_send_ipv6_over_tun(get_tun_fd(), ip6_buf);
                    break;
                }

                default:
                    printf("INFO: Unknown ethertype %04x for packet %d\n", l3_header_buf->ethertype, l3_header_buf->seq_num);
                    break;
            }

            break;
        }

        case (BORDER_PACKET_CONF_TYPE): {
            border_conf_header_t *conf_header_buf = (border_conf_header_t *)packet;

            switch (conf_header_buf->conftype) {
                case (BORDER_CONF_SYNACK): {
                    printf("INFO: SYNACK-Packet %d received\n", conf_header_buf->seq_num);
                    signal_connection_established();
                    break;
                }

                case (BORDER_CONF_CONTEXT): {
                    printf("INFO: Context packet (%d) received, "
                           "but nothing is implemented yet for this case.\n",
                           conf_header_buf->seq_num);
                    break;
                }

                case (BORDER_CONF_IPADDR): {
                    char str_addr[IPV6_ADDR_LEN];
                    border_addr_packet_t *addr_packet = (border_addr_packet_t *)packet;

                    printf("INFO: Address packet (%d) received.\n",
                           conf_header_buf->seq_num);
                    inet_ntop(AF_INET6, &addr_packet->addr, str_addr, IPV6_ADDR_LEN);
                    tun_add_addr(str_addr);
                }

                default:
                    printf("INFO: Unknown conftype %02x for packet %d\n",
                           conf_header_buf->conftype,
                           conf_header_buf->seq_num);
                    break;
            }

            break;
        }

        default:
            printf("INFO: Unknown border packet type %02x for packet %d\n", packet->type, packet->seq_num);
            //print_packet_hex((unsigned char *)packet,len);
            break;
    }
}

void multiplex_send_context_over_tty(const border_context_t *context)
{
    border_context_packet_t *con_packet = (border_context_packet_t *)get_serial_out_buffer(0);

    con_packet->empty = 0;
    con_packet->type = BORDER_PACKET_CONF_TYPE;
    con_packet->conftype = BORDER_CONF_CONTEXT;
    memcpy(
        &con_packet->context,
        context,
        sizeof(border_context_t)
    );

    flowcontrol_send_over_tty(
        (border_packet_t *) con_packet,
        sizeof(border_context_packet_t)
    );
}

void multiplex_send_addr_over_tty(struct in6_addr *addr)
{
    border_addr_packet_t *packet = (border_addr_packet_t *)get_serial_out_buffer(0);

    packet->empty = 0;
    packet->type = BORDER_PACKET_CONF_TYPE;
    packet->conftype = BORDER_CONF_IPADDR;
    memcpy(
        &packet->addr,
        addr,
        sizeof(struct in6_addr)
    );

    flowcontrol_send_over_tty(
        (border_packet_t *) packet,
        sizeof(border_addr_packet_t)
    );
}

void multiplex_send_ipv6_over_tty(const struct ip6_hdr *packet)
{
    border_l3_header_t *l3_hdr = (border_l3_header_t *)get_serial_out_buffer(0);
    size_t packet_size = sizeof(struct ip6_hdr) + packet->ip6_plen;

    l3_hdr->empty = 0;
    l3_hdr->type = BORDER_PACKET_L3_TYPE;
    l3_hdr->ethertype = ETHERTYPE_IPV6;
    memcpy(
        get_serial_out_buffer(0) + sizeof(border_l3_header_t),
        packet,
        packet_size
    );

    flowcontrol_send_over_tty(
        (border_packet_t *) l3_hdr,
        sizeof(border_l3_header_t) + packet_size
    );
}
