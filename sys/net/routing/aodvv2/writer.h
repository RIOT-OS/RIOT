/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     aodvv2
 * @{
 *
 * @file        writer.h
 * @brief       writer to create RFC5444 aodvv2 messages
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifndef AODVV2_WRITER_H_
#define AODVV2_WRITER_H_

#include "common/netaddr.h"
#include "rfc5444/rfc5444_writer.h"
#include "mutex.h"

#include "constants.h"
#include "seqnum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Wrapper for the rfc5444_writer_target that the _write_packet() callback receives.
 *          _write_packet() needs to know the type, payload and target address
 *          of the RFC5444 message to be sent as well, but the oonf api does not
 *          offer this feature. Having this wrapper enables the use of the
 *          container_of macro to fetch this information.
 *          It is hacky, but it does the trick.
 */
struct writer_target
{
    struct rfc5444_writer_target interface;     /**< Interface for generating rfc5444 packets */
    struct netaddr target_addr;                 /**< Address to which the packet should be sent */
    struct aodvv2_packet_data packet_data;      /**< Payload of the AODVv2 Message */
    int type;                                   /**< Type of the AODVv2 Message (i.e. rfc5444_msg_type) */
};

/**
 * @brief   oonf api voodo. Pointer to a callback function which is passed to
 *          writer_init() and called when the packet is ready to send.
 */
typedef void (*write_packet_func_ptr)(
    struct rfc5444_writer *wr, struct rfc5444_writer_target *iface,
    void *buffer, size_t length);

/**
 * @brief   Initialize RFC5444 writer
 * @param ptr   pointer to "send_packet" callback
 */
void aodv_packet_writer_init(write_packet_func_ptr ptr);

/**
 * @brief   Clean up after the RFC5444 writer
 */
void aodv_packet_writer_cleanup(void);

/**
 * @brief   Send a RREQ. DO NOT use this function to dispatch packets from anything else
 *          than the sender_thread. To send RREQs, use aodv_send_rreq().
 * @param packet_data      parameters of the RREQ
 * @param next_hop Address the RREP is sent to
 */
void aodv_packet_writer_send_rreq(struct aodvv2_packet_data *packet_data, struct netaddr *next_hop);

/**
 * @brief   Send a RREP. DO NOT use this function to dispatch packets from anything else
 *          than the sender_thread. To send RREPs, use aodv_send_rrep().
 * @param packet_data parameters of the RREP
 * @param next_hop Address the RREP is sent to
 */
void aodv_packet_writer_send_rrep(struct aodvv2_packet_data *packet_data, struct netaddr *next_hop);

/**
 * @brief   Send a RERR. DO NOT use this function to dispatch packets from anything else
 *          than the sender_thread. To send RERRs, use aodv_send_rerr().
 * @param unreachable_nodes[] array containing all newly unreachable nodes. each
 *                            in a struct unreachable_node
 * @param len                 length of unreachable_nodes[]
 * @param hoplimit            the message's hop limit
 * @param next_hop            Address the RREP is sent to
 */
void aodv_packet_writer_send_rerr(struct unreachable_node unreachable_nodes[], size_t len,
                      int hoplimit, struct netaddr *next_hop);

#ifdef  __cplusplus
}
#endif

#endif /* AODVV2_WRITER_H_ */
