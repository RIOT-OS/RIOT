/*
 * Copyright (C) 2018 Baptiste Cartier
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc430
 * @{
 *
 * @file
 * @brief       Getters and setter for the cc430
 *
 * @author      Baptiste Cartier
 */

#ifndef CC430_RF_GETSET_H
#define CC430_RF_GETSET_H

#include <stdint.h>

#include "cc430_rf_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t cc430_get_max_packet_size(void);
int8_t cc430_set_channel(cc110x_t *dev, uint8_t channr);
uint8_t cc430_get_radio_state(void);
uint8_t cc430_set_address(cc110x_t *dev, uint8_t address);

#ifdef __cplusplus
}
#endif

#endif /* CC430_RF_GETSET_H */
/** @} */
