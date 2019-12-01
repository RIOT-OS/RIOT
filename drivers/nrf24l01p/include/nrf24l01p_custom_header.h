/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief   Conversion functions to convert between enums and real values
 *          of the NRF24L01P device driver
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_CUSTOM_HEADER_H
#define NRF24L01P_CUSTOM_HEADER_H

#include <stdint.h>

#include "kernel_defines.h"
#include "nrf24l01p_constants.h"
#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_GNRC_SIXLOWPAN)
/**
 * @brief   Put source layer-2 address in custom header to
 *          be able to use 6LoWPAN
 */
#define NRF24L01P_CUSTOM_HEADER             (1)
/**
 * @brief   Layer-2 MTU
 */
#define NRF24L01P_MTU                       (NRF24L01P_MAX_PAYLOAD_WIDTH - \
                                             (1 + NRF24L01P_MAX_ADDR_WIDTH))
#else
/**
 * @brief   Do not put source layer-2 address in custom header
 */
#define NRF24L01P_CUSTOM_HEADER             (0)
/**
 * @brief Layer-2 MTU
 */
#define NRF24L01P_MTU                       NRF24L01P_MAX_PAYLOAD_WIDTH
#endif /* IS_USED(MODULE_GNRC_SIXLOWPAN) */

#if NRF24L01P_CUSTOM_HEADER
/**
 * @brief Indicate header byte
 */
#define NRF24L01P_CSTM_HDR_PREEMBLE       (0b10000000)
/**
 * @brief Mask of source address width
 */
#define NRF24L01P_CSTM_HDR_SRC_AW_MASK    (0b00000111)
/**
 * @brief Padding byte for ShockBurst
 */
#define NRF24L01P_CSTM_HDR_PADDING        (0x00)
#endif /* NRF24L01P_CUSTOM_HEADER */
/**
 * @brief Mask of destination address width
 */
#define NRF24L01P_CSTM_HDR_DST_AW_MASK    (0b00111000)

/**
 * @brief   Header of a ShockBurst frame
 */
typedef struct {
    /**
     * @brief    7 6 5 4 3 2 1 0
     *          +-+-+-+-+-+-+-+-+
     *          |1|r|d|d|d|s|s|s|
     *          +-+-+-+-+-+-+-+-+
     *          r = reserved
     *          d = destination address width
     *          s = source address width
     * If the protocol is ShockBurst, received frames may
     * be padded due to static payload length. The 7th bin
     * in the hader must be 1. Leading bytes starting with
     * 0 are padding bytes and must be removed.
     * Any frame having a value of ddd or sss not being in
     * [NRF24L01P_MIN_ADDR_WIDTH; NRF24L01P_MAX_ADDR_WIDTH]
     * shall be dropped.
     */
    uint8_t addr_width;
    /**
     * @brief   Recipient address
     */
    uint8_t dst_addr[NRF24L01P_MAX_ADDR_WIDTH];
#if NRF24L01P_CUSTOM_HEADER
    /**
     * @brief   Sender address
     */
    uint8_t src_addr[NRF24L01P_MAX_ADDR_WIDTH];
#endif
} shockburst_hdr_t;

/**
 * @brief   Set custom header preemble
 *
 * @param[out] hdr      SB header
 */
static inline void sb_hdr_init(shockburst_hdr_t *hdr)
{
#if NRF24L01P_CUSTOM_HEADER
    hdr->addr_width = NRF24L01P_CSTM_HDR_PREEMBLE;
#else
    hdr->addr_width = 0;
#endif
}

/**
 * @brief   Set destination address field in a ShockBurst header
 *
 * @param[out] hdr      SB header
 * @param[in] width     Destination address width
 */
static inline void sb_hdr_set_dst_addr_width(shockburst_hdr_t *hdr,
                                             uint8_t width)
{
    hdr->addr_width &= ~(NRF24L01P_CSTM_HDR_DST_AW_MASK);
    hdr->addr_width |= ((width  << 3) & NRF24L01P_CSTM_HDR_DST_AW_MASK);
}

/**
 * @brief   Get destination address field in a ShockBurst header
 *
 * @param[out] hdr      SB header
 *
 * @return              Destination address width
 */
static inline uint8_t sb_hdr_get_dst_addr_width(shockburst_hdr_t *hdr)
{
    return (hdr->addr_width & NRF24L01P_CSTM_HDR_DST_AW_MASK) >> 3;
}

#if NRF24L01P_CUSTOM_HEADER
/**
 * @brief   Set source address field in a ShockBurst header
 *
 * @param[out] hdr      SB header
 * @param[in] width     Source address width
 */
static inline void sb_hdr_set_src_addr_width(shockburst_hdr_t *hdr,
                                             uint8_t width)
{
    hdr->addr_width &= ~(NRF24L01P_CSTM_HDR_SRC_AW_MASK);
    hdr->addr_width |= (width & NRF24L01P_CSTM_HDR_SRC_AW_MASK);
}

/**
 * @brief   Get source address field in a ShockBurst header
 *
 * @param[out] hdr      SB header
 *
 * @return              Source address width
 */
static inline uint8_t sb_hdr_get_src_addr_width(shockburst_hdr_t *hdr)
{
    return hdr->addr_width & NRF24L01P_CSTM_HDR_SRC_AW_MASK;
}
#endif /* NRF24L01P_CUSTOM_HEADER */

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_CUSTOM_HEADER_H */
/** @} */
