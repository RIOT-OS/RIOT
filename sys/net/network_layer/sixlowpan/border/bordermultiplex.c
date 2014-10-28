/**
 * 6lowpan border router multiplexer
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    bordermultiplex.c
 * @brief   multiplexiing border router information
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "board_uart0.h"
#include "sixlowpan/error.h"

#include "flowcontrol.h"
#include "../lowpan.h"
#include "../icmp.h"
#include "border.h"

#include "bordermultiplex.h"

#define END         (0xC0)
#define ESC         (0xDB)
#define END_ESC     (0xDC)
#define ESC_ESC     (0xDD)

void demultiplex(border_packet_t *packet)
{
    switch (packet->type) {
        case (BORDER_PACKET_RAW_TYPE): {
            fputs(((char *)packet) + sizeof(border_packet_t), stdin);
            break;
        }

        case (BORDER_PACKET_L3_TYPE): {
            border_l3_header_t *l3_header_buf = (border_l3_header_t *)packet;

            switch (l3_header_buf->ethertype) {
                case (BORDER_ETHERTYPE_IPV6): {
                    ipv6_hdr_t *ipv6_buf = (ipv6_hdr_t *)(((unsigned char *)packet) + sizeof(border_l3_header_t));
                    ipv6_send_packet(ipv6_buf);
                    break;
                }

                default:
                    printf("ERROR: Unknown ethertype 0x%04x\n", l3_header_buf->ethertype);
                    break;
            }

            break;
        }

        case (BORDER_PACKET_CONF_TYPE): {
            border_conf_header_t *conf_header_buf = (border_conf_header_t *)packet;

            switch (conf_header_buf->conftype) {
                case (BORDER_CONF_CONTEXT): {
                    border_context_packet_t *context = (border_context_packet_t *)packet;
                    ipv6_addr_t target_addr;
                    ipv6_addr_set_all_nodes_addr(&target_addr);
                    mutex_lock(&lowpan_context_mutex);
                    lowpan_context_update(
                        context->context.cid,
                        &context->context.prefix,
                        context->context.length,
                        context->context.comp,
                        context->context.lifetime
                    );
                    mutex_unlock(&lowpan_context_mutex);
                    abr_add_context(context->context.version, abr_addr, context->context.cid);
                    /* Send router advertisement */
                    break;
                }

                case (BORDER_CONF_IPADDR): {
                    //border_addr_packet_t *addr_packet = (border_addr_packet_t *)packet;
                    /* add address */
                    break;
                }

                default:
                    printf("ERROR: Unknown conftype %02x\n", conf_header_buf->conftype);
                    break;
            }

            break;
        }

        default:
            printf("ERROR: Unknown border packet type %02x\n", packet->type);
            break;
    }
}

void multiplex_send_ipv6_over_uart(ipv6_hdr_t *packet)
{
    border_l3_header_t *serial_buf;

    serial_buf = (border_l3_header_t *)get_serial_out_buffer(0);
    serial_buf->empty = 0;
    serial_buf->type = BORDER_PACKET_L3_TYPE;
    serial_buf->ethertype = BORDER_ETHERTYPE_IPV6;
    memcpy(get_serial_in_buffer(0) + sizeof(border_l3_header_t), packet, IPV6_HDR_LEN + packet->length);

    flowcontrol_send_over_uart((border_packet_t *) serial_buf, sizeof(border_l3_header_t));
}

void multiplex_send_addr_over_uart(ipv6_addr_t *addr)
{
    border_addr_packet_t *serial_buf;

    serial_buf = (border_addr_packet_t *)get_serial_in_buffer(0);
    serial_buf->empty = 0;
    serial_buf->type = BORDER_PACKET_CONF_TYPE;
    serial_buf->conftype = BORDER_CONF_IPADDR;
    memcpy(&serial_buf->addr, addr, sizeof(ipv6_addr_t));

    flowcontrol_send_over_uart((border_packet_t *) serial_buf, sizeof(border_addr_packet_t));
}

int readpacket(uint8_t *packet_buf, size_t size)
{
    uint8_t *line_buf_ptr = packet_buf;
    uint8_t esc = 0;

    while (1) {
        uint8_t byte = uart0_readc();

        if (byte == END) {
            break;
        }

        if ((size_t)(line_buf_ptr - packet_buf) >= size - 1) {
            return -SIXLOWERROR_ARRAYFULL;
        }

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

        *line_buf_ptr++ = byte;
    }

    return (line_buf_ptr - packet_buf - 1);
}

int writepacket(uint8_t *packet_buf, size_t size)
{
    uint8_t *byte_ptr = packet_buf;

    while ((size_t)(byte_ptr - packet_buf) < size) {
        if ((size_t)(byte_ptr - packet_buf) > BORDER_BUFFER_SIZE) {
            return -1;
        }

        switch (*byte_ptr) {
            case (END): {
                *byte_ptr = END_ESC;
                uart0_putc(ESC);
                break;
            }

            case (ESC): {
                *byte_ptr = ESC_ESC;
                uart0_putc(ESC);
                break;
            }

            default: {
                break;
            }
        }

        uart0_putc(*byte_ptr);
        byte_ptr++;
    }

    uart0_putc(END);

    return (byte_ptr - packet_buf);
}
