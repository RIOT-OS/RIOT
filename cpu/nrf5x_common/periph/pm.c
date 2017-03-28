/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
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

void pm_off(void)
{
#ifdef CPU_FAM_NRF51
    NRF_POWER->RAMON = 0;
#else
    for (int i = 0; i < 8; i++) {
        NRF_POWER->RAM[i].POWERCLR = (POWER_RAM_POWERCLR_S1RETENTION_Msk |
                                      POWER_RAM_POWERCLR_S0RETENTION_Msk);
    }
#endif
    NRF_POWER->SYSTEMOFF = 1;
    while(1) {}
}
