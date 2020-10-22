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

#define ENABLE_DEBUG    0

#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/param.h>

#include "esp_heap_caps.h"
#include "esp_heap_port.h"
#include "esp_heap_trace.h"
#include "priv/esp_heap_caps_priv.h"

#if IS_ACTIVE(ENABLE_DEBUG)
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif

#include "esp_log.h"

static const char *TAG = "heap_caps";
extern heap_region_t g_heap_region[HEAP_REGIONS_MAX];

/**
 * @brief Initialize regions of memory to the collection of heaps at runtime.
 */
void esp_heap_caps_init_region(heap_region_t *region, size_t max_num)
{
    uint8_t num;
    mem_blk_t *mem_start, *mem_end;

    for (num = 0; num < max_num; num++) {
        mem_start = (mem_blk_t *)HEAP_ALIGN(region[num].start_addr);
        mem_end = (mem_blk_t *)(HEAP_ALIGN(region[num].start_addr + region[num].total_size));
        if ((uint8_t *)mem_end != region[num].start_addr + region[num].total_size)
            mem_end = (mem_blk_t *)((uint8_t *)mem_end - sizeof(void *));
        mem_end = (mem_blk_t *)((uint8_t *)mem_end - MEM_HEAD_SIZE);

        ESP_EARLY_LOGV(TAG, "heap %d start from %p to %p total %d bytes, mem_blk from %p to %p total",
                            num, region[num].start_addr, region[num].start_addr + region[num].total_size,
                            region[num].total_size, mem_start, mem_end);

        mem_start->prev = NULL;
        mem_start->next = mem_end;

        mem_end->prev = mem_start;
        mem_end->next = NULL;

        g_heap_region[num].free_blk = mem_start;
        g_heap_region[num].min_free_bytes = g_heap_region[num].free_bytes = blk_link_size(mem_start);
    }
}

/**
 * @brief Get the total free size of all the regions that have the given capabilities
 */
size_t heap_caps_get_free_size(uint32_t caps)
{
    size_t bytes = 0;

    for (int i = 0; i < HEAP_REGIONS_MAX; i++)
        if (caps == (caps & g_heap_region[i].caps))
            bytes += g_heap_region[i].free_bytes;

    return bytes;
}

/**
 * @brief Get the total minimum free memory of all regions with the given capabilities
 */
size_t heap_caps_get_minimum_free_size(uint32_t caps)
{
    size_t bytes = 0;

    for (int i = 0; i < HEAP_REGIONS_MAX; i++)
        if (caps == (caps & g_heap_region[i].caps))
            bytes += g_heap_region[i].min_free_bytes;

    return bytes;
}

/**
 * @brief Allocate a chunk of memory which has the given capabilities
 */
