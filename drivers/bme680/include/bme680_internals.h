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

/*
 * @brief   registers
 * **/

#define BME680_REGISTER_CTRL_HUM    (0x72)
#define BME680_REGISTER_CTRL_MEAS   (0x74)
#define BME680_REGISTER_ID          (0x50)
#define BME680_REGISTER_CTRL_GAS    (0x70)
#define BME680_REGISTER_GAS_WAIT_0  (0x6D)
#define BME680_REGISTER_RES_HEAT_0  (0x63)

#define BME680_REGISTER_CTRL_GAS_L  (0x71)

#define BME680_REGISTER_MEAS_STATUS_0   (0x1D)

#define BME680_PRESS_MSB            (0x1F) // Contains the MSB part [19:12] of the raw pressure measurement output data
#define BME680_PRESS_LSB            (0x20) // Contains the LSB part [11:4] of the raw pressure measurement output data
#define BME680_PRESS_XLSB           (0x21) // Contains the XLSB part [3:0] of the raw pressure measurement output data. Contents depend on pressure resolution controlled by oversampling setting.

#define BME680_REGISTER_TEMP        (0x22)

#define BME680_REGISTER_GAS_R   (0x26)

#define BME680_REGISTER_MEAS_STATUS_0   (0x1D)

#define BME680_REGISTER_RESET       (0xE0)  // different in SPI!

#define BME680_REGISTER_CHIP_ID     (0xD0)

#define BME680_REGISTER_PAR_T1      (0xE9)
#define BME680_REGISTER_PAR_T2      (0x8A)
#define BME680_REGISTER_PAR_T3      (0x8C)
#define BME680_REGISTER_TEMP_ADC    (0x22)

#define BME680_REGISTER_PAR_H1      (0xE2)
#define BME680_REGISTER_PAR_H2      (0xE2)
#define BME680_REGISTER_PAR_H3      (0xE4)
#define BME680_REGISTER_PAR_H4      (0xE5)
#define BME680_REGISTER_PAR_H5      (0xE6)
#define BME680_REGISTER_PAR_H6      (0xE7)
#define BME680_REGISTER_PAR_H7      (0xE8)
#define BME680_REGISTER_HUM_ADC     (0x25)

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
#define BME680_REGISTER_PRESS_ADC     (0x1F)

#define BME680_REGISTER_PAR_G1      (0xED)
#define BME680_REGISTER_PAR_G2      (0xEB)
#define BME680_REGISTER_PAR_G3      (0xEE)
#define BME680_REGISTER_RES_HEAT_RANGE    (0x02)
#define BME680_REGISTER_RES_HEAT_VAL    (0x00)

/*
 * @brief   register bits
*/

/*
* @brief    operation modes
*/
#define BME680_FORCED_MODE          (0x00)
#define BME680_SLEEP_MODE           (0x01)
#define BME680_RESET                (0xB6)

#ifdef __cplusplus
}
#endif

#endif /* BME680_INTERNALS_H */
/** @} */
