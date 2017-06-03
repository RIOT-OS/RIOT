/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
#include "periph/init.h"
#ifdef MODULE_PERIPH_MCG
#include "mcg.h"
#endif
#if defined(MODULE_PERIPH_LLWU)
#include "llwu.h"
#elif defined(MODULE_PM_LAYERED)
#include "pm_layered.h"
#endif

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* Clear LLS protection */
    /* Clear VLPS, VLPW, VLPR protection */
    /* Note: This register can only be written once after each reset, so we must
     * enable all power modes that we wish to use. */
    SMC->PMPROT |= SMC_PMPROT_ALLS_MASK | SMC_PMPROT_AVLP_MASK;
#ifdef MODULE_PERIPH_MCG
    /* initialize the CPU clocking provided by the MCG module */
    kinetis_mcg_init();
#endif

#if defined(MODULE_PERIPH_LLWU)
    /* initialize the LLWU module for sleep/wakeup management */
    llwu_init();
#elif defined(MODULE_PM_LAYERED)
    /* Block LLS mode since we are not using the LLWU module, which is required
     * to be able to wake up from LLS */
    pm_block(KINETIS_PM_LLS);
#endif

    /* trigger static peripheral initialization */
    periph_init();
}
