/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       irq_disable_restore test application
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>

#include "fmt.h"
#include "irq.h"
#include "ztimer.h"

#define DELAY       (10 * US_PER_MS)

static atomic_uint a = ATOMIC_VAR_INIT(0);
static atomic_uint a_during_isr = ATOMIC_VAR_INIT(0);
static atomic_uint b = ATOMIC_VAR_INIT(0);
static atomic_uint b_during_isr = ATOMIC_VAR_INIT(0);

static void busy_delay(void)
{
    uint32_t start = ztimer_now(ZTIMER_USEC);
    while (ztimer_now(ZTIMER_USEC) - start < DELAY) { }
}

/* Timer callback run in interrupt context; should not trigger between
 * irq_disable() and irq_restore()
 */
static void timer_callback(void *unused)
{
    (void)unused;
    atomic_store(&a_during_isr, atomic_load(&a));
    atomic_store(&b_during_isr, atomic_load(&b));
}

int main(void)
{
    ztimer_t xt = { .callback = timer_callback };

    print_str("Test for irq_disable() / irq_restore()\n"
              "======================================\n"
              "\n");

    /* Short enable-restore sequence to check if the irq_is_enabled() function
     * is reporting the correct IRQ state */
    print_str("Verifying IRQ state tracking works: ");
    int state_a = irq_is_enabled();
    unsigned state = irq_disable();
    int state_b = irq_is_enabled();
    irq_restore(state);
    int state_c = irq_is_enabled();
    if ((state_a != 0) && (state_b == 0) && (state_c != 0)) {
        print_str("[SUCCESS]\n");
    }
    else {
        print_str("[FAILURE]\n");
    }

    print_str("Verifying test works: ");
    ztimer_set(ZTIMER_USEC, &xt, DELAY / 2);
    atomic_store(&a, 1);
    busy_delay();
    atomic_store(&b, 1);

    /* Timer should have fired in the middle of busy_delay(), thus value of
     * a now and during ISR should both be 1, but value of b during ISR should
     * still be 0 but now it should be 1 */
    if ((atomic_load(&a) == atomic_load(&a_during_isr)) &&
            (atomic_load(&b) != atomic_load(&b_during_isr)))
        {
        print_str("[SUCCESS]\n");
    }
    else {
        print_str("[FAILURE]\n");
    }

    print_str("Test result: ");
    ztimer_set(ZTIMER_USEC, &xt, DELAY / 2);
    state = irq_disable();
    atomic_store(&a, 2);
    busy_delay();
    atomic_store(&b, 2);
    irq_restore(state);

    /* irq_disable() should have delayed execution of the timer until both
     * a and b have been set to 2.
     */

    if ((atomic_load(&a) == atomic_load(&a_during_isr)) &&
            (atomic_load(&b) == atomic_load(&b_during_isr)))
        {
        print_str("[SUCCESS]\n");
    }
    else {
        print_str("[FAILURE]\n");
    }

    return 0;
}
