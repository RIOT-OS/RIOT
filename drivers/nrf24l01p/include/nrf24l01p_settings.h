/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief       Low-level driver for nrf24l01+ transceiver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 */

#ifndef NRF24L01P_SETTINGS_H
#define NRF24L01P_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters
 * @{
 */
#define INITIAL_ADDRESS_WIDTH       5
#define NRF24L01P_MAX_DATA_LENGTH   32
#ifndef INITIAL_RF_CHANNEL
#define INITIAL_RF_CHANNEL          5
#endif
#define INITIAL_RX_POWER_0dB        0
/** @} */

/**
 * @name    Timing parameters
 * @{
 */
#define DELAY_CS_TOGGLE_US          2
#define DELAY_AFTER_FUNC_US         2
#define DELAY_CE_HIGH_US            (20)
#define DELAY_CHANGE_PWR_MODE_US    (1500)
#define DELAY_CHANGE_TXRX_US        (130)
#define DELAY_CE_START_US           (5)
/*
 * This is the time which is needed to physically transmit the data.
 * Compare nrf24l01+ pruduct specification p.42. It is computed just
 * for this setup
 */
#define DELAY_DATA_ON_AIR           (1300)
/** @} */

/**
 * @name    Command definitions
 * @{
 */
#define CMD_R_REGISTER          0x00
#define CMD_W_REGISTER          0x20
#define CMD_R_RX_PAYLOAD        0x61
#define CMD_W_TX_PAYLOAD        0xa0
#define CMD_FLUSH_TX            0xe1
#define CMD_FLUSH_RX            0xe2
#define CMD_REUSE_TX_PL         0xe3
#define CMD_R_RX_PL_WID         0x60
#define CMD_W_ACK_PAYLOAD       0xa8
#define CMD_W_TX_PAYLOAD_NOACK  0xb0
#define CMD_NOOP                0xff

#define REGISTER_MASK           0x1F
/** @} */

/**
 * @name    Register address definitions
 * @{
 */
#define REG_CONFIG              0x00
#define REG_EN_AA               0x01
#define REG_EN_RXADDR           0x02
#define REG_SETUP_AW            0x03
#define REG_SETUP_RETR          0x04
#define REG_RF_CH               0x05
#define REG_RF_SETUP            0x06
#define REG_STATUS              0x07
#define REG_OBSERVE_TX          0x08
#define REG_RPD                 0x09
#define REG_RX_ADDR_P0          0x0a
#define REG_RX_ADDR_P1          0x0b
#define REG_RX_ADDR_P2          0x0c
#define REG_RX_ADDR_P3          0x0d
#define REG_RX_ADDR_P4          0x0e
#define REG_RX_ADDR_P5          0x0f
#define REG_TX_ADDR             0x10
#define REG_RX_PW_P0            0x11
#define REG_RX_PW_P1            0x12
#define REG_RX_PW_P2            0x13
#define REG_RX_PW_P3            0x14
#define REG_RX_PW_P4            0x15
#define REG_RX_PW_P5            0x16
#define REG_FIFO_STATUS         0x17
#define REG_DYNPD               0x1c
#define REG_FEATURE             0x1d
/** @} */

/**
 * @name    EN_AA register bitmaps
 * @{
 */
#define ENAA_P0                 0x01
#define ENAA_P1                 0x02
#define ENAA_P2                 0x04
#define ENAA_P3                 0x08
#define ENAA_P4                 0x10
#define ENAA_P5                 0x20
/** @} */

/**
 * @name    CONFIG register bitmaps
 */
#define MASK_RX_DR              0x40
#define MASK_TX_DS              0x20
#define MASK_MAX_RT             0x10
#define EN_CRC                  0x08
#define CRCO                    0x04
#define PWR_UP                  0x02
#define PRIM_RX                 0x01
/** @} */

/**
 * @name    STATUS register bitmaps
 * @{
 */
#define RX_DR                   0x40
#define TX_DS                   0x20
#define MAX_RT                  0x10
#define RX_P_NO                 0x0e
#define TX_FULL                 0x01
#define ALL_INT_MASK            0x70
/** @} */

/**
 * @name    RF_SETUP register bitmaps
 * @{
 */
#define RF_SETUP_CONT_WAVE      (1 << 7)
#define RF_SETUP_RF_DR_LOW      (1 << 5)
#define RF_SETUP_PLL_LOCK       (1 << 4)
#define RF_SETUP_RF_DR_HIGH     (1 << 3)
#define RF_SETUP_RF_PWR         (3 << 1)
/** @} */

/**
 * @name    Channel mask for the RF_CH register
 */
#define RF_CH_MASK              0x7f

/**
 * @name    DYNPD register bitmaps
 * @{
 */
#define DYNPD_DPL_P5            (1 << 5)
#define DYNPD_DPL_P4            (1 << 4)
#define DYNPD_DPL_P3            (1 << 3)
#define DYNPD_DPL_P2            (1 << 2)
#define DYNPD_DPL_P1            (1 << 1)
#define DYNPD_DPL_P0            (1 << 0)
/** @} */

/**
 * @name    FEATURE register bitmaps
 * @{
 */
#define FEATURE_EN_DPL          (1 << 2)
#define FEATURE_EN_ACK_PAY      (1 << 1)
#define FEATURE_EN_DYN_ACK      (1 << 0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_SETTINGS_H */
/** @} */
