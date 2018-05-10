/*
 * Copyright (C) 2017 SKF AB
 * Copyright (C) 2018 Ishraq Ibne Ashraf
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
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"
#include "mcg.h"

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

/* Kinetis E series doesn't have SMC and MCG units */
#if !defined(KINETIS_SERIES_E)
    /* Clear LLS protection */
    /* Clear VLPS, VLPW, VLPR protection */
    /* Note: This register can only be written once after each reset, so we must
     * enable all power modes that we wish to use. */
    SMC->PMPROT |= SMC_PMPROT_ALLS_MASK | SMC_PMPROT_AVLP_MASK;
    /* initialize the CPU clocking provided by the MCG module */
    kinetis_mcg_init();
#endif /* !defined(KINETIS_SERIES_E) */

    /* trigger static peripheral initialization */
    periph_init();
}
