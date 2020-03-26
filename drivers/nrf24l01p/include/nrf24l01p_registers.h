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
 * @brief   Register map of NRF24L01P devices
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_REGISTERS_H
#define NRF24L01P_REGISTERS_H

#include <stdint.h>

#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    NRF24L01P CONFIG register
 *
 * Address and layout of NRF24L01P configuration register
 * @{
 */
/**
 * @brief CONFIG register address
 */
#define NRF24L01P_REG_CONFIG               (0x00)
/**
 * @brief   CONFIG register type
 */
typedef uint8_t nrf24l01p_reg8_config_t;

/**
 * @brief   Flag: MASK_RX_DR
 */
#define NRF24L01P_FLG_MASK_RX_DR           (0b01000000)
/**
 *@brief    Value of: MASK_RX_DR
 */
#define NRF24L01P_VAL_MASK_RX_DR(reg)      (((reg) & \
                                              NRF24L01P_FLG_MASK_RX_DR) >> 6)
/**
 * @brief   Flag: MASK_TX_DS
 */
#define NRF24L01P_FLG_MASK_TX_DS           (0b00100000)
/**
 * @brief   Value of: MASK_TX_DS
 */
#define NRF24L01P_VAL_MASK_TX_DS(reg)      (((reg) & \
                                              NRF24L01P_FLG_MASK_TX_DS) >> 5)
/**
 * @brief   Flag: MASK_MAX_RT
 */
#define NRF24L01P_FLG_MASK_MAX_RT          (0b00010000)
/**
 * @brief   Value of: MASK_MAX_RT
 */
#define NRF24L01P_VAL_MASK_MAX_RT          (((reg) & \
                                              NRF24L01P_FLG_MASK_MAX_RT) >> 4)

/**
 * @brief   Flag: EN_CRC
 */
#define NRF24L01P_FLG_EN_CRC               (0b00001000)
/**
 * @brief   Value of: EN_CRC
 */
#define NRF24L01P_VAL_EN_CRC(reg)          (((reg) & \
                                              NRF24L01P_FLG_EN_CRC) >> 3)
/**
 * @brief   Flag: CRCO - 1 byte
 */
#define NRF24L01P_FLG_CRCO_1_BYTE          (0b00000000)
/**
 * @brief   Flag: CRCO - 2 byte
 */
#define NRF24L01P_FLG_CRCO_2_BYTE          (0b00000100)
/**
 * @brief   Value of: CRCO
 */
#define NRF24L01P_VAL_CRCO(reg)            (((reg) & \
                                              NRF24L01P_FLG_CRCO_2_BYTE) >> 2)
/**
 * @brief   Mask to configure CRC
 */
#define NRF24L01P_MSK_CRC                  (0b00001100)

/**
 * @brief   Flag: PWR_UP
 */
#define NRF24L01P_FLG_PWR_UP               (0b00000010)
/**
 * @brief   Value of: PWR_UP
 */
#define NRF24L01P_VAL_PWR_UP(reg)          (((reg) & \
                                              NRF24L01P_FLG_PWR_UP) >> 1)

/**
 * @brief   Flag: PRIM_RX
 */
#define NRF24L01P_FLG_PRIM_RX              (0b00000001)
/**
 * @brief   Value of: PRIM_RX
 */
#define NRF24L01P_VAL_PRIM_RX(reg)         ((reg) & NRF24L01P_FLG_PRIM_RX)
/** @} */

/**
 * @name    NRF24L01P EN_AA register
 *
 * Address and layout of NRF24L01P EN_AA register
 * @{
 */
/**
 * @brief   EN_AA register address
 */
#define NRF24L01P_REG_EN_AA                (0x01)
/**
 * @brief   EN_AA register type
 */
typedef uint8_t nrf24l01p_reg8_en_aa_t;

/**
 * @brief   Flag: ENAA_P5
 */
#define NRF24L01P_FLG_ENAA_P5              (0b00100000)
/**
 * @brief   Value of: ENAA_P5
 */
#define NRF24L01P_VAL_ENAA_P5(reg)         (((reg) & \
                                              NRF24L01P_FLG_ENAA_P5) >> 5)
/**
 * @brief Flag: ENAA_P4
 */
