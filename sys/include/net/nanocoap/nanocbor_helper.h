/*
 * Copyright (c) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_nanocoap_nanocbor_helper NanoCBOR encoder helper for CoAP block-wise
 * @ingroup     net_nanocoap
 * @brief       Nanocbor helpers for CoAP block-wise
 *
 * This module wraps NanoCoAP and gcoap around the NanoCBOR stream-like interface to generate single
 * packets and sliced block-wise payloads automatically from nanocbor encoder serialization calls.
 * The content is serialized for every individual (block-wise) request. This module supports single
 * packets (without block-wise option) and block1/block2 payloads and will automatically update the
 * etag option in the packet if a dummy etag header has been placed.
 *
 * This module assumes that the serialized content does not change between successive
 * serializations. In case this happens it should be detected via the etag that can be included in
 * the packet.
 *
 * ## Usage
 *
 * 1. Initialize a regular packet with a block-wise option and all the other options required to be
 * included. Optionally include a dummy etag value of at least 4 bytes when the payload might change
 * between successive CoAP packets. Wrap up with a call to @ref coap_opt_finish :
 *
 * ```
 * ssize_t plen = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);
 * ```
 *
 * 2. Allocate both a `nanocbor_encoder_t` and the @ref coap_nanocbor_slicer_helper_t and initialize
 * them:
 * ```
 * nanocbor_encoder_t enc;
 * coap_nanocbor_slicer_helper_t helper;
 *
 * coap_nanocbor_slicer_helper_init(&helper, pdu, slicer);
 * coap_nanocbor_encoder_blockwise_init(&enc, &helper);
 * ```
 *
 * 3. Serialize the required CBOR structure via NanoCBOR encoder calls:
 * ```
 * nanocbor_fmt_uint(&enc, 1);
 * ```
 *
 * 4. After encoding the full CBOR structure, wrap up the block-wise payload and return from the
 * CoAP handler:
 * ```
 * return plen + coap_nanocbor_block2_finish(pdu, &helper);
 * ```
 * This finishes the block-wise structure and writes the generated etag into the CoAP option.
 *
 * If a simple payload is required without supporting block-wise, the slicer does not have to be
 * initialized and a `NULL` pointer can be passed in the @ref coap_nanocbor_slicer_helper_init call.
 * In this case only the available payload space will be used, possibly truncating the CBOR
 * structure.
 *
 * @{
 *
 * @file
 * @brief       NanoCBOR for CoAP API
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef NET_NANOCOAP_NANOCBOR_HELPER_H
#define NET_NANOCOAP_NANOCBOR_HELPER_H

#include <stdint.h>
#include "net/coap.h"
#include "checksum/fletcher32.h"
#include "nanocbor/nanocbor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Length of the ETAG used, matches the size of the fletcher32 checksum
 */
#define COAP_NANOCBOR_ETAG_LEN  4

/**
 * @brief CoAP nanocbor slicer helper struct
 *
 * Contains all the data needed to generate individual CoAP block-wise slices
 */
typedef struct {
    coap_block_slicer_t *slicer;    /**< coap block-wise slicer struct */
    uint8_t *buf;                   /**< Buffer to slice into, usually the CoAP payload */
    union {
        size_t full_len;            /**< Total length of the serialized response */
        size_t buf_len;             /**< Total available space in the payload buf */
    };
    size_t payload_len;           /**< Number of bytes written into the current slice */
    uint8_t fletcher_tmp;           /**< Temporary storage for half a fletcher32 words */
    fletcher32_ctx_t fletcher_ctx;  /**< Fletcher32 context for etag generation */
} coap_nanocbor_slicer_helper_t;

/**
 * @brief Initialize the CoAP NanoCBOR slicer helper struct with the payload buffer and the CoAP
 * slicer
 *
 * @param helper    CoAP NanoCBOR slicer helper to initialize
 * @param pdu       CoAP packet to initialize the helper for
 * @param slicer    Block wise slicer used with the CoAP packet
 */
void coap_nanocbor_slicer_helper_init(coap_nanocbor_slicer_helper_t *helper, const coap_pkt_t *pdu,
                                      coap_block_slicer_t *slicer);

/**
 * @brief Initialize the NanoCBOR encoder with the @ref coap_nanocbor_slicer_helper_t
 *
 * @param encoder   NanoCBOR encoder context to initialize
 * @param helper    CoAP NanoCBOR slicer helper
 */
void coap_nanocbor_encoder_init(nanocbor_encoder_t *encoder,
                                coap_nanocbor_slicer_helper_t *helper);

/**
 * @brief Finish the CoAP reply packet with the block2 option.
 *
 * Finishes the block2 option in the packet and writes the correct etag into the option if such
 * option is already present in the packet.
 *
 * @param pdu       CoAP packet to finish
 * @param helper    CoAP NanoCBOR slicer helper
 *
 * @returns The full length of the CoAP packet
 * @returns Negative on error
 */
ssize_t coap_nanocbor_block2_finish(coap_pkt_t *pdu, coap_nanocbor_slicer_helper_t *helper);

/**
 * @brief Finish the CoAP request packet with the block1 option.
 *
 * Finishes the block1 option in the packet and writes the correct etag into the option if such
 * option is already present in the packet.
 *
 * @param pdu       CoAP packet to finish
 * @param helper    CoAP NanoCBOR slicer helper
 *
 * @returns The full length of the CoAP packet
 * @returns Negative on error
 */
ssize_t coap_nanocbor_block1_finish(coap_pkt_t *pdu, coap_nanocbor_slicer_helper_t *helper);

/**
 * @brief Finish the CoAP request packet without any block option
 *
 * Writes the correct etag into the option if such option is already present in the packet
 *
 * @param pdu       CoAP packet to finish
 * @param helper    CoAP NanoCBOR slicer helper
 *
 * @returns The full length of the CoAP packet
 * @returns Negative on error
 */
ssize_t coap_nanocbor_finish(coap_pkt_t *pdu, coap_nanocbor_slicer_helper_t *helper);
#ifdef __cplusplus
}
#endif
#endif /* NET_NANOCOAP_NANOCBOR_HELPER_H */
/** @} */
