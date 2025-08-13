/*
 * SPDX-FileCopyrightText: 2015 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_mulle
 * @{
 *
 * @file
 * @brief       NVRAM offsets for the Eistec Mulle IoT board
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#include "nvram.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mulle_nvram_address {
    /** @brief NVRAM magic number, used to identify an initialized FRAM device. */
    MULLE_NVRAM_MAGIC        = 0x0000,
    /** @brief Reboot counter */
    MULLE_NVRAM_BOOT_COUNT = 0x0004,
} mulle_nvram_address_t;

#define MULLE_NVRAM_MAGIC_EXPECTED (0x4c4c554dul) /* == "MULL" in ASCII */

extern nvram_t *mulle_nvram;

#ifdef __cplusplus
}
#endif

/** @} */
