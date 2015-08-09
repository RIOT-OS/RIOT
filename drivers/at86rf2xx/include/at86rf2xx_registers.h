/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
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
 */

#ifndef AT86RF2XX_REGISTERS_H_
#define AT86RF2XX_REGISTERS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Constant part numbers of the AT86RF2xx device family
 * @{
 */
#define NG_AT86RF212B_PARTNUM       (0x07)
#define NG_AT86RF231_PARTNUM        (0x03)
#define NG_AT86RF232_PARTNUM        (0x0a)
#define NG_AT86RF233_PARTNUM        (0x0b)
/** @} */

/**
 * @brief   Assign the part number for the device we are building the driver for
 * @{
 */
#ifdef MODULE_NG_AT86RF212B
#define AT86RF2XX_PARTNUM           NG_AT86RF212B_PARTNUM
#elif MODULE_NG_AT86RF232
#define AT86RF2XX_PARTNUM           NG_AT86RF232_PARTNUM
#elif MODULE_NG_AT86RF233
#define AT86RF2XX_PARTNUM           NG_AT86RF233_PARTNUM
#else /* MODULE_NG_AT86RF231 as default device */
#define AT86RF2XX_PARTNUM           NG_AT86RF231_PARTNUM
#endif
/** @} */

/**
 * @brief   SPI access specifiers
 * @{
 */
#define AT86RF2XX_ACCESS_REG                                    (0x80)
#define AT86RF2XX_ACCESS_FB                                     (0x20)
#define AT86RF2XX_ACCESS_SRAM                                   (0x00)
#define AT86RF2XX_ACCESS_READ                                   (0x00)
#define AT86RF2XX_ACCESS_WRITE                                  (0x40)
/** @} */

/**
 * @brief   Register addresses
 * @{
 */
#define AT86RF2XX_REG__TRX_STATUS                               (0x01)
#define AT86RF2XX_REG__TRX_STATE                                (0x02)
#define AT86RF2XX_REG__TRX_CTRL_0                               (0x03)
#define AT86RF2XX_REG__TRX_CTRL_1                               (0x04)
#define AT86RF2XX_REG__PHY_TX_PWR                               (0x05)
#define AT86RF2XX_REG__PHY_RSSI                                 (0x06)
#define AT86RF2XX_REG__PHY_ED_LEVEL                             (0x07)
#define AT86RF2XX_REG__PHY_CC_CCA                               (0x08)
#define AT86RF2XX_REG__CCA_THRES                                (0x09)
#define AT86RF2XX_REG__RX_CTRL                                  (0x0A)
#define AT86RF2XX_REG__SFD_VALUE                                (0x0B)
#define AT86RF2XX_REG__TRX_CTRL_2                               (0x0C)
#define AT86RF2XX_REG__ANT_DIV                                  (0x0D)
#define AT86RF2XX_REG__IRQ_MASK                                 (0x0E)
#define AT86RF2XX_REG__IRQ_STATUS                               (0x0F)
#define AT86RF2XX_REG__VREG_CTRL                                (0x10)
#define AT86RF2XX_REG__BATMON                                   (0x11)
#define AT86RF2XX_REG__XOSC_CTRL                                (0x12)
#define AT86RF2XX_REG__CC_CTRL_1                                (0x14)
#define AT86RF2XX_REG__RX_SYN                                   (0x15)
#ifdef MODULE_NG_AT86RF212B
#define AT86RF2XX_REG__RF_CTRL_0                                (0x16)
#endif
#define AT86RF2XX_REG__XAH_CTRL_1                               (0x17)
#define AT86RF2XX_REG__FTN_CTRL                                 (0x18)
#define AT86RF2XX_REG__PLL_CF                                   (0x1A)
#define AT86RF2XX_REG__PLL_DCU                                  (0x1B)
#define AT86RF2XX_REG__PART_NUM                                 (0x1C)
#define AT86RF2XX_REG__VERSION_NUM                              (0x1D)
#define AT86RF2XX_REG__MAN_ID_0                                 (0x1E)
#define AT86RF2XX_REG__MAN_ID_1                                 (0x1F)
#define AT86RF2XX_REG__SHORT_ADDR_0                             (0x20)
#define AT86RF2XX_REG__SHORT_ADDR_1                             (0x21)
#define AT86RF2XX_REG__PAN_ID_0                                 (0x22)
#define AT86RF2XX_REG__PAN_ID_1                                 (0x23)
#define AT86RF2XX_REG__IEEE_ADDR_0                              (0x24)
#define AT86RF2XX_REG__IEEE_ADDR_1                              (0x25)
#define AT86RF2XX_REG__IEEE_ADDR_2                              (0x26)
#define AT86RF2XX_REG__IEEE_ADDR_3                              (0x27)
#define AT86RF2XX_REG__IEEE_ADDR_4                              (0x28)
#define AT86RF2XX_REG__IEEE_ADDR_5                              (0x29)
#define AT86RF2XX_REG__IEEE_ADDR_6                              (0x2A)
#define AT86RF2XX_REG__IEEE_ADDR_7                              (0x2B)
#define AT86RF2XX_REG__XAH_CTRL_0                               (0x2C)
#define AT86RF2XX_REG__CSMA_SEED_0                              (0x2D)
#define AT86RF2XX_REG__CSMA_SEED_1                              (0x2E)
#define AT86RF2XX_REG__CSMA_BE                                  (0x2F)
#define AT86RF2XX_REG__TST_CTRL_DIGI                            (0x36)
/** @} */

/**
 * @brief   Bitfield definitions for the TRX_CTRL_0 register
 * @{
 */
#define AT86RF2XX_TRX_CTRL_0_MASK__PAD_IO                       (0xC0)
#define AT86RF2XX_TRX_CTRL_0_MASK__PAD_IO_CLKM                  (0x30)
#define AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL                 (0x08)
#define AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL                    (0x07)

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
 * @brief   Bitfield definitions for the TRX_CTRL_1 register
 * @{
 */
#define AT86RF2XX_TRX_CTRL_1_MASK__PA_EXT_EN                    (0x80)
#define AT86RF2XX_TRX_CTRL_1_MASK__IRQ_2_EXT_EN                 (0x40)
#define AT86RF2XX_TRX_CTRL_1_MASK__TX_AUTO_CRC_ON               (0x20)
#define AT86RF2XX_TRX_CTRL_1_MASK__RX_BL_CTRL                   (0x10)
#define AT86RF2XX_TRX_CTRL_1_MASK__SPI_CMD_MODE                 (0x0C)
#define AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE                (0x02)
#define AT86RF2XX_TRX_CTRL_1_MASK__IRQ_POLARITY                 (0x01)
/** @} */

/**
 * @brief   Bitfield definitions for the TRX_CTRL_2 register
 * @{
 */
#define AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE                 (0x80)
#define AT86RF2XX_TRX_CTRL_2_MASK__FREQ_MODE                    (0x3F)
#define AT86RF2XX_TRX_CTRL_2_MASK__TRX_OFF_AVDD_EN              (0x40)
#define AT86RF2XX_TRX_CTRL_2_MASK__OQPSK_SCRAM_EN               (0x20)
#define AT86RF2XX_TRX_CTRL_2_MASK__ALT_SPECTRUM                 (0x10)
#define AT86RF2XX_TRX_CTRL_2_MASK__BPSK_OQPSK                   (0x08)
#define AT86RF2XX_TRX_CTRL_2_MASK__SUB_MODE                     (0x04)
#define AT86RF2XX_TRX_CTRL_2_MASK__OQPSK_DATA_RATE              (0x03)
/** @} */

/**
 * @brief   Bitfield definitions for the IRQ_STATUS register
 * @{
 */
#define AT86RF2XX_IRQ_STATUS_MASK__BAT_LOW                      (0x80)
#define AT86RF2XX_IRQ_STATUS_MASK__TRX_UR                       (0x40)
#define AT86RF2XX_IRQ_STATUS_MASK__AMI                          (0x20)
#define AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE                  (0x10)
#define AT86RF2XX_IRQ_STATUS_MASK__TRX_END                      (0x08)
#define AT86RF2XX_IRQ_STATUS_MASK__RX_START                     (0x04)
#define AT86RF2XX_IRQ_STATUS_MASK__PLL_UNLOCK                   (0x02)
#define AT86RF2XX_IRQ_STATUS_MASK__PLL_LOCK                     (0x01)
/** @} */

/**
 * @brief   Bitfield definitions for the TRX_STATUS register
 * @{
 */
#define AT86RF2XX_TRX_STATUS_MASK__CCA_DONE                     (0x80)
#define AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS                   (0x40)
#define AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS                   (0x1F)

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
 * @brief   Bitfield definitions for the TRX_STATE register
 * @{
 */
#define AT86RF2XX_TRX_STATE_MASK__TRAC                          (0xe0)

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
#define AT86RF2XX_TRX_STATE__TRAC_NO_ACK                        (0xa0)
#define AT86RF2XX_TRX_STATE__TRAC_INVALID                       (0xe0)
/** @} */

/**
 * @brief   Bitfield definitions for the PHY_CCA register
 * @{
 */
#define AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST                  (0x80)
#define AT86RF2XX_PHY_CC_CCA_MASK__CCA_MODE                     (0x60)
#define AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL                      (0x1F)

#define AT86RF2XX_PHY_CC_CCA_DEFAULT__CCA_MODE                  (0x20)
/** @} */

/**
 * @brief   Bitfield definitions for the PHY_TX_PWR register
 * @{
 */
#ifdef MODULE_NG_AT86RF212B
#define AT86RF2XX_PHY_TX_PWR_MASK__PA_BOOST                     (0x80)
#define AT86RF2XX_PHY_TX_PWR_MASK__GC_PA                        (0x60)
#define AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR                       (0x1F)
#elif  MODULE_NG_AT86RF231
#define AT86RF2XX_PHY_TX_PWR_MASK__PA_BUF_LT                    (0xC0)
#define AT86RF2XX_PHY_TX_PWR_MASK__PA_LT                        (0x30)
#define AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR                       (0x0F)
#else
#define AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR                       (0x0F)
#endif
#define AT86RF2XX_PHY_TX_PWR_DEFAULT__PA_BUF_LT                 (0xC0)
#define AT86RF2XX_PHY_TX_PWR_DEFAULT__PA_LT                     (0x00)
#define AT86RF2XX_PHY_TX_PWR_DEFAULT__TX_PWR                    (0x00)
/** @} */

/**
 * @brief   Bitfield definitions for the PHY_RSSI register
 * @{
 */
#define AT86RF2XX_PHY_RSSI_MASK__RX_CRC_VALID                   (0x80)
#define AT86RF2XX_PHY_RSSI_MASK__RND_VALUE                      (0x60)
#define AT86RF2XX_PHY_RSSI_MASK__RSSI                           (0x1F)
/** @} */

/**
 * @brief   Bitfield definitions for the XOSC_CTRL register
 * @{
 */
#define AT86RF2XX_XOSC_CTRL__XTAL_MODE_CRYSTAL                  (0xF0)
#define AT86RF2XX_XOSC_CTRL__XTAL_MODE_EXTERNAL                 (0xF0)
/** @} */

/**
 * @brief   Timing values
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

/**
 * @brief   Bitfield definitions for the XAH_CTRL_0 register
 * @{
 */
#define AT86RF2XX_XAH_CTRL_0__MAX_FRAME_RETRIES                 (0xF0)
#define AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES                  (0x0E)
#define AT86RF2XX_XAH_CTRL_0__SLOTTED_OPERATION                 (0x01)
/** @} */

/**
 * @brief   Bitfield definitions for the XAH_CTRL_1 register
 * @{
 */
#define AT86RF2XX_XAH_CTRL_1__AACK_FLTR_RES_FT                  (0x20)
#define AT86RF2XX_XAH_CTRL_1__AACK_UPLD_RES_FT                  (0x10)
#define AT86RF2XX_XAH_CTRL_1__AACK_ACK_TIME                     (0x04)
#define AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE                    (0x02)
/** @} */

/**
 * @brief   Bitfield definitions for the CSMA_SEED_1 register
 * @{
 */
#define AT86RF2XX_CSMA_SEED_1__AACK_SET_PD                      (0x20)
#define AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK                     (0x10)
#define AT86RF2XX_CSMA_SEED_1__AACK_I_AM_COORD                  (0x08)
/** @} */

/**
 * @brief   Bitfield definitions for the RF_CTRL_0 register
 * @{
 */
#ifdef MODULE_NG_AT86RF212B
#define AT86RF2XX_RF_CTRL_0_MASK__PA_LT                         (0xC0)
#define AT86RF2XX_RF_CTRL_0_MASK__GC_TX_OFFS                    (0x03)

#define AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__0DB                     (0x01)
#define AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__1DB                     (0x02)
#define AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__2DB                     (0x03)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_REGISTERS_H_ */
/** @} */
