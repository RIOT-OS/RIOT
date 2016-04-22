/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc26x0_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 UART interface
 *
 */

#ifndef CC26x0_UART_H
#define CC26x0_UART_H

#include "cc26x0.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_BASE       (0x40001000) /**< UART base address */

/**
 * @brief UART component registers
 */
typedef struct {
    reg32_t DR; /**< data */
    union {
        reg32_t RSR; /**< status */
        reg32_t ECR; /**< error clear */
    };
    reg32_t __reserved1[4]; /**< meh */
    reg32_t FR; /**< flag */
    reg32_t __reserved2[2]; /**< meh */
    reg32_t IBRD; /**< integer baud-rate divisor */
    reg32_t FBRD; /**< fractional baud-rate divisor */
    reg32_t LCRH; /**< line control */
    reg32_t CTL; /**< control */
    reg32_t IFLS; /**< interrupt fifo level select */
    reg32_t IMSC; /**< interrupt mask set/clear */
    reg32_t RIS; /**< raw interrupt status */
    reg32_t MIS; /**< masked interrupt status */
    reg32_t ICR; /**< interrupt clear */
    reg32_t DMACTL; /**< DMA control */
} uart_regs_t;

/**
 * @brief   UART register values
 * @{
 */
#define UART_DR_DATA_mask       0xFF
#define UART_DR_FE              0x100
#define UART_DR_PE              0x200
#define UART_DR_BE              0x400
#define UART_DR_OE              0x800

#define UART_ECR_FE             0x1
#define UART_ECR_PE             0x2
#define UART_ECR_BE             0x4
#define UART_ECR_OE             0x8

#define UART_FR_CTS             0x1
#define UART_FR_BUSY            0x4
#define UART_FR_RXFE            0x10
#define UART_FR_TXFF            0x20
#define UART_FR_RXFF            0x40
#define UART_FR_TXFE            0x80

#define UART_LCRH_PEN           0x1
#define UART_LCRH_EPS           0x2
#define UART_LCRH_RXFE          0x4
#define UART_LCRH_STP2          0x8
#define UART_LCRH_FEN           0x10
#define UART_LCRH_WLEN_mask     0x60
#define UART_LCRH_WLEN_5        0x0
#define UART_LCRH_WLEN_6        0x20
#define UART_LCRH_WLEN_7        0x40
#define UART_LCRH_WLEN_8        0x60
#define UART_LCRH_SPS           0x80

#define UART_CTL_UARTEN         0x1
#define UART_CTL_LBE            0x80
#define UART_CTL_TXE            0x100
#define UART_CTL_RXE            0x200
#define UART_CTL_RTS            0x800
#define UART_CTL_RTSEN          0x4000
#define UART_CTL_CTSEN          0x8000

#define UART_MIS_CTSMMIS        0x1
#define UART_MIS_RXMIS          0x10
#define UART_MIS_TXMIS          0x20
#define UART_MIS_RTMIS          0x40
#define UART_MIS_FEMIS          0x80
#define UART_MIS_PEMIS          0x100
#define UART_MIS_BEMIS          0x200
#define UART_MIS_OEMIS          0x400

#define UART_IMSC_CTSMIM        0x2
#define UART_IMSC_RXIM          0x10
#define UART_IMSC_TXIM          0x20
#define UART_IMSC_RTIM          0x40
#define UART_IMSC_FEIM          0x80
#define UART_IMSC_PEIM          0x100
#define UART_IMSC_BEIM          0x200
#define UART_IMSC_OEIM          0x400

#define UART_IFLS_TXSEL_1_8     0x0
#define UART_IFLS_TXSEL_2_8     0x1
#define UART_IFLS_TXSEL_4_8     0x2
#define UART_IFLS_TXSEL_6_8     0x3
#define UART_IFLS_TXSEL_7_8     0x4

#define UART_IFLS_RXSEL_1_8     0x0
#define UART_IFLS_RXSEL_2_8     0x8
#define UART_IFLS_RXSEL_4_8     0x10
#define UART_IFLS_RXSEL_6_8     0x18
#define UART_IFLS_RXSEL_7_8     0x20
/** @} */

#define UART ((uart_regs_t *) (UART_BASE)) /**< UART register bank */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26x8_UART_H */
/** @} */
