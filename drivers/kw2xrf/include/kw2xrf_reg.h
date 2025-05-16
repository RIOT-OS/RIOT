/*
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * The description of the registers was extracted from the
 * Reference Manual MKW2xDxxxRM.pdf. After the release of MCR20A Device,
 * it was extended by the undocumented registers from the file MCR20reg.h.
 *
 * Portions of this file are derived from material that is
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

/**
 * @ingroup     drivers_kw2xrf
 * @{
 *
 * @file
 * @brief       Register and command definitions for the MKW2XD Modem
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

/** Option to for SPI-IF to read a register */
#define MKW2XDRF_REG_READ               (uint8_t)(1 << 7)

/** Option to for SPI-IF to write a register */
#define MKW2XDRF_REG_WRITE              (uint8_t)(0)

/** Option to for SPI-IF to read data from the RX/TX-Buffer */
#define MKW2XDRF_BUF_READ               (uint8_t)(1 << 7 | 1 << 6)

/** Option to for SPI-IF to write data to RX/TX-Buffer */
#define MKW2XDRF_BUF_WRITE              (uint8_t)(1 << 6)

/** Option for SPI-IF */
#define MKW2XDRF_BUF_BYTE_READ          (uint8_t)(1 << 7 | 1 << 6 | 1 << 5)

/** Option for SPI-IF */
#define MKW2XDRF_BUF_BYTE_WRITE         (uint8_t)(1 << 6 | 1 << 5)

/** Transceiver Sequence Selector, define Values for XCVSEQ */
typedef enum {
    XCVSEQ_IDLE = 0,
    XCVSEQ_RECEIVE,
    XCVSEQ_TRANSMIT,
    XCVSEQ_CCA,
    XCVSEQ_TX_RX,
    XCVSEQ_CONTINUOUS_CCA,
}
kw2xrf_physeq_t;

enum mkw2xdrf_dregister {
    MKW2XDM_IRQSTS1 = 0x0,              /**< Interrupt Request Status 1 */
    MKW2XDM_IRQSTS2 = 0x1,              /**< Interrupt Request Status 2 */
    MKW2XDM_IRQSTS3 = 0x2,              /**< Interrupt Request Status 3 */
    MKW2XDM_PHY_CTRL1 = 0x3,            /**< PHY Control 1 */
    MKW2XDM_PHY_CTRL2 = 0x4,            /**< PHY Control 2 */
    MKW2XDM_PHY_CTRL3 = 0x5,            /**< PHY Control 3 */
    MKW2XDM_RX_FRM_LEN = 0x6,           /**< Receive Frame Length */
    MKW2XDM_PHY_CTRL4 = 0x7,            /**< PHY Control 4 */
    MKW2XDM_SRC_CTRL = 0x8,             /**< SRC Control */
    MKW2XDM_SRC_ADDRS_SUM_LSB = 0x9,    /**< SRC Address SUM LSB */
    MKW2XDM_SRC_ADDRS_SUM_MSB = 0xa,    /**< SRC Address SUM MSB */
    MKW2XDM_CCA1_ED_FNL = 0xb,          /**< CCA1 ED FNL */
    MKW2XDM_EVENT_TIMER_LSB = 0xc,      /**< Event Timer LSB */
    MKW2XDM_EVENT_TIMER_MSB = 0xd,      /**< Event Timer MSB */
    MKW2XDM_EVENT_TIMER_USB = 0xe,      /**< Event Timer USB */
    MKW2XDM_TIMESTAMP_LSB = 0xf,        /**< Timestamp LSB */
    MKW2XDM_TIMESTAMP_MSB = 0x10,       /**< Timestamp MSB */
    MKW2XDM_TIMESTAMP_USB = 0x11,       /**< Timestamp USB */
    MKW2XDM_T3CMP_LSB = 0x12,           /**< Timer 3 Compare Value LSB */
    MKW2XDM_T3CMP_MSB = 0x13,           /**< Timer 3 Compare Value MSB */
    MKW2XDM_T3CMP_USB = 0x14,           /**< Timer 3 Compare Value USB */
    MKW2XDM_T2PRIMECMP_LSB = 0x15,      /**< Timer 2-Prime Compare Value LSB */
    MKW2XDM_T2PRIMECMP_MSB = 0x16,      /**< Timer 2-Prime Compare Value MSB */
    MKW2XDM_T1CMP_LSB = 0x17,           /**< Timer 1 Compare Value LSB */
    MKW2XDM_T1CMP_MSB = 0x18,           /**< Timer 1 Compare Value MSB */
    MKW2XDM_T1CMP_USB = 0x19,           /**< Timer 1 Compare Value USB */
    MKW2XDM_T2CMP_LSB = 0x1a,           /**< Timer 2 Compare Value LSB */
    MKW2XDM_T2CMP_MSB = 0x1b,           /**< Timer 2 Compare Value MSB */
    MKW2XDM_T2CMP_USB = 0x1c,           /**< Timer 2 Compare Value USB */
    MKW2XDM_T4CMP_LSB = 0x1d,           /**< Timer 4 Compare Value LSB */
    MKW2XDM_T4CMP_MSB = 0x1e,           /**< Timer 4 Compare Value MSB */
    MKW2XDM_T4CMP_USB = 0x1f,           /**< Timer 4 Compare Value USB */
    MKW2XDM_PLL_INT0 = 0x20,            /**< PLL Integer Value for PAN0 */
    MKW2XDM_PLL_FRAC0_LSB = 0x21,       /**< PLL Frequency Fractional Value for PAN0 */
    MKW2XDM_PLL_FRAC0_MSB = 0x22,       /**< PLL Frequency Fractional Value for PAN0 */
    MKW2XDM_PA_PWR = 0x23,              /**< PA Power Control (PA_PWR) */
    MKW2XDM_SEQ_STATE = 0x24,           /**< Sequence Manager State */
    MKW2XDM_LQI_VALUE = 0x25,           /**< Link Quality Indicator */
    MKW2XDM_RSSI_CCA_CNT = 0x26,        /**< RSSI CCA CNT */
    MKW2XDM_ASM_CTRL1 = 0x28,
    MKW2XDM_ASM_CTRL2 = 0x29,
    MKW2XDM_ASM_DATA_0 = 0x2A,
    MKW2XDM_ASM_DATA_1 = 0x2B,
    MKW2XDM_ASM_DATA_2 = 0x2C,
    MKW2XDM_ASM_DATA_3 = 0x2D,
    MKW2XDM_ASM_DATA_4 = 0x2E,
    MKW2XDM_ASM_DATA_5 = 0x2F,
    MKW2XDM_ASM_DATA_6 = 0x30,
    MKW2XDM_ASM_DATA_7 = 0x31,
    MKW2XDM_ASM_DATA_8 = 0x32,
    MKW2XDM_ASM_DATA_9 = 0x33,
    MKW2XDM_ASM_DATA_A = 0x34,
    MKW2XDM_ASM_DATA_B = 0x35,
    MKW2XDM_ASM_DATA_C = 0x36,
    MKW2XDM_ASM_DATA_D = 0x37,
    MKW2XDM_ASM_DATA_E = 0x38,
    MKW2XDM_ASM_DATA_F = 0x39,
    MKW2XDM_OVERWRITE_VER = 0x3b,       /**< Overwrite Version Number */
    MKW2XDM_CLK_OUT_CTRL = 0x3c,        /**< CLK_OUT Control */
    MKW2XDM_PWR_MODES = 0x3d,           /**< Power Modes */
    MKW2XDM_IAR_INDEX = 0x3e,           /**< IAR Index */
    MKW2XDM_IAR_DATA = 0x3f,            /**< IAR Data */
};

