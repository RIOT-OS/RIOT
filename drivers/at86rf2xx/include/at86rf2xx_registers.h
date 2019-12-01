/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Register and command definitions for AT86RF2xx devices
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 */

#ifndef AT86RF2XX_REGISTERS_H
#define AT86RF2XX_REGISTERS_H

#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_AT86RFA1)
#include <avr/io.h>

#define AT86RFA1_REG__TST_CTRL_DIGI                            (&TST_CTRL_DIGI)
#define AT86RFA1_REG__TRXFBST                                  (&TRXFBST)
#define AT86RFA1_REG__TRXFBEND                                 (&TRXFBEND)
#define AT86RFA1_REG__TRXPR                                    (&TRXPR)

/**
 * @brief   AT86RF2XX transceiver register address offset
 *          for ATmega128RFA1
 */
#define AT86RFA1_REGISTER_OFFSET            (0x140)

/**
 * @brief AT86RFA1: access common register
 */
#define AT86RFA1_REG(reg)       (&(_SFR_MEM8((reg) + AT86RFA1_REGISTER_OFFSET)))
#endif

#if IS_USED(MODULE_AT86RFR2)
#include <avr/io.h>

/**
 * @name    Additional AT86RFR2 MCU register addresses
 * @{
 */
#define AT86RFR2_REG__IRQ_MASK1                                (&IRQ_MASK1)
#define AT86RFR2_REG__IRQ_STATUS1                              (&IRQ_STATUS1)
#define AT86RFR2_REG__TST_CTRL_DIGI                            (&TST_CTRL_DIGI)
#define AT86RFR2_REG__TRXFBST                                  (&TRXFBST)
#define AT86RFR2_REG__TRXFBEND                                 (&TRXFBEND)
#define AT86RFR2_REG__TRXPR                                    (&TRXPR)
/** @} */

/**
 * @brief   AT86RF2XX transceiver register address offset
 *          for ATmega256RFR2
 */
#define AT86RFR2_REGISTER_OFFSET            (0x140)

/**
 * @brief AT86RFR2: access common register
 */
#define AT86RFR2_REG(reg)       (&(_SFR_MEM8((reg) + AT86RFR2_REGISTER_OFFSET)))
#endif

/**
 * @name    Register addresses
 * @{
 */
#define AT86RF2XX_REG__TRX_STATUS                               (0x01)  /* all */
#define AT86RF2XX_REG__TRX_STATE                                (0x02)  /* all */
#define AT86RF2XX_REG__TRX_CTRL_0                               (0x03)  /* all */
#define AT86RF2XX_REG__TRX_CTRL_1                               (0x04)  /* all */
#define AT86RF2XX_REG__PHY_TX_PWR                               (0x05)  /* all */
#define AT86RF2XX_REG__PHY_RSSI                                 (0x06)  /* all */
#define AT86RF2XX_REG__PHY_ED_LEVEL                             (0x07)  /* all */
#define AT86RF2XX_REG__PHY_CC_CCA                               (0x08)  /* all */
#define AT86RF2XX_REG__CCA_THRES                                (0x09)  /* all */
#define AT86RF2XX_REG__RX_CTRL                                  (0x0A)  /* all */
#define AT86RF2XX_REG__SFD_VALUE                                (0x0B)  /* all */
#define AT86RF2XX_REG__TRX_CTRL_2                               (0x0C)  /* all */
#define AT86RF2XX_REG__ANT_DIV                                  (0x0D)  /* all */
#define AT86RF2XX_REG__IRQ_MASK                                 (0x0E)  /* all */
#define AT86RF2XX_REG__IRQ_STATUS                               (0x0F)  /* all */
#define AT86RF2XX_REG__VREG_CTRL                                (0x10)  /* all */
#define AT86RF2XX_REG__BATMON                                   (0x11)  /* all */
#define AT86RF2XX_REG__XOSC_CTRL                                (0x12)  /* all */
#define AT86RF2XX_REG__CC_CTRL_0                                (0x13)  /* AT86RF212B, AT86RF233, AT86RFR2 */
#define AT86RF2XX_REG__CC_CTRL_1                                (0x14)  /* AT86RF212B, AT86RF233, AT86RFR2 */
#define AT86RF2XX_REG__RX_SYN                                   (0x15)  /* all */
#define AT86RF2XX_REG__RF_CTRL_0                                (0x16)  /* AT86RF212B */
#define AT86RF2XX_REG__TRX_RPC                                  (0x16)  /* AT86RF233, AT86RFR2 */
#define AT86RF2XX_REG__XAH_CTRL_1                               (0x17)  /* all */
#define AT86RF2XX_REG__FTN_CTRL                                 (0x18)  /* all */
#define AT86RF2XX_REG__XAH_CTRL_2                               (0x19)  /* At86RF232, AT86RF233 */
#define AT86RF2XX_REG__PLL_CF                                   (0x1A)  /* all */
#define AT86RF2XX_REG__PLL_DCU                                  (0x1B)  /* all */
#define AT86RF2XX_REG__PART_NUM                                 (0x1C)  /* all */
#define AT86RF2XX_REG__VERSION_NUM                              (0x1D)  /* all */
#define AT86RF2XX_REG__MAN_ID_0                                 (0x1E)  /* all */
#define AT86RF2XX_REG__MAN_ID_1                                 (0x1F)  /* all */
#define AT86RF2XX_REG__SHORT_ADDR_0                             (0x20)  /* all */
#define AT86RF2XX_REG__SHORT_ADDR_1                             (0x21)  /* all */
#define AT86RF2XX_REG__PAN_ID_0                                 (0x22)  /* all */
#define AT86RF2XX_REG__PAN_ID_1                                 (0x23)  /* all */
#define AT86RF2XX_REG__IEEE_ADDR_0                              (0x24)  /* all */
#define AT86RF2XX_REG__IEEE_ADDR_1                              (0x25)  /* all */
#define AT86RF2XX_REG__IEEE_ADDR_2                              (0x26)  /* all */
#define AT86RF2XX_REG__IEEE_ADDR_3                              (0x27)  /* all */
#define AT86RF2XX_REG__IEEE_ADDR_4                              (0x28)  /* all */
#define AT86RF2XX_REG__IEEE_ADDR_5                              (0x29)  /* all */
#define AT86RF2XX_REG__IEEE_ADDR_6                              (0x2A)  /* all */
#define AT86RF2XX_REG__IEEE_ADDR_7                              (0x2B)  /* all */
#define AT86RF2XX_REG__XAH_CTRL_0                               (0x2C)  /* all */
#define AT86RF2XX_REG__CSMA_SEED_0                              (0x2D)  /* all */
#define AT86RF2XX_REG__CSMA_SEED_1                              (0x2E)  /* all */
#define AT86RF2XX_REG__CSMA_BE                                  (0x2F)  /* all */
#define AT86RF2XX_REG__TST_CTRL_DIGI                            (0x36)  /* AT86RF232, AT86RF233 */
#define AT86RF2XX_REG__TST_AGC                                  (0x3C)  /* AT86RF233 */
#define AT86RF2XX_REG__TST_SDM                                  (0x3D)  /* AT86RF233 */
#define AT86RF2XX_REG__PHY_TX_TIME                              (0x3B)  /* AT86RF233 */
#define AT86RF2XX_REG__PHY_PMU_VALUE                            (0x3B)  /* AT86RF233 */
/** @} */

