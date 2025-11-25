/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief   Board specific configuration for all NRF24L01+ (NG) devices
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#include "board.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "kernel_defines.h"
#include "nrf24l01p_ng_constants.h"
#include "nrf24l01p_ng.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NRF24L01P_NG_PARAM_SPI
/**
 * @brief   SPI bus
 */
#define NRF24L01P_NG_PARAM_SPI              SPI_DEV(0)
#endif

#ifndef NRF24L01P_NG_PARAM_SPI_CLK
/**
 * @brief   SPI clock speed
 */
#define NRF24L01P_NG_PARAM_SPI_CLK          SPI_CLK_5MHZ
#endif

#ifndef NRF24L01P_NG_PARAM_CS
/**
 * @brief   SPI CS gpio pin
 */
#define NRF24L01P_NG_PARAM_CS               GPIO_UNDEF
#endif

#ifndef NRF24L01P_NG_PARAM_CE
/**
 * @brief   NRF24L01+ chip enable gpio pin
 */
#define NRF24L01P_NG_PARAM_CE               GPIO_UNDEF
#endif

#ifndef NRF24L01P_NG_PARAM_IRQ
/**
 * @brief   NRF24L01+ interrupt gpio pin
 */
#define NRF24L01P_NG_PARAM_IRQ              GPIO_UNDEF
#endif

#ifndef NRF24L01P_NG_PARAM_CRC_LEN
/**
 * @brief   Default CRC length
 *          @see nrf24l01p_ng_crc_t
 */
#define NRF24L01P_NG_PARAM_CRC_LEN          (NRF24L01P_NG_CRC_2BYTE)
#endif

#ifndef NRF24L01P_NG_PARAM_TX_POWER
/**
 * @brief   Default TX power
 *          @see nrf24l01p_ng_tx_power_t
 */
#define NRF24L01P_NG_PARAM_TX_POWER         (NRF24L01P_NG_TX_POWER_0DBM)
#endif

#ifndef NRF24L01P_NG_PARAM_DATA_RATE_LVL
/**
 * @brief   Default data rate
 *          @see nrf24l01p_ng_rfdr_t
 */
#define NRF24L01P_NG_PARAM_DATA_RATE        (NRF24L01P_NG_RF_DR_2MBPS)
#endif

#ifndef NRF24L01P_NG_PARAM_CHANNEL
/**
 * @brief   Default channel in [0; 124]
 */
#define NRF24L01P_NG_PARAM_CHANNEL          (4)
#endif

#ifndef NRF24L01P_NG_PARAM_MAX_RETRANSM
/**
 * @brief   Default number of retransmissions
 */
#define NRF24L01P_NG_PARAM_MAX_RETRANSM     (5)
#endif

#ifndef NRF24L01P_NG_PARAM_RETRANSM_DELAY
/**
 * @brief   Default retransmission delay
 *          @see nrf24l01p_ng_ard_t
 */
#define NRF24L01P_NG_PARAM_RETRANSM_DELAY   (NRF24L01P_NG_ARD_2750US)
#endif

#ifndef NRF24L01P_NG_PARAMS
/**
 * @brief Default NRF24L01+ device parameters
 */
#define NRF24L01P_NG_PARAMS    {                                               \
        .spi = NRF24L01P_NG_PARAM_SPI,                                         \
        .spi_clk = NRF24L01P_NG_PARAM_SPI_CLK,                                 \
        .pin_cs = NRF24L01P_NG_PARAM_CS,                                       \
        .pin_ce = NRF24L01P_NG_PARAM_CE,                                       \
        .pin_irq = NRF24L01P_NG_PARAM_IRQ,                                     \
        .config =       {                                                      \
            .cfg_crc = NRF24L01P_NG_PARAM_CRC_LEN,                             \
            .cfg_tx_power = NRF24L01P_NG_PARAM_TX_POWER,                       \
            .cfg_data_rate = NRF24L01P_NG_PARAM_DATA_RATE,                     \
            .cfg_channel = NRF24L01P_NG_PARAM_CHANNEL,                         \
            .cfg_max_retr = NRF24L01P_NG_PARAM_MAX_RETRANSM,                   \
            .cfg_retr_delay = NRF24L01P_NG_PARAM_RETRANSM_DELAY,               \
        }                                                                      \
}
#endif

/**
 * @brief   Static array that holds NRF24L01+ device configurations
 */
static const nrf24l01p_ng_params_t nrf24l01p_ng_params[] = {
    NRF24L01P_NG_PARAMS
};

/**
 * @brief   Number of NRF24L01+ device configurations
 */
#define NRF24L01P_NG_NUM                    ARRAY_SIZE(nrf24l01p_ng_params)

#ifdef __cplusplus
}
#endif

/** @} */
