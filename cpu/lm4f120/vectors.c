/*
 * Copyright (C) 2(void *) (0UL)14-2(void *) (0UL)15 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
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
WEAK_DEFAULT void isr_usart0(void);         // UART 0
WEAK_DEFAULT void isr_usart1(void);         // UART 1
WEAK_DEFAULT void isr_timer0(void);         // 16 bit timer 0 A
WEAK_DEFAULT void isr_timer1(void);         // 16 bit timer 1 A
WEAK_DEFAULT void isr_wtimer0(void);        // 32 bit timer 0 A
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_tamp_stamp(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_dma1_stream0(void);
WEAK_DEFAULT void isr_dma1_stream1(void);
WEAK_DEFAULT void isr_dma1_stream2(void);
WEAK_DEFAULT void isr_dma1_stream3(void);
WEAK_DEFAULT void isr_dma1_stream4(void);
WEAK_DEFAULT void isr_dma1_stream5(void);
WEAK_DEFAULT void isr_dma1_stream6(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_can1_tx(void);
WEAK_DEFAULT void isr_can1_rx0(void);
WEAK_DEFAULT void isr_can1_rx1(void);
WEAK_DEFAULT void isr_can1_sce(void);
WEAK_DEFAULT void isr_tim1_brk_tim9(void);
WEAK_DEFAULT void isr_tim1_up_tim10(void);
WEAK_DEFAULT void isr_tim1_trg_com_tim11(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim4(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c2_ev(void);
WEAK_DEFAULT void isr_i2c2_er(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_otg_fs_wkup(void);
WEAK_DEFAULT void isr_tim8_brk_tim12(void);
WEAK_DEFAULT void isr_tim8_up_tim13(void);
WEAK_DEFAULT void isr_tim8_trg_com_tim14(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_dma1_stream7(void);
WEAK_DEFAULT void isr_fsmc(void);
WEAK_DEFAULT void isr_sdio(void);
WEAK_DEFAULT void isr_tim5(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_dma2_stream0(void);
WEAK_DEFAULT void isr_dma2_stream1(void);
WEAK_DEFAULT void isr_dma2_stream2(void);
WEAK_DEFAULT void isr_dma2_stream3(void);
WEAK_DEFAULT void isr_dma2_stream4(void);
WEAK_DEFAULT void isr_eth(void);
WEAK_DEFAULT void isr_eth_wkup(void);
WEAK_DEFAULT void isr_can2_tx(void);
WEAK_DEFAULT void isr_can2_rx0(void);
WEAK_DEFAULT void isr_can2_rx1(void);
WEAK_DEFAULT void isr_can2_sce(void);
WEAK_DEFAULT void isr_otg_fs(void);
WEAK_DEFAULT void isr_dma2_stream5(void);
WEAK_DEFAULT void isr_dma2_stream6(void);
WEAK_DEFAULT void isr_dma2_stream7(void);
WEAK_DEFAULT void isr_usart6(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_otg_hs_ep1_out(void);
WEAK_DEFAULT void isr_otg_hs_ep1_in(void);
WEAK_DEFAULT void isr_otg_hs_wkup(void);
WEAK_DEFAULT void isr_otg_hs(void);
WEAK_DEFAULT void isr_dcmi(void);
WEAK_DEFAULT void isr_cryp(void);
WEAK_DEFAULT void isr_hash_rng(void);
WEAK_DEFAULT void isr_fpu(void);

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
    // Peripherial interrupts start here.
    (void *) dummy_handler,         // GPIO Port A                      16
    (void *) dummy_handler,         // GPIO Port B                      17
    (void *) dummy_handler,         // GPIO Port C                      18
    (void *) dummy_handler,         // GPIO Port D                      19
    (void *) dummy_handler,         // GPIO Port E                      20
    (void *) isr_usart0,            // UART 0                           21
    (void *) isr_usart1,            // UART 1                           22
    (void *) dummy_handler,         // SSI 0                            23
    (void *) dummy_handler,         // I2C 0                            24
    (void *) (0UL),                 // Reserved                         25
    (void *) (0UL),                 // Reserved                         26
    (void *) (0UL),                 // Reserved                         27
    (void *) (0UL),                 // Reserved                         28
    (void *) (0UL),                 // Reserved                         29
    (void *) dummy_handler,         // ADC 0 Seq 0                      30
    (void *) dummy_handler,         // ADC 0 Seq 1                      31
    (void *) dummy_handler,         // ADC 0 Seq 2                      32
    (void *) dummy_handler,         // ADC 0 Seq 3                      33
    (void *) dummy_handler,         // WDT 0 and 1                      34
    (void *) isr_timer0,            // 16/32 bit timer 0 A              35
    (void *) dummy_handler,         // 16/32 bit timer 0 B              36
    (void *) isr_timer1,            // 16/32 bit timer 1 A              37
    (void *) dummy_handler,         // 16/32 bit timer 1 B              38
    (void *) dummy_handler,         // 16/32 bit timer 2 A              39
    (void *) dummy_handler,         // 16/32 bit timer 2 B              40
    (void *) dummy_handler,         // Analog comparator 0              41
    (void *) dummy_handler,         // Analog comparator 1              42
    (void *) (0UL),                 // Reserved                         43
    (void *) dummy_handler,         // System control                   44
    (void *) dummy_handler,         // Flash + EEPROM control           45
    (void *) dummy_handler,         // GPIO Port F                      46
    (void *) (0UL),                 // Reserved                         47
    (void *) (0UL),                 // Reserved                         48
    (void *) dummy_handler,         // UART 2                           49
    (void *) dummy_handler,         // SSI 1                            50
    (void *) dummy_handler,         // 16/32 bit timer 3 A              51
    (void *) dummy_handler,         // 16/32 bit timer 3 B              52
    (void *) dummy_handler,         // I2C 1                            53
    (void *) (0UL),                 // Reserved                         54
    (void *) dummy_handler,         // CAN 0                            55
    (void *) (0UL),                 // Reserved                         56
    (void *) (0UL),                 // Reserved                         57
    (void *) (0UL),                 // Reserved                         58
    (void *) dummy_handler,         // Hibernation module               59
    (void *) dummy_handler,         // USB                              60
    (void *) (0UL),                 // Reserved                         61
    (void *) dummy_handler,         // UDMA SW                          62
    (void *) dummy_handler,         // UDMA Error                       63
    (void *) dummy_handler,         // ADC 1 Seq 0)                     64
    (void *) dummy_handler,         // ADC 1 Seq 1                      65
    (void *) dummy_handler,         // ADC 1 Seq 2                      66
    (void *) dummy_handler,         // ADC 1 Seq 3                      67
    (void *) (0UL),                 // Reserved                         68
    (void *) (0UL),                 // Reserved                         69
    (void *) (0UL),                 // Reserved                         70
    (void *) (0UL),                 // Reserved                         71
    (void *) (0UL),                 // Reserved                         72
    (void *) dummy_handler,         // SSI 2                            73
    (void *) dummy_handler,         // SSI 2                            74
    (void *) dummy_handler,         // UART 3                           75
    (void *) dummy_handler,         // UART 4                           76
    (void *) dummy_handler,         // UART 5                           77
    (void *) dummy_handler,         // UART 6                           78
    (void *) dummy_handler,         // UART 7                           79
    (void *) (0UL),                 // Reserved                         80
    (void *) (0UL),                 // Reserved                         81
    (void *) (0UL),                 // Reserved                         82
    (void *) (0UL),                 // Reserved                         83
    (void *) dummy_handler,         // I2C 2                            84
    (void *) dummy_handler,         // I2C 4                            85
    (void *) dummy_handler,         // 16/32 bit timer 4 A              86
    (void *) dummy_handler,         // 16/32 bit timer 4 B              87
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
    (void *) dummy_handler,         // 16/32 bit timer 5 A              108
    (void *) dummy_handler,         // 16/32 bit timer 5 B              109
    (void *) isr_wtimer0,       // 32/64 bit timer 0 A              110
    (void *) dummy_handler,         // 32/64 bit timer 0 B              111
    (void *) dummy_handler,         // 32/64 bit timer 1 A              112
    (void *) dummy_handler,         // 32/64 bit timer 1 B              113
    (void *) dummy_handler,         // 32/64 bit timer 2 A              114
    (void *) dummy_handler,         // 32/64 bit timer 2 B              115
    (void *) dummy_handler,         // 32/64 bit timer 3 A              116
    (void *) dummy_handler,         // 32/64 bit timer 3 B              117
    (void *) dummy_handler,         // 32/64 bit timer 4 A              118
    (void *) dummy_handler,         // 32/64 bit timer 4 B              119
    (void *) dummy_handler,         // 32/64 bit timer 5 A              120
    (void *) dummy_handler,         // 32/64 bit timer 5 B              121
    (void *) dummy_handler,         // System Exception                 122
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
