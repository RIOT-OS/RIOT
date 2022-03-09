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
 * @brief       Implementation of the CPU initialization
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"
#include "nrf_clock.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

/**
 * @brief   Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize hf clock */
    clock_init_hf();

#ifdef NVMC_ICACHECNF_CACHEEN_Msk
    /* enable instruction cache */
    NRF_NVMC_S->ICACHECNF = (NVMC_ICACHECNF_CACHEEN_Msk);
#endif

    if (IS_USED(MODULE_NRFXLIB_NRF_MODEM)) {
        /* Move POWER and IPC peripherals and half of the RAM to unsecure domain
           as required by NRF modem library */
        NRF_SPU_S->PERIPHID[IPC_IRQn].PERM &= ~SPU_EXTDOMAIN_PERM_SECATTR_Msk;
        NRF_SPU_S->PERIPHID[CLOCK_POWER_IRQn].PERM &= ~SPU_EXTDOMAIN_PERM_SECATTR_Msk;
        /* Mark first half of RAM as non secure */
        for (unsigned i=0; i<15; i++)
        {
            NRF_SPU_S->RAMREGION[i].PERM &= ~SPU_RAMREGION_PERM_SECATTR_Msk;
        }
    }
    /* call cortexm default initialization */
    cortexm_init();

    /* enable wake up on events for __WFE CPU sleep */
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();
}