/**
 * @name   Bitfield definitions for the IRQ_MASK1 register
 * @{
 */
#define AT86RF2XX_IRQ_MASK1__TX_START                           (0x01)  /* AT86RFR2 */
#define AT86RF2XX_IRQ_MASK1__MAF_0_AMI                          (0x02)  /* AT86RFR2 */
#define AT86RF2XX_IRQ_MASK1__MAF_1_AMI                          (0x04)  /* AT86RFR2 */
#define AT86RF2XX_IRQ_MASK1__MAF_2_AMI                          (0x08)  /* AT86RFR2 */
#define AT86RF2XX_IRQ_MASK1__MAF_3_AMI                          (0x10)  /* AT86RFR2 */
/** @} */

/**
 * @name   Bitfield definitions for the IRQ_STATUS1 register
 * @{
 */
#define AT86RF2XX_IRQ_STATUS1__TX_START                         (0x01)  /* AT86RFR2 */
#define AT86RF2XX_IRQ_STATUS1__MAF_0_AMI                        (0x02)  /* AT86RFR2 */
#define AT86RF2XX_IRQ_STATUS1__MAF_1_AMI                        (0x04)  /* AT86RFR2 */
#define AT86RF2XX_IRQ_STATUS1__MAF_2_AMI                        (0x08)  /* AT86RFR2 */
#define AT86RF2XX_IRQ_STATUS1__MAF_3_AMI                        (0x10)  /* AT86RFR2 */
/** @} */

/**
 * @name    Bitfield definitions for the TRXPR register
 * @{
 */
#define AT86RF2XX_TRXPR_MASK__ATBE                              (0x08)  /* AT86RFR2 */
#define AT86RF2XX_TRXPR_MASK__TRXTST                            (0x04)  /* AT86RFR2 */
#define AT86RF2XX_TRXPR_MASK__SLPTR                             (0x02)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_TRXPR_MASK__TRXRST                            (0x01)  /* AT86RFA1, AT86RFR2 */
/** @} */

/**
 * @name    Bitfield definitions for the TRX_STATUS register
 * @{
 */
#define AT86RF2XX_TRX_STATUS_MASK__CCA_DONE                     (0x80)  /* all */
#define AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS                   (0x40)  /* all */
#define AT86RF2XX_TRX_STATUS_MASK__TST_STATUS                   (0x20)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS                   (0x1F)  /* all */