#define NRF24L01P_FLG_ENAA_P4              (0b00010000)
/**
 * @brief   Value of: ENAA_P4
 */
#define NRF24L01P_VAL_ENAA_P4(reg)         (((reg) & \
                                              NRF24L01P_FLG_ENAA_P4) >> 4)
/**
 * @brief   Flaga: ENAA_P3
 */
#define NRF24L01P_FLG_ENAA_P3              (0b00001000)
/**
 * @brief   Value of: ENAA_P3
 */
#define NRF24L01P_VAL_ENAA_P3(reg)         (((reg) & \
                                              NRF24L01P_FLG_ENAA_P3) >> 3)
/**
 * @brief   Flag: ENAA_P2
 */
#define NRF24L01P_FLG_ENAA_P2              (0b00000100)
/**
 * @brief   Value of: ENAA_P2
 */
#define NRF24L01P_VAL_ENAA_P2(reg)         (((reg) & \
                                              NRF24L01P_FLG_ENAA_P2) >> 2)
/**
 * @brief   Flag: ENAA_P1
 */
#define NRF24L01P_FLG_ENAA_P1              (0b00000010)
/**
 * @brief   Value of: ENAA_P1
 */
#define NRF24L01P_VAL_ENAA_P1(reg)         (((reg) & \
                                              NRF24L01P_FLG_ENAA_P1) >> 1)
/**
 * @brief   Flag: ENAA_P0
 */
#define NRF24L01P_FLG_ENAA_P0              (0b00000001)
/**
 * @brief   Value of: ENAA_P0
 */
#define NRF24L01P_VAL_ENAA_P0(reg)         ((reg) & NRF24L01P_FLG_ENAA_P0)
/** @} */

/**
 * @name    NRF24L01P EN_RXADDR register
 *
 * Address and layout of NRF24L01P EN_RXADDR register
 * @{
 */
/**
 * @brief   EN_RXADDR register address
 */
#define NRF24L01P_REG_EN_RXADDR            (0x02)
/**
 * @brief   EN_RXADDR register type
 */
typedef uint8_t nrf24l01p_reg8_en_rxaddr_t;

/**
 * @brief   Flag: ERX_P5
 */
#define NRF24L01P_FLG_ERX_P5               (0b00100000)
/**
 * @brief   Value of: ERX_P5
 */
#define NRF24L01P_VAL_ERX_P5(reg)          (((reg) & \
                                              NRF24L01P_FLG_ERX_P5) >> 5)
/**
 * @brief   Flag: ERX_P4
 */
#define NRF24L01P_FLG_ERX_P4               (0b00010000)
/**
 * @brief   Value of ERX_P4
 */
#define NRF24L01P_VAL_ERX_P4(reg)          (((reg) & \
                                              NRF24L01P_FLG_ERX_P4) >> 4)
/**
 * @brief   Flag: ERX_P3
 */
#define NRF24L01P_FLG_ERX_P3               (0b00001000)
/**
 * @brief   Value of: ERX_P3
 */
#define NRF24L01P_VAL_ERX_P3(reg)          (((reg) & \
                                              NRF24L01P_FLG_ERX_P3) >> 3)
/**
 * @brief   Flag: ERX_P2
 */
#define NRF24L01P_FLG_ERX_P2               (0b00000100)
/**
 * @brief   Value of: ERX_P2
 */
#define NRF24L01P_VAL_ERX_P2(reg)          (((reg) & \
                                              NRF24L01P_FLG_ERX_P2) >> 2)
/**
 * @brief   Flag: ERX_P1
 */
#define NRF24L01P_FLG_ERX_P1               (0b00000010)
/**
 * @brief   Value of: ERX_P1
 */
#define NRF24L01P_VAL_ERX_P1(reg)          (((reg) & \
                                              NRF24L01P_FLG_ERX_P1) >> 1)
/**
 * @brief   Flag: ERX_P0
 */
#define NRF24L01P_FLG_ERX_P0               (0b00000001)
/**
 * @brief   Value of: ERX_P0
 */
#define NRF24L01P_VAL_ERX_P0(reg)          ((reg) & NRF24L01P_FLG_ERX_P0)
/** @} */

/**
 * @name    NRF24L01P SETUP_AW register
 *
 * Address and layout of NRF24L01P SETUP_AW register
 * @{
 */
