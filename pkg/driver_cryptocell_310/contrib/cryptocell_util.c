/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       Setup function necessary to enable ARM CryptoCell module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 */

#include "vendor/nrf52840.h"
#include "sns_silib.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG    0
#include "debug.h"

CRYS_RND_WorkBuff_t *rndWorkBuff_ptr;
CRYS_RND_State_t *rndState_ptr;

CRYS_RND_State_t rndState = { 0 };
CRYS_RND_WorkBuff_t rndWorkBuff = { 0 };

/* Defined by the CryptoCell Library */
extern void CRYPTOCELL_IRQHandler(void);

/* This function must be defined to use the CryptoCell module on the NRF52840 board */
void isr_cryptocell(void)
{
    CRYPTOCELL_IRQHandler();
}

void cryptocell_310_enable(void)
{
    NRF_CRYPTOCELL->ENABLE = 1;
    NVIC_EnableIRQ(CRYPTOCELL_IRQn);
}

void cryptocell_310_disable(void)
{
    NRF_CRYPTOCELL->ENABLE = 0;
    NVIC_DisableIRQ(CRYPTOCELL_IRQn);
}

void driver_cryptocell_310_setup(void)
{
    int ret = 0;

    rndState_ptr = &rndState;
    rndWorkBuff_ptr = &rndWorkBuff;

    cryptocell_310_enable();

    ret = SaSi_LibInit();
    if (ret != SA_SILIB_RET_OK) {
        DEBUG("SaSi_LibInit failed: 0x%x\n", ret);
    }

    ret = CRYS_RndInit(rndState_ptr, rndWorkBuff_ptr);
    if (ret != SA_SILIB_RET_OK) {
        DEBUG("CRYS_RndInit failed: 0x%x\n", ret);
    }
}

void driver_cryptocell_310_terminate(void)
{
    int ret = 0;

    SaSi_LibFini();

    ret = CRYS_RND_UnInstantiation(rndState_ptr);
    if (ret != SA_SILIB_RET_OK) {
        DEBUG("CRYS_RND_UnInstatiation failed: 0x%x\n", ret);
    }
}
