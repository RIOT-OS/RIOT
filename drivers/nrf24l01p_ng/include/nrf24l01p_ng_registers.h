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
 * @brief   Register map of NRF24L01+ (NG) devices
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_NG_REGISTERS_H
#define NRF24L01P_NG_REGISTERS_H

#include <stdint.h>

#include "nrf24l01p_ng.h"
#include "nrf24l01p_ng_communication.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    NRF24L01+ CONFIG register
 *
 * Address and layout of NRF24L01+ configuration register
 * @{
 */
/**
 * @brief CONFIG register address
 */
#define NRF24L01P_NG_REG_CONFIG             (0x00)
/**
 * @brief   Flag: MASK_RX_DR
 */
#define NRF24L01P_NG_FLG_MASK_RX_DR         (0x40)
/**
 *@brief    Value of: MASK_RX_DR
 */
#define NRF24L01P_NG_VAL_MASK_RX_DR(reg)    (((reg) & \
                                            NRF24L01P_NG_FLG_MASK_RX_DR) >> 6)
/**
 * @brief   Flag: MASK_TX_DS
 */
#define NRF24L01P_NG_FLG_MASK_TX_DS         (0x20)
/**
 * @brief   Value of: MASK_TX_DS
 */
#define NRF24L01P_NG_VAL_MASK_TX_DS(reg)    (((reg) & \
                                            NRF24L01P_NG_FLG_MASK_TX_DS) >> 5)
/**
 * @brief   Flag: MASK_MAX_RT
 */
#define NRF24L01P_NG_FLG_MASK_MAX_RT        (0x10)
/**
 * @brief   Value of: MASK_MAX_RT
 */
#define NRF24L01P_NG_VAL_MASK_MAX_RT        (((reg) & \
                                            NRF24L01P_NG_FLG_MASK_MAX_RT) >> 4)
/**
 * @brief   Flag: EN_CRC
 */
#define NRF24L01P_NG_FLG_EN_CRC             (0x08)
/**
 * @brief   Value of: EN_CRC
 */
#define NRF24L01P_NG_VAL_EN_CRC(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_EN_CRC) >> 3)
/**
 * @brief   Flag: CRCO - 1 byte
 */
#define NRF24L01P_NG_FLG_CRCO_1_BYTE        (0x00)
/**
 * @brief   Flag: CRCO - 2 byte
 */
#define NRF24L01P_NG_FLG_CRCO_2_BYTE        (0x04)
/**
 * @brief   Mask to configure CRC
 */
#define NRF24L01P_NG_MSK_CRC                (0x0c)
/**
 * @brief    Flag: CRCO
 */
#define NRF24L01P_NG_FLG_CRCO(val)          (((val) << 2) & \
                                            NRF24L01P_NG_MSK_CRC)
/**
 * @brief   Value of: CRCO
 */
#define NRF24L01P_NG_VAL_CRCO(reg)          (((reg) & \
                                            NRF24L01P_NG_FLG_CRCO_2_BYTE) >> 2)
/**
 * @brief   Flag: PWR_UP
 */
#define NRF24L01P_NG_FLG_PWR_UP             (0x02)
/**
 * @brief   Value of: PWR_UP
 */
#define NRF24L01P_NG_VAL_PWR_UP(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_PWR_UP) >> 1)
/**
 * @brief   Flag: PRIM_RX
 */
#define NRF24L01P_NG_FLG_PRIM_RX            (0x01)
/**
 * @brief   Value of: PRIM_RX
 */
#define NRF24L01P_NG_VAL_PRIM_RX(reg)       ((reg) & NRF24L01P_NG_FLG_PRIM_RX)
/** @} */

/**
 * @name    NRF24L01+ EN_AA register
 *
 * Address and layout of NRF24L01+ EN_AA register
 * @{
 */
/**
 * @brief   EN_AA register address
 */
#define NRF24L01P_NG_REG_EN_AA              (0x01)
/**
 * @brief   Flag: ENAA_P5
 */
#define NRF24L01P_NG_FLG_ENAA_P5            (0x20)
/**
 * @brief   Value of: ENAA_P5
 */
#define NRF24L01P_NG_VAL_ENAA_P5(reg)       (((reg) & \
                                            NRF24L01P_NG_FLG_ENAA_P5) >> 5)
