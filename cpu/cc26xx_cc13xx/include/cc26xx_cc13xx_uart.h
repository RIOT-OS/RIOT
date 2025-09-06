/*
 * SPDX-FileCopyrightText: 2016 Leon George
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup      cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx UART interface
 *
 */

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    UART component registers
 */
typedef struct {
    reg32_t DR; /**< Data */
    union {
        reg32_t RSR; /**< Status */
        reg32_t ECR; /**< Error clear */
    };
    reg32_t __reserved1[4]; /**< Reserved */
    reg32_t FR; /**< flag */
    reg32_t __reserved2[2]; /**< Reserved */
    reg32_t IBRD; /**< Integer baud-rate divisor */
    reg32_t FBRD; /**< Fractional baud-rate divisor */
    reg32_t LCRH; /**< Line control */
    reg32_t CTL; /**< Control */
    reg32_t IFLS; /**< Interrupt fifo level select */
    reg32_t IMSC; /**< Interrupt mask set/clear */
    reg32_t RIS; /**< Raw interrupt status */
    reg32_t MIS; /**< Masked interrupt status */
    reg32_t ICR; /**< Interrupt clear */
    reg32_t DMACTL; /**< MMA control */
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

#define UART_LCRH_PEN           0x2
#define UART_LCRH_EPS           0x4
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

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define UART0_BASE       (PERIPH_BASE + 0x1000) /**< UART0 base address */
#define UART1_BASE       (PERIPH_BASE + 0xB000) /**< UART1 base address */
/** @} */

/**
 * @brief   UART0 register bank
 */
#define UART0            ((uart_regs_t *) (UART0_BASE))
/**
 * @brief   UART1 register bank
 */
#define UART1            ((uart_regs_t *) (UART1_BASE))

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