#define AT86RF2XX_TRX_STATUS__P_ON                              (0x00)
#define AT86RF2XX_TRX_STATUS__BUSY_RX                           (0x01)
#define AT86RF2XX_TRX_STATUS__BUSY_TX                           (0x02)
#define AT86RF2XX_TRX_STATUS__RX_ON                             (0x06)
#define AT86RF2XX_TRX_STATUS__TRX_OFF                           (0x08)
#define AT86RF2XX_TRX_STATUS__PLL_ON                            (0x09)
#define AT86RF2XX_TRX_STATUS__SLEEP                             (0x0F)
#define AT86RF2XX_TRX_STATUS__BUSY_RX_AACK                      (0x11)
#define AT86RF2XX_TRX_STATUS__BUSY_TX_ARET                      (0x12)
#define AT86RF2XX_TRX_STATUS__RX_AACK_ON                        (0x16)
#define AT86RF2XX_TRX_STATUS__TX_ARET_ON                        (0x19)
#define AT86RF2XX_TRX_STATUS__RX_ON_NOCLK                       (0x1C)
#define AT86RF2XX_TRX_STATUS__RX_AACK_ON_NOCLK                  (0x1D)
#define AT86RF2XX_TRX_STATUS__BUSY_RX_AACK_NOCLK                (0x1E)
#define AT86RF2XX_TRX_STATUS__STATE_TRANSITION_IN_PROGRESS      (0x1F)
/** @} */

/**
 * @name    Bitfield definitions for the TRX_STATE register
 * @{
 */
#define AT86RF2XX_TRX_STATE_MASK__TRAC_STATUS                   (0xE0)  /* all */
#define AT86RF2XX_TRX_STATE_MASK__TRX_CMD                       (0x1F)  /* all */

#define AT86RF2XX_TRX_STATE__NOP                                (0x00)
#define AT86RF2XX_TRX_STATE__TX_START                           (0x02)
#define AT86RF2XX_TRX_STATE__FORCE_TRX_OFF                      (0x03)
#define AT86RF2XX_TRX_STATE__FORCE_PLL_ON                       (0x04)
#define AT86RF2XX_TRX_STATE__RX_ON                              (0x06)
#define AT86RF2XX_TRX_STATE__TRX_OFF                            (0x08)
#define AT86RF2XX_TRX_STATE__PLL_ON                             (0x09)
#define AT86RF2XX_TRX_STATE__RX_AACK_ON                         (0x16)
#define AT86RF2XX_TRX_STATE__TX_ARET_ON                         (0x19)
#define AT86RF2XX_TRX_STATE__TRAC_SUCCESS                       (0x00)
#define AT86RF2XX_TRX_STATE__TRAC_SUCCESS_DATA_PENDING          (0x20)
#define AT86RF2XX_TRX_STATE__TRAC_SUCCESS_WAIT_FOR_ACK          (0x40)
#define AT86RF2XX_TRX_STATE__TRAC_CHANNEL_ACCESS_FAILURE        (0x60)
#define AT86RF2XX_TRX_STATE__TRAC_NO_ACK                        (0xA0)
#define AT86RF2XX_TRX_STATE__TRAC_INVALID                       (0xE0)
/** @} */

/**
 * @name    Bitfield definitions for the TRX_CTRL_0 register
 * @{
 */
#define AT86RF2XX_TRX_CTRL_0_MASK__PAD_IO                       (0xC0)  /* AT86RF212B, AT86RF231, AT86RF232 */
#define AT86RF2XX_TRX_CTRL_0_MASK__PAD_IO_CLKM                  (0x30)  /* AT86RF212B, AT86RF231, AT86RF232 */
#define AT86RF2XX_TRX_CTRL_0_MASK__TOM_EN                       (0x80)  /* AT86RF233 */
#if IS_USED(MODULE_AT86RF233)
#define AT86RF233_TRX_CTRL_0_MASK__PMU_EN                       (0x20)  /* AT86RF233 [5, 5] */
#endif
#if IS_USED(MODULE_AT86RFR2)
#define AT86RFR2_TRX_CTRL_0_MASK__PMU_EM                        (0x40)  /* AT86RFR2 [6, 6] */
#endif
#define AT86RF2XX_TRX_CTRL_0_MASK__PMU_START                    (0x20)  /* AT86RFR2 */
#define AT86RF2XX_TRX_CTRL_0_MASK__PMU_IF_INVERSE               (0x10)  /* AT86RF233, AT86RFR2 */
#define AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL                 (0x08)  /* AT86RF212B, AT86RF231, AT86RF232, AT86Rf233 */
#define AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL                    (0x07)  /* AT86RF212B, AT86RF231, AT86RF232, AT86Rf233 */

#define AT86RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO                    (0x00)
#define AT86RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO_CLKM               (0x10)
#define AT86RF2XX_TRX_CTRL_0_DEFAULT__CLKM_SHA_SEL              (0x08)
#define AT86RF2XX_TRX_CTRL_0_DEFAULT__CLKM_CTRL                 (0x01)

#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF                     (0x00)
#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__1MHz                    (0x01)
#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__2MHz                    (0x02)
#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__4MHz                    (0x03)
#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__8MHz                    (0x04)
#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__16MHz                   (0x05)
#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__250kHz                  (0x06)
#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__62_5kHz                 (0x07)
/** @} */

/**
 * @name    Bitfield definitions for the TRX_CTRL_1 register
 * @{
 */
