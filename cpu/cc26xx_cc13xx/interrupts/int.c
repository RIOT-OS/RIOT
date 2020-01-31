/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx MCU interrupt handler configuration
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cc26xx_cc13xx_int.h"

#include <stddef.h>

cc26xx_cc13xx_ints_t cc26xx_cc13xx_ints;

void isr_rfc_cpe0(void)
{
    if (cc26xx_cc13xx_ints.isr_rfc_cpe0 != NULL) {
        cc26xx_cc13xx_ints.isr_rfc_cpe0();
    }
}

void isr_rfc_cpe1(void)
{
    if (cc26xx_cc13xx_ints.isr_rfc_cpe1 != NULL) {
        cc26xx_cc13xx_ints.isr_rfc_cpe1();
    }
}
