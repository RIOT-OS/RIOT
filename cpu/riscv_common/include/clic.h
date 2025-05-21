/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_riscv_common
 * @{
 *
 * @file
 * @brief           RISCV CLIC interrupt controller definitions
 *
 * RISCV implementations using this peripheral must define the `CLIC_BASE_ADDR`,
 * in order to use the PLIC as interrupt controller. Also required is
 * CLIC_NUM_INTERRUPTS.
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 */

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CLIC callback declaration
 *
 * @param   irq     Interrupt number
 */
typedef void (*clic_isr_cb_t)(unsigned irq);

/**
 * @brief RISC-V CLIC per interrupt configuration registers
 */
typedef struct __attribute((packed)) {
    volatile uint8_t ip;        /**< Interrupt pending */
    volatile uint8_t ie;        /**< Interrupt enable */
    volatile uint8_t attr;      /**< Interrupt attributes */
    volatile uint8_t ctl;       /**< Interrupt control */
} clic_clicint_t;

/**
 * @brief Initialize the CLIC interrupt controller
 */
void clic_init(void);

/**
 * @brief Enable a single interrupt
 *
 * @param   irq         Interrupt number to enable
 * @param   priority    Priority level to configure
 */
void clic_enable_interrupt(unsigned irq, unsigned priority);

/**
 * @brief Disable a single interrupt
 *
 * @param   irq     Interrupt number to disable
 */
void clic_disable_interrupt(unsigned irq);

/**
 * @brief Set the priority of an interrupt
 *
 * @param   irq         Interrupt number to configure
 * @param   priority    Priority level to configure
 */
void clic_set_priority(unsigned irq, unsigned priority);

/**
 * @brief Set the handler for an interrupt
 *
 * @param   irq     Interrupt number to configure
 * @param   cb      Callback handler to configure
 */
void clic_set_handler(unsigned irq, clic_isr_cb_t cb);

/**
 * @brief CLIC interrupt handler
 *
 * @internal
 *
 * @param   irq     Interrupt number to call the handler for
 */
void clic_isr_handler(uint32_t irq);

#ifdef __cplusplus
}
#endif

/** @} */
