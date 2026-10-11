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
#include <string.h>
#include "assert.h"

#include "rbpf.h"
#include "rbpf/instruction.h"
#include "rbpf/config.h"

extern int rbpf_engine_run(rbpf_application_t *rbpf, size_t func_idx, const void *ctx, int64_t *result);

int rbpf_application_run_ctx(rbpf_application_t *rbpf, void *ctx, size_t ctx_size, int64_t *result)
{
    return rbpf_application_run_ctx_idx_function(rbpf, 0, ctx, ctx_size, result);
}

int rbpf_application_run_ctx_name_function(rbpf_application_t *rbpf, const char *function, void *ctx, size_t ctx_size, int64_t *result)
{
    ssize_t idx = rbpf_application_function_by_name(rbpf, function);
    if (idx < 0) {
        return RBPF_UNKNOWN_FUNCTION;
    }
    return rbpf_application_run_ctx_idx_function(rbpf, idx, ctx, ctx_size, result);
}

int rbpf_application_run_ctx_idx_function(rbpf_application_t *rbpf, size_t func_idx, void *ctx, size_t ctx_size, int64_t *result)
{
    rbpf_memory_region_permission_init(&rbpf->arg_region_permission, ctx, ctx_size,
                            RBPF_MEM_REGION_READ | RBPF_MEM_REGION_WRITE);

    assert(rbpf->flags & RBPF_FLAG_SETUP_DONE);
    return rbpf_engine_run(rbpf, func_idx, ctx, result);
}

int rbpf_application_setup(rbpf_application_t *rbpf, uint8_t *stack,
                            const uint8_t *application, size_t application_len)
{
    int res = rbpf_verify_application_structure(application, application_len);
    if (res < 0) {
        return res;
    }
    rbpf->stack = stack;
    rbpf->application = application;
    rbpf->application_len = application_len;

    rbpf_memory_region_permission_init(&rbpf->stack_region_permission,
                            rbpf->stack,
                            RBPF_STACK_SIZE,
                            RBPF_MEM_REGION_READ | RBPF_MEM_REGION_WRITE);

    rbpf_memory_region_permission_init(&rbpf->data_region_permission, rbpf_application_data(rbpf),
                            rbpf_application_data_len(
                                rbpf), RBPF_MEM_REGION_READ | RBPF_MEM_REGION_WRITE);
    rbpf_memory_region_permission_init(&rbpf->rodata_region_permission, rbpf_application_rodata(rbpf),
                            rbpf_application_rodata_len(rbpf), RBPF_MEM_REGION_READ);

    /* Manually build the linked list of regions */
    rbpf->stack_region_permission.next = &rbpf->data_region_permission;
    rbpf->data_region_permission.next = &rbpf->rodata_region_permission;
    rbpf->rodata_region_permission.next = &rbpf->arg_region_permission;

    rbpf->flags |= RBPF_FLAG_SETUP_DONE;
    return 0;
}

void rbpf_add_memory_region_permission(rbpf_application_t *rbpf, rbpf_memory_region_permission_t *region)
{
    region->next = rbpf->arg_region_permission.next;
    rbpf->arg_region_permission.next = region;
}

ssize_t rbpf_application_function_by_name(const rbpf_application_t *rbpf, const char *name)
{
    for (size_t i = 0; i < rbpf_application_num_functions(rbpf); i++) {
        const rbpf_symbol_header_t *header = &(rbpf_symbol_table(rbpf)[i]);
        uint16_t name_offset = header->name_offset;
        const char *found_name = (const char *)rbpf_application_rodata(rbpf) + name_offset;
        if (strncmp(name, found_name, rbpf_application_rodata_len(rbpf) - name_offset) == 0) {
            return i;
        }
    }
    return -1;

}

ssize_t rbpf_application_get_function_name(const rbpf_application_t *rbpf, size_t idx, char *name, size_t name_len)
{
    if (idx > rbpf_application_num_functions(rbpf)) {
        return -1;
    }

    const rbpf_symbol_header_t *header = &(rbpf_symbol_table(rbpf)[idx]);

    uint16_t name_offset = header->name_offset;

    const char *found_name = (const char *)rbpf_application_rodata(rbpf) + name_offset;


    size_t found_name_len = strnlen(found_name, rbpf_application_rodata_len(rbpf) - name_offset);

    if (found_name_len >= name_len) {
        return -2;
    }

    strncpy(name, found_name, name_len);
    return found_name_len;
}

const bpf_instruction_t *rbpf_application_text_at_function(const rbpf_application_t *rbpf, size_t func_idx)
{
    if (func_idx > rbpf_application_num_functions(rbpf)) {
        return NULL;
    }

    const rbpf_symbol_header_t *header = &(rbpf_symbol_table(rbpf)[func_idx]);

    size_t offset = header->location_offset;
    if (offset > rbpf_application_text_len(rbpf)) {
        return NULL;
    }

    const uint8_t *instruction_start = (const uint8_t*)rbpf_application_text(rbpf) + offset;

    return (const bpf_instruction_t*)instruction_start;
}
