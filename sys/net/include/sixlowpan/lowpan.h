/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sixlowpan_lowpan 6LoWPAN adaptation layer
 * @ingroup     net_sixlowpan
 * @brief       IPv6 over LoW Power wireless Area Networks
 * @{
 *
 * @file        include/sixlowpan/lowpan.h
 * @brief       6LoWPAN LoWPAN layer header
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_LOWPAN_H
#define SIXLOWPAN_LOWPAN_H

#include <stdint.h>

#include "transceiver.h"
#include "net_help.h"
#include "net_if.h"
#include "sixlowpan/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   6LoWPAN dispatch value for uncompressed IPv6 packets.
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.1">
 *          RFC 4944, section 5.1
 *      </a>
 */
#define SIXLOWPAN_IPV6_DISPATCH     (0x41)

/**
 * @brief   6LoWPAN dispatch value for IPv6 header compression (part of
 *          first byte of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 4944, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC1_DISPATCH    (0x60)

/**
 * @brief   Flag for Flow Label elision (part of first byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC1_FL_C        (0x10)

/**
 * @brief   Flag for Traffic Class elision (part of first byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC1_TC_C        (0x08)

/**
 * @brief   Flag for Next Header Compression (part of first byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC1_NH          (0x04)

/**
 * @brief   Flag for Context Identifier Extention (part of second byte
 *          of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_CID         (0x80)

/**
 * @brief   Flag for Source Address Compression (part of second byte
 *          of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_SAC         (0x40)

/**
 * @brief   Bits for Source Address Mode (part of second byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_SAM         (0x30)

/**
 * @brief   Flag for Destination Address Compression (part of second
 *          byte of LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_DAC         (0x04)

/**
 * @brief   Bits for Destination Address Mode (part of second byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_DAM         (0x03)

/**
 * @brief   Flag for Multicast Compression (part of second byte of
 *          LOWPAN_IPHC).
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1.1">
 *          RFC 6282, section 3.1.1
 *      </a>
 */
#define SIXLOWPAN_IPHC2_M           (0x08)


/**
 * 6LoWPAN dispatch value for fragmentation header (first fragment)
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.1">
 *          RFC 4944, section 5.1
 *      </a>
 */
#define SIXLOWPAN_FRAG1_DISPATCH    (0xc0)

/**
 * 6LoWPAN dispatch value for fragmentation header (subsequent fragment)
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-5.1">
 *          RFC 4944, section 5.1
 *      </a>
 */
#define SIXLOWPAN_FRAGN_DISPATCH    (0xe0)


/**
 * 6LoWPAN fragmentation header length (first fragment)
 */
#define SIXLOWPAN_FRAG1_HDR_LEN     (4)

/**
 * 6LoWPAN fragmentation header length (subsequent fragment)
 */
#define SIXLOWPAN_FRAGN_HDR_LEN     (5)

/**
 * @brief message type for notification
 *
 * @see sixlowpan_lowpan_register()
 */
#define LOWPAN_FRAME_RECEIVED        (UPPER_LAYER_1)

/**
 * @brief   Data type to configure 6LoWPAN IPv6 header compression.
 */
typedef enum __attribute__((packed)) {
    LOWPAN_IPHC_DISABLE = 0,    ///< header compression disabled
    LOWPAN_IPHC_ENABLE = 1      ///< header compression enabled
} sixlowpan_lowpan_iphc_status_t;

/**
 * @brief   Data type to represent an 6LoWPAN frame as byte stream.
 */
typedef struct __attribute__((packed)) {
    uint8_t length;             ///< length of the byte stream.
    uint8_t *data;              ///< the byte stream representing the 6LoWPAN frame.
} sixlowpan_lowpan_frame_t;


/**
 * @brief   Initializes all addresses on an interface needed for 6LoWPAN.
 *
 * @param[in] if_id     The interface to use with 6LoWPAN.
 *
 * @return  1 on success, 0 on failure.
 */
int sixlowpan_lowpan_init_interface(int if_id);

/**
 * @brief   Checks if an EUI-64 was set from a short address. If so
 *          it returns this address, else 0
 *
 * @param[in] iid   An EUI-64.
 *
 * @return  The short address on success, 0 on failure.
 */
static inline uint16_t sixlowpan_lowpan_eui64_to_short_addr(const net_if_eui64_t *iid)
{
    if (iid->uint32[0] == HTONL(0x000000ff) &&
        iid->uint16[2] == HTONS(0xfe00)) {
        return NTOHS(iid->uint16[3]);
    }

    return 0;
}

/**
 * @brief   Initializes all addresses and prefixes on an interface needed
 *          for 6LoWPAN. Calling this function together with
 *          sixlowpan_lowpan_init_interface() is not necessary.
 *
 * @param[in] if_id     The interface to use with 6LoWPAN.
 * @param[in] prefix    the address prefix to advertise.
 *
 * @return  1 on success, 0 on failure.
 */
int sixlowpan_lowpan_init_adhoc_interface(int if_id,
        const ipv6_addr_t *prefix);

/**
 * @brief   Initializes a 6LoWPAN border router with an address
 *
 * @note    Currently only working with addresses generated from
 *          IEEE 802.15.4 16-bit short addresses.
 *
 * @param[in] if_id                 The interface to use with 6LoWPAN.
 *
 * @return  1 on success, 0 on failure.
 */
int sixlowpan_lowpan_border_init(int if_id);

/**
 * @brief   Send data via 6LoWPAN to destination node or next hop dest.
 *
 * @param[in] if_id     The interface to send the data over.
 * @param[in] dest      Hardware address of the next hop or destination node.
 * @param[in] dest_len  Length of the destination address in byte.
 * @param[in] data      Data to send to destination node (may be
 *                      manipulated).
 * @param[in] data_len  Length of data.
 *
 * @return  length of transmitted data on success, -1 on failure.
 */
int sixlowpan_lowpan_sendto(int if_id, const void *dest, int dest_len,
                            uint8_t *data, uint16_t data_len);

/**
 * @brief   Set header compression status for 6LoWPAN.
 *
 * @param[in] status    Header compression status to set to.
 */
void sixlowpan_lowpan_set_iphc_status(
    sixlowpan_lowpan_iphc_status_t status);

/**
 * @brief   Initialize 6LoWPAN neighbor discovery (i.e. send
 *          router advertisement with Source Link-Layer Address Option)
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861">
 *          RFC 4861
 *      </a>, <a href="http://tools.ietf.org/html/rfc6775">
 *          RFC 6775
 *      </a>
 *
 */
void sixlowpan_lowpan_bootstrapping(void);

/**
 * @brief   Registers a thread to read received 6LoWPAN frames. The
 *          6LoWPAN frames are delivered as sixlowpan_lowpan_frame_t
 *          structs.
 *
 * @param[in] pid   The PID of the receiver thread.
 *
 * @return  1 on success, ENOMEM if maximum number of registrable
 *          threads is exceeded.
 */
uint8_t sixlowpan_lowpan_register(kernel_pid_t pid);

#if ENABLE_DEBUG
/**
 * @brief   Print current buffer of assembled (i. e. not fragmented)
 *          6LoWPAN packages.
 */
void sixlowpan_lowpan_print_fifo_buffers(void);

/**
 * @brief   Print current buffer for 6LoWPAN fragmentation reassembly.
 */
void sixlowpan_lowpan_print_reassembly_buffers(void);
#endif

/**
 * @brief   Initializes 6LoWPAN module.
 *
 * @return  1 on success, 0 on failure.
 */
int sixlowpan_lowpan_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SIXLOWPAN_LOWPAN_H */
/** @} */
