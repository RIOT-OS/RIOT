/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tcp TCP
 * @ingroup     net_gnrc
 * @brief       RIOT's tcp implementation for the gnrc stack
 *
 * @{
 *
 * @file
 * @brief       TCP paket handling declarations
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef GNRC_TCP_INTERNAL_PKT_H
#define GNRC_TCP_INTERNAL_PKT_H

#include <stdint.h>
#include "net/gnrc/pkt.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Build a reset packet from an incomming packet.
 *
 * This function builds a reset from an incomming packet
 * for cases where the connection has not been established
 *
 * @param[out] out_pkt    outgoing reset packet
 * @param[in]  in_pkt     incomming packet
 *
 * @return                Zero on success
 * @return                -ENOMEM if pktbuf is full.
 */
int _pkt_build_reset_from_pkt(gnrc_pktsnip_t **out_pkt, gnrc_pktsnip_t *in_pkt);

/**
 * @brief Build and allocate a tcp paket, tcb stores pointer to new paket.
 *
 * @param[in,out] tcb        This connections transmission control block.
 * @param[out] out_pkt       Pointer to paket to build
 * @param[out] seq_con       Number of Bytes, the packet will consume in sequence number space
 * @param[in]  ctl           control bits to set in out_pkt
 * @param[in]  seq_num       sequence number of the new paket
 * @param[in]  ack_num       acknowledgment number of the new paket
 * @param[in]  payload       pointer to payload buffer
 * @param[in]  payload_len   payload size
 *
 * @return   Zero on success.
 * @return   -ENOMEM if pktbuf is full.
 */
int _pkt_build(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t **out_pkt, uint16_t *seq_con,
               const uint16_t ctl, const uint32_t seq_num, const uint32_t ack_num,
               void *payload, const size_t payload_len);

/**
 * @brief Sends a packet to the peer
 *
 * @param[in,out] tcb          This connections Transmission control block.
 * @param[in]     out_pkt      pointer to paket to send
 * @param[in]     seq_con      sequence number consumption of the paket to send
 * @param[in]     retransmit   is this a retransmission ?
 *
 * @return   Zero on success.
 * @return   -EINVAL if out_pkt was NULL
 */
int _pkt_send(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *out_pkt, const uint16_t seq_con,
              const bool retransmit);

/**
 * @brief Checks sequence number
 *
 * @param[in,out] tcb   This connections Transmission control block.
 * @param[in] seq_num   sequence number from the segment
 * @param[in] seg_len   length of a segments payload
 *
 * @return    Zero if the sequence number is invalid
 * @return    Non-zero if the sequence number is acceptable
 */
int _pkt_chk_seq_num(const gnrc_tcp_tcb_t *tcb, const uint32_t seq_num, const uint32_t seg_len);

/**
 * @brief Extracts the length of a segment
 *
 * @param[in] pkt   Packet to calculate payload length
 *
 * @return   number consumption in sequence number space
 */
uint32_t _pkt_get_seg_len(gnrc_pktsnip_t *pkt);

/**
 * @brief Calculates a segments payload length
 *
 * @param[in] pkt   Packet to calculate payload length
 *
 * @return   the segments payload length in bytes
 */
uint32_t _pkt_get_pay_len(gnrc_pktsnip_t *pkt);

/**
 * @brief Adds a paket to the retransmission mechanism
 *
 * @param[in,out] tcb      This connections Transmission control block.
 * @param[in] pkt          paket to add to the retransmission mechanism
 * @param[in] retransmit   Flag used to indicate that pkt is a retransmit.
 *
 * @return   Zero on success
 * @return   -ENOMEM if the retransmission queue is full
 * @return   -EINVAL if pkt is null
 */
int _pkt_setup_retransmit(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *pkt, const bool retransmit);

/**
 * @brief Acknowledges and removes packet from the retransmission mechanism
 *
 * @param[in,out] tcb   This connections Transmission control block.
 * @param[in] ack       Acknowldegment number used to acknowledge packets
 *
 * @return   Zero on success
 * @return   -ENODATA if there is nothing to acknowledge
 */
int _pkt_acknowledge(gnrc_tcp_tcb_t *tcb, const uint32_t ack);

/**
 * @brief Calculates checksum over payload, tcp-header and network layer header
 *
 * @param[in] hdr          gnrc_pktsnip_t to tcp-header
 * @param[in] pseudo_hdr   gnrc_pktsnip_t to network layer header
 * @param[in] payload      gnrc_pktsnip_t to payload
 *
 * @return                 non zero checksum if given network layer is supported
 * @return                 zero if given network layer is not supported
 */
uint16_t _pkt_calc_csum(const gnrc_pktsnip_t *hdr, const gnrc_pktsnip_t *pseudo_hdr,
                        const gnrc_pktsnip_t *payload);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_INTERNAL_PKT_H */
/** @} */