/**
 * @brief Flag: ENAA_P4
 */
#define NRF24L01P_NG_FLG_ENAA_P4            (0x10)
/**
 * @brief   Value of: ENAA_P4
 */
#define NRF24L01P_NG_VAL_ENAA_P4(reg)       (((reg) & \
                                            NRF24L01P_NG_FLG_ENAA_P4) >> 4)
/**
 * @brief   Flaga: ENAA_P3
 */
#define NRF24L01P_NG_FLG_ENAA_P3            (0x08)
/**
 * @brief   Value of: ENAA_P3
 */
#define NRF24L01P_NG_VAL_ENAA_P3(reg)       (((reg) & \
                                            NRF24L01P_NG_FLG_ENAA_P3) >> 3)
/**
 * @brief   Flag: ENAA_P2
 */
#define NRF24L01P_NG_FLG_ENAA_P2            (0x04)
/**
 * @brief   Value of: ENAA_P2
 */
#define NRF24L01P_NG_VAL_ENAA_P2(reg)       (((reg) & \
                                            NRF24L01P_NG_FLG_ENAA_P2) >> 2)
/**
 * @brief   Flag: ENAA_P1
 */
#define NRF24L01P_NG_FLG_ENAA_P1            (0x02)
/**
 * @brief   Value of: ENAA_P1
 */
#define NRF24L01P_NG_VAL_ENAA_P1(reg)       (((reg) & \
                                            NRF24L01P_NG_FLG_ENAA_P1) >> 1)
/**
 * @brief   Flag: ENAA_P0
 */
#define NRF24L01P_NG_FLG_ENAA_P0            (0x01)
/**
 * @brief   Value of: ENAA_P0
 */
#define NRF24L01P_NG_VAL_ENAA_P0(reg)       ((reg) & NRF24L01P_NG_FLG_ENAA_P0)
/** @} */

/**
 * @name    NRF24L01+ EN_RXADDR register
 *
 * Address and layout of NRF24L01+ EN_RXADDR register
 * @{
 */
/**
 * @brief   EN_RXADDR register address
 */
#define NRF24L01P_NG_REG_EN_RXADDR          (0x02)
/**
 * @brief   Flag: ERX_P5
 */
#define NRF24L01P_NG_FLG_ERX_P5             (0x20)
/**
 * @brief   Value of: ERX_P5
 */
#define NRF24L01P_NG_VAL_ERX_P5(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_ERX_P5) >> 5)
/**
 * @brief   Flag: ERX_P4
 */
#define NRF24L01P_NG_FLG_ERX_P4             (0x10)
/**
 * @brief   Value of ERX_P4
 */
#define NRF24L01P_NG_VAL_ERX_P4(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_ERX_P4) >> 4)
/**
 * @brief   Flag: ERX_P3
 */
#define NRF24L01P_NG_FLG_ERX_P3             (0x08)
/**
 * @brief   Value of: ERX_P3
 */
#define NRF24L01P_NG_VAL_ERX_P3(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_ERX_P3) >> 3)
/**
 * @brief   Flag: ERX_P2
 */
#define NRF24L01P_NG_FLG_ERX_P2             (0x04)
/**
 * @brief   Value of: ERX_P2
 */
#define NRF24L01P_NG_VAL_ERX_P2(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_ERX_P2) >> 2)
/**
 * @brief   Flag: ERX_P1
 */
#define NRF24L01P_NG_FLG_ERX_P1             (0x02)
/**
 * @brief   Value of: ERX_P1
 */
#define NRF24L01P_NG_VAL_ERX_P1(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_ERX_P1) >> 1)
/**
 * @brief   Flag: ERX_P0
 */
#define NRF24L01P_NG_FLG_ERX_P0             (0x01)
/**
 * @brief   Value of: ERX_P0
 */
#define NRF24L01P_NG_VAL_ERX_P0(reg)        ((reg) & NRF24L01P_NG_FLG_ERX_P0)
/** @} */

/**
 * @name    NRF24L01+ SETUP_AW register
 *
 * Address and layout of NRF24L01+ SETUP_AW register
 * @{
 */
/**
 * @brief   SETUP_AW register address
 */
#define NRF24L01P_NG_REG_SETUP_AW           (0x03)
/**
 * @brief   Flag: AW - 3 bytes
 */
