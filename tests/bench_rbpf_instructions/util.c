/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests bpf virtual machine
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "embUnit.h"
#include "timex.h"
#include "ztimer.h"
#include "rbpf/shared.h"
#include "rbpf/instruction.h"
#include "unaligned.h"
#include "util.h"

void fill_instruction(const bpf_instruction_t *instr, test_application_t *test_app)
{
    test_app->header.data_len = 0;
    test_app->header.rodata_len = 68;
    test_app->header.text_len = sizeof(uint64_t) * (NUM_INSTRUCTIONS + 1);

    for (size_t i = 0; i < NUM_INSTRUCTIONS; i++) {
        memcpy(&test_app->text[i], instr, sizeof(bpf_instruction_t));
    }
    static const bpf_instruction_t return_instr = {
        .opcode = BPF_INSTRUCTION_CLS_BRANCH | BPF_INSTRUCTION_BRANCH_EXIT
    };
    memcpy(&test_app->text[NUM_INSTRUCTIONS], &return_instr, sizeof(bpf_instruction_t));
}
