/*
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cortexm_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       common periph/pm functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/pm.h"

#ifdef PROVIDES_PM_SET_LOWEST_CORTEXM
void pm_set_lowest(void)
{
    /* don't do anything here */
}
#endif
