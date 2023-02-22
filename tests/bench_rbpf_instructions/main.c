/*
 * Copyright (C) 2023 Freie Universität Berlin
 * Copyright (C) 2023 Inria
 * Copyright (C) 2023 Koen Zandberg <koen@bergzand.net>
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
 * @brief       Benchmark application for individual rbpf virtual machine instructions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include "timex.h"
#include "ztimer.h"
#include "rbpf.h"
#include "rbpf/shared.h"
#include "rbpf/instruction.h"
#include "unaligned.h"
#include "util.h"

/* ibpf defines this within the struct */
static uint8_t _bpf_stack[512];

static const test_content_t tests[] = {
    {
        .instruction = {
            .opcode = 0x87,
        },
        .name = "ALU neg64",
    },
    {
        .instruction = {
            .opcode = 0x84,
        },
        .name = "ALU neg32",
    },
    {
        .instruction = {
            .opcode = 0x07,
        },
        .name = "ALU Add",
    },
    {
        .instruction = {
            .opcode = 0x0c,
        },
        .name = "ALU Add32",
    },
    {
        .instruction = {
            .opcode = 0x0f,
        },
        .name = "ALU Add imm",
    },
    {
        .instruction = {
            .opcode = 0x04,
        },
        .name = "ALU Add32 imm",
    },
    {
        .instruction = {
            .opcode = 0x2f,
            .dst = 0,
            .immediate = 45,
        },
        .name = "ALU mul imm",
    },
    {
        .instruction = {
            .opcode = 0x24,
            .dst = 0,
            .immediate = 45,
        },
        .name = "ALU mul32 imm",
    },
    {
        .instruction = {
            .opcode = 0x77,
            .dst = 0,
            .immediate = 5,
        },
        .name = "ALU rsh imm",
    },
    {
        .instruction = {
            .opcode = 0x37,
            .dst = 0,
            .immediate = 5,
        },
        .name = "ALU div imm",
    },
    {
        .instruction = {
            .opcode = 0x34,
            .dst = 0,
            .immediate = 5,
        },
        .name = "ALU div32 imm",
    },
    {
        .instruction = {
            .opcode = 0x79,
            .dst = 0,
            .src = 10,
            .offset = -16,
        },
        .name = "MEM ldxdw",
    },
    {
        .instruction = {
            .opcode = 0x7a,
            .dst = 10,
            .offset = -16,
            .immediate = 45,
        },
        .name = "MEM stdw",
    },
    {
        .instruction = {
            .opcode = 0x7b,
            .dst = 10,
            .src = 0,
            .offset = -16,
        },
        .name = "MEM stxdw",
    },
    {
        .instruction = {
            .opcode = 0x05,
            .offset = 0,
        },
        .name = "Branch always",
    },
    {
        .instruction = {
            .opcode = 0x1d,
            .offset = 0,
            .dst = 10,
            .src = 10,
        },
        .name = "Branch eq (jump)",
    },
    {
        .instruction = {
            .opcode = 0x1d,
            .offset = 0,
            .dst = 0,
            .src = 10,
        },
        .name = "Branch eq (cont)",
    },
};

static test_application_t test_app;

int main(void)
{
#if CSV_OUT
    puts("duration,code,usperinst,instrpersec");
#else
    printf("| %-16s | %-8s | %-6s | %-6s | %-16s |\n",
           "Test", "duration", "code", "us/instr", "instr per sec");
#endif
    for (size_t test_idx = 0; test_idx < ARRAY_SIZE(tests); test_idx++) {
        rbpf_application_t rbpf = { 0 };
        rbpf_application_setup(&rbpf, _bpf_stack, (const rbpf_application_t*)&test_app, sizeof(test_app));
        rbpf.flags |= RBPF_FLAG_PREFLIGHT_DONE;

        int64_t res = 0;

        /* prepare instructions */
        fill_instruction(&tests[test_idx].instruction, &test_app);

        uint32_t begin = ztimer_now(ZTIMER_USEC); // unsigned long long -> uint64_t
        int result = rbpf_application_run_ctx(&rbpf, NULL, 0, &res);
        uint32_t end = ztimer_now(ZTIMER_USEC);
        float duration = (float)(end-begin);
        float us_per_op = duration/NUM_INSTRUCTIONS;
        float kops_per_sec = (float)(NUM_INSTRUCTIONS*US_PER_MS) / duration;
#if CSV_OUT
        printf("%f,%d,%f,%f\n",
                duration/US_PER_MS, (signed)result, us_per_op, kops_per_sec);
#else
        printf("| %-16s | %2.4fms | %6d | %2.4fus | %7.2f kops/sec\n",
                tests[test_idx].name,
                duration/US_PER_MS, (signed)result, us_per_op, kops_per_sec);
#endif

    }

    return 0;
}
