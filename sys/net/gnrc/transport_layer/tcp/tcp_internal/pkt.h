/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GNRC_TCP_INTERNAL_PKT_H_
#define GNRC_TCP_INTERNAL_PKT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TODO
 */
tcp_hdr_t* _pkt_get_tcp_hdr(gnrc_pktsnip_t* pkt);

/**
 * @brief Build a reset packet from an incomming packet.
 *
 * This function builds a reset from an incomming packet 
 * for cases where the connection has not been established
 * 
 * @param[out] out_pkt    outgoing reset packet
 * @param[in]  in_pkt     incomming packet
 * @return                Zero on success
 * @return                -ENOMEM if pktbuf is full.
 */
int8_t _pkt_build_reset_from_pkt(gnrc_pktsnip_t** out_pkt, gnrc_pktsnip_t* in_pkt);


/**
 * @brief Build and allocate a tcp paket in paketbuffer, tcb stores pointer to new paket.
 *
 * @param[in]  tcb           transmission control block that contains segment information
 * @param[out] out_pkt       Pointer to buildt packet
 * @param[out] seq_con       Number of Bytes the packet would consume in sequence number spce
 * @param[in]  ctl           control bits to set in pkt
 * @param[in]  seq_num       sequence number to use in new paket
 * @param[in]  ack_num       acknowledgment number to use in new paket
 * @param[in]  payload       pointer to payload buffer
 * @param[in]  payload_len   payload size
 *
 * @return                  Zero on success.
 * @return                  -ENOMEM if pktbuf is full.
 */
int8_t _pkt_build(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t** out_pkt, uint16_t *seq_con,
                   const uint16_t ctl, const uint32_t seq_num, const uint32_t ack_num,
                   uint8_t *payload, const size_t payload_len);

/**
 * TODO
 */
int8_t _pkt_send(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *out_pkt, const uint16_t seq_con);

/**
 * TODO
 */
int8_t _pkt_chk_seq_num(gnrc_tcp_tcb_t *tcb, uint32_t snum, uint32_t slen);

/**
 * TODO
 */
uint32_t _pkt_get_seg_len(gnrc_pktsnip_t *pkt);

/**
 * TODO
 */
int8_t _pkt_setup_retransmit(gnrc_tcp_tcb_t* tcb, gnrc_pktsnip_t* pkt);

/**
 * TODO
 */
void _pkt_acknowledge(gnrc_tcp_tcb_t* tcb, uint32_t ack);

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

#endif
