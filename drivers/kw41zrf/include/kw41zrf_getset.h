/*
 * Copyright (C) 2017 SKF AB
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_kw41zrf
 * @{
 *
 * @file
 * @brief       get/set interfaces for kw41zrf driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stdint.h>
#include "kw41zrf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Internal device option flags
 * @{
 */
#define KW41ZRF_OPT_CCA_BEFORE_TX   (0x01u) /**< cca before tx active */
#define KW41ZRF_OPT_PROMISCUOUS     (0x02u) /**< promiscuous mode active */
#define KW41ZRF_OPT_PRELOADING      (0x04u) /**< preloading enabled */
#define KW41ZRF_OPT_AUTOACK         (0x80u) /**< automatic sending of ACKs */
#define KW41ZRF_OPT_ACK_PENDING     (0x81u) /**< set pending bit on auto ACKs */
/** @} */

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
int16_t kw41zrf_get_tx_power(kw41zrf_t *dev);

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
 * @brief   Set if the kw41zrf device is a coordinator
 *
 * @param[in] is_coord  is device a coordinator
 */
void kw41zrf_set_pan_coord(uint8_t is_coord);

/**
 * @brief   Get PAN ID of given device
 *
 * @param[in] dev       kw41zrf device descriptor
 *
 * @return              current PAN ID
 */
uint16_t kw41zrf_get_pan(kw41zrf_t *dev);

/**
 * @brief   Set short address of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] addr      short address
 */
void kw41zrf_set_addr_short(kw41zrf_t *dev, const network_uint16_t *addr);

/**
 * @brief   Set long address of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] addr      long address
 */
void kw41zrf_set_addr_long(kw41zrf_t *dev, const eui64_t *addr);

/**
 * @brief   Get short address of a given device
 *
 * @param[in]  dev       kw41zrf device descriptor
 * @param[out] addr      current short address
 */
void kw41zrf_get_addr_short(kw41zrf_t *dev, network_uint16_t *addr);

/**
 * @brief   Get long address of a given device
 *
 * @param[in]  dev       kw41zrf device descriptor
 * @param[out] addr      current long address
 */
void kw41zrf_get_addr_long(kw41zrf_t *dev, eui64_t *addr);

/**
 * @brief   Get CCA threshold of a given device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @return              current CCA threshold
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
 * @brief   Set CCA type of a kw41zrf device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] type      CCA type
 */
void kw41zrf_set_cca_type(kw41zrf_t *dev, uint8_t type);

/**
 * @brief   Get CCA type of a kw41zrf device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @return              current CCA type
 */
uint8_t kw41zrf_get_cca_type(kw41zrf_t *dev);

/**
 * @brief Set CCA3 mode of a kw41zrf device
 *
 * @param[in] mode      CCA3 Mode   0: energy detection AND carrier sensing
 *                                  1: energy detection OR carrier sensing
 */
void kw41zrf_set_cca3_mode(uint8_t mode);

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
 * @brief Set receive watermark to signal when the packet buffer is part full
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
void kw41zrf_set_option(kw41zrf_t *dev, uint8_t option, uint8_t state);

/**
 * @brief Get the first free index of the Source Address Present partition_0
 *
 * @return first non-valid index
 */
uint8_t kw41zrf_get_1st_free_idx_sap0(void);

/**
 * @brief Enable or disable a partition
 *
 * @param[in] en            Enable (1) or disable (0)
 * @param[in] partition_shift Bit position for the partition
 */
void kw41zrf_set_partition_enable(uint8_t en, uint8_t partition_shift);

/**
 * @brief Get partition enable status
 *
 * @param[in] partition_mask  Mask for the partition bits
 * @param[in] partition_shift Bit position for the partition
 *
 * @return Partition enable status
 */
uint8_t kw41zrf_get_partition_enable(uint32_t partition_mask, uint8_t partition_shift);

/**
 * @brief Get partition start value
 *
 * @param[in] partition_mask  Mask for the partition bits
 * @param[in] partition_shift Bit position for the partition
 *
 * @return Partition start value
 */
uint8_t kw41zrf_get_partition_start(uint32_t partition_mask, uint8_t partition_shift);

/**
 * @brief Get link quality indicator(lqi) value
 *
 * @param[in] dev   kw41zrf device descriptor
 *
 * @return          lqi value
 */
inline uint8_t kw41zrf_get_lqi_value(kw41zrf_t *dev)
{
    (void) dev;
    return (ZLL->LQI_AND_RSSI & ZLL_LQI_AND_RSSI_LQI_VALUE_MASK) >> ZLL_LQI_AND_RSSI_LQI_VALUE_SHIFT;
}

/**
 * @brief Get received signal strength indicator(rssi) value
 *
 * @param[in] dev   kw41zrf device descriptor
 *
 * @return          rssi value
 */
inline int8_t kw41zrf_get_rssi_value(kw41zrf_t *dev)
{
    (void) dev;
    return (int8_t)((ZLL->LQI_AND_RSSI & ZLL_LQI_AND_RSSI_RSSI_MASK)
                           >> ZLL_LQI_AND_RSSI_RSSI_SHIFT);
}

#ifdef __cplusplus
}
#endif

/** @} */