#define AT86RF2XX_TRX_CTRL_1_MASK__PA_EXT_EN                    (0x80)  /* AT86RF212B, AT86RF231, AT86RF233, AT86RFA1, AT86RFR2 */
#define AT86RF2XX_TRX_CTRL_1_MASK__IRQ_2_EXT_EN                 (0x40)  /* all */
#define AT86RF2XX_TRX_CTRL_1_MASK__TX_AUTO_CRC_ON               (0x20)  /* all */
#define AT86RF2XX_TRX_CTRL_1_MASK__RX_BL_CTRL                   (0x10)  /* AT86RF212B, AT86RF231, AT86RF232, AT86RF233 */
#define AT86RF2XX_TRX_CTRL_1_MASK__PLL_TX_FLT                   (0x10)  /* AT86RFR2 */
#define AT86RF2XX_TRX_CTRL_1_MASK__SPI_CMD_MODE                 (0x0C)  /* AT86RF212B, AT86RF231, AT86RF232, AT86RF233 */
#define AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE                (0x02)  /* AT86RF212B, AT86RF231, AT86RF232, AT86RF233 */
#define AT86RF2XX_TRX_CTRL_1_MASK__IRQ_POLARITY                 (0x01)  /* AT86RF212B, AT86RF231, AT86RF232, AT86RF233 */
/** @} */

/**
 * @name    Bitfield definitions for the PHY_TX_PWR register
 * @{
 */
#define AT86RF2XX_PHY_TX_PWR_MASK__PA_BOOST                     (0x80)  /* AT86RF212B */
#define AT86RF2XX_PHY_TX_PWR_MASK__GC_PA                        (0x60)  /* AT86RF212B */
#define AT86RF2XX_PHY_TX_PWR_MASK__PA_BUF_LT                    (0xC0)  /* AT86RF231, AT86RFA1 */
#define AT86RF2XX_PHY_TX_PWR_MASK__PA_LT                        (0x30)  /* AT86RF231, AT86RFA1 */
#if IS_USED(MODULE_AT86RF212B)
    #define AT86RF212B_PHY_TX_PWR_MASK__TX_PWR                  (0x1F)  /* AT86RF212B [4, 0] */
#endif
#if IS_USED(MODULE_AT86RF231)
    #define AT86RF231_PHY_TX_PWR_MASK__TX_PWR                   (0x0F) /* AT86RF231 [3, 0] */
#endif
#if IS_USED(MODULE_AT86RF232)
    #define AT86RF232_PHY_TX_PWR_MASK__TX_PWR                   (0x0F) /* AT86RF232 [3, 0] */
#endif
#if IS_USED(MODULE_AT86RF233)
    #define AT86RF233_PHY_TX_PWR_MASK__TX_PWR                   (0x0F) /* AT86RF233 [3, 0] */
#endif
#if IS_USED(MODULE_AT86RFA1)
    #define AT86RFA1_PHY_TX_PWR_MASK__TX_PWR                    (0x0F) /* AT86RFA1 [3, 0] */
#endif
#if IS_USED(MODULE_AT86RFR2)
    #define AT86RFR2_PHY_TX_PWR_MASK__TX_PWR                    (0x0F) /* AT86RFR2 [3, 0] */
#endif

#define AT86RF2XX_PHY_TX_PWR_DEFAULT__PA_BUF_LT                 (0xC0)
#define AT86RF2XX_PHY_TX_PWR_DEFAULT__PA_LT                     (0x00)
#define AT86RF2XX_PHY_TX_PWR_DEFAULT__TX_PWR                    (0x00)
/** @} */

/**
 * @name    Bitfield definitions for the PHY_RSSI register
 * @{
 */
#define AT86RF2XX_PHY_RSSI_MASK__RX_CRC_VALID                   (0x80)  /* all */
#define AT86RF2XX_PHY_RSSI_MASK__RND_VALUE                      (0x60)  /* all */
#define AT86RF2XX_PHY_RSSI_MASK__RSSI                           (0x1F)  /* all */
/** @} */

/**
 * @name    Bitfield definitions for the PHY_ED_LEVEL register
 * @{
 */
#define AT86RF2XX_PHY_ED_LEVEL_MASK__PHY_ED_LEVEL               (0xFF) /* all */
/** @} */

/**
 * @name    Bitfield definitions for the PHY_CC_CCA register
 * @{
 */
#define AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST                  (0x80)  /* all */
#define AT86RF2XX_PHY_CC_CCA_MASK__CCA_MODE                     (0x60)  /* all */
#define AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL                      (0x1F)  /* all */

#define AT86RF2XX_PHY_CC_CCA_DEFAULT__CCA_MODE                  (0x20)
/** @} */

/**
 * @name    Bitfield definitions for the CCA_THRES register
 * @{
 */
#define AT86RF2XX_CCA_THRES_MASK__CCA_CS_THRES                  (0xF0)  /* AT86RF212B, AT86RF231, AT86RFA1, AT86RFR2 */
#define AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES                  (0x0F)  /* all */
#define AT86RF2XX_CCA_THRES_MASK__RSVD_HI_NIBBLE                (0xC0)  /* Where does this come from? */
/** @} */

