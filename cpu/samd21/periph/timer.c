/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation for the SAM3X8E CPU
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"

#include "periph/timer.h"
#include "periph_conf.h"


typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * @brief Timer state memory
 */
timer_conf_t config[TIMER_NUMOF];


/**
 * @brief Setup the given timer
 *
 * The SAM3X8E has 3 timers. Each timer has 3 independent channels.
 * RIOT uses the timers in WAVE mode with the following clock chaining:
 *
 * ----------                ----------
 * |        |                |        |-> IRQ-compareA
 * | TCx[2] | ---- TIOA2 --->| TCx[0] |-> IRQ-compareB
 * |        |        |       |        |-> IRQ-compareC
 * ----------        |       ----------
 *     ^             |
 *     |             |       ----------
 *     |             |       |        |-> IRQ-compareA
 * TIMER_CLOCK1      ------->| TCx[1] |-> IRQ-compareB
 *                           |        |-> IRQ-compareC
 *                           ----------
 *
 * For each timer, channel 0 is used to implement a prescaler. Channel 0 is
 * driven by the MCK / 2 (42MHz) (TIMER_CLOCK1).
 */
int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
//     Tcc *tim;
//
//     /* select the timer and enable the timer specific peripheral clocks */
//     switch (dev) {
// #if TIMER_0_EN
//         case TIMER_0:
//             tim = TIMER_0_DEV;
//             // PMC->PMC_PCER0 = (1 << ID_TC0) | (1 << ID_TC1) | (1 << ID_TC2);
//             PM->APBCMASK.reg |= PM_APBCMASK_TCC0;
//             break;
// #endif
// #if TIMER_1_EN
//         case TIMER_1:
//             tim = TIMER_1_DEV;
//             // PMC->PMC_PCER0 = (1 << ID_TC3) | (1 << ID_TC4);
//             // PMC->PMC_PCER1 = (1 << (ID_TC5 - 32));
//             PM->APBCMASK.reg |= PM_APBCMASK_TCC1;
//             break;
// #endif
// #if TIMER_2_EN
//         case TIMER_2:
//             tim = TIMER_2_DEV;
//             // PMC->PMC_PCER1 = (1 << (ID_TC6 - 32)) | (1 << (ID_TC7 - 32)) | (1 << (ID_TC8 - 32));
//             PM->APBCMASK.reg = PM_APBCMASK_TCC2;
//             break;
// #endif
//         case TIMER_UNDEFINED:
//         default:
//             return -1;
//     }
//
//     /* save callback */
//     config[dev].cb = callback;
//
//     /* configure the timer block by connecting TIOA2 to XC0 and XC1 */
//     tim->TC_BMR = TC_BMR_TC0XC0S_TIOA2 | TC_BMR_TC1XC1S_TIOA2;
//
//     /* configure and enable channels 0 and 1 to use XC0 and XC1 as input */
//     tim->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0 | TC_CMR_WAVE | TC_CMR_EEVT_XC0;
//     tim->TC_CHANNEL[1].TC_CMR = TC_CMR_TCCLKS_XC1 | TC_CMR_WAVE | TC_CMR_EEVT_XC0;
//     tim->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;    /* and start */
//     tim->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;    /* and start */
//
//     /* configure channel 2:
//      * - select wave mode
//      * - set input clock to TIMER_CLOCK1 (MCK/2)
//      * - reload on TC_CV == TC_RC
//      * - let TIOA2 signal be toggled when TC_CV == TC_RC
//      */
//     tim->TC_CHANNEL[2].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE
//                               | TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPC_TOGGLE;
//
//     /* configure the frequency of channel 2 to 1us * ticks_per_us
//      *
//      * note: as channels 0 and 1 are only incremented on rising edges of TIOA2 line and
//      * channel 2 toggles this line on each timer tick, the actual frequency driving ch0/1
//      * is f_ch2 / 2 --> f_ch0/1 = (MCK / 2 / 2 / 1000000) * ticks_per_us.
//      */
//     tim->TC_CHANNEL[2].TC_RC = ((F_CPU / 1000000) / 4) * ticks_per_us;
//
//     /* start channel 2 */
//     tim->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
//
//     /* enable interrupts for given timer */
//     timer_irq_enable(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    return timer_set_absolute(dev, channel, timer_read(dev) + timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
//     Tc *tim;
//
//     /* get timer base register address */
//     switch (dev) {
// #if TIMER_0_EN
//         case TIMER_0:
//             tim = TIMER_0_DEV;
//             break;
// #endif
// #if TIMER_1_EN
//         case TIMER_1:
//             tim = TIMER_1_DEV;
//             break;
// #endif
// #if TIMER_2_EN
//         case TIMER_2:
//             tim = TIMER_2_DEV;
//             break;
// #endif
//         case TIMER_UNDEFINED:
//         default:
//             return -1;
//     }
//
//     /* set timeout value */
//     switch (channel) {
//         case 0:
//             tim->TC_CHANNEL[0].TC_RA = value;
//             tim->TC_CHANNEL[0].TC_IER = TC_IER_CPAS;
//             break;
//         case 1:
//             tim->TC_CHANNEL[0].TC_RB = value;
//             tim->TC_CHANNEL[0].TC_IER = TC_IER_CPBS;
//             break;
//         case 2:
//             tim->TC_CHANNEL[0].TC_RC = value;
//             tim->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
//             break;
//         case 3:
//             tim->TC_CHANNEL[1].TC_RA = value;
//             tim->TC_CHANNEL[1].TC_IER = TC_IER_CPAS;
//             break;
//         case 4:
//             tim->TC_CHANNEL[1].TC_RB = value;
//             tim->TC_CHANNEL[1].TC_IER = TC_IER_CPBS;
//             break;
//         case 5:
//             tim->TC_CHANNEL[1].TC_RC = value;
//             tim->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;
//             break;
//         default:
//             return -1;
//     }
//
    return 1;
}

