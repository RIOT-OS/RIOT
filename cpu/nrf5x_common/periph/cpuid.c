/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief       CPUID interface implementation
 *
 * The NRF52832 provides a 64-bit unique identifier that is unique for each
 * shipped unit.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    memcpy(id, (void *)NRF_FICR->DEVICEID, CPUID_LEN);
}
