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
 * @brief   Constants from the datasheet of the NRF24L01P transceiver
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_CONSTANTS_H
#define NRF24L01P_CONSTANTS_H

#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum width of a payload, restricted by
 *          a FIFO size of 32 bytes
 */
#define NRF24L01P_MAX_PAYLOAD_WIDTH         (32)

/**
 * @brief   Maximum number of retransmissions, if
 *          ESB is used as protocol
 */
#define NRF24L01P_MAX_RETRANSMISSIONS       (15)

/**
 * @brief   Base frequency
 */
#define NRF24L01P_BASE_FRQ_MHZ              (2400)

/**
 * @brief   Maximum supported frequency
 */
#define NRF24L01P_MAX_FRQ_MHZ               (2525)

/**
 * @brief   Reset value of TX_ADDR register
 */
#define NRF24L01P_DEFAULT_TX_ADDR           { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 }

/**
 * @brief   Reset value of RX_ADDR_P0 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P0         { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 }

/**
 * @brief   Reset value of RX_ADDR_P1 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P1         { 0xC2, 0xC2, 0xC2, 0xC2, 0xC2 }

/**
 * @brief   Reset value of RX_ADDR_P2 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P2         (0xC3)

/**
 * @brief   Reset value of RX_ADDR_P3 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P3         (0xC4)

/**
 * @brief   Reset value of RX_ADDR_P4 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P4         (0xC5)

/**
 * @brief   Reset value of RX_ADDR_P5 register
 */
#define NRF24L01P_DEFAULT_L2ADDR_P5         (0xC6)

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_CONSTANTS_H */
/** @} */
