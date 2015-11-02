/*
 * Copyright (C) Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include "cpu.h"

void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    /* SSW will only set IDIV values up to 16. If a value > 16 is
     * defined, this sets the IDIV manually to the desired value. */
#if (XMC_CLK_IDIV > 16)
    SCU_CLK->CLKCR |= XMC_CLK_IDIV << SCU_CLK_CLKCR_IDIV_Pos;

    /* wait while voltage regulator is busy doing what it does */
    while ((SCU_CLK->CLKCR & SCU_CLK_CLKCR_VDDC2HIGH_Msk) ||
           (SCU_CLK->CLKCR & SCU_CLK_CLKCR_VDDC2LOW_Msk)) ;
#endif
}
