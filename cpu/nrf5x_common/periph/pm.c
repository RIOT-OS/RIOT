/*
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2014-2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       NRF5x specific power management implementations
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"

#ifdef NRF_POWER_S
#define NRF_POWER NRF_POWER_S
#endif

/* Workaround inconsistency between nRF9160 / nRF53 headers */
#ifdef REGULATORS_SYSTEMOFF_SYSTEMOFF_Enable
#define REGULATORS_SYSTEMOFF_SYSTEMOFF_Enter REGULATORS_SYSTEMOFF_SYSTEMOFF_Enable
#endif

void pm_off(void)
{
#if defined(REGULATORS_SYSTEMOFF_SYSTEMOFF_Enter)
    NRF_REGULATORS_S->SYSTEMOFF = REGULATORS_SYSTEMOFF_SYSTEMOFF_Enter;
#elif defined(CPU_FAM_NRF51)
    NRF_POWER->RAMON = 0;
#else
    for (unsigned int i = 0; i < ARRAY_SIZE(NRF_POWER->RAM); i++) {
        NRF_POWER->RAM[i].POWERCLR = (POWER_RAM_POWERCLR_S1RETENTION_Msk |
                                      POWER_RAM_POWERCLR_S0RETENTION_Msk);
    }
#endif
    while (1) {}
}
