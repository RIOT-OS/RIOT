/*
 * Copyright (C) 2017 Technische Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        driver_periph_wdt WDT
 * @{
 *
 * @file
 * @brief           WDT peripheral driver interface definition
 *
 * @author          Thomas Geithner <thomas.geithner@dai-labor.de>
 */

#ifndef PERIPH_WDT_H
#define PERIPH_WDT_H

#include <stdint.h>

/**
 * @brief   Possible WDT timing modes
 */
typedef enum {
    WDT_EXACT = 0,   /**< WDT interval shall be exact as requested */
    WDT_MIN   = 1,   /**< WDT interval shall be >= than requested */
    WDT_MAX   = 2,   /**< WDT interval shall be <= than requested */
} wdt_mode_t;

/**
 * @brief   Initialize the Watchdog Timer
 *
 * The watchdog timer (WDT) is initialized preferred interval in microseconds.
 * Depending on the hardware, this exact interval might be not feasible. So,
 * the initialization routine select the best suitable time (real WDT interval)
 * depending on the selected WDT mode:
 *
 * WDT_EXACT: the real WDT interval shall be exact as specified by the preferred
 *            interval (within the bounds of the timer resolution)
 *
 * WDT_MIN:   the real WDT interval shall be >= than the preferred interval,
 *            or the maximum possible one
 *
 * WDT_MAX:   the real WDT interval shall be <= than the preferred interval,
 *            or the minimum possible one
 *
 *
 * The function initializes but does not start the WDT.
 *
 *
 * @param[in] t_wdt         preferred WDT interval in microseconds
 *
 * @return                  -1 on error
 * @return                  applied WDT interval in microseconds
 */
int wdt_init(uint32_t t_wdt, wdt_mode_t mode);


/**
 * @brief   Enables the Watchdog Timer
 *
 * Enables the WDT. From this time on, the WDT has to be reseted using
 * wdt_reset() in intervals shorter than the real WDT interval
 * returned by wdt_init() or wdt_init_max() to avoid a system
 * reboot.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int wdt_enable(void);

/**
 * @brief   Disables the Watchdog Timer
 *
 * Disables the WDT. If successful, resetting the WDT with wdt_reset()
 * is not longer required.
 * Depending on the hardware, this might not be possible; some implementations
 * do not allow disabling a started WDT.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int wdt_disable(void);

/**
 * @brief   Checks, if the Watchdog Timer is enabled
 *
 * Checks, if the Watchdog Timer is enabled
 *
 * @return                  0 if the WDT is disabled
 * @return                  1 if the WDT is enabled
 */
int wdt_is_enabled(void);

/**
 * @brief   Resets the Watchdog Timer
 *
 * Resets the WDT.
 *
 */
void wdt_reset(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_WDT_H */
