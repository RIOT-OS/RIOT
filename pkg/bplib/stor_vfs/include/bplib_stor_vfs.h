/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Notify the storage implementation to clear the cache for the given contact
 */
void bplib_stor_vfs_contact_changed(uint32_t contact_index);

/**
 * @brief Notify the storage implementation to clear the cache for the given channel
 */
void bplib_stor_vfs_channel_changed(uint32_t channel_index);

#ifdef __cplusplus
}
#endif