/**
 * @brief   SETUP_AW register address
 */
#define NRF24L01P_REG_SETUP_AW             (0x03)
/**
 * @brief   SETUP_AW register type
 */
typedef uint8_t nrf24l01p_reg8_setup_aw_t;

/**
 * @brief   Flag: AW - 3 bytes
 */
#define NRF24L01P_FLG_AW_3                 (0b00000001)
/**
 * @brief   Flag: AW - 4 bytes
 */
#define NRF24L01P_FLG_AW_4                 (0b00000010)
/**
 * @brief   Flag: AW - 5 bytes
 */
#define NRF24L01P_FLG_AW_5                 (0b00000011)
/**
 * @brief   Mask to configure AW
 */
#define NRF24L01P_MSK_AW                   (0b00000011)
/**
 * @brief   Flag AW from value [1; 3]
 */
#define NRF24L01P_FLG_AW(val)              ((val) & NRF24L01P_MSK_AW)
/**
 * @brief   Value of: AW
 */
#define NRF24L01P_VAL_AW(reg)              ((reg) & NRF24L01P_MSK_AW)
/** @} */

/**
 * @name    NRF24L01P SETUP_RETR register
 *
 * Address and layout of NRF24L01P SETUP_AW register
 * @{
 */
/**
 * @brief   SETUP_RETR register address
 */
#define NRF24L01P_REG_SETUP_RETR           (0x04)
/**
 * @brief   SETUP_RETR register type
 */
typedef uint8_t nrf24l01p_reg8_setup_retr_t;

/**
 * @brief   Flag: ARD - 250 us
 */
#define NRF24L01P_FLG_ARD_250_US           (0b00000000)
/**
 * @brief   Flag: ARD - 500 us
 */
#define NRF24L01P_FLG_ARD_500_US           (0b00010000)
/**
 * @brief   Flag: ARD - 750 us
 */
#define NRF24L01P_FLG_ARD_750_US           (0b00100000)
/**
 * @brief   Flag: ARD - 1000 us
 */
#define NRF24L01P_FLG_ARD_1000_US          (0b00110000)
/**
 * @brief   Flag: ARD - 1250 us
 */
#define NRF24L01P_FLG_ARD_1250_US          (0b01000000)
/**
 * @brief   Flag: ARD - 1500 us
 */
#define NRF24L01P_FLG_ARD_1500_US          (0b01010000)
/**
 * @brief   Flag: ARD - 1750 us
 */
#define NRF24L01P_FLG_ARD_1750_US          (0b01100000)
/**
 * @brief   Flag: ARD - 2000 us
 */
#define NRF24L01P_FLG_ARD_2000_US          (0b01110000)
/**
 * @brief   Flag: ARD - 2250 us
 */
#define NRF24L01P_FLG_ARD_2250_US          (0b10000000)
/**
 * @brief   Flag: ARD - 2500 us
 */
#define NRF24L01P_FLG_ARD_2500_US          (0b10010000)
/**
 * @brief   Flag: ARD - 2750 us
 */
#define NRF24L01P_FLG_ARD_2750_US          (0b10100000)
/**
 * @brief   Flag: ARD - 3000 us
 */
#define NRF24L01P_FLG_ARD_3000_US          (0b10110000)
/**
 * @brief   Flag: ARD - 3250 us
 */
#define NRF24L01P_FLG_ARD_3250_US          (0b11000000)
/**
 * @brief   Flag: ARD - 3500 us
 */
#define NRF24L01P_FLG_ARD_3500_US          (0b11010000)
/**
 * @brief   Flag: ARD - 3750 us
 */
#define NRF24L01P_FLG_ARD_3750_US          (0b11100000)
/**
 * @brief   Flag: ARD - 4000 us
 */
#define NRF24L01P_FLG_ARD_4000_US          (0b11110000)
/**
 * @brief   Mask to configure ARD
 */
#define NRF24L01P_MSK_ARD                  (0b11110000)
/**
 * @brief   Flag ARD from value [0; 15]
 */
#define NRF24L01P_FLG_ARD(val)             (((val) << 4) & NRF24L01P_MSK_ARD)
/**
 * @brief   Value of: ARD
 */
