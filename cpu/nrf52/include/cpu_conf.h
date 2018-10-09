/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_nrf52 Nordic nRF52 MCU
 * @ingroup         cpu
 * @brief           Nordic nRF52 family of CPUs
 * @{
 *
 * @file
 * @brief       nRF52 specific CPU configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

/**
 * @brief   SoftDevice settings
 * @{
 */
#ifdef SOFTDEVICE_PRESENT
/*
 * BLE callbacks may take place in the IDLE stack
 */
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT (2048)
#endif
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE THREAD_STACKSIZE_DEFAULT
#endif
#endif /* SOFTDEVICE_PRESENT */
/** @} */

/**
 * @name    ARM Cortex-M specific CPU configuration
 * @{
 */
#if defined(SOFTDEVICE_PRESENT) || defined(DOXYGEN)

/**
 * @brief Enable Cortex-M subpriorities for nRF52 with SoftDevice
 *
 * On NRF52, when the SoftDevice is used, we use Cortex-M
 * subpriorities to set the "normal" RIOT
 */
#define CPU_CORTEXM_PRIORITY_GROUPING   (1U)

/**
 * @brief Defines PendSV priority for nRF52 with SoftDevice
 *
 * On NRF52, when the SoftDevice is present, PendSV *must* run at the
 * and of any interrupt chain, or otherwise we will experience
 * occasional hard faults.  To run PendSV as the last interrupt, we
 * set NVIC priority grouping to 1 bit and PendSV priority to a lower
 * supriority.
 */
#define CPU_CORTEXM_PENDSV_IRQ_PRIO     (CPU_DEFAULT_IRQ_PRIO+1U)

#endif /* SOFTDEVICE_PRESENT || DOXYGEN */
/** @} */

#include "cpu_conf_common.h"

#ifdef CPU_MODEL_NRF52832XXAA
#include "vendor/nrf52.h"
#include "vendor/nrf52_bitfields.h"
#elif defined(CPU_MODEL_NRF52840XXAA)
#include "vendor/nrf52840.h"
#include "vendor/nrf52840_bitfields.h"
#else
#error "The CPU_MODEL of your board is currently not supported"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ARM Cortex-M specific CPU configuration
 * @{
 */

/**
 * @brief Define the default interrupt priority
 *
 * With the SoftDevice, the application has access to the interrupt
 * levels 2, 3, and 5-7.  The SD API calls and non-time critical
 * processing is run on interrupt level 4.  If the RIOT default IRQ
 * priority is lower than this, the system seems to become unstable.
 * Hence, with the SD, the priorities available for RIOT interrupts
 * are basically 5-7.  As we want to run PENDSV on a lower priority,
 * with priority grouping (see below), we set the default IRQ priority
 * to 6.  You may run your high priority interruts are priorites 2 or
 * 3, but you have to be very careful not to block and not to process
 * for too long.
 *
 * Handlers running at the priority levels 2 or 3 have no access to
 * SoftDevice functions.
 */
#if defined(SOFTDEVICE_PRESENT)
#define CPU_DEFAULT_IRQ_PRIO            (6U)
#else
#define CPU_DEFAULT_IRQ_PRIO            (2U)
#endif /* not SOFTDEVICE */

#define CPU_FLASH_BASE                  (0x00000000)
#ifdef CPU_MODEL_NRF52832XXAA
#define CPU_IRQ_NUMOF                   (38U)
#elif CPU_MODEL_NRF52840XXAA
#define CPU_IRQ_NUMOF                   (46U)
#endif
/** @} */

/**
 * Enables the cpu_init subfunctions in
 * cpu/cortexm_common/cortexm_init.c
 */
#define CPU_CORTEXM_INIT_SUBFUNCTIONS 1

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE                  (4096U)

#if defined(CPU_MODEL_NRF52832XXAA)
#define FLASHPAGE_NUMOF                 (128U)
#elif defined(CPU_MODEL_NRF52840XXAA)
#define FLASHPAGE_NUMOF                 (256U)
#endif
/** @} */

/**
 * @brief   SoftDevice settings
 * @{
 */

/*
 * All NVIC calls should go through the SD wrappers.
 *
 * See `cpu/cortexm_common/cortexm_init.c` for DONT_OVERRIDE_NVIC
 */
#ifdef SOFTDEVICE_PRESENT
#ifndef DONT_OVERRIDE_NVIC
#include "nrf_soc.h"
#include "nrf_nvic.h"
#undef  NVIC_EnableIRQ
#define NVIC_EnableIRQ       nrf5_sd_nvic_EnableIRQ
#undef  NVIC_DisableIRQ
#define NVIC_DisableIRQ      nrf5_sd_nvic_DisableIRQ
#undef  NVIC_GetPendingIRQ
#define NVIC_GetPendingIRQ   nrf5_sd_nvic_GetPendingIRQ
#undef  NVIC_SetPendingIRQ
#define NVIC_SetPendingIRQ   nrf5_sd_nvic_SetPendingIRQ
#undef  NVIC_ClearPendingIRQ
#define NVIC_ClearPendingIRQ nrf5_sd_nvic_ClearPendingIRQ
#undef  NVIC_SetPriority
#define NVIC_SetPriority     nrf5_sd_nvic_SetPriority
#undef  NVIC_GetPriority
#define NVIC_GetPriority     nrf5_sd_nvic_GetPriority

/**
 * @brief Calls core_panic()
 *
 * Calls core_panic() with suitable arguments.  Defined in `cpu.c`.
 */
#ifdef __GNUC__
/* Note that NORETURN may be still undefined */
__attribute__((noreturn))
#endif
void nrf5_sd_nvic_panic(unsigned int);

/*
 * Note that the sd_nvic_* functions are also inline,
 * meaning that with constant arguments the compiler
 * is often able to optimise away these calls.
 */

static inline void nrf5_sd_nvic_EnableIRQ(IRQn_Type IRQn) {
    const unsigned int err_code = sd_nvic_EnableIRQ(IRQn);
    if (NRF_SUCCESS != err_code) nrf5_sd_nvic_panic(err_code);
}

static inline void nrf5_sd_nvic_DisableIRQ(IRQn_Type IRQn) {
    const unsigned int err_code = sd_nvic_DisableIRQ(IRQn);
    if (NRF_SUCCESS != err_code) nrf5_sd_nvic_panic(err_code);
}

static inline uint32_t nrf52_sd_nvic_GetPendingIRQ(IRQn_Type IRQn) {
    uint32_t pending_irq;
    const unsigned int err_code = sd_nvic_GetPendingIRQ(IRQn, &pending_irq);
    if (NRF_SUCCESS != err_code) nrf5_sd_nvic_panic(err_code);
    return pending_irq;
}

static inline void nrf5_sd_nvic_SetPendingIRQ(IRQn_Type IRQn) {
    const unsigned int err_code = sd_nvic_SetPendingIRQ(IRQn);
    if (NRF_SUCCESS != err_code) nrf5_sd_nvic_panic(err_code);
}

static inline void nrf5_sd_nvic_ClearPendingIRQ(IRQn_Type IRQn) {
    const unsigned int err_code = sd_nvic_ClearPendingIRQ(IRQn);
    if (NRF_SUCCESS != err_code) nrf5_sd_nvic_panic(err_code);
}

static inline void nrf52_sd_nvic_SetPriority(IRQn_Type IRQn, uint32_t priority) {
    const unsigned int err_code = sd_nvic_SetPriority(IRQn, priority);
    if (NRF_SUCCESS != err_code) nrf5_sd_nvic_panic(err_code);
}

static inline uint32_t nrf52_sd_nvic_GetPriority(IRQn_Type IRQn) {
    uint32_t priority;
    const unsigned int err_code = sd_nvic_GetPriority(IRQn, &priority);
    if (NRF_SUCCESS != err_code) nrf5_sd_nvic_panic(err_code);
    return priority;
}

#endif /* DONT_OVERRIDE_NVIC */
#endif /* SOFTDEVICE_PRESENT */
/** @} */

/**
 * @brief   Put the CPU in the low-power 'wait for event' state
 */
static inline void nrf52_sleep(void)
{
    __SEV();
    __WFE();
    __asm("nop");
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
