/*
 * SPDX-FileCopyrightText: 2017 SKF AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_kinetis
 * @{
 *
 * @file
 * @brief       Kinetis CPU initialization
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */

#include "cpu.h"
#include "kernel_init.h"
#include "periph/init.h"
#include "stdio_base.h"
#ifdef MODULE_PERIPH_MCG
#include "mcg.h"
#endif

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
#ifdef SMC
    /* Clear LLS protection */
    /* Clear VLPS, VLPW, VLPR protection */
    /* Note: This register can only be written once after each reset, so we must
     * enable all power modes that we wish to use. */
    SMC->PMPROT |= SMC_PMPROT_ALLS_MASK | SMC_PMPROT_AVLP_MASK;
#endif
#ifdef MODULE_PERIPH_MCG
    /* initialize the CPU clocking provided by the MCG module */
    kinetis_mcg_init();
#endif

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}
