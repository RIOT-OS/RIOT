/*
 * Copyright (C) 2017 Technische Universität Berlin
 *           (C) 2019 Inria
 *           (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_wdt WDT
 * @ingroup     drivers_periph
 * @brief       Watchdog timer peripheral driver
 *
 * A watchdog timer (WDT) is an electronic or software timer that is
 * used to detect and recover from unusual or suspect behaviour as well as
 * software malfunctions. During `NORMAL` operation the application will reset
 * wdt_kick() the timer counter preventing it from hitting 0. If due to
 * software or hardware error the WDT counter isn't reset, it will trigger
 * corrective actions, i.e. a system reboot or callback (not supported by all
 * platforms).
 *
 * This interface defines two operation modes `NORMAL` and `WINDOW`. In
 * `NORMAL` operation if the WDT counter isn't reset before `max_time` then a
 * reboot/callback is triggered. In `WINDOW` operation if a reset isn't triggered
 * or if it's triggered outside of the time `window = [min_time, max_time]` a reboot is
 * triggered.
 *
 *
 * NORMAL operation
 * ================
 *
 * In the code snippet and diagram `time` is an arbitrary value such that
 * `time < MAX_TIME`.
 *
 * @verbatim
 * 0(ms)                                            MAX_TIME(ms)
 * |----------------------------------------------------|
 *
 *                          time(ms)
 * |--------------------------| - - - - - - - - - - - - |
 *
 *                            ^
 *                            |
 *                        wdt_kick()
 * @endverbatim
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "periph/wdt.h"
 * #include "xtimer.h"
 *
 * int main(void)
 * {
 *     wdt_setup_reboot(0, MAX_TIME);
 *     wdt_start();
 *
 *     while (1) {
 *         xtimer_usleep(time);
 *         wdt_kick();
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~

 *
 * WINDOW operation
 * ==================
 *
 * In the code snippet and diagram `time` is an arbitrary value such that
 * `time < (MAX_TIME - MIN_TIME)`.
 *
 * @verbatim
 * 0(ms)                 MIN_TIME(ms)                MAX_TIME(ms)
 *  |-------------------------|-----------WINDOW----------|
 *
 *                                  time(ms)
 *                            |--------| - - - - - - - - -|
 *                                     ^
 *                                     |
 *                                wdt_kick()
 * @endverbatim
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "periph/wdt.h"
 * #include "xtimer.h"
 *
 * int main(void)
 * {
 *     wdt_setup_reboot(MIN_TIME, MAX_TIME);
 *     wdt_start();
 *
 *     while (1) {
 *         xtimer_usleep(MIN_TIME + time);
 *         wdt_kick();
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * WDT callback
 * ============
 *
 * Before reboot WDT can trigger Early Wakeup Interrupt and execute a callback
 * to perform specific safety operations of data logging before the actual reboot.
 * This function is highly platform dependent so check the platform documentation
 * for details on its constraints.
 *
 * The callback will be executed CONFIG_WDT_WARNING_PERIOD before the actual reboot.
 * The value of CONFIG_WDT_WARNING_PERIOD may be configurable or a fixed value. If
 * a platform allows this value to be configured, the feature
 * `periph_wdt_warning_period` is provided. But is in any case defined at
 * compile time. Specific platform implementation should assert improper values.
 *
 *
 * In the code snippet and diagram `time` is an arbitrary value such that
 * `time < MAX_TIME`.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "periph/wdt.h"
 * #include "xtimer.h"
 *
 * static void wdt_cb(void *arg)
 * {
 *     (void) arg;
 *      puts("wdt cb called, doing something now...");
 * }
 *
 * int main(void)
 * {
 *     wdt_setup_reboot_with_callback(0, MAX_TIME, wdt_cb, arg);
 *     wdt_start();
 *     while (1) {
 *         xtimer_usleep(time);
 *         wdt_kick();
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @verbatim
 * |---------------------MAX_TIME-----------------------|
 *                      |---CONFIG_WDT_WARNING_PERIOD---|
 *                      ^                               ^
 *                      |                               |
 *                   wdt_cb()                        reboot
 * @endverbatim
 *
 * To include this feature, (If your platform supports it) in your application
 * Makefile add:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *    USEMODULE += periph_wdt_cb
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * WDT Auto-Start
 * ==============
 *
 * It is possible to enable the Watchdog in early boot, before application startup:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *    USEMODULE += periph_wdt_auto_start
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The watchdog will automatically be initialized with the parameters
 * @ref CONFIG_PERIPH_WDT_WIN_MIN_MS and @ref CONFIG_PERIPH_WDT_WIN_MAX_MS
 *
 * It is also possible to automatically kick the watchdog.
 * This is a very non-invasive way of using the watchdog, but it is also very
 * weak as it can only detect situations where low-priority threads are
 * starved from execution and may even trigger wrongly in situations where the
 * system just experiences high load, but would otherwise have recovered on it's own.
 *
 * If you want to enable it anyway, select this module:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *    USEMODULE += auto_init_wdt_thread
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * If you are using an event thread, you can also use the watchdog to ensure that events
 * are processed in time.
 * To do so, add
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 *    USEMODULE += auto_init_wdt_event
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * @{
 *
 * @file        wdt.h
 * @brief       watchdog peripheral interface definitions
 *
 * @author      Thomas Geithner <thomas.geithner@dai-labor.de>
 *              Francisco Molina <francois-xavier.molina@inria.fr>
 *              Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef PERIPH_WDT_H
#define PERIPH_WDT_H

#include <stdint.h>
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     NWDT_TIME_LOWER_LIMIT
 *
 * @brief   Lower limit in ms for wdt operating in NORMAL mode
 */
