/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "nrf_clock.h"
#include "periph_conf.h"
#include "periph/init.h"

/* FTPAN helper functions */
static bool ftpan_32(void);
static bool ftpan_37(void);
static bool ftpan_36(void);

#ifdef SOFTDEVICE_PRESENT
#include "nrf_sdh.h"
#endif

/**
 * @brief   Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* Workaround for FTPAN-32
     * "DIF: Debug session automatically enables TracePort pins." */
    if (ftpan_32()) {
        CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
    }

    /* Workaround for FTPAN-37
     * "AMLI: EasyDMA is slow with Radio, ECB, AAR and CCM." */
    if (ftpan_37()) {
        *(volatile uint32_t *)0x400005A0 = 0x3;
    }

    /* Workaround for FTPAN-36
     * "CLOCK: Some registers are not reset when expected." */
    if (ftpan_36()) {
        NRF_CLOCK->EVENTS_DONE = 0;
        NRF_CLOCK->EVENTS_CTTO = 0;
    }

    /* initialize hf clock */
    clock_init_hf();

    /* enable instruction cache */
    NRF_NVMC->ICACHECNF = (NVMC_ICACHECNF_CACHEEN_Msk);

    /* start the Cortex-M specific configuration and initialisation */
    cortexm_init_isr_priorities();
#ifdef SOFTDEVICE_PRESENT
    const ret_code_t err_code = nrf_sdh_enable_request();
    if (err_code != NRF_SUCCESS) {
        core_panic(PANIC_GENERAL_ERROR, "Unable to enable NRF5 soft device.");
    }

    /* fixup swi0 (used as softdevice PendSV trampoline) */
    const ret_code_t err_code3 = sd_nvic_SetPriority(SWI0_EGU0_IRQn, 6);
    if (NRF_SUCCESS != err_code3) {
	core_panic(PANIC_SOFT_REBOOT, "cpu: unable to set SW0 interrupts priority with nRF SoftDevice");
    }
    const ret_code_t err_code2 = sd_nvic_EnableIRQ(SWI0_EGU0_IRQn);
    if (NRF_SUCCESS != err_code2) {
	core_panic(PANIC_SOFT_REBOOT, "cpu: unable to enable SW0 interrupts with nRF SoftDevice");
    }
#endif
    /* finalise the Cortex-M specific configuration and initialisation */
    cortexm_init_fpu();
    cortexm_init_misc();

    /* trigger static peripheral initialization */
    periph_init();
}

/**
 * @brief   Check workaround for FTPAN-32
 */
static bool ftpan_32(void)
{
    if ((((*(uint32_t *)0xF0000FE0) & 0x000000FF) == 0x6)
        && (((*(uint32_t *)0xF0000FE4) & 0x0000000F) == 0x0)) {
        if ((((*(uint32_t *)0xF0000FE8) & 0x000000F0) == 0x30)
            && (((*(uint32_t *)0xF0000FEC) & 0x000000F0) == 0x0)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief   Check workaround for FTPAN-36
 */
static bool ftpan_36(void)
{
    if ((((*(uint32_t *)0xF0000FE0) & 0x000000FF) == 0x6)
        && (((*(uint32_t *)0xF0000FE4) & 0x0000000F) == 0x0)) {
        if ((((*(uint32_t *)0xF0000FE8) & 0x000000F0) == 0x30)
            && (((*(uint32_t *)0xF0000FEC) & 0x000000F0) == 0x0)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief   Check workaround for FTPAN-37
 */
static bool ftpan_37(void)
{
    if ((((*(uint32_t *)0xF0000FE0) & 0x000000FF) == 0x6)
        && (((*(uint32_t *)0xF0000FE4) & 0x0000000F) == 0x0)) {
        if ((((*(uint32_t *)0xF0000FE8) & 0x000000F0) == 0x30)
            && (((*(uint32_t *)0xF0000FEC) & 0x000000F0) == 0x0)) {
            return true;
        }
    }
    return false;
}
