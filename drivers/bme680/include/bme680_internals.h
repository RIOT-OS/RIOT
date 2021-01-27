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
#define BME680_I2C_ADDR_1                   (0x76)

/**
 * @brief    I2C address when SDO pin is HIGH
 */
#define BME680_I2C_ADDR_2                   (0x77)

/**
 * @brief    SPI settings
 */
#define BME680_SPI_SPEED                    (SPI_CLK_1MHZ)
#define BME680_SPI_MODE                     (SPI_MODE_0)
#define BME680_SPI_PAGE_0                   (0x0)
#define BME680_SPI_PAGE_1                   (0x10)
#define BME680_SPI_PAGE_0_MASK              (0x80)

/**
 * @brief    default chip id
 */
#define BME680_CHIP_ID                      (0x61)

/**
 * @brief   registers
 */
#define BME680_REGISTER_CALIB_3             (0x00)
#define BME680_REGISTER_ADC                 (0x1F)
#define BME680_REGISTER_ADC_GAS             (0x2A)
#define BME680_REGISTER_MEAS_STATUS_0       (0x1D)
#define BME680_REGISTER_RES_HEAT_0          (0x5A)
#define BME680_REGISTER_GAS_WAIT_0          (0x64)
#define BME680_REGISTER_CTRL_GAS            (0x70)
#define BME680_REGISTER_CTRL_GAS_1          (0x71)
#define BME680_REGISTER_CTRL_HUM            (0x72)
#define BME680_REGISTER_SPI_MEM_PAGE        (0x73)
#define BME680_REGISTER_CTRL_MEAS           (0x74)
#define BME680_REGISTER_CONFIG              (0x75)
#define BME680_REGISTER_CHIP_ID             (0xD0)
#define BME680_REGISTER_CALIB_2             (0x8A)
#define BME680_REGISTER_RESET               (0xE0)
#define BME680_REGISTER_CALIB_1             (0xE1)

/**
 * @brief   structs for reading out register values
 */
typedef struct __attribute__((packed)) {
    uint8_t h2_msb;
    uint8_t h1_h2_lsb;
    uint8_t h1_msb;
    int8_t par_h3;
    int8_t par_h4;
    int8_t par_h5;
    uint8_t par_h6;
    int8_t par_h7;
    uint16_t par_t1;
    int16_t par_g2;
    int8_t par_g1;
    int8_t par_g3;
} bme680_calib_chunk1_t;

typedef struct __attribute__((packed)) {
    int16_t par_t2;
    int8_t par_t3;
    uint8_t padding1;
    uint16_t par_p1;
    int16_t par_p2;
    int8_t par_p3;
    int8_t padding2;
    int16_t par_p4;
    int16_t par_p5;
    int8_t par_p7;
    int8_t par_p6;
    uint8_t padding3[2];
    int16_t par_p8;
    int16_t par_p9;
    uint8_t par_p10;
} bme680_calib_chunk2_t;

typedef struct __attribute__((packed)) {
    uint8_t res_heat_val;
    uint8_t padding1;
    uint8_t res_heat_range;
    uint8_t padding2;
    uint8_t range_sw_error;
} bme680_calib_chunk3_t;

typedef struct __attribute__((packed)) {
    uint8_t press_adc_msb;
    uint8_t press_adc_lsb;
    uint8_t press_adc_xlsb;
    uint8_t temp_adc_msb;
    uint8_t temp_adc_lsb;
    uint8_t temp_adc_xlsb;
    uint8_t hum_adc_msb;
    uint8_t hum_adc_lsb;
} bme680_adc_readout_t;

typedef struct __attribute__((packed)) {
    uint8_t gas_adc_msb;
    uint8_t gas_adc_lsb;
} bme680_adc_readout_gas_t;

/*
* @brief    data status
*/
#define BME680_NEW_DATA                     (0x80)
#define BME680_GAS_MEASUREMENT_SUCCESS      (0x30)

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

#define BME680_RES_HEAT_RANGE_MASK          (0x30)
#define BME680_GAS_RANGE_MASK               (0x0F)
#define BME680_HUM_SETTINGS_MASK            (0xF8)
#define BME680_FILTER_SETTINGS_MASK         (0xE3)
#define BME680_GAS_SETTINGS_MASK            (0xE0)
#define BME680_H1_H2_MASK                   (0x0F)
#define BME680_SPI_MEM_PAGE_MASK            (0x10)

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
