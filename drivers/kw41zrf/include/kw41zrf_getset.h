/*
 * Copyright (C) 2017 SKF AB
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_kw41zrf
 * @{
 *
 * @file
 * @brief       get/set interfaces for kw41zrf driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef KW41ZRF_GETSET_H
#define KW41ZRF_GETSET_H

#include "kw41zrf.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Transceiver sequence identifiers */
enum kw41zrf_xcvseq {
    XCVSEQ_IDLE             =  0b000,
    XCVSEQ_RECEIVE          =  0b001,
    XCVSEQ_TRANSMIT         =  0b010,
    XCVSEQ_CCA              =  0b011,
    XCVSEQ_TX_RX            =  0b100,
    XCVSEQ_CONTINUOUS_CCA   =  0b101,
    /* Other values are reserved */
    /* Special value for idle_seq when sleeping */
    XCVSEQ_DSM_IDLE         = 0b1000,
};

/**
 * @brief   Set tx power of given device
 *
 * @param[in] dev        kw41zrf device descriptor
 * @param[in] txpower    transmit power in dBm
 */
void kw41zrf_set_tx_power(kw41zrf_t *dev, int16_t txpower);

/**
 * @brief   Get tx power value of given device
 *
 * @param[in] dev       kw41zrf device descriptor
 *
 * @return              current tx power value
 */
uint16_t kw41zrf_get_txpower(kw41zrf_t *dev);

/**
 * @brief   Set channel of given  device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] val       channel
 */
int kw41zrf_set_channel(kw41zrf_t *dev, uint8_t val);

/**
 * @brief   Get channel of given device
 *
 * @param[in] dev       kw41zrf device descriptor
 *
 * @return              current channel
 */
uint8_t kw41zrf_get_channel(kw41zrf_t *dev);

/**
 * @brief   Set PAN ID of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] pan       PAN ID value
 */
void kw41zrf_set_pan(kw41zrf_t *dev, uint16_t pan);

/**
 * @brief   Set short address of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] addr      short address
 */
void kw41zrf_set_addr_short(kw41zrf_t *dev, uint16_t addr);

/**
 * @brief   Set long address of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] addr      long address
 */
void kw41zrf_set_addr_long(kw41zrf_t *dev, uint64_t addr);

/**
 * @brief   Get short address of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @return              current short address
 */
uint16_t kw41zrf_get_addr_short(kw41zrf_t *dev);

/**
 * @brief   Get long address of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @return              current long address
 */
uint64_t kw41zrf_get_addr_long(kw41zrf_t *dev);

/**
 * @brief   Get CCA threshhold of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @return              current CCA threshhold
 */
int8_t kw41zrf_get_cca_threshold(kw41zrf_t *dev);

/**
 * @brief   Set CCA threshold of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] value     CCA threshold
 */
void kw41zrf_set_cca_threshold(kw41zrf_t *dev, int8_t value);

/**
 * @brief   Set CCA mode of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] mode      CCA mode
 */
void kw41zrf_set_cca_mode(kw41zrf_t *dev, uint8_t mode);

/**
 * @brief   Get CCA mode of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @return              current CCA mode
 */
uint8_t kw41zrf_get_cca_mode(kw41zrf_t *dev);

/**
 * @brief   Get latest ED measurement from the device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @return              most recent ED level
 */
int8_t kw41zrf_get_ed_level(kw41zrf_t *dev);

/**
 * @brief               Perform one CCA measurement and return the result
 *
 * @param[in] dev       kw41zrf device descriptor
 *
 * @return              0 if channel is idle
 * @return              1 if channel is busy
 */
int kw41zrf_cca(kw41zrf_t *dev);

/**
 * @brief Set receive watermark
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] value     watermark
 */
void kw41zrf_set_rx_watermark(kw41zrf_t *dev, uint8_t value);

/**
 * @brief   Set netopt a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] option    Netopt option type
 * @param[in] state     state
 */
void kw41zrf_set_option(kw41zrf_t *dev, uint16_t option, bool state);

#ifdef __cplusplus
}
#endif

#endif /* KW41ZRF_GETSET_H */
/** @} */
