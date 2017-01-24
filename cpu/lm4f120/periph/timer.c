/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *               2015 Marc Poulhiès <dkm@kataplop.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file        timer.c
 * @brief       Implementation of the low-level timer driver for the LM4F120
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *              Marc Poulhiès <dkm@kataplop.net>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard file in case no timers are defined */
#if TIMER_NUMOF

/**
 * @brief Struct holding the configuration data
 * @{
 */
typedef struct {
    timer_cb_t cb;          /**< timeout callback */
    void *arg;              /**< argument to the callback */
    unsigned int divisor;   /**< software clock divisor */
} timer_conf_t;

static timer_conf_t config[TIMER_NUMOF];
/**@}*/

#include "hw_timer.h"

/* enable timer interrupts */
static inline void _irq_enable(tim_t dev);

/* Missing from driverlib */
static inline unsigned long
PRIV_TimerPrescaleSnapshotGet(unsigned long ulbase, unsigned long ultimer) {
    return((ultimer == TIMER_A) ? HWREG(ulbase + TIMER_O_TAPS) :
           HWREG(ulbase + TIMER_O_TBPS));
}

static inline unsigned long long _scaled_to_ll_value(unsigned int uncorrected, unsigned int divisor)
{
    const unsigned long long scaledv = (unsigned long long) uncorrected * divisor;
    return scaledv;
}

static inline unsigned int _llvalue_to_scaled_value(unsigned long long corrected, unsigned int divisor)
{
    const unsigned long long scaledv = corrected / divisor;
    return scaledv;
}

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    if (dev >= TIMER_NUMOF){
        return -1;
    }

    config[dev].cb = cb;
    config[dev].arg = arg;
    config[dev].divisor = ROM_SysCtlClockGet() / freq;

    unsigned int sysctl_timer;
    unsigned int timer_base;
    unsigned int timer_side = TIMER_A;
    unsigned int timer_cfg = TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC_UP | TIMER_TAMR_TAMIE;
    unsigned int timer_max_val;
    unsigned int timer_intbit = TIMER_TIMA_TIMEOUT | TIMER_TIMA_MATCH;

    switch(dev){
#if TIMER_0_EN
    case TIMER_0:
        sysctl_timer = SYSCTL_PERIPH_WTIMER0;
        timer_base = WTIMER0_BASE;
        timer_max_val = TIMER_0_MAX_VALUE;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        sysctl_timer = SYSCTL_PERIPH_WTIMER1;
        timer_base = WTIMER1_BASE;
        timer_max_val = TIMER_1_MAX_VALUE;
        break;
#endif
    default:
        return -1; /* unreachable */
    }


    ROM_SysCtlPeripheralEnable(sysctl_timer);

    ROM_TimerDisable(timer_base, timer_side);
    ROM_TimerConfigure(timer_base, timer_cfg);

    unsigned long long lltimer_val_max = _scaled_to_ll_value(timer_max_val, config[dev].divisor);

    ROM_TimerPrescaleSet(timer_base, timer_side, lltimer_val_max >> 32);
    ROM_TimerLoadSet(timer_base, timer_side, lltimer_val_max & 0xFFFFFFFF);
    ROM_TimerIntClear(timer_base, timer_intbit);

    ROM_TimerIntEnable(timer_base, timer_intbit);

    _irq_enable(dev);
    timer_start(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    unsigned int corrected_now;
    int retval;

    if (dev >= TIMER_NUMOF){
        return -1;
    }

    corrected_now = timer_read(dev);
    retval = timer_set_absolute(dev, channel, corrected_now+timeout);

    return retval;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    unsigned int timer_base;
    unsigned int timer_side = TIMER_A;
    unsigned long long scaledv;

    if (dev >= TIMER_NUMOF){
        return -1;
    }

    switch(dev){
#if TIMER_0_EN
    case TIMER_0:
        timer_base = WTIMER0_BASE;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        timer_base = WTIMER1_BASE;
        break;
#endif
    default:
        return -1; /* unreachable */
        break;
    }
    ROM_TimerDisable(timer_base, timer_side);

    scaledv = _scaled_to_ll_value(value, config[dev].divisor);

    if (scaledv>>32){
        ROM_TimerPrescaleMatchSet(timer_base, timer_side, scaledv >> 32);
    }
    else {
        ROM_TimerPrescaleMatchSet(timer_base, timer_side, 0);
    }

    ROM_TimerMatchSet(timer_base, timer_side, (unsigned long) (scaledv & 0xFFFFFFFF));
    ROM_TimerEnable(timer_base, timer_side);

    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    unsigned int timer_intbit = TIMER_TIMA_TIMEOUT;
    unsigned int timer_base;

    if (dev >= TIMER_NUMOF){
        return -1;
    }

    switch(dev){
#if TIMER_0_EN
    case TIMER_0:
        timer_base = WTIMER0_BASE;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        timer_base = WTIMER1_BASE;
        break;
#endif
    default:
        return -1; /* unreachable */
        break;
    }

    ROM_TimerIntClear(timer_base, timer_intbit);
    return 1;
}

