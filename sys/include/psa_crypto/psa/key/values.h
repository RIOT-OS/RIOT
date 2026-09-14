/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file
 * @brief       Key values definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of required allocated asymmetric key pair slots.
 *
 * @details These should be defined by the developer to
 *          fit their requirements. The default number is 0.
 */
#ifndef CONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT
#define CONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT  0
#endif

/**
 * @brief   Number of required allocated single key slots.
 *
 * @details These should be defined by the developer to
 *          fit their requirements. The default number is 0.
 */
#ifndef CONFIG_PSA_SINGLE_KEY_COUNT
#define CONFIG_PSA_SINGLE_KEY_COUNT  0
#endif

/**
 * @brief   Number of required allocated protected key slots.
 *
 * @details These should be defined by the developer to
 *          fit their requirements. The default number is 5.
 */
#ifndef CONFIG_PSA_PROTECTED_KEY_COUNT
#if (IS_USED(MODULE_PSA_SECURE_ELEMENT))
#define CONFIG_PSA_PROTECTED_KEY_COUNT 5
#else
#define CONFIG_PSA_PROTECTED_KEY_COUNT  0
#endif
#endif

#ifdef __cplusplus
}
#endif

/** @} */