int timer_clear(tim_t dev, int channel)
{
//     Tc *tim;
//
//     /* get timer base register address */
//     switch (dev) {
// #if TIMER_0_EN
//         case TIMER_0:
//             tim = TIMER_0_DEV;
//             break;
// #endif
// #if TIMER_1_EN
//         case TIMER_1:
//             tim = TIMER_1_DEV;
//             break;
// #endif
// #if TIMER_2_EN
//         case TIMER_2:
//             tim = TIMER_2_DEV;
//             break;
// #endif
//         case TIMER_UNDEFINED:
//         default:
//             return -1;
//     }
//
//     /* disable the channels interrupt */
//     switch (channel) {
//         case 0:
//             tim->TC_CHANNEL[0].TC_IDR = TC_IDR_CPAS;
//             break;
//         case 1:
//             tim->TC_CHANNEL[0].TC_IDR = TC_IDR_CPBS;
//             break;
//         case 2:
//             tim->TC_CHANNEL[0].TC_IDR = TC_IDR_CPCS;
//             break;
//         case 3:
//             tim->TC_CHANNEL[1].TC_IDR = TC_IDR_CPAS;
//             break;
//         case 4:
//             tim->TC_CHANNEL[1].TC_IDR = TC_IDR_CPBS;
//             break;
//         case 5:
//             tim->TC_CHANNEL[1].TC_IDR = TC_IDR_CPCS;
//             break;
//         default:
//             return -1;
//     }

    return 1;
}

/*
 * The timer channels 1 and 2 are configured to run with the same speed and
 * have the same value (they run in parallel), so only on of them is returned.
 */
unsigned int timer_read(tim_t dev)
{
//     switch (dev) {
// #if TIMER_0_EN
//         case TIMER_0:
//             return TIMER_0_DEV->TC_CHANNEL[0].TC_CV;
// #endif
// #if TIMER_1_EN
//         case TIMER_1:
//             return TIMER_1_DEV->TC_CHANNEL[0].TC_CV;
// #endif
// #if TIMER_2_EN
//         case TIMER_2:
//             return TIMER_2_DEV->TC_CHANNEL[0].TC_CV;
// #endif
//         case TIMER_UNDEFINED:
//         default:
//             return 0;
//     }
return 0;
}

/*
 * For stopping the counting of channels 1 + 2, channel 0 is disabled.
 */
void timer_stop(tim_t dev)
{
//     switch (dev) {
// #if TIMER_0_EN
//         case TIMER_0:
//             TIMER_0_DEV->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKDIS;
//             break;
// #endif
// #if TIMER_1_EN
//         case TIMER_1:
//             TIMER_1_DEV->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKDIS;
//             break;
// #endif
// #if TIMER_2_EN
//         case TIMER_2:
//             TIMER_2_DEV->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKDIS;
//             break;
// #endif
//         case TIMER_UNDEFINED:
//             break;
//     }
}

