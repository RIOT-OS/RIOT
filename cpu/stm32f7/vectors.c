/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f7
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
/* STM32F7 specific interrupt vectors */
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
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_otg_fs_wkup(void);
WEAK_DEFAULT void isr_tim8_brk_tim12(void);
WEAK_DEFAULT void isr_tim8_up_tim13(void);
WEAK_DEFAULT void isr_tim8_trg_com_tim14(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_dma1_stream7(void);
WEAK_DEFAULT void isr_fmc(void);
WEAK_DEFAULT void isr_sdmmc1(void);
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
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_uart7(void);
WEAK_DEFAULT void isr_uart8(void);
WEAK_DEFAULT void isr_spi4(void);
WEAK_DEFAULT void isr_spi5(void);
WEAK_DEFAULT void isr_spi6(void);
WEAK_DEFAULT void isr_sai1(void);
WEAK_DEFAULT void isr_ltdc(void);
WEAK_DEFAULT void isr_ltdc_er(void);
WEAK_DEFAULT void isr_dma2d(void);
WEAK_DEFAULT void isr_sai2(void);
WEAK_DEFAULT void isr_quadspi(void);
WEAK_DEFAULT void isr_lptim1(void);
WEAK_DEFAULT void isr_cec(void);
WEAK_DEFAULT void isr_i2c4_ev(void);
WEAK_DEFAULT void isr_i2c4_er(void);
WEAK_DEFAULT void isr_spdif_rx(void);
#if defined(CPU_MODEL_STM32F769NI)
WEAK_DEFAULT void isr_dsi(void);
WEAK_DEFAULT void isr_dfsdm1_flt0(void);
WEAK_DEFAULT void isr_dfsdm1_flt1(void);
WEAK_DEFAULT void isr_dfsdm1_flt2(void);
WEAK_DEFAULT void isr_dfsdm1_flt3(void);
WEAK_DEFAULT void isr_sdmmc2(void);
WEAK_DEFAULT void isr_can3_tx(void);
WEAK_DEFAULT void isr_can3_rx0(void);
WEAK_DEFAULT void isr_can3_rx1(void);
WEAK_DEFAULT void isr_can3_sce(void);
WEAK_DEFAULT void isr_jpeg(void);
WEAK_DEFAULT void isr_mdios(void);
#endif

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M7 handlers */
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
    /* STM specific peripheral handlers */
    (void*) isr_wwdg,
    (void*) isr_pvd,
    (void*) isr_tamp_stamp,
    (void*) isr_rtc_wkup,
    (void*) isr_flash,
    (void*) isr_rcc,
    (void*) isr_exti,
    (void*) isr_exti,
    (void*) isr_exti,
    (void*) isr_exti,
    (void*) isr_exti,
    (void*) isr_dma1_stream0,
    (void*) isr_dma1_stream1,
    (void*) isr_dma1_stream2,
    (void*) isr_dma1_stream3,
    (void*) isr_dma1_stream4,
    (void*) isr_dma1_stream5,
    (void*) isr_dma1_stream6,
    (void*) isr_adc,
    (void*) isr_can1_tx,
    (void*) isr_can1_rx0,
    (void*) isr_can1_rx1,
    (void*) isr_can1_sce,
    (void*) isr_exti,
    (void*) isr_tim1_brk_tim9,
    (void*) isr_tim1_up_tim10,
    (void*) isr_tim1_trg_com_tim11,
    (void*) isr_tim1_cc,
    (void*) isr_tim2,
    (void*) isr_tim3,
    (void*) isr_tim4,
    (void*) isr_i2c1_ev,
    (void*) isr_i2c1_er,
    (void*) isr_i2c2_ev,
    (void*) isr_i2c2_er,
    (void*) isr_spi1,
    (void*) isr_spi2,
    (void*) isr_usart1,
    (void*) isr_usart2,
    (void*) isr_usart3,
    (void*) isr_exti,
    (void*) isr_rtc_alarm,
    (void*) isr_otg_fs_wkup,
    (void*) isr_tim8_brk_tim12,
    (void*) isr_tim8_up_tim13,
    (void*) isr_tim8_trg_com_tim14,
    (void*) isr_tim8_cc,
    (void*) isr_dma1_stream7,
    (void*) isr_fmc,
    (void*) isr_sdmmc1,
    (void*) isr_tim5,
    (void*) isr_spi3,
    (void*) isr_uart4,
    (void*) isr_uart5,
    (void*) isr_tim6_dac,
    (void*) isr_tim7,
    (void*) isr_dma2_stream0,
    (void*) isr_dma2_stream1,
    (void*) isr_dma2_stream2,
    (void*) isr_dma2_stream3,
    (void*) isr_dma2_stream4,
    (void*) isr_eth,
    (void*) isr_eth_wkup,
    (void*) isr_can2_tx,
    (void*) isr_can2_rx0,
    (void*) isr_can2_rx1,
    (void*) isr_can2_sce,
    (void*) isr_otg_fs,
    (void*) isr_dma2_stream5,
    (void*) isr_dma2_stream6,
    (void*) isr_dma2_stream7,
    (void*) isr_usart6,
    (void*) isr_i2c3_ev,
    (void*) isr_i2c3_er,
    (void*) isr_otg_hs_ep1_out,
    (void*) isr_otg_hs_ep1_in,
    (void*) isr_otg_hs_wkup,
    (void*) isr_otg_hs,
    (void*) isr_dcmi,
    (void*) (0UL),
    (void*) isr_rng,
    (void*) isr_fpu,
    (void*) isr_uart7,
    (void*) isr_uart8,
    (void*) isr_spi4,
    (void*) isr_spi5,
    (void*) isr_spi6,
    (void*) isr_sai1,
    (void*) isr_ltdc,
    (void*) isr_ltdc_er,
    (void*) isr_dma2d,
    (void*) isr_sai2,
    (void*) isr_quadspi,
    (void*) isr_lptim1,
    (void*) isr_cec,
    (void*) isr_i2c4_ev,
    (void*) isr_i2c4_er,
    (void*) isr_spdif_rx,
#if defined(CPU_MODEL_STM32F769NI)
    (void*) isr_dsi,
    (void*) isr_dfsdm1_flt0,
    (void*) isr_dfsdm1_flt1,
    (void*) isr_dfsdm1_flt2,
    (void*) isr_dfsdm1_flt3,
    (void*) isr_sdmmc2,
    (void*) isr_can3_tx,
    (void*) isr_can3_rx0,
    (void*) isr_can3_rx1,
    (void*) isr_can3_sce,
    (void*) isr_jpeg,
    (void*) isr_mdios,
#endif
};