#define MKW2XDM_IRQSTS1_RX_FRM_PEND                 (1 << 7)
#define MKW2XDM_IRQSTS1_PLL_UNLOCK_IRQ              (1 << 6)
#define MKW2XDM_IRQSTS1_FILTERFAIL_IRQ              (1 << 5)
#define MKW2XDM_IRQSTS1_RXWTRMRKIRQ                 (1 << 4)
#define MKW2XDM_IRQSTS1_CCAIRQ                      (1 << 3)
#define MKW2XDM_IRQSTS1_RXIRQ                       (1 << 2)
#define MKW2XDM_IRQSTS1_TXIRQ                       (1 << 1)
#define MKW2XDM_IRQSTS1_SEQIRQ                      (1 << 0)

#define MKW2XDM_IRQSTS2_CRCVALID                    (1 << 7)
#define MKW2XDM_IRQSTS2_CCA                         (1 << 6)
#define MKW2XDM_IRQSTS2_SRCADDR                     (1 << 5)
#define MKW2XDM_IRQSTS2_PI                          (1 << 4)
#define MKW2XDM_IRQSTS2_TMRSTATUS                   (1 << 3)
#define MKW2XDM_IRQSTS2_PB_ERR_IRQ                  (1 << 1)
#define MKW2XDM_IRQSTS2_WAKE_IRQ                    (1 << 0)

#define MKW2XDM_IRQSTS3_TMR4MSK                     (1 << 7)
#define MKW2XDM_IRQSTS3_TMR3MSK                     (1 << 6)
#define MKW2XDM_IRQSTS3_TMR2MSK                     (1 << 5)
#define MKW2XDM_IRQSTS3_TMR1MSK                     (1 << 4)
#define MKW2XDM_IRQSTS3_TMR4IRQ                     (1 << 3)
#define MKW2XDM_IRQSTS3_TMR3IRQ                     (1 << 2)
#define MKW2XDM_IRQSTS3_TMR2IRQ                     (1 << 1)
#define MKW2XDM_IRQSTS3_TMR1IRQ                     (1 << 0)
#define MKW2XDM_IRQSTS3_TMR_IRQ_MASK                0xfu
#define MKW2XDM_IRQSTS3_TMR_IRQ_SHIFT               0x0u
#define MKW2XDM_IRQSTS3_TMR_IRQ(x)                  (((uint8_t)(((uint8_t)(x))<<MKW2XDM_IRQSTS3_TMR_IRQ_SHIFT))&MKW2XDM_IRQSTS3_TMR_IRQ_MASK)

#define MKW2XDM_PHY_CTRL1_TMRTRIGEN                 (1 << 7)
#define MKW2XDM_PHY_CTRL1_SLOTTED                   (1 << 6)
#define MKW2XDM_PHY_CTRL1_CCABFRTX                  (1 << 5)
#define MKW2XDM_PHY_CTRL1_RXACKRQD                  (1 << 4)
#define MKW2XDM_PHY_CTRL1_AUTOACK                   (1 << 3)
#define MKW2XDM_PHY_CTRL1_XCVSEQ_MASK               0x07u
#define MKW2XDM_PHY_CTRL1_XCVSEQ(x)                 (((uint8_t)(((uint8_t)(x))<<0))&MKW2XDM_PHY_CTRL1_XCVSEQ_MASK)

