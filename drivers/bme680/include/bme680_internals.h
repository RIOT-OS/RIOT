/**
 * Copyright (C) 2018 Mesotic SAS
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of the
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 * The information provided is believed to be accurate and reliable.
 * The copyright holder assumes no responsibility
 * for the consequences of use
 * of such information nor for any infringement of patents or
 * other rights of third parties which may result from its use.
 * No license is granted by implication or otherwise under any patent or
 * patent rights of the copyright holder.
 *
 */

#ifndef BME680_INTERNALS_H
#define BME680_INTERNALS_H

/* header includes */
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/** @name        Common macros                          */
/******************************************************************************/

/** BME680 General config */
#define BME680_POLL_PERIOD_MS        10

/** BME680 I2C addresses */
#define BME680_I2C_ADDR_PRIMARY        0x76
#define BME680_I2C_ADDR_SECONDARY    0x77

/** BME680 unique chip identifier */
#define BME680_CHIP_ID              0x61

/** BME680 coefficients related defines */
#define BME680_COEFF_SIZE            41
#define BME680_COEFF_ADDR1_LEN        25
#define BME680_COEFF_ADDR2_LEN        16

/** BME680 field_x related defines */
#define BME680_FIELD_LENGTH            15
#define BME680_FIELD_ADDR_OFFSET    17

/** Soft reset command */
#define BME680_SOFT_RESET_CMD       0xb6

/** Register map */
/** Other coefficient's address */
#define BME680_ADDR_RES_HEAT_VAL_ADDR    0x00
#define BME680_ADDR_RES_HEAT_RANGE_ADDR    0x02
#define BME680_ADDR_RANGE_SW_ERR_ADDR    0x04
#define BME680_ADDR_SENS_CONF_START        0x5A
#define BME680_ADDR_GAS_CONF_START        0x64

/** Field settings */
#define BME680_FIELD0_ADDR                0x1d

/** Heater settings */
#define BME680_RES_HEAT0_ADDR            0x5a
#define BME680_GAS_WAIT0_ADDR            0x64

/** Sensor configuration registers */
#define BME680_CONF_HEAT_CTRL_ADDR        0x70
#define BME680_CONF_ODR_RUN_GAS_NBC_ADDR    0x71
#define BME680_CONF_OS_H_ADDR            0x72
#define BME680_MEM_PAGE_ADDR            0xf3
#define BME680_CONF_T_P_MODE_ADDR        0x74
#define BME680_CONF_ODR_FILT_ADDR        0x75

/** Coefficient's address */
#define BME680_COEFF_ADDR1    0x89
#define BME680_COEFF_ADDR2    0xe1

/** Chip identifier */
#define BME680_CHIP_ID_ADDR    0xd0

/** Soft reset register */
#define BME680_SOFT_RESET_ADDR        0xe0

/** Heater control settings */
#define BME680_ENABLE_HEATER        0x00
#define BME680_DISABLE_HEATER        0x08

/** Gas measurement settings */
#define BME680_DISABLE_GAS_MEAS        0x00
#define BME680_ENABLE_GAS_MEAS        0x01

/** Over-sampling settings */
#define BME680_OS_NONE        0
#define BME680_OS_1X        1
#define BME680_OS_2X        2
#define BME680_OS_4X        3
#define BME680_OS_8X        4
#define BME680_OS_16X        5

/** IIR filter settings */
#define BME680_FILTER_SIZE_0    0
#define BME680_FILTER_SIZE_1    1
#define BME680_FILTER_SIZE_3    2
#define BME680_FILTER_SIZE_7    3
#define BME680_FILTER_SIZE_15    4
#define BME680_FILTER_SIZE_31    5
#define BME680_FILTER_SIZE_63    6
#define BME680_FILTER_SIZE_127    7

/** Power mode settings */
#define BME680_SLEEP_MODE    0
#define BME680_FORCED_MODE    1

/** Delay related macro declaration */
#define BME680_RESET_PERIOD    10


/** Ambient humidity shift value for compensation */
#define BME680_HUM_REG_SHIFT_VAL    4

/** Run gas enable and disable settings */
#define BME680_RUN_GAS_DISABLE    0
#define BME680_RUN_GAS_ENABLE    1

/** Buffer length macro declaration */
#define BME680_TMP_BUFFER_LENGTH    40
#define BME680_REG_BUFFER_LENGTH    6
#define BME680_FIELD_DATA_LENGTH    3
#define BME680_GAS_REG_BUF_LENGTH    20

/** Settings selector */
#define BME680_OST_SEL            1
#define BME680_OSP_SEL            2
#define BME680_OSH_SEL            4
#define BME680_GAS_MEAS_SEL        8
#define BME680_FILTER_SEL        16
#define BME680_HCNTRL_SEL        32
#define BME680_RUN_GAS_SEL        64
#define BME680_NBCONV_SEL        128
#define BME680_GAS_SENSOR_SEL        (BME680_GAS_MEAS_SEL | BME680_RUN_GAS_SEL | BME680_NBCONV_SEL)

/** Number of conversion settings*/
#define BME680_NBCONV_MIN        0
#define BME680_NBCONV_MAX        10

