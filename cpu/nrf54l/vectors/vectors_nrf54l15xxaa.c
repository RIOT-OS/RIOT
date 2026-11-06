/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf54l
 * @{
 *
 * @file
 * @brief       nRF54L15 interrupt vector definitions
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* nRF54L15 specific interrupt vectors */
WEAK_DEFAULT void isr_swi00(void);
WEAK_DEFAULT void isr_swi01(void);
WEAK_DEFAULT void isr_swi02(void);
WEAK_DEFAULT void isr_swi03(void);
WEAK_DEFAULT void isr_spu00(void);
WEAK_DEFAULT void isr_mpc00(void);
WEAK_DEFAULT void isr_aar00_ccm00(void);
WEAK_DEFAULT void isr_ecb00(void);
WEAK_DEFAULT void isr_cracen(void);
WEAK_DEFAULT void isr_serial00(void);
WEAK_DEFAULT void isr_rramc(void);
WEAK_DEFAULT void isr_vpr00(void);
WEAK_DEFAULT void isr_ctrlap(void);
WEAK_DEFAULT void isr_cm33ss(void);
WEAK_DEFAULT void isr_timer00(void);
WEAK_DEFAULT void isr_spu10(void);
WEAK_DEFAULT void isr_timer10(void);
WEAK_DEFAULT void isr_egu10(void);
WEAK_DEFAULT void isr_radio_0(void);
WEAK_DEFAULT void isr_radio_1(void);
WEAK_DEFAULT void isr_spu20(void);
WEAK_DEFAULT void isr_serial20(void);
WEAK_DEFAULT void isr_serial21(void);
WEAK_DEFAULT void isr_serial22(void);
WEAK_DEFAULT void isr_egu20(void);
WEAK_DEFAULT void isr_timer20(void);
WEAK_DEFAULT void isr_timer21(void);
WEAK_DEFAULT void isr_timer22(void);
WEAK_DEFAULT void isr_timer23(void);
WEAK_DEFAULT void isr_timer24(void);
WEAK_DEFAULT void isr_pdm20(void);
WEAK_DEFAULT void isr_pdm21(void);
WEAK_DEFAULT void isr_pwm20(void);
WEAK_DEFAULT void isr_pwm21(void);
WEAK_DEFAULT void isr_pwm22(void);
WEAK_DEFAULT void isr_saadc(void);
WEAK_DEFAULT void isr_nfct(void);
WEAK_DEFAULT void isr_temp(void);
WEAK_DEFAULT void isr_gpiote20_0(void);
WEAK_DEFAULT void isr_gpiote20_1(void);
WEAK_DEFAULT void isr_tampc(void);
WEAK_DEFAULT void isr_i2s20(void);
WEAK_DEFAULT void isr_qdec20(void);
WEAK_DEFAULT void isr_qdec21(void);
WEAK_DEFAULT void isr_grtc_0(void);
WEAK_DEFAULT void isr_grtc_1(void);
WEAK_DEFAULT void isr_grtc_2(void);
WEAK_DEFAULT void isr_grtc_3(void);
WEAK_DEFAULT void isr_spu30(void);
WEAK_DEFAULT void isr_serial30(void);
WEAK_DEFAULT void isr_clock_power(void);
WEAK_DEFAULT void isr_comp_lpcomp(void);
WEAK_DEFAULT void isr_wdt30(void);
WEAK_DEFAULT void isr_wdt31(void);
WEAK_DEFAULT void isr_gpiote30_0(void);
WEAK_DEFAULT void isr_gpiote30_1(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [SWI00_IRQn]        = isr_swi00,
    [SWI01_IRQn]        = isr_swi01,
    [SWI02_IRQn]        = isr_swi02,
    [SWI03_IRQn]        = isr_swi03,
    [SPU00_IRQn]        = isr_spu00,
    [MPC00_IRQn]        = isr_mpc00,
    [AAR00_CCM00_IRQn]  = isr_aar00_ccm00,
    [ECB00_IRQn]        = isr_ecb00,
    [CRACEN_IRQn]       = isr_cracen,
    [SERIAL00_IRQn]     = isr_serial00,
    [RRAMC_IRQn]        = isr_rramc,
    [VPR00_IRQn]        = isr_vpr00,
    [CTRLAP_IRQn]       = isr_ctrlap,
    [CM33SS_IRQn]       = isr_cm33ss,
    [TIMER00_IRQn]      = isr_timer00,
    [SPU10_IRQn]        = isr_spu10,
    [TIMER10_IRQn]      = isr_timer10,
    [EGU10_IRQn]        = isr_egu10,
    [RADIO_0_IRQn]      = isr_radio_0,
    [RADIO_1_IRQn]      = isr_radio_1,
    [SPU20_IRQn]        = isr_spu20,
    [SERIAL20_IRQn]     = isr_serial20,
    [SERIAL21_IRQn]     = isr_serial21,
    [SERIAL22_IRQn]     = isr_serial22,
    [EGU20_IRQn]        = isr_egu20,
    [TIMER20_IRQn]      = isr_timer20,
    [TIMER21_IRQn]      = isr_timer21,
    [TIMER22_IRQn]      = isr_timer22,
    [TIMER23_IRQn]      = isr_timer23,
    [TIMER24_IRQn]      = isr_timer24,
    [PDM20_IRQn]        = isr_pdm20,
    [PDM21_IRQn]        = isr_pdm21,
    [PWM20_IRQn]        = isr_pwm20,
    [PWM21_IRQn]        = isr_pwm21,
    [PWM22_IRQn]        = isr_pwm22,
    [SAADC_IRQn]        = isr_saadc,
    [NFCT_IRQn]         = isr_nfct,
    [TEMP_IRQn]         = isr_temp,
    [GPIOTE20_0_IRQn]   = isr_gpiote20_0,
    [GPIOTE20_1_IRQn]   = isr_gpiote20_1,
    [TAMPC_IRQn]        = isr_tampc,
    [I2S20_IRQn]        = isr_i2s20,
    [QDEC20_IRQn]       = isr_qdec20,
    [QDEC21_IRQn]       = isr_qdec21,
    [GRTC_0_IRQn]       = isr_grtc_0,
    [GRTC_1_IRQn]       = isr_grtc_1,
    [GRTC_2_IRQn]       = isr_grtc_2,
    [GRTC_3_IRQn]       = isr_grtc_3,
    [SPU30_IRQn]        = isr_spu30,
    [SERIAL30_IRQn]     = isr_serial30,
    [CLOCK_POWER_IRQn]  = isr_clock_power,
    [COMP_LPCOMP_IRQn]  = isr_comp_lpcomp,
    [WDT30_IRQn]        = isr_wdt30,
    [WDT31_IRQn]        = isr_wdt31,
    [GPIOTE30_0_IRQn]   = isr_gpiote30_0,
    [GPIOTE30_1_IRQn]   = isr_gpiote30_1,
};
