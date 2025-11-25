/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_riscv_common
 * @{
 *
 * @file
 * @brief       Platform-Level interrupt controller driver
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef PLIC_H
#define PLIC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   PLIC callback declaration
 *
 * @param   irq     Interrupt number
 */
typedef void (*plic_isr_cb_t)(int irq);

/**
 * @brief Initialize the Platform-level interrupt controller
 */
void plic_init(void);

/**
 * @brief   Disable an interrupt on the PLIC
 *
 * @param   irq     Interrupt number
 */
void plic_disable_interrupt(unsigned irq);

/**
 * @brief   Enable an interrupt on the PLIC
 *
 * @param   irq     Interrupt number
 */
void plic_enable_interrupt(unsigned irq);

/**
 * @brief   Set an interrupt priority
 *
 * @param   irq         Interrupt number
 * @param   priority    Priority
 */
void plic_set_priority(unsigned irq, unsigned priority);

/**
 * @brief   Set the interrupt callback
 *
 * @param   irq     Interrupt number
 * @param   cb      Callback to call on interrupt
 */
void plic_set_isr_cb(unsigned irq, plic_isr_cb_t cb);

/**
 * @brief   External ISR callback
 */

/**
 * @brief   Interrupt handler for the PLIC
 *
 * Must be called from the trap handler when an interrupt from the PLIC is
 * pending
 */
void plic_isr_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* PLIC_H */
/** @} */
