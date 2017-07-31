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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief   Possible WDT timing modes
 */
typedef enum {
    WDT_EXACT,   /**< WDT interval shall be exact as requested */
    WDT_MIN,     /**< WDT interval shall be >= than requested */
    WDT_MAX,     /**< WDT interval shall be <= than requested */
} wdt_timing_t;

/**
 * @brief   Possible WDT return values
 */
enum {
    WDT_OK         =  0,   /**< everything in order */
    WDT_ERR        = -1,   /**< general WDT error */
    WDT_ERR_NO_IRQ = -2,   /**< no support for IRQ mode */
};

/**
 * @brief   Possible WDT callback return values
 */
typedef enum {
    WDT_CB_RESET    =  0,   /**< system shall reset after returning from ISR */
    WDT_CB_NO_RESET = -1,   /**< system shall not reset after returning from ISR */
} wdt_isr_return_t;

/**
 * @brief   Signature for WDT interrupt callback
 *
 * @param[in] arg           context to the callback (optional)
 *
 *
 * @return                  WDT_CB_RESET system is reset after returning from callback
 * @return                  WDT_CB_NO_RESET system does not reset after returning from callback
 */
typedef wdt_isr_return_t(*wdt_cb_t)(void *arg);

/**
 * @brief   Initialize the Watchdog Timer
 *
 * The watchdog timer (WDT) is initialized preferred interval in microseconds.
 * Depending on the hardware, this exact interval might be not feasible. So,
 * the initialization routine select the best suitable time (real WDT interval)
 * depending on the selected WDT timing mode:
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
 * @return                  WDT_ERR on error
 * @return                  applied WDT interval in microseconds
 */
int wdt_init(uint32_t t_wdt, wdt_timing_t timing);

/**
 * @brief   Initialize the Watchdog Timer
 *
 * Initialized the watchdog timer (WDT) similarly to wdt_init(), but with an
 * additional callback function which is executed when a WDT interrupt is
 * triggered. After returning from the callback function, a system reset is
 * performed.
 *
 *
 * @param[in] t_wdt         preferred WDT interval in microseconds
 * @param[in] wdt_cb        WDT interrupt callback function
 * @param[in] arg           optional context passed to the callback functions
 *
 * @return                  WDT_ERR_NO_IRQ if WDT interrupts are not supported
 * @return                  WDT_ERR on other errors
 * @return                  applied WDT interval in microseconds
 */
int wdt_init_cb(uint32_t t_wdt, wdt_timing_t timing, wdt_cb_t wdt_cb, void* arg);

/**
 * @brief   Enables the Watchdog Timer
 *
 * Enables the WDT. From this time on, the WDT has to be reseted using
 * wdt_reset() in intervals shorter than the real WDT interval
 * returned by wdt_init() or wdt_init_max() to avoid a system
 * reboot.
 *
 * @return                  WDT_OK on success
 * @return                  WDT_ERR on error
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
 * @return                  WDT_OK on success
 * @return                  WDT_ERR on error
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