#define MKW2XDM_PHY_CTRL2_CRC_MSK                   (1 << 7)
#define MKW2XDM_PHY_CTRL2_PLL_UNLOCK_MSK            (1 << 6)
#define MKW2XDM_PHY_CTRL2_FILTERFAIL_MSK            (1 << 5)
#define MKW2XDM_PHY_CTRL2_RX_WMRK_MSK               (1 << 4)
#define MKW2XDM_PHY_CTRL2_CCAMSK                    (1 << 3)
#define MKW2XDM_PHY_CTRL2_RXMSK                     (1 << 2)
#define MKW2XDM_PHY_CTRL2_TXMSK                     (1 << 1)
#define MKW2XDM_PHY_CTRL2_SEQMSK                    (1 << 0)

#define MKW2XDM_PHY_CTRL3_TMR4CMP_EN                (1 << 7)
#define MKW2XDM_PHY_CTRL3_TMR3CMP_EN                (1 << 6)
#define MKW2XDM_PHY_CTRL3_TMR2CMP_EN                (1 << 5)
#define MKW2XDM_PHY_CTRL3_TMR1CMP_EN                (1 << 4)
#define MKW2XDM_PHY_CTRL3_PB_ERR_MSK                (1 << 1)
#define MKW2XDM_PHY_CTRL3_WAKE_MSK                  (1 << 0)

#define MKW2XDM_RX_FRM_LENGTH_MASK                  0x7Fu

#define MKW2XDM_PHY_CTRL4_TRCV_MSK                  (1 << 7)
#define MKW2XDM_PHY_CTRL4_TC3TMOUT                  (1 << 6)
#define MKW2XDM_PHY_CTRL4_PANCORDNTR0               (1 << 5)
#define MKW2XDM_PHY_CTRL4_CCATYPE_MASK              0x18u
#define MKW2XDM_PHY_CTRL4_CCATYPE_SHIFT             3
#define MKW2XDM_PHY_CTRL4_CCATYPE(x)                (((uint8_t)(((uint8_t)(x))<<MKW2XDM_PHY_CTRL4_CCATYPE_SHIFT))&MKW2XDM_PHY_CTRL4_CCATYPE_MASK)
#define MKW2XDM_PHY_CTRL4_TMRLOAD                   (1 << 2)
#define MKW2XDM_PHY_CTRL4_PROMISCUOUS               (1 << 1)
#define MKW2XDM_PHY_CTRL4_TC2PRIME_EN               (1 << 0)

#define MKW2XDM_SRC_CTRL_INDEX_MASK                 0xF0u
#define MKW2XDM_SRC_CTRL_INDEX_SHIFT                4
#define MKW2XDM_SRC_CTRL_ACK_FRM_PND                (1 << 3)
#define MKW2XDM_SRC_CTRL_SRCADDR_EN                 (1 << 2)
#define MKW2XDM_SRC_CTRL_INDEX_EN                   (1 << 1)
#define MKW2XDM_SRC_CTRL_INDEX_DISABLE              (1 << 0)

#define MKW2XDM_PLL_INT0_MASK                       0x1Fu
#define MKW2XDM_PLL_INT0_VAL(x)                     (((uint8_t)(((uint8_t)(x))<<0))&MKW2XDM_PLL_INT0_MASK)

#define MKW2XDM_PA_PWR_MASK                         0x1Fu
#define MKW2XDM_PA_PWR(x)                           (((uint8_t)(((uint8_t)(x))<<0))&MKW2XDM_PA_PWR_MASK)

#define MKW2XDM_CLK_OUT_EXTEND                      (1 << 7)
#define MKW2XDM_CLK_OUT_HIZ                         (1 << 6)
#define MKW2XDM_CLK_OUT_SR                          (1 << 5)
#define MKW2XDM_CLK_OUT_DS                          (1 << 4)
#define MKW2XDM_CLK_OUT_EN                          (1 << 3)
#define MKW2XDM_CLK_OUT_DIV_MASK                    0x03u
#define MKW2XDM_CLK_OUT_DIV(x)                      (((uint8_t)(((uint8_t)(x))<<0))&MKW2XDM_CLK_OUT_DIV_MASK)

#define MKW2XDM_PWR_MODES_XTAL_READY                (1 << 5)
#define MKW2XDM_PWR_MODES_XTALEN                    (1 << 4)
#define MKW2XDM_PWR_MODES_AUTODOZE                  (1 << 1)
#define MKW2XDM_PWR_MODES_PMC_MODE                  (1 << 0)