#define NRF24L01P_VAL_ARD(reg)             (((reg) & NRF24L01P_MSK_ARD) >> 4)
/**
 * @brief Mask to configure ARC
 */
#define NRF24L01P_MSK_ARC                  (0b00001111)
/**
 * @brief   Flag ARC from value [0; 15]
 */
#define NRF24L01P_FLG_ARC(val)             ((val) & NRF24L01P_MSK_ARC)
/**
 * @brief   Value of: ARC
 */
#define NRF24L01P_VAL_ARC(reg)             ((reg) & NRF24L01P_MSK_ARC)
/** @} */

/**
 * @name    NRF24L01P RF_CH register
 *
 * Address and layout of NRF24L01P SETUP_AW register
 * @{
 */
/**
 * @brief   RF_CH register address
 */
#define NRF24L01P_REG_RF_CH                (0x05)
/**
 * @brief   RF_CH register type
 */
typedef uint8_t nrf24l01p_reg8_rf_ch_t;

/**
 * @brief   Mask to configure RF_CH
 */
#define NRF24L01P_MSK_RF_CH                (0b01111111)
/**
 * @brief   Flag RF_CH from value [0; 127]
 */
#define NRF24L01P_FLG_RF_CH(val)           ((val) & NRF24L01P_MSK_RF_CH)
/**
 * @brief   Value of: RF_CH
 */
#define NRF24L01P_VAL_RF_CH(reg)           ((reg) & NRF24L01P_MSK_RF_CH)
/** @} */

/**
 * @name    NRF24L01P RF_SETUP register
 *
 * Address and layout of NRF24L01P RF_SETUP register
 * @{
 */
/**
 * @brief   RF_SETUP register address
 */
#define NRF24L01P_REG_RF_SETUP             (0x06)
/**
 * @brief   RF_SETUP register type
 */
typedef uint8_t nrf24l01p_reg8_rf_setup_t;

/**
 * @brief   Flag: CONT_WAVE
 */
#define NRF24L01P_FLG_CONT_WAVE            (0b10000000)
/**
 * @brief   Value of: CONT_WAVE
 */
#define NRF24L01P_VAL_CONT_WAVE(reg)       (((reg) & \
                                              NRF24L01P_FLG_CONT_WAVE) >> 7)
/**
 * @brief   Flag: RF_DR_LOW
 */
#define NRF24L01P_FLG_RF_DR_LOW            (0b00100000)
/**
 * @brief   Value of: RF_DR_LOW
 */
#define NRF24L01P_VAL_RF_DR_LOW(reg)       (((reg) & \
                                              NRF24L01P_FLG_RF_DR_LOW) >> 5)

/**
 * @brief   Flag: PLL_LOCK
 */
#define NRF24L01P_FLG_PLL_LOCK             (0b00010000)
/**
 * @brief   Value of: PLL_LOCK
 */
#define NRF24L01P_VAL_PLL_LOCK(reg)        (((reg) & \
                                              NRF24L01P_FLG_PLL_LOCK) >> 4)

/**
 * @brief   Flag: RF_DR_HIGH
 */
#define NRF24L01P_FLG_RF_DR_HIGH           (0b00001000)
/**
 * @brief   Value of: RF_DR_HIGH
 */
#define NRF24L01P_VAL_RF_DR_HIGH(reg)      (((reg) & \
                                              NRF24L01P_FLG_RF_DR_HIGH) >> 3)
/**
 * @brief   Flag: RF_DR - 250 kbit/s
 */
#define NRF24L01P_FLG_RF_DR_250_KBPS        (0b00001000)
/**
 * @brief   Flag: RF_DR - 1000 kbit/s
 */
#define NRF24L01P_FLG_RF_DR_1_MBPS          (0b00000000)
/**
 * @brief   Flag: RF_DR - 2000 kbit/s
 */
#define NRF24L01P_FLG_RF_DR_2_MBPS          (0b00100000)
/**
 * @brief   Mask to configure RF_DR
 */
#define NRF24L01P_MSK_RF_DR                (0b00101000)
/**
 * @brief   FLG RF_DR
 */
#define NRF24L01P_FLG_RF_DR(val)           (((val & 1) << 5) | \
                                            ((val & 2) << 2))
/**
 * @brief   Value of: RF_DR
 */
