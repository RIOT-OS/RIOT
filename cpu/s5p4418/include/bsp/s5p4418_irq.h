/*
 * Copyright (C) 2017 PDTECTH Co., LTD
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author      Phuong Dang <kamejoko80@yahoo.com>
 */

#ifndef __S5P4418_IRQ_H__
#define __S5P4418_IRQ_H__

#include <stdint.h>
#include <stdbool.h>
#include "s5p4418_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_IRQ_NUMBER        (59)

#define IRQ_PRIORITY_LEVEL_0  (0)
#define IRQ_PRIORITY_LEVEL_1  (1)
#define IRQ_PRIORITY_LEVEL_2  (2)
#define IRQ_PRIORITY_LEVEL_3  (3)
#define IRQ_PRIORITY_LEVEL_4  (4)
#define IRQ_PRIORITY_LEVEL_5  (5)
#define IRQ_PRIORITY_LEVEL_6  (6)
#define IRQ_PRIORITY_LEVEL_7  (7)
#define IRQ_PRIORITY_LEVEL_8  (8)
#define IRQ_PRIORITY_LEVEL_9  (9)
#define IRQ_PRIORITY_LEVEL_10 (10)
#define IRQ_PRIORITY_LEVEL_11 (11)
#define IRQ_PRIORITY_LEVEL_12 (12)
#define IRQ_PRIORITY_LEVEL_13 (13)
#define IRQ_PRIORITY_LEVEL_14 (14)
#define IRQ_PRIORITY_LEVEL_15 (15)

/**
 * @brief IRQ info
 */
typedef struct {
    const char *name;       /**< irq type name */
    void  (*handler)(void); /**< irq handler   */
}irq_typedef;

/**
 * @brief NVIC register set
 */
typedef struct {
    volatile uint32_t IRQSTATUS;           /**< 0x000  IRQ Status Register                      */
    volatile uint32_t FIQSTATUS;           /**< 0x004  FIQ Status Register                      */
    volatile uint32_t RAWINTR;             /**< 0x008  RAW Interrupt Status Register            */
    volatile uint32_t INTSELECT;           /**< 0x00C  Interrupt Select Register                */
    volatile uint32_t INTENABLE;           /**< 0x010  Interrupt Enable Register                */
    volatile uint32_t INTENCLEAR;          /**< 0x014  Interrupt Enable Clear Register          */
    volatile uint32_t SOFTINT;             /**< 0x018  Software Interrupt Register              */
    volatile uint32_t SOFTINTCLEAR;        /**< 0x01C  Software Interrupt Clear Register        */
    volatile uint32_t PROTECTION;          /**< 0x020  Protection Enable Register               */
    volatile uint32_t SWPRIORITYMASK;      /**< 0x024  Software Priority Mask Register          */
    volatile uint32_t VECTPRIORITYDAISY;   /**< 0x028  Vector Priority Register For Daisy Chain */
    volatile uint32_t Reserved0[53];       /**< 0x02C  Reserved0  n = (T - H)/4 -1 = 53         */
    volatile uint32_t VECTADDR[32];        /**< 0x100  Vector Address 0 Registers               */
    volatile uint32_t Reserved1[32];       /**< 0x180  Reserved1  n = (T - H)/4 - 1 = 32        */
    volatile uint32_t VICVECTPRIORITY[32]; /**< 0x100  Vector Priority Register                 */
    volatile uint32_t Reserved2[800];      /**< 0x280  Reserved2  n = (T - H)/4 - 1 = 800       */
    volatile uint32_t VICADDRESS;          /**< 0xF00  Vector Address Register                  */
    volatile uint32_t Reserved3[55];       /**< 0xF04  Reserved3  n = (T - H)/4 - 1 = 55        */
    volatile uint32_t VICPERIPHID[4];      /**< 0xFE0  Peripheral Identification Register       */
    volatile uint32_t VICPCELLID[4];       /**< 0xFF0  Prime cell Identification Register       */
}vic_typedef;

/**
 * @brief VIC channel Base address
 */
#define VIC0   ((vic_typedef *)S5P4418_VIC0_BASE)
#define VIC1   ((vic_typedef *)S5P4418_VIC1_BASE)

void  s5p4418_irq_init(void);
bool  s5p4418_irq_register(void(*cbfunc)(void), uint8_t irq, uint8_t priority);
void  s5p4418_irq_unregister(uint8_t irq);
void  s5p4418_irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif
