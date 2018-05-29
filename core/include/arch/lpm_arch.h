/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file
 * @brief       Architecture dependent interface for power mode management
 *
 * This file acts as a wrapper between the kernels power management interface and the architecture
 * dependent implementation of power management.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LPM_ARCH_H
#define LPM_ARCH_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Define the mapping between the architecture independent interfaces
          and the kernel internal interfaces
 *
 * This mapping is done for compatibility of existing platforms,
 * new platforms should always use the *_arch_* interfaces.
 * @{
 */
#ifdef COREIF_NG
#define lpm_init            lpm_arch_init
#define lpm_set             lpm_arch_set
#define lpm_get             lpm_arch_get
#define lpm_awake           lpm_arch_awake
#define lpm_begin_awake     lpm_arch_begin_awake
#define lpm_end_awake       lpm_arch_end_awake
#endif
/** @} */

/**
 * @name Available power modes
 */
enum lpm_mode {
    LPM_ON,             /**< MCU is active */
    LPM_IDLE,           /**< MCU is idle */
    LPM_SLEEP,          /**< MCU in sleep mode */
    LPM_POWERDOWN,      /**< MCU is powered down */
    LPM_OFF,            /**< MCU is off */
    LPM_UNKNOWN = -1    /**< status unknown/unavailable */
};

/**
 * @brief Initialize the controller power management
 */
void lpm_arch_init(void);

/**
 * @brief Try to set the controller to a given power mode
 *
 * @param[in] target    the desired power mode
 *
 * @return              the power mode that was actually set
 */
enum lpm_mode lpm_arch_set(enum lpm_mode target);

/**
 * @brief Get the controller's current power mode
 *
 * @return              the power mode the controller is currently in
 */
enum lpm_mode lpm_arch_get(void);

/**
 * @brief Wakeup the controller from a low-power sleep mode
 */
void lpm_arch_awake(void);

/**
 * @brief This hook is called on the beginning of a wake-up phase
 */
void lpm_arch_begin_awake(void);

/**
 * @brief This hook is called on the end of a wake-up phase
 */
void lpm_arch_end_awake(void);

#ifdef __cplusplus
}
#endif

#endif /* LPM_ARCH_H */
/** @} */