#define NRF24L01P_NG_FLG_AW_3               (0x01)
/**
 * @brief   Flag: AW - 4 bytes
 */
#define NRF24L01P_NG_FLG_AW_4               (0x02)
/**
 * @brief   Flag: AW - 5 bytes
 */
#define NRF24L01P_NG_FLG_AW_5               (0x03)
/**
 * @brief   Mask to configure AW
 */
#define NRF24L01P_NG_MSK_AW                 (0x03)
/**
 * @brief   Flag AW from value [1; 3]
 */
#define NRF24L01P_NG_FLG_AW(val)            ((val) & NRF24L01P_NG_MSK_AW)
/**
 * @brief   Value of: AW
 */
#define NRF24L01P_NG_VAL_AW(reg)            ((reg) & NRF24L01P_NG_MSK_AW)
/** @} */

/**
 * @name    NRF24L01+ SETUP_RETR register
 *
 * Address and layout of NRF24L01+ SETUP_AW register
 * @{
 */
/**
 * @brief   SETUP_RETR register address
 */
#define NRF24L01P_NG_REG_SETUP_RETR         (0x04)
/**
 * @brief   Flag: ARD - 250 us
 */
#define NRF24L01P_NG_FLG_ARD_250_US         (0x00)
/**
 * @brief   Flag: ARD - 500 us
 */
#define NRF24L01P_NG_FLG_ARD_500_US         (0x10)
/**
 * @brief   Flag: ARD - 750 us
 */
#define NRF24L01P_NG_FLG_ARD_750_US         (0x20)
/**
 * @brief   Flag: ARD - 1000 us
 */
#define NRF24L01P_NG_FLG_ARD_1000_US        (0x30)
/**
 * @brief   Flag: ARD - 1250 us
 */
#define NRF24L01P_NG_FLG_ARD_1250_US        (0x40)
/**
 * @brief   Flag: ARD - 1500 us
 */
#define NRF24L01P_NG_FLG_ARD_1500_US        (0x50)
/**
 * @brief   Flag: ARD - 1750 us
 */
#define NRF24L01P_NG_FLG_ARD_1750_US        (0x60)
/**
 * @brief   Flag: ARD - 2000 us
 */
#define NRF24L01P_NG_FLG_ARD_2000_US        (0x70)
/**
 * @brief   Flag: ARD - 2250 us
 */
#define NRF24L01P_NG_FLG_ARD_2250_US        (0x80)
/**
 * @brief   Flag: ARD - 2500 us
 */
#define NRF24L01P_NG_FLG_ARD_2500_US        (0x90)
/**
 * @brief   Flag: ARD - 2750 us
 */
#define NRF24L01P_NG_FLG_ARD_2750_US        (0xa0)
/**
 * @brief   Flag: ARD - 3000 us
 */
#define NRF24L01P_NG_FLG_ARD_3000_US        (0xb0)
/**
 * @brief   Flag: ARD - 3250 us
 */
#define NRF24L01P_NG_FLG_ARD_3250_US        (0xc0)
/**
 * @brief   Flag: ARD - 3500 us
 */
#define NRF24L01P_NG_FLG_ARD_3500_US        (0xd0)
/**
 * @brief   Flag: ARD - 3750 us
 */
#define NRF24L01P_NG_FLG_ARD_3750_US        (0xe0)
/**
 * @brief   Flag: ARD - 4000 us
 */
#define NRF24L01P_NG_FLG_ARD_4000_US        (0xf0)
/**
 * @brief   Mask to configure ARD
 */
#define NRF24L01P_NG_MSK_ARD                (0xf0)
/**
 * @brief   Flag ARD from value [0; 15]
 */
#define NRF24L01P_NG_FLG_ARD(val)           (((val) << 4) & \
                                            NRF24L01P_NG_MSK_ARD)
/**
 * @brief   Value of: ARD
 */
#define NRF24L01P_NG_VAL_ARD(reg)           (((reg) & \
                                            NRF24L01P_NG_MSK_ARD) >> 4)
/**
 * @brief Mask to configure ARC
 */
#define NRF24L01P_NG_MSK_ARC                (0x0f)
/**
 * @brief   Flag ARC from value [0; 15]
 */
#define NRF24L01P_NG_FLG_ARC(val)           ((val) & NRF24L01P_NG_MSK_ARC)
/**
 * @brief   Value of: ARC
 */
