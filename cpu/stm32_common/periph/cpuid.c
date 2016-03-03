/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 * Copyright (C) 2015 James Hollister
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Implementation of the CPUID driver interface
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      James Hollister <jhollisterjr@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"

extern uint32_t _cpuid_address;

void cpuid_get(void *id)
{
    memcpy(id, &_cpuid_address, CPUID_LEN);
}