/**
 * @name    Bitfield definitions for the RX_CTRL register
 * @{
 */
#define AT86RF2XX_RX_CTRL_MASK__PEL_SHIFT_VALUE                 (0xC0)  /* AT86RF233 */
#define AT86RF2XX_RX_CTRL_MASK__JCM_EN                          (0x20)  /* AT86RF212B */
#define AT86RF2XX_RX_CTRL_MASK__PDT_THRES                       (0x0F)  /* AT86RF231, AT86RF232, AT86RF233, AT86RFA1, AT86RFR2 */
/** @} */

/**
 * @name    Bitfield definitions for the SFD_VALUE register
 * @{
 */
#define AT86RF2XX_SFD_VALUE_MASK__SFD_VALUE                     (0xFF) /* AT86RF212B, AT86RF231, AT86RF233, AT86RFA1, AT86RFR2 */
/** @} */

/**
 * @name    Bitfield definitions for the TRX_CTRL_2 register
 * @{
 */
#define AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE                 (0x80)  /* all */
#define AT86RF2XX_TRX_CTRL_2_MASK__FREQ_MODE                    (0x3F)  /* all */
#define AT86RF2XX_TRX_CTRL_2_MASK__TRX_OFF_AVDD_EN              (0x40)  /* AT86RF212B */
#define AT86RF2XX_TRX_CTRL_2_MASK__OQPSK_SCRAM_EN               (0x20)  /* AT86RF212B, AT86RF233 */
#define AT86RF2XX_TRX_CTRL_2_MASK__ALT_SPECTRUM                 (0x10)  /* AT86RF212B */
#define AT86RF2XX_TRX_CTRL_2_MASK__BPSK_OQPSK                   (0x08)  /* AT86RF212B */
#define AT86RF2XX_TRX_CTRL_2_MASK__SUB_MODE                     (0x04)  /* AT86RF212B */
#if IS_USED(MODULE_AT86RF212B)
#define AT86RF212B_TRX_CTRL_2_MASK__OQPSK_DATA_RATE             (0x03)  /* AT86RF212B [1, 0] */
#endif
#if IS_USED(MODULE_AT86RF231)
#define AT86RF231_TRX_CTRL_2_MASK__OQPSK_DATA_RATE              (0x03) /* AT86RF231 [1, 0] */
#endif
#if IS_USED(MODULE_AT86RF233)
#define AT86RF233_TRX_CTRL_2_MASK__OQPSK_DATA_RATE              (0x07) /* AT86RF233 [2, 0] */
#endif
#if IS_USED(MODULE_AT86RFA1)
#define AT86RFA1_TRX_CTRL_2_MASK__OQPSK_DATA_RATE               (0x03) /* AT86RF2A1 [1, 0] */
#endif
#if IS_USED(MODULE_AT86RFR2)
#define AT86RFR2_TRX_CTRL_2_MASK__OQPSK_DATA_RATE               (0x03) /* AT86RFR2 [1, 0] */
#endif
/** @} */

/**
 * @name    Bitfield definitions for the ANT_DIV register
 * @{
 */
#define AT86RF2XX_ANT_DIV_MASK__ANT_SEL                         (0x80)  /* all */
#define AT86RF2XX_ANT_DIV_MASK__ANT_DIV_EN                      (0x08)  /* all */
#define AT86RF2XX_ANT_DIV_MASK__ANT_EXT_SW_EN                   (0x04)  /* all */
#define AT86RF2XX_ANT_DIV_MASK__ANT_CTRL                        (0x03)  /* all */
/** @} */

/**
 * @name    Bitfield definitions for the IRQ_MASK register
 * @{
 */
#define AT86RF2XX_IRQ_MASK_MASK__BAT_LOW                        (0x80)  /* AT86RF212B, AT86RF231, AT86RF232, AT86RF233 */
#define AT86RF2XX_IRQ_MASK_MASK__AWAKE                          (0x80)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_IRQ_MASK_MASK__TRX_UR                         (0x40)  /* AT86RF212B, AT86RF231, AT86RF232; AT86RF233 */
#define AT86RF2XX_IRQ_MASK_MASK__TX_END                         (0x40)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_IRQ_MASK_MASK__AMI                            (0x20)  /* all */
#define AT86RF2XX_IRQ_MASK_MASK__CCA_ED_DONE                    (0x10)  /* all */
#define AT86RF2XX_IRQ_MASK_MASK__TRX_END                        (0x08)  /* AT86RF212B, AT86RF231, AT86RF232, AT86RF231 */
#define AT86RF2XX_IRQ_MASK_MASK__RX_END                         (0x08)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_IRQ_MASK_MASK__RX_START                       (0x04)  /* all */
#define AT86RF2XX_IRQ_MASK_MASK__PLL_UNLOCK                     (0x02)  /* all */
#define AT86RF2XX_IRQ_MASK_MASK__PLL_LOCK                       (0x01)  /* all */
/** @} */

/**
 * @name    Bitfield definitions for the IRQ_STATUS register
 * @{
 */
