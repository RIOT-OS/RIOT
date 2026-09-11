/*
 * SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       MSI-like mailbox: a thread posts, the ISR prints
 *
 * This is not PCI Express MSI. A thread writes a generic mailbox slot. The
 * Cortex-M backend pends HASH_RNG_IRQn, and `isr_hash_rng()` drains the
 * mailbox and prints there.
 *
 * @author      Ajit Upadhyay <u.ajiit@gmail.com>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "cpu.h"
#include "msi.h"
#include "thread.h"
#include "ztimer.h"

#ifndef MSI_TEST_IRQN
#  define MSI_TEST_IRQN     HASH_RNG_IRQn
#endif

#define POST_COUNT          (4U)
#define POST_GAP_MSEC       (200U)

static char _poster_stack[THREAD_STACKSIZE_SMALL];
static volatile unsigned _isr_hits;

static void _on_msi(unsigned vec, uint32_t event, uint32_t data, void *arg)
{
    const char *name = arg;

    printf("[ISR] %s vec=%u event=%" PRIu32 " data=0x%" PRIx32 "\n",
           name, vec, event, data);
    _isr_hits++;
}

void isr_hash_rng(void)
{
    msi_isr();
}

static void *_poster(void *arg)
{
    (void)arg;

    for (unsigned i = 1; i <= POST_COUNT; i++) {
        uint32_t event = i;
        uint32_t data = 0xA000u + i;
        unsigned vec = (i & 1u);

        printf("[thread] post vec=%u event=%" PRIu32 " data=0x%" PRIx32 "\n",
               vec, event, data);

        int res = msi_post(vec, event, data);
        if (res < 0) {
            printf("[thread] post failed: %d\n", res);
        }

        ztimer_sleep(ZTIMER_MSEC, POST_GAP_MSEC);
    }

    return NULL;
}

int main(void)
{
    puts("MSI-like mailbox test");
    puts("thread writes mailbox + doorbell, ISR prints the payload");

    if (msi_init(MSI_TEST_IRQN) < 0) {
        puts("msi_init() failed");
        return 1;
    }

    if ((msi_register(0, _on_msi, "doorbell-0") < 0) ||
        (msi_register(1, _on_msi, "doorbell-1") < 0)) {
        puts("msi_register() failed");
        return 1;
    }

    thread_create(_poster_stack, sizeof(_poster_stack),
                  THREAD_PRIORITY_MAIN - 1, 0,
                  _poster, NULL, "msi_poster");

    ztimer_sleep(ZTIMER_MSEC, (POST_COUNT + 1) * POST_GAP_MSEC);

    printf("done: ISR handled %u of %u posts\n", _isr_hits, POST_COUNT);
    if (_isr_hits != POST_COUNT) {
        puts("TEST FAILED");
        return 1;
    }

    puts("TEST PASSED");
    return 0;
}
