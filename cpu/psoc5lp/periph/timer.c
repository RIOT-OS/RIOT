/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_psoc5lp
 * @{
 *
 * @file
 * @brief       Implementation of the low-level timer driver for the PSoC 5LP
 *
 * @author      Murat Cakmak <mail@muratcakmak.net>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

#include <project.h>

/* guard file in case no timers are defined */
#if TIMER_0_EN

/*************************** MACRO DEFINITIONS ********************************/
/**
 * @brief Timer Reset Values
 */
#define TIMER0_CONTROL_RESET_DISABLE        (0)
#define TIMER0_CONTROL_RESET_ENABLE         (1)

/**
 * @brief Calculates Divider value of Timer Clock
 */
#define TIMER_CLK_DIVIDER_VALUE(us_per_tick) (BCLK__BUS_CLK__MHZ * (us_per_tick))

/*************************** TYPE DEFINITIONS ********************************/
/**
 * @brief Struct holding the configuration data for a UART device
 */
typedef struct {
    void (*cb)(int);            /**< timeout callback */
} timer_conf_t;

/******************************* VARIABLES ***********************************/

/**
 * @brief UART device configurations
 *
 * @param[in] us_per_tick   number of us passed for one timer tick
 */
static timer_conf_t config[TIMER_NUMOF];

/**************************** PRIVATE FUNCTIONS ******************************/

/**
 * @brief Handler function for Timer Interrupt
 */
CY_ISR(TIMER0_IRQHandler)
{
    /*
     * Read Status register in order to clear the sticky Terminal Count (TC) bit
	 * in the status register. Note that the function is not called, but rather
	 * the status is read directly.
	 */
    TIMER0_STATUS;

    /* Warn client about timeout */
    config[TIMER_0].cb(0);

    if (sched_context_switch_request) {
        thread_yield();
    }

    /* Disable interrupt */
    TIMER0_IRQn_Disable();
}

/**************************** PUBLIC FUNCTIONS *******************************/
int timer_init(tim_t dev, unsigned int us_per_tick, void (*callback)(int))
{
    if (dev == TIMER_0) {

        /* save callback */
        config[TIMER_0].cb = callback;

        /* Set divider value for Timer Timing */
        CLK_TIMER_SetDividerValue(TIMER_CLK_DIVIDER_VALUE(us_per_tick));

        /* Register Handler function for Timer Interrupt */
        TIMER0_IRQn_StartEx(TIMER0_IRQHandler);

        /* Disable Timer Interrupt as initial */
        TIMER0_IRQn_Disable();

        /* Start Timer */
        TIMER0_Start();

        return 0;
    }
    return -1;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    if (dev == TIMER_0) {
        unsigned int now = timer_read(dev);
        timer_set_absolute(dev, channel, now + timeout);
        return 1;
    }
    return -1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    /* To suppress unused variable warning */
    (void)channel;

    if (dev == TIMER_0) {

        TIMER0_WriteCompare(value);
        TIMER0_IRQn_Enable();

        return 1;
    }
    return -1;
}

int timer_clear(tim_t dev, int channel)
{
    /* To suppress unused variable warning */
    (void)channel;

    if (dev == TIMER_0) {
        TIMER0_Stop();

        return 1;
    }
    return -1;
}

unsigned int timer_read(tim_t dev)
{
    if (dev == TIMER_0) {
        return TIMER0_ReadCounter();
    }
    return 0;
}

void timer_start(tim_t dev)
{
    if (dev == TIMER_0) {
        TIMER0_Start();
    }
}

void timer_stop(tim_t dev)
{
    if (dev == TIMER_0) {
        TIMER0_Stop();
    }
}

void timer_irq_enable(tim_t dev)
{
    if (dev == TIMER_0) {
        TIMER0_IRQn_Enable();
    }
}

void timer_irq_disable(tim_t dev)
{
    if (dev == TIMER_0) {
        TIMER0_IRQn_Disable();
    }
}

void timer_reset(tim_t dev)
{
    if (dev == TIMER_0) {
        TIMER0_CONTROL_Write(TIMER0_CONTROL_RESET_ENABLE);
        asm("nop");                     /* just wait a cycle */
        TIMER0_CONTROL_Write(TIMER0_CONTROL_RESET_DISABLE);
    }
}

#endif /* TIMER_0_EN */
