/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_tcp
 *
 * @{
 *
 * @file
 * @brief       TCP packet handling declarations.
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef GNRC_TCP_PKT_H
#define GNRC_TCP_PKT_H

#include <stdint.h>
#include "net/gnrc.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Build a reset packet from an incoming packet.
 *
 * @note This function builds a reset from an incoming packet
 *       in cases where the connection has not been established.
 *
 * @param[out] out_pkt    Outgoing reset packet
 * @param[in]  in_pkt     Incoming packet
 *
 * @returns   Zero on success
 *            -ENOMEM if pktbuf is full.
 */
int _gnrc_tcp_pkt_build_reset_from_pkt(gnrc_pktsnip_t **out_pkt,
                                       gnrc_pktsnip_t *in_pkt);

/**
 * @brief Build and allocate a TCB packet, TCB stores pointer to new packet.
 *
 * @param[in,out] tcb           TCB holding the connection information.
 * @param[out]    out_pkt       Pointer to packet to build.
 * @param[out]    seq_con       Sequence number consumption of built packet.
 * @param[in]     ctl           Control bits to set in @p out_pkt.
 * @param[in]     seq_num       Sequence number of the new packet.
 * @param[in]     ack_num       Acknowledgment number of the new packet.
 * @param[in]     payload       Pointer to payload buffer.
 * @param[in]     payload_len   Payload size.
 *
 * @returns   Zero on success.
 *            -ENOMEM if pktbuf is full.
 */
int _gnrc_tcp_pkt_build(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t **out_pkt,
                        uint16_t *seq_con, const uint16_t ctl,
                        const uint32_t seq_num, const uint32_t ack_num,
                        void *payload, const size_t payload_len);

/**
 * @brief Sends packet to peer.
 *
 * @param[in,out] tcb          TCB holding the connection information.
 * @param[in]     out_pkt      Pointer to packet to send.
 * @param[in]     seq_con      Sequence number consumption of the packet to send.
 * @param[in]     retransmit   Flag so mark that packet this is a retransmission.
 *
 * @returns   Zero on success.
 *            -EINVAL if out_pkt was NULL.
 */
int _gnrc_tcp_pkt_send(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *out_pkt,
                       const uint16_t seq_con, const bool retransmit);

/**
 * @brief Verify sequence number.
 *
 * @param[in,out] tcb       TCB holding the connection information.
 * @param[in]     seq_num   Sequence number from the segment.
 * @param[in]     seg_len   Length of a segments payload.
 *
 * @returns    Zero if the sequence number is acceptable.
 *             Negative value if the sequence number is not acceptable.
 */
int _gnrc_tcp_pkt_chk_seq_num(const gnrc_tcp_tcb_t *tcb, const uint32_t seq_num,
                              const uint32_t seg_len);

/**
 * @brief Extracts the length of a segment.
 *
 * @param[in] pkt   Packet to calculate the segments length.
 *
 * @returns   Segments length in bytes (== sequence number consumption).
 */
uint32_t _gnrc_tcp_pkt_get_seg_len(gnrc_pktsnip_t *pkt);

/**
 * @brief Calculates a packets payload length.
 *
 * @param[in] pkt   Packet to calculate payload length.
 *
 * @returns   The packets payload length in bytes.
 */
uint32_t _gnrc_tcp_pkt_get_pay_len(gnrc_pktsnip_t *pkt);

/**
 * @brief Adds a packet to the retransmission mechanism.
 *
 * @param[in,out] tcb          TCB holding the connection information.
 * @param[in]     pkt          Packet to add to the retransmission mechanism.
 * @param[in]     retransmit   Flag used to indicate that @p pkt is a retransmit.
 *
 * @returns   Zero on success.
 *            -ENOMEM if the retransmission queue is full.
 *            -EINVAL if pkt is null.
 */
int _gnrc_tcp_pkt_setup_retransmit(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *pkt,
                                   const bool retransmit);

/**
 * @brief Acknowledges and removes packet from the retransmission mechanism.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 * @param[in]     ack   Acknowldegment number used to acknowledge packets.
 *
 * @returns   Zero on success.
 *            -ENODATA if there is nothing to acknowledge.
 */
int _gnrc_tcp_pkt_acknowledge(gnrc_tcp_tcb_t *tcb, const uint32_t ack);

/**
 * @brief Calculates checksum over payload, TCP header and network layer header.
 *
 * @param[in] hdr          Gnrc_pktsnip_t to TCP header.
 * @param[in] pseudo_hdr   Gnrc_pktsnip_t to network layer header.
 * @param[in] payload      Gnrc_pktsnip_t to payload.
 *
 * @returns   Non-zero checksum if given network layer is supported.
 *            Zero if given network layer is not supported.
 */
uint16_t _gnrc_tcp_pkt_calc_csum(const gnrc_pktsnip_t *hdr,
                                 const gnrc_pktsnip_t *pseudo_hdr,
                                 const gnrc_pktsnip_t *payload);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_PKT_H */
/** @} */
