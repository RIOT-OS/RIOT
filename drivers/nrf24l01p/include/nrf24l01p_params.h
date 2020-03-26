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
 * @brief   Board specific configuration for all connected NRF24L01P devices
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_PARAMS_H
#define NRF24L01P_PARAMS_H

#include "board.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "kernel_defines.h"
#include "nrf24l01p_constants.h"
#include "nrf24l01p_internal.h"
#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NRF24L01P_PARAM_SPI
/**
 * @brief   SPI bus
 */
#define NRF24L01P_PARAM_SPI                 SPI_DEV(0)
#endif

#ifndef NRF24L01P_PARAM_SPI_CLK
/**
 * @brief   SPI clock speed
 */
#define NRF24L01P_PARAM_SPI_CLK             SPI_CLK_5MHZ
#endif

#ifndef NRF24L01P_PARAM_CS
/**
 * @brief   SPI CS gpio pin
 */
#define NRF24L01P_PARAM_CS                  GPIO_UNDEF
#endif

#ifndef NRF24L01P_PARAM_CE
/**
 * @brief   NRF24L01P chip enable gpio pin
 */
#define NRF24L01P_PARAM_CE                  GPIO_UNDEF
#endif

#ifndef NRF24L01P_PARAM_IRQ
/**
 * @brief   NRF24L01P interrupt gpio pin
 */
#define NRF24L01P_PARAM_IRQ                 GPIO_UNDEF
#endif

#ifndef NRF24L01P_PARAM_RX_P0_ADDR
/**
 * @brief   Pipe 0 address (enabled by default)
 */
#define NRF24L01P_PARAM_RX_P0_ADDR          NRF24L01P_DEFAULT_L2ADDR_P0
#endif

#ifndef NRF24L01P_PARAM_RX_P1_ADDR
/**
 * @brief   Pipe 1 address (enabled by default)
 */
#define NRF24L01P_PARAM_RX_P1_ADDR          NRF24L01P_DEFAULT_L2ADDR_P1
#endif

#ifndef NRF24L01P_PARAM_RX_P2_ADDR
/**
 * @brief   Pipe 2 address (disabled by default)
 */
#define NRF24L01P_PARAM_RX_P2_ADDR          NRF24L01P_L2ADDR_UNDEF
#endif

#ifndef NRF24L01P_PARAM_RX_P3_ADDR
/**
 * @brief   Pipe 3 address (disabled by default)
 */
#define NRF24L01P_PARAM_RX_P3_ADDR          NRF24L01P_L2ADDR_UNDEF
#endif

#ifndef NRF24L01P_PARAM_RX_P4_ADDR
/**
 * @brief   Pipe 4 address (disabled by default)
 */
#define NRF24L01P_PARAM_RX_P4_ADDR          NRF24L01P_L2ADDR_UNDEF
#endif

#ifndef NRF24L01P_PARAM_RX_P5_ADDR
/**
 * @brief   Pipe 5 address (disabled by default)
 */
#define NRF24L01P_PARAM_RX_P5_ADDR          NRF24L01P_L2ADDR_UNDEF
#endif

#ifndef NRF24L01P_PARAM_PROTOCOL
/**
 * @brief   Default protocol [ShockBurst | Enhanced SchockBurst]
 *          @see nrf24l01p_protocol_t
 */
#define NRF24L01P_PARAM_PROTOCOL            NRF24L01P_ENHANCED_SHOCKBURST
#endif

#ifndef NRF24L01P_PARAM_CRC_LEN
/**
 * @brief   Default CRC length [0; 3]
 *          @see nrf24l01p_crc_t
 */
#define NRF24L01P_PARAM_CRC_LEN             (NRF24L01P_CRC_2BYTE)
#endif

#ifndef NRF24L01P_PARAM_TX_POWER_LVL
/**
 * @brief   Default TX power in [0; 3]
 *          @see nrf24l01p_tx_power_t
 */
#define NRF24L01P_PARAM_TX_POWER_LVL        (NRF24L01P_TX_POWER_0DBM)
#endif

#if (NRF24L01P_PARAM_PROTOCOL == NRF24L01P_SHOCKBURST)
#ifndef NRF24L01P_PARAM_DATA_RATE_LVL
/**
 * @brief   Default data rate [0; 2]
 *          @see nrf24l01p_rfdr_t
 */
#define NRF24L01P_PARAM_DATA_RATE_LVL       (NRF24L01P_RF_DR_1MBPS)
#endif
#else
#ifndef NRF24L01P_PARAM_DATA_RATE_LVL
/**
 * @brief   Default data rate [0; 2]
 *          @see nrf24l01p_rfdr_t
 */
#define NRF24L01P_PARAM_DATA_RATE_LVL       (NRF24L01P_RF_DR_2MBPS)
#endif
#endif

#ifndef NRF24L01P_PARAM_CHANNEL
/**
 * @brief   Default channel in [0; 15]
 */
#define NRF24L01P_PARAM_CHANNEL             (4)
#endif

#if (NRF24L01P_PARAM_PROTOCOL == NRF24L01P_SHOCKBURST)
#ifndef NRF24L01P_PARAM_MAX_RETRANSM
/**
 * @brief   Default number of retransmissions
 */
