/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdint.h>

/**
 * @file
 * @brief   GRLIB APBUART (Generic UART) register definitions
 *
 * Reference: GRLIB IP Core User's Manual, APBUART section.
 */

/* Register offsets */
#define APBUART_DATA        0x00    /* Data register (RX/TX) */
#define APBUART_STATUS      0x04    /* Status register */
#define APBUART_CTRL        0x08    /* Control register */
#define APBUART_SCALER      0x0C    /* Scaler reload register */
#define APBUART_FIFO        0x10    /* FIFO debug register */

/* Status register bits */
#define APBUART_STATUS_DR   (1 << 0)  /* Data Ready  */
#define APBUART_STATUS_TS   (1 << 1)  /* TX Shift register empty */
#define APBUART_STATUS_TE   (1 << 2)  /* TX FIFO empty */
#define APBUART_STATUS_BR   (1 << 3)  /* Break received */
#define APBUART_STATUS_OV   (1 << 4)  /* Overrun error */
#define APBUART_STATUS_PE   (1 << 5)  /* Parity error */
#define APBUART_STATUS_FE   (1 << 6)  /* Framing error */
#define APBUART_STATUS_TH   (1 << 7)  /* TX FIFO half full */
#define APBUART_STATUS_RH   (1 << 8)  /* RX FIFO half full */
#define APBUART_STATUS_TF   (1 << 9)  /* TX FIFO full */
#define APBUART_STATUS_RF   (1 << 10) /* RX FIFO full */

/* Control register bits */
#define APBUART_CTRL_RE     (1 << 0)  /* Receiver enable */
#define APBUART_CTRL_TE     (1 << 1)  /* Transmitter enable */
#define APBUART_CTRL_RI     (1 << 2)  /* Receiver interrupt enable */
#define APBUART_CTRL_TI     (1 << 3)  /* Transmitter interrupt enable */
#define APBUART_CTRL_PS     (1 << 4)  /* Parity select (0=even, 1=odd) */
#define APBUART_CTRL_PE     (1 << 5)  /* Parity enable */
#define APBUART_CTRL_FL     (1 << 6)  /* Flow control enable */
#define APBUART_CTRL_LB     (1 << 7)  /* Loopback enable */
#define APBUART_CTRL_DB     (1 << 11) /* FIFO debug mode enable */

/**
 *  scaler:
 * BAUD = SYSCLK / ((SCALER + 1) * 8)
 * SCALER = SYSCLK / (BAUD * 8) - 1
 */
#define APBUART_SCALER_VAL(clk, baud)   ((clk) / ((baud) * 8) - 1)

/* Macros to access registers */
#define APBUART_REG(base, offset)   (*(volatile uint32_t *)((uintptr_t)(base) + (offset)))
