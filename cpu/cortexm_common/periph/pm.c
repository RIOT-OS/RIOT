/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#ifdef NEEDS_CORTEXM_PM_FALLBACK
#define NEEDS_CORTEXM_PM_SET_LOWEST
#define NEEDS_CORTEXM_PM_REBOOT
#endif

#if defined(NEEDS_CORTEXM_PM_SET_LOWEST)
void pm_set_lowest(void)
{
    cortexm_sleep(0);
}
#endif

#if defined(NEEDS_CORTEXM_PM_REBOOT)
void pm_reboot(void)
{
    NVIC_SystemReset();
}
#endif
