/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief   Constants from the datasheet of the NRF24L01+ (NG) transceiver
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_NG_CONSTANTS_H
#define NRF24L01P_NG_CONSTANTS_H

#include "nrf24l01p_ng.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum width of a payload, restricted by
 *          a FIFO size of 32 bytes
 */
#define NRF24L01P_NG_MAX_PAYLOAD_WIDTH      (32)

/**
 * @brief   Maximum number of retransmissions, if
 *          ESB is used as protocol
 */
#define NRF24L01P_NG_MAX_RETRANSMISSIONS    (15)

/**
 * @brief   Base frequency
 */
#define NRF24L01P_NG_BASE_FRQ_MHZ           (2400)

/**
 * @brief   Maximum supported frequency
 */
#define NRF24L01P_NG_MAX_FRQ_MHZ            (2525)

/**
 * @brief    Number of supported channels
 *
 * frequency range: 2400 - 2525 Mhz
 * occupied bandwidth for each channel:
 *  250 kbps: < 1Mhz
 *  1000 kbps: < 1Mhz
 *  2000 kbps: < 2Mhz
 *
 * -> spaces between each channel´s
 *    base frequency must be at least
 *    2 Mhz, to be sufficient for all
 *    data rates.
 */
#define NRF24L01P_NG_NUM_CHANNELS           (NRF24L01P_NG_MAX_FRQ_MHZ - \
                                            NRF24L01P_NG_BASE_FRQ_MHZ)

/**
 * @brief   Reset value of TX_ADDR register
 */
#define NRF24L01P_NG_DEFAULT_TX_ADDR        { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 }

/**
 * @brief   Reset value of RX_ADDR_P0 register
 */
#define NRF24L01P_NG_DEFAULT_ADDR_P0        { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 }

/**
 * @brief   Reset value of RX_ADDR_P1 register
 */
#define NRF24L01P_NG_DEFAULT_ADDR_P1        { 0xC2, 0xC2, 0xC2, 0xC2, 0xC2 }

/**
 * @brief   Reset value of RX_ADDR_P2 register
 */
#define NRF24L01P_NG_DEFAULT_ADDR_P2        (0xC3)

/**
 * @brief   Reset value of RX_ADDR_P3 register
 */
#define NRF24L01P_NG_DEFAULT_ADDR_P3        (0xC4)

/**
 * @brief   Reset value of RX_ADDR_P4 register
 */
#define NRF24L01P_NG_DEFAULT_ADDR_P4        (0xC5)

/**
 * @brief   Reset value of RX_ADDR_P5 register
 */
#define NRF24L01P_NG_DEFAULT_ADDR_P5        (0xC6)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NRF24L01P_NG_CONSTANTS_H */