enum mkw2xdrf_iregister {
    MKW2XDMI_PART_ID = 0x00,               /**< Part Identification */
    MKW2XDMI_XTAL_TRIM = 0x01,             /**< XTAL 32 MHz Trim */
    MKW2XDMI_MACPANID0_LSB = 0x03,         /**< MAC PAN ID for PAN0 */
    MKW2XDMI_MACPANID0_MSB = 0x04,         /**< MAC PAN ID for PAN0 */
    MKW2XDMI_MACSHORTADDRS0_LSB = 0x05,    /**< MAC Short Address for PAN0 */
    MKW2XDMI_MACSHORTADDRS0_MSB = 0x06,    /**< MAC Short Address for PAN0 */
    MKW2XDMI_MACLONGADDRS0_0 = 0x07,       /**< MAC Long Address for PAN0 */
    MKW2XDMI_MACLONGADDRS0_1 = 0x08,       /**< MAC Long Address for PAN0 */
    MKW2XDMI_MACLONGADDRS0_2 = 0x09,       /**< MAC Long Address for PAN0 */
    MKW2XDMI_MACLONGADDRS0_3 = 0x0a,       /**< MAC Long Address for PAN0 */
    MKW2XDMI_MACLONGADDRS0_4 = 0x0b,       /**< MAC Long Address for PAN0 */
    MKW2XDMI_MACLONGADDRS0_5 = 0x0c,       /**< MAC Long Address for PAN0 */
    MKW2XDMI_MACLONGADDRS0_6 = 0x0d,       /**< MAC Long Address for PAN0 */
    MKW2XDMI_MACLONGADDRS0_7 = 0x0e,       /**< MAC Long Address for PAN0 */
    MKW2XDMI_RX_FRAME_FILTER = 0x0f,       /**< Receive Frame Filter */
    MKW2XDMI_PLL_INT1 = 0x10,              /**< Frequency Integer for PAN1 */
    MKW2XDMI_PLL_FRAC1_LSB = 0x11,         /**< Frequency Fractional Value for PAN1 */
    MKW2XDMI_PLL_FRAC1_MSB = 0x12,         /**< Frequency Fractional Value for PAN1 */
    MKW2XDMI_MACPANID1_LSB = 0x13,         /**< Frequency Fractional Value for PAN1 */
    MKW2XDMI_MACPANID1_MSB = 0x14,         /**< Frequency Fractional Value for PAN1 */
    MKW2XDMI_MACSHORTADDRS1_LSB = 0x15,    /**< MAC Short Address for PAN1 */
    MKW2XDMI_MACSHORTADDRS1_MSB = 0x16,    /**< MAC Short Address for PAN1 */
    MKW2XDMI_MACLONGADDRS1_0 = 0x17,       /**< MAC Long Address for PAN1 */
    MKW2XDMI_MACLONGADDRS1_1 = 0x18,       /**< MAC Long Address for PAN1 */
    MKW2XDMI_MACLONGADDRS1_2 = 0x19,       /**< MAC Long Address for PAN1 */
    MKW2XDMI_MACLONGADDRS1_3 = 0x1a,       /**< MAC Long Address for PAN1 */
    MKW2XDMI_MACLONGADDRS1_4 = 0x1b,       /**< MAC Long Address for PAN1 */
    MKW2XDMI_MACLONGADDRS1_5 = 0x1c,       /**< MAC Long Address for PAN1 */
    MKW2XDMI_MACLONGADDRS1_6 = 0x1d,       /**< MAC Long Address for PAN1 */
    MKW2XDMI_MACLONGADDRS1_7 = 0x1e,       /**< MAC Long Address for PAN1 */
    MKW2XDMI_DUAL_PAN_CTRL = 0x1f,         /**< Dual PAN Control */
    MKW2XDMI_DUAL_PAN_DWELL = 0x20,        /**< Channel Frequency Dwell Time */
    MKW2XDMI_DUAL_PAN_STS = 0x21,          /**< Dual PAN Status */
    MKW2XDMI_CCA1_THRESH = 0x22,           /**< Clear Channel Assessment 1 Threshold */
    MKW2XDMI_CCA1_ED_OFFSET_COMP = 0x23,   /**< Clear Channel Assessment / ED Offset Computation */
    MKW2XDMI_LQI_OFFSET_COMP = 0x24,       /**< LQI Offset Computation */
    MKW2XDMI_CCA_CTRL = 0x25,              /**< CCA Control */
    MKW2XDMI_CCA2_CORR_PEAKS = 0x26,       /**< Clear Channel Assessment 2 Threshold Peak Compare */
    MKW2XDMI_CCA2_THRESH = 0x27,           /**< Clear Channel Assessment 2 Threshold */
    MKW2XDMI_TMR_PRESCALE = 0x28,          /**< Event Timer Time Base */
    MKW2XDMI_GPIO_DATA = 0x2a,             /**< GPIO Data */
    MKW2XDMI_GPIO_DIR = 0x2b,              /**< GPIO Direction Control */
    MKW2XDMI_GPIO_PUL_EN = 0x2c,           /**< GPIO Pullup Enable */
    MKW2XDMI_GPIO_SEL = 0x2d,              /**< GPIO Pullup Select */
    MKW2XDMI_GPIO_DS = 0x2e,               /**< GPIO Drive Strength */
    MKW2XDMI_ANT_PAD_CTRL = 0x30,          /**< Antenna Control */
    MKW2XDMI_MISC_PAD_CTRL = 0x31,         /**< Miscellaneous Pad Control */
    MKW2XDMI_BSM_CTRL = 0x32,
    MKW2XDMI__RNG = 0x34,
    MKW2XDMI_RX_BYTE_COUNT = 0x35,
    MKW2XDMI_RX_WTR_MARK = 0x36,
    MKW2XDMI_SOFT_RESET = 0x37,
    MKW2XDMI_TXDELAY = 0x38,
    MKW2XDMI_ACKDELAY = 0x39,
    MKW2XDMI_SEQ_MGR_CTRL = 0x3A,
    MKW2XDMI_SEQ_MGR_STS = 0x3B,
    MKW2XDMI_SEQ_T_STS = 0x3C,
    MKW2XDMI_ABORT_STS = 0x3D,
    MKW2XDMI_CCCA_BUSY_CNT = 0x3E,
    MKW2XDMI_SRC_ADDR_CHECKSUM1 = 0x3F,
    MKW2XDMI_SRC_ADDR_CHECKSUM2 = 0x40,
    MKW2XDMI_SRC_TBL_VALID1 = 0x41,
    MKW2XDMI_SRC_TBL_VALID2 = 0x42,
    MKW2XDMI_FILTERFAIL_CODE1 = 0x43,
    MKW2XDMI_FILTERFAIL_CODE2 = 0x44,
    MKW2XDMI_SLOT_PRELOAD = 0x45,
    MKW2XDMI_CORR_VT = 0x47,
    MKW2XDMI_SYNC_CTRL = 0x48,
    MKW2XDMI_PN_LSB_0 = 0x49,
    MKW2XDMI_PN_LSB_1 = 0x4A,
    MKW2XDMI_PN_MSB_0 = 0x4B,
    MKW2XDMI_PN_MSB_1 = 0x4C,
    MKW2XDMI_CORR_NVAL = 0x4D,
    MKW2XDMI_TX_MODE_CTRL = 0x4E,
    MKW2XDMI_SNF_THR = 0x4F,
    MKW2XDMI_FAD_THR = 0x50,
    MKW2XDMI_ANT_AGC_CTRL = 0x51,          /**< Antenna AGC and FAD Control */
    MKW2XDMI_AGC_THR1 = 0x52,
    MKW2XDMI_AGC_THR2 = 0x53,
    MKW2XDMI_AGC_HYS = 0x54,
    MKW2XDMI_AFC = 0x55,
    MKW2XDMI_LPPS_CTRL = 0x56,             /**< LPPS_CTRL */
    MKW2XDMI_PHY_STS = 0x58,
    MKW2XDMI_RX_MAX_CORR = 0x59,
    MKW2XDMI_RX_MAX_PREAMBLE = 0x5A,
    MKW2XDMI_RSSI = 0x5B,
    MKW2XDMI_PLL_DIG_CTRL = 0x5E,
    MKW2XDMI_VCO_CAL = 0x5F,
    MKW2XDMI_VCO_BEST_DIFF = 0x60,
    MKW2XDMI_VCO_BIAS = 0x61,
    MKW2XDMI_KMOD_CTRL = 0x62,
    MKW2XDMI_KMOD_CAL = 0x63,
    MKW2XDMI_PA_CAL = 0x64,
    MKW2XDMI_PA_PWRCAL = 0x65,
    MKW2XDMI_ATT_RSSI1 = 0x66,
    MKW2XDMI_ATT_RSSI2 = 0x67,
    MKW2XDMI_RSSI_OFFSET = 0x68,
    MKW2XDMI_RSSI_SLOPE = 0x69,
    MKW2XDMI_RSSI_CAL1 = 0x6A,
    MKW2XDMI_RSSI_CAL2 = 0x6B,
    MKW2XDMI_XTAL_CTRL = 0x6E,
    MKW2XDMI_XTAL_COMP_MIN = 0x6F,
    MKW2XDMI_XTAL_COMP_MAX = 0x70,
    MKW2XDMI_XTAL_GM = 0x71,
    MKW2XDMI_LNA_TUNE = 0x74,
    MKW2XDMI_LNA_AGCGAIN = 0x75,
    MKW2XDMI_CHF_PMA_GAIN = 0x78,
    MKW2XDMI_CHF_IBUF = 0x79,
    MKW2XDMI_CHF_QBUF = 0x7A,
    MKW2XDMI_CHF_IRIN = 0x7B,
    MKW2XDMI_CHF_QRIN = 0x7C,
    MKW2XDMI_CHF_IL = 0x7D,
    MKW2XDMI_CHF_QL = 0x7E,
    MKW2XDMI_CHF_CC1 = 0x7F,
    MKW2XDMI_CHF_CCL = 0x80,
    MKW2XDMI_CHF_CC2 = 0x81,
    MKW2XDMI_CHF_IROUT = 0x82,
    MKW2XDMI_CHF_QROUT = 0x83,
    MKW2XDMI_RSSI_CTRL = 0x86,
    MKW2XDMI_PA_BIAS = 0x89,
    MKW2XDMI_PA_TUNING = 0x8A,
    MKW2XDMI_PMC_HP_TRIM = 0x8D,
    MKW2XDMI_VREGA_TRIM = 0x8E,
    MKW2XDMI_VCO_CTRL1 = 0x91,
    MKW2XDMI_VCO_CTRL2 = 0x92,
    MKW2XDMI_ANA_SPARE_OUT1 = 0x95,
    MKW2XDMI_ANA_SPARE_OUT2 = 0x96,
    MKW2XDMI_ANA_SPARE_IN = 0x97,
    MKW2XDMI_MISCELLANEOUS = 0x98,
    MKW2XDMI_SEQ_MGR_OVRD0 = 0x9A,
    MKW2XDMI_SEQ_MGR_OVRD1 = 0x9B,
    MKW2XDMI_SEQ_MGR_OVRD2 = 0x9C,
    MKW2XDMI_SEQ_MGR_OVRD3 = 0x9D,
    MKW2XDMI_SEQ_MGR_OVRD4 = 0x9E,
    MKW2XDMI_SEQ_MGR_OVRD5 = 0x9F,
    MKW2XDMI_SEQ_MGR_OVRD6 = 0xA0,
    MKW2XDMI_SEQ_MGR_OVRD7 = 0xA1,
    MKW2XDMI_TESTMODE_CTRL = 0xA3,
    MKW2XDMI_DTM_CTRL1= 0xA4,
    MKW2XDMI_DTM_CTRL2= 0xA5,
    MKW2XDMI_ATM_CTRL1= 0xA6,
    MKW2XDMI_ATM_CTRL2= 0xA7,
    MKW2XDMI_ATM_CTRL3= 0xA8,
    MKW2XDMI_LIM_FE_TEST_CTRL = 0xAA,
    MKW2XDMI_CHF_TEST_CTRL = 0xAB,
    MKW2XDMI_VCO_TEST_CTRL = 0xAC,
    MKW2XDMI_PLL_TEST_CTRL = 0xAD,
    MKW2XDMI_PA_TEST_CTRL = 0xAE,
    MKW2XDMI_PMC_TEST_CTRL = 0xAF,
    MKW2XDMI_SCAN_DTM_PROTECT_1 = 0xFE,
    MKW2XDMI_SCAN_DTM_PROTECT_0 = 0xFF,
};

