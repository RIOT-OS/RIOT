/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "coverage.h"

static struct gcov_info *gcov_info_head;

/**
 * Is called by gcc-generated constructor code for each object file compiled
 * with -fprofile-arcs.
 */
void __gcov_init(struct gcov_info *info)
{
    info->next = gcov_info_head;
    gcov_info_head = info;
}

void __gcov_merge_add(gcov_type *counters, unsigned int n_counters)
{
    (void)counters;
    (void)n_counters;
    /* Unused. */
}

void __gcov_exit(void)
{
    /* Unused. */
}

/**
 * buff_write_u64 - Store 64 bit data on a buffer and return the size
 */

static inline void buff_write_u64(void *buffer, size_t *off, uint64_t v)
{
    memcpy((uint8_t*)buffer + *off, (uint8_t*)&v, sizeof(v));
    *off = *off + sizeof(uint64_t);
}

/**
 * buff_write_u32 - Store 32 bit data on a buffer and return the size
 */
static inline void buff_write_u32(void *buffer, size_t *off, uint32_t v)
{
    memcpy((uint8_t*)buffer + *off, (uint8_t*)&v, sizeof(v));
    *off = *off + sizeof(uint32_t);
}

size_t calculate_buff_size(struct gcov_info *info)
{
    uint32_t iter;
    uint32_t iter_1;
    uint32_t iter_2;

    /* Few fixed values at the start: magic number,
     * version, stamp, and checksum.
     */
#ifdef GCOV_12_FORMAT
    uint32_t size = sizeof(uint32_t) * 4;
#else
    uint32_t size = sizeof(uint32_t) * 3;
#endif

    for (iter = 0U; iter < info->n_functions; iter++) {
        /* space for TAG_FUNCTION and FUNCTION_LENGTH
         * ident
         * lineno_checksum
         * cfg_checksum
         */
        size += (sizeof(uint32_t) * 5);

        for (iter_1 = 0U; iter_1 < GCOV_COUNTERS; iter_1++) {
            if (!info->merge[iter_1]) {
                continue;
            }

            /*  for function counter and number of values  */
            size += (sizeof(uint32_t) * 2);

            for (iter_2 = 0U;
                    iter_2 < info->functions[iter]->ctrs->num;
                    iter_2++) {

                /* Iter for values which is uint64_t */
                size += (sizeof(uint64_t));
            }

        }
    }

    return size;
}

/**
 * populate_buffer - convert from gcov data set (info) to
 * .gcda file format.
 * This buffer will now have info similar to a regular gcda
 * format.
 */
size_t populate_buffer(uint8_t *buffer, struct gcov_info *info)
{
    struct gcov_fn_info *functions;
    struct gcov_ctr_info *counters_per_func;
    uint32_t iter_functions;
    uint32_t iter_counts;
    uint32_t iter_counter_values;
    size_t buffer_write_position = 0;

    /* File header. */
    buff_write_u32(buffer,
            &buffer_write_position,
            GCOV_DATA_MAGIC);

    buff_write_u32(buffer,
            &buffer_write_position,
            info->version);

    buff_write_u32(buffer,
            &buffer_write_position,
            info->stamp);

#ifdef GCOV_12_FORMAT
    buff_write_u32(buffer,
            &buffer_write_position,
            info->checksum);
#endif

    for (iter_functions = 0U;
            iter_functions < info->n_functions;
            iter_functions++) {

        functions = info->functions[iter_functions];

        buff_write_u32(buffer,
                &buffer_write_position,
                GCOV_TAG_FUNCTION);

        buff_write_u32(buffer,
                &buffer_write_position,
                GCOV_TAG_FUNCTION_LENGTH);

        buff_write_u32(buffer,
                &buffer_write_position,
                functions->ident);

        buff_write_u32(buffer,
                &buffer_write_position,
                functions->lineno_checksum);

        buff_write_u32(buffer,
                &buffer_write_position,
                functions->cfg_checksum);

        counters_per_func = functions->ctrs;

        for (iter_counts = 0U;
                iter_counts < GCOV_COUNTERS;
                iter_counts++) {

            if (!info->merge[iter_counts]) {
                continue;
            }

            buff_write_u32(buffer,
                    &buffer_write_position,
                    GCOV_TAG_FOR_COUNTER(iter_counts));

#ifdef GCOV_12_FORMAT
            /* GCOV 12 counts the length by bytes */
            buff_write_u32(buffer,
                    &buffer_write_position,
                    counters_per_func->num * 2U * 4);
#else
            buff_write_u32(buffer,
                    &buffer_write_position,
                    counters_per_func->num * 2U);
#endif

            for (iter_counter_values = 0U;
                    iter_counter_values < counters_per_func->num;
                    iter_counter_values++) {

                buff_write_u64(buffer,
                        &buffer_write_position,
                        counters_per_func->\
                        values[iter_counter_values]);
            }

            counters_per_func++;
        }
    }

    return buffer_write_position;
}

void dump_on_console(const char *filename, uint8_t *ptr, size_t len)
{
    uint32_t iter;

    printf("\n%c", FILE_START_INDICATOR);
    while (*filename != '\0') {
        printf("%c", *filename++);
    }
    printf("%c", GCOV_DUMP_SEPARATOR);

    /* Data dump */

    for (iter = 0U; iter < len; iter++) {
        printf(" %02x", (uint8_t)*ptr++);
    }
}

/**
 * Retrieves gcov coverage data and sends it over the given interface.
 */
void gcov_coverage_dump(void)
{
    uint8_t *buffer;
    size_t size;
    size_t written_size;
    struct gcov_info *gcov_list_first = gcov_info_head;
    struct gcov_info *gcov_list = gcov_info_head;

    printf("\nGCOV_COVERAGE_DUMP_START");
    while (gcov_list) {
        size = calculate_buff_size(gcov_list);

        buffer = malloc(size);
        if (!buffer) {
            printf("No Mem available to continue dump\n");
            goto coverage_dump_end;
        }

        written_size = populate_buffer(buffer, gcov_list);
        if (written_size != size) {
            printf("Write Error on buff\n");
            goto coverage_dump_end;
        }

        dump_on_console(gcov_list->filename, buffer, size);

        free(buffer);
        gcov_list = gcov_list->next;
        if (gcov_list_first == gcov_list) {
            goto coverage_dump_end;
        }
    }
coverage_dump_end:
    printf("\nGCOV_COVERAGE_DUMP_END\n");
    return;
}

/* Initialize the gcov by calling the required constructors */
void gcov_static_init(void)
{
    extern uintptr_t __init_array_start, __init_array_end;
    uintptr_t func_pointer_start = (uintptr_t) &__init_array_start;
    uintptr_t func_pointer_end = (uintptr_t) &__init_array_end;

    while (func_pointer_start < func_pointer_end) {
        void (**p)(void);
        /* get function pointer */
        p = (void (**)(void)) func_pointer_start;
        (*p)();
        func_pointer_start += sizeof(p);
    }
}
