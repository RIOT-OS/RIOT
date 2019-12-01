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
#ifndef NRF24L01P_CONVERSION_H
#define NRF24L01P_CONVERSION_H

#include <stdint.h>

#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Convert @ref nrf24l01p_aw_t to actual address width
 *
 * @param[in] address_width Address width enum
 *
 * @return                  Address width in [bytes]
 */
static inline uint8_t nrf24l01p_etoval_aw(nrf24l01p_aw_t address_width)
{
    if (address_width <= NRF24L01P_AW_3BYTE) {
        return 3;
    }
    if (address_width == NRF24L01P_AW_4BYTE) {
        return 4;
    }
    return 5;
}

/**
 * @brief   Convert address width in [bytes] to @ref nrf24l01p_aw_t
 *
 * @param[in] address_width Address width in [bytes]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_aw_t nrf24l01p_valtoe_aw(uint8_t address_width)
{
    if (address_width <= 3) {
        return NRF24L01P_AW_3BYTE;
    }
    if (address_width == 4) {
        return NRF24L01P_AW_4BYTE;
    }
    return NRF24L01P_AW_5BYTE;
}

/**
 * @brief   Convert @ref nrf24l01p_ard_t to actual retransmission delay
 *
 * @param[in] retr_delay    Retransmission delay enum
 *
 * @return                  Retransmission delay in [us]
 */
static inline uint16_t nrf24l01p_etoval_ard(nrf24l01p_ard_t retr_delay)
{
    if (retr_delay >= NRF24L01P_ARD_4000US) {
        return 4000;
    }
    return (retr_delay + 1) * 250;
}

/**
 * @brief   Convert retransmission delay in [us] to @ref nrf24l01p_ard_t
 *
 * @param[in] retr_delay    Retransmission delay in [us]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_ard_t nrf24l01p_valtoe_ard(uint16_t retr_delay)
{
    if (retr_delay >= 4000) {
        return NRF24L01P_ARD_4000US;
    }
    return (nrf24l01p_ard_t)(retr_delay / 250);
}

/**
 * @brief   Convert @ref nrf24l01p_crc_t to actual CRC length
 *
 * @param[in] crc_len       CRC length enum
 *
 * @return                  CRC length in [bytes]
 */
static inline uint8_t nrf24l01p_etoval_crc(nrf24l01p_crc_t crc_len)
{
    if (crc_len <= NRF24L01P_CRC_0BYTE) {
        return 0;
    }
    if (crc_len == NRF24L01P_CRC_1BYTE) {
        return 1;
    }
    return 2;
}

/**
 * @brief   Convert CRC length in [bytes] to @ref nrf24l01p_crc_t
 *
 * @param[in] crc_len       CRC length in [bytes]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_crc_t nrf24l01p_valtoe_crc(uint8_t crc_len)
{
    if (!crc_len) {
        return NRF24L01P_CRC_0BYTE;
    }
    if (crc_len == 1) {
        return NRF24L01P_CRC_1BYTE;
    }
    return NRF24L01P_CRC_2BYTE;
}

/**
 * @brief   Convert @ref nrf24l01p_tx_power_t to actual Tx power
 *
 * @param[in] power         RF power enum
 *
 * @return                  RF power in [dbm]
 */
static inline int8_t nrf24l01p_etoval_tx_power(nrf24l01p_tx_power_t power)
{
    if (power <= NRF24L01P_TX_POWER_MINUS_18DBM) {
        return -18;
    }
    if (power == NRF24L01P_TX_POWER_MINUS_12DBM) {
        return -12;
    }
    if (power == NRF24L01P_TX_POWER_MINUS_6DBM) {
        return -6;
    }
    return 0;
}

/**
 * @brief   Convert RF power in [dbm] to @ref nrf24l01p_tx_power_t
 *
 * @param[in] power         RF power in [dbm]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_tx_power_t nrf24l01p_valtoe_tx_power(int16_t power)
{
    if (power <= -18) {
        return NRF24L01P_TX_POWER_MINUS_18DBM;
    }
    if (power <= -12) {
        return NRF24L01P_TX_POWER_MINUS_12DBM;
    }
    if (power <= -6) {
        return NRF24L01P_TX_POWER_MINUS_6DBM;
    }
    return NRF24L01P_TX_POWER_0DBM;
}

/**
 * @brief   Convert @ref nrf24l01p_rfdr_t to actual air data rate
 *
 * @param[in] data_rate     Air data rate enum
 *
 * @return                  Air data rate in [kbit/s]
 */
static inline uint16_t nrf24l01p_etoval_rfdr(nrf24l01p_rfdr_t data_rate)
{
    if (data_rate <= NRF24L01P_RF_DR_1MBPS) {
        return 1000;
    }
    if (data_rate == NRF24L01P_RF_DR_250KBPS) {
        return 250;
    }
    return 2000;
}

/**
 * @brief   Convert Air data rate in [kbit/s] to @ref nrf24l01p_rfdr_t
 *
 * @param[in] data_rate     Air data rate in [kbit/s]
 *
 * @return                  Corresponding enum
 */
static inline nrf24l01p_rfdr_t nrf24l01p_valtoe_rfdr(uint16_t data_rate)
{
    if (data_rate <= 250) {
        return NRF24L01P_RF_DR_250KBPS;
    }
    if (data_rate <= 1000) {
        return NRF24L01P_RF_DR_1MBPS;
    }
    return NRF24L01P_RF_DR_2MBPS;
}

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_CONVERSION_H */
/** @} */
