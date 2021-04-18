/*
 * Copyright (C) 2021 TUBA Freiberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "kernel_defines.h"
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
static named_lock_t nsec_lock = { .name = "NSEC", .mut = MUTEX_INIT_LOCKED };

static ztimer_t sec_tim = { .callback = release, .arg = &sec_lock };
static ztimer_t msec_tim = { .callback = release, .arg = &msec_lock };
static ztimer_t usec_tim = { .callback = release, .arg = &usec_lock };
static ztimer_t nsec_tim = { .callback = release, .arg = &nsec_lock };

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
    if (IS_USED(MODULE_ZTIMER_NSEC)) {
        ztimer_set(ZTIMER_NSEC, &nsec_tim, 50 * NS_PER_US * US_PER_MS);
    }

    printf("time %s:\t%" PRIu32 "\n", "Wait", (uint32_t)ztimer_now(ZTIMER_USEC));

    puts("waiting for locks");
    /* wait for mutexes */
    mutex_lock(&sec_lock.mut);
    mutex_lock(&msec_lock.mut);
    mutex_lock(&usec_lock.mut);
    if (IS_USED(MODULE_ZTIMER_NSEC)) {
        mutex_lock(&nsec_lock.mut);
    }

    printf("time %s:\t%" PRIu32 "\n", sec_lock.name, sec_lock.release_time);
    printf("time %s:\t%" PRIu32 "\n", msec_lock.name, msec_lock.release_time);
    printf("time %s:\t%" PRIu32 "\n", usec_lock.name, usec_lock.release_time);
    if (IS_USED(MODULE_ZTIMER_NSEC)) {
        printf("time %s:\t%" PRIu32 "\n", nsec_lock.name, nsec_lock.release_time);
    }

    printf("SUCCESS!\n");
    return 0;
}