#define NRF24L01P_NG_VAL_ARC(reg)           ((reg) & NRF24L01P_NG_MSK_ARC)
/** @} */

/**
 * @name    NRF24L01+ RF_CH register
 *
 * Address and layout of NRF24L01+ SETUP_AW register
 * @{
 */
/**
 * @brief   RF_CH register address
 */
#define NRF24L01P_NG_REG_RF_CH              (0x05)
/**
 * @brief   Mask to configure RF_CH
 */
#define NRF24L01P_NG_MSK_RF_CH              (0x7f)
/**
 * @brief   Flag RF_CH from value [0; 124]
 */
#define NRF24L01P_NG_FLG_RF_CH(val)         ((val) & NRF24L01P_NG_MSK_RF_CH)
/**
 * @brief   Value of: RF_CH
 */
#define NRF24L01P_NG_VAL_RF_CH(reg)         ((reg) & NRF24L01P_NG_MSK_RF_CH)
/** @} */

/**
 * @name    NRF24L01+ RF_SETUP register
 *
 * Address and layout of NRF24L01+ RF_SETUP register
 * @{
 */
/**
 * @brief   RF_SETUP register address
 */
#define NRF24L01P_NG_REG_RF_SETUP           (0x06)
/**
 * @brief   Flag: CONT_WAVE
 */
#define NRF24L01P_NG_FLG_CONT_WAVE          (0x80)
/**
 * @brief   Value of: CONT_WAVE
 */
#define NRF24L01P_NG_VAL_CONT_WAVE(reg)     (((reg) & \
                                            NRF24L01P_NG_FLG_CONT_WAVE) >> 7)
/**
 * @brief   Flag: RF_DR_LOW
 */
#define NRF24L01P_NG_FLG_RF_DR_LOW          (0x20)
/**
 * @brief   Value of: RF_DR_LOW
 */
#define NRF24L01P_NG_VAL_RF_DR_LOW(reg)     (((reg) & \
                                            NRF24L01P_NG_FLG_RF_DR_LOW) >> 5)
/**
 * @brief   Flag: PLL_LOCK
 */
#define NRF24L01P_NG_FLG_PLL_LOCK           (0x10)
/**
 * @brief   Value of: PLL_LOCK
 */
#define NRF24L01P_NG_VAL_PLL_LOCK(reg)      (((reg) & \
                                            NRF24L01P_NG_FLG_PLL_LOCK) >> 4)
/**
 * @brief   Flag: RF_DR_HIGH
 */
#define NRF24L01P_NG_FLG_RF_DR_HIGH         (0x08)
/**
 * @brief   Value of: RF_DR_HIGH
 */
#define NRF24L01P_NG_VAL_RF_DR_HIGH(reg)    (((reg) & \
                                            NRF24L01P_NG_FLG_RF_DR_HIGH) >> 3)
/**
 * @brief   Flag: RF_DR - 250 kbit/s
 */
#define NRF24L01P_NG_FLG_RF_DR_250_KBPS     (0x08)
/**
 * @brief   Flag: RF_DR - 1000 kbit/s
 */
#define NRF24L01P_NG_FLG_RF_DR_1_MBPS       (0x00)
/**
 * @brief   Flag: RF_DR - 2000 kbit/s
 */
#define NRF24L01P_NG_FLG_RF_DR_2_MBPS       (0x20)
/**
 * @brief   Mask to configure RF_DR
 */
#define NRF24L01P_NG_MSK_RF_DR              (0x28)
/**
 * @brief   FLG RF_DR
 */
#define NRF24L01P_NG_FLG_RF_DR(val)         ((((val) & 1) << 5) | \
                                            (((val) & 2) << 2))
/**
 * @brief   Value of: RF_DR
 */
#define NRF24L01P_NG_VAL_RF_DR(reg)         ((((reg) & \
                                            NRF24L01P_NG_FLG_RF_DR_LOW) >> 5) \
                                            | \
                                            (((reg) & \
                                            NRF24L01P_NG_FLG_RF_DR_HIGH) >> 2))
/**
 * @brief   Flag: RF_PWR - -18 dbm
 */
#define NRF24L01P_NG_FLG_RF_PWR_MINUS_18    (0x00)
/**
 * @brief   Flag: RF_PWR - -12 dbm
 */
