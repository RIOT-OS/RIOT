/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_cryptoauthlib
 * @{
 *
 * @file
 * @brief       Minimal configuration for the library build.
 *              For documentation please see https://github.com/MicrochipTech/cryptoauthlib#readme
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ATECC508A device support
 */
#define ATCA_ATECC508A_SUPPORT

/**
 * @brief ATECC608A device support
 */
#define ATCA_ATECC608A_SUPPORT

/**
 * @brief Map atca delay function to hal delay ms implementation
 */
#define atca_delay_ms   hal_delay_ms

/**
 * @brief Map atca delay function to hal delay us implementation
 */
#define atca_delay_us   hal_delay_us

#ifdef __cplusplus
}
#endif
/** @} */
