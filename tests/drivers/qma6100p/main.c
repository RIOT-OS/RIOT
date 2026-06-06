/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-FileCopyrightText: 2026 Léandre Le Duc <leandre.leduc38@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the QMA6100P accelerometer driver
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 * @author      Léandre Le Duc <leandre.leduc38@gmail.com>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "qma6100p.h"
#include "thread.h"
#include "thread_flags.h"

/* verification: lower ODR to prove the data-ready ISR is locked to the data rate */

static const qma6100p_odr_t rates[] = {
    QMA6100P_ODR_12HZ5,
    QMA6100P_ODR_25HZ,
    QMA6100P_ODR_100HZ,
};
static const unsigned expect_hz[] = { 12, 25, 100 };

#define QMA6100P_PARAM_RATE rates[0]

#include "qma6100p_params.h"
#include "ztimer.h"

#define SLEEP_1S        (1U)

/* thread flag raised by the data-ready ISR to wake the reader thread */
#define FLAG_DATA_READY (1U << 0)

/* incremented in ISR context on every data-ready interrupt */
static volatile unsigned irq_count;

static kernel_pid_t reader_pid = KERNEL_PID_UNDEF;
static char reader_stack[THREAD_STACKSIZE_MAIN];

static qma6100p_t dev;

static void callback(void *args)
{
    (void)args;
    irq_count++;
    if (reader_pid != KERNEL_PID_UNDEF) {
        thread_flags_set(thread_get(reader_pid), FLAG_DATA_READY);
    }
}

/* Wakes on each data-ready signal and reads the sample over I2C. */
static void *reader_thread(void *arg)
{
    (void)arg;
    qma6100p_data_t data;

    while (1) {
        thread_flags_wait_any(FLAG_DATA_READY);

        int res = qma6100p_read(&dev, &data);
        assert(res != QMA6100P_NO_NEW_DATA); /**<interrupt-driven: a wake must always carry fresh data */

        if (res == QMA6100P_DATA_READY) {
            printf("[data] x=%" PRId32 " y=%" PRId32 " z=%" PRId32 " ug\n",
                   data.x, data.y, data.z);
        }
        else if (res < 0) {
            printf("[data] read error: %d\n", res);
        }
    }
    return NULL;
}

static inline unsigned int _measure_irq_hz(void)
{
    unsigned before = irq_count;
    ztimer_sleep(ZTIMER_SEC, SLEEP_1S);
    return irq_count - before;
}

static const qma6100p_int_t interrupt = { .params = qma6100p_int_params[0], .cb = callback };

int main(void)
{
    int res;

    ztimer_sleep(ZTIMER_SEC, SLEEP_1S);
    puts("=== QMA6100P accelerometer driver test ===");
    printf("[init] I2C_DEV(%d) addr 0x%02x ... ",
           (int)qma6100p_params[0].i2c, qma6100p_params[0].addr);

    res = qma6100p_init(&dev, &qma6100p_params[0]);
    if (res < 0) {
        printf("FAILED (%d)\n", res);
        return 1;
    }
    puts("OK");

    assert(dev.params.rate == QMA6100P_ODR_12HZ5);

    puts("\n--- data-ready interrupt rate sweep ---");
    for (unsigned int i = 0; i < ARRAY_SIZE(rates); i++) {
        unsigned int try;
        qma6100p_params_t p = *qma6100p_params; /* mutable copy */
        p.rate = rates[i];

        printf("[ODR %u/%u] set rate -> %u Hz ... ",
               i + 1, (unsigned)ARRAY_SIZE(rates), expect_hz[i]);

        res = qma6100p_init(&dev, &p);
        if (res < 0 || dev.params.rate != rates[i]) {
            printf("FAILED to apply rate (res=%d)\n", res);
            res = -1;
            goto out;
        }

        res = qma6100p_set_data_ready_int(&dev, &interrupt);
        if (res < 0) {
            printf("FAILED to enable data-ready int (res=%d)\n", res);
            goto out;
        }
        puts("OK");

        res = -1;
        for (try = 0; try < 3; try++) {
            unsigned hz = _measure_irq_hz();
            int pass = (expect_hz[i] - 1 <= hz && hz <= expect_hz[i] + 1);
            printf("  try %u/3: measured %u Hz (expect ~%u) -> %s\n",
                   try + 1, hz, expect_hz[i], pass ? "PASS" : "off");
            if (pass) {
                res = 0;
                break;
            }
        }

        if (res < 0) {
            printf("[ODR %u/%u] FAILED: rate never matched ~%u Hz\n",
                   i + 1, (unsigned)ARRAY_SIZE(rates), expect_hz[i]);
            goto out;
        }
        printf("[ODR %u/%u] PASS (~%u Hz)\n",
               i + 1, (unsigned)ARRAY_SIZE(rates), expect_hz[i]);
    }

    puts("\n=== ALL TESTS PASSED ===");

    /* stream samples: ISR signals the reader thread, which reads over I2C */
    puts("\n--- interrupt-driven streaming ---");
    reader_pid = thread_create(reader_stack, sizeof(reader_stack),
                               THREAD_PRIORITY_MAIN - 1, 0,
                               reader_thread, NULL, "qma_reader");
    if (reader_pid < 0) {
        puts("Failed to create reader thread");
        res = 1;
        goto out;
    }

    return 0;

out:
    puts("\n=== TEST FAILED ===");
    return res;
}