unsigned int timer_read(tim_t dev)
{
    unsigned int timer_base;
    unsigned int timer_side = TIMER_A;
    unsigned long long high_bits, high_bits_dup;
    unsigned long long low_bits;
    unsigned long long total;
    unsigned int scaled_value;

    if (dev >= TIMER_NUMOF){
        return -1;
    }

    switch(dev){
#if TIMER_0_EN
    case TIMER_0:
        timer_base = WTIMER0_BASE;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        timer_base = WTIMER1_BASE;
        break;
#endif
    default:
        return -1; /* unreachable */
        break;
    }

    /* handle overflow happening between the 2 register reads */
    do {
      high_bits = ((unsigned long long)PRIV_TimerPrescaleSnapshotGet(timer_base, timer_side)) << 32;
      low_bits = (unsigned long long)ROM_TimerValueGet(timer_base, timer_side);
      high_bits_dup = ((unsigned long long)PRIV_TimerPrescaleSnapshotGet(timer_base, timer_side)) << 32;
    } while (high_bits != high_bits_dup);

    total = high_bits + low_bits;
    DEBUG("Combined %lx:%lx\n", (unsigned long) (total>>32), (unsigned long) (total & 0xFFFFFFFF));

    scaled_value = _llvalue_to_scaled_value(total, config[dev].divisor);

    return scaled_value;
}

void timer_start(tim_t dev)
{
    unsigned int timer_base;
    unsigned int timer_side = TIMER_A;

    if (dev >= TIMER_NUMOF){
        return ;
    }

    switch(dev){
#if TIMER_0_EN
    case TIMER_0:
        timer_base = WTIMER0_BASE;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        timer_base = WTIMER1_BASE;
        break;
#endif
    default:
        return; /* unreachable */
    }

    ROM_TimerEnable(timer_base, timer_side);
}

void timer_stop(tim_t dev)
{
    unsigned int timer_base;
    unsigned int timer_side = TIMER_A;

    if (dev >= TIMER_NUMOF){
        return;
    }

    switch(dev){
#if TIMER_0_EN
    case TIMER_0:
        timer_base = WTIMER0_BASE;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        timer_base = WTIMER1_BASE;
        break;
#endif
    default:
        return; /* unreachable */
    }

    ROM_TimerDisable(timer_base, timer_side);
}

static inline void _irq_enable(tim_t dev)
{
    unsigned int timer_intbase;

    if (dev >= TIMER_NUMOF){
        return;
    }

    switch(dev){
#if TIMER_0_EN
    case TIMER_0:
        timer_intbase = INT_WTIMER0A;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        timer_intbase = INT_WTIMER1A;
        break;
#endif
    default:
        return; /* unreachable */
    }

    ROM_IntPrioritySet(timer_intbase, 32);
    ROM_IntEnable(timer_intbase);
}

#if TIMER_0_EN
void isr_wtimer0a(void)
{
    /* Clears both IT */
    ROM_TimerIntClear(WTIMER0_BASE, TIMER_TIMA_TIMEOUT | TIMER_TIMA_MATCH);
    config[TIMER_0].cb(config[TIMER_0].arg, 0);
    cortexm_isr_end();
}
#endif /* TIMER_0_EN */

#if TIMER_1_EN
void isr_wtimer1a(void)
{
    ROM_TimerIntClear(WTIMER1_BASE, TIMER_TIMA_TIMEOUT | TIMER_TIMA_MATCH);

    config[TIMER_1].cb(config[TIMER_0].arg, 0);
    cortexm_isr_end();
}
#endif /* TIMER_1_EN */

#endif /* TIMER_NUMOF */
/** @} */
