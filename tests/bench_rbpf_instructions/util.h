/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "rbpf.h"
#include "rbpf/instruction.h"

typedef struct {
    bpf_instruction_t instruction;
    const char *name;
} test_content_t;

typedef struct {
    rbpf_header_t header;
    uint8_t rodata[68];
    uint64_t text[NUM_INSTRUCTIONS + 1];
} test_application_t;

void fill_instruction(const bpf_instruction_t *instr, test_application_t *test_app);

#ifdef __cplusplus
}
#endif
#endif /* TEST_UTIL_H */
/** @} */
