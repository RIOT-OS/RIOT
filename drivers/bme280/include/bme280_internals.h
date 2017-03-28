/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bme280
 * @brief       Internal addresses, registers, constants for the BME280 sensor.
 * @{
 * @file
 * @brief       Internal addresses, registers, constants for the BME280 sensor.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 */

#ifndef BME280_INTERNALS_H
#define BME280_INTERNALS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name BME280 registers
 * @{
 */
#define BME280_DIG_T1_LSB_REG                   0x88
#define BME280_DIG_T1_MSB_REG                   0x89
#define BME280_DIG_T2_LSB_REG                   0x8A
#define BME280_DIG_T2_MSB_REG                   0x8B
#define BME280_DIG_T3_LSB_REG                   0x8C
#define BME280_DIG_T3_MSB_REG                   0x8D
#define BME280_DIG_P1_LSB_REG                   0x8E
#define BME280_DIG_P1_MSB_REG                   0x8F
#define BME280_DIG_P2_LSB_REG                   0x90
#define BME280_DIG_P2_MSB_REG                   0x91
#define BME280_DIG_P3_LSB_REG                   0x92
#define BME280_DIG_P3_MSB_REG                   0x93
#define BME280_DIG_P4_LSB_REG                   0x94
#define BME280_DIG_P4_MSB_REG                   0x95
#define BME280_DIG_P5_LSB_REG                   0x96
#define BME280_DIG_P5_MSB_REG                   0x97
#define BME280_DIG_P6_LSB_REG                   0x98
#define BME280_DIG_P6_MSB_REG                   0x99
#define BME280_DIG_P7_LSB_REG                   0x9A
#define BME280_DIG_P7_MSB_REG                   0x9B
#define BME280_DIG_P8_LSB_REG                   0x9C
#define BME280_DIG_P8_MSB_REG                   0x9D
#define BME280_DIG_P9_LSB_REG                   0x9E
#define BME280_DIG_P9_MSB_REG                   0x9F

#define BME280_DIG_H1_REG                       0xA1

#define BME280_CHIP_ID                          0x60    /* The identifier of the BME280 */
#define BME280_CHIP_ID_REG                      0xD0
#define BME280_RST_REG                          0xE0 /* Softreset Reg */

#define BME280_DIG_H2_LSB_REG                   0xE1
#define BME280_DIG_H2_MSB_REG                   0xE2
#define BME280_DIG_H3_REG                       0xE3
#define BME280_DIG_H4_MSB_REG                   0xE4 /* H4[11:4] */
#define BME280_DIG_H4_H5_REG                    0xE5 /* H5[3:0]  H4[3:0] */
#define BME280_DIG_H5_MSB_REG                   0xE6 /* H5[11:4] */
#define BME280_DIG_H6_REG                       0xE7

#define BME280_CTRL_HUMIDITY_REG                0xF2 /* Ctrl Humidity Reg */
#define BME280_STAT_REG                         0xF3 /* Status Reg */
#define BME280_CTRL_MEAS_REG                    0xF4 /* Ctrl Measure Reg */
#define BME280_CONFIG_REG                       0xF5 /* Configuration Reg */
#define BME280_PRESSURE_MSB_REG                 0xF7 /* Pressure MSB */
#define BME280_PRESSURE_LSB_REG                 0xF8 /* Pressure LSB */
#define BME280_PRESSURE_XLSB_REG                0xF9 /* Pressure XLSB */
#define BME280_TEMPERATURE_MSB_REG              0xFA /* Temperature MSB */
#define BME280_TEMPERATURE_LSB_REG              0xFB /* Temperature LSB */
#define BME280_TEMPERATURE_XLSB_REG             0xFC /* Temperature XLSB */
#define BME280_HUMIDITY_MSB_REG                 0xFD /* Humidity MSB */
#define BME280_HUMIDITY_LSB_REG                 0xFE /* Humidity LSB */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BME280_INTERNALS_H */
/** @} */
