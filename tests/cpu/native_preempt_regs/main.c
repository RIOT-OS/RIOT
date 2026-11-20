/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests that native preserves the state of a thread that is
 *              preempted by an interrupt
 *
 * The main thread puts known values into the SSE registers (and on x86_64
 * into the red zone below the stack pointer), busy-waits and checks that the
 * values are still there. Meanwhile, a timer periodically wakes up a higher
 * priority thread, which preempts the main thread and overwrites the SSE
 * registers.
 *
 * The C library uses the SSE registers e.g. in `memcmp()`, so if they get
 * corrupted, any code may fail sporadically.
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "ztimer.h"

#if !defined(__x86_64__) && !defined(__i386__)
#  error "This test is only implemented for x86 hosts"
#endif

#define TEST_DURATION_MS    (2000U)
#define PREEMPT_PERIOD_US   (100U)
#define BUSY_LOOPS          (20000UL)
#define RED_ZONE_PATTERN    (0x0123456789abcdefULL)
#define MEMCMP_SIZE         (4096U)

static char _preempter_stack[THREAD_STACKSIZE_DEFAULT];
static uint8_t _memcmp_a[MEMCMP_SIZE];
static uint8_t _memcmp_b[MEMCMP_SIZE];
static volatile unsigned _preemptions;

static void *_preempter(void *arg)
{
    (void)arg;
    while (1) {
        ztimer_sleep(ZTIMER_USEC, PREEMPT_PERIOD_US);
        /* set all bits of xmm0 - xmm7 */
        __asm__ volatile (
            "pcmpeqd %%xmm0, %%xmm0\n"
            "pcmpeqd %%xmm1, %%xmm1\n"
            "pcmpeqd %%xmm2, %%xmm2\n"
            "pcmpeqd %%xmm3, %%xmm3\n"
            "pcmpeqd %%xmm4, %%xmm4\n"
            "pcmpeqd %%xmm5, %%xmm5\n"
            "pcmpeqd %%xmm6, %%xmm6\n"
            "pcmpeqd %%xmm7, %%xmm7\n"
            ::: "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7");
        _preemptions++;
    }
    return NULL;
}

/* Copies @p in to @p out through xmm0 - xmm7 with a busy-wait in between */
static void _copy_via_xmm(const uint8_t in[8][16], uint8_t out[8][16])
{
    unsigned long loops = BUSY_LOOPS;

    __asm__ volatile (
        "movdqu   0(%[in]), %%xmm0\n"
        "movdqu  16(%[in]), %%xmm1\n"
        "movdqu  32(%[in]), %%xmm2\n"
        "movdqu  48(%[in]), %%xmm3\n"
        "movdqu  64(%[in]), %%xmm4\n"
        "movdqu  80(%[in]), %%xmm5\n"
        "movdqu  96(%[in]), %%xmm6\n"
        "movdqu 112(%[in]), %%xmm7\n"
        "1: dec %[loops]\n"
        "jnz 1b\n"
        "movdqu %%xmm0,   0(%[out])\n"
        "movdqu %%xmm1,  16(%[out])\n"
        "movdqu %%xmm2,  32(%[out])\n"
        "movdqu %%xmm3,  48(%[out])\n"
        "movdqu %%xmm4,  64(%[out])\n"
        "movdqu %%xmm5,  80(%[out])\n"
        "movdqu %%xmm6,  96(%[out])\n"
        "movdqu %%xmm7, 112(%[out])\n"
        : [loops] "+r" (loops)
        : [in] "r" (in), [out] "r" (out)
        : "memory", "cc",
          "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7");
}

/* Same check as above, but with real C library code: Depending on the C
 * library and the CPU, memcmp() uses SSE/AVX registers. The buffers only
 * differ in the last byte, so memcmp() has to compare all of them. */
static bool _memcmp_correct(void)
{
    _memcmp_b[MEMCMP_SIZE - 1] = _memcmp_a[MEMCMP_SIZE - 1];
    if (memcmp(_memcmp_a, _memcmp_b, MEMCMP_SIZE) != 0) {
        return false;
    }

    _memcmp_b[MEMCMP_SIZE - 1]++;
    if (memcmp(_memcmp_a, _memcmp_b, MEMCMP_SIZE) == 0) {
        return false;
    }

    return true;
}

#ifdef __x86_64__
/* Writes a value just below the stack pointer, busy-waits and reads it back.
 * Leaf functions are allowed to use these 128 bytes (the "red zone") without
 * adjusting the stack pointer. */
static uint64_t _copy_via_red_zone(uint64_t value)
{
    unsigned long loops = BUSY_LOOPS;
    uint64_t result;

    __asm__ volatile (
        "mov %[value], -8(%%rsp)\n"
        "1: dec %[loops]\n"
        "jnz 1b\n"
        "mov -8(%%rsp), %[result]\n"
        : [loops] "+r" (loops), [result] "=r" (result)
        : [value] "r" (value)
        : "memory", "cc");

    return result;
}
#endif

int main(void)
{
    uint8_t in[8][16];
    unsigned runs = 0;
    unsigned xmm_errors = 0;
    unsigned red_zone_errors = 0;
    unsigned memcmp_errors = 0;

    for (unsigned i = 0; i < sizeof(in); i++) {
        ((uint8_t *)in)[i] = i;
    }
    for (unsigned i = 0; i < MEMCMP_SIZE; i++) {
        _memcmp_a[i] = i;
        _memcmp_b[i] = i;
    }

    thread_create(_preempter_stack, sizeof(_preempter_stack),
                  THREAD_PRIORITY_MAIN - 1, 0, _preempter, NULL, "preempter");

    uint32_t end = ztimer_now(ZTIMER_MSEC) + TEST_DURATION_MS;
    while ((int32_t)(end - ztimer_now(ZTIMER_MSEC)) > 0) {
        uint8_t out[8][16] = { 0 };

        _copy_via_xmm(in, out);
        if (memcmp(in, out, sizeof(in)) != 0) {
            xmm_errors++;
        }
#ifdef __x86_64__
        if (_copy_via_red_zone(RED_ZONE_PATTERN) != RED_ZONE_PATTERN) {
            red_zone_errors++;
        }
#endif
        if (!_memcmp_correct()) {
            memcmp_errors++;
        }
        runs++;
    }

    printf("preemptions: %u\n", _preemptions);
    printf("SSE registers corrupted: %u/%u\n", xmm_errors, runs);
#ifdef __x86_64__
    printf("red zone corrupted: %u/%u\n", red_zone_errors, runs);
#endif
    printf("memcmp() wrong: %u/%u\n", memcmp_errors, runs);

    if ((_preemptions > 0) && (xmm_errors == 0) && (red_zone_errors == 0) &&
        (memcmp_errors == 0)) {
        puts("SUCCESS");
    }
    else {
        puts("FAILURE");
    }
    return 0;
}
