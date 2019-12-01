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
 * @brief   Mapping between frequencies and channels for the
 *          NRF24L01P device driver
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_CHANNELS_H
#define NRF24L01P_CHANNELS_H

#include "nrf24l01p_internal.h"
#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Supported number of frequency channels
 */
#define NRF24L01P_NUM_CHANNELS              (16)

/**
 * @brief   Table that maps a channel to a 2.4Ghz offset
 */
extern const uint8_t vchanmap[NRF24L01P_NUM_CHANNELS];

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_CHANNELS_H */
/** @} */
