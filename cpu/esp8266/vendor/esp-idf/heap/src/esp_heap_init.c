// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_heap_caps.h"

heap_region_t g_heap_region[HEAP_REGIONS_MAX];

/**
 * @brief Initialize the capability-aware heap allocator.
 */
void heap_caps_init(void)
{
    extern char _heap_start;

#ifndef CONFIG_SOC_FULL_ICACHE
    extern char _lit4_end;

    g_heap_region[0].start_addr = (uint8_t *)&_lit4_end;
    g_heap_region[0].total_size = ((size_t)(0x4010C000 - (uint32_t)&_lit4_end));
    g_heap_region[0].caps = MALLOC_CAP_32BIT;
#endif

    g_heap_region[HEAP_REGIONS_MAX - 1].start_addr = (uint8_t *)&_heap_start;
    g_heap_region[HEAP_REGIONS_MAX - 1].total_size = ((size_t)(0x40000000 - (uint32_t)&_heap_start));
    g_heap_region[HEAP_REGIONS_MAX - 1].caps = MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_DMA;

    esp_heap_caps_init_region(g_heap_region, HEAP_REGIONS_MAX);
}