#define NRF24L01P_VAL_RF_DR(reg)           ((((reg) & \
                                            NRF24L01P_FLG_RF_DR_LOW) >> 5) | \
                                            (((reg) & \
                                            NRF24L01P_FLG_RF_DR_HIGH) >> 2))
/**
 * @brief   Flag: RF_PWR - -18 dbm
 */
#define NRF24L01P_FLG_RF_PWR_MINUS_18      (0b00000000)
/**
 * @brief   Flag: RF_PWR - -12 dbm
 */
#define NRF24L01P_FLG_RF_PWR_MINUS_12      (0b00000010)
/**
 * @brief   Flag: RF_PWR - -6 dbm
 */
#define NRF24L01P_FLG_RF_PWR_MINUS_6       (0b00000100)
/**
 * @brief   Flag: RF_PWR - 0 dbm
 */
#define NRF24L01P_FLG_RF_PWR_0             (0b00000110)
/**
 * @brief   Mask to configure RF_PWR
 */
#define NRF24L01P_MSK_RF_PWR               (0b00000110)
/**
 * @brief   Flag RF_PWR from value [0; 3]
 */
#define NRF24L01P_FLG_RF_PWR(val)          (((val) << 1) & \
                                             NRF24L01P_MSK_RF_PWR)
/**
 * @brief   Value of: RF_PWR
 */
#define NRF24L01P_VAL_RF_PWR(reg)          (((reg) & \
                                              NRF24L01P_MSK_RF_PWR) >> 1)
/** @} */

/**
 * @name    NRF24L01P STATUS register
 *
 * Address and layout of NRF24L01P STATUS register
 * @{
 */
/**
 * @brief   STATUS register address
 */
#define NRF24L01P_REG_STATUS               (0x07)
/**
 * @brief   STATUS register type
 */
typedef uint8_t nrf24l01p_reg8_status_t;

/**
 * @brief   Flag: RX_DR
 */
#define NRF24L01P_FLG_RX_DR                (0b01000000)
/**
 * @brief    Value of: RX_DR
 */
#define NRF24L01P_VAL_RX_DR(reg)           (((reg) & \
                                              NRF24L01P_FLG_RX_DR) >> 6)
/**
 * @brief   Flag: TX_DS
 */
#define NRF24L01P_FLG_TX_DS                (0b00100000)
/**
 * @brief   Value of: TX_DS
 */
#define NRF24L01P_VAL_TX_DS(reg)           (((reg) & \
                                              NRF24L01P_FLG_TX_DS) >> 5)
/**
 * @brief   Flag: MAX_RT
 */
#define NRF24L01P_FLG_MAX_RT               (0b00010000)
/**
 * @brief   Value of: MAX_RT
 */
#define NRF24L01P_VAL_MAX_RT(reg)          (((reg) & \
                                              NRF24L01P_FLG_MAX_RT) >> 4)

/**
 * @brief   Flag: RX_P_NO - Rx FIFO empty
 */
#define NRF24L01P_FLG_RX_P_NO_NONE         (0b00001110)
/**
 * @brief   Mask to read RX_P_NO
 */
#define NRF24L01P_MSK_RX_P_NO              (0b00001110)
/**
 * @brief   Flag RX_P_NO from value [0; 7]
 */
#define NRF24L01P_FLG_RX_P_NO(val)         (((val) << 1) & \
                                             NRF24L01P_MSK_RX_P_NO)
/**
 * @brief   Value of: RX_P_NO
 */
#define NRF24L01P_VAL_RX_P_NO(reg)         (((reg) & \
                                              NRF24L01P_MSK_RX_P_NO) >> 1)

/**
 * @brief   Flag: TX_FULL
 */
#define NRF24L01P_FLG_TX_FULL              (0b00000001)
/**
 * @brief   Value of: TX_FULL
 */
#define NRF24L01P_VAL_TX_FULL(reg)         ((reg) & NRF24L01P_FLG_TX_FULL)
/** @} */

/**
 * @name    NRF24L01P OBSERVE_TX register
 *
 * Address and layout of NRF24L01P STATUS register
 * @{
 */
/**
 * @brief   OBSERVE_TX register address
 */