void timer_start(tim_t dev)
{
//     switch (dev) {
// #if TIMER_0_EN
//         case TIMER_0:
//             TIMER_0_DEV->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKEN;
//             break;
// #endif
// #if TIMER_1_EN
//         case TIMER_1:
//             TIMER_1_DEV->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKEN;
//             break;
// #endif
// #if TIMER_2_EN
//         case TIMER_2:
//             TIMER_2_DEV->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKEN;
//             break;
// #endif
//         case TIMER_UNDEFINED:
//             break;
//     }
}

void timer_irq_enable(tim_t dev)
{
//     switch (dev) {
// #if TIMER_0_EN
//         case TIMER_0:
//             NVIC_EnableIRQ(TC0_IRQn);
//             NVIC_EnableIRQ(TC1_IRQn);
//             break;
// #endif
// #if TIMER_1_EN
//         case TIMER_1:
//             NVIC_EnableIRQ(TC3_IRQn);
//             NVIC_EnableIRQ(TC4_IRQn);
//             break;
// #endif
// #if TIMER_2_EN
//         case TIMER_2:
//             NVIC_EnableIRQ(TC6_IRQn);
//             NVIC_EnableIRQ(TC7_IRQn);
//             break;
// #endif
//         case TIMER_UNDEFINED:
//             break;
//     }
}

void timer_irq_disable(tim_t dev)
{
//     switch (dev) {
// #if TIMER_0_EN
//         case TIMER_0:
//             NVIC_DisableIRQ(TC0_IRQn);
//             NVIC_DisableIRQ(TC1_IRQn);
//             break;
// #endif
// #if TIMER_1_EN
//         case TIMER_1:
//             NVIC_DisableIRQ(TC3_IRQn);
//             NVIC_DisableIRQ(TC4_IRQn);
//             break;
// #endif
// #if TIMER_2_EN
//         case TIMER_2:
//             NVIC_DisableIRQ(TC6_IRQn);
//             NVIC_DisableIRQ(TC7_IRQn);
//             break;
// #endif
//         case TIMER_UNDEFINED:
//             break;
//     }
}

void timer_reset(tim_t dev)
{
//     switch (dev) {
// #if TIMER_0_EN
//         case TIMER_0:
//             TIMER_0_DEV->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
//             TIMER_0_DEV->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG;
//             break;
// #endif
// #if TIMER_1_EN
//         case TIMER_1:
//             TIMER_1_DEV->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
//             TIMER_1_DEV->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG;
//             break;
// #endif
// #if TIMER_2_EN
//         case TIMER_2:
//             TIMER_2_DEV->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
//             TIMER_2_DEV->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG;
//             break;
// #endif
//         case TIMER_UNDEFINED:
//             break;
//     }
}


#if TIMER_0_EN
__attribute__ ((naked))
void TIMER_0_ISR1(void)
{
    ISR_ENTER();
    // uint32_t status = TIMER_0_DEV->TC_CHANNEL[0].TC_SR;
    // if (status & TC_SR_CPAS) {
    //     TIMER_0_DEV->TC_CHANNEL[0].TC_IDR = TC_IDR_CPAS;
    //     config[TIMER_0].cb(0);
    // }
    // else if (status & TC_SR_CPBS) {
    //     TIMER_0_DEV->TC_CHANNEL[0].TC_IDR = TC_IDR_CPBS;
    //     config[TIMER_0].cb(1);
    // }
    // else if (status & TC_SR_CPCS) {
    //     TIMER_0_DEV->TC_CHANNEL[0].TC_IDR = TC_IDR_CPCS;
    //     config[TIMER_0].cb(2);
    // }
    ISR_EXIT();
}