#define MKW2XDMI_PART_ID_MANUF_ID_MASK                          0x60u
#define MKW2XDMI_PART_ID_VERSION_MASK                           0x1Cu
#define MKW2XDMI_PART_ID_MASK_SET_MASK                          0x07u

#define MKW2XDMI_RX_FRAME_FILTER_FRM_VER_MASK                   0xC0u
#define MKW2XDMI_RX_FRAME_FILTER_FRM_VER_SHIFT                  6
#define MKW2XDMI_RX_FRAME_FILTER_FRM_VER(x)                     (((uint8_t)(((uint8_t)(x))<<MKW2XDMI_RX_FRAME_FILTER_FRM_VER_SHIFT))&MKW2XDMI_RX_FRAME_FILTER_FRM_VER_MASK)
#define MKW2XDMI_RX_FRAME_FILTER_ACTIVE_PROMISCUOUS             (1 << 5)
#define MKW2XDMI_RX_FRAME_FILTER_NS_FT                          (1 << 4)
#define MKW2XDMI_RX_FRAME_FILTER_CMD_FT                         (1 << 3)
#define MKW2XDMI_RX_FRAME_FILTER_ACK_FT                         (1 << 2)
#define MKW2XDMI_RX_FRAME_FILTER_DATA_FT                        (1 << 1)
#define MKW2XDMI_RX_FRAME_FILTER_BEACON_FT                      (1 << 0)

#define MKW2XDMI_PLL_INT1_MASK                                  0x1Fu

#define MKW2XDMI_DUAL_PAN_CTRL_DUAL_PAN_SAM_LVL_MASK            0xF0
#define MKW2XDMI_DUAL_PAN_CTRL_DUAL_PAN_SAM_LVL_SHIFT           4
#define MKW2XDMI_DUAL_PAN_CTRL_DUAL_PAN_SAM_LVL(x)              (((uint8_t)(((uint8_t)(x))<<MKW2XDMI_DUAL_PAN_CTRL_DUAL_PAN_SAM_LVL_SHIFT))&MKW2XDMI_DUAL_PAN_CTRL_DUAL_PAN_SAM_LVL_MASK)
#define MKW2XDMI_DUAL_PAN_CTRL_CURRENT_NETWORK                 (1 << 3)
#define MKW2XDMI_DUAL_PAN_CTRL_PANCORDNTR1                     (1 << 2)
#define MKW2XDMI_DUAL_PAN_CTRL_DUAL_PAN_AUTO                   (1 << 1)
#define MKW2XDMI_DUAL_PAN_CTRL_ACTIVE_NETWORK                  (1 << 0)

#define MKW2XDMI_DUAL_PAN_STS_RECD_ON_PAN1                     (1 << 7)
#define MKW2XDMI_DUAL_PAN_STS_RECD_ON_PAN0                     (1 << 6)
#define MKW2XDMI_DUAL_PAN_STS_DUAL_PAN_REMAIN_MASK             0x3Fu

#define MKW2XDMI_CCA_CTRL_AGC_FRZ_EN                           (1 << 6)
#define MKW2XDMI_CCA_CTRL_CONT_RSSI_EN                         (1 << 5)
#define MKW2XDMI_CCA_CTRL_QI_RSSI_NOT_CORR                     (1 << 4)
#define MKW2XDMI_CCA_CTRL_CCA3_AND_NOT_OR                      (1 << 3)
#define MKW2XDMI_CCA_CTRL_OWER_COMP_EN_LQI                     (1 << 2)
#define MKW2XDMI_CCA_CTRL_OWER_COMP_EN_ED                      (1 << 1)
#define MKW2XDMI_CCA_CTRL_OWER_COMP_EN_CCA1                    (1 << 0)

