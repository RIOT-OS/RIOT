/*
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 *
 * @file
 * @brief       get/set interfaces for kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef KW2XRF_GETSET_H
#define KW2XRF_GETSET_H

#include "kw2xrf_reg.h"
#include "kw2xrf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set tx power of given device
 *
 * @param[in] dev        kw2xrf device descriptor
 * @param[in] txpower    transmit power in dBm
 */
void kw2xrf_set_tx_power(kw2xrf_t *dev, int16_t txpower);

/**
 * @brief   Get tx power value of given device
 *
 * @param[in] dev       kw2xrf device descriptor
 *
 * @return              current tx power value
 */
uint16_t kw2xrf_get_txpower(kw2xrf_t *dev);

/**
 * @brief   Set channel of given  device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @param[in] val       channel
 */
int kw2xrf_set_channel(kw2xrf_t *dev, uint8_t val);

/**
 * @brief   Get channel of given device
 *
 * @param[in] dev       kw2xrf device descriptor
 *
 * @return              current channel
 */
uint8_t kw2xrf_get_channel(kw2xrf_t *dev);

/**
 * @brief   Abort current sequence of device
 *
 * @param[in] dev       kw2xrf device descriptor
 */
void kw2xrf_abort_sequence(kw2xrf_t *dev);

/**
 * @brief   Set idle sequence state of device
 *
 * @param[in] dev       kw2xrf device descriptor
 */
void kw2xrf_set_idle_sequence(kw2xrf_t *dev);

/**
 * @brief   Set sequence state of device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @param[in] seq       sequence
 */
void kw2xrf_set_sequence(kw2xrf_t *dev, kw2xrf_physeq_t seq);

/**
 * @brief   Set PAN ID of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @param[in] pan       PAN ID value
 */
void kw2xrf_set_pan(kw2xrf_t *dev, uint16_t pan);

/**
 * @brief   Set short address of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @param[in] addr      short address
 */
void kw2xrf_set_addr_short(kw2xrf_t *dev, uint16_t addr);

/**
 * @brief   Set long address of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @param[in] addr      long address
 */
void kw2xrf_set_addr_long(kw2xrf_t *dev, uint64_t addr);

/**
 * @brief   Get short address of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @return              current short address
 */
uint16_t kw2xrf_get_addr_short(kw2xrf_t *dev);

/**
 * @brief   Get long address of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @return              current long address
 */
uint64_t kw2xrf_get_addr_long(kw2xrf_t *dev);

/**
 * @brief   Get CCA threshold of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @return              current CCA threshold
 */
int8_t kw2xrf_get_cca_threshold(kw2xrf_t *dev);

/**
 * @brief   Set CCA threshold of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @param[in] value     CCA threshold
 */
void kw2xrf_set_cca_threshold(kw2xrf_t *dev, int8_t value);

/**
 * @brief   Set CCA mode of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @param[in] mode      CCA mode
 */
void kw2xrf_set_cca_mode(kw2xrf_t *dev, uint8_t mode);

/**
 * @brief   Get CCA mode of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @return              current CCA mode
 */
uint8_t kw2xrf_get_cca_mode(kw2xrf_t *dev);

/**
 * @brief   Get RSSI from LQI value
 *
 * @param[in] value     Current LQI value
 *
 * @return              deduced RSSI in dBm
 */
int16_t kw2xrf_get_rssi(uint32_t value);

/**
 * @brief   Get state of a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @return              current state
 */
netopt_state_t kw2xrf_get_status(kw2xrf_t *dev);

/**
 * @brief               Enable continuous CCA
 *
 * @param[in] dev       kw2xrf device descriptor
 *
 * @return              CCA value
 */
int kw2xrf_cca(kw2xrf_t *dev);

/**
 * @brief Set receive watermark
 *
 * @param[in] dev       kw2xrf device descriptor
 * @param[in] value     watermark
 */
void kw2xrf_set_rx_watermark(kw2xrf_t *dev, uint8_t value);

/**
 * @brief   Set netopt a given device
 *
 * @param[in] dev       kw2xrf device descriptor
 * @param[in] option    Netopt option type
 * @param[in] state     state
 */
void kw2xrf_set_option(kw2xrf_t *dev, uint16_t option, bool state);

#ifdef __cplusplus
}
#endif

#endif /* KW2XRF_GETSET_H */
/** @} */