#define NRF24L01P_REG_OBSERVE_TX           (0x08)
/**
 * @brief   OBSERVE_TX register type
 */
typedef uint8_t nrf24l01p_reg8_observe_tx_t;

/**
 * @brief   Mask to read PLOS_CNT
 */
#define NRF24L01P_MSK_PLOS_CNT             (0b11110000)
/**
 * @brief   Flag PLOS_CNT from value [0; 15]
 */
#define NRF24L01P_FLG_PLOS_CNT(val)        (((val) << 4) & \
                                             NRF24L01P_MSK_PLOS_CNT)
/**
 * @brief   Value of: PLOS_CNT
 */
#define NRF24L01P_VAL_PLOS_CNT(reg)        (((reg) & \
                                              NRF24L01P_MSK_PLOS_CNT) >> 4)

/**
 * @brief Mask to read ARC_CNT
 */
#define NRF24L01P_MSK_ARC_CNT              (0b00001111)
/**
 * @brief   Flag ARC_CNT from value [0; 15]
 */
#define NRF24L01P_FLG_ARC_CNT(val)         ((val) & NRF24L01P_MSK_ARC_CNT)
/**
 * @brief   Value of: ARC_CNT
 */
#define NRF24L01P_VAL_ARC_CNT(reg)         ((reg) & NRF24L01P_MSK_ARC_CNT)
/** @} */

/**
 * @name    NRF24L01P RPD register
 *
 * Address and layout of NRF24L01P RPD register
 * @{
 */
/**
 * @brief   RPD register address
 */
#define NRF24L01P_REG_RPD                  (0x09)
/**
 * @brief   RPD register type
 */
typedef uint8_t nrf24l01p_reg8_rpd_t;

/**
 * @brief   Flag: RPD
 */
#define NRF24L01P_FLG_RPD                  (0b00000001)
/**
 * @brief   Value of: RPD
 */
#define NRF24L01P_VAL_RPD(reg)             ((reg) & NRF24L01P_FLG_RPD)
/** @} */

/**
 * @name    NRF24L01P RX_ADDR_Px registers
 *
 * Addresses of NRF24L01P RX_ADDR_Px registers and TX_ADDR register
 * @{
 */
/**
 * @brief   RX_ADDR_P0 register address
 */
#define NRF24L01P_REG_RX_ADDR_P0           (0x0A)
/**
 * @brief   RX_ADDR_P1 register address
 */
#define NRF24L01P_REG_RX_ADDR_P1           (0x0B)
/**
 * @brief   RX_ADDR_P[0 | 1] register type
 */
typedef uint8_t nrf24l01p_reg40_rx_addr_px_t[NRF24L01P_MAX_ADDR_WIDTH];

/**
 * @brief   RX_ADDR_P2 register address
 */
#define NRF24L01P_REG_RX_ADDR_P2           (0x0C)
/**
 * @brief   RX_ADDR_P3 register address
 */
#define NRF24L01P_REG_RX_ADDR_P3           (0x0D)
/**
 * @brief   RX_ADDR_P4 register address
 */
#define NRF24L01P_REG_RX_ADDR_P4           (0x0E)
/**
 * @brief   RX_ADDR_P5 register address
 */
#define NRF24L01P_REG_RX_ADDR_P5           (0x0F)
/**
 * @brief   RX_ADDR_P[2 | 3 | 4 | 5] register type
 */
typedef uint8_t nrf24l01p_reg8_rx_addr_px_t;

/**
 * @brief   TX_ADDR register address
 */
#define NRF24L01P_REG_TX_ADDR              (0x10)
/**
 * @brief   TX_ADDR register type
 */
typedef uint8_t nrf24l01p_reg40_tx_addr_t[NRF24L01P_MAX_ADDR_WIDTH];
/** @} */

/**
 * @name    NRF24L01P RX_PW_Px registers
 *
 * Address and layout of NRF24L01P RX_PW_Px registers
 * @{
 */
/**
 * @brief   RX_PW_P0 register address
 */
#define NRF24L01P_REG_RX_PW_P0             (0x11)
/**
 * @brief   RX_PW_P1 register address
 */
#define NRF24L01P_REG_RX_PW_P1             (0x12)
/**
 * @brief   RX_PW_P2 register address
 */