#if defined(DOXYGEN)
#define NWDT_TIME_LOWER_LIMIT
#endif
/**
 * @def     NWDT_TIME_UPPER_LIMIT
 *
 * @brief   Upper limit in ms for wdt operating in NORMAL mode
 */
#if defined(DOXYGEN)
#define NWDT_TIME_UPPER_LIMIT
#endif
/**
 * @def     WWDT_TIME_LOWER_LIMIT
 *
 * @brief   Lower limit in ms for wdt operating in WINDOW mode
 */
#if defined(DOXYGEN)
#define WWDT_TIME_LOWER_LIMIT
#endif
/**
 * @def     WWDT_TIME_UPPER_LIMIT
 *
 * @brief   Upper limit in ms for wdt operating in WINDOW mode
 */
#if defined(DOXYGEN)
#define WWDT_TIME_UPPER_LIMIT
#endif

/**
 * @def     WDT_HAS_STOP
 *
 * @brief   Set to 1 if the platform supports wdt_stop(), 0 otherwise
 */
#ifndef WDT_HAS_STOP
#define WDT_HAS_STOP              (0)
#endif
/**
 * @def     WDT_HAS_INIT
 *
 * @brief   Set to 1 if the platform implements wdt_init(), 0 otherwise
 */
#ifndef WDT_HAS_INIT
#define WDT_HAS_INIT              (0)
#endif

/**
 * @brief   If `periph_wdt_auto_start` is used, this will be the lower bound
 *          of when the WDT can be kicked.
 */
#ifndef CONFIG_PERIPH_WDT_WIN_MIN_MS
#define CONFIG_PERIPH_WDT_WIN_MIN_MS    (0)
#endif

/**
 * @brief   If `periph_wdt_auto_start` is used, this will be the max period
 *          after which the WDT must be kicked or else it will reboot the
 *          system.
 */
#ifndef CONFIG_PERIPH_WDT_WIN_MAX_MS
#define CONFIG_PERIPH_WDT_WIN_MAX_MS    (1024)
#endif

/**
 * @brief    Start watchdog timer
 */
void wdt_start(void);

/**
 * @brief    Stop watchdog timer
 *
 * @note     Not all platforms support stopping the WDT. if WDT_HAS_STOP = 0
 *           once the wdt timer is enabled it can't be stopped.
 */
void wdt_stop(void);

/**
 * @brief    Reset the watchdog timer counter, delay system reset
 */
void wdt_kick(void);

/**
 * @brief    Set up the wdt timer.
 *
 * @note     If NORMAL watchdog only use max_time (min_time=0).<br>
 *           If WINDOW watchdog set min_time and max_time.
 *
 * @param[in] min_time       lower bound for WINDOW watchdog in ms, has to be 0
 *                           for NORMAL watchdog operation
 * @param[in] max_time       upper bound for WINDOW watchdog in ms, time before
 *                           reset for NORMAL watchdog
 */
void wdt_setup_reboot(uint32_t min_time, uint32_t max_time);

/**
 * @brief    Initialize WDT module
 *
 * @note     Only implemented and called for platforms with WDT_HAS_INIT = 1.
 *
 */
void wdt_init(void);

#if defined(MODULE_PERIPH_WDT_CB) || defined(DOXYGEN)
/**
 * @defgroup    drivers_periph_wdt_conf  WDT compile configurations
 * @ingroup     drivers_periph_wdt
 * @ingroup     config
 * @{
 */
/**
 * @def     CONFIG_WDT_WARNING_PERIOD
 *
 * @brief   Period (ms) before reboot where wdt_cb() is executed.
 *          Defined per implementation.
 */
#ifndef CONFIG_WDT_WARNING_PERIOD
#define CONFIG_WDT_WARNING_PERIOD          (1)
#endif
/** @} */

/**
 * @brief Signature for the watchdog early warning callback
 *
 * @param[in] arg           optional argument which is passed to the
 *                          callback
 */
typedef void (*wdt_cb_t)(void *arg);

/**
 * @brief    Set up the wdt timer with callback
 *
 * @note     If NORMAL watchdog only use max_time (min_time=0).<br>
 *           If WINDOW watchdog set min_time and max_time.
 *
 * @param[in] min_time       lower bound for WINDOW watchdog in ms, has to be 0
 *                           for NORMAL watchdog.
 * @param[in] max_time       upper bound for WINDOW watchdog in ms, time before
 *                           reset for NORMAL watchdog.
 * @param[in] wdt_cb         wdt callback, can be NULL
 * @param[in] arg            optional argument which is passed to the
 *                           callback, can be NULL
 */
void wdt_setup_reboot_with_callback(uint32_t min_time, uint32_t max_time,
                                    wdt_cb_t wdt_cb, void *arg);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_WDT_H */
