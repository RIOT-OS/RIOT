/*
 * timer.c
 *
 *  Created on: 4 sie 2015
 *      Author: kkk
 */

/**
 * @ingroup     cpu_efm32wg
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Kubaszek Mateusz <mathir@student.agh.edu.pl>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "../emlib/inc/em_timer.h"
#include "../emlib/inc/em_cmu.h"

#if defined LETIMER_EN
#else

/** Unified IRQ handler for all timers */
static inline void irq_handler(tim_t timer, TIMER_TypeDef *dev);

/** Type for timer state */
typedef struct {
    void (*cb)(int);
} timer_conf_t;

/** Timer state memory */
timer_conf_t config[TIMER_NUMOF];


int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    TIMER_Init_TypeDef timer_init;
    TIMER_Init_TypeDef* timer = &timer_init;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            /* enable timer peripheral clock */
            TIMER_0_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_0_IRQ_CHAN, TIMER_IRQ_PRIO);
            /* Eables Compare modules */
        	TIMER_0_DEV->CC[0].CTRL |= _TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
        	TIMER_0_DEV->CC[1].CTRL |= _TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
        	TIMER_0_DEV->CC[2].CTRL |= _TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
            /* select timer */
            TIMER_TopSet((TIMER_TypeDef *)TIMER_0_DEV, 0xffff);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            /* enable timer peripheral clock */
            TIMER_1_CLKEN();
            /* set timer's IRQ priority */
            NVIC_SetPriority(TIMER_1_IRQ_CHAN, TIMER_IRQ_PRIO);
            /* Enables compare modules */
        	TIMER_1_DEV->CC[0].CTRL |= _TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
        	TIMER_1_DEV->CC[1].CTRL |= _TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
        	TIMER_1_DEV->CC[2].CTRL |= _TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
            /* select timer */
            TIMER_TopSet((TIMER_TypeDef *)TIMER_1_DEV, 0xffff);
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* set callback function */
    config[dev].cb = callback;

    timer->enable = false;
    timer->debugRun = true;
   	switch(ticks_per_us){
   	case 1:
   		timer->prescale = timerPrescale1024;
   		break;
   	case 2:
   		timer->prescale = timerPrescale4;
   		break;
   	case 4:
   		timer->prescale = timerPrescale2;
   		break;
   	case 8:
   		timer->prescale = timerPrescale1;
   		break;
   	default:
		timer->prescale = timerPrescale1;
   		break;
   	}
   	timer->clkSel = timerClkSelHFPerClk;
   	timer->fallAction = timerInputActionNone;
   	timer->riseAction = timerInputActionNone;
   	timer->mode = timerModeUp;
   	timer->dmaClrAct = false;
   	timer->quadModeX4 = false;
   	timer->oneShot = false;
   	timer->sync = false;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
        	TIMER_Init(TIMER_0_DEV, timer);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
        	TIMER_Init(TIMER_1_DEV, timer);
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* enable the timer's interrupt */
    timer_irq_enable(dev);

    /* start the timer */
    timer_start(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    int now = timer_read(dev);
    return timer_set_absolute(dev, channel, now + timeout - 1);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    TIMER_TypeDef *timer;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            timer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            timer = TIMER_1_DEV;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    switch (channel) {
        case 0:
        	timer->CC[0].CCV = (uint16_t) value;
            timer->IFC |= 1<<_TIMER_IFC_CC0_SHIFT;
            timer->IEN |= 1<<_TIMER_IEN_CC0_SHIFT;
            break;
        case 1:
        	timer->CC[1].CCV = (uint16_t) value;
			timer->IFC |= 1<<_TIMER_IFC_CC1_SHIFT;
			timer->IEN |= 1<<_TIMER_IEN_CC1_SHIFT;
            break;
        case 2:
        	timer->CC[2].CCV = (uint16_t) value;
			timer->IFC |= 1<<_TIMER_IFC_CC2_SHIFT;
			timer->IEN |= 1<<_TIMER_IEN_CC2_SHIFT;
            break;
        default:
            return -1;
    }

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    TIMER_TypeDef *timer;

    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            timer = TIMER_0_DEV;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            timer = TIMER_1_DEV;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    switch (channel) {
        case 0:
        	timer->IEN &= ~_TIMER_IEN_CC0_MASK;
            break;
        case 1:
        	timer->IEN &= ~_TIMER_IEN_CC1_MASK;
            break;
        case 2:
        	timer->IEN &= ~_TIMER_IEN_CC2_MASK;
            break;
        default:
            return -1;
    }

    return 0;
}

unsigned int timer_read(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            return TIMER_0_DEV->CNT;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            return TIMER_1_DEV->CNT;
            break;
#endif
        case TIMER_UNDEFINED:
        default:
            return 0;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            ((TIMER_TypeDef*)TIMER_0_DEV)->CMD = TIMER_CMD_START;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            ((TIMER_TypeDef*)TIMER_1_DEV)->CMD = TIMER_CMD_START;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_stop(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
        	((TIMER_TypeDef*)TIMER_0_DEV)->CMD = TIMER_CMD_STOP;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
        	((TIMER_TypeDef*)TIMER_1_DEV)->CMD = TIMER_CMD_STOP;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_enable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_EnableIRQ(TIMER_0_IRQ_CHAN);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_EnableIRQ(TIMER_1_IRQ_CHAN);
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            NVIC_DisableIRQ(TIMER_0_IRQ_CHAN);
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQ_CHAN);
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void timer_reset(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CNT = 0;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CNT = 0;
            break;
#endif
        case TIMER_UNDEFINED:
            break;
    }
}

void TIMER_0_ISR(void)
{
    irq_handler(TIMER_0, TIMER_0_DEV);
}

void TIMER_1_ISR(void)
{
    irq_handler(TIMER_1, TIMER_1_DEV);
}

static inline void irq_handler(tim_t timer, TIMER_TypeDef *dev)
{
    if (dev->IF & dev->IEN & _TIMER_IF_CC0_MASK) {
        dev->IFC |= _TIMER_IFC_CC0_MASK;
        dev->IEN &= ~_TIMER_IEN_CC0_MASK;
        config[timer].cb(0);
    }
    else if (dev->IF & dev->IEN & dev->IEN & _TIMER_IF_CC1_MASK) {
    	dev->IFC |= _TIMER_IFC_CC1_MASK;
		dev->IEN &= ~_TIMER_IEN_CC1_MASK;
        config[timer].cb(1);
    }
    else if (dev->IF & dev->IEN & _TIMER_IF_CC2_MASK) {
        dev->IFC |= _TIMER_IFC_CC2_MASK;
        dev->IEN &= ~_TIMER_IEN_CC2_MASK;
        config[timer].cb(2);
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif
