/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @brief   Public declarations for the multiplexing jobs via the
 *          serial interface for the 6LoWPAN Border Router driver.
 */
#ifndef MULTIPLEX_H
#define MULTIPLEX_H

#include <netinet/ip6.h>
#include <stdint.h>

#include "sixlowdriver.h"

#define MTU                     1280    ///< MTU for IPv6 packets on serial interface.

/* packet types of tty-packets */
#define BORDER_PACKET_RAW_TYPE  0       ///< Packet type for raw packets.
#define BORDER_PACKET_CONF_TYPE 2       ///< Packet type for configuration packets.
#define BORDER_PACKET_L3_TYPE   3       ///< Packet type for layer 3 packets.

/* configuration types */
#define BORDER_CONF_CONTEXT     2       ///< Configuration packet type for context updates.
#define BORDER_CONF_IPADDR      3       ///< Configuration packet type for IP address updates.

/* ethertypes for L3 packets */
#define ETHERTYPE_IPV6          0x86DD  ///< Ethertype for IPv6-Datagrams.

/**
 * @brief   Describes packets for transmission via serial interface.
 */
typedef struct __attribute__((packed)) border_packet_t {
    /**
     * @brief Reserved byte.
     *
     * Must be always 0 to distinguish packets from MSB-A2
     * stdout/stdin/stderr.
     */
    uint8_t empty;
    uint8_t type;       ///< Type of the packet.
    uint8_t seq_num;    ///< Sequence number of the packet (for flow control).
} border_packet_t;

/**
 * @brief   Describes a layer 3 packet header for transmission via
 *          serial interface.
 * @extends border_packet_t
 */
typedef struct __attribute__((packed)) border_l3_header_t {
    uint8_t empty;
    uint8_t type;
    uint8_t seq_num;
    uint16_t ethertype; ///< Ethertype of the layer 3 packet.
} border_l3_header_t;

/**
 * @brief   Describes a configuration packet header for transmission via
 *          serial interface.
 * @extends border_packet_t
 */
typedef struct __attribute__((packed)) border_conf_header_t {
    uint8_t empty;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;   ///< Configuration packet type of this packet.
} border_conf_header_t;

/**
 * @brief   Describes an address configuration packet.
 * @extends border_conf_header_t
 *
 * This packet type enables the driver to add new IPv6 addresses to
 * the border router.
 */
typedef struct __attribute__((packed)) border_addr_packet_t {
    uint8_t empty;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
    /**
     * @brief   Version for this IP address (send with the ABRO for PIs,
     *          s. draft-ietf-6lowpan-nd-17).
     */
    uint16_t version;
    struct in6_addr addr;   ///< New IPv6-Address to be added to this border router.
} border_addr_packet_t;

/**
 * @brief   Describes a context configuration packet.
 * @extends border_conf_header_t
 *
 * This packet type enables the driver to manipulate Context Informations
 * in the LoWPAN.
 */
typedef struct __attribute__((packed)) border_context_packet_t {
    uint8_t empty;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
    border_context_t context;   ///< Describes the context to be manipulated.
} border_context_packet_t;

/**
 * @brief   Size of all packet buffers in this driver.
 *
 * @ref border_l3_header_t was since packets of this type may be the
 * longest (with payload).
 */
#define BUFFER_SIZE sizeof (border_l3_header_t) + MTU

/**
 * @brief   Initializes multiplexer
 * @param[in] tty_dev   Filename of the serial interface over which the
 *                      multiplexer should multiplex.
 * @return 0 if successfull, -1 if not.
 */
int init_multiplex(const char *tty_dev);

/**
 * @brief   Returns a pointer to a cell in the buffer for the output
 *          data, that shall be send via the serial interface.
 * @param[in]   offset  The offset from the start of the buffer.
 * @return  Pointer to a cell in the buffer for the output
 *          data. The size of the buffer is then
 *          \ref BUFFER_SIZE - <em>offset</em>.
 */
uint8_t *get_serial_out_buffer(int offset);

/**
 * @brief   Returns a pointer to a cell in the buffer for the input
 *          data, that was received via the serial interface.
 * @param[in]   offset  The offset from the start of the buffer.
 * @return  Pointer to a cell in the buffer for the input
 *          data. The size of the buffer is then
 *          \ref BUFFER_SIZE - <em>offset</em>.
 */
uint8_t *get_serial_in_buffer(int offset);

/**
 * @brief   Demultiplexes a packet, that was received via the serial
 *          interface.
 * @param[in]   packet  Packet, that should be demultiplexed.
 * @param[in]   len     Length of the packet, that should be
 *                      demultiplexed.
 */
void demultiplex(const border_packet_t *packet, int len);

/**
 * @brief   Sends an IPv6 datagram via the serial interface.
 * @param[in]   packet  The IPv6 datagram that is to be send via the
 *                      serial interface and starts with an IPv6 header.
 *
 * The function uses the payload length field of the IPv6 Header to
 * determine the length of the overall packet. The payload bytes
 * <em>must</em> follow the header in memory.
 */
void multiplex_send_ipv6_over_tty(const struct ip6_hdr *packet);

/**
 * @brief   Sends context information via the serial interface.
 * @param[in]   context The context information that is to be send via
 *                      the serial interface.
 */
void multiplex_send_context_over_tty(const border_context_t *context);

/**
 * @brief   Sends new IPv6 address via the serial interface.
 * @param[in]   addr    The new address that is to be send via
 *                      the serial interface.
 */
void multiplex_send_addr_over_tty(struct in6_addr *addr);

/**
 * @brief   Reads a packet up to a length of <em>size</em> bytes from
 *          the serial interface and saves it to <em>packet_buf</em>.
 * @param[out]  packet_buf  The buffer the read packet should be written
 *                          into.
 * @param[in]   size        The maximum number of bytes to be read.
 * @return  The number of bytes read.
 */
int readpacket(uint8_t *packet_buf, size_t size);

/**
 * @brief   Writes a packet up to a length of <em>size</em> bytes from
 *          <em>packet_buf</em> to the serial interface.
 * @param[in]   packet_buf  The buffer from which the packet should be
 *                          written.
 * @param[in]   size        The maximum number of bytes to be written.
 * @return  The number of bytes written.
 */
int writepacket(uint8_t *packet_buf, size_t size);


#endif /* SIXLOWBORDER_H*/
