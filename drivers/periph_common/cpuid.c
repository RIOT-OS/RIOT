/*
 * SPDX-FileCopyrightText: 2017 Eistec AB
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 James Hollister
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_periph_cpuid
 * @{
 *
 * @file
 * @brief       Generic implementation of the CPUID driver interface
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      James Hollister <jhollisterjr@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"

typedef struct {
    uint8_t id[CPUID_LEN];
} cpuid_t;

#ifdef CPUID_ADDR
void cpuid_get(void *id)
{
    cpuid_t *dest = id;
    const volatile cpuid_t *src = (const void *)CPUID_ADDR;
    *dest = *src;
}
#endif