#define NRF24L01P_NG_FLG_RF_PWR_MINUS_12    (0x02)
/**
 * @brief   Flag: RF_PWR - -6 dbm
 */
#define NRF24L01P_NG_FLG_RF_PWR_MINUS_6     (0x04)
/**
 * @brief   Flag: RF_PWR - 0 dbm
 */
#define NRF24L01P_NG_FLG_RF_PWR_0           (0x06)
/**
 * @brief   Mask to configure RF_PWR
 */
#define NRF24L01P_NG_MSK_RF_PWR             (0x06)
/**
 * @brief   Flag RF_PWR from value [0; 3]
 */
#define NRF24L01P_NG_FLG_RF_PWR(val)        (((val) << 1) & \
                                            NRF24L01P_NG_MSK_RF_PWR)
/**
 * @brief   Value of: RF_PWR
 */
#define NRF24L01P_NG_VAL_RF_PWR(reg)        (((reg) & \
                                            NRF24L01P_NG_MSK_RF_PWR) >> 1)
/** @} */

/**
 * @name    NRF24L01+ STATUS register
 *
 * Address and layout of NRF24L01+ STATUS register
 * @{
 */
/**
 * @brief   STATUS register address
 */
#define NRF24L01P_NG_REG_STATUS             (0x07)
/**
 * @brief   Flag: RX_DR
 */
#define NRF24L01P_NG_FLG_RX_DR              (0x40)
/**
 * @brief    Value of: RX_DR
 */
#define NRF24L01P_NG_VAL_RX_DR(reg)         (((reg) & \
                                            NRF24L01P_NG_FLG_RX_DR) >> 6)
/**
 * @brief   Flag: TX_DS
 */
#define NRF24L01P_NG_FLG_TX_DS              (0x20)
/**
 * @brief   Value of: TX_DS
 */
#define NRF24L01P_NG_VAL_TX_DS(reg)         (((reg) & \
                                            NRF24L01P_NG_FLG_TX_DS) >> 5)
/**
 * @brief   Flag: MAX_RT
 */
#define NRF24L01P_NG_FLG_MAX_RT             (0x10)
/**
 * @brief   Value of: MAX_RT
 */
#define NRF24L01P_NG_VAL_MAX_RT(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_MAX_RT) >> 4)

/**
 * @brief   Flag: RX_P_NO - Rx FIFO empty
 */
#define NRF24L01P_NG_FLG_RX_P_NO_NONE       (0x0e)
/**
 * @brief   Mask to read RX_P_NO
 */
#define NRF24L01P_NG_MSK_RX_P_NO            (0x0e)
/**
 * @brief   Flag RX_P_NO from value [0; 7]
 */
#define NRF24L01P_NG_FLG_RX_P_NO(val)       (((val) << 1) & \
                                            NRF24L01P_NG_MSK_RX_P_NO)
/**
 * @brief   Value of: RX_P_NO
 */
#define NRF24L01P_NG_VAL_RX_P_NO(reg)       (((reg) & \
                                            NRF24L01P_NG_MSK_RX_P_NO) >> 1)

/**
 * @brief   Flag: TX_FULL
 */
#define NRF24L01P_NG_FLG_TX_FULL            (0x01)
/**
 * @brief   Value of: TX_FULL
 */
#define NRF24L01P_NG_VAL_TX_FULL(reg)       ((reg) & NRF24L01P_NG_FLG_TX_FULL)
/** @} */

/**
 * @name    NRF24L01+ OBSERVE_TX register
 *
 * Address and layout of NRF24L01+ STATUS register
 * @{
 */
/**
 * @brief   OBSERVE_TX register address
 */
#define NRF24L01P_NG_REG_OBSERVE_TX         (0x08)
/**
 * @brief   Mask to read PLOS_CNT
 */
#define NRF24L01P_NG_MSK_PLOS_CNT           (0xf0)
/**
 * @brief   Flag PLOS_CNT from value [0; 15]
 */
#define NRF24L01P_NG_FLG_PLOS_CNT(val)      (((val) << 4) & \
                                            NRF24L01P_NG_MSK_PLOS_CNT)
/**
 * @brief   Value of: PLOS_CNT
 */
#define NRF24L01P_NG_VAL_PLOS_CNT(reg)      (((reg) & \
                                            NRF24L01P_NG_MSK_PLOS_CNT) >> 4)