void *_heap_caps_malloc(size_t size, uint32_t caps, const char *file, size_t line)
{
    mem_blk_t *mem_blk, *next_mem_blk;
    void *ret_mem = NULL;
    uint32_t num;
    uint32_t mem_blk_size;

    if (line == 0) {
        ESP_EARLY_LOGV(TAG, "caller func %p", file);
    } else {
        ESP_EARLY_LOGV(TAG, "caller file %s line %d", file, line);
    }

    for (num = 0; num < HEAP_REGIONS_MAX; num++) {
        bool trace;
        size_t head_size;

        if ((g_heap_region[num].caps & caps) != caps)
            continue;

        _heap_caps_lock(num);

        trace = heap_trace_is_on();

        mem_blk_size = ptr2memblk_size(size, trace);

        ESP_EARLY_LOGV(TAG, "malloc size is %d(%x) blk size is %d(%x) region is %d", size, size,
                            mem_blk_size, mem_blk_size, num);

        if (mem_blk_size > g_heap_region[num].free_bytes)
            goto next_region;

        mem_blk = (mem_blk_t *)g_heap_region[num].free_blk;

        ESP_EARLY_LOGV(TAG, "malloc start %p", mem_blk);

        while (mem_blk && !mem_blk_is_end(mem_blk) && (mem_blk_is_used(mem_blk) || blk_link_size(mem_blk) < mem_blk_size)) {
            ESP_EARLY_LOGV(TAG, "malloc mem_blk %p next %p used %x traced %x, size %d", mem_blk, mem_blk_next(mem_blk),
                                mem_blk_is_used(mem_blk), mem_blk_is_traced(mem_blk), blk_link_size(mem_blk));
            mem_blk = mem_blk_next(mem_blk);
        }

        ESP_EARLY_LOGV(TAG, "malloc end %p, end %d", mem_blk, mem_blk_is_end(mem_blk));

        if (!mem_blk || mem_blk_is_end(mem_blk))
            goto next_region;

        ret_mem = blk2ptr(mem_blk, trace);
        ESP_EARLY_LOGV(TAG, "ret_mem is %p", ret_mem);

        head_size = mem_blk_head_size(trace);

        if (blk_link_size(mem_blk) >= mem_blk_size + head_size + MEM_BLK_MIN)
            next_mem_blk = (mem_blk_t *)((uint8_t *)mem_blk + mem_blk_size);
        else 
            next_mem_blk = mem_blk_next(mem_blk);

        ESP_EARLY_LOGV(TAG, "next_mem_blk is %p", next_mem_blk);

        if (mem_blk_next(mem_blk) != next_mem_blk) {
            next_mem_blk->prev = next_mem_blk->next = NULL;

            mem_blk_set_prev(next_mem_blk, mem_blk);
            mem_blk_set_next(next_mem_blk, mem_blk_next(mem_blk));

            ESP_EARLY_LOGV(TAG, "mem_blk1 %p, mem_blk->prev %p(%p), mem_blk->next %p(%p)", mem_blk, mem_blk_prev(mem_blk),
                                mem_blk->prev, mem_blk_next(mem_blk), mem_blk->next);

            mem_blk_set_prev(mem_blk_next(mem_blk), next_mem_blk);
            mem_blk_set_next(mem_blk, next_mem_blk);
        }

        mem_blk_set_used(mem_blk);
        if (trace) {
            mem_blk_set_traced((mem2_blk_t *)mem_blk, file, line);
            ESP_EARLY_LOGV(TAG, "mem_blk1 %p set trace", mem_blk);
        }

        if (g_heap_region[num].free_blk == mem_blk) {
            mem_blk_t *free_blk = mem_blk;

            while (free_blk && !mem_blk_is_end(free_blk) && mem_blk_is_used(free_blk)) {
                free_blk = mem_blk_next(free_blk);
            }

            ESP_EARLY_LOGV(TAG, "reset free_blk from %p to %p", g_heap_region[num].free_blk, free_blk);
            g_heap_region[num].free_blk = free_blk;
        } else {
            ESP_EARLY_LOGV(TAG, "free_blk is %p", g_heap_region[num].free_blk);
        }

        mem_blk_size = blk_link_size(mem_blk);
        g_heap_region[num].free_bytes -= mem_blk_size;

        if (g_heap_region[num].min_free_bytes > g_heap_region[num].free_bytes)
            g_heap_region[num].min_free_bytes = g_heap_region[num].free_bytes;

        ESP_EARLY_LOGV(TAG, "mem_blk2 %p, mem_blk->prev %p(%p), mem_blk->next %p(%p)", mem_blk, mem_blk_prev(mem_blk),
                            mem_blk->prev, mem_blk_next(mem_blk), mem_blk->next);
        ESP_EARLY_LOGV(TAG, "next_mem_blk %p, next_mem_blk->prev %p(%p), next_mem_blk->next %p(%p)", next_mem_blk,
                            mem_blk_prev(next_mem_blk), next_mem_blk->prev, mem_blk_next(next_mem_blk), next_mem_blk->next);
        ESP_EARLY_LOGV(TAG, "last_mem_blk %p, last_mem_blk->prev %p(%p), last_mem_blk->next %p(%p)", mem_blk_next(next_mem_blk),
                            mem_blk_prev(mem_blk_next(next_mem_blk)), mem_blk_next(next_mem_blk)->prev, mem_blk_next(mem_blk_next(next_mem_blk)), mem_blk_next(next_mem_blk)->next);

next_region:
        _heap_caps_unlock(num);

        if (ret_mem)
            break;
    }

    ESP_EARLY_LOGV(TAG, "malloc return mem %p", ret_mem);

    return ret_mem;
}

/**
 * @brief Free memory previously allocated via heap_caps_(m/c/r/z)alloc().
 */
