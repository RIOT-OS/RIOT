/*
 * Copyright (C) 2014 volatiles UG (haftungsbeschränkt)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_tm4c123
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation, uses full-with (32 bit) timers
 *
 * @author      Benjamin Valentin <benjamin.valentin@volatiles.de>
 *
 * @}
 */

#include <stdlib.h>

#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "hwtimer_cpu.h"

#include "board.h"

#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/** Unified IRQ handler for all timers */
static inline void irq_handler(tim_t timer, TIMER0_Type *dev);

/** Type for timer state */
typedef struct {
    void (*cb)(int);
} timer_conf_t;

/** Timer state memory */
static timer_conf_t config[TIMER_NUMOF];
static uint16_t   overflow[TIMER_NUMOF];

static inline int get_timer_base(tim_t dev) {
    switch(dev) {
#if TIMER_0_EN
        case TIMER_0:
        return TIMER0_BASE;
#endif
#if TIMER_1_EN
        case TIMER_1:
        return TIMER1_BASE;
#endif
#if TIMER_2_EN
        case TIMER_2:
        return TIMER2_BASE;
#endif
#if TIMER_3_EN
        case TIMER_3:
        return TIMER3_BASE;
#endif
#if TIMER_4_EN
        case TIMER_4:
        return TIMER4_BASE;
#endif
#if TIMER_5_EN
        case TIMER_5:
        return TIMER5_BASE;
#endif
        default:
        return -1;
    }
}

static inline IRQn_Type get_timer_irq(tim_t dev) {
    switch(dev) {
#if TIMER_0_EN
        case TIMER_0:
        return TIMER0A_IRQn;
#endif
#if TIMER_1_EN
        case TIMER_1:
        return TIMER1A_IRQn;
#endif
#if TIMER_2_EN
        case TIMER_2:
        return TIMER2A_IRQn;
#endif
#if TIMER_3_EN
        case TIMER_3:
        return TIMER3A_IRQn;
#endif
#if TIMER_4_EN
        case TIMER_4:
        return TIMER4A_IRQn;
#endif
#if TIMER_5_EN
        case TIMER_5:
        return TIMER5A_IRQn;
#endif
        default:
        return -1;
    }
}

/**
 * @brief Initialize the given timer
 *
 * Each timer device is running with the given speed. Each can contain one or more channels
 * as defined in periph_conf.h. The timer is configured in up-counting mode and will count
 * until TIMER_x_MAX_VALUE as defined in used board's periph_conf.h until overflowing.
 *
 * The timer will be started automatically after initialization with interrupts enabled.
 *
 * @param[in] dev           the timer to initialize
 * @param[in] ticks_per_us  the timers speed in ticks per us
 * @param[in] callback      this callback is called in interrupt context, the emitting channel is
 *                          passed as argument
 *
 * @return                  returns 0 on success, -1 if speed not applicable of unknown device given
 */
int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int)) {
    DEBUG("timer_init(%d)\n", dev);
    DEBUG("Running at %d Hz\n", ROM_SysCtlClockGet());

    config[dev].cb = callback;

    uint32_t timer = get_timer_base(dev);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0 + dev);
    ROM_TimerConfigure(timer, TIMER_CFG_PERIODIC_UP);
    ROM_TimerLoadSet(timer, TIMER_A, HWTIMER_MAXTICKS);

    NVIC_EnableIRQ(get_timer_irq(dev));

    // enable timeout interrupt, just because
    ROM_TimerIntEnable(timer, TIMER_TIMA_TIMEOUT);

    timer_start(dev);

    return 0;
}

/**
 * @brief Set a given timer channel for the given timer device. The callback given during
 * initialization is called when timeout ticks have passed after calling this function
 *
 * @param[in] dev           the timer device to set
 * @param[in] channel       the channel to set
 * @param[in] timeout       timeout in ticks after that the registered callback is executed
 *
 * @return                  1 on success, -1 on error
 */
int timer_set(tim_t dev, int channel, unsigned int timeout) {
    DEBUG("timer_set(%d, %d)\n", dev, timeout);

    return timer_set_absolute(dev, channel, timeout + timer_read(dev));
}

/**
 * @brief Set an absolute timeout value for the given channel of the given timer device
 *
 * @param[in] dev           the timer device to set
 * @param[in] channel       the channel to set
 * @param[in] value         the absolute compare value when the callback will be triggered
 *
 * @return                  1 on success, -1 on error
 */
int timer_set_absolute(tim_t dev, int channel, unsigned int value) {
    DEBUG("timer_set_absolute(%d, %u)\n", dev, value);

    ROM_TimerMatchSet(get_timer_base(dev), TIMER_A, value * TIMER_0_PRESCALER);

    timer_irq_enable(dev);

    return 1;
}

