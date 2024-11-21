/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_wdt_thread wdt thread control
 * @ingroup     sys
 * @brief       Utility functions for controlling the wdt thread
 *
 * @{
 * @file
 * @brief       WDT thread control functions
 *
 * @author      Mariem Charrada <mariem.charrada@ml-pa.com>
 */

#ifndef WDT_THREAD_H
#define WDT_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Stop the wdt thread
 *
 * This function is useful for stopping at any time the wdt thread
 */
void wdt_thread_stop(void);


#ifdef __cplusplus
}
#endif

#endif /* WDT_THREAD_H */
/** @} */
