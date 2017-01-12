/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf51
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "periph/pm.h"
#include "cpu.h"

void pm_off(void)
{
    NRF_POWER->RAMON = 0;
    NRF_POWER->SYSTEMOFF = 1;
}
