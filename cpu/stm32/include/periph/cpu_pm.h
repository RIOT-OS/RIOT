/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Power Management (PM) CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    PM definitions
 * @{
 */
/**
 * @brief   Number of usable low power modes
 */
#define PM_NUM_MODES            (3U)

/**
 * @name    Power modes
 * @{
 */
#define STM32_PM_IDLE           (2U)  /**< Index of IDLE mode */
#define STM32_PM_STOP           (1U)  /**< Index of STOP mode */
#define STM32_PM_STANDBY        (0U)  /**< Index of STANDBY mode */
/** @} */

#ifndef PM_EWUP_CONFIG
/**
 * @brief   Wake-up pins configuration (CSR register)
 */
#define PM_EWUP_CONFIG          (0U)
#endif
/** @} */

/**
 * @brief   Check whether the backup domain voltage regulator is on
 */
bool pm_backup_regulator_is_on(void);

/**
 * @brief   Enable the backup domain voltage regulator to retain backup
 *          register content during standby and VBAT mode
 */
void pm_backup_regulator_on(void);

/**
 * @brief   Disable the backup domain voltage regulator
 */
void pm_backup_regulator_off(void);

#ifdef __cplusplus
}
#endif

/** @} */