/**
 * @brief Mask to read ARC_CNT
 */
#define NRF24L01P_NG_MSK_ARC_CNT            (0x0f)
/**
 * @brief   Flag ARC_CNT from value [0; 15]
 */
#define NRF24L01P_NG_FLG_ARC_CNT(val)       ((val) & NRF24L01P_NG_MSK_ARC_CNT)
/**
 * @brief   Value of: ARC_CNT
 */
#define NRF24L01P_NG_VAL_ARC_CNT(reg)       ((reg) & NRF24L01P_NG_MSK_ARC_CNT)
/** @} */

/**
 * @name    NRF24L01+ RPD register
 *
 * Address and layout of NRF24L01+ RPD register
 * @{
 */
/**
 * @brief   RPD register address
 */
#define NRF24L01P_NG_REG_RPD                (0x09)
/**
 * @brief   Flag: RPD
 */
#define NRF24L01P_NG_FLG_RPD                (0x01)
/**
 * @brief   Value of: RPD
 */
#define NRF24L01P_NG_VAL_RPD(reg)           ((reg) & NRF24L01P_NG_FLG_RPD)
/** @} */

/**
 * @name    NRF24L01+ RX_ADDR_Px registers
 *
 * Addresses of NRF24L01+ RX_ADDR_Px registers and TX_ADDR register
 * @{
 */
/**
 * @brief   RX_ADDR_P0 register address
 */
#define NRF24L01P_NG_REG_RX_ADDR_P0         (0x0A)
/**
 * @brief   RX_ADDR_P1 register address
 */
#define NRF24L01P_NG_REG_RX_ADDR_P1         (0x0B)
/**
 * @brief   RX_ADDR_P2 register address
 */
#define NRF24L01P_NG_REG_RX_ADDR_P2         (0x0C)
/**
 * @brief   RX_ADDR_P3 register address
 */
#define NRF24L01P_NG_REG_RX_ADDR_P3         (0x0D)
/**
 * @brief   RX_ADDR_P4 register address
 */
#define NRF24L01P_NG_REG_RX_ADDR_P4         (0x0E)
/**
 * @brief   RX_ADDR_P5 register address
 */
#define NRF24L01P_NG_REG_RX_ADDR_P5         (0x0F)
/**
 * @brief   TX_ADDR register address
 */
#define NRF24L01P_NG_REG_TX_ADDR            (0x10)
/** @} */

/**
 * @name    NRF24L01+ RX_PW_Px registers
 *
 * Address and layout of NRF24L01+ RX_PW_Px registers
 * @{
 */
/**
 * @brief   RX_PW_P0 register address
 */
#define NRF24L01P_NG_REG_RX_PW_P0           (0x11)
/**
 * @brief   RX_PW_P1 register address
 */
#define NRF24L01P_NG_REG_RX_PW_P1           (0x12)
/**
 * @brief   RX_PW_P2 register address
 */
#define NRF24L01P_NG_REG_RX_PW_P2           (0x13)
/**
 * @brief   RX_PW_P3 register address
 */
#define NRF24L01P_NG_REG_RX_PW_P3           (0x14)
/**
 * @brief   RX_PW_P4 register address
 */
#define NRF24L01P_NG_REG_RX_PW_P4           (0x15)
/**
 * @brief   RX_PW_P5 register address
 */
#define NRF24L01P_NG_REG_RX_PW_P5           (0x16)
/**
 * @brief   Mask to configure RX_PW_PX
 */
#define NRF24L01P_NG_MSK_RX_PW_PX           (0x1f)
/**
 * @brief   Flag RX_PW_Px from value [0; 32]
 */
#define NRF24L01P_NG_FLG_RX_PW_PX(val)      ((val) & NRF24L01P_NG_MSK_RX_PW_PX)
/**
 * @brief   Value of: RX_PW_PX
 */
#define NRF24L01P_NG_VAL_RX_PW_PX(reg)      ((reg) & NRF24L01P_NG_MSK_RX_PW_PX)
/** @} */

/**
 * @name    NRF24L01+ FIFO_STATUS register
 *
 * Address and layout of NRF24L01+ FIFO_STATUS register
 * @{
 */
/**
 * @brief   FIFO_STATUS register address
 */
