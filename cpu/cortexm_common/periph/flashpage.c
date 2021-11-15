/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @ingroup     drivers_periph_flashpage
 * @{
 * @file
 * @brief       common periph/flashpage functions
 *
 * @author  Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#include "periph/flashpage.h"

/**
 * @brief   Memory markers, defined in the linker script
 * @{
 */
extern uint32_t _end_fw;
extern uint32_t _erom;

unsigned flashpage_first_free(void)
{
    return flashpage_page(&_end_fw) + 1;
}

unsigned flashpage_last_free(void)
{
    return flashpage_page(&_erom) - 1;
}