#define NRF24L01P_REG_RX_PW_P2             (0x13)
/**
 * @brief   RX_PW_P3 register address
 */
#define NRF24L01P_REG_RX_PW_P3             (0x14)
/**
 * @brief   RX_PW_P4 register address
 */
#define NRF24L01P_REG_RX_PW_P4             (0x15)
/**
 * @brief   RX_PW_P5 register address
 */
#define NRF24L01P_REG_RX_PW_P5             (0x16)
/**
 * @brief   RX_PW_Px register type
 */
typedef uint8_t nrf24l01p_reg8_rx_pw_px_t;

/**
 * @brief   Mask to configure RX_PW_PX
 */
#define NRF24L01P_MSK_RX_PW_PX             (0b0011111)
/**
 * @brief   Flag RX_PW_Px from value [0; 32]
 */
#define NRF24L01P_FLG_RX_PW_PX(val)        ((val) & NRF24L01P_MSK_RX_PW_PX)
/**
 * @brief   Value of: RX_PW_PX
 */
#define NRF24L01P_VAL_RX_PW_PX(reg)        ((reg) & NRF24L01P_MSK_RX_PW_PX)
/** @} */

/**
 * @name    NRF24L01P FIFO_STATUS register
 *
 * Address and layout of NRF24L01P FIFO_STATUS register
 * @{
 */
/**
 * @brief   FIFO_STATUS register address
 */
#define NRF24L01P_REG_FIFO_STATUS          (0x17)
/**
 * @brief   FIFO_STATUS register type
 */
typedef uint8_t nrf24l01p_reg8_fifo_status_t;

/**
 * @brief   Flag: TX_REUSE
 */
#define NRF24L01P_FLG_TX_REUSE             (0b01000000)
/**
 * @brief   Value of: TX_REUSE
 */
#define NRF24L01P_VAL_TX_REUSE(reg)        (((reg) & \
                                              NRF24L01P_FLG_TX_REUSE) >> 6)
/**
 * @brief   Flag: TX_FULL
 *          (also indicated in the STATUS register)
 */
#define NRF24L01P_FLG_TX_FULL_             (0b00100000)
/**
 * @brief   Value of: TX_FULL
 */
#define NRF24L01P_VAL_TX_FULL_(reg)        (((reg) & \
                                              NRF24L01P_FLG_TX_FULL_) >> 5)
/**
 * @brief   Flag: TX_EMPTY
 */
#define NRF24L01P_FLG_TX_EMPTY             (0b00010000)
/**
 * @brief    Value of: TX_EMPTY
 */
#define NRF24L01P_VAL_TX_EMPTY(reg)        (((reg) & \
                                              NRF24L01P_FLG_TX_EMPTY) >> 4)
/**
 * @brief   Flag: RX_FULL
 */
#define NRF24L01P_FLG_RX_FULL              (0b00000010)
/**
 * @brief   Value of: RX_FULL
 */
#define NRF24L01P_VAL_RX_FULL(reg)         (((reg) & \
                                              NRF24L01P_FLG_RX_FULL) >> 1)
/**
 * @brief   Flag: RX_EMPTY
 */
#define NRF24L01P_FLG_RX_EMPTY             (0b00000001)
/**
 * @brief   Value of: RX_EMPTY
 */
#define NRF24L01P_VAL_RX_EMPTY(reg)        ((reg) & NRF24L01P_FLG_RX_EMPTY)
/** @} */

/**
 * @name    NRF24L01P DYNPD register
 *
 * Address and layout of NRF24L01P DYNPD register
 * @{
 */
/**
 * @brief   DYPD register address
 */
#define NRF24L01P_REG_DYNPD                (0x1C)
/**
 * @brief   DYNDP register type
 */
typedef uint8_t nrf24l01p_reg8_dynpd_t;
/**
 * @brief   Flag: DPL_P5
 */
#define NRF24L01P_FLG_DPL_P5               (0b00100000)
/**
 * @brief   Value of: DPL_P5
 */
#define NRF24L01P_VAL_DPL_P5(reg)          (((reg) & \
                                              NRF24L01P_FLG_DPL_P5) >> 5)
/**
 * @brief   Flag: DPL_P4
 */
#define NRF24L01P_FLG_DPL_P4               (0b00010000)
/**
 * @brief   Value of DPL_P4
 */