#define NRF24L01P_NG_REG_FIFO_STATUS        (0x17)
/**
 * @brief   Flag: TX_REUSE
 */
#define NRF24L01P_NG_FLG_TX_REUSE           (0x40)
/**
 * @brief   Value of: TX_REUSE
 */
#define NRF24L01P_NG_VAL_TX_REUSE(reg)      (((reg) & \
                                            NRF24L01P_NG_FLG_TX_REUSE) >> 6)
/**
 * @brief   Flag: TX_FULL
 *          (also indicated in the STATUS register)
 */
#define NRF24L01P_NG_FLG_TX_FULL_           (0x20)
/**
 * @brief   Value of: TX_FULL
 */
#define NRF24L01P_NG_VAL_TX_FULL_(reg)      (((reg) & \
                                            NRF24L01P_NG_FLG_TX_FULL_) >> 5)
/**
 * @brief   Flag: TX_EMPTY
 */
#define NRF24L01P_NG_FLG_TX_EMPTY           (0x10)
/**
 * @brief    Value of: TX_EMPTY
 */
#define NRF24L01P_NG_VAL_TX_EMPTY(reg)      (((reg) & \
                                            NRF24L01P_NG_FLG_TX_EMPTY) >> 4)
/**
 * @brief   Flag: RX_FULL
 */
#define NRF24L01P_NG_FLG_RX_FULL            (0x02)
/**
 * @brief   Value of: RX_FULL
 */
#define NRF24L01P_NG_VAL_RX_FULL(reg)       (((reg) & \
                                            NRF24L01P_NG_FLG_RX_FULL) >> 1)
/**
 * @brief   Flag: RX_EMPTY
 */
#define NRF24L01P_NG_FLG_RX_EMPTY           (0x01)
/**
 * @brief   Value of: RX_EMPTY
 */
#define NRF24L01P_NG_VAL_RX_EMPTY(reg)      ((reg) & NRF24L01P_NG_FLG_RX_EMPTY)
/** @} */

/**
 * @name    NRF24L01+ DYNPD register
 *
 * Address and layout of NRF24L01+ DYNPD register
 * @{
 */
/**
 * @brief   DYPD register address
 */
#define NRF24L01P_NG_REG_DYNPD              (0x1C)
/**
 * @brief   Flag: DPL_P5
 */
#define NRF24L01P_NG_FLG_DPL_P5             (0x20)
/**
 * @brief   Value of: DPL_P5
 */
#define NRF24L01P_NG_VAL_DPL_P5(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_DPL_P5) >> 5)
/**
 * @brief   Flag: DPL_P4
 */
#define NRF24L01P_NG_FLG_DPL_P4             (0x10)
/**
 * @brief   Value of DPL_P4
 */
#define NRF24L01P_NG_VAL_DPL_P4(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_DPL_P4) >> 4)
/**
 * @brief   Flag: DPL_P3
 */
#define NRF24L01P_NG_FLG_DPL_P3             (0x08)
/**
 * @brief   Value of DPL_P3
 */
#define NRF24L01P_NG_VAL_DPL_P3(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_DPL_P3) >> 3)
/**
 * @brief   Flag: DPL_P2
 */
#define NRF24L01P_NG_FLG_DPL_P2             (0x04)
/**
 * @brief   Value of DPL_P2
 */
#define NRF24L01P_NG_VAL_DPL_P2(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_DPL_P2) >> 2)
/**
 * @brief   Flag: DPL_P1
 */
#define NRF24L01P_NG_FLG_DPL_P1             (0x02)
/**
 * @brief   Value of: DPL_P1
 */
#define NRF24L01P_NG_VAL_DPL_P1(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_DPL_P1) >> 1)
/**
 * @brief   Flag: DPL_P0
 */
#define NRF24L01P_NG_FLG_DPL_P0             (0x01)
/**
 * @brief   Value of: DPL_P0
 */
#define NRF24L01P_NG_VAL_DPL_P0(reg)        ((reg) & NRF24L01P_NG_FLG_DPL_P0)
/** @} */

/**
 * @name    NRF24L01+ FEATURES register
 *
 * Address and layout of NRF24L01+ FEATURES register
 * @{
 */
/**
 * @brief   FEATURES register address
 */
#define NRF24L01P_NG_REG_FEATURES           (0x1D)
/**
 * @brief   Flag: EN_DPL
 */
