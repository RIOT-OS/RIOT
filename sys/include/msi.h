/*
 * SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_msi MSI-like mailbox doorbell
 * @ingroup     sys
 * @brief       Architecture-independent message-signaled interrupt mailbox
 *
 * This is not PCI Express MSI. It copies the PCIe MSI idea: a writer posts a
 * payload into a memory slot, then an architecture backend triggers a
 * doorbell.
 *
 * @code{.unparsed}
 * sender                 mailbox                    backend / ISR
 * ------                 -------                    -------------
 * write slot.payload --> event, data
 * write slot.valid   --> published
 * trigger doorbell   -->                            msi_isr()
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
 * @brief   Initialize the mailbox and architecture doorbell
 *
 * The meaning of @p doorbell is defined by the selected architecture
 * backend. The Cortex-M backend interprets it as an external IRQ number.
 *
 * @param[in] doorbell  Architecture-specific doorbell identifier
 *
 * @retval  0     The mailbox and doorbell were initialized
 * @retval  <0    The backend rejected or could not initialize the doorbell
 */
int msi_init(int doorbell);

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
 * Writes the slot, then asks the architecture backend to trigger the
 * doorbell configured in @ref msi_init. The slot must be empty
 * (`valid == 0`).
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
 * The architecture-specific doorbell handler must call this function.
 */
void msi_isr(void);

#ifdef __cplusplus
}
#endif

/** @} */
