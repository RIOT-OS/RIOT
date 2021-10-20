/*
 * Copyright (C) 2021 Michael Richardson <mcr@sandelman.ca>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the CFG-PAGE driver
 *
 * @author      Michael Richardson <mcr@sandelman.ca>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "board.h"
#include "cfg_page.h"

#include "native_internal.h"

int main(void)
{
    nanocbor_value_t valuereader;
    const uint8_t *strvalue;
    size_t strlen;
    puts("CFG-PAGE test application starting...");

    if(cfg_page_get_value(&cfgpage, 1, &valuereader) == 1 &&
       nanocbor_get_tstr(&valuereader, &strvalue, &strlen) == NANOCBOR_OK) {
        printf("key: 1 found value: %.*s\n", strlen, strvalue);
    }

    cfg_page_print(&cfgpage);

    real_exit(0);
}
