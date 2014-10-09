/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_lpm Power Management
 * @ingroup     core
 * @brief       The kernels power management interface
 * @{
 *
 * @file        lpm.h
 * @brief       Power management interface
 *
 * This interface needs to be implemented for each platform.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 */

#ifndef LPM_H_
#define LPM_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "arch/lpm_arch.h"

/**
 * @brief   Initialization of power management (including clock setup)
 *
 * This function is invoked once during boot.
 */
void lpm_init(void);

/**
 * @brief   Switches the MCU to a new power mode
 * @param[in]   target      Target power mode
 * @return                  The previous power mode
 */
enum lpm_mode lpm_set(enum lpm_mode target);

/**
 * @brief   Switches the MCU to active power mode LPM_ON
 */
void lpm_awake(void);

/**
 * @brief   Begin to switch MCU to active power mode.
 */
void lpm_begin_awake(void);

/**
 * @brief   Finish to switch MCU to active power mode.
 */
void lpm_end_awake(void);

/**
 * @brief   Returns the current power mode
 * @return  Current power mode
 */
enum lpm_mode lpm_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __LPM_H_ */
/** @} */
