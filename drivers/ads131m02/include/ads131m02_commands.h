/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ads131m02
 * @{
 *
 * @file
 * @brief       Commands for the ADS131M02 ADC
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name ADS131M02 Commands
 * @{
 */
/**
 * @brief   Reset command
 */
#define ADS131M02_CMD_RESET                 (BIT4 | BIT0)
/**
 * @brief   Standby command
 */
#define ADS131M02_CMD_STANDBY               (BIT5 | BIT1)
/**
 * @brief   Wakeup command
 */
#define ADS131M02_CMD_WAKEUP                (BIT5 | BIT4 | BIT1 | BIT0)
/**
 * @brief   Lock command
 */
#define ADS131M02_CMD_LOCK                  (BIT10 | BIT8 | BIT6 | BIT4 | BIT2 | BIT0)
/**
 * @brief   Unlock command
 */
#define ADS131M02_CMD_UNLOCK                (BIT10 | BIT9 | BIT6 | BIT4 | BIT2 | BIT0)
/**
 * @brief   Read register command
 */
#define ADS131M02_CMD_RREG(a, n)            (BIT15 | BIT13 | ((a) << 7) | ((n) - 1))
/**
 * @brief   Write register command
 */
#define ADS131M02_CMD_WREG(a, n)            (BIT14 | BIT13 | ((a) << 7) | ((n) - 1))
/** @} */

/**
 * @name ADS131M02 Command Responses
 * @{
 */
/**
 * @brief   Response to reset command
 */
#define ADS131M02_RSP_RESET                  (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | \
                                              BIT10 | BIT9 | BIT8 | BIT5 | BIT1)
/**
 * @brief   Response to standby command
 */
#define ADS131M02_RSP_STANDBY                (BIT5 | BIT1)
/**
 * @brief   Response to wakeup command
 */
#define ADS131M02_RSP_WAKEUP                 (BIT5 | BIT4 | BIT1 | BIT0)
/**
 * @brief   Response to lock command
 */
#define ADS131M02_RSP_LOCK                   (BIT10 | BIT8 | BIT6 | BIT4 | BIT2 | BIT0)
/**
 * @brief   Response to unlock command
 */
#define ADS131M02_RSP_UNLOCK                 (BIT10 | BIT9 | BIT6 | BIT4 | BIT2 | BIT0)
/** @brief   Response to read register command
 *
 * @param   a   Register address
 * @param   n   Number of registers to read
 */
#define ADS131M02_RSP_RREG(a, n)             (BIT15 | BIT14 | BIT13 | ((a) << 7) | ((n) - 1))
/** @brief   Response to write register command
 *
 * @param   a   Register address
 * @param   n   Number of registers to write
 */
#define ADS131M02_RSP_WREG(a, n)             (BIT14 | ((a) << 7) | ((n) - 1))
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
