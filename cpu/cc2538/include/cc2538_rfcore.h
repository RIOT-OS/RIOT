/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief           CC2538 RF core interface
 *
 * @author          Ian Martin <ian@locicontrols.com>
 *
 * @addtogroup cpu_cc2538
 * @{
 *
 * @defgroup cc2538-rfcore CC2538 RF core interface
 * @{
 */

#ifndef CC2538_RFCORE_H
#define CC2538_RFCORE_H

#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RF Core component registers
 */
typedef struct {
    cc2538_reg_t FFSM_SRCRESMASK0;        /**< RF Source address matching result */
    cc2538_reg_t FFSM_SRCRESMASK1;        /**< RF Source address matching result */
    cc2538_reg_t FFSM_SRCRESMASK2;        /**< RF Source address matching result */
    cc2538_reg_t FFSM_SRCRESINDEX;        /**< RF Source address matching result */
    cc2538_reg_t FFSM_SRCEXTPENDEN0;      /**< RF Source address matching control */
    cc2538_reg_t FFSM_SRCEXTPENDEN1;      /**< RF Source address matching control */
    cc2538_reg_t FFSM_SRCEXTPENDEN2;      /**< RF Source address matching control */
    cc2538_reg_t FFSM_SRCSHORTPENDEN0;    /**< RF Source address matching control */
    cc2538_reg_t FFSM_SRCSHORTPENDEN1;    /**< RF Source address matching control */
    cc2538_reg_t FFSM_SRCSHORTPENDEN2;    /**< RF Source address matching control */
    cc2538_reg_t FFSM_EXT_ADDR0;          /**< RF Local address information */
    cc2538_reg_t FFSM_EXT_ADDR1;          /**< RF Local address information */
    cc2538_reg_t FFSM_EXT_ADDR2;          /**< RF Local address information */
    cc2538_reg_t FFSM_EXT_ADDR3;          /**< RF Local address information */
    cc2538_reg_t FFSM_EXT_ADDR4;          /**< RF Local address information */
    cc2538_reg_t FFSM_EXT_ADDR5;          /**< RF Local address information */
    cc2538_reg_t FFSM_EXT_ADDR6;          /**< RF Local address information */
    cc2538_reg_t FFSM_EXT_ADDR7;          /**< RF Local address information */
    cc2538_reg_t FFSM_PAN_ID0;            /**< RF Local address information */
    cc2538_reg_t FFSM_PAN_ID1;            /**< RF Local address information */
    cc2538_reg_t FFSM_SHORT_ADDR0;        /**< RF Local address information */
    cc2538_reg_t FFSM_SHORT_ADDR1;        /**< RF Local address information */
    cc2538_reg_t RESERVED1[10];           /**< Reserved bytes */
    cc2538_reg_t XREG_FRMFILT0;           /**< RF Frame Filter 0 */
    cc2538_reg_t XREG_FRMFILT1;           /**< RF Frame Filter 1 */
    cc2538_reg_t XREG_SRCMATCH;           /**< RF Source address matching and pending bits */
    cc2538_reg_t XREG_SRCSHORTEN0;        /**< RF Short address matching */
    cc2538_reg_t XREG_SRCSHORTEN1;        /**< RF Short address matching */
    cc2538_reg_t XREG_SRCSHORTEN2;        /**< RF Short address matching */
    cc2538_reg_t XREG_SRCEXTEN0;          /**< RF Extended address matching */
    cc2538_reg_t XREG_SRCEXTEN1;          /**< RF Extended address matching */
    cc2538_reg_t XREG_SRCEXTEN2;          /**< RF Extended address matching */
    cc2538_reg_t XREG_FRMCTRL0;           /**< RF Frame handling */
    cc2538_reg_t XREG_FRMCTRL1;           /**< RF Frame handling */
    cc2538_reg_t XREG_RXENABLE;           /**< RF RX enabling */
    cc2538_reg_t XREG_RXMASKSET;          /**< RF RX enabling */
    cc2538_reg_t XREG_RXMASKCLR;          /**< RF RX disabling */
    cc2538_reg_t XREG_FREQTUNE;           /**< RF Crystal oscillator frequency tuning */
    cc2538_reg_t XREG_FREQCTRL;           /**< RF Controls the RF frequency */
    cc2538_reg_t XREG_TXPOWER;            /**< RF Controls the output power */
    cc2538_reg_t XREG_TXCTRL;             /**< RF Controls the TX settings */
    cc2538_reg_t XREG_FSMSTAT0;           /**< RF Radio status register */
    cc2538_reg_t XREG_FSMSTAT1;           /**< RF Radio status register */
    cc2538_reg_t XREG_FIFOPCTRL;          /**< RF FIFOP threshold */
    cc2538_reg_t XREG_FSMCTRL;            /**< RF FSM options */
    cc2538_reg_t XREG_CCACTRL0;           /**< RF CCA threshold */
    cc2538_reg_t XREG_CCACTRL1;           /**< RF Other CCA Options */
    cc2538_reg_t XREG_RSSI;               /**< RF RSSI status register */

    union {
        cc2538_reg_t XREG_RSSISTAT;       /**< RF RSSI valid status register */
        struct {
            cc2538_reg_t RSSI_VALID :  1; /**< RSSI value is valid */
            cc2538_reg_t RESERVED   : 31; /**< Reserved bits */
        } XREG_RSSISTATbits;
    } cc2538_rfcore_xreg_rssistat;

    cc2538_reg_t XREG_RXFIRST;            /**< RF First byte in RX FIFO */
    cc2538_reg_t XREG_RXFIFOCNT;          /**< RF Number of bytes in RX FIFO */
    cc2538_reg_t XREG_TXFIFOCNT;          /**< RF Number of bytes in TX FIFO */
    cc2538_reg_t XREG_RXFIRST_PTR;        /**< RF RX FIFO pointer */
    cc2538_reg_t XREG_RXLAST_PTR;         /**< RF RX FIFO pointer */
    cc2538_reg_t XREG_RXP1_PTR;           /**< RF RX FIFO pointer */
    cc2538_reg_t RESERVED2;               /**< Reserved bytes */
    cc2538_reg_t XREG_TXFIRST_PTR;        /**< RF TX FIFO pointer */
    cc2538_reg_t XREG_TXLAST_PTR;         /**< RF TX FIFO pointer */
    cc2538_reg_t XREG_RFIRQM0;            /**< RF interrupt masks */
    cc2538_reg_t XREG_RFIRQM1;            /**< RF interrupt masks */
    cc2538_reg_t XREG_RFERRM;             /**< RF error interrupt mask */
    cc2538_reg_t RESERVED3;               /**< Reserved bytes */

    union {
        cc2538_reg_t XREG_RFRND;          /**< RF Random data */
        struct {
            cc2538_reg_t IRND       :  1; /**< Random bit from the I channel of the receiver */
            cc2538_reg_t QRND       :  1; /**< Random bit from the Q channel of the receiver */
            cc2538_reg_t RESERVED   : 30; /**< Reserved bits */
        } XREG_RFRNDbits;
    } cc2538_rfcore_xreg_rfrnd;

    cc2538_reg_t XREG_MDMCTRL0;           /**< RF Controls modem */
    cc2538_reg_t XREG_MDMCTRL1;           /**< RF Controls modem */
    cc2538_reg_t XREG_FREQEST;            /**< RF Estimated RF frequency offset */
    cc2538_reg_t XREG_RXCTRL;             /**< RF Tune receive section */
    cc2538_reg_t XREG_FSCTRL;             /**< RF Tune frequency synthesizer */
    cc2538_reg_t XREG_FSCAL0;             /**< RF Tune frequency calibration */
    cc2538_reg_t XREG_FSCAL1;             /**< RF Tune frequency calibration */
    cc2538_reg_t XREG_FSCAL2;             /**< RF Tune frequency calibration */
    cc2538_reg_t XREG_FSCAL3;             /**< RF Tune frequency calibration */
    cc2538_reg_t XREG_AGCCTRL0;           /**< RF AGC dynamic range control */
    cc2538_reg_t XREG_AGCCTRL1;           /**< RF AGC reference level */
    cc2538_reg_t XREG_AGCCTRL2;           /**< RF AGC gain override */
    cc2538_reg_t XREG_AGCCTRL3;           /**< RF AGC control */
    cc2538_reg_t XREG_ADCTEST0;           /**< RF ADC tuning */
    cc2538_reg_t XREG_ADCTEST1;           /**< RF ADC tuning */
    cc2538_reg_t XREG_ADCTEST2;           /**< RF ADC tuning */
    cc2538_reg_t XREG_MDMTEST0;           /**< RF Test register for modem */
    cc2538_reg_t XREG_MDMTEST1;           /**< RF Test Register for Modem */
    cc2538_reg_t XREG_DACTEST0;           /**< RF DAC override value */
    cc2538_reg_t XREG_DACTEST1;           /**< RF DAC override value */
    cc2538_reg_t XREG_DACTEST2;           /**< RF DAC test setting */
    cc2538_reg_t XREG_ATEST;              /**< RF Analog test control */
    cc2538_reg_t XREG_PTEST0;             /**< RF Override power-down register */
    cc2538_reg_t XREG_PTEST1;             /**< RF Override power-down register */
    cc2538_reg_t RESERVED4[32];           /**< Reserved bytes */
    cc2538_reg_t XREG_CSPCTRL;            /**< RF CSP control bit */
    cc2538_reg_t XREG_CSPSTAT;            /**< RF CSP status register */
    cc2538_reg_t XREG_CSPX;               /**< RF CSP X data register */
    cc2538_reg_t XREG_CSPY;               /**< RF CSP Y data register */
    cc2538_reg_t XREG_CSPZ;               /**< RF CSP Z data register */
    cc2538_reg_t XREG_CSPT;               /**< RF CSP T data register */
    cc2538_reg_t RESERVED5[5];            /**< Reserved bytes */
    cc2538_reg_t XREG_RFC_OBS_CTRL0;      /**< RF observation mux control */
    cc2538_reg_t XREG_RFC_OBS_CTRL1;      /**< RF observation mux control */
    cc2538_reg_t XREG_RFC_OBS_CTRL2;      /**< RF observation mux control */
    cc2538_reg_t RESERVED6[12];           /**< Reserved bytes */
    cc2538_reg_t XREG_TXFILTCFG;          /**< RF TX filter configuration */
    cc2538_reg_t RESERVED7[5];            /**< Reserved bytes */
    cc2538_reg_t SFR_MTCSPCFG;            /**< RF MAC Timer event configuration */
    cc2538_reg_t SFR_MTCTRL;              /**< RF MAC Timer control register */
    cc2538_reg_t SFR_MTIRQM;              /**< RF MAC Timer interrupt mask */
    cc2538_reg_t SFR_MTIRQF;              /**< RF MAC Timer interrupt flags */
    cc2538_reg_t SFR_MTMSEL;              /**< RF MAC Timer multiplex select */
    cc2538_reg_t SFR_MTM0;                /**< RF MAC Timer multiplexed register 0 */
    cc2538_reg_t SFR_MTM1;                /**< RF MAC Timer multiplexed register 1 */
    cc2538_reg_t SFR_MTMOVF2;             /**< RF MAC Timer multiplexed overflow register 2 */
    cc2538_reg_t SFR_MTMOVF1;             /**< RF MAC Timer multiplexed overflow register 1 */
    cc2538_reg_t SFR_MTMOVF0;             /**< RF MAC Timer multiplexed overflow register 0 */
    cc2538_reg_t SFR_RFDATA;              /**< RF Tx/Rx FIFO */
    cc2538_reg_t SFR_RFERRF;              /**< RF error interrupt flags */
    cc2538_reg_t SFR_RFIRQF1;             /**< RF interrupt flags */
    cc2538_reg_t SFR_RFIRQF0;             /**< RF interrupt flags */
    cc2538_reg_t SFR_RFST;                /**< RF CSMA-CA/strobe processor */
} cc2538_rfcore_t;

#define RFCORE ( (cc2538_rfcore_t*)0x40088580 ) /**< RF Core instance */

/** @brief RF Core opcodes */
enum {
    DECZ           = 0xc5,                /**< Decrement Z */
    DECY           = 0xc4,                /**< Decrement Y */
    DECX           = 0xc3,                /**< Decrement X */
    INCZ           = 0xc2,                /**< Increment Z */
    INCY           = 0xc1,                /**< Increment Y */
    INCX           = 0xc0,                /**< Increment X */
    INCMAXY        = 0xc8,                /**< Increment Y not greater than M. | M (M = 0-7) */
    RANDXY         = 0xbd,                /**< Load random value into X. */
    INT            = 0xba,                /**< Interrupt */
    WAITX          = 0xbc,                /**< Wait for X MAC timer overflows */
    SETCMP1        = 0xbe,                /**< Set the compare value of the MAC timer to the current timer value. */
    WAIT_W         = 0x80,                /**< Wait for W MAC timer overflows | W (W = 0-31) */
    WEVENT1        = 0xb8,                /**< Wait until MAC timer event 1 */
    WEVENT2        = 0xb9,                /**< Wait until MAC timer event 2 */
    LABEL          = 0xbb,                /**< Set loop label */
    RPT_C          = 0xa0,                /**< Conditional repeat | N | C (N = 0, 8; C = 0-7) */
    SKIP_S_C       = 0x00,                /**< Conditional skip instruction | S | N | C */
    STOP           = 0xd2,                /**< Stop program execution */
    SNOP           = 0xd0,                /**< No operation */
    SRXON          = 0xd3,                /**< Enable and calibrate frequency synthesizer for RX */
    STXON          = 0xd9,                /**< Enable TX after calibration */
    STXONCCA       = 0xda,                /**< Enable calibration and TX if CCA indicates a clear channel */
    SSAMPLECCA     = 0xdb,                /**< Sample the current CCA value to SAMPLED_CCA */
    SRFOFF         = 0xdf,                /**< Disable RX or TX and frequency synthesizer. */
    SFLUSHRX       = 0xdd,                /**< Flush RX FIFO buffer and reset demodulator */
    SFLUSHTX       = 0xde,                /**< Flush TX FIFO buffer */
    SACK           = 0xd6,                /**< Send acknowledge frame with pending field cleared */
    SACKPEND       = 0xd7,                /**< Send acknowledge frame with the pending field set */
    SNACK          = 0xd8,                /**< Abort sending of acknowledge frame */
    SRXMASKBITSET  = 0xd4,                /**< Set bit in RXENABLE register */
    SRXMASKBITCLR  = 0xd5,                /**< Clear bit in RXENABLE register */
    ISSTOP         = 0xe2,                /**< Stop program execution */
    ISSTART        = 0xe1,                /**< Start program execution */
    ISRXON         = 0xe3,                /**< Enable and calibrate frequency synthesizer for RX */
    ISRXMASKBITSET = 0xe4,                /**< Set bit in RXENABLE */
    ISRXMASKBITCLR = 0xe5,                /**< Clear bit in RXENABLE */
    ISTXON         = 0xe9,                /**< Enable TX after calibration */
    ISTXONCCA      = 0xea,                /**< Enable calibration and TX if CCA indicates a clear channel */
    ISSAMPLECCA    = 0xeb,                /**< Sample the current CCA value to SAMPLED_CCA */
    ISRFOFF        = 0xef,                /**< Disable RX or TX, and the frequency synthesizer. */
    ISFLUSHRX      = 0xed,                /**< Flush RX FIFO buffer and reset demodulator */
    ISFLUSHTX      = 0xee,                /**< Flush TX FIFO buffer */
    ISACK          = 0xe6,                /**< Send acknowledge frame with the pending field cleared */
    ISACKPEND      = 0xe7,                /**< Send acknowledge frame with the pending field set */
    ISNACK         = 0xe8,                /**< Abort sending of acknowledge frame */
    ISCLEAR        = 0xff,                /**< Clear CSP program memory, reset program counter */
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CC2538_RFCORE_H */

/** @} */
/** @} */
/** @} */
