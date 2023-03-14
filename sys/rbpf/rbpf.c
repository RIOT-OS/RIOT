/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdbool.h>
#include "assert.h"

#include "rbpf.h"
#include "rbpf/instruction.h"
#include "rbpf/config.h"

extern int rbpf_engine_run(rbpf_application_t *rbpf, const void *ctx, int64_t *result);

int rbpf_application_run_ctx(rbpf_application_t *rbpf, void *ctx, size_t ctx_len, int64_t *result)
{
    rbpf_memory_region_init(&rbpf->arg_region, ctx, ctx_len,
                            RBPF_MEM_REGION_READ | RBPF_MEM_REGION_WRITE);

    assert(rbpf->flags & RBPF_FLAG_SETUP_DONE);
    return rbpf_engine_run(rbpf, ctx, result);
}

void rbpf_application_setup(rbpf_application_t *rbpf, uint8_t *stack,
                            const rbpf_application_t *application, size_t application_len)
{
    rbpf->stack = stack;
    rbpf->application = application;
    rbpf->application_len = application_len;

    rbpf_memory_region_init(&rbpf->stack_region,
                            rbpf->stack,
                            RBPF_STACK_SIZE,
                            RBPF_MEM_REGION_READ | RBPF_MEM_REGION_WRITE);

    rbpf_memory_region_init(&rbpf->data_region, rbpf_application_data(rbpf),
                            rbpf_application_data_len(
                                rbpf), RBPF_MEM_REGION_READ | RBPF_MEM_REGION_WRITE);
    rbpf_memory_region_init(&rbpf->rodata_region, rbpf_application_rodata(rbpf),
                            rbpf_application_rodata_len(rbpf), RBPF_MEM_REGION_READ);

    /* Manually build the linked list of regions */
    rbpf->stack_region.next = &rbpf->data_region;
    rbpf->data_region.next = &rbpf->rodata_region;
    rbpf->rodata_region.next = &rbpf->arg_region;

    rbpf->flags |= RBPF_FLAG_SETUP_DONE;
}

void rbpf_add_region(rbpf_application_t *rbpf, rbpf_mem_region_t *region)
{
    region->next = rbpf->arg_region.next;
    rbpf->arg_region.next = region;
}