void _heap_caps_free(void *ptr, const char *file, size_t line)
{
    int num;
    mem_blk_t *mem_blk;
    mem_blk_t *tmp, *next, *prev, *last;

    if ((int)line == 0) {
        ESP_EARLY_LOGV(TAG, "caller func %p", file);
    } else {
        ESP_EARLY_LOGV(TAG, "caller file %s line %d", file, line);
    }

    if (!ptr) {
        ESP_EARLY_LOGE(TAG, "free(ptr=NULL)");
        if ((int)line == 0) {
            ESP_EARLY_LOGE(TAG, "caller func %p", file);
        } else {
            ESP_EARLY_LOGE(TAG, "caller file %s line %d", file, line);
        }
        return;
    }

    num = get_blk_region(ptr);

    if (num >= HEAP_REGIONS_MAX) {
        ESP_EARLY_LOGE(TAG, "free(ptr_region=NULL)");
        return;
    }

    mem_blk = ptr2blk(ptr, ptr_is_traced(ptr));
    if (!mem_blk_is_used(mem_blk)) {
        ESP_EARLY_LOGE(TAG, "%p already freed\n", ptr);
        return;
    }

    ESP_EARLY_LOGV(TAG, "Free(ptr=%p, mem_blk=%p, region=%d)", ptr, mem_blk, num);

    _heap_caps_lock(num);

    g_heap_region[num].free_bytes += blk_link_size(mem_blk);

    ESP_EARLY_LOGV(TAG, "ptr prev=%p next=%p", mem_blk_prev(mem_blk), mem_blk_next(mem_blk));
    ESP_EARLY_LOGV(TAG, "ptr1 prev->next=%p next->prev=%p", mem_blk_prev(mem_blk) ? mem_blk_next(mem_blk_prev(mem_blk)) : NULL,
                        mem_blk_prev(mem_blk_next(mem_blk)));

    mem_blk_set_unused(mem_blk);
    mem_blk_set_untraced((mem2_blk_t *)mem_blk);

    prev = mem_blk_prev(mem_blk);
    next = mem_blk_next(mem_blk);
    last = mem_blk_next(next);

    if (prev && !mem_blk_is_used(prev)) {
        mem_blk_set_next(prev, next);
        mem_blk_set_prev(next, prev);
        tmp = prev;
    } else
        tmp = mem_blk;

    if (last && !mem_blk_is_used(next)) {
        mem_blk_set_next(tmp, last);
        mem_blk_set_prev(last, tmp);
    }

    ESP_EARLY_LOGV(TAG, "ptr2 prev->next=%p next->prev=%p", mem_blk_prev(mem_blk) ? mem_blk_next(mem_blk_prev(mem_blk)) : NULL,
                        mem_blk_prev(mem_blk_next(mem_blk)));

    if ((uint8_t *)mem_blk < (uint8_t *)g_heap_region[num].free_blk) {
        ESP_EARLY_LOGV(TAG, "Free update free block from %p to %p", g_heap_region[num].free_blk, mem_blk);
        g_heap_region[num].free_blk = mem_blk;
    }

    _heap_caps_unlock(num);
}

/**
 * @brief Allocate a chunk of memory which has the given capabilities. The initialized value in the memory is set to zero.
 */
void *_heap_caps_calloc(size_t count, size_t size, uint32_t caps, const char *file, size_t line)
{
    void *p = _heap_caps_malloc(count * size, caps, file, line);
    if (p)
        memset(p, 0, count * size);

    return p;
}

/**
 * @brief Reallocate memory previously allocated via heap_caps_(m/c/r/z)alloc().
 */
void *_heap_caps_realloc(void *mem, size_t newsize, uint32_t caps, const char *file, size_t line)
{
    void *return_addr = (void *)__builtin_return_address(0);

    void *p = _heap_caps_malloc(newsize, caps, file, line);
    if (p && mem) {
        size_t mem_size = ptr_size(mem);
        size_t min = MIN(newsize, mem_size);

        memcpy(p, mem, min);
        _heap_caps_free(mem, (char *)return_addr, line);
    }

    return p;
}

/**
 * @brief Allocate a chunk of memory which has the given capabilities. The initialized value in the memory is set to zero.
 */
void *_heap_caps_zalloc(size_t size, uint32_t caps, const char *file, size_t line)
{
    void *p = _heap_caps_malloc(size, caps, file, line);
    if (p)
        memset(p, 0, size);

    return p;
}
