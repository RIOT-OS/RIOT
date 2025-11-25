/*
 * Copyright (C) 2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf53
 * @{
 *
 * @file
 * @brief       nRF5340 interrupt vector definitions
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

/* nRF5340 specific interrupt vectors */
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_cache(void);
WEAK_DEFAULT void isr_spu(void);
WEAK_DEFAULT void isr_clock_power(void);
WEAK_DEFAULT void isr_serial0(void);
WEAK_DEFAULT void isr_serial1(void);
WEAK_DEFAULT void isr_spim4(void);
WEAK_DEFAULT void isr_serial2(void);
WEAK_DEFAULT void isr_serial3(void);
WEAK_DEFAULT void isr_gpiote0(void);
WEAK_DEFAULT void isr_saadc(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_timer2(void);
WEAK_DEFAULT void isr_rtc0(void);
WEAK_DEFAULT void isr_rtc1(void);
WEAK_DEFAULT void isr_wdt0(void);
WEAK_DEFAULT void isr_wdt1(void);
WEAK_DEFAULT void isr_comp_ltcomp(void);
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
WEAK_DEFAULT void isr_pdm0(void);
WEAK_DEFAULT void isr_i2s0(void);
WEAK_DEFAULT void isr_ipc(void);
WEAK_DEFAULT void isr_qspi(void);
WEAK_DEFAULT void isr_nfct(void);
WEAK_DEFAULT void isr_gpiote1(void);
WEAK_DEFAULT void isr_qdec0(void);
WEAK_DEFAULT void isr_qdec1(void);
WEAK_DEFAULT void isr_usbd(void);
WEAK_DEFAULT void isr_usb_regulator(void);
WEAK_DEFAULT void isr_kmu(void);
WEAK_DEFAULT void isr_cryptocell(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
 [0] = isr_fpu,
 [1] = isr_cache,
 [3] = isr_spu,
 [5] = isr_clock_power,
 [8] = isr_serial0,
 [9] = isr_serial1,
 [10] = isr_spim4,
 [11] = isr_serial2,
 [12] = isr_serial3,
 [13] = isr_gpiote0,
 [14] = isr_saadc,
 [15] = isr_timer0,
 [16] = isr_timer1,
 [17] = isr_timer2,
 [20] = isr_rtc0,
 [21] = isr_rtc1,
 [24] = isr_wdt0,
 [25] = isr_wdt1,
 [26] = isr_comp_ltcomp,
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
 [38] = isr_pdm0,
 [40] = isr_i2s0,
 [42] = isr_ipc,
 [43] = isr_qspi,
 [45] = isr_nfct,
 [47] = isr_gpiote1,
 [51] = isr_qdec0,
 [52] = isr_qdec1,
 [54] = isr_usbd,
 [55] = isr_usb_regulator,
 [57] = isr_kmu,
 [68] = isr_cryptocell
};
