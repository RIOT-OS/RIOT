/*
 * SPDX-FileCopyrightText: 2021 TUBA Freiberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief    high level ztimer test application
 *
 * @author   Karl Fessel <karl.fessel@ovgu.de>
 *
 *
 * @}
 */

#include <stdio.h>

#include "ztimer.h"
#include "mutex.h"

/* only header information is used we do not need to use MODULE_TIMEX */
#include "timex.h"

typedef struct named_lock {
    char *name;
    mutex_t mut;
    uint32_t release_time;
} named_lock_t;

void release(void *arg);

static named_lock_t sec_lock = { .name =  "SEC", .mut = MUTEX_INIT_LOCKED };
static named_lock_t msec_lock = { .name = "MSEC", .mut = MUTEX_INIT_LOCKED };
static named_lock_t usec_lock = { .name = "USEC", .mut = MUTEX_INIT_LOCKED };

static ztimer_t sec_tim = { .callback = release, .arg = &sec_lock };
static ztimer_t msec_tim = { .callback = release, .arg = &msec_lock };
static ztimer_t usec_tim = { .callback = release, .arg = &usec_lock };

void release(void *arg)
{
    named_lock_t *e = arg;

    e->release_time = (uint32_t)ztimer_now(ZTIMER_USEC);
    puts(e->name);
    mutex_unlock(&e->mut);
}

int main(void)
{
    puts("starting ztimers");
    /* start a timer on each high level ztimer*/
    ztimer_set(ZTIMER_SEC, &sec_tim, 1);
    ztimer_set(ZTIMER_MSEC, &msec_tim, 200);
    ztimer_set(ZTIMER_USEC, &usec_tim, 100 * US_PER_MS);

    printf("time %s:\t%" PRIu32 "\n", "Wait", (uint32_t)ztimer_now(ZTIMER_USEC));

    puts("waiting for locks");
    /* wait for mutexes */
    mutex_lock(&sec_lock.mut);
    mutex_lock(&msec_lock.mut);
    mutex_lock(&usec_lock.mut);

    printf("time %s:\t%" PRIu32 "\n", sec_lock.name, sec_lock.release_time);
    printf("time %s:\t%" PRIu32 "\n", msec_lock.name, msec_lock.release_time);
    printf("time %s:\t%" PRIu32 "\n", usec_lock.name, usec_lock.release_time);

    printf("SUCCESS!\n");
    return 0;
}
