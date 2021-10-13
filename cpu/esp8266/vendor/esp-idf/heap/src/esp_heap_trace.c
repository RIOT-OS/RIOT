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

#include <string.h>

#include "esp_heap_caps.h"
#include "esp_heap_port.h"
#include "esp_heap_trace.h"
#include "priv/esp_heap_caps_priv.h"

//#define CONFIG_TRACE_ALL
//#define CONFIG_TRACE_MEM_LINK 1
//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "esp_log.h"

#ifdef CONFIG_TRACE_ALL
#define HEAP_INFO_STATE " is %s"
#define HEAP_INFO_STATE_PARAM(_p) ,mem_blk_is_used(_p)?"used":"freed"
#else
#define HEAP_INFO_STATE ""
#define HEAP_INFO_STATE_PARAM(_p)
#endif

#ifdef CONFIG_TRACE_MEM_LINK
#define HEAP_INFO "p %p, prev %p(%p) next %p(%p) size %d"HEAP_INFO_STATE
#define HEAP_INFO_PARAM(_p) (_p),mem_blk_prev(_p),(_p)->prev,mem_blk_next(_p),(_p)->next,blk_link_size(_p)HEAP_INFO_STATE_PARAM(_p)
#else
#define HEAP_INFO "mem @%p size %d"HEAP_INFO_STATE
#define HEAP_INFO_PARAM(_p) (_p),blk_link_size(_p)HEAP_INFO_STATE_PARAM(_p)
#endif

static const char *TAG = "heap_trace";
static int s_heap_trace_mode = HEAP_TRACE_NONE;
extern heap_region_t g_heap_region[HEAP_REGIONS_MAX];

/**
 * @brief Empty function just for passing compiling some place.
 */
esp_err_t heap_trace_init_standalone(heap_trace_record_t *record_buffer, size_t num_records)
{
    return ESP_OK;
}

/**
 * @brief Check if heap trace is on
 */
int heap_trace_is_on(void)
{
    return s_heap_trace_mode == HEAP_TRACE_LEAKS;
}

/**
 * @brief Start heap tracing. All heap allocations will be traced, until heap_trace_stop() is called.
 */
esp_err_t heap_trace_start(heap_trace_mode_t mode)
{
    s_heap_trace_mode = mode;

    return ESP_OK;
}

/**
 * @brief Stop heap tracing.
 */
esp_err_t heap_trace_stop(void)
{
    s_heap_trace_mode = HEAP_TRACE_NONE;

    return ESP_OK;
}

/**
 * @brief Resume heap tracing which was previously stopped.
 */
esp_err_t heap_trace_resume(void)
{
    s_heap_trace_mode = HEAP_TRACE_LEAKS;

    return ESP_OK;
}

/**
 * @brief Dump heap trace record data to stdout
 */
void heap_trace_dump(void)
{
    uint8_t num;
    mem_blk_t *mem_start, *mem_end, *p;

    for (num = 0; num < HEAP_REGIONS_MAX; num++) {
        mem_start = (mem_blk_t *)HEAP_ALIGN(g_heap_region[num].start_addr);
        mem_end = (mem_blk_t *)(HEAP_ALIGN(g_heap_region[num].start_addr + g_heap_region[num].total_size));
        if ((uint8_t *)mem_end != g_heap_region[num].start_addr + g_heap_region[num].total_size)
            mem_end = (mem_blk_t *)((uint8_t *)mem_end - sizeof(void *));
        mem_end = (mem_blk_t *)((uint8_t *)mem_end - MEM_HEAD_SIZE);

        _heap_caps_lock(num);

        ESP_EARLY_LOGI(TAG, "\r\n\r\n");
        ESP_EARLY_LOGD(TAG, "start %p end %p", mem_start, mem_end);
        ESP_EARLY_LOGD(TAG, "free blk %p", g_heap_region[num].free_blk);
        ESP_EARLY_LOGD(TAG, "size %d mini size %d", g_heap_region[num].free_bytes, g_heap_region[num].min_free_bytes);

        p = mem_start;
        while (p != mem_end) {
            if (mem_blk_is_used(p) && mem_blk_is_traced(p)) {
                mem2_blk_t *mem2_blk = (mem2_blk_t *)p;
                size_t line = mem2_blk_line(mem2_blk);

                if (!line) {
                    ESP_EARLY_LOGI(TAG, HEAP_INFO " caller func %p", HEAP_INFO_PARAM(p), mem2_blk->file);
                } else {
                    const char *file = rindex(mem2_blk->file, '/');
                    if (file)
                        file++;
                    else
                        file = mem2_blk->file;

                    ESP_EARLY_LOGI(TAG, HEAP_INFO " caller file %s line %d", HEAP_INFO_PARAM(p), file, line);
                }
            }
#ifdef CONFIG_TRACE_ALL
            else {
                ESP_EARLY_LOGI(TAG, HEAP_INFO, HEAP_INFO_PARAM(p));
            }
#endif
            p = mem_blk_next(p);

            _heap_caps_feed_wdt(g_heap_region[num].caps & caps);
        }

        _heap_caps_unlock(num);
    }
}