#define MKW2XDMI_CCA2_CORR_PEAKS_CCA2_MIN_NUM_CORR_TH_MASK     0x70u
#define MKW2XDMI_CCA2_CORR_PEAKS_CCA2_MIN_NUM_CORR_TH_SHIFT    4
#define MKW2XDMI_CCA2_CORR_PEAKS_CCA2_MIN_NUM_CORR_TH(x)       (((uint8_t)(((uint8_t)(x))<<MKW2XDMI_CCA2_CORR_PEAKS_CCA2_MIN_NUM_CORR_TH_SHIFT))&MKW2XDMI_CCA2_CORR_PEAKS_CCA2_MIN_NUM_CORR_TH_MASK)
#define MKW2XDMI_CCA2_CORR_PEAKS_CCA2_NUM_CORR_PEAKS_MASK      0x0Fu

#define MKW2XDMI_TMR_PRESCALE_MASK           0x7u
#define MKW2XDMI_TMR_PRESCALE_SHIFT          0x0u
#define MKW2XDMI_TMR_PRESCALE_SET(x)         (((uint8_t)(((uint8_t)(x))<<MKW2XDMI_TMR_PRESCALE_SHIFT))&MKW2XDMI_TMR_PRESCALE_MASK)

#define MKW2XDMI_GPIO_DATA8                  (1 << 7)
#define MKW2XDMI_GPIO_DATA7                  (1 << 6)
#define MKW2XDMI_GPIO_DATA6                  (1 << 5)
#define MKW2XDMI_GPIO_DATA5                  (1 << 4)
#define MKW2XDMI_GPIO_DATA4                  (1 << 3)
#define MKW2XDMI_GPIO_DATA3                  (1 << 2)
#define MKW2XDMI_GPIO_DATA2                  (1 << 1)
#define MKW2XDMI_GPIO_DATA1                  (1 << 0)

#define MKW2XDMI_GPIO_DIR8                   (1 << 7)
#define MKW2XDMI_GPIO_DIR7                   (1 << 6)
#define MKW2XDMI_GPIO_DIR6                   (1 << 5)
#define MKW2XDMI_GPIO_DIR5                   (1 << 4)
#define MKW2XDMI_GPIO_DIR4                   (1 << 3)
#define MKW2XDMI_GPIO_DIR3                   (1 << 2)
#define MKW2XDMI_GPIO_DIR2                   (1 << 1)
#define MKW2XDMI_GPIO_DIR1                   (1 << 0)

#define MKW2XDMI_GPIO_PUL_EN8                (1 << 7)
#define MKW2XDMI_GPIO_PUL_EN7                (1 << 6)
#define MKW2XDMI_GPIO_PUL_EN6                (1 << 5)
#define MKW2XDMI_GPIO_PUL_EN5                (1 << 4)
#define MKW2XDMI_GPIO_PUL_EN4                (1 << 3)
#define MKW2XDMI_GPIO_PUL_EN3                (1 << 2)
#define MKW2XDMI_GPIO_PUL_EN2                (1 << 1)
#define MKW2XDMI_GPIO_PUL_EN1                (1 << 0)

#define MKW2XDMI_GPIO_PUL_SEL8               (1 << 7)
#define MKW2XDMI_GPIO_PUL_SEL7               (1 << 6)
#define MKW2XDMI_GPIO_PUL_SEL6               (1 << 5)
#define MKW2XDMI_GPIO_PUL_SEL5               (1 << 4)
#define MKW2XDMI_GPIO_PUL_SEL4               (1 << 3)
#define MKW2XDMI_GPIO_PUL_SEL3               (1 << 2)
#define MKW2XDMI_GPIO_PUL_SEL2               (1 << 1)
#define MKW2XDMI_GPIO_PUL_SEL1               (1 << 0)

#define MKW2XDMI_GPIO_DS8                    (1 << 7)
#define MKW2XDMI_GPIO_DS7                    (1 << 6)
#define MKW2XDMI_GPIO_DS6                    (1 << 5)
#define MKW2XDMI_GPIO_DS5                    (1 << 4)
#define MKW2XDMI_GPIO_DS4                    (1 << 3)
#define MKW2XDMI_GPIO_DS3                    (1 << 2)
#define MKW2XDMI_GPIO_DS2                    (1 << 1)
#define MKW2XDMI_GPIO_DS1                    (1 << 0)

#define MKW2XDMI_ANT_PAD_CTRL_ANTX_POL3      (1 << 7)
#define MKW2XDMI_ANT_PAD_CTRL_ANTX_POL2      (1 << 6)
#define MKW2XDMI_ANT_PAD_CTRL_ANTX_POL1      (1 << 5)
#define MKW2XDMI_ANT_PAD_CTRL_ANTX_POL0      (1 << 4)
#define MKW2XDMI_ANT_PAD_CTRL_ANTX_CTRLMODE  (1 << 3)
#define MKW2XDMI_ANT_PAD_CTRL_ANTX_HZ        (1 << 2)
#define MKW2XDMI_ANT_PAD_CTRL_ANTX_EN_MASK   0x03u
#define MKW2XDMI_ANT_PAD_CTRL_ANTX_EN(x)     (((uint8_t)(((uint8_t)(x))<<0))&MKW2XDMI_ANT_PAD_CTRL_ANTX_EN_MASK)

#define MKW2XDMI_MISC_PAD_CTRL_MISO_HIZ_EN   (1 << 3)
#define MKW2XDMI_MISC_PAD_CTRL_IRQ_B_OD      (1 << 2)
#define MKW2XDMI_MISC_PAD_CTRL_NON_GPIO_DS   (1 << 1)
#define MKW2XDMI_MISC_PAD_CTRL_ANTX_CURR     (1 << 0)

