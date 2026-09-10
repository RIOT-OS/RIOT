/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdint.h>

/**
 * @file
 * @brief   GRLIB GRGPIO (General Purpose I/O port) register definitions
 *
 * Reference: GRLIB IP Core User's Manual, GRGPIO section.
 */

/**
 * @brief   GRGPIO register layout
 */
typedef struct {
    volatile uint32_t data;     /**< 0x00 - I/O port value (read=pin level, write=output latch) */
    volatile uint32_t output;   /**< 0x04 - Output register (drives the output pins) */
    volatile uint32_t dir;      /**< 0x08 - Direction register (1=output, 0=input) */
    volatile uint32_t imask;    /**< 0x0C - Interrupt mask (1=interrupt enabled) */
    volatile uint32_t ipol;     /**< 0x10 - Interrupt polarity (1=active high / rising edge) */
    volatile uint32_t iedge;    /**< 0x14 - Interrupt edge (1=edge triggered, 0=level triggered) */
} grgpio_t;

/** @brief  Access GRGPIO registers from a base address */
#define GRGPIO_DEV(base)    ((grgpio_t *)(uintptr_t)(base))
