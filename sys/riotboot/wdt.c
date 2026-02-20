/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 */

/**
 * @ingroup     sys_riotboot_wdt
 * @{
 *
 * @file
 * @brief       RIOT bootloader watchdog timer
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.loc>
 * @}
 */

#include "periph/wdt.h"

void riotboot_wdt_start(unsigned msec)
{
    if (WDT_HAS_INIT) {
        wdt_init();
    }
    wdt_setup_reboot(0, msec);
    wdt_start();
}

void riotboot_wdt_stop(void)
{
    wdt_stop();
}
