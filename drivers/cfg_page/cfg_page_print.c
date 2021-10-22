/*
 * Copyright (C) 2021  Michael Richardson <mcr@sandelman.ca>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cfg_page
 * @brief       configuration data flash page
 * @{
 *
 * @file
 * @brief       configuration page debug routines
 *
 * @author      Michael Ricahrdson <mcr@sandelman.ca>
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "nanocbor/nanocbor.h"
#include "checksum/crc16_ccitt.h"
#include "cfg_page.h"
#include "od.h"

#define ENABLE_DEBUG 1
#include "debug.h"

/* for MTD_1 */
#include "board.h"

int cfg_page_print(cfg_page_desc_t *cpd)
{
    static unsigned char cfg_page_temp[MTD_SECTOR_SIZE];
    nanocbor_value_t reader;
    nanocbor_value_t values;

    if(cfg_page_init_reader(cpd, cfg_page_temp, sizeof(cfg_page_temp), &reader) < 0) {
        return -1;
    }

    if(nanocbor_get_type(&reader) != NANOCBOR_TYPE_MAP ||
       nanocbor_enter_map(&reader, &values) != NANOCBOR_OK) {
        return -2;
    }

    //printf("buffer is at: %p\n", cfg_page_temp);

    while(!nanocbor_at_end(&values)) {
        uint32_t key;
        unsigned int type;
        const uint8_t *str = NULL;
        size_t         len;
        if(nanocbor_get_uint32(&values, &key) < 0) {
            DEBUG("non interger key value found: %d", nanocbor_get_type(&values));
            return -3;
        }

        type = nanocbor_get_type(&values);
        printf("key: %02u[type=%02u] where=%p:%d ", key, type, values.cur,
               values.cur-cfg_page_temp);
        switch(type) {
        case NANOCBOR_TYPE_BSTR:
            if(nanocbor_get_bstr(&values, &str, &len) == NANOCBOR_OK) {
                od_hex_dump_ext(str, len, 16, 0);
            }
            break;

        case NANOCBOR_TYPE_TSTR:
            if(nanocbor_get_tstr(&values, &str, &len) == NANOCBOR_OK) {
                /* XXX print as bounded string? */
                //od_hex_dump_ext(str, len, 16, 0);
                printf("  value[%04u]: %.*s\n", len, len, str);
            }
            break;

        default:
            nanocbor_skip(&values);
            printf("\n");
        }

    }

    return 0;
}