#define MKW2XDMI_ANT_AGC_CTRL_SNF_EN         (1 << 7)
#define MKW2XDMI_ANT_AGC_CTRL_AGC_EN         (1 << 6)
#define MKW2XDMI_ANT_AGC_CTRL_AGC_LEVEL_MASK  0x30u
#define MKW2XDMI_ANT_AGC_CTRL_AGC_LEVEL_SHIFT 4
#define MKW2XDMI_ANT_AGC_CTRL_AGC_LEVEL(x)   (((uint8_t)(((uint8_t)(x))<<MKW2XDMI_ANT_AGC_CTRL_AGC_LEVEL_SHIFT))&MKW2XDMI_ANT_AGC_CTRL_AGC_LEVEL_MASK)
#define MKW2XDMI_ANT_AGC_CTRL_ANTX           (1 << 1)
#define MKW2XDMI_ANT_AGC_CTRL_AD_EN          (1 << 0)

#define MKW2XDMI_LPPS_CTRL_LPPS_EN           (1 << 0)

#define MKW2XDMI_SOFT_RESET_SOG_RST          (1 << 7)
#define MKW2XDMI_SOFT_RESET_REGS_RST         (1 << 4)
#define MKW2XDMI_SOFT_RESET_PLL_RST          (1 << 3)
#define MKW2XDMI_SOFT_RESET_TX_RST           (1 << 2)
#define MKW2XDMI_SOFT_RESET_RX_RST           (1 << 1)
#define MKW2XDMI_SOFT_RESET_SEQ_MGR_RST      (1 << 0)

#define MKW2XDMI_SEQ_MGR_CTRL_SEQ_STATE_CTRL_MASK       0x3
#define MKW2XDMI_SEQ_MGR_CTRL_SEQ_STATE_CTRL_SHIFT      6
#define MKW2XDMI_SEQ_MGR_CTRL_NO_RX_RECYCLE             (1 << 5)
#define MKW2XDMI_SEQ_MGR_CTRL_LATCH_PREAMBLE            (1 << 4)
#define MKW2XDMI_SEQ_MGR_CTRL_EVENT_TMR_DO_NOT_LATCH    (1 << 3)
#define MKW2XDMI_SEQ_MGR_CTRL_CLR_NEW_SEQ_INHIBIT       (1 << 2)
#define MKW2XDMI_SEQ_MGR_CTRL_PSM_LOCK_DIS              (1 << 1)
#define MKW2XDMI_SEQ_MGR_CTRL_PLL_ABORT_OVRD            (1 << 0)

#define MKW2XDMI_SEQ_MGR_STS_TMR2_SEQ_TRIG_ARMED        (1 << 7)
#define MKW2XDMI_SEQ_MGR_STS_RX_MODE                    (1 << 6)
#define MKW2XDMI_SEQ_MGR_STS_RX_TIMEOUT_PENDING         (1 << 5)
#define MKW2XDMI_SEQ_MGR_STS_NEW_SEQ_INHIBIT            (1 << 4)
#define MKW2XDMI_SEQ_MGR_STS_SEQ_IDLE                   (1 << 3)
#define MKW2XDMI_SEQ_MGR_STS_XCVSEQ_ACTUAL_MASK         7

#define MKW2XDMI_ABORT_STS_PLL_ABORTED                  (1 << 2)
#define MKW2XDMI_ABORT_STS_TC3_ABORTED                  (1 << 1)
#define MKW2XDMI_ABORT_STS_SW_ABORTED                   (1 << 0)

#define MKW2XDMI_TESTMODE_CTRL_HOT_ANT                  (1 << 4)
#define MKW2XDMI_TESTMODE_CTRL_IDEAL_RSSI_EN            (1 << 3)
#define MKW2XDMI_TESTMODE_CTRL_IDEAL_PFC_EN             (1 << 2)
#define MKW2XDMI_TESTMODE_CTRL_CONTINUOUS_EN            (1 << 1)
#define MKW2XDMI_TESTMODE_CTRL_FPGA_EN                  (1 << 0)

#define MKW2XDMI_DTM_CTRL1_ATM_LOCKED                   (1 << 7)
#define MKW2XDMI_DTM_CTRL1_DTM_EN                       (1 << 6)
#define MKW2XDMI_DTM_CTRL1_PAGE5                        (1 << 5)
#define MKW2XDMI_DTM_CTRL1_PAGE4                        (1 << 4)
#define MKW2XDMI_DTM_CTRL1_PAGE3                        (1 << 3)
#define MKW2XDMI_DTM_CTRL1_PAGE2                        (1 << 2)
#define MKW2XDMI_DTM_CTRL1_PAGE1                        (1 << 1)
#define MKW2XDMI_DTM_CTRL1_PAGE0                        (1 << 0)

#define MKW2XDMI_TX_MODE_CTRL_TX_INV                    (1 << 4)
#define MKW2XDMI_TX_MODE_CTRL_BT_EN                     (1 << 3)
#define MKW2XDMI_TX_MODE_CTRL_DTS2                      (1 << 2)
#define MKW2XDMI_TX_MODE_CTRL_DTS1                      (1 << 1)
#define MKW2XDMI_TX_MODE_CTRL_DTS0                      (1 << 0)
#define MKW2XDMI_TX_MODE_CTRL_DTS_MASK                  7

#ifdef __cplusplus
}
#endif

/** @} */
