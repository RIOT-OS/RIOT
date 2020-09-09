/*
 * Copyright (C) 2009 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * Parts taken from FeuerWhere-Project, lpc2387.h.
 */

#ifndef LPC23XX_H
#define LPC23XX_H

#include "vendor/lpc23xx.h"
#include "arm7_common.h"
#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define F_CCO                   288000000
#define CL_CPU_DIV              4                                   ///< CPU clock divider
#define F_RC_OSCILLATOR         4000000                             ///< Frequency of internal RC oscillator
#define F_RTC_OSCILLATOR        32767                               ///< Frequency of RTC oscillator

#define VIC_SIZE                32

#define PLLCFG_N(n)             ((n - 1) << 16)
#define PLLCFG_M(m)             (m - 1)

#define GPIO_INT 17
#define IRQP_GPIO 4

#define _XTAL       (72000)

/**
 * @name RTC constants
 * @{
 */
#define IMSEC       0x00000001
#define IMMIN       0x00000002
#define IMHOUR      0x00000004
#define IMDOM       0x00000008
#define IMDOW       0x00000010
#define IMDOY       0x00000020
#define IMMON       0x00000040
#define IMYEAR      0x00000080

#define AMRSEC      0x00000001  /* Alarm mask for Seconds */
#define AMRMIN      0x00000002  /* Alarm mask for Minutes */
#define AMRHOUR     0x00000004  /* Alarm mask for Hours */
#define AMRDOM      0x00000008  /* Alarm mask for Day of Month */
#define AMRDOW      0x00000010  /* Alarm mask for Day of Week */
#define AMRDOY      0x00000020  /* Alarm mask for Day of Year */
#define AMRMON      0x00000040  /* Alarm mask for Month */
#define AMRYEAR     0x00000080  /* Alarm mask for Year */

#define ILR_RTCCIF  BIT0
#define ILR_RTCALF  BIT1
#define ILR_RTSSF   BIT2

#define CCR_CLKEN   0x01
#define CCR_CTCRST  0x02
#define CCR_CLKSRC  0x10
/** @} */

/**
 * @name WD constants
 * @{
 */
#define WDEN    BIT0
#define WDRESET BIT1
#define WDTOF   BIT2
#define WDINT   BIT3
/** @} */

/**
 * @name    INTWAKE Constants
 * @{
 */
#define EXTWAKE0    BIT0
#define EXTWAKE1    BIT1
#define EXTWAKE2    BIT2
#define EXTWAKE3    BIT3
#define ETHWAKE     BIT4
#define USBWAKE     BIT5
#define CANWAKE     BIT6
#define GPIO0WAKE   BIT7
#define GPIO2WAKE   BIT8
#define BODWAKE     BIT14
#define RTCWAKE     BIT15
/** @} */

/**
 * @name UART Constants
 * @{
 */
#define ULSR_RDR    BIT0
#define ULSR_OE     BIT1
#define ULSR_PE     BIT2
#define ULSR_FE     BIT3
#define ULSR_BI     BIT4
#define ULSR_THRE   BIT5
#define ULSR_TEMT   BIT6
#define ULSR_RXFE   BIT7

#define UIIR_INT_STATUS     (BIT0)              ///< Interrupt Status
#define UIIR_THRE_INT       (BIT1)              ///< Transmit Holding Register Empty
#define UIIR_RDA_INT        (BIT2)              ///< Receive Data Available
#define UIIR_RLS_INT        (BIT1 | BIT2)       ///< Receive Line Status
#define UIIR_CTI_INT        (BIT2 | BIT3)       ///< Character Timeout Indicator
#define UIIR_ID_MASK        (BIT1 | BIT2 | BIT3)
#define UIIR_ABEO_INT       BIT8
#define UIIR_ABTO_INT       BIT9
/** @} */

/**
 * @name    SSP Status Register Constants
 * @{
 */
#define SSPSR_TFE           BIT0                ///< Transmit FIFO Empty. This bit is 1 is the Transmit FIFO is empty, 0 if not.
#define SSPSR_TNF           BIT1                ///< Transmit FIFO Not Full. This bit is 0 if the Tx FIFO is full, 1 if not.
#define SSPSR_RNE           BIT2                ///< Receive FIFO Not Empty. This bit is 0 if the Receive FIFO is empty, 1 if not.
#define SSPSR_RFF           BIT3                ///< Receive FIFO Full. This bit is 1 if the Receive FIFO is full, 0 if not.
#define SSPSR_BSY           BIT4                ///< Busy. This bit is 0 if the SSPn controller is idle, or 1 if it is currently sending/receiving a frame and/or the Tx FIFO is not empty.
/** @} */

/**
 * @name Timer register offsets
 * @{
 */
#define TXIR           0x00
#define TXTCR          0x04
#define TXTC           0x08                     ///< Timer counter
#define TXPR           0x0C
#define TXPC           0x10
#define TXMCR          0x14
#define TXMR0          0x18
#define TXMR1          0x1C
#define TXMR2          0x20
#define TXMR3          0x24
#define TXCCR          0x28
#define TXCR0          0x2C
#define TXCR1          0x30
#define TXCR2          0x34
#define TXCR3          0x38
#define TXEMR          0x3C
#define TXCTCR         0x70
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LPC23XX_H */
