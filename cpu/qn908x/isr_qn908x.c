/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @{
 *
 * @file
 * @brief       Default interrupt service routine definitions for NXP QN908x
 *
 * These weak default definitions act as a fallback definition if no driver
 * defines a ISR for the specific interrupt.
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "vectors_cortexm.h"
#include "vectors_qn908x.h"

/* These are defined in vectors_cortexm.c. */
extern void reset_handler_default(void);
extern uint32_t _estack; /* Exception stack pointer. */

/**
 * @brief Jump to the reset_handle_default handler with the exception stack.
 */
__attribute__((noreturn)) static inline void cpu_restart(void)
{
    /* Reset the stack pointer to the beginning again and jump to the reset
     * handler at the expected address.
     */
    __asm volatile ("msr msp, %[estack]\n"
                    "mov pc, %[entry]\n"
                    :
                    : [ estack ] "r" (&_estack),
                    [ entry ] "r" (reset_handler_default)
                    : "memory");

    /* This function doesn't return anyway. */
    while (1) {}
}

/**
 * @brief Remap the flash to address 0 on start.
 *
 * The bootloader will jump to the flash at address 0x21000000 which is aliased
 * to the flash on this CPU. However, our program is linked to run as if the
 * flash is mapped at address 0 which is the common case. The range starting at
 * address 0 can be mapped to flash, RAM or ROM via the SYS_MODE_CTRL register,
 * but on reset the default value (0x0) means that it is mapped to ROM.
 * We need to remap the flash and change the program counter to be running from
 * the right address range (0).
 */
void pre_startup(void)
{
    register unsigned int pc;

    /* Disable interrupts */
    __disable_irq();

    /* Check whether we are running from the 0x21000000 range. If that's the
     * case we need to remap the flash to the address 0 in SYS_MODE_CTRL and
     * jump back to the reset_handler_default so everything starts as running
     * from the address 0x0 instead.
     */
    __asm volatile ("mov %0, pc" : "=r" (pc));

    if ((pc & 0x21000000) == 0x21000000) {
        SYSCON->SYS_MODE_CTRL |= 1;
        cpu_restart();
    }
}

/* QN908x interrupt service routines */
WEAK_DEFAULT void isr_ext_gpio_wakeup(void);
WEAK_DEFAULT void isr_osc(void);
WEAK_DEFAULT void isr_acmp0(void);
WEAK_DEFAULT void isr_acmp1(void);
WEAK_DEFAULT void isr_rtc_sec(void);
WEAK_DEFAULT void isr_rtc_fr(void);
WEAK_DEFAULT void isr_cs_wakeup(void);
WEAK_DEFAULT void isr_cs(void);
WEAK_DEFAULT void isr_gpioa(void);
WEAK_DEFAULT void isr_gpiob(void);
WEAK_DEFAULT void isr_dma0(void);
WEAK_DEFAULT void isr_pin_int0(void);
WEAK_DEFAULT void isr_pin_int1(void);
WEAK_DEFAULT void isr_pin_int2(void);
WEAK_DEFAULT void isr_pin_int3(void);
WEAK_DEFAULT void isr_osc_int_low(void);
WEAK_DEFAULT void isr_usb0(void);
WEAK_DEFAULT void isr_flexcomm0(void);
WEAK_DEFAULT void isr_flexcomm1(void);
WEAK_DEFAULT void isr_flexcomm2(void);
WEAK_DEFAULT void isr_flexcomm3(void);
WEAK_DEFAULT void isr_ble(void);
WEAK_DEFAULT void isr_fsp(void);
WEAK_DEFAULT void isr_qdec0(void);
WEAK_DEFAULT void isr_qdec1(void);
WEAK_DEFAULT void isr_ctimer0(void);
WEAK_DEFAULT void isr_ctimer1(void);
WEAK_DEFAULT void isr_ctimer2(void);
WEAK_DEFAULT void isr_ctimer3(void);
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_dac(void);
WEAK_DEFAULT void isr_xtal_ready(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_spifi0(void);
WEAK_DEFAULT void isr_sct0(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_calib(void);
WEAK_DEFAULT void isr_ble_tx(void);
WEAK_DEFAULT void isr_ble_rx(void);
WEAK_DEFAULT void isr_ble_freq_hop(void);
WEAK_DEFAULT void isr_bod(void);

void dummy_handler(void)
{
    dummy_handler_default();
}
