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
#include <stdalign.h>
#include <stdio.h>
#include <stdint.h>
#include "rbpf.h"
#include "rbpf/shared.h"
#include "embUnit.h"
#include "xtimer.h"

#include "blob/rbpf/fletcher32_rbpf.bin.h"

static const alignas(uint16_t) unsigned char wrap_around_data[] =
        "AD3Awn4kb6FtcsyE0RU25U7f55Yncn3LP3oEx9Gl4qr7iDW7I8L6Pbw9jNnh0sE4DmCKuc"
        "d1J8I34vn31W924y5GMS74vUrZQc08805aj4Tf66HgL1cO94os10V2s2GDQ825yNh9Yuq3"
        "QHcA60xl31rdA7WskVtCXI7ruH1A4qaR6Uk454hm401lLmv2cGWt5KTJmr93d3JsGaRRPs"
        "4HqYi4mFGowo8fWv48IcA3N89Z99nf0A0H2R6P0uI4Tir682Of3Rk78DUB2dIGQRRpdqVT"
        "tLhgfET2gUGU65V3edSwADMqRttI9JPVz8JS37g5QZj4Ax56rU1u0m0K8YUs57UYG5645n"
        "byNy4yqxu7";

static uint8_t _rbpf_stack[512];

typedef struct {
    __bpf_shared_ptr(const uint16_t *, data);
    uint32_t words;
} fletcher32_ctx_t;

static void tests_bpf_run1(void)
{
    fletcher32_ctx_t ctx = {
        .data = (const uint16_t*)(uintptr_t)wrap_around_data,
        .words = sizeof(wrap_around_data)/2,
    };
    rbpf_application_t rbpf;
    rbpf_mem_region_t region;
    rbpf_application_setup(&rbpf, _rbpf_stack,
        (void*)fletcher32_rbpf_bin, sizeof(fletcher32_rbpf_bin));

    rbpf_memory_region_init(&region,
                   (void*)wrap_around_data, sizeof(wrap_around_data), RBPF_MEM_REGION_READ);
    rbpf_add_region(&rbpf, &region);
    int64_t result = 0;
    int res = 0;
    for (unsigned i = 0; i < 1000; i++) {
        res = rbpf_application_run_ctx(&rbpf, &ctx, sizeof(ctx), &result);
    }

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(0x5bac8c3d, (uint32_t)result);
}

Test *tests_bpf(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_bpf_run1),
    };

    EMB_UNIT_TESTCALLER(bpf_tests, NULL, NULL, fixtures);
    return (Test*)&bpf_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_bpf());
    TESTS_END();

    return 0;
}
