/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Implementation of some tools
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>
#include <malloc.h>

#include "esp/common_macros.h"
#include "sdk/ets.h"
#include "tools.h"

extern void malloc_stats (void);
extern unsigned int get_free_heap_size (void);
extern uint8_t _eheap;  /* end of heap (defined in esp8266.riot-os.app.ld) */
extern uint8_t _sheap;  /* start of heap (defined in esp8266.riot-os.app.ld) */

void print_meminfo (void)
{
    struct mallinfo minfo = mallinfo();
    ets_printf("heap: %lu (free %lu) byte\n", &_eheap - &_sheap, get_free_heap_size());
    ets_printf("sysmem: %d (used %d, free %d)\n", minfo.arena, minfo.uordblks, minfo.fordblks);
}

void esp_hexdump (const void* addr, uint32_t num, char width, uint8_t per_line)
{
    uint32_t count = 0;
    uint32_t size;

    uint8_t*  addr8  = (uint8_t*) addr;
    uint16_t* addr16 = (uint16_t*)addr;
    uint32_t* addr32 = (uint32_t*)addr;
    uint64_t* addr64 = (uint64_t*)addr;

    switch (width) {
        case 'b': size = 1; break;
        case 'h': size = 2; break;
        case 'w': size = 4; break;
        case 'g': size = 8; break;
        default : size = 1; break;
    }

    while (count < num) {
        if (count % per_line == 0) {
            printf ("%08x: ", (uint32_t)((uint8_t*)addr+count*size));
        }
        switch (width) {
            case 'b': printf("%02" PRIx8 " ", addr8[count++]); break;
            case 'h': printf("%04" PRIx16 " ", addr16[count++]); break;
            case 'w': printf("%08" PRIx32 " ", addr32[count++]); break;
            case 'g': printf("%016" PRIx64 " ", addr64[count++]); break;

            default : printf("."); count++; break;
        }
        if (count % per_line == 0) {
            printf ("\n");
        }
    }
    printf ("\n");
}