#define AT86RF2XX_IRQ_STATUS_MASK__BAT_LOW                      (0x80)  /* AT86RF212B, AT86RF231, AT86RF232, AT86RF233 */
#define AT86RF2XX_IRQ_STATUS_MASK__AWAKE                        (0x80)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_IRQ_STATUS_MASK__TRX_UR                       (0x40)  /* AT86RF212B, AT86RF231, AT86RF232; AT86RF233 */
#define AT86RF2XX_IRQ_STATUS_MASK__TX_END                       (0x40)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_IRQ_STATUS_MASK__AMI                          (0x20)  /* all */
#define AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE                  (0x10)  /* all */
#define AT86RF2XX_IRQ_STATUS_MASK__TRX_END                      (0x08)  /* AT86RF212B, AT86RF231, AT86RF232, AT86RF231 */
#define AT86RF2XX_IRQ_STATUS_MASK__RX_END                       (0x08)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_IRQ_STATUS_MASK__RX_START                     (0x04)  /* all */
#define AT86RF2XX_IRQ_STATUS_MASK__PLL_UNLOCK                   (0x02)  /* all */
#define AT86RF2XX_IRQ_STATUS_MASK__PLL_LOCK                     (0x01)  /* all */
/** @} */

/**
 * @name    Bitfield definitions for the VREG_CTRL register
 * @{
 */
#define AT86RF2XX_VREG_CTRL_MASK__AVREG_EXT                     (0x80)  /* all */
#define AT86RF2XX_VREG_CTRL_MASK__AVDD_OK                       (0x40)  /* all */
#define AT86RF2XX_VREG_CTRL_MASK__DVREG_EXT                     (0x08)  /* AT86RF212B, AT86RF231, AT86RF232, AT86RF233 */
#define AT86RF2XX_VREG_CTRL_MASK__DVDD_OK                       (0x04)  /* all */
/** @} */

/**
 * @name    Bitfield definitions for the BATMON register
 * @{
 */
#define AT86RF2XX_BATMON_MASK__PLL_LOCK_CP                      (0x80)  /* AT86RF212B */
#define AT86RF2XX_BATMON_MASK__BAT_LOW                          (0x80)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_BATMON_MASK__BAT_LOW_EN                       (0x40)  /* AT86RFA1, AT86RFR2 */
#define AT86RF2XX_BATMON_MASK__BATMON_OK                        (0x20)  /* all */
#define AT86RF2XX_BATMON_MASK__BATMON_HR                        (0x10)  /* all */
#define AT86RF2XX_BATMON_MASK__BATMON_VTH                       (0x0F)  /* all */
/** @} */

/**
 * @name    Bitfield definitions for the XOSC_CTRL register
 * @{
 */
#define AT86RF2XX_XOSC_CTRL_MASK__XTAL_MODE                     (0xF0)  /* all */
#define AT86RF2XX_XOSC_CTRL_MASK__XTAL_TRIM                     (0x0F)  /* all */

#if IS_USED(MODULE_AT86RF212B)
#define AT86RF212B_XOSC_CTRL__XTAL_MODE_EXTERNAL                ((0x04) << 4)
#endif
#if IS_USED(MODULE_AT86RF231)
#define AT86RF231_XOSC_CTRL__XTAL_MODE_EXTERNAL                 ((0x04) << 4)
#endif
#if IS_USED(MODULE_AT86RF232)
#define AT86RF232_XOSC_CTAL__XTAL_MODE_EXTERNAL                 ((0x05) << 4)
#endif
#if IS_USED(MODULE_AT86RF233)
#define AT86RF233_XOSC_CTAL__XTAL_MODE_EXTERNAL                 ((0x04) << 4)
#endif
#if IS_USED(MODULE_AT86RFA1)
#define AT86RFA1_XOSC_CTAL__XTAL_MODE_EXTERNAL                  ((0x05) << 4)
#endif
#if IS_USED(MODULE_AT86RFR2)
#define AT86RFR2_XOSC_CTAL__XTAL_MODE_EXTERNAL                  ((0x05) << 4)
#endif

#define AT86RF2XX_XOSC_CTAL__XTAL_MODE_INTERNAL                 ((0x0F) << 4)
/** @} */

/**
 * @name    Bitfield definitions for the CC_CTRL_0 register
 * @{
 */
#define AT86RF2XX_CC_CTRL_0_MASK__CC_NUMBER                     (0xFF) /* AT86RF212B, AT86RF233, AT86RFR2 */
/** @} */

/**
 * @name    Bitfield definitions for the CC_CTRL_1 register
 * @{
 */
#if IS_USED(MODULE_AT86RF212B)
#define AT86RF212B_CC_CTRL_1_MASK__CC_BAND                      (0x07) /* AT86RF212B [2, 0] */
#endif
#if IS_USED(MODULE_AT86RF233)
#define AT86RF233_CC_CTRL_1_MASK__CC_BAND                       (0x0F) /* AT86RF233 [3, 0] */
#endif
#if IS_USED(MODULE_AT86RFR2)
#define AT86RFR2_CC_CTRL_1_MASK__CC_BAND                        (0x0F) /* AT86RFR2 [3, 0] */
#endif
/** @} */