/**
 * @brief Clear the given channel of the given timer device
 *
 * @param[in] dev           the timer device to clear
 * @param[in] channel       the channel on the given device to clear
 *
 * @return                  1 on success, -1 on error
 */
int timer_clear(tim_t dev, int channel) {
    DEBUG("TODO: timer_clear(%d, %d)\n", dev, channel);

    // since we don't have channels in 32bit mode, just disable the interrupt

    // timer_irq_disable(dev);

    return 1;
}

/**
 * @brief Read the current value of the given timer device
 *
 * @param[in] dev           the timer to read the current value from
 *
 * @return                  the timers current value
 */
unsigned int timer_read(tim_t dev) {
    uint32_t value = ROM_TimerValueGet(get_timer_base(dev), TIMER_A);

    DEBUG("timer_read(%d) = [%d] %u -> %d\n", dev, overflow[dev], value, value / TIMER_0_PRESCALER + (overflow[dev] << 22));
    return value / TIMER_0_PRESCALER + (overflow[dev] << 22);
}

/**
 * @brief Start the given timer. This function is only needed if the timer was stopped manually before
 *
 * @param[in] dev           the timer device to stop
 */
void timer_start(tim_t dev) {
    DEBUG("%s(%d)\n", __FUNCTION__, dev);
    ROM_TimerEnable(get_timer_base(dev), TIMER_A);
}

/**
 * @brief Stop the given timer - this will effect all of the timer's channels
 *
 * @param[in] dev           the timer to stop
 */
void timer_stop(tim_t dev) {
    DEBUG("%s(%d)\n", __FUNCTION__, dev);
    ROM_TimerDisable(get_timer_base(dev), TIMER_A);
}

/**
 * @brief Enable the interrupts for the given timer
 *
 * @param[in] dev           timer to enable interrupts for
 */
void timer_irq_enable(tim_t dev) {
    DEBUG("%s(%d)\n", __FUNCTION__, dev);
    ROM_TimerIntEnable(get_timer_base(dev), TIMER_TIMA_MATCH);
}

/**
 * @brief Disable interrupts for the given timer
 *
 * @param[in] dev           the timer to disable interrupts for
 */
void timer_irq_disable(tim_t dev) {
    DEBUG("%s(%d)\n", __FUNCTION__, dev);
    ROM_TimerIntDisable(get_timer_base(dev), TIMER_TIMA_MATCH);
}

/**
 * @brief Reset the up-counting value to zero for the given timer
 *
 * Note that this function effects all currently set channels and it can lead to non-deterministic timeouts
 * if any channel is active when this function is called.
 *
 * @param[in] dev           the timer to reset
 */
void timer_reset(tim_t dev) {
    DEBUG("%s(%d)\n", __FUNCTION__, dev);
    ((TIMER0_Type *) get_timer_base(dev))->TAV = 0;
}

#if TIMER_0_EN
void TIMER_0_ISR(void)
{
    irq_handler(TIMER_0, TIMER_0_DEV);
}
#endif
#if TIMER_1_EN
__attribute__ ((naked)) void TIMER_1_ISR(void)
{
    irq_handler(TIMER_1, TIMER_1_DEV);
}
#endif
#if TIMER_2_EN
__attribute__ ((naked)) void TIMER_2_ISR(void)
{
    irq_handler(TIMER_2, TIMER_2_DEV);
}
#endif
#if TIMER_3_EN
__attribute__ ((naked)) void TIMER_3_ISR(void)
{
    irq_handler(TIMER_3, TIMER_3_DEV);
}
#endif
#if TIMER_4_EN
__attribute__ ((naked)) void TIMER_4_ISR(void)
{
    irq_handler(TIMER_4, TIMER_4_DEV);
}
#endif
#if TIMER_5_EN
__attribute__ ((naked)) void TIMER_5_ISR(void)
{
    irq_handler(TIMER_5, TIMER_5_DEV);
}
#endif

static inline void irq_handler(tim_t timer, TIMER0_Type *dev)
{
    if (ROM_TimerIntStatus((uint32_t) dev, 0) & TIMER_TIMA_TIMEOUT) {
        ROM_TimerIntClear ((uint32_t) dev, TIMER_TIMA_TIMEOUT);
        DEBUG("Timer %d overflowed\n", timer);
        ++overflow[timer];
    } else if (ROM_TimerIntStatus((uint32_t) dev, 0) & TIMER_TIMA_MATCH) {
        ROM_TimerIntClear ((uint32_t) dev, TIMER_TIMA_MATCH);
        timer_irq_disable(timer);
        config[timer].cb(timer);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
