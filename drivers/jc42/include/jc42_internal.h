/*
 * Copyright (C) 2017 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_jc42
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for jc42 sensors.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    JC42 register addresses
 * @{
 */
#define JC42_REG_CAP                    (0x00)
#define JC42_REG_CONFIG                 (0x01)
#define JC42_REG_TEMP_UPPER             (0x02)
#define JC42_REG_TEMP_LOWER             (0x03)
#define JC42_REG_TEMP_CRITICAL          (0x04)
#define JC42_REG_TEMP                   (0x05)
#define JC42_REG_MANID                  (0x06)
#define JC42_REG_DEVICEID               (0x07)
/** @} */

/**
 * @name    JC42 constants
 * @{
 */
#define JC42_BUS_FREE_TIME_US           (1U)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