/**
 * @name    Bitfield definitions for the RX_SYN register
 * @{
 */
#define AT86RF2XX_RX_SYN_MASK__RX_PDT_DIS                       (0x80)  /* all */
#define AT86RF2XX_RX_SYN_MASK__RX_OVERRIDE                      (0x70)  /* AT86RF212B */
#define AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL                     (0x0F)  /* all */
/** @} */

/**
 * @name    Bitfield definitions for the RF_CTRL_0 register
 * @{
 */
#define AT86RF2XX_RF_CTRL_0_MASK__PA_LT                         (0xC0)  /* AT86RF212B */
#define AT86RF2XX_RF_CTRL_0_MASK__IF_SHIFT_MODE                 (0x0C)  /* AT86RF212B */
#define AT86RF2XX_RF_CTRL_0_MASK__GC_TX_OFFS                    (0x03)  /* AT86RF212B */

#define AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__0DB                     (0x01)
#define AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__1DB                     (0x02)
#define AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__2DB                     (0x03)
/** @} */

/**
 * @name    Bitfield definitions for the TRX_RPC register
 * @{
 */
#define AT86RF2XX_TRX_RPC_MASK__RX_RPC_CTRL                     (0xC0)  /* AT86RF233, AT86RFR2 */
#define AT86RF2XX_TRX_RPC_MASK__RX_RPC_EN                       (0x20)  /* AT86RF233, AT86RFR2 */
#define AT86RF2XX_TRX_RPC_MASK__PDT_RPC_EN                      (0x10)  /* AT86RF233, AT86RFR2 */
#define AT86RF2XX_TRX_RPC_MASK__PLL_RPC_EN                      (0x08)  /* AT86RF233, AT86RFR2 */
#define AT86RF2XX_TRX_RPC_MASK__XAH_TX_RPC_EN                   (0x04)  /* AT86RF233 */
#define AT86RF2XX_TRX_RPC_MASK__IPAN_RPC_EN                     (0x02)  /* AT86RF233, AT86RFR2 */
#define AT86RF2XX_TRX_RPC_MASK__XAH_RPC_EN                      (0x01)  /* AT86RFR2 */
/** @} */

/**
 * @name    Bitfield definitions for the XAH_CTRL_1 register
 * @{
 */
#define AT86RF2XX_XAH_CTRL_1_MASK__ARET_TX_TS_EN                (0x80)  /* AT86RF232, AT86RF233 */
#define AT86RF2XX_XAH_CTRL_1_MASK__CSMA_LBT_MODE                (0x40)  /* AT86RF212B */
#define AT86RF2XX_XAH_CTRL_1_MASK__AACK_FLTR_RES_FT             (0x20)  /* all */
#define AT86RF2XX_XAH_CTRL_1_MASK__AACK_UPLD_RES_FT             (0x10)  /* all */
#define AT86RF2XX_XAH_CTRL_1_MASK__AACK_ACK_TIME                (0x04)  /* all */
#define AT86RF2XX_XAH_CTRL_1_MASK__AACK_PROM_MODE               (0x02)  /* all */
#define AT86RF2XX_XAH_CTRL_1_MASK__AACK_SPC_EN                  (0x01)  /* AT86RF233 */
/** @} */

/**
 * @name    Bitfield definitions for the FTN_CTRL register
 * @{
 */
#define AT86RF2XX_FTN_CTRL_MASK__FTN_START                      (0x80)  /* all */
#define AT86RF2XX_FTN_CTRL_MASK__FTN_FTNV                       (0x3F)  /* AT86RF233 */
/** @} */

/**
 * @name    Bitfield definitions for the XAH_CTRL_2 register
 *
 * This register contains both the CSMA-CA retry counter and the frame retry
 * counter. At this moment only the at86rf232 and the at86rf233 support this
 * register.
 * @{
 */
#define AT86RF2XX_XAH_CTRL_2_MASK__ARET_FRAME_RETRIES           (0xF0)  /* AT86RF232, AT86RF233 */
#define AT86RF2XX_XAH_CTRL_2_MASK__ARET_CSMA_RETRIES            (0x0E)  /* AT86RF232, AT86RF233 */
/** @} */

/**
 * @name    Bitfield definitions for the PLL_CF register
 * @{
 */
#define AT86RF2XX_PLL_CF_MASK__PLL_CF_START                     (0x80)  /* all */
#define AT86RF2XX_PLL_CF_MASK__PLL_CF                           (0x0F)  /* AT86RF212B, AT86RF232 */
/** @} */

/**
 * @name    Bitfield definitions for the PLL_DCU register
 * @{
 */
#define AT86RF2XX_PLL_DCU_MASK__PLL_DCU_START                   (0x80) /* all */
/** @} */

/**
 * @name    Bitfield definitions for the XAH_CTRL_0 register
 * @{
 */
