/*
 * SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_msi MSI-like mailbox doorbell
 * @ingroup     sys
 * @brief       Message-signaled interrupt style mailbox for Cortex-M
 *
 * This is not PCI Express MSI, and it is not the STM32 MSI oscillator.
 * It copies the PCIe MSI idea: a writer posts a payload into a memory
 * slot, then a doorbell write pends an NVIC IRQ.
 *
 * @code{.unparsed}
 * thread                 mailbox                    NVIC / ISR
 * ------                 -------                    ---------
 * write slot.payload --> event, data
 * write slot.valid   --> published
 * NVIC_SetPendingIRQ -->                            msi_isr()
 *                                                   callback()
 * @endcode
 *
 * Each vector is one independent `{mailbox, doorbell}` pair, similar to
 * one MSI-X table entry.
 *
 * @{
 *
 * @file
 * @brief       MSI-like mailbox doorbell API
 *
 * @author      Ajit Upadhyay <u.ajiit@gmail.com>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of MSI-like vectors (mailbox slots)
 */
#ifndef MSI_VEC_NUMOF
#  define MSI_VEC_NUMOF    (4U)
#endif

/**
 * @brief   Callback invoked from `msi_isr()` for a posted vector
 *
 * @param[in] vec    Vector that was posted
 * @param[in] event  Event code written by the sender
 * @param[in] data   Payload written by the sender
 * @param[in] arg    Argument given to @ref msi_register
 */
typedef void (*msi_cb_t)(unsigned vec, uint32_t event, uint32_t data, void *arg);

/**
 * @brief   Initialize the mailbox and enable @p irqn as the doorbell
 *
 * @param[in] irqn  Cortex-M IRQ number (`IRQn_Type`), for example
 *                  `HASH_RNG_IRQn` on STM32F746
 *
 * @return  0 on success
 * @return  -EINVAL if @p irqn is out of range
 */
int msi_init(int irqn);

/**
 * @brief   Register the handler for one vector
 *
 * @param[in] vec  Vector index, `0 .. MSI_VEC_NUMOF-1`
 * @param[in] cb   Function called from the doorbell ISR
 * @param[in] arg  Passed to @p cb
 *
 * @return  0 on success
 * @return  -EINVAL if @p vec is invalid or @p cb is NULL
 */
int msi_register(unsigned vec, msi_cb_t cb, void *arg);

/**
 * @brief   Post a message and ring the doorbell
 *
 * Writes the slot, then pends the IRQ configured in @ref msi_init.
 * The slot must be empty (`valid == 0`).
 *
 * @param[in] vec    Vector index
 * @param[in] event  Event code
 * @param[in] data   Payload
 *
 * @return  0 on success
 * @return  -EINVAL if @p vec is invalid or the doorbell is not initialized
 * @return  -EBUSY  if the slot is still occupied
 */
int msi_post(unsigned vec, uint32_t event, uint32_t data);

/**
 * @brief   Drain posted slots and invoke registered callbacks
 *
 * Call this from the board/CPU ISR that matches the IRQ given to
 * @ref msi_init. On STM32F746 that is typically `isr_hash_rng()`.
 */
void msi_isr(void);

#ifdef __cplusplus
}
#endif

/** @} */
