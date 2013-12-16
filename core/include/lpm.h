/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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

/**
 * @brief   Available power modes
 */
enum lpm_mode {
    LPM_ON,             ///< MCU is active
    LPM_IDLE,           ///< MCU is idle
    LPM_SLEEP,          ///< MCU in sleep mode
    LPM_POWERDOWN,      ///< MCU is powered down
    LPM_OFF             ///< MCU is off
};

/**
 * @brief   Initialization of power management (including clock setup)
 *
 * This function is invoked once during boot.
 */
void lpm_init(void);

/**
 * @brief   Switches the MCU to a new power mode
 * @param[in]   target      Target power mode
 */
enum lpm_mode lpm_set(enum lpm_mode target);

void lpm_awake(void);

void lpm_begin_awake(void);
void lpm_end_awake(void);

/**
 * @brief   Returns the current power mode
 * @return  Current power mode
 */
enum lpm_mode lpm_get(void);


/** @} */
#endif /* LPM_H_ */
