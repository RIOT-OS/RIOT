/*
 * Copyright(C) 2017 Francois Berder <fberder@outlook.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include <stdint.h>
#include <string.h>
#include "board.h"
#include "periph/cpuid.h"

 void cpuid_get(void *id)
 {
    memcpy(id, (uint32_t*)&DEVID, CPUID_LEN);
 }
