/*
 * Copyright (c) 2017 - 2021, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @ingroup     pkg_nrfx
 * @{
 *
 * @file
 * @brief       This file contains macros that should be implemented according
 *              to the needs of the host environment into which nrfx is
 *              integrated.
 *
 * @author      Nordic Semiconductor ASA
 */

#ifndef NRFX_GLUE_H
#define NRFX_GLUE_H

#include "irq.h"
#include "assert.h"
#include <soc/nrfx_irqs.h>


#ifdef __cplusplus
extern "C" {
#endif
// this redefine needs to be done because RIOT uses different ISR names
#define POWER_CLOCK_IRQHandler isr_power_clock 
#define RADIO_IRQHandler isr_radio
#define UARTE0_UART0_IRQHandler isr_uart0
#define SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler ?
#define SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler ?
#define NFCT_IRQHandler isr_nfct
#define GPIOTE_IRQHandler isr_gpiote
#define SAADC_IRQHandler isr_saadc
#define TIMER0_IRQHandler isr_timer0
#define TIMER1_IRQHandler isr_timer1
#define TIMER2_IRQHandler isr_timer2
#define RTC0_IRQHandler isr_rtc0
#define TEMP_IRQHandler isr_temp
#define RNG_IRQHandler isr_rng
#define ECB_IRQHandler isr_ecb
#define CCM_AAR_IRQHandler isr_ccm_aar
#define WDT_IRQHandler isr_wdt
#define RTC1_IRQHandler isr_rtc1
#define QDEC_IRQHandler isr_qdec
#define COMP_LPCOMP_IRQHandler isr_lpcomp
#define SWI0_EGU0_IRQHandler isr_swi0
#define SWI1_EGU1_IRQHandler isr_swi1
#define SWI2_EGU2_IRQHandler isr_swi2
#define SWI3_EGU3_IRQHandler isr_swi3
#define SWI4_EGU4_IRQHandler isr_swi4
#define SWI5_EGU5_IRQHandler isr_swi5
#define TIMER3_IRQHandler isr_timer3
#define TIMER4_IRQHandler isr_timer4
#define PWM0_IRQHandler isr_pwm0
#define PDM_IRQHandler isr_pdm
#define MWU_IRQHandler isr_mwu
#define PWM1_IRQHandler isr_pwm1
#define PWM2_IRQHandler isr_pwm2
#define SPIM2_SPIS2_SPI2_IRQHandler isr_spi2
#define RTC2_IRQHandler isr_rtc2
#define I2S_IRQHandler isr_i2s
#define FPU_IRQHandler isr_fpu
#define USBD_IRQHandler isr_usbd
#define UARTE1_IRQHandler isr_uarte1
#define QSPI_IRQHandler isr_qspi
#define CRYPTOCELL_IRQHandler isr_cryptocell
#define PWM3_IRQHandler isr_spi3 // this is reversed
#define SPIM3_IRQHandler isr_pwm3 // this is reversed

/**
 * @brief Macro for placing a runtime assertion.
 *
 * @param expression Expression to be evaluated.
 */
#define NRFX_ASSERT(expression) assert(expression)

/**
 * @brief Macro for placing a compile time assertion.
 *
 * @param expression Expression to be evaluated.
 */
#define NRFX_STATIC_ASSERT(expression) static_assert(expression)

/**
 * @brief Macro for setting the priority of a specific IRQ.
 *
 * @param irq_number IRQ number.
 * @param priority   Priority to be set.
 */
#define NRFX_IRQ_PRIORITY_SET(irq_number, priority) NVIC_SetPriority(irq_number, priority)

/**
 * @brief Macro for enabling a specific IRQ.
 *
 * @param irq_number IRQ number.
 */
#define NRFX_IRQ_ENABLE(irq_number) NVIC_EnableIRQ(irq_number)

/**
 * @brief Macro for checking if a specific IRQ is enabled.
 *
 * @param irq_number IRQ number.
 *
 * @retval true  If the IRQ is enabled.
 * @retval false Otherwise.
 */
#define NRFX_IRQ_IS_ENABLED(irq_number) notimpl

/**
 * @brief Macro for disabling a specific IRQ.
 *
 * @param irq_number IRQ number.
 */
#define NRFX_IRQ_DISABLE(irq_number) NVIC_DisableIRQ(irq_number)

/**
 * @brief Macro for setting a specific IRQ as pending.
 *
 * @param irq_number IRQ number.
 */
#define NRFX_IRQ_PENDING_SET(irq_number) notimpl

/**
 * @brief Macro for clearing the pending status of a specific IRQ.
 *
 * @param irq_number IRQ number.
 */
#define NRFX_IRQ_PENDING_CLEAR(irq_number) NVIC_ClearPendingIRQ(irq_number)

/**
 * @brief Macro for checking the pending status of a specific IRQ.
 *
 * @retval true  If the IRQ is pending.
 * @retval false Otherwise.
 */
#define NRFX_IRQ_IS_PENDING(irq_number) notimpl

/** @brief Macro for entering into a critical section. */
#define NRFX_CRITICAL_SECTION_ENTER() irq_disable()

/** @brief Macro for exiting from a critical section. */
#define NRFX_CRITICAL_SECTION_EXIT() irq_enable()

/**
 * @brief When set to a non-zero value, this macro specifies that
 *        nrfx_coredep_delay_us uses a precise DWT-based solution.
 *        A compilation error is generated if the DWT unit is not present
 *        in the SoC used.
 */
#define NRFX_DELAY_DWT_BASED    0

/**
 * @brief Macro for delaying the code execution for at least the specified time.
 *
 * @param us_time Number of microseconds to wait.
 */
#define NRFX_DELAY_US(us_time) notimpl

/** @brief Atomic 32-bit unsigned type. */
#define nrfx_atomic_t uint32_t

/**
 * @brief Macro for storing a value to an atomic object and returning its previous value.
 *
 * @param[in] p_data Atomic memory pointer.
 * @param[in] value  Value to store.
 *
 * @return Previous value of the atomic object.
 */
#define NRFX_ATOMIC_FETCH_STORE(p_data, value) *p_data; *p_data = value;

/**
 * @brief Macro for running a bitwise OR operation on an atomic object and returning its previous value.
 *
 * @param[in] p_data Atomic memory pointer.
 * @param[in] value  Value of the second operand in the OR operation.
 *
 * @return Previous value of the atomic object.
 */
#define NRFX_ATOMIC_FETCH_OR(p_data, value) notimpl

/**
 * @brief Macro for running a bitwise AND operation on an atomic object
 *        and returning its previous value.
 *
 * @param[in] p_data Atomic memory pointer.
 * @param[in] value  Value of the second operand in the AND operation.
 *
 * @return Previous value of the atomic object.
 */
#define NRFX_ATOMIC_FETCH_AND(p_data, value) notimpl

/**
 * @brief Macro for running a bitwise XOR operation on an atomic object
 *        and returning its previous value.
 *
 * @param[in] p_data Atomic memory pointer.
 * @param[in] value  Value of the second operand in the XOR operation.
 *
 * @return Previous value of the atomic object.
 */
#define NRFX_ATOMIC_FETCH_XOR(p_data, value) notimpl

/**
 * @brief Macro for running an addition operation on an atomic object
 *        and returning its previous value.
 *
 * @param[in] p_data Atomic memory pointer.
 * @param[in] value  Value of the second operand in the ADD operation.
 *
 * @return Previous value of the atomic object.
 */
#define NRFX_ATOMIC_FETCH_ADD(p_data, value) notimpl

/**
 * @brief Macro for running a subtraction operation on an atomic object
 *        and returning its previous value.
 *
 * @param[in] p_data Atomic memory pointer.
 * @param[in] value  Value of the second operand in the SUB operation.
 *
 * @return Previous value of the atomic object.
 */
#define NRFX_ATOMIC_FETCH_SUB(p_data, value) notimpl

/**
 * @brief Macro for running compare and swap on an atomic object.
 *
 * Value is updated to the new value only if it previously equaled old value.
 *
 * @param[in,out] p_data    Atomic memory pointer.
 * @param[in]     old_value Expected old value.
 * @param[in]     new_value New value.
 *
 * @retval true  If value was updated.
 * @retval false If value was not updated because location was not equal to @p old_value.
 */
#define NRFX_ATOMIC_CAS(p_data, old_value, new_value) notimpl

/**
 * @brief Macro for counting leading zeros.
 *
 * @param[in] value A word value.
 *
 * @return Number of leading 0-bits in @p value, starting at the most significant bit position.
 *         If x is 0, the result is undefined.
 */
#define NRFX_CLZ(value) notimpl

/**
 * @brief Macro for counting trailing zeros.
 *
 * @param[in] value A word value.
 *
 * @return Number of trailing 0-bits in @p value, starting at the least significant bit position.
 *         If x is 0, the result is undefined.
 */
#define NRFX_CTZ(value) __builtin_ctz(value)

/**
 * @brief When set to a non-zero value, this macro specifies that the
 *        nrfx_error_codes and the nrfx_err_t type itself are defined
 *        in a customized way and the default definitions from @c <nrfx_error.h>
 *        should not be used.
 */
#define NRFX_CUSTOM_ERROR_CODES 0

/**
 * @brief When set to a non-zero value, this macro specifies that inside HALs
 *        the event registers are read back after clearing, on devices that
 *        otherwise could defer the actual register modification.
 */
#define NRFX_EVENT_READBACK_ENABLED 1

/** @brief Bitmask that defines DPPI channels that are reserved for use outside of the nrfx library. */
#define NRFX_DPPI_CHANNELS_USED   0

/** @brief Bitmask that defines DPPI groups that are reserved for use outside of the nrfx library. */
#define NRFX_DPPI_GROUPS_USED     0

/** @brief Bitmask that defines PPI channels that are reserved for use outside of the nrfx library. */
#define NRFX_PPI_CHANNELS_USED    0

/** @brief Bitmask that defines PPI groups that are reserved for use outside of the nrfx library. */
#define NRFX_PPI_GROUPS_USED      0

/** @brief Bitmask that defines GPIOTE channels that are reserved for use outside of the nrfx library. */
#define NRFX_GPIOTE_CHANNELS_USED 0

/** @brief Bitmask that defines EGU instances that are reserved for use outside of the nrfx library. */
#define NRFX_EGUS_USED            0

/** @brief Bitmask that defines TIMER instances that are reserved for use outside of the nrfx library. */
#define NRFX_TIMERS_USED          0


#ifdef __cplusplus
}
#endif

#endif /* NRFX_GLUE_H */
/** @} */
