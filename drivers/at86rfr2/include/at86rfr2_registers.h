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
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef AT86RFR2_REGISTERS_H_
#define AT86RFR2_REGISTERS_H_

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Constant part numbers of the AT86RF2xx device family
 * @{
 */
#define AT86RF212B_PARTNUM       (0x07)
#define AT86RF231_PARTNUM        (0x03)
#define AT86RF232_PARTNUM        (0x0a)
#define AT86RF233_PARTNUM        (0x0b)
#define AT85RFR2_PARTNUM			 (0x94) // RFR2 family
/** @} */

/**
 * @brief   Assign the part number for the device we are building the driver for
 * @{
 */
#ifdef MODULE_AT86RF212B
#define AT86RF2XX_PARTNUM           AT86RF212B_PARTNUM
#elif MODULE_AT86RF232
#define AT86RF2XX_PARTNUM           AT86RF232_PARTNUM
#elif MODULE_AT86RF233
#define AT86RF2XX_PARTNUM           AT86RF233_PARTNUM
#elif MODULE_AT86RFR2
#define AT86RF2XX_PARTNUM           AT85RFR2_PARTNUM
#else /* MODULE_AT86RF231 as default device */
#define AT86RF2XX_PARTNUM           AT86RF231_PARTNUM
#endif
/** @} */

///**
// * @brief   SPI access specifiers
// * @{
// */
//#define AT86RF2XX_ACCESS_REG                                    (0x80)
//#define AT86RF2XX_ACCESS_FB                                     (0x20)
//#define AT86RF2XX_ACCESS_SRAM                                   (0x00)
//#define AT86RF2XX_ACCESS_READ                                   (0x00)
//#define AT86RF2XX_ACCESS_WRITE                                  (0x40)
///** @} */

/**
 * @brief Register addresses
 * @{
 */
#define AT86RF2XX_REG__TRX_STATUS                               (&TRX_STATUS)
#define AT86RF2XX_REG__TRX_STATE                                (&TRX_STATE)
#define AT86RF2XX_REG__TRX_CTRL_0                               (&TRX_CTRL_0)
#define AT86RF2XX_REG__TRX_CTRL_1                               (&TRX_CTRL_1)

#define AT86RF2XX_REG__PHY_TX_PWR                               (&PHY_TX_PWR)
#define AT86RF2XX_REG__PHY_RSSI                                 (&PHY_RSSI)
#define AT86RF2XX_REG__PHY_ED_LEVEL                             (&PHY_ED_LEVEL)
#define AT86RF2XX_REG__PHY_CC_CCA                               (&PHY_CC_CCA)

#define AT86RF2XX_REG__CCA_THRES                                (&CCA_THRES)
#define AT86RF2XX_REG__RX_CTRL                                  (&RX_CTRL)
#define AT86RF2XX_REG__SFD_VALUE                                (&SFD_VALUE)
#define AT86RF2XX_REG__TRX_CTRL_2                               (&TRX_CTRL_2)

#define AT86RF2XX_REG__ANT_DIV                                  (&ANT_DIV)
#define AT86RF2XX_REG__IRQ_MASK                                 (&IRQ_MASK)
#define AT86RF2XX_REG__IRQ_MASK1                                (&IRQ_MASK1)
#define AT86RF2XX_REG__IRQ_STATUS                               (&IRQ_STATUS)
#define AT86RF2XX_REG__IRQ_STATUS1                              (&IRQ_STATUS1)
#define AT86RF2XX_REG__VREG_CTRL                                (&VREG_CTRL)

#define AT86RF2XX_REG__BATMON                                   (&BATMON)
#define AT86RF2XX_REG__XOSC_CTRL                                (&XOSC_CTRL)
#define AT86RF2XX_REG__CC_CTRL_0                                (&CC_CTRL_0)
#define AT86RF2XX_REG__CC_CTRL_1                                (&CC_CTRL_1)

#define AT86RF2XX_REG__RX_SYN                                   (&RX_SYN)
// TODO not defined register  TRX_RPC _SFR_MEM8(&0x156)
#define AT86RF2XX_REG__XAH_CTRL_1                               (&XAH_CTRL_1)
#define AT86RF2XX_REG__FTN_CTRL                                 (&FTN_CTRL)

#define AT86RF2XX_REG__PLL_CF                                   (&PLL_CF)
#define AT86RF2XX_REG__PLL_DCU                                  (&PLL_DCU)
#define AT86RF2XX_REG__PART_NUM                                 (&PART_NUM)
#define AT86RF2XX_REG__VERSION_NUM                              (&VERSION_NUM)

#define AT86RF2XX_REG__MAN_ID_0                                 (&MAN_ID_0)
#define AT86RF2XX_REG__MAN_ID_1                                 (&MAN_ID_1)
#define AT86RF2XX_REG__SHORT_ADDR_0                             (&SHORT_ADDR_0)
#define AT86RF2XX_REG__SHORT_ADDR_1                             (&SHORT_ADDR_1)

#define AT86RF2XX_REG__PAN_ID_0                                 (&PAN_ID_0)
#define AT86RF2XX_REG__PAN_ID_1                                 (&PAN_ID_1)
#define AT86RF2XX_REG__IEEE_ADDR_0                              (&IEEE_ADDR_0)
#define AT86RF2XX_REG__IEEE_ADDR_1                              (&IEEE_ADDR_1)

#define AT86RF2XX_REG__IEEE_ADDR_2                              (&IEEE_ADDR_2)
#define AT86RF2XX_REG__IEEE_ADDR_3                              (&IEEE_ADDR_3)
#define AT86RF2XX_REG__IEEE_ADDR_4                              (&IEEE_ADDR_4)
#define AT86RF2XX_REG__IEEE_ADDR_5                              (&IEEE_ADDR_5)

#define AT86RF2XX_REG__IEEE_ADDR_6                              (&IEEE_ADDR_6)
#define AT86RF2XX_REG__IEEE_ADDR_7                              (&IEEE_ADDR_7)
#define AT86RF2XX_REG__XAH_CTRL_0                               (&XAH_CTRL_0)
#define AT86RF2XX_REG__CSMA_SEED_0                              (&CSMA_SEED_0)

#define AT86RF2XX_REG__CSMA_SEED_1                              (&CSMA_SEED_1)
#define AT86RF2XX_REG__CSMA_BE                                  (&CSMA_BE)
#define AT86RF2XX_REG__TST_CTRL_DIGI                            (&TST_CTRL_DIGI)
// TODO not defined register  TST_RX_LENGTH _SFR_MEM8(0x17B)

// TODO not defined register  TRXFBST _SFR_MEM8(0x180)
// TODO not defined register  TRXFBEND _SFR_MEM8(0x1FF)
#define AT86RF2XX_REG__TRXFBST                          		(&TRXFBST)
#define AT86RF2XX_REG__TRXFBEND                          		(&TRXFBEND)

#define AT86RF2XX_REG__TRXPR                          			(&TRXPR)
/** @} */

/**
 * @brief   Bitfield definitions for the TRX_CTRL_0 register
 * @{
 */
// TODO this changed
//#define AT86RF2XX_TRX_CTRL_0_MASK__PAD_IO                       (0xC0)
//#define AT86RF2XX_TRX_CTRL_0_MASK__PAD_IO_CLKM                  (0x30)
//#define AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL                 (0x08)
//#define AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL                    (0x07)

//#define AT86RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO                    (0x00)
//#define AT86RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO_CLKM               (0x10)
//#define AT86RF2XX_TRX_CTRL_0_DEFAULT__CLKM_SHA_SEL              (0x08)
//#define AT86RF2XX_TRX_CTRL_0_DEFAULT__CLKM_CTRL                 (0x01)

//#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF                     (0x00)
//#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__1MHz                    (0x01)
//#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__2MHz                    (0x02)
//#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__4MHz                    (0x03)
//#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__8MHz                    (0x04)
//#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__16MHz                   (0x05)
//#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__250kHz                  (0x06)
//#define AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__62_5kHz                 (0x07)

#define AT86RF2XX_TRX_CTRL_0_MASK__PMU_EN                       (0x40)
#define AT86RF2XX_TRX_CTRL_0_MASK__PMU_START                    (0x20)
#define AT86RF2XX_TRX_CTRL_0_MASK__PMU_IF_INV                   (0x10)
/** @} */

/**
 * @brief   Bitfield definitions for the TRX_CTRL_1 register
 * @{
 */
#define AT86RF2XX_TRX_CTRL_1_MASK__PA_EXT_EN                    (0x80)
#define AT86RF2XX_TRX_CTRL_1_MASK__IRQ_2_EXT_EN                 (0x40)
#define AT86RF2XX_TRX_CTRL_1_MASK__TX_AUTO_CRC_ON               (0x20)
#define AT86RF2XX_TRX_CTRL_1_MASK__PLL_TX_FLT                   (0x10)
// TODO this changed
//#define AT86RF2XX_TRX_CTRL_1_MASK__RX_BL_CTRL                   (0x10)
//#define AT86RF2XX_TRX_CTRL_1_MASK__SPI_CMD_MODE                 (0x0C)
//#define AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE                (0x02)
//#define AT86RF2XX_TRX_CTRL_1_MASK__IRQ_POLARITY                 (0x01)
/** @} */

/**
 * @brief   Bitfield definitions for the TRX_CTRL_2 register
 * @{
 */
// TODO this changed
#define AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE                 (0x80)
//#define AT86RF2XX_TRX_CTRL_2_MASK__FREQ_MODE                    (0x3F)
//#define AT86RF2XX_TRX_CTRL_2_MASK__TRX_OFF_AVDD_EN              (0x40)
//#define AT86RF2XX_TRX_CTRL_2_MASK__OQPSK_SCRAM_EN               (0x20)
//#define AT86RF2XX_TRX_CTRL_2_MASK__ALT_SPECTRUM                 (0x10)
//#define AT86RF2XX_TRX_CTRL_2_MASK__BPSK_OQPSK                   (0x08)
//#define AT86RF2XX_TRX_CTRL_2_MASK__SUB_MODE                     (0x04)
#define AT86RF2XX_TRX_CTRL_2_MASK__OQPSK_DATA_RATE              (0x03)
/** @} */

/**
 * @brief   Bitfield definitions for the IRQ_MASK register
 * @{
 */
#define AT86RF2XX_IRQ_STATUS_MASK__AWAKE	                (0x80)
#define AT86RF2XX_IRQ_STATUS_MASK__TX_END                   (0x40)
#define AT86RF2XX_IRQ_STATUS_MASK__AMI                      (0x20)
#define AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE              (0x10)
#define AT86RF2XX_IRQ_STATUS_MASK__RX_END                   (0x08)
#define AT86RF2XX_IRQ_STATUS_MASK__RX_START                 (0x04)
#define AT86RF2XX_IRQ_STATUS_MASK__PLL_UNLOCK               (0x02)
#define AT86RF2XX_IRQ_STATUS_MASK__PLL_LOCK                 (0x01)

/**
 * @brief   Bitfield definitions for the IRQ_MASK1 register
 * @{
 */
#define AT86RF2XX_IRQ_STATUS_MASK1__TX_START                	(0x01)
#define AT86RF2XX_IRQ_STATUS_MASK1__MAF_0_AMI_EN	                (0x02)
#define AT86RF2XX_IRQ_STATUS_MASK1__MAF_1_AMI_EN	                (0x04)
#define AT86RF2XX_IRQ_STATUS_MASK1__MAF_2_AMI_EN	                (0x08)
#define AT86RF2XX_IRQ_STATUS_MASK1__MAF_3_AMI_EN	                (0x10)
/** @} */

/**
 * @brief   Bitfield definitions for the TRX_STATUS register
 * @{
 */
#define AT86RF2XX_TRX_STATUS_MASK__CCA_DONE                     (0x80)
#define AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS                   (0x40)
#define AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS                   (0x1F)
// TODO not defined register  TST_STATUS - Test mode status
#define AT86RF2XX_TRX_STATUS_MASK__TST_STATUS                   (0x20)

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
// TODO this changed
// reserved values not listed in datasheet
//#define AT86RF2XX_TRX_STATUS__RX_ON_NOCLK                       (0x1C)
//#define AT86RF2XX_TRX_STATUS__RX_AACK_ON_NOCLK                  (0x1D)
//#define AT86RF2XX_TRX_STATUS__BUSY_RX_AACK_NOCLK                (0x1E)
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
 * @brief   Bitfield definitions for the PHY_CC_CCA register
 * @{
 */
#define AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST                  (0x80)
#define AT86RF2XX_PHY_CC_CCA_MASK__CCA_MODE                     (0x60)
#define AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL                      (0x1F)

#define AT86RF2XX_PHY_CC_CCA_DEFAULT__CCA_MODE                  (0x20) // Mode 2, Carrier sense only
/** @} */

/**
 * @brief   Bitfield definitions for the CCA_THRES register
 * @{
 */
#define AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES                  (0x0F)

#define AT86RF2XX_CCA_THRES_MASK__RSVD_HI_NIBBLE                (0xF0)
/** @} */

/**
 * @brief   Bitfield definitions for the PHY_TX_PWR register
 * @{
 */
#define AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR                       (0x0F)
#define AT86RF2XX_PHY_TX_PWR_DEFAULT__TX_PWR                    (0x00)// 3.5dBm
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
#define AT86RF2XX_XOSC_CTRL__XTAL_MODE_CRYSTAL                  (0xF0) // (0x40) // TODO check if this on purpose wrong
#define AT86RF2XX_XOSC_CTRL__XTAL_MODE_EXTERNAL                 (0xF0)
/** @} */

/**
 * @brief   Timing values
 * @{
 */
// TODO confirm and check timing
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
#define AT86RF2XX_CSMA_SEED_1__CSMA_SEED_1                      (0x07)
/** @} */


/**
 * @brief   Bitfield definitions for the  TRXPR � Transceiver Pin Register
 * @{
 */
#define AT86RF2XX_TRXPR_ATBE                      				(0x08)
#define AT86RF2XX_TRXPR_TRXTST                     				(0x04)
#define AT86RF2XX_TRXPR_SLPTR                      				(0x02)
#define AT86RF2XX_TRXPR_TRXRST                     				(0x01)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AT86RFR2_REGISTERS_H_ */
/** @} */