#define NRF24L01P_NG_FLG_EN_DPL             (0x04)
/**
 * @brief   Value of: EN_DPL
 */
#define NRF24L01P_NG_VAL_EN_DPL(reg)        (((reg) & \
                                            NRF24L01P_NG_FLG_EN_DPL) >> 2)
/**
 * @brief   Flag: EN_ACK_PAY
 */
#define NRF24L01P_NG_FLG_EN_ACK_PAY         (0x02)
/**
 * @brief   Value of: EN_ACK_PAY
 */
#define NRF24L01P_NG_VAL_EN_ACK_PAY(reg)    (((reg) & \
                                            NRF24L01P_NG_FLG_EN_ACK_PAY) >> 1)
/**
 * @brief   Flag: EN_DYN_ACK
 */
#define NRF24L01P_NG_FLG_EN_DYN_ACK         (0x01)
/**
 * @brief   Value of: EN_DYN_ACK
 */
#define NRF24L01P_NG_VAL_EN_DYN_ACK(reg)    ((reg) & \
                                            NRF24L01P_NG_FLG_EN_DYN_ACK)
/** @} */

/**
 * @brief   Read the contents of an 8 bit register
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] reg_addr      Address of the register to be written
 *
 * @return                  Register value
 */
static inline
uint8_t nrf24l01p_ng_reg8_read(const nrf24l01p_ng_t *dev, uint8_t reg_addr) {
    uint8_t reg_val;
    nrf24l01p_ng_read_reg(dev, reg_addr, &reg_val, 1);
    return reg_val;
}

/**
 * @brief   Write the contents of an 8 bit register
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] reg_addr      Address of the register to be written
 * @param[in] reg_val       Value to be written to the register
 *
 * @return                  Status register value
 */
static inline
uint8_t nrf24l01p_ng_reg8_write(const nrf24l01p_ng_t *dev,
                                uint8_t reg_addr, uint8_t reg_val) {
    return nrf24l01p_ng_write_reg(dev, reg_addr, &reg_val, 1);
}

/**
 * @brief   Set bits in a certain 8-bit register
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] reg_addr      Address of the register to be modified
 * @param[in, out] reg_val  Register bits that are being set
 *
 * @return                  Status register value
 */
static inline
uint8_t nrf24l01p_ng_reg8_set(const nrf24l01p_ng_t *dev,
                              uint8_t reg_addr, uint8_t *reg_val) {
    uint8_t reg_val_old;
    nrf24l01p_ng_read_reg(dev, reg_addr, &reg_val_old, sizeof(reg_val_old));
    *reg_val = reg_val_old | *reg_val;
    return nrf24l01p_ng_write_reg(dev, reg_addr, reg_val, sizeof(*reg_val));
}
/**
 * @brief   Clear bits in a certain 8-bit register
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] reg_addr      Address of the register to be modified
 * @param[in, out] reg_val  Register bits that are being cleared
 *
 * @return                  Status register value
 */
static inline
uint8_t nrf24l01p_ng_reg8_clear(const nrf24l01p_ng_t *dev,
                                uint8_t reg_addr, uint8_t *reg_val) {
    uint8_t reg_val_old;
    nrf24l01p_ng_read_reg(dev, reg_addr, &reg_val_old, sizeof(reg_val_old));
    *reg_val = reg_val_old &= ~(*reg_val);
    return nrf24l01p_ng_write_reg(dev, reg_addr, reg_val, sizeof(*reg_val));
}

/**
 * @brief   Modify bits in a certain 8-bit register
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] reg_addr      Address of the register to be modified
 * @param[in] mask          Mask of bits to be modified
 * @param[in, out] reg_val  Register bits that are being modified
 *
 * @return                  Status register value
 */
static inline
uint8_t nrf24l01p_ng_reg8_mod(const nrf24l01p_ng_t *dev, uint8_t reg_addr,
                              uint8_t mask, uint8_t *reg_val) {
    uint8_t reg_val_old;
    nrf24l01p_ng_read_reg(dev, reg_addr, &reg_val_old, sizeof(reg_val_old));
    reg_val_old &= ~mask;
    *reg_val = reg_val_old | *reg_val;
    return nrf24l01p_ng_write_reg(dev, reg_addr, reg_val, sizeof(*reg_val));
}

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_NG_REGISTERS_H */
/** @} */
