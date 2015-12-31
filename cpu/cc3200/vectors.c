/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa#gmail.com>
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
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 * @author      Dinh Nguyen <nqdinhddt@gmail.com>
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
WEAK_DEFAULT void isr_sha(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_des(void);
WEAK_DEFAULT void isr_sdhost(void);
WEAK_DEFAULT void isr_i2s(void);
WEAK_DEFAULT void isr_camera(void);
WEAK_DEFAULT void isr_nwp_to_apps(void);
WEAK_DEFAULT void isr_prcm(void);
WEAK_DEFAULT void isr_shared_spi(void);
WEAK_DEFAULT void isr_generic_spi(void);
WEAK_DEFAULT void isr_link_spi(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[256] = {
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
    (void *) isr_gpio_porta,        /* GPIO Port A0                     16 */
    (void *) isr_gpio_portb,        /* GPIO Port A1                     17 */
    (void *) isr_gpio_portc,        /* GPIO Port A2                     18 */
    (void *) isr_gpio_portd,        /* GPIO Port A3                     19 */
    (void *) (0UL),                 /* Reversed                         20 */
    (void *) isr_uart0,             /* UART 0                           21 */
    (void *) isr_uart1,             /* UART 1                           22 */
    (void *) (0UL),                 /* Reversed                         23 */
    (void *) isr_i2c0,              /* I2C 0                            24 */
    (void *) (0UL),                 /* Reserved                         25 */
    (void *) (0UL),                 /* Reserved                         26 */
    (void *) (0UL),                 /* Reserved                         27 */
    (void *) (0UL),                 /* Reserved                         28 */
    (void *) (0UL),                 /* Reserved                         29 */
    (void *) isr_adc0_seq0,         /* ADC Channel 0                    30 */
    (void *) isr_adc0_seq1,         /* ADC Channel 1                    31 */
    (void *) isr_adc0_seq2,         /* ADC Channel 2                    32 */
    (void *) isr_adc0_seq3,         /* ADC Channel 3                    33 */
    (void *) isr_wdt,               /* WDT 0 and 1                      34 */
    (void *) isr_timer0a,           /* 16/32 bit timer 0 A              35 */
    (void *) isr_timer0b,           /* 16/32 bit timer 0 B              36 */
    (void *) isr_timer1a,           /* 16/32 bit timer 1 A              37 */
    (void *) isr_timer1b,           /* 16/32 bit timer 1 B              38 */
    (void *) isr_timer2a,           /* 16/32 bit timer 2 A              39 */
    (void *) isr_timer2b,           /* 16/32 bit timer 2 B              40 */
    (void *) (0UL),                 /* Reserved                         41 */
    (void *) (0UL),                 /* Reserved                         42 */
    (void *) (0UL),                 /* Reserved                         43 */
    (void *) (0UL),                 /* Reserved                         44 */
    (void *) isr_flashctl,          /* Flash + EEPROM control           45 */
    (void *) (0UL),                 /* Reverved                         46 */
    (void *) (0UL),                 /* Reserved                         47 */
    (void *) (0UL),                 /* Reserved                         48 */
    (void *) (0UL),                 /* Reserved                         49 */
    (void *) (0UL),                 /* Reserved                         50 */
    (void *) isr_timer3a,           /* 16/32 bit timer 3 A              51 */
    (void *) isr_timer3b,           /* 16/32 bit timer 3 B              52 */
    (void *) isr_i2c1,              /* I2C 1                            53 */
    (void *) (0UL),                 /* Reserved                         54 */
    (void *) (0UL),                 /* CAN 0                            55 */
    (void *) (0UL),                 /* Reserved                         56 */
    (void *) (0UL),                 /* Reserved                         57 */
    (void *) (0UL),                 /* Reserved                         58 */
    (void *) (0UL),                 /* Reserved                         59 */
    (void *) (0UL),                 /* Reserved                         60 */
    (void *) (0UL),                 /* Reserved                         61 */
    (void *) isr_udma_sw,           /* UDMA SW                          62 */
    (void *) isr_udma_error,        /* UDMA Error                       63 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                          73 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                          83 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                          93 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         103 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         113 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         123 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         133 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         143 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         153 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         163 */
    (void *) isr_sha,               /* SHA                              164 */
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         166 */
    (void *) isr_aes,               /* AES                              167 */
    (void *) (0UL),                 /* Reserved                         168 */
    (void *) isr_des,               /* DES                              169 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         174 */
    (void *) isr_sdhost,            /* SDHost                           175 */
    (void *) (0UL),                 /* Reserved                         176 */
    (void *) isr_i2s,               /* I2S                              177 */
    (void *) (0UL),                 /* Reserved                         178 */
    (void *) isr_camera,            /* Camera                           179 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         186 */
    (void *) isr_nwp_to_apps,       /* NWP to APPS Interrupt            187 */
    (void *) isr_prcm,              /* Power, Reset and Clock module    188 */
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         190 */
    (void *) isr_shared_spi,        /* Shared SPI                       191 */
    (void *) isr_generic_spi,       /* Generic SPI                      192 */
    (void *) isr_link_spi,          /* Link SPI                         193 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         203 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         213 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         223 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         233 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         243 */
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),
    (void *) (0UL),                 /* Reserved                         253 */
    (void *) (0UL),
    (void *) (0UL)                  /* Reserved                         255 */
};
/** @} */
