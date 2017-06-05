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
/* tm4c1294 specific interrupt vectors */
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
WEAK_DEFAULT void isr_fpu(void);
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
    (void*) dummy_handler,                  /* Reserved */
    (void*) dummy_handler,                  /* Reserved */
    (void*) dummy_handler,                  /* Reserved */
    (void*) dummy_handler,                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) debug_mon_default,      /* debug monitor exception */
    (void*) dummy_handler,                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* Peripherial interrupts start here.*/
    (void *) isr_gpio_porta,        /* GPIO Port A                      16 */
    (void *) isr_gpio_portb,        /* GPIO Port B                      17 */
    (void *) isr_gpio_portc,        /* GPIO Port C                      18 */
    (void *) isr_gpio_portd,        /* GPIO Port D                      19 */
    (void *) isr_gpio_porte,        /* GPIO Port E                      20 */
    (void *) isr_uart0,             /* UART 0                           21 */
    (void *) isr_uart1,             /* UART 1                           22 */
    (void *) isr_ssi0,              /* SSI 0                            23 */
    (void *) isr_i2c0,              /* I2C 0                            24 */
    (void *) dummy_handler,                 /* PWM Fault                        25 */
    (void *) dummy_handler,                 /* PWM Generator 0                  26 */
    (void *) dummy_handler,                 /* PWM Generator 1                  27 */
    (void *) dummy_handler,                 /* PWM Generator 2                  28 */
    (void *) dummy_handler,                 /* Quadrature Encoder 0             29 */
    (void *) isr_adc0_seq0,         /* ADC 0 Seq 0                      30 */
    (void *) isr_adc0_seq1,         /* ADC 0 Seq 1                      31 */
    (void *) isr_adc0_seq2,         /* ADC 0 Seq 2                      32 */
    (void *) isr_adc0_seq3,         /* ADC 0 Seq 3                      33 */
    (void *) isr_wdt,               /* WDT 0 and 1                      34 */
    (void *) isr_timer0a,           /* 16/32 bit timer 0 A              35 */
    (void *) isr_timer0b,           /* 16/32 bit timer 0 B              36 */
    (void *) isr_timer1a,           /* 16/32 bit timer 1 A              37 */
    (void *) isr_timer1b,           /* 16/32 bit timer 1 B              38 */
    (void *) isr_timer2a,           /* 16/32 bit timer 2 A              39 */
    (void *) isr_timer2b,           /* 16/32 bit timer 2 B              40 */
    (void *) isr_comp0,             /* Analog comparator 0              41 */
    (void *) isr_comp1,             /* Analog comparator 1              42 */
    (void *) dummy_handler,                 /* Analog comparator 2              43 */
    (void *) isr_sysctl,            /* System control                   44 */
    (void *) isr_flashctl,          /* Flash + EEPROM control           45 */
    (void *) isr_gpio_portf,        /* GPIO Port F                      46 */
    (void *) dummy_handler,                 /* GPIO Port G                      47 */
    (void *) dummy_handler,                 /* GPIO Port H                      48 */
    (void *) isr_uart2,             /* UART 2                           49 */
    (void *) isr_ssi2,              /* SSI 1                            50 */
    (void *) isr_timer3a,           /* 16/32 bit timer 3 A              51 */
    (void *) isr_timer3b,           /* 16/32 bit timer 3 B              52 */
    (void *) isr_i2c1,              /* I2C 1                            53 */
    (void *) dummy_handler,                 /* Quadrature Encoder 1             54 */
    (void *) isr_can0,              /* CAN 0                            55 */
    (void *) dummy_handler,                 /* CAN 1                         56 */
    (void *) dummy_handler,                 /* Ethernet                57 */
    (void *) dummy_handler,                 /* Reserved                         58 */
    (void *) isr_hibernate,         /* Hibernation module               59 */
    (void *) isr_usb,               /* USB0                              60 */
    (void *) dummy_handler,                 /* PWM Generator 3                          61 */
    (void *) isr_udma_sw,           /* UDMA SW Transfer                         62 */
    (void *) isr_udma_error,        /* UDMA Error                       63 */
    (void *) isr_adc1_seq0,         /* ADC 1 Seq 0                      64 */
    (void *) isr_adc1_seq1,         /* ADC 1 Seq 1                      65 */
    (void *) isr_adc1_seq2,         /* ADC 1 Seq 2                      66 */
    (void *) isr_adc1_seq3,         /* ADC 1 Seq 3                      67 */
    (void *) dummy_handler,                 /* Reserved                         68 */
    (void *) dummy_handler,                 /* Reserved                         69 */
    (void *) dummy_handler,                 /* GPIO Port J                         70 */
    (void *) dummy_handler,                 /* GPIO Port K                         71 */
    (void *) dummy_handler,                 /* GPIO Port L                         72 */
    (void *) isr_ssi2,              /* SSI 2                            73 */
    (void *) isr_ssi3,              /* SSI 3                            74 */
    (void *) isr_uart3,             /* UART 3                           75 */
    (void *) isr_uart4,             /* UART 4                           76 */
    (void *) isr_uart5,             /* UART 5                           77 */
    (void *) isr_uart6,             /* UART 6                           78 */
    (void *) isr_uart7,             /* UART 7                           79 */
    (void *) dummy_handler,                 /* Reserved                         80 */
    (void *) dummy_handler,                 /* Reserved                         81 */
    (void *) dummy_handler,                 /* Reserved                         82 */
    (void *) dummy_handler,                 /* Reserved                         83 */
    (void *) isr_i2c2,              /* I2C 2                            84 */
    (void *) isr_i2c4,              /* I2C 4                            85 */
    (void *) isr_timer4a,           /* 16/32 bit timer 4 A              86 */
    (void *) isr_timer4b,           /* 16/32 bit timer 4 B              87 */
    (void *) dummy_handler,                 /* Reserved                         88 */
    (void *) dummy_handler,                 /* Reserved                         89 */
    (void *) dummy_handler,                 /* Reserved                         90 */
    (void *) dummy_handler,                 /* Reserved                         91 */
    (void *) dummy_handler,                 /* Reserved                         92 */
    (void *) dummy_handler,                 /* Reserved                         93 */
    (void *) dummy_handler,                 /* Reserved                         94 */
    (void *) dummy_handler,                 /* Reserved                         95 */
    (void *) dummy_handler,                 /* Reserved                         96 */
    (void *) dummy_handler,                 /* Reserved                         97 */
    (void *) dummy_handler,                 /* Reserved                         98 */
    (void *) dummy_handler,                 /* Reserved                         99 */
    (void *) dummy_handler,                 /* Reserved                         100 */
    (void *) dummy_handler,                 /* Reserved                         101 */
    (void *) dummy_handler,                 /* Reserved                         102 */
    (void *) dummy_handler,                 /* Reserved                         103 */
    (void *) dummy_handler,                 /* Reserved                         104 */
    (void *) dummy_handler,                 /* Reserved                         105 */
    (void *) dummy_handler,                 /* Reserved                         106 */
    (void *) dummy_handler,                 /* Reserved                         107 */
    (void *) isr_timer5a,           /* 16/32 bit timer 5 A              108 */
    (void *) isr_timer5b,           /* 16/32 bit timer 5 B              109 */
    (void *) isr_wtimer0a,          /* 32/64 bit timer 0 A              110 */
    (void *) isr_wtimer0b,          /* 32/64 bit timer 0 B              111 */
    (void *) isr_wtimer1a,          /* 32/64 bit timer 1 A              112 */
    (void *) isr_wtimer1b,          /* 32/64 bit timer 1 B              113 */
    (void *) isr_wtimer2a,          /* 32/64 bit timer 2 A              114 */
    (void *) isr_wtimer2b,          /* 32/64 bit timer 2 B              115 */
    (void *) isr_wtimer3a,          /* 32/64 bit timer 3 A              116 */
    (void *) isr_wtimer3b,          /* 32/64 bit timer 3 B              117 */
    (void *) isr_wtimer4a,          /* 32/64 bit timer 4 A              118 */
    (void *) isr_wtimer4b,          /* 32/64 bit timer 4 B              119 */
    (void *) isr_wtimer5a,          /* 32/64 bit timer 5 A              120 */
    (void *) isr_wtimer5b,          /* 32/64 bit timer 5 B              121 */
    (void *) isr_fpu,               /* FPU                              122 */
    (void *) dummy_handler,                 /* PECI 0                         123 */
    (void *) dummy_handler,                 /* LPC 0                         124 */
    (void *) dummy_handler,                 /* I2C 4 Master and Slave           125 */
    (void *) dummy_handler,                 /* I2C 5 Master and Slave           126 */
    (void *) dummy_handler,                 /* GPIO Port M                        127 */
    (void *) dummy_handler,                 /* GPIO Port N                      128 */
    (void *) dummy_handler,                 /* Quadrature Encoder 2                         129 */
    (void *) dummy_handler,                 /* Fan 0                         130 */
    (void *) dummy_handler,                 /* Reserved                         131 */
    (void *) dummy_handler,                 /* GPIO Port P(Summary or 0)                         132 */
    (void *) dummy_handler,                 /* GPIO Port P1                         133 */
    (void *) dummy_handler,                 /* GPIO Port P2                        134 */
    (void *) dummy_handler,                 /* GPIO Port P3                         135 */
    (void *) dummy_handler,                 /* GPIO Port P4                         136 */
    (void *) dummy_handler,                 /* GPIO Port P5                         137 */
    (void *) dummy_handler,                 /* GPIO Port P6                         138 */
    (void *) dummy_handler,                 /* GPIO Port P7                         139 */
    (void *) dummy_handler,                 /* GPIO Port Q(Summary or 0)                         140 */
    (void *) dummy_handler,                 /* GPIO Port Q1                        141 */
    (void *) dummy_handler,                 /* GPIO Port Q2                         142 */
    (void *) dummy_handler,                 /* GPIO Port Q3                         143 */
    (void *) dummy_handler,                 /* GPIO Port Q4                         144 */
    (void *) dummy_handler,                 /* GPIO Port Q5                         145 */
    (void *) dummy_handler,                 /* GPIO Port Q6                         146 */
    (void *) dummy_handler,                 /* GPIO Port Q7                         147 */
    (void *) dummy_handler,                 /* GPIO Port R                         148 */
    (void *) dummy_handler,                 /* GPIO Port S                         149 */
    (void *) dummy_handler,                 /* PWM 1 Generator 0                         150 */
    (void *) dummy_handler,                 /* PWM 1 Generator 1                         151 */
    (void *) dummy_handler,                 /* PWM 1 Generator 2                         152 */
    (void *) dummy_handler,                 /* PWM 1 Generator 3                         153 */
    (void *) dummy_handler                  /* PWM 1 Fault                         154 */
};
/** @} */
