/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
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
 * @brief   Application for testing context switching triggered from IRQ
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "mutex.h"
#include "thread.h"
#include "periph/gpio.h"
#include "board.h"

#ifndef TEST_REPETITIONS
#define TEST_REPETITIONS    500
#endif

static mutex_t mut1 = MUTEX_INIT_LOCKED;
static mutex_t mut2 = MUTEX_INIT_LOCKED;
static mutex_t mut3 = MUTEX_INIT_LOCKED;

static char t2_stack[THREAD_STACKSIZE_DEFAULT];
static char t3_stack[THREAD_STACKSIZE_DEFAULT];

static uint16_t counter = 0;

static void cb_btn0(void *arg)
{
    (void) arg;
    thread_yield_higher();
    if (counter++ == TEST_REPETITIONS) {
        mutex_unlock(&mut1);
    }
    else {
        mutex_unlock(&mut2);
        gpio_toggle(BTN1_PIN);
        gpio_toggle(BTN1_PIN);
    }
}

static void cb_btn1(void *arg)
{
    (void) arg;
    thread_yield_higher();
    if (counter++ == TEST_REPETITIONS) {
        mutex_unlock(&mut1);
    }
    else {
        mutex_unlock(&mut3);
        gpio_toggle(BTN0_PIN);
        gpio_toggle(BTN0_PIN);
    }
}

static void *t2_impl(void *unused)
{
    (void)unused;
    while (1) {
	mutex_lock(&mut2);
    }
    return NULL;
}

static void *t3_impl(void *unused)
{
    (void)unused;
    while (1) {
	mutex_lock(&mut3);
    }
    return NULL;
}

int main(void)
{
    printf("Testing %u context switches triggered from ISR\n", (unsigned)TEST_REPETITIONS);
    thread_create(t2_stack, sizeof(t2_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  t2_impl, NULL, "t2");

    thread_create(t3_stack, sizeof(t3_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  t3_impl, NULL, "t3");

    gpio_init_int(BTN0_PIN, GPIO_OUT, BTN0_INT_FLANK, cb_btn0, NULL);
    gpio_init_int(BTN1_PIN, GPIO_OUT, BTN1_INT_FLANK, cb_btn1, NULL);

    gpio_toggle(BTN0_PIN);
    gpio_toggle(BTN0_PIN);

    mutex_lock(&mut1);

    puts("TEST PASSED");

    return 0;
}
