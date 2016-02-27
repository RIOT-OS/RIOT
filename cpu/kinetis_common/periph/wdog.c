/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_wdog
 *
 * @{
 *
 * @file
 * @brief       Low-level WDOG driver implementation
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include "wdog.h"
#include "cpu_conf.h"
#include "periph_conf.h"

#ifndef KINETIS_WDOG_ADVANCED
/**
 * Attempts to determine the type of the WDOG,
 * using the WDOG_STCTRLH_CLKSRC_MASK field.
 */
#ifdef WDOG_STCTRLH_CLKSRC_MASK
#define KINETIS_WDOG_ADVANCED    1
#endif
#endif

/**
 * @brief Disable hardware watchdog.
 *
 * For advanced WDOG (mostly Kinetis MCUs with Cortex-M4 inside):
 * The following unlock sequence must be completed within 256 bus cycles or
 * the watchdog will reset the system. The watchdog is enabled by default at
 * power on.
 *
 * The sequence is:
 * 1. Write 0xC520 to the unlock register
 * 2. Write 0xD928 to the unlock register
 *
 * Watchdog is now unlocked to allow us to change its settings
 *
 * 3. Clear the WDOGEN bit of the WDOG_STCTRLH register to completely disable
 * the watchdog.
 *
 * It is now possible to single step through the code without the watchdog
 * resetting the system.
 *
 * TODO: Only disable watchdog on debug builds.
 */
void wdog_disable(void)
{
#if KINETIS_WDOG_ADVANCED
    /* unlock and disable the WDOG */
    WDOG->UNLOCK = (uint16_t)0xc520;
    WDOG->UNLOCK = (uint16_t)0xd928;
    WDOG->STCTRLH = (uint16_t)(WDOG_STCTRLH_WAITEN_MASK
                               | WDOG_STCTRLH_STOPEN_MASK
                               | WDOG_STCTRLH_ALLOWUPDATE_MASK
                               | WDOG_STCTRLH_CLKSRC_MASK);
#else
    /* disable the COP WDOG */
    SIM->COPC = (uint32_t)0x00u;
#endif
}
