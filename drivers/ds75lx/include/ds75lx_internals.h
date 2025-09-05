/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ds75lx
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the DS75LX sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   DS75LX I2C address (7 bit address)
 */
#define DS75LX_ADDR                     (0x48)

/**
 * @name    DS75LX registers
 * @{
 */
#define DS75LX_REG_TEMPERATURE          (0x00)
#define DS75LX_REG_CONFIGURATION        (0x01)
#define DS75LX_REG_T_HYST               (0x02)
#define DS75LX_REG_T_OS                 (0x03)
/** @} */

/**
 * @name    Configuration register bits
 * @{
 */
#define DS75LX_CONF_SD_POS              (0)
#define DS75LX_CONF_TM_POS              (1)
#define DS75LX_CONF_POL_POS             (2)
#define DS75LX_CONF_F0_POS              (3)
#define DS75LX_CONF_F0_MASK             (0x18)
#define DS75LX_CONF_R0_POS              (5)
#define DS75LX_CONF_R0_MASK             (0x60)
/** @} */

/**
 * @brief   Max conversion time unit (ms)
 */
#define DS75LX_MAX_CONVERSION_TIME      (25U)

#ifdef __cplusplus
}
#endif

/** @} */