#define AT86RF2XX_XAH_CTRL_0_MASK__MAX_FRAME_RETRIES            (0xF0)  /* all */
#define AT86RF2XX_XAH_CTRL_0_MASK__MAX_CSMA_RETRIES             (0x0E)  /* all */
#define AT86RF2XX_XAH_CTRL_0_MASK__SLOTTED_OPERATION            (0x01)  /* all */

#define AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__0               (0x00)
#define AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__1               (0x01)
#define AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__2               (0x02)
#define AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__3               (0x03)
#define AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__4               (0x04)
#define AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__5               (0x05)
#define AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__6               (0x06)
#define AT86RF2XX_XAH_CTRL_0_MAX_FRAME_RETRIES__7               (0x07)

#define AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__0                (0x00)
#define AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__1                (0x01)
#define AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__2                (0x02)
#define AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__3                (0x03)
#define AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__4                (0x04)
#define AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__5                (0x05)
#define AT86RF2XX_XAH_CTRL_0_MAX_CSMA_RETRIES__NO_CSMA          (0x07)
/** @} */

/**
 * @name    Bitfield definitions for the CSMA_SEED_0 register
 * @{
 */
#define AT86RF2XX_CSMA_SEED_0_MASK__CSMA_SEED_0                 (0xFF) /* all */
/** @} */

/**
 * @name    Bitfield definitions for the CSMA_SEED_1 register
 * @{
 */
#define AT86RF2XX_CSMA_SEED_1_MASK__AACK_FVN_MODE               (0xC0)  /* all */
#define AT86RF2XX_CSMA_SEED_1_MASK__AACK_SET_PD                 (0x20)  /* all */
#define AT86RF2XX_CSMA_SEED_1_MASK__AACK_DIS_ACK                (0x10)  /* all */
#define AT86RF2XX_CSMA_SEED_1_MASK__AACK_I_AM_COORD             (0x08)  /* all */
#define AT86RF2XX_CSMA_SEED_1_MASK__CSMA_SEED_1                 (0x07)  /* all */
/** @} */

/**
 * @name    Bitfield definitions for the CSMA_BE register
 * @{
 */
#define AT86RF2XX_CSMA_BE_MASK__MAX_BE                          (0xF0)  /* all */
#define AT86RF2XX_CSMA_BE_MASK__MIN_BE                          (0x0F)  /* all */

#define AT86RF2XX_CSMA_BE_MAX_BE__MAX                           (0x08)
/** @} */

/**
 * @name    Bitfield definitions for the TST_CTRL_DIGI register
 * @{
 */
#define AT86RF2XX_TST_CTRL_DIGI_MASK__TST_CTRL_DIG              (0x0F) /* AT86RF232, AT86RF233 */
/** @} */

/**
 * @name    Bitfield definitions for the TST_AGC register
 * @{
 */
#define AT86RF2XX_TST_AGC_MASK__AGC_HOLD_SEL                    (0x20)  /* AT86RF233 */
#define AT86RF2XX_TST_AGC_MASK__AGC_RST                         (0x10)  /* AT86RF233 */
#define AT86RF2XX_TST_AGC_MASK__AGC_OFF                         (0x08)  /* AT86RF233 */
#define AT86RF2XX_TST_AGC_MASK__AGC_HOLD                        (0x04)  /* AT86RF233 */
#define AT86RF2XX_TST_AGC_MASK__GC                              (0x03)  /* AT86RF233 */
/** @} */

/**
 * @name    Bitfield definitions for the TST_SDM register
 * @{
 */
#define AT86RF2XX_TST_SDM_MASK__MOD_SEL                         (0x80)  /* AT86RF233 */
#define AT86RF2XX_TST_SDM_MASK__MOD                             (0x40)  /* AT86RF233 */
#define AT86RF2XX_TST_SDM_MASK__TX_RX                           (0x20)  /* AT86RF233 */
#define AT86RF2XX_TST_SDM_MASK__TX_RX_SEL                       (0x10)  /* AT86RF233 */
/** @} */

/**
 * @name    Bitfield definitions for the PHY_TX_TIME register
 * @{
 */
#define AT86RF2XX_PHY_TX_TIME_MASK__IRC_TX_TIME                 (0x0F) /* AT86RF233 */
/** @} */

/**
 * @name    Bitfield definitions for the PHY_PMU_VALUE register
 * @{
 */
#define AT86RF2XX_PHY_PMU_VALUE_MASK__PHY_PMU_VALUE             (0xFF) /* AT86RF233 */
/** @} */

/**
 * @name    Timing values
 * @{
 */
#define AT86RF2XX_TIMING__VCC_TO_P_ON                           (330)
#define AT86RF2XX_TIMING__SLEEP_TO_TRX_OFF                      (380)
#define AT86RF2XX_TIMING__TRX_OFF_TO_PLL_ON                     (110)
#define AT86RF2XX_TIMING__TRX_OFF_TO_RX_ON                      (110)
#define AT86RF2XX_TIMING__PLL_ON_TO_BUSY_TX                     (16)
#define AT86RF2XX_TIMING__RESET                                 (100)
#define AT86RF2XX_TIMING__RESET_TO_TRX_OFF                      (37)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_REGISTERS_H */
/** @} */
