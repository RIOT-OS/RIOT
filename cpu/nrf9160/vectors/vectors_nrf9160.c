/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf9160
 * @{
 *
 * @file
 * @brief       nRF9160 interrupt vector definitions
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
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

/* nRF9160 specific interrupt vectors */
WEAK_DEFAULT void isr_spu(void);
WEAK_DEFAULT void isr_clock_power(void);
WEAK_DEFAULT void isr_uarte0_spim0_spis0_twim0_twis0(void);
WEAK_DEFAULT void isr_uarte1_spim1_spis1_twim1_twis1(void);
WEAK_DEFAULT void isr_uarte2_spim2_spis2_twim2_twis2(void);
WEAK_DEFAULT void isr_uarte3_spim3_spis3_twim3_twis3(void);
WEAK_DEFAULT void isr_gpiote0(void);
WEAK_DEFAULT void isr_saadc(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_timer2(void);
WEAK_DEFAULT void isr_rtc0(void);
WEAK_DEFAULT void isr_rtc1(void);
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_egu0(void);
WEAK_DEFAULT void isr_egu1(void);
WEAK_DEFAULT void isr_egu2(void);
WEAK_DEFAULT void isr_egu3(void);
WEAK_DEFAULT void isr_egu4(void);
WEAK_DEFAULT void isr_egu5(void);
WEAK_DEFAULT void isr_pwm0(void);
WEAK_DEFAULT void isr_pwm1(void);
WEAK_DEFAULT void isr_pwm2(void);
WEAK_DEFAULT void isr_pwm3(void);
WEAK_DEFAULT void isr_pdm(void);
WEAK_DEFAULT void isr_i2s(void);
WEAK_DEFAULT void isr_ipc(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_gpiote1(void);
WEAK_DEFAULT void isr_kmu(void);
WEAK_DEFAULT void isr_cryptocell(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [3] = isr_spu,                /* SPU */
    [5] = isr_clock_power,        /* power_clock */
    [8] = isr_uarte0_spim0_spis0_twim0_twis0,
    [9] = isr_uarte1_spim1_spis1_twim1_twis1,
    [10] = isr_uarte2_spim2_spis2_twim2_twis2,
    [11] = isr_uarte3_spim3_spis3_twim3_twis3,
    [13] = isr_gpiote0,             /* gpiote0 */
    [14] = isr_saadc,
    [15] = isr_timer0,
    [16] = isr_timer1,
    [17] = isr_timer2,
    [20] = isr_rtc0,
    [21] = isr_rtc1,
    [24] = isr_wdt,
    [27] = isr_egu0,
    [28] = isr_egu1,
    [29] = isr_egu2,
    [30] = isr_egu3,
    [31] = isr_egu4,
    [32] = isr_egu5,
    [33] = isr_pwm0,
    [34] = isr_pwm1,
    [35] = isr_pwm2,
    [36] = isr_pwm3,
    [38] = isr_pdm,
    [40] = isr_i2s,
    [42] = isr_ipc,
    [44] = isr_fpu,
    [49] = isr_gpiote1,
    [57] = isr_kmu,
    [64] = isr_cryptocell
};
