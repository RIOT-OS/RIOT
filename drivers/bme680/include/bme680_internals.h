/*
 * Copyright (C) 2020 OVGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sensors
 * @brief       Internal addresses, registers, constants for the BME680 sensor.
 *
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the BME680 sensor.
 * 
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>
 */

#ifndef BME680_INTERNALS_H
#define BME680_INTERNALS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    I2C address when SDO pin is LOW
 */
#define BME680_I2C_ADDR_1   (0x76)

/**
 * @brief    I2C address when SDO pin is HIGH
 */
#define BME680_I2C_ADDR_2   (0x77)

/**
 * @brief    SPI settings
 */
#define BME680_SPI_SPEED    (SPI_CLK_1MHZ)
#define BME680_SPI_MODE     (SPI_MODE_0)
#define BME680_SPI_PAGE_0     (0b00000000)
#define BME680_SPI_PAGE_1     (0b00010000)

#define BME680_CHIP_ID              (0x61)

/**
 * @brief   registers
 */

#define BME680_REGISTER_RESET       (0xE0)
#define BME680_REGISTER_CHIP_ID     (0xD0)

#define BME680_REGISTER_CTRL_HUM    (0x72)
#define BME680_REGISTER_CTRL_MEAS   (0x74)
#define BME680_REGISTER_CTRL_GAS    (0x70)
#define BME680_REGISTER_GAS_WAIT_0  (0x5A)
#define BME680_REGISTER_RES_HEAT_0  (0x64)
#define BME680_REGISTER_CTRL_GAS_1  (0x71)
#define BME680_REGISTER_CONFIG      (0x75)

#define BME680_REGISTER_MEAS_STATUS_0   (0x1D)

#define BME680_REGISTER_PAR_T1      (0xE9)
#define BME680_REGISTER_PAR_T2      (0x8A)
#define BME680_REGISTER_PAR_T3      (0x8C)
#define BME680_REGISTER_TEMP_ADC_XLSB    (0x24)
#define BME680_REGISTER_TEMP_ADC_LSB    (0x23)
#define BME680_REGISTER_TEMP_ADC_MSB    (0x22)

#define BME680_REGISTER_PAR_H1_H2_LSB   (0xE2)
#define BME680_REGISTER_PAR_H1_MSB      (0xE3)
#define BME680_REGISTER_PAR_H2_MSB      (0xE1)
#define BME680_REGISTER_PAR_H3      (0xE4)
#define BME680_REGISTER_PAR_H4      (0xE5)
#define BME680_REGISTER_PAR_H5      (0xE6)
#define BME680_REGISTER_PAR_H6      (0xE7)
#define BME680_REGISTER_PAR_H7      (0xE8)
#define BME680_REGISTER_HUM_ADC_LSB     (0x26)
#define BME680_REGISTER_HUM_ADC_MSB     (0x25)

#define BME680_REGISTER_PAR_P1      (0x8E)
#define BME680_REGISTER_PAR_P2      (0x90)
#define BME680_REGISTER_PAR_P3      (0x92)
#define BME680_REGISTER_PAR_P4      (0x94)
#define BME680_REGISTER_PAR_P5      (0x96)
#define BME680_REGISTER_PAR_P6      (0x99)
#define BME680_REGISTER_PAR_P7      (0x98)
#define BME680_REGISTER_PAR_P8      (0x9C)
#define BME680_REGISTER_PAR_P9      (0x9E)
#define BME680_REGISTER_PAR_P10      (0xA0)
#define BME680_REGISTER_PRESS_ADC_XLSB     (0x21)
#define BME680_REGISTER_PRESS_ADC_LSB     (0x20)
#define BME680_REGISTER_PRESS_ADC_MSB     (0x1F)

#define BME680_REGISTER_PAR_G1      (0xED)
#define BME680_REGISTER_PAR_G2      (0xEB)
#define BME680_REGISTER_PAR_G3      (0xEE)
#define BME680_REGISTER_RES_HEAT_RANGE    (0x02)
#define BME680_REGISTER_RES_HEAT_VAL    (0x00)
#define BME680_REGISTER_GAS_R_LSB    (0x2B)

#define BME680_REGISTER_SPI_MEM_PAGE    (0x73)

#define BME680_REGISTER_GAS_ADC_MSB     (0x2A)
#define BME680_REGISTER_GAS_ADC_LSB     (0x2B)
#define BME680_REGISTER_GAS_RANGE       (0x2B)
#define BME680_REGISTER_RANGE_SWITCHING_ERROR   (0x04)

/*
* @brief    data status
*/
#define BME680_NEW_DATA                 (0x80)
#define BME680_GAS_MEASUREMENT_SUCCESS  (0x30)

/*
* @brief    values for calculating gas resistance
*/
#define CONST_ARRAY1_INT {                              \
    2147483647, 2147483647, 2147483647, 2147483647,     \
    2147483647, 2126008810, 2147483647, 2130303777,     \
    2147483647, 2147483647, 2143188679, 2136746228,     \
    2147483647, 2126008810, 2147483647, 2147483647      \
}

#define CONST_ARRAY2_INT {                              \
    4096000000, 2048000000, 1024000000, 512000000,      \
    255744255, 127110228, 64000000, 32258064,           \
    16016016, 8000000, 4000000, 2000000,                \
    1000000, 500000, 250000, 125000                     \
    }

#define BME680_RES_HEAT_RANGE_MASK      (0x30)
#define BME680_GAS_RANGE_MASK           (0x7)

/*
* @brief    SPI communication
*/
#define BME680_READ_SPI             (0b10000000)
#define BME680_SPI_PAGE_0_MASK      (0b10000000)

/*
* @brief    commands and operation modes
*/
#define BME680_FORCED_MODE          (0x01)
#define BME680_SLEEP_MODE           (0x00)
#define BME680_RESET                (0xB6)
#define BME680_RUN_GAS              (0x10)

#ifdef __cplusplus
}
#endif

#endif /* BME680_INTERNALS_H */
/** @} */
