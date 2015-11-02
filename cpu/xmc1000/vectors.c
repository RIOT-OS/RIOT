/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu_conf.h"
#include "vectors_cortexm.h"

#define CLKVAL1_SSW ((XMC_CLK_FDIV) |                   \
                     (XMC_CLK_IDIV << 8) |              \
                     (XMC_CLK_PCLKSEL << 16) |          \
                     (XMC_CLK_SSW_DISABLED << 31))

#define CLKVAL2_SSW ((XMC_GATING_CLEAR_VADC << SCU_CLK_CGATCLR0_VADC_Pos)   | \
                     (XMC_GATING_CLEAR_CCU40 << SCU_CLK_CGATCLR0_CCU40_Pos) | \
                     (XMC_GATING_CLEAR_USIC0 << SCU_CLK_CGATCLR0_USIC0_Pos) | \
                     (XMC_GATING_CLEAR_WDT << SCU_CLK_CGATCLR0_WDT_Pos) |     \
                     (XMC_GATING_CLEAR_RTC << SCU_CLK_CGATCLR0_RTC_Pos) |     \
                     (XMC_GATING_SSW_DISABLED << 31))

/* refers to the ldscript */
extern uint32_t _estack;

void dummy_handler(void)
{
    dummy_handler_default();
}

/* Cortex-M handlers */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);

/* XMC1100 peripheral handlers */
WEAK_DEFAULT void isr_scu0(void);
WEAK_DEFAULT void isr_scu1(void);
WEAK_DEFAULT void isr_scu2(void);

WEAK_DEFAULT void isr_eru0(void);
WEAK_DEFAULT void isr_eru1(void);
WEAK_DEFAULT void isr_eru2(void);
WEAK_DEFAULT void isr_eru3(void);

WEAK_DEFAULT void isr_usic0(void);
WEAK_DEFAULT void isr_usic1(void);
WEAK_DEFAULT void isr_usic2(void);
WEAK_DEFAULT void isr_usic3(void);
WEAK_DEFAULT void isr_usic4(void);
WEAK_DEFAULT void isr_usic5(void);

WEAK_DEFAULT void isr_vadc0(void);
WEAK_DEFAULT void isr_vadc1(void);

WEAK_DEFAULT void isr_ccu0(void);
WEAK_DEFAULT void isr_ccu1(void);
WEAK_DEFAULT void isr_ccu2(void);
WEAK_DEFAULT void isr_ccu3(void);

/* interrupt vector table */
ISR_VECTORS const void *isr_vector[] = {
    /* Stack pointer */
    (void *)(&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M handlers */
    (void *) reset_handler_default, /* entry point of the program */
    (void *) nmi_default,           /* non maskable interrupt */
    (void *) hard_fault_default,    /* do not end up here */
    (void *) CLKVAL1_SSW,           /* used for clock configuration */
    (void *) CLKVAL2_SSW            /* used for gating configuration */
};

__attribute__((naked, used, section(".veneers"))) void veneer_jumps(void)
{
    asm volatile (
        ".fill 0x0c                   \n"   /* 12 bytes offset */

        "ldr r0, =hard_fault_default  \n"   /* hard fault */
        "mov pc, r0                   \n"
        ".fill 0x1c                   \n"   /* 7 slots unused */

        "ldr r0, =isr_svc             \n"   /* supervisor call */
        "mov pc, r0                   \n"
        ".fill 0x08                   \n"   /* 2 slots unused */

        "ldr r0, =isr_pendsv          \n"   /* used for context switching */
        "mov pc, r0                   \n"
        "ldr r0, =isr_systick         \n"   /* system tick, not used in RIOT */
        "mov pc, r0                   \n"
        "ldr r0, =isr_scu0            \n"   /* system control unit */
        "mov pc, r0                   \n"
        "ldr r0, =isr_scu1            \n"   /* system control unit */
        "mov pc, r0                   \n"
        "ldr r0, =isr_scu2            \n"   /* system control unit */
        "mov pc, r0                   \n"
        "ldr r0, =isr_eru0            \n"   /* event request unit */
        "mov pc, r0                   \n"
        "ldr r0, =isr_eru1            \n"   /* event request unit */
        "mov pc, r0                   \n"
        "ldr r0, =isr_eru2            \n"   /* event request unit */
        "mov pc, r0                   \n"
        "ldr r0, =isr_eru3            \n"   /* event request unit */
        "mov pc, r0                   \n"
        ".fill 0x08                   \n"   /* 2 slots unused */

        "ldr r0, =isr_usic0           \n"   /* universal serial interface channel */
        "mov pc, r0                   \n"
        "ldr r0, =isr_usic1           \n"   /* universal serial interface channel */
        "mov pc, r0                   \n"
        "ldr r0, =isr_usic2           \n"   /* universal serial interface channel */
        "mov pc, r0                   \n"
        "ldr r0, =isr_usic3           \n"   /* universal serial interface channel */
        "mov pc, r0                   \n"
        "ldr r0, =isr_usic4           \n"   /* universal serial interface channel */
        "mov pc, r0                   \n"
        "ldr r0, =isr_usic5           \n"   /* universal serial interface channel */
        "mov pc, r0                   \n"
        "ldr r0, =isr_vadc0           \n"   /* analog-to-digital converter */
        "mov pc, r0                   \n"
        "ldr r0, =isr_vadc1           \n"   /* analog-to-digital converter */
        "mov pc, r0                   \n"
        ".fill 0x10                   \n"   /* 4 slots unused */
        "ldr r0, =isr_ccu0            \n"   /* capture/compare unit 4 */
        "mov pc, r0                   \n"
        "ldr r0, =isr_ccu1            \n"   /* capture/compare unit 4 */
        "mov pc, r0                   \n"
        "ldr r0, =isr_ccu2            \n"   /* capture/compare unit 4 */
        "mov pc, r0                   \n"
        "ldr r0, =isr_ccu3            \n"   /* capture/compare unit 4 */
        "mov pc, r0                   \n"
        ".fill 0x1c                   \n"   /* 7 slots unused */
        );
}
