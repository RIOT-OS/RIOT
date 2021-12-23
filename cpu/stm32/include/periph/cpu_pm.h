/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef PERIPH_CPU_PM_H
#define PERIPH_CPU_PM_H

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
#define PM_NUM_MODES    (2U)

/**
 * @name    Power modes
 * @{
 */
#define STM32_PM_STOP         (1U)  /**< Index of STOP mode */
#define STM32_PM_STANDBY      (0U)  /**< Index of STANDBY mode */
/** @} */

#ifndef PM_EWUP_CONFIG
/**
 * @brief   Wake-up pins configuration (CSR register)
 */
#define PM_EWUP_CONFIG          (0U)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_PM_H */
/** @} */
