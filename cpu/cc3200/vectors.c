/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file        vectors.c
 * @brief       Interrupt vector definitions
 *
 * @author      Attilio Dona'
 */

#include <stdint.h>
#include "vectors_cortexm.h"

/* get the start of the ISR stack as defined in the linkerscript */
extern uint32_t _estack;

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);

/* CC3200 specific interrupt vectors */
WEAK_DEFAULT void isr_gpio_porta0(void);
WEAK_DEFAULT void isr_gpio_porta1(void);
WEAK_DEFAULT void isr_gpio_porta2(void);
WEAK_DEFAULT void isr_gpio_porta3(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_adc0_seq0(void);
WEAK_DEFAULT void isr_adc0_seq1(void);
WEAK_DEFAULT void isr_adc0_seq2(void);
WEAK_DEFAULT void isr_adc0_seq3(void);
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_timer0a(void);
WEAK_DEFAULT void isr_timer0b(void);
WEAK_DEFAULT void isr_timer1a(void);
WEAK_DEFAULT void isr_timer1b(void);
WEAK_DEFAULT void isr_timer2a(void);
WEAK_DEFAULT void isr_timer2b(void);
WEAK_DEFAULT void isr_flashctl(void);
WEAK_DEFAULT void isr_timer3a(void);
WEAK_DEFAULT void isr_timer3b(void);
WEAK_DEFAULT void isr_udma_sw(void);
WEAK_DEFAULT void isr_udma_error(void);
WEAK_DEFAULT void isr_sha(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_des(void);
WEAK_DEFAULT void isr_sdhost(void);
WEAK_DEFAULT void isr_i2s(void);
WEAK_DEFAULT void isr_camera(void);
WEAK_DEFAULT void isr_shared_spi(void);
WEAK_DEFAULT void isr_generic_spi(void);
WEAK_DEFAULT void isr_link_spi(void);
WEAK_DEFAULT void isr_nwp(void);
WEAK_DEFAULT void isr_prcm(void);

//*****************************************************************************
//
// The vector table.
//
//*****************************************************************************
ISR_VECTORS const void *interrupt_vector[] = {
        (void*) (&_estack),
        reset_handler_default, // The reset handler
        nmi_default,           // The NMI handler
        hard_fault_default,    // The hard fault handler
        mem_manage_default,    // The memory manage fault handler
        bus_fault_default,     // The bus fault handler
        usage_fault_default,   // The usage fault handler
        0,                     // Reserved
        0,                     // Reserved
        0,                     // Reserved
        0,                     // Reserved
        isr_svc,               /* system call interrupt, in RIOT used for
                                * switching into thread context on boot */
        debug_mon_default,     // Debug monitor handler
        0,                     // Reserved
        isr_pendsv,            /* pendSV interrupt, in RIOT the actual
                                * context switching is happening here */
        isr_systick,           // SysTick interrupt, not used in RIOT
        isr_gpio_porta0,       // GPIO Port A0
        isr_gpio_porta1,       // GPIO Port A1
        isr_gpio_porta2,       // GPIO Port A2
        isr_gpio_porta3,       // GPIO Port A3
        0,                     // Reserved
        isr_uart0,             // UART0 Rx and Tx
        isr_uart1,             // UART1 Rx and Tx
        0,                     // Reserved
        isr_i2c0,              // I2C0 Master and Slave
        0, 0, 0, 0, 0,         // Reserved
        isr_adc0_seq0,         // ADC Channel 0
        isr_adc0_seq1,         // ADC Channel 1
        isr_adc0_seq2,         // ADC Channel 2
        isr_adc0_seq3,         // ADC Channel 3
        isr_wdt,               // Watchdog Timer
        isr_timer0a,           // Timer 0 subtimer A  35*4 = 0x8C
        isr_timer0b,           // Timer 0 subtimer B
        isr_timer1a,           // Timer 1 subtimer A
        isr_timer1b,           // Timer 1 subtimer B
        isr_timer2a,           // Timer 2 subtimer A
        isr_timer2b,           // Timer 2 subtimer B
        0, 0, 0, 0,            // Reserved
        isr_flashctl,          // Flash
        0, 0, 0, 0, 0,         // Reserved
        isr_timer3a,           // Timer 3 subtimer A
        isr_timer3b,           // Timer 3 subtimer B
        0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        isr_udma_sw,           // uDMA Software Transfer
        isr_udma_error,        // uDMA Error
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Reserved
        isr_sha,               // SHA
        0, 0,                  // Reserved
        isr_aes,               // AES
        0,                     // Reserved
        isr_des,               // DES
        0, 0, 0, 0, 0,         // Reserved
        isr_sdhost,            // SDHost
        0,                     // Reserved
        isr_i2s,               // I2S
        0,                     // Reserved
        isr_camera,            // Camera
        0, 0, 0, 0, 0, 0, 0,   // Reserved
        isr_nwp,               // NWP to APPS Interrupt
        isr_prcm,              // Power, Reset and Clock module
        0, 0,                  // Reserved
        isr_shared_spi,        // Shared SPI
        isr_generic_spi,       // Generic SPI
        isr_link_spi,          // Link SPI
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Reserved
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Reserved
        0, 0                            // Reserved
        };

