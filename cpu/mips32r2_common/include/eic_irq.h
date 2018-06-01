/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_mips32r2_common Imagination Technologies MIPS32R2 Common
 * @ingroup         cpu
 * @brief           Imagination Technologies MIPS32R2 Common implementation
 * @{
 *
 * @file
 * @brief       API for supporting External Interrupt Controllers (EIC mode)
 *
 * @author      Neil Jones <neil.jones@imgtec.com>
 */

#ifndef EIC_IRQ_H
#define EIC_IRQ_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ brief Internal Interrupt numbers
 *
 * MIPS cores have a few internally generated interrupts from the Timer,
 * Performance Counters and Fast Debug Channel hardware, in EIC mode these
 * become outputs from the core and are connected to the external controller,
 * the external control then loops these back at whichever IPL it decides
 *
 * We use negative numbers to represent these, leaving positive numbers free for
 * the SoC specific interrupts
 * @{
 */
#define EIC_IRQ_TIMER (-1)
#define EIC_IRQ_FDC   (-2)
#define EIC_IRQ_PC    (-3)
/** @} */

/**
 * @brief   Configure and route the interrupt
 */
void eic_irq_configure(int irq_num);

/**
 * @brief   Enable an interrupt
 */
void eic_irq_enable(int irq_num);

/**
 * @brief   Disable an interrupt
 */
void eic_irq_disable(int irq_num);

/**
 * @brief   Acknowledge an interrupt
 */
void eic_irq_ack(int irq_num);

#ifdef __cplusplus
}
#endif

#endif /* EIC_IRQ_H */
/** @} */
