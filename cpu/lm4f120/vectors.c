/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa#gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file        vectors.c
 * @brief       Interrupt vector definitions
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
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
/* LM4F120 specific interrupt vectors */
WEAK_DEFAULT void isr_gpio_porta(void);
WEAK_DEFAULT void isr_gpio_portb(void);
WEAK_DEFAULT void isr_gpio_portc(void);
WEAK_DEFAULT void isr_gpio_portd(void);
WEAK_DEFAULT void isr_gpio_porte(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_ssi0(void);
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
WEAK_DEFAULT void isr_comp0(void);
WEAK_DEFAULT void isr_comp1(void);
WEAK_DEFAULT void isr_sysctl(void);
WEAK_DEFAULT void isr_flashctl(void);
WEAK_DEFAULT void isr_gpio_portf(void);
WEAK_DEFAULT void isr_uart2(void);
WEAK_DEFAULT void isr_ssi1(void);
WEAK_DEFAULT void isr_timer3a(void);
WEAK_DEFAULT void isr_timer3b(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_can0(void);
WEAK_DEFAULT void isr_hibernate(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_udma_sw(void);
WEAK_DEFAULT void isr_udma_error(void);
WEAK_DEFAULT void isr_adc1_seq0(void);
WEAK_DEFAULT void isr_adc1_seq1(void);
WEAK_DEFAULT void isr_adc1_seq2(void);
WEAK_DEFAULT void isr_adc1_seq3(void);
WEAK_DEFAULT void isr_ssi2(void);
WEAK_DEFAULT void isr_ssi3(void);
WEAK_DEFAULT void isr_uart3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_uart6(void);
WEAK_DEFAULT void isr_uart7(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_i2c4(void);
WEAK_DEFAULT void isr_timer4a(void);
WEAK_DEFAULT void isr_timer4b(void);
WEAK_DEFAULT void isr_timer5a(void);
WEAK_DEFAULT void isr_timer5b(void);
WEAK_DEFAULT void isr_wtimer0a(void);
WEAK_DEFAULT void isr_wtimer0b(void);
WEAK_DEFAULT void isr_wtimer1a(void);
WEAK_DEFAULT void isr_wtimer1b(void);
WEAK_DEFAULT void isr_wtimer2a(void);
WEAK_DEFAULT void isr_wtimer2b(void);
WEAK_DEFAULT void isr_wtimer3a(void);
WEAK_DEFAULT void isr_wtimer3b(void);
WEAK_DEFAULT void isr_wtimer4a(void);
WEAK_DEFAULT void isr_wtimer4b(void);
WEAK_DEFAULT void isr_wtimer5a(void);
WEAK_DEFAULT void isr_wtimer5b(void);
WEAK_DEFAULT void isr_sysex(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) mem_manage_default,     /* memory manage exception */
    (void*) bus_fault_default,      /* bus fault exception */
    (void*) usage_fault_default,    /* usage fault exception */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) debug_mon_default,      /* debug monitor exception */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* Peripherial interrupts start here.*/
    (void *) isr_gpio_porta,        // GPIO Port A                      16
    (void *) isr_gpio_portb,        // GPIO Port B                      17
    (void *) isr_gpio_portc,        // GPIO Port C                      18
    (void *) isr_gpio_portd,        // GPIO Port D                      19
    (void *) isr_gpio_porte,        // GPIO Port E                      20
    (void *) isr_uart0,             // UART 0                           21
    (void *) isr_uart1,             // UART 1                           22
    (void *) isr_ssi0,              // SSI 0                            23
    (void *) isr_i2c0,              // I2C 0                            24
    (void *) (0UL),                 // Reserved                         25
    (void *) (0UL),                 // Reserved                         26
    (void *) (0UL),                 // Reserved                         27
    (void *) (0UL),                 // Reserved                         28
    (void *) (0UL),                 // Reserved                         29
    (void *) isr_adc0_seq0,         // ADC 0 Seq 0                      30
    (void *) isr_adc0_seq1,         // ADC 0 Seq 1                      31
    (void *) isr_adc0_seq2,         // ADC 0 Seq 2                      32
    (void *) isr_adc0_seq3,         // ADC 0 Seq 3                      33
    (void *) isr_wdt,               // WDT 0 and 1                      34
    (void *) isr_timer0a,           // 16/32 bit timer 0 A              35
    (void *) isr_timer0b,           // 16/32 bit timer 0 B              36
    (void *) isr_timer1a,           // 16/32 bit timer 1 A              37
    (void *) isr_timer1b,           // 16/32 bit timer 1 B              38
    (void *) isr_timer2a,           // 16/32 bit timer 2 A              39
    (void *) isr_timer2b,           // 16/32 bit timer 2 B              40
    (void *) isr_comp0,             // Analog comparator 0              41
    (void *) isr_comp1,             // Analog comparator 1              42
    (void *) (0UL),                 // Reserved                         43
    (void *) isr_sysctl,            // System control                   44
    (void *) isr_flashctl,          // Flash + EEPROM control           45
    (void *) isr_gpio_portf,        // GPIO Port F                      46
    (void *) (0UL),                 // Reserved                         47
    (void *) (0UL),                 // Reserved                         48
    (void *) isr_uart2,             // UART 2                           49
    (void *) isr_ssi2,              // SSI 1                            50
    (void *) isr_timer3a,           // 16/32 bit timer 3 A              51
    (void *) isr_timer3b,           // 16/32 bit timer 3 B              52
    (void *) isr_i2c1,              // I2C 1                            53
    (void *) (0UL),                 // Reserved                         54
    (void *) isr_can0,              // CAN 0                            55
    (void *) (0UL),                 // Reserved                         56
    (void *) (0UL),                 // Reserved                         57
    (void *) (0UL),                 // Reserved                         58
    (void *) isr_hibernate,         // Hibernation module               59
    (void *) isr_usb,               // USB                              60
    (void *) (0UL),                 // Reserved                         61
    (void *) isr_udma_sw,           // UDMA SW                          62
    (void *) isr_udma_error,        // UDMA Error                       63
    (void *) isr_adc1_seq0,         // ADC 1 Seq 0                      64
    (void *) isr_adc1_seq1,         // ADC 1 Seq 1                      65
    (void *) isr_adc1_seq2,         // ADC 1 Seq 2                      66
    (void *) isr_adc1_seq3,         // ADC 1 Seq 3                      67
    (void *) (0UL),                 // Reserved                         68
    (void *) (0UL),                 // Reserved                         69
    (void *) (0UL),                 // Reserved                         70
    (void *) (0UL),                 // Reserved                         71
    (void *) (0UL),                 // Reserved                         72
    (void *) isr_ssi2,              // SSI 2                            73
    (void *) isr_ssi3,              // SSI 3                            74
    (void *) isr_uart3,             // UART 3                           75
    (void *) isr_uart4,             // UART 4                           76
    (void *) isr_uart5,             // UART 5                           77
    (void *) isr_uart6,             // UART 6                           78
    (void *) isr_uart7,             // UART 7                           79
    (void *) (0UL),                 // Reserved                         80
    (void *) (0UL),                 // Reserved                         81
    (void *) (0UL),                 // Reserved                         82
    (void *) (0UL),                 // Reserved                         83
    (void *) isr_i2c2,              // I2C 2                            84
    (void *) isr_i2c4,              // I2C 4                            85
    (void *) isr_timer4a,           // 16/32 bit timer 4 A              86
    (void *) isr_timer4b,           // 16/32 bit timer 4 B              87
    (void *) (0UL),                 // Reserved                         88
    (void *) (0UL),                 // Reserved                         89
    (void *) (0UL),                 // Reserved                         90
    (void *) (0UL),                 // Reserved                         91
    (void *) (0UL),                 // Reserved                         92
    (void *) (0UL),                 // Reserved                         93
    (void *) (0UL),                 // Reserved                         94
    (void *) (0UL),                 // Reserved                         95
    (void *) (0UL),                 // Reserved                         96
    (void *) (0UL),                 // Reserved                         97
    (void *) (0UL),                 // Reserved                         98
    (void *) (0UL),                 // Reserved                         99
    (void *) (0UL),                 // Reserved                         100
    (void *) (0UL),                 // Reserved                         101
    (void *) (0UL),                 // Reserved                         102
    (void *) (0UL),                 // Reserved                         103
    (void *) (0UL),                 // Reserved                         104
    (void *) (0UL),                 // Reserved                         105
    (void *) (0UL),                 // Reserved                         106
    (void *) (0UL),                 // Reserved                         107
    (void *) isr_timer5a,           // 16/32 bit timer 5 A              108
    (void *) isr_timer5b,           // 16/32 bit timer 5 B              109
    (void *) isr_wtimer0a,          // 32/64 bit timer 0 A              110
    (void *) isr_wtimer0b,          // 32/64 bit timer 0 B              111
    (void *) isr_wtimer1a,          // 32/64 bit timer 1 A              112
    (void *) isr_wtimer1b,          // 32/64 bit timer 1 B              113
    (void *) isr_wtimer2a,          // 32/64 bit timer 2 A              114
    (void *) isr_wtimer2b,          // 32/64 bit timer 2 B              115
    (void *) isr_wtimer3a,          // 32/64 bit timer 3 A              116
    (void *) isr_wtimer3b,          // 32/64 bit timer 3 B              117
    (void *) isr_wtimer4a,          // 32/64 bit timer 4 A              118
    (void *) isr_wtimer4b,          // 32/64 bit timer 4 B              119
    (void *) isr_wtimer5a,          // 32/64 bit timer 5 A              120
    (void *) isr_wtimer5b,          // 32/64 bit timer 5 B              121
    (void *) isr_sysex,             // System Exception                 122
    (void *) (0UL),                 // Reserved                         123
    (void *) (0UL),                 // Reserved                         124
    (void *) (0UL),                 // Reserved                         125
    (void *) (0UL),                 // Reserved                         126
    (void *) (0UL),                 // Reserved                         127
    (void *) (0UL),                 // Reserved                         128
    (void *) (0UL),                 // Reserved                         129
    (void *) (0UL),                 // Reserved                         130
    (void *) (0UL),                 // Reserved                         131
    (void *) (0UL),                 // Reserved                         132
    (void *) (0UL),                 // Reserved                         133
    (void *) (0UL),                 // Reserved                         134
    (void *) (0UL),                 // Reserved                         135
    (void *) (0UL),                 // Reserved                         136
    (void *) (0UL),                 // Reserved                         137
    (void *) (0UL),                 // Reserved                         138
    (void *) (0UL),                 // Reserved                         139
    (void *) (0UL),                 // Reserved                         140
    (void *) (0UL),                 // Reserved                         141
    (void *) (0UL),                 // Reserved                         142
    (void *) (0UL),                 // Reserved                         143
    (void *) (0UL),                 // Reserved                         144
    (void *) (0UL),                 // Reserved                         145
    (void *) (0UL),                 // Reserved                         146
    (void *) (0UL),                 // Reserved                         147
    (void *) (0UL),                 // Reserved                         148
    (void *) (0UL),                 // Reserved                         149
    (void *) (0UL),                 // Reserved                         150
    (void *) (0UL),                 // Reserved                         151
    (void *) (0UL),                 // Reserved                         152
    (void *) (0UL),                 // Reserved                         153
    (void *) (0UL)                  // Reserved                         154
};
/** @} */
