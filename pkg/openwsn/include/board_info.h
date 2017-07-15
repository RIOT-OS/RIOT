/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 *
 * @}
 */

#ifndef __BOARD_INFO_H
#define __BOARD_INFO_H

#include "stdint.h"
#include "string.h"
#include "cpu.h"

//=========================== defines =========================================

#define INTERRUPT_DECLARATION(); //no declaration
#define DISABLE_INTERRUPTS()    irq_disable();
#define ENABLE_INTERRUPTS()     irq_enable();

//===== timer

#define PORT_TIMER_WIDTH                    uint32_t
#define PORT_RADIOTIMER_WIDTH               uint32_t

#define PORT_SIGNED_INT_WIDTH               int32_t
#define PORT_TICS_PER_MS                    32
#define SCHEDULER_WAKEUP()                  //EXTI->SWIER |= EXTI_Line1;
#define SCHEDULER_ENABLE_INTERRUPT()        //enable in board use EXTI_Line1

//===== pinout

// [PA.2] radio SLP_TR_CNTL
#define PORT_PIN_RADIO_SLP_TR_CNTL_HIGH()       //GPIOA->ODR |= (1<<2);
#define PORT_PIN_RADIO_SLP_TR_CNTL_LOW()        //GPIOA->ODR &= ~(1<<2);
// radio reset line
// radio /RST
#define PORT_PIN_RADIO_RESET_HIGH()         //GPIOC->ODR |= 0X0040;// nothing
#define PORT_PIN_RADIO_RESET_LOW()          //GPIOC->ODR &= ~0X0040;// nothing
//#define PORT_PIN_RADIO_RESET_LOW()            GPIOC->ODR &= ~(1<<1);

// TODO: CHECK TIMINGS!
//===== IEEE802154E timing
// time-slot related
#define PORT_TsSlotDuration                 491     // counter counts one extra count, see datasheet
// execution speed related
#define PORT_maxTxDataPrepare               66      // 2014us (measured 746us)
#define PORT_maxRxAckPrepare                20      //  305us (measured  83us)
#define PORT_maxRxDataPrepare               33      // 1007us (measured  84us)
#define PORT_maxTxAckPrepare                30      //  305us (measured 219us)
// radio speed related
#define PORT_delayTx                        10      //  214us (measured 219us)
#define PORT_delayRx                        0       //    0us (can not measure)
// radio watchdog

//===== adaptive_sync accuracy

#define SYNC_ACCURACY                       2     // ticks

//=========================== variables =======================================

static const uint8_t rreg_uriquery[] = "h=ucb";
static const uint8_t infoBoardname[] = "riot-os";
static const uint8_t infouCName[] = "various";
static const uint8_t infoRadioName[] = "riot-netdev";

//=========================== prototypes ======================================

//=========================== public ==========================================

//=========================== private =========================================

#endif
