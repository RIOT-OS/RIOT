//------------------------------------------------------------------------------
//
//  Copyright (C) 2017
//
//  Module      : TIMER
//  File        : s5p4418_timer.h
//  Description : S5P4418 TIMER FUNCTIONS
//  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)
//  History     :
//------------------------------------------------------------------------------

#ifndef __S5P4418_TIMER_H__
#define __S5P4418_TIMER_H__

#include <stdint.h>
#include <stdbool.h>
#include "s5p4418_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_TIMER_CHANNEL_INDEX (3)

#define TIMER_CH0 (0)
#define TIMER_CH1 (1)
#define TIMER_CH2 (2)
#define TIMER_CH3 (3)

typedef struct
{
    volatile uint32_t TCFG0;      // 0x00   // Clock-Prescalar and dead-zone configurations
    volatile uint32_t TCFG1;      // 0x04   // Clock multiplexers and DMA mode select
    volatile uint32_t TCON;       // 0x08   // Timer control register
    volatile uint32_t TCNTB0;     // 0x0C   // Timer 0 count buffer register
    volatile uint32_t TCMPB0;     // 0x10   // Timer 0 compare buffer register
    volatile uint32_t TCNTO0;     // 0x14   // Timer 0 count observation register
    volatile uint32_t TCNTB1;     // 0x18   // Timer 1 count buffer register
    volatile uint32_t TCMPB1;     // 0x1C   // Timer 1 compare buffer register
    volatile uint32_t TCNTO1;     // 0x20   // Timer 1 count observation register
    volatile uint32_t TCNTB2;     // 0x24   // Timer 2 count buffer register
    volatile uint32_t TCMPB2;     // 0x28   // Timer 2 compare buffer register
    volatile uint32_t TCNTO2;     // 0x2C   // Timer 2 count observation register
    volatile uint32_t TCNTB3;     // 0x30   // Timer 3 count buffer register
    volatile uint32_t TCMPB3;     // 0x34   // Timer 3 compare buffer register
    volatile uint32_t TCNTO3;     // 0x38   // Timer 3 count observation register
    volatile uint32_t TCNTB4;     // 0x3C   // Timer 4 count buffer register
    volatile uint32_t TCNTO4;     // 0x40   // Timer 4 count observation register
    volatile uint32_t TINT_CSTAT; // 0x44   // Timer interrupt control and status register
}timer_typedef;

/* Timer base address */
#define TIMER ((timer_typedef *)S5P4418_TIMER_BASE)

void     s5p4418_timer_init(void);
void     s5p4418_timer_setup(uint8_t ch, uint32_t t_us, bool irq_on);
void     s5p4418_timer_start(uint8_t ch, bool auto_reload);
void     s5p4418_timer_stop(uint8_t ch);
uint32_t s5p4418_timer_get_cnt(uint8_t ch);
uint32_t s5p4418_timer_get_tick(uint8_t ch);
void     s5p4418_timer_set_cnt(uint8_t ch, uint32_t cnt);
bool     s5p4418_timer_get_irq_flag(uint8_t ch);
void     s5p4418_timer_clear_irq_flag(uint8_t ch);
uint8_t  s5p4418_timer_get_irq_number(uint8_t ch);
bool     s5p4418_timer_register_irq(uint8_t ch, void(*cbfunc)(void), uint8_t priority);
void     s5p4418_timer_unregister_irq(uint8_t ch);
#ifdef __cplusplus
}
#endif


#endif // __S5P4418_IRQ_H__
