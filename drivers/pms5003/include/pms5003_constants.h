/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pms5003
 *
 * @{
 * @file
 * @brief       Constants for PMS5003
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef PMS5003_CONSTANTS_H
#define PMS5003_CONSTANTS_H

#include "pms5003.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Symbol rate of the PMS5003 UART interface
 */
#define PMS5003_BAUDRATE            (9600)
/**
 * @brief   PMS5003 uart command length
 *
 *          2 + 2 + 1 + 2 (start + length + cmd + check bytes)
 */
#define PMS5003_CMD_FRAME_LEN       7U
/**
 * @brief   PMS5003 particualate matter measurements frame length
 *
 *          2 + 2x13 + 2 (start + length + data + check bytes)
 */
#define PMS5003_PM_FRAME_LEN        32U
/**
 * @brief   PMS5003 non-read cmd response frame length
 *
 *          2 + 2 + 2 + 2 (start + length + data + check bytes)
 */
#define PMS5003_RESP_FRAME_LEN      8U
/**
 * @brief   PMS5003 frame crc length, 2 bytes
 */
#define PMS5003_CRC_LEN             2U
/**
 * @brief   PMS5003 start bytes length, 2 bytes
 */
#define PMS5003_START_BYTES_LEN     2U
/**
 * @brief   PMS5003 frame length 2 bytes
 *
 *          Frame length does not include start bytes and frame length bytes
 */
#define PMS5003_FRAME_LENGTH_LEN    2U

/**
 * @brief   PMS5003 active mode fast mode min interval
 *
 * If the concentration the change is big the sensorwould be changed to fast
 * mode automatically with the interval of 200~800ms, the higher of the
 * concentration, the shorter of the interval.
 */
#define PMS5003_MODE_ACTIVE_FAST_MIN_P    (200 * US_PER_MS)

/**
 * @brief   PMS5003 active mode fast mode max interval
 *
 * If the concentration the change is big the sensorwould be changed to fast
 * mode automatically with the interval of 200~800ms, the higher of the
 * concentration, the shorter of the interval.
 */

#define PMS5003_MODE_ACTIVE_FAST_MAX_P    (800 * US_PER_MS)
/**
 * @brief   PMS5003 active mode stable
 *
 * If the concentration change is small the sensorwould run at stable mode with
 * the real interval of 2.3s
 */
#define PMS5003_MODE_ACTIVE_STABLE_P    (2300 * US_PER_MS)
/**
 * @brief     PMS5003 default FAN warmup period
 *
 * @note      Not used by the driver expected to be used by application
 *
 */
#define PMS5003_FAN_WARMUP_S       30

#ifdef __cplusplus
}
#endif

#endif /* PMS5003_CONSTANTS_H */
/** @} */
