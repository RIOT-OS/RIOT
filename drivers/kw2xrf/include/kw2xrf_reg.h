/*
 * Copyright (C) 2015 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 *
 * @file
 * @brief       Register and command definitions for the MKW2XD Modem
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef MKW2XD_MODEM_REG_H
#define MKW2XD_MODEM_REG_H

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

#define MKW2XDM_PHY_CTRL1_TMRTRIGEN                 (1 << 7)
#define MKW2XDM_PHY_CTRL1_SLOTTED                   (1 << 6)
#define MKW2XDM_PHY_CTRL1_CCABFRTX                  (1 << 5)
#define MKW2XDM_PHY_CTRL1_RXACKRQD                  (1 << 4)
#define MKW2XDM_PHY_CTRL1_AUTOACK                   (1 << 3)
#define MKW2XDM_PHY_CTRL1_XCVSEQ_MASK               0x03u
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
    MKW2XDMI_GPIO_DATA = 0x2a,             /**< GPIO Data */
    MKW2XDMI_GPIO_DIR = 0x2b,              /**< GPIO Direction Control */
    MKW2XDMI_GPIO_PUL_EN = 0x2c,           /**< GPIO Pullup Enable */
    MKW2XDMI_GPIO_SEL = 0x2d,              /**< GPIO Pullup Select */
    MKW2XDMI_GPIO_DS = 0x2e,               /**< GPIO Drive Strength */
    MKW2XDMI_ANT_PAD_CTRL = 0x30,          /**< Antenna Control */
    MKW2XDMI_MISC_PAD_CTRL = 0x31,         /**< Miscellaneous Pad Control */
    MKW2XDMI_ANT_AGC_CTRL = 0x51,          /**< Antenna AGC and FAD Control */
    MKW2XDMI_LPPS_CTRL = 0x56,             /**< LPPS_CTRL */
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

#ifdef __cplusplus
}
#endif

#endif
/** @} */