/** Mask definitions */
#define BME680_GAS_MEAS_MSK    0x30
#define BME680_NBCONV_MSK    0X0F
#define BME680_FILTER_MSK    0X1C
#define BME680_OST_MSK        0XE0
#define BME680_OSP_MSK        0X1C
#define BME680_OSH_MSK        0X07
#define BME680_HCTRL_MSK    0x08
#define BME680_RUN_GAS_MSK    0x10
#define BME680_MODE_MSK        0x03
#define BME680_RHRANGE_MSK    0x30
#define BME680_RSERROR_MSK    0xf0
#define BME680_NEW_DATA_MSK    0x80
#define BME680_GAS_INDEX_MSK    0x0f
#define BME680_GAS_RANGE_MSK    0x0f
#define BME680_GASM_VALID_MSK    0x20
#define BME680_HEAT_STAB_MSK    0x10
#define BME680_MEM_PAGE_MSK    0x10
#define BME680_SPI_RD_MSK    0x80
#define BME680_SPI_WR_MSK    0x7f
#define    BME680_BIT_H1_DATA_MSK    0x0F

/** Bit position definitions for sensor settings */
#define BME680_GAS_MEAS_POS    4
#define BME680_FILTER_POS    2
#define BME680_OST_POS        5
#define BME680_OSP_POS        2
#define BME680_RUN_GAS_POS    4

/** Array Index to Field data mapping for Calibration Data*/
#define BME680_T2_LSB_REG    (1)
#define BME680_T2_MSB_REG    (2)
#define BME680_T3_REG        (3)
#define BME680_P1_LSB_REG    (5)
#define BME680_P1_MSB_REG    (6)
#define BME680_P2_LSB_REG    (7)
#define BME680_P2_MSB_REG    (8)
#define BME680_P3_REG        (9)
#define BME680_P4_LSB_REG    (11)
#define BME680_P4_MSB_REG    (12)
#define BME680_P5_LSB_REG    (13)
#define BME680_P5_MSB_REG    (14)
#define BME680_P7_REG        (15)
#define BME680_P6_REG        (16)
#define BME680_P8_LSB_REG    (19)
#define BME680_P8_MSB_REG    (20)
#define BME680_P9_LSB_REG    (21)
#define BME680_P9_MSB_REG    (22)
#define BME680_P10_REG        (23)
#define BME680_H2_MSB_REG    (25)
#define BME680_H2_LSB_REG    (26)
#define BME680_H1_LSB_REG    (26)
#define BME680_H1_MSB_REG    (27)
#define BME680_H3_REG        (28)
#define BME680_H4_REG        (29)
#define BME680_H5_REG        (30)
#define BME680_H6_REG        (31)
#define BME680_H7_REG        (32)
#define BME680_T1_LSB_REG    (33)
#define BME680_T1_MSB_REG    (34)
#define BME680_GH2_LSB_REG    (35)
#define BME680_GH2_MSB_REG    (36)
#define BME680_GH1_REG        (37)
#define BME680_GH3_REG        (38)

/** BME680 register buffer index settings*/
#define BME680_REG_FILTER_INDEX        (5)
#define BME680_REG_TEMP_INDEX        (4)
#define BME680_REG_PRES_INDEX        (4)
#define BME680_REG_HUM_INDEX        (2)
#define BME680_REG_NBCONV_INDEX        (1)
#define BME680_REG_RUN_GAS_INDEX    (1)
#define BME680_REG_HCTRL_INDEX        (0)

/** BME680 pressure calculation macros */
/** This max value is used to provide precedence to multiplication or division
 * in pressure compensation equation to achieve least loss of precision and
 * avoiding overflows.
 * i.e Comparing value, BME680_MAX_OVERFLOW_VAL = INT32_C(1 << 30)
 */
#define BME680_MAX_OVERFLOW_VAL     (0x40000000)

/** Macro to combine two 8 bit data's to form a 16 bit data */
#define CONCAT_BYTES(msb, lsb)    (((uint16_t)msb << 8) | (uint16_t)lsb)

/** Type definitions */
/**
 * Generic communication function pointer
 * @param[in] dev_id: Place holder to store the id of the device structure
 *                    Can be used to store the index of the Chip select or
 *                    I2C address of the device.
 * @param[in] reg_addr:    Used to select the register the where data needs to
 *                      be read from or written to.
 * @param[in/out] reg_data: Data array to read/write
 * @param[in] len: Length of the data array
 */
typedef int8_t (*bme680_com_fptr_t)(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);

/**
 * Delay function pointer
 * @param[in] period: Time period in milliseconds
 */
typedef void (*bme680_delay_fptr_t)(uint32_t period);

/* structure definitions */
/**
 * @brief Sensor field data structure
 */
typedef struct {
    uint8_t status;                /**< Contains new_data, gasm_valid & heat_stab */
    uint8_t gas_index;            /**< The index of the heater profile used */
    uint8_t meas_index;         /**< Measurement index to track order */
    int16_t temperature;        /**< Temperature in degree celsius */
    uint32_t pressure;            /**< Pressure in Pascal */
    uint32_t humidity;            /**< Humidity in % relative humidity x1000 */
    uint32_t gas_resistance;     /**< Gas resistance in Ohms */
} bme680_data_t;

/**
 * @brief BME680 sensor settings structure which comprises of ODR,
 * over-sampling and filter settings.
 */
typedef struct {
    uint8_t os_hum;        /**< Humidity oversampling */
    uint8_t os_temp;    /**< Temperature oversampling */
    uint8_t os_pres;    /**< Pressure oversampling */
    uint8_t filter;        /**< Filter coefficient */
} bme680_tph_sett_t;

/**
 * @brief BME680 gas sensor which comprises of gas settings
 *  and status parameters
 */
typedef struct {
    uint8_t nb_conv;        /**< Variable to store nb conversion */
    uint8_t heatr_ctrl;        /**< Variable to store heater control */
    uint8_t run_gas;        /**< Run gas enable value */
    uint16_t heatr_temp;    /**< Heater temperature value */
    uint16_t heatr_dur;     /**< Duration profile value */
} bme680_gas_sett_t;

#ifdef __cplusplus
}
#endif

#endif /* BME680_INTERNALS_H */
