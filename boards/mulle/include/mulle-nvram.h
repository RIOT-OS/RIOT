/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef MULLE_NVRAM_H
#define MULLE_NVRAM_H

#include "nvram.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup     board_mulle
 * @{
 *
 * @file
 * @brief       NVRAM offsets for the Eistec Mulle IoT board
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

typedef enum mulle_nvram_address {
    /** @brief NVRAM magic number, used to identify an initialized FRAM device. */
    MULLE_NVRAM_MAGIC        = 0x0000,
    /** @brief Reboot counter */
    MULLE_NVRAM_BOOT_COUNT = 0x0004,
} mulle_nvram_address_t;

#define MULLE_NVRAM_MAGIC_EXPECTED (0x4c4c554dul) /* == "MULL" in ASCII */

extern nvram_t *mulle_nvram;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MULLE_NVRAM_H */