#define NRF24L01P_VAL_DPL_P4(reg)          (((reg) & \
                                              NRF24L01P_FLG_DPL_P4) >> 4)
/**
 * @brief   Flag: DPL_P3
 */
#define NRF24L01P_FLG_DPL_P3               (0b00001000)
/**
 * @brief   Value of DPL_P3
 */
#define NRF24L01P_VAL_DPL_P3(reg)          (((reg) & \
                                              NRF24L01P_FLG_DPL_P3) >> 3)
/**
 * @brief   Flag: DPL_P2
 */
#define NRF24L01P_FLG_DPL_P2               (0b00000100)
/**
 * @brief   Value of DPL_P2
 */
#define NRF24L01P_VAL_DPL_P2(reg)          (((reg) & \
                                              NRF24L01P_FLG_DPL_P2) >> 2)
/**
 * @brief   Flag: DPL_P1
 */
#define NRF24L01P_FLG_DPL_P1               (0b00000010)
/**
 * @brief   Value of: DPL_P1
 */
#define NRF24L01P_VAL_DPL_P1(reg)          (((reg) & \
                                              NRF24L01P_FLG_DPL_P1) >> 1)
/**
 * @brief   Flag: DPL_P0
 */
#define NRF24L01P_FLG_DPL_P0               (0b00000001)
/**
 * @brief   Value of: DPL_P0
 */
#define NRF24L01P_VAL_DPL_P0(reg)          ((reg) & NRF24L01P_FLG_DPL_P0)
/** @} */

/**
 * @name    NRF24L01P FEATURES register
 *
 * Address and layout of NRF24L01P FEATURES register
 * @{
 */
/**
 * @brief   FEATURES register address
 */
#define NRF24L01P_REG_FEATURES             (0x1D)
/**
 * @brief   FEATURES register type
 */
typedef uint8_t nrf24l01p_reg8_features_t;
/**
 * @brief   Flag: EN_DPL
 */
#define NRF24L01P_FLG_EN_DPL               (0b00000100)
/**
 * @brief   Value of: EN_DPL
 */
#define NRF24L01P_VAL_EN_DPL(reg)          (((reg) & \
                                              NRF24L01P_FLG_EN_DPL) >> 2)
/**
 * @brief   Flag: EN_ACK_PAY
 */
#define NRF24L01P_FLG_EN_ACK_PAY           (0b00000010)
/**
 * @brief   Value of: EN_ACK_PAY
 */
#define NRF24L01P_VAL_EN_ACK_PAY(reg)      (((reg) & \
                                              NRF24L01P_FLG_EN_ACK_PAY) >> 1)
/**
 * @brief   Flag: EN_DYN_ACK
 */
#define NRF24L01P_FLG_EN_DYN_ACK           (0b00000001)
/**
 * @brief   Value of: EN_DYN_ACK
 */
#define NRF24L01P_VAL_EN_DYN_ACK(reg)      ((reg) & NRF24L01P_FLG_EN_DYN_ACK)
/** @} */

/**
 * @brief   Set bits in a certain 8-bit register
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] reg_addr      Address of the register to be modified
 * @param[in, out] reg_val  Register bits that are being set
 *
 * @return                  Status register value
 */
uint8_t nrf24l01p_reg8_set(const nrf24l01p_t *dev, uint8_t reg_addr,
                           uint8_t *reg_val);

/**
 * @brief   Clear bits in a certain 8-bit register
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] reg_addr      Address of the register to be modified
 * @param[in, out] reg_val  Register bits that are being cleared
 *
 * @return                  Status register value
 */
uint8_t nrf24l01p_reg8_clear(const nrf24l01p_t *dev, uint8_t reg_addr,
                             uint8_t *reg_val);

/**
 * @brief   Modify bits in a certain 8-bit register
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] reg_addr      Address of the register to be modified
 * @param[in] mask          Mask of bits to be modified
 * @param[in, out] reg_val  Register bits that are being cleared
 *
 * @return                  Status register value
 */
uint8_t nrf24l01p_reg8_mod(const nrf24l01p_t *dev, uint8_t reg_addr,
                           uint8_t mask, uint8_t *reg_val);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_REGISTERS_H */
/** @} */
