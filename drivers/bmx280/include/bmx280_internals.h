/*
 * SPDX-FileCopyrightText: 2016 Kees Bakker, SODAQ
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_bmx280
 * @brief       Internal addresses, registers, constants for the BMX280 family sensors.
 * @{
 * @file
 * @brief       Internal addresses, registers, constants for the BMX280 family sensors.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Device specific chip ID
 * @{
 */
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
#define BMX280_CHIP_ID_VAL              (0x60)
#else
#define BMX280_CHIP_ID_VAL              (0x58)
#endif
/** @} */

/**
 * @name    BME280 registers
 * @{
 */
#define BMX280_CHIP_ID_REG              (0xD0)
#define BMEX80_RST_REG                  (0xE0)      /* Softreset Reg */

#define BMX280_DIG_T1_LSB_REG           (0x88)
#define BMX280_DIG_T1_MSB_REG           (0x89)
#define BMX280_DIG_T2_LSB_REG           (0x8A)
#define BMX280_DIG_T2_MSB_REG           (0x8B)
#define BMX280_DIG_T3_LSB_REG           (0x8C)
#define BMX280_DIG_T3_MSB_REG           (0x8D)
#define BMX280_DIG_P1_LSB_REG           (0x8E)
#define BMX280_DIG_P1_MSB_REG           (0x8F)
#define BMX280_DIG_P2_LSB_REG           (0x90)
#define BMX280_DIG_P2_MSB_REG           (0x91)
#define BMX280_DIG_P3_LSB_REG           (0x92)
#define BMX280_DIG_P3_MSB_REG           (0x93)
#define BMX280_DIG_P4_LSB_REG           (0x94)
#define BMX280_DIG_P4_MSB_REG           (0x95)
#define BMX280_DIG_P5_LSB_REG           (0x96)
#define BMX280_DIG_P5_MSB_REG           (0x97)
#define BMX280_DIG_P6_LSB_REG           (0x98)
#define BMX280_DIG_P6_MSB_REG           (0x99)
#define BMX280_DIG_P7_LSB_REG           (0x9A)
#define BMX280_DIG_P7_MSB_REG           (0x9B)
#define BMX280_DIG_P8_LSB_REG           (0x9C)
#define BMX280_DIG_P8_MSB_REG           (0x9D)
#define BMX280_DIG_P9_LSB_REG           (0x9E)
#define BMX280_DIG_P9_MSB_REG           (0x9F)

#define BME280_DIG_H1_REG               (0xA1)
#define BME280_DIG_H2_LSB_REG           (0xE1)
#define BME280_DIG_H2_MSB_REG           (0xE2)
#define BME280_DIG_H3_REG               (0xE3)
#define BME280_DIG_H4_MSB_REG           (0xE4)      /* H4[11:4] */
#define BME280_DIG_H4_H5_REG            (0xE5)      /* H5[3:0]  H4[3:0] */
#define BME280_DIG_H5_MSB_REG           (0xE6)      /* H5[11:4] */
#define BME280_DIG_H6_REG               (0xE7)

#define BMX280_STAT_REG                 (0xF3)      /* Status Reg */
#define BMX280_CTRL_MEAS_REG            (0xF4)      /* Ctrl Measure Reg */
#define BMX280_CONFIG_REG               (0xF5)      /* Configuration Reg */
#define BMX280_PRESSURE_MSB_REG         (0xF7)      /* Pressure MSB */
#define BMX280_PRESSURE_LSB_REG         (0xF8)      /* Pressure LSB */
#define BMX280_PRESSURE_XLSB_REG        (0xF9)      /* Pressure XLSB */
#define BMX280_TEMPERATURE_MSB_REG      (0xFA)      /* Temperature MSB */
#define BMX280_TEMPERATURE_LSB_REG      (0xFB)      /* Temperature LSB */
#define BMX280_TEMPERATURE_XLSB_REG     (0xFC)      /* Temperature XLSB */
#define BME280_CTRL_HUM_REG             (0xF2)      /* Ctrl Humidity Reg */
#define BME280_HUMIDITY_MSB_REG         (0xFD)      /* Humidity MSB */
#define BME280_HUMIDITY_LSB_REG         (0xFE)      /* Humidity LSB */
/** @} */

/**
 * @name    Bitmasks for selected registers
 * @{
 */
#define MEAS_OSRS_T_POS                 (5U)
#define MEAS_OSRS_P_POS                 (2U)
#define STAT_MEASURING                  (1 << 3)
#define RESET_WORD                      (0xB6)
/** @} */

/**
 * @name    The base address for the row of data registers
 * @{
 */
#define DATA_BASE                       BMX280_PRESSURE_MSB_REG
/** @} */

/**
 * @name    Calibration data base addresses, block sizes, and offsets
 * @{
 */
#define CALIB_T_P_BASE                  (BMX280_DIG_T1_LSB_REG)
#define CALIB_T_P_LEN                   (24U)
#define OFFSET_T_P(x)                   (x - CALIB_T_P_BASE)
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
#define CALIB_H_BASE                    (BME280_DIG_H2_LSB_REG)
#define CALIB_H_LEN                     (7U)
#define OFFSET_H(x)                     (x - CALIB_H_BASE)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
