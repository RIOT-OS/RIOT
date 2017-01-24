/*
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup
 * @{
 *
 * @file
 * @brief       get/set interfaces for kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#include "kw2xrf.h"

#ifdef __cplusplus
extern "C" {
#endif

void kw2xrf_set_tx_power(kw2xrf_t *dev, int16_t txpower);

uint16_t kw2xrf_get_txpower(kw2xrf_t *dev);

uint8_t kw2xrf_get_channel(kw2xrf_t *dev);

int kw2xrf_set_channel(kw2xrf_t *dev, uint8_t val);

void kw2xrf_abort_sequence(kw2xrf_t *dev);

void kw2xrf_set_idle_sequence(kw2xrf_t *dev);

void kw2xrf_set_sequence(kw2xrf_t *dev, kw2xrf_physeq_t seq);

void kw2xrf_set_pan(kw2xrf_t *dev, uint16_t pan);

void kw2xrf_set_addr_short(kw2xrf_t *dev, uint16_t addr);

void kw2xrf_set_addr_long(kw2xrf_t *dev, uint64_t addr);

uint16_t kw2xrf_get_addr_short(kw2xrf_t *dev);

uint64_t kw2xrf_get_addr_long(kw2xrf_t *dev);

int8_t kw2xrf_get_cca_threshold(kw2xrf_t *dev);

void kw2xrf_set_cca_threshold(kw2xrf_t *dev, int8_t value);

void kw2xrf_set_cca_mode(kw2xrf_t *dev, uint8_t mode);

uint8_t kw2xrf_get_cca_mode(kw2xrf_t *dev);

uint32_t kw2xrf_get_rssi(uint32_t value);

netopt_state_t kw2xrf_get_status(kw2xrf_t *dev);

int kw2xrf_cca(kw2xrf_t *dev);

void kw2xrf_set_rx_watermark(kw2xrf_t *dev, uint8_t value);

void kw2xrf_set_option(kw2xrf_t *dev, uint16_t option, bool state);

#ifdef __cplusplus
}
#endif
/** @} */