__attribute__ ((naked))
void TIMER_0_ISR2(void)
{
    ISR_ENTER();
    // uint32_t status = TIMER_0_DEV->TC_CHANNEL[1].TC_SR;
    // if (status & TC_SR_CPAS) {
    //     TIMER_0_DEV->TC_CHANNEL[1].TC_IDR = TC_IDR_CPAS;
    //     config[TIMER_0].cb(3);
    // }
    // else if (status & TC_SR_CPBS) {
    //     TIMER_0_DEV->TC_CHANNEL[1].TC_IDR = TC_IDR_CPBS;
    //     config[TIMER_0].cb(4);
    // }
    // else if (status & TC_SR_CPCS) {
    //     TIMER_0_DEV->TC_CHANNEL[1].TC_IDR = TC_IDR_CPCS;
    //     config[TIMER_0].cb(5);
    // }
    ISR_EXIT();
}
#endif /* TIMER_0_EN */


#if TIMER_1_EN
__attribute__ ((naked))
void TIMER_1_ISR1(void)
{
    ISR_ENTER();
    // uint32_t status = TIMER_1_DEV->TC_CHANNEL[0].TC_SR;
    // if (status & TC_SR_CPAS) {
    //     TIMER_1_DEV->TC_CHANNEL[0].TC_IDR = TC_IDR_CPAS;
    //     config[TIMER_1].cb(0);
    // }
    // if (status & TC_SR_CPBS) {
    //     TIMER_1_DEV->TC_CHANNEL[0].TC_IDR = TC_IDR_CPBS;
    //     config[TIMER_1].cb(1);
    // }
    // if (status & TC_SR_CPCS) {
    //     TIMER_1_DEV->TC_CHANNEL[0].TC_IDR = TC_IDR_CPCS;
    //     config[TIMER_1].cb(2);
    // }
    ISR_EXIT();
}

__attribute__ ((naked))
void TIMER_1_ISR2(void)
{
    ISR_ENTER();
    // uint32_t status = TIMER_1_DEV->TC_CHANNEL[1].TC_SR;
    // if (status & TC_SR_CPAS) {
    //     TIMER_1_DEV->TC_CHANNEL[1].TC_IDR = TC_IDR_CPAS;
    //     config[TIMER_1].cb(3);
    // }
    // if (status & TC_SR_CPBS) {
    //     TIMER_1_DEV->TC_CHANNEL[1].TC_IDR = TC_IDR_CPBS;
    //     config[TIMER_1].cb(4);
    // }
    // if (status & TC_SR_CPCS) {
    //     TIMER_1_DEV->TC_CHANNEL[1].TC_IDR = TC_IDR_CPCS;
    //     config[TIMER_1].cb(5);
    // }
    ISR_EXIT();
}
#endif /* TIMER_1_EN */


#if TIMER_2_EN
__attribute__ ((naked))
void TIMER_2_ISR1(void)
{
    ISR_ENTER();
    // uint32_t status = TIMER_2_DEV->TC_CHANNEL[0].TC_SR;
    // if (status & TC_SR_CPAS) {
    //     TIMER_2_DEV->TC_CHANNEL[0].TC_IDR = TC_IDR_CPAS;
    //     config[TIMER_2].cb(0);
    // }
    // else if (status & TC_SR_CPBS) {
    //     TIMER_2_DEV->TC_CHANNEL[0].TC_IDR = TC_IDR_CPBS;
    //     config[TIMER_2].cb(1);
    // }
    // else if (status & TC_SR_CPCS) {
    //     TIMER_2_DEV->TC_CHANNEL[0].TC_IDR = TC_IDR_CPCS;
    //     config[TIMER_2].cb(2);
    // }
    ISR_EXIT();
}

__attribute__ ((naked))
void TIMER_2_ISR2(void)
{
    ISR_ENTER();
    // uint32_t status = TIMER_2_DEV->TC_CHANNEL[1].TC_SR;
    // if (status & TC_SR_CPAS) {
    //     TIMER_2_DEV->TC_CHANNEL[1].TC_IDR = TC_IDR_CPAS;
    //     config[TIMER_2].cb(3);
    // }
    // else if (status & TC_SR_CPBS) {
    //     TIMER_2_DEV->TC_CHANNEL[1].TC_IDR = TC_IDR_CPBS;
    //     config[TIMER_2].cb(4);
    // }
    // else if (status & TC_SR_CPCS) {
    //     TIMER_2_DEV->TC_CHANNEL[1].TC_IDR = TC_IDR_CPCS;
    //     config[TIMER_2].cb(5);
    // }
    ISR_EXIT();
}
#endif /* TIMER_2_EN */
