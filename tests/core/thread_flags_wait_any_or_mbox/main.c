/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test application for testing the thread_flags interaction of
 *              core_mbox
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "atomic_utils.h"
#include "kernel_defines.h"
#include "mbox.h"
#include "test_utils/expect.h"
#include "thread_flags.h"
#include "ztimer.h"

#define MSG_TYPE    42
#define MSG_VAL     1337

static msg_t queue[4];
static mbox_t mbox = MBOX_INIT(queue, ARRAY_SIZE(queue));
static uint8_t msg_put_from_isr = 0;
static uint8_t flags_set_from_isr = 0;

void cb_mbox_put(void *arg)
{
    (void)arg;
    msg_t msg = { .type = MSG_TYPE, .content.value = MSG_VAL };
    mbox_try_put(&mbox, &msg);
    atomic_store_u8(&msg_put_from_isr, 1);
}

void cb_flags_set(void *arg)
{
    thread_t *target = arg;
    thread_flags_set(target, THREAD_FLAG_TIMEOUT);
    atomic_store_u8(&flags_set_from_isr, 1);
}

void cb_both(void *arg)
{
    thread_t *target = arg;
    thread_flags_set(target, THREAD_FLAG_TIMEOUT);
    msg_t msg = { .type = MSG_TYPE, .content.value = MSG_VAL };
    mbox_try_put(&mbox, &msg);
    atomic_store_u8(&msg_put_from_isr, 1);
    atomic_store_u8(&flags_set_from_isr, 1);
}

static void setup(msg_t *msg, thread_flags_t *flags)
{
    atomic_store_u8(&msg_put_from_isr, 0);
    atomic_store_u8(&flags_set_from_isr, 0);
    *flags = THREAD_FLAG_TIMEOUT;
    memset(msg, 0, sizeof(*msg));
}

static void teardown(void)
{
    thread_flags_clear(THREAD_FLAG_TIMEOUT | THREAD_FLAG_MSG_WAITING);
    msg_t garbagebin;
    while (mbox_try_get(&mbox, &garbagebin)) {
        /* drain mbox */
    }
}

int main(void)
{
    msg_t msg;
    thread_flags_t flags;
    ztimer_t zt_mbox = { .callback = cb_mbox_put };
    ztimer_t zt_flags = { .callback = cb_flags_set, .arg = thread_get_active() };
    ztimer_t zt_both = { .callback = cb_both, .arg = thread_get_active() };

    puts("testing immediate return with flag already set");
    setup(&msg, &flags);
    thread_flags_set(thread_get_active(), THREAD_FLAG_TIMEOUT);
    flags = thread_flags_wait_any_or_mbox(&mbox, &msg, flags);
    expect(flags == THREAD_FLAG_TIMEOUT);
    expect((msg.type == 0) && (msg.content.value == 0));
    expect(0 == atomic_load_u8(&msg_put_from_isr));
    expect(0 == atomic_load_u8(&flags_set_from_isr));
    teardown();
    puts("[OK]");

    puts("testing immediate return with msg already in queue");
    msg.type = MSG_TYPE + 1;
    msg.content.value = MSG_VAL + 1;
    mbox_put(&mbox, &msg);
    setup(&msg, &flags);
    flags = thread_flags_wait_any_or_mbox(&mbox, &msg, flags);
    expect(flags == 0);
    expect((msg.type == MSG_TYPE + 1) && (msg.content.value == MSG_VAL + 1));
    expect(0 == atomic_load_u8(&msg_put_from_isr));
    expect(0 == atomic_load_u8(&flags_set_from_isr));
    teardown();
    puts("[OK]");

    puts("testing immediate return with both msg already in queue and flag set");
    msg.type = MSG_TYPE + 1;
    msg.content.value = MSG_VAL + 1;
    mbox_put(&mbox, &msg);
    thread_flags_set(thread_get_active(), THREAD_FLAG_TIMEOUT);
    setup(&msg, &flags);
    flags = thread_flags_wait_any_or_mbox(&mbox, &msg, flags);
    expect(flags == 0);
    expect((msg.type == MSG_TYPE + 1) && (msg.content.value == MSG_VAL + 1));
    expect(0 == atomic_load_u8(&msg_put_from_isr));
    expect(0 == atomic_load_u8(&flags_set_from_isr));
    teardown();
    puts("[OK]");

    puts("testing actual blocking + flag comes first");
    setup(&msg, &flags);
    ztimer_set(ZTIMER_MSEC, &zt_flags, 3);
    flags = thread_flags_wait_any_or_mbox(&mbox, &msg, flags);
    expect(flags == THREAD_FLAG_TIMEOUT);
    expect((msg.type == 0) && (msg.content.value == 0));
    expect(0 == atomic_load_u8(&msg_put_from_isr));
    expect(1 == atomic_load_u8(&flags_set_from_isr));
    teardown();
    puts("[OK]");

    puts("testing actual blocking + msg comes first");
    setup(&msg, &flags);
    ztimer_set(ZTIMER_MSEC, &zt_mbox, 3);
    flags = thread_flags_wait_any_or_mbox(&mbox, &msg, flags);
    expect(flags == 0);
    expect((msg.type == MSG_TYPE) && (msg.content.value == MSG_VAL));
    expect(1 == atomic_load_u8(&msg_put_from_isr));
    expect(0 == atomic_load_u8(&flags_set_from_isr));
    teardown();
    puts("[OK]");

    puts("testing actual blocking + msg and flag come together");
    setup(&msg, &flags);
    ztimer_set(ZTIMER_MSEC, &zt_both, 3);
    flags = thread_flags_wait_any_or_mbox(&mbox, &msg, flags);
    expect(flags == 0);
    expect((msg.type == MSG_TYPE) && (msg.content.value == MSG_VAL));
    expect(1 == atomic_load_u8(&msg_put_from_isr));
    expect(1 == atomic_load_u8(&flags_set_from_isr));
    teardown();
    puts("[OK]");

    puts("TEST PASSED");

    return 0;
}
