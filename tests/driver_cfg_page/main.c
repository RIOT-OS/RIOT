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
    size_t len;
    int i;
    puts("CFG-PAGE test application starting...");

    if(cfg_page_get_value(&cfgpage, 1, &valuereader) == 1 &&
       nanocbor_get_tstr(&valuereader, &strvalue, &len) == NANOCBOR_OK) {
        printf("key: 1 found value: %.*s\n", len, strvalue);
    }

    cfg_page_print(&cfgpage);

    for(i=0; i<256; i++) {
        printf("writing iteration %d\n", i);
        uint8_t buf2[16];
        snprintf((char *)buf2, 16, "bob%04x", i);
        if(cfg_page_set_str_value(&cfgpage, 1, buf2, strlen((const char *)buf2)) != 0) {
            printf("set key 1 failed\n");
            break;
        }
        snprintf((char *)buf2, 16, "frank%04x", i);
        if(cfg_page_set_str_value(&cfgpage, 37, buf2, strlen((const char *)buf2)) != 0) {
            printf("set key 2 failed\n");
            break;
        }
        snprintf((char *)buf2, 16, "george%04x", i);
        if(cfg_page_set_str_value(&cfgpage, 65537, buf2, strlen((const char *)buf2)) != 0) {
            printf("set key 3 failed\n");
            break;
        }
    }

    cfg_page_print(&cfgpage);

    real_exit(0);
}