#define NRF24L01P_PARAM_MAX_RETRANSM        (0)
#endif
#else
#ifndef NRF24L01P_PARAM_MAX_RETRANSM
/**
 * @brief   Default number of retransmissions
 */
#define NRF24L01P_PARAM_MAX_RETRANSM        (5)
#endif
#endif

#ifndef NRF24L01P_PARAM_RETRANSM_DELAY
/**
 * @brief   Default retransmission delay
 */
#define NRF24L01P_PARAM_RETRANSM_DELAY      (10)
#endif

#ifndef NRF24L01P_PARAM_PLW_PADD_P0
/**
 * @brief   Default payload width pipe 0
 *          (Only used for ShockBurst)
 */
#define NRF24L01P_PARAM_PLW_PADD_P0         (0)
#endif

#ifndef NRF24L01P_PARAM_PLW_PADD_P1
/**
 * @brief   Default payload width pipe 1
 *          (Only used for ShockBurst)
 */
#define NRF24L01P_PARAM_PLW_PADD_P1         (0)
#endif

#ifndef NRF24L01P_PARAM_PLW_PADD_P2
/**
 * @brief   Default payload width pipe 2
 *          (Only used for ShockBurst)
 */
#define NRF24L01P_PARAM_PLW_PADD_P2         (0)
#endif

#ifndef NRF24L01P_PARAM_PLW_PADD_P3
/**
 * @brief   Default payload width pipe 3
 *          (Only used for ShockBurst)
 */
#define NRF24L01P_PARAM_PLW_PADD_P3         (0)
#endif

#ifndef NRF24L01P_PARAM_PLW_PADD_P4
/**
 * @brief   Default payload width pipe 4
 *          (Only used for ShockBurst)
 */
#define NRF24L01P_PARAM_PLW_PADD_P4         (0)
#endif

#ifndef NRF24L01P_PARAM_PLW_PADD_P5
/**
 * @brief   Default payload width pipe 5
 *          (Only used for ShockBurst)
 */
#define NRF24L01P_PARAM_PLW_PADD_P5         (0)
#endif

#ifndef NRF24L01P_PARAMS
/**
 * @brief Default NRF24L01P device parameters
 */
#define NRF24L01P_PARAMS    {                                                  \
        .spi = NRF24L01P_PARAM_SPI,                                            \
        .spi_clk = NRF24L01P_PARAM_SPI_CLK,                                    \
        .pin_cs = NRF24L01P_PARAM_CS,                                          \
        .pin_ce = NRF24L01P_PARAM_CE,                                          \
        .pin_irq = NRF24L01P_PARAM_IRQ,                                        \
        .urxaddr.rxaddrpx = {                                                  \
            .rx_pipe_0_addr = NRF24L01P_PARAM_RX_P0_ADDR,                      \
            .rx_pipe_1_addr = NRF24L01P_PARAM_RX_P1_ADDR,                      \
            .rx_pipe_2_addr = NRF24L01P_PARAM_RX_P2_ADDR,                      \
            .rx_pipe_3_addr = NRF24L01P_PARAM_RX_P3_ADDR,                      \
            .rx_pipe_4_addr = NRF24L01P_PARAM_RX_P4_ADDR,                      \
            .rx_pipe_5_addr = NRF24L01P_PARAM_RX_P5_ADDR,                      \
        },                                                                     \
        .config =       {                                                      \
            .cfg_protocol = NRF24L01P_PARAM_PROTOCOL,                          \
            .cfg_crc = NRF24L01P_PARAM_CRC_LEN,                                \
            .cfg_tx_power = NRF24L01P_PARAM_TX_POWER_LVL,                      \
            .cfg_data_rate = NRF24L01P_PARAM_DATA_RATE_LVL,                    \
            .cfg_channel = NRF24L01P_PARAM_CHANNEL,                            \
            .cfg_addr_width = sizeof((uint8_t[])NRF24L01P_PARAM_RX_P0_ADDR) -  \
                              2U,                                              \
            .cfg_max_retr = NRF24L01P_PARAM_MAX_RETRANSM,                      \
            .cfg_retr_delay = NRF24L01P_PARAM_RETRANSM_DELAY,                  \
            .cfg_plw_padd_p0 = NRF24L01P_PARAM_PLW_PADD_P0,                    \
            .cfg_plw_padd_p1 = NRF24L01P_PARAM_PLW_PADD_P1,                    \
            .cfg_plw_padd_p2 = NRF24L01P_PARAM_PLW_PADD_P2,                    \
            .cfg_plw_padd_p3 = NRF24L01P_PARAM_PLW_PADD_P3,                    \
            .cfg_plw_padd_p4 = NRF24L01P_PARAM_PLW_PADD_P4,                    \
            .cfg_plw_padd_p5 = NRF24L01P_PARAM_PLW_PADD_P5                     \
        }                                                                      \
}
#endif

/**
 * @brief   Static array that holds NRF24L01P device configurations
 */
static const nrf24l01p_params_t nrf24l01p_params[] = {
    NRF24L01P_PARAMS
};

/**
 * @brief   Number of NRF24L01P device configurations
 */
#define NRF24L01P_NUM                       ARRAY_SIZE(nrf24l01p_params)

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_PARAMS_H */
/** @} */
