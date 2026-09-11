/*
 * SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_msi
 * @{
 *
 * @file
 * @brief       Architecture backend interface for MSI-like doorbells
 *
 * This interface separates the architecture-independent mailbox protocol
 * from the mechanism used to notify the receiving CPU.
 *
 * @author      Ajit Upadhyay <u.ajiit@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize an architecture-specific doorbell
 *
 * @param[in] doorbell  Architecture-specific doorbell identifier
 *
 * @retval  0           The doorbell was initialized
 * @retval  <0          The backend rejected or could not initialize it
 */
int msi_arch_init(int doorbell);

/**
 * @brief   Trigger the initialized architecture-specific doorbell
 *
 * @pre     @ref msi_arch_init completed successfully
 */
void msi_arch_trigger(void);

#ifdef __cplusplus
}
#endif

/** @} */
