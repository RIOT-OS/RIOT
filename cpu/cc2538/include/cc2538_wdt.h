/*
 * Copyright (C) 2017 Technische Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc2538_wdt CC2538 WDT
 * @{
 *
 * @file
 * @brief           CC2538 WDT interface
 *
 * @author          Thomas Geithner <thomas.geithner@dai-labor.de>
 */

#ifndef CC2538_WDT_H
#define CC2538_WDT_H

#include <stdint.h>

/**
 * @brief   Initialize the Watchdog Timer
 *
 * The watchdog timer (WDT) is initialized preferred interval in microseconds.
 * Depending on the hardware, this exact interval might be not feasible. So,
 * the initialization routine select the best suitable time (real WDT interval).
 * If possible, the real WDT interval should not be shorter than the given
 * argument, or otherwise, the maximum possible and interval is used.
 * The real WDT interval selected by the routine is returned (in microseconds).
 *
 * The function initializes but does not start the WDT.
 *
 *
 * @param[in] t_wdt         preferred WDT interval in microseconds
 *
 * @return                  -1 on error
 * @return                  applied WDT interval in microseconds
 */
int cc2538_wdt_init(uint32_t t_wdt);


/**
 * @brief   Initialize the Watchdog Timer
 *
 * Initializes the WDT similarly to cc2538_wdt_init(), but in contrast the
 * real WDT interval should not be longer than the given argument. If this is
 * not possible, the shortest available interval is used.
 *
 *
 * @param[in] t_wdt         preferred WDT interval in microseconds
 *
 * @return                  -1 on error
 * @return                  applied WDT interval in microseconds
 */
int cc2538_wdt_init_max(uint32_t t_wdt);

/**
 * @brief   Enables the Watchdog Timer
 *
 * Enables the WDT. From this time on, the WDT has to be reseted using
 * cc2538_wdt_reset() in intervals shorter than the real WDT interval
 * returned by cc2538_wdt_init() or cc2538_wdt_init_max() to avoid a system
 * reboot.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int cc2538_wdt_enable(void);

/**
 * @brief   Disables the Watchdog Timer
 *
 * Disables the WDT. If successful, resetting the WDT with cc2538_wdt_reset()
 * is not longer required.
 * Depending on the hardware, this might not be possible; some implementations
 * do not allow disabling a started WDT.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int cc2538_wdt_disable(void);

/**
 * @brief   Checks, if the Watchdog Timer is enabled
 *
 * Checks, if the Watchdog Timer is enabled
 *
 * @return                  0 if the WDT is disabled
 * @return                  1 if the WDT is enabled
 */
int cc2538_wdt_is_enabled(void);

/**
 * @brief   Resets the Watchdog Timer
 *
 * Resets the WDT.
 *
 */
void cc2538_wdt_reset(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC2538_WDT_H */
