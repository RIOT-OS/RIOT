/*
 * timer.c
 *
 *  Created on: 24 wrz 2015
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
#include "cpu_conf.h"
#include "../emlib/inc/em_letimer.h"
#include "../emlib/inc/em_cmu.h"
#include "../emlib/inc/em_usart.h"
#define ENABLE_DEBUG (0)
#include "debug.h"

//volatile uint16_t CNT;
//volatile uint32_t CC0;
//volatile char CC0_FLAG;
//volatile uint32_t CC1;
//volatile char CC1_FLAG;

#if defined LETIMER_EN

/** Type for timer state */
typedef struct {
    void (*cb)(int);
} timer_conf_t;

/** Timer state memory */
timer_conf_t config[TIMER_NUMOF];

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    DEBUG("%s: ticks_per_us=%u\n", __func__, ticks_per_us);
//	CC0 = 0;
//	CC1 = 0;
//	CC0_FLAG = 0;
//	CC1_FLAG = 0;

    LETIMER_Reset(LETIMER0);
    LETIMER0->CNT = 0xffff;
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockEnable(cmuClock_LETIMER0, true);

    /* Configuring LETIMER */
    const LETIMER_Init_TypeDef letimerInit = {.enable = false, /* Start counting when init completed. */
    .debugRun = false, /* Counter shall not keep running during debug halt. */
    .rtcComp0Enable = false, /* Don't start counting on RTC COMP0 match. */
    .rtcComp1Enable = false, /* Don't start counting on RTC COMP1 match. */
    .comp0Top = false, /* Top value is 0xffff */
    .bufTop = false, /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
    .out0Pol = 0, /* Idle value for output 0. */
    .out1Pol = 0, /* Idle value for output 1. */
    .ufoa0 = letimerUFOANone, /* PWM output on output 0 */
    .ufoa1 = letimerUFOANone, /* Pulse output on output 1*/
    .repMode = letimerRepeatFree /* Count until stopped */
    };
    /* set callback function */
    config[dev].cb = callback;
    /* Initialize LETIMER */
    LETIMER_Init(LETIMER0, &letimerInit);
    /* Enable NVIC interrupts */
    LETIMER_IntEnable(LETIMER0, LETIMER_IF_UF);
    timer_start(0);
    timer_irq_enable(0);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    int now = timer_read(dev);
    printf("now - %x\n", now);
    return timer_set_absolute(dev, channel, now + timeout - 1);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
//    uint32_t cur;
    switch(channel) {
    case 0:
        LETIMER0->COMP0 = value ^ 0xffff;
        LETIMER0->IFC |= _LETIMER_IFC_COMP0_MASK;
        LETIMER0->IEN |= _LETIMER_IEN_COMP0_MASK;
//	    cur = timer_read(0);
//		if((value - cur) >= 0xffff){
//			CC0 = value;
//			CC0_FLAG = 1;
//
//		} else {
//			LETIMER0->COMP0 = value^0xffff;
//			LETIMER0->IFC |= _LETIMER_IFC_COMP0_MASK;
//			LETIMER0->IEN |= _LETIMER_IEN_COMP0_MASK;
//		}
        break;
    case 1:
        LETIMER0->COMP1 = value ^ 0xffff;
        LETIMER0->IFC |= _LETIMER_IFC_COMP1_MASK;
        LETIMER0->IEN |= _LETIMER_IEN_COMP1_MASK;
//		cur = timer_read(0);
//		if((value - cur) >= 0xffff){
//			CC1 = value;
//			CC1_FLAG = 1;
//		} else {
//			LETIMER0->COMP1 = value^0xffff;
//			LETIMER0->IFC |= _LETIMER_IFC_COMP1_MASK;
//			LETIMER0->IEN |= _LETIMER_IEN_COMP1_MASK;
//		}
//		break;
    }

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    DEBUG("%s: channel=%i\n", __func__, channel);
    switch(channel) {
    case 0:
        LETIMER0->IEN &= ~_LETIMER_IEN_COMP0_MASK;
        break;
    case 1:
        LETIMER0->IEN &= ~_LETIMER_IEN_COMP1_MASK;
        break;
    default:
        return -1;
    }
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    unsigned int cnt;
    cnt = ((LETIMER0->CNT ^ 0xffff) & 0xffff);
    return cnt;
}

void timer_start(tim_t dev)
{
    DEBUG("%s: \n", __func__);
    LETIMER0->CMD |= _LETIMER_CMD_START_MASK;
}

void timer_stop(tim_t dev)
{
    DEBUG("%s:\n", __func__);
    LETIMER0->CMD |= _LETIMER_CMD_STOP_MASK;
}

void timer_irq_enable(tim_t dev)
{
    DEBUG("%s:\n", __func__);
    NVIC_EnableIRQ(LETIMER0_IRQn);

}

void timer_irq_disable(tim_t dev)
{
    DEBUG("%s:\n", __func__);
    NVIC_DisableIRQ(LETIMER0_IRQn);
}

void timer_reset(tim_t dev)
{
    DEBUG("%s:\n", __func__);
//    CNT = 0;
    LETIMER0->CNT = 0xffff;
}

void LETIMER0_IRQHandler(void)
{
    if(LETIMER0->IF & _LETIMER_IF_UF_MASK) {
        LETIMER0->IFC |= _LETIMER_IF_UF_MASK;
//        ++CNT;
//        puts("interrupt");
//        uint32_t cnt = timer_read(0);
//        if(CC0_FLAG) {
//            if((CC0 - cnt) >= 0xffff) {
//                CC0 -= 0xffff;
//                DEBUG("%s: cc0 set\n", __func__);
//            } else {
//                LETIMER0->COMP0 = (CC0 ^ 0xffff) & 0xffff;
//                LETIMER0->IFC |= _LETIMER_IFC_COMP0_MASK;
//                LETIMER0->IEN |= _LETIMER_IEN_COMP0_MASK;
//                CC0_FLAG = 0;
//                DEBUG("%s: comp0 set\n", __func__);
//            }
//        }
//        if(CC1_FLAG) {
//            if((CC1 - cnt) >= 0xffff) {
//                CC1 -= 0xffff;
//                DEBUG("%s: cc1 set\n", __func__);
//            } else {
//                LETIMER0->COMP1 = (CC1 ^ 0xffff) & 0xffff;
//                LETIMER0->IFC |= _LETIMER_IFC_COMP1_MASK;
//                LETIMER0->IEN |= _LETIMER_IEN_COMP1_MASK;
//                CC1_FLAG = 0;
//                DEBUG("%s: comp1 set\n", __func__);
//            }
//        }
    } else if((LETIMER0->IF & _LETIMER_IF_COMP0_MASK) && (LETIMER0->IEN & _LETIMER_IEN_COMP0_MASK)) {
        LETIMER0->IFC |= _LETIMER_IFC_COMP0_MASK;
        LETIMER0->IEN &= ~_LETIMER_IEN_COMP0_MASK;
//        CC0_FLAG = 0;
        DEBUG("%s: callback\n", __func__);
        config[TIMER_0].cb(0);
    } else if((LETIMER0->IF & _LETIMER_IF_COMP1_MASK) && (LETIMER0->IEN & _LETIMER_IEN_COMP1_MASK)) {
        LETIMER0->IFC |= _LETIMER_IFC_COMP1_MASK;
        LETIMER0->IEN &= ~_LETIMER_IEN_COMP1_MASK;
//        CC1_FLAG = 0;
        DEBUG("%s: callback\n", __func__);
        config[TIMER_0].cb(1);
    }
    if(sched_context_switch_request) {
        thread_yield();
    }
}

#endif

