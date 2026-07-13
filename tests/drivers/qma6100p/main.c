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
#include "qma6100p_params.h"
#include "sema.h"
#include "thread.h"
#include "ztimer.h"

static const qma6100p_odr_t rates[] = {
    QMA6100P_ODR_12HZ5,
    QMA6100P_ODR_25HZ,
    QMA6100P_ODR_100HZ,
    QMA6100P_ODR_200HZ,
    QMA6100P_ODR_400HZ,
    QMA6100P_ODR_800HZ,
    QMA6100P_ODR_1600HZ,
};
static const unsigned expect_hz[] = { 12, 25, 100, 200, 400, 800, 1600 };

/* waited on by the reader thread, posted by the data-ready ISR to wake it */
static sema_t data_ready = SEMA_CREATE_LOCKED();

/* incremented in ISR context on every data-ready interrupt */
static volatile unsigned irq_count;

static char reader_stack[THREAD_STACKSIZE_MAIN];

static qma6100p_t dev;

static void callback_stream(void *args)
{
    (void)args;
    irq_count++;
    /* cap at 1 to make the semaphore binary */
    if (sema_get_value(&data_ready) == 0) {
        sema_post(&data_ready);
    }
}

static void callback(void *args)
{
    (void)args;
    irq_count++;
}

/* Wakes on each data-ready signal and reads the sample over I2C. */
static void *reader_thread(void *arg)
{
    (void)arg;
    qma6100p_data_t data;

    while (1) {
        sema_wait(&data_ready);

        int res = qma6100p_read(&dev, &data);
        /* interrupt-driven: a wake must always carry fresh data */
        assert(res != QMA6100P_NO_NEW_DATA);

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

/* Window over which the average data-ready frequency is observed */
#define MEASURE_WINDOW_SEC 4

static inline unsigned int _measure_irq_hz(void)
{
    unsigned before = irq_count;
    ztimer_sleep(ZTIMER_SEC, MEASURE_WINDOW_SEC);
    return (irq_count - before + (MEASURE_WINDOW_SEC / 2)) / MEASURE_WINDOW_SEC;
}

/* (Re)initialize the device at @p rate and enable the data-ready interrupt on
 * INT1. Each test calls this so it does not depend on any prior test state. */
static int _setup(qma6100p_odr_t rate, qma6100p_int_cb_t cb)
{
    qma6100p_params_t p = *qma6100p_params; /* mutable copy */
    p.rate = rate;

    int res = qma6100p_init(&dev, &p);
    if (res < 0) {
        return res;
    }

    if (dev.params.rate != rate) {
        return -1;
    }

    return qma6100p_set_data_ready_int(&dev, QMA6100P_INT1, cb, NULL);
}

static int test_init(void)
{
    int res;
    printf("[init] I2C_DEV(%d) addr 0x%02x ... ",
           (int)qma6100p_params[0].i2c, qma6100p_params[0].addr);

    res = qma6100p_init(&dev, &qma6100p_params[0]);
    if (res < 0) {
        printf("[init] FAILED (res=%d)\n", res);
        return res;
    }
    puts("OK");

    assert(dev.params.rate == QMA6100P_ODR_12HZ5);

    return res;
}

static int test_data_ready(void)
{
    int res;

    puts("\n--- data-ready interrupt rate sweep ---");
    for (unsigned int i = 0; i < ARRAY_SIZE(rates); i++) {
        printf("[ODR %u/%u] set rate -> %u Hz ... ",
               i + 1, (unsigned)ARRAY_SIZE(rates), expect_hz[i]);

        res = _setup(rates[i], callback);
        if (res < 0) {
            printf("[ODR %u/%u] FAILED to set up with rate %uHz (res=%d)\n",
                   i + 1, (unsigned)ARRAY_SIZE(rates), expect_hz[i], res);
            return res;
        }
        puts("OK");

        res = -1;
        for (unsigned int try = 0; try < 3; try++) {
            unsigned hz = _measure_irq_hz();
            /* 5% tolerance (observed ODR drift + 1 count), floored at 1 Hz */
            unsigned tol = expect_hz[i] / 20;
            if (tol < 1) {
                tol = 1;
            }
            int pass = (hz + tol >= expect_hz[i] && hz <= expect_hz[i] + tol);
            printf("  try %u/3: measured %u Hz (expect ~%u) -> %s\n",
                   try + 1, hz, expect_hz[i], pass ? "PASS" : "FAIL");
            if (pass) {
                res = 0;
                break;
            }
        }

        if (res < 0) {
            printf("[ODR %u/%u] FAILED: rate never matched ~%u Hz\n",
                   i + 1, (unsigned)ARRAY_SIZE(rates), expect_hz[i]);
            return res;
        }
        printf("[ODR %u/%u] PASS (~%u Hz)\n",
               i + 1, (unsigned)ARRAY_SIZE(rates), expect_hz[i]);
    }

    return res;
}

static int test_ulps(void)
{
    const qma6100p_odr_t rate = QMA6100P_ODR_100HZ;
    const unsigned expect_hz = 100;

    puts("\n--- ULPS mode test ---");

    int res = _setup(rate, callback);
    if (res < 0) {
        printf("[ULPS] FAILED to set up (res=%d)\n", res);
        return res;
    }

    /* enter ULPS: IRQs must stop */
    res = qma6100p_set_low_power(&dev);
    if (res < 0) {
        printf("[ULPS] FAILED to enter ULPS (res=%d)\n", res);
        return res;
    }

    irq_count = 0;
    unsigned hz = _measure_irq_hz();

    printf("[ULPS] in ULPS: %u IRQs/s (expect 0) -> %s\n",
           hz, hz == 0 ? "PASS" : "FAIL");
    if (hz != 0) {
        return -1;
    }

    /* wake up: ULPS disables all interrupts, so re-enable data-ready */
    res = qma6100p_set_active_mode(&dev);
    if (res < 0) {
        printf("[ULPS] FAILED to exit ULPS (res=%d)\n", res);
        return res;
    }
    res = qma6100p_set_data_ready_int(&dev, QMA6100P_INT1, callback, NULL);
    if (res < 0) {
        printf("[ULPS] FAILED to re-enable data-ready int after wake (res=%d)\n", res);
        return res;
    }

    /* exiting ULPS does a full reset + re-init: confirm the ODR is restored
     * and the interrupt fires again */
    irq_count = 0;
    unsigned hz_after = _measure_irq_hz();

    unsigned tol = expect_hz / 20; /* 5% tolerance */
    int pass_wake = (hz_after + tol >= expect_hz && hz_after <= expect_hz + tol);
    printf("[ULPS] after wake: %u IRQs/s (expect ~%u) -> %s\n",
           hz_after, expect_hz, pass_wake ? "PASS" : "FAIL");
    if (!pass_wake) {
        return -1;
    }

    return res;
}

static int test_streaming(void)
{
    puts("\n--- interrupt-driven streaming ---");

    int res = _setup(QMA6100P_ODR_12HZ5, callback_stream);
    if (res < 0) {
        printf("[stream] FAILED to set up (res=%d)\n", res);
        return res;
    }

    kernel_pid_t reader_pid = thread_create(reader_stack, sizeof(reader_stack),
                                            THREAD_PRIORITY_MAIN - 1, 0,
                                            reader_thread, NULL, "qma_reader");
    if (reader_pid < 0) {
        puts("[stream] FAILED to create reader thread");
        return -1;
    }

    return 0;
}

int main(void)
{
    int res;

    ztimer_sleep(ZTIMER_SEC, 5);
    puts("=== QMA6100P accelerometer driver test ===");

    res = test_init();
    if (res < 0) {
        goto out;
    }
    res = test_data_ready();
    if (res < 0) {
        goto out;
    }

    res = test_ulps();
    if (res < 0) {
        goto out;
    }

    /* run last: the reader thread it spawns runs forever */
    res = test_streaming();
    if (res < 0) {
        goto out;
    }

    return 0;

out:
    puts("\n=== TEST FAILED ===");
    return res;
}
