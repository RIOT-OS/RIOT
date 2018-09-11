/**
 * Copyright (C) 2017 - 2018 Bosch Sensortec GmbH
 * Copyright (C) 2018 Mesotic SAS
 * Based on Bosch Sensortech driver from
 * https://github.com/BoschSensortec/BME680_driver
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
 *
 */

/* @brief Sensor driver for BME680 sensor */
#include "bme680.h"
#define ENABLE_DEBUG (0)
#include "debug.h"

#include "xtimer.h"
/**
 * @brief This internal API is used to read the calibrated data from the sensor.
 *
 * This function is used to retrieve the calibration
 * data from the image registers of the sensor.
 *
 * @note Registers 89h  to A1h for calibration data 1 to 24
 *        from bit 0 to 7
 * @note Registers E1h to F0h for calibration data 25 to 40
 *        from bit 0 to 7
 * @param[in] dev    Structure instance of bme680_dev.
 *
 * @return Result of API execution status.
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */

static int8_t get_calib_data(bme680_t *dev);

/**
 * @brief This internal API is used to calculate the Heat duration value.
 *
 * @param[in] dur    Value of the duration to be shared.
 *
 * @return uint8_t threshold duration after calculation.
 */
static uint8_t calc_heater_dur(uint16_t dur);

/**
 * @brief This internal API is used to calculate the
 * temperature value value in float format
 *
 * @param[in] dev    Structure instance of bme680_dev.
 * @param[in] temp_adc    Contains the temperature ADC value .
 *
 * @return Calculated temperature.
 */
static int16_t calc_temperature(bme680_t *dev, uint32_t temp_adc);

/**
 * @brief This internal API is used to calculate the
 * pressure value value in float format
 *
 * @param[in] dev    Structure instance of bme680_dev.
 * @param[in] pres_adc    Contains the pressure ADC value .
 *
 * @return Calculated pressure.
 */
static uint32_t calc_pressure(bme680_t *dev, uint32_t pres_adc);

/**
 * @brief This internal API is used to calculate the
 * humidity value value in float format
 *
 * @param[in] dev    Structure instance of bme680_dev.
 * @param[in] hum_adc    Contains the humidity ADC value.
 *
 * @return Calculated humidity.
 */
static uint32_t calc_humidity(bme680_t *dev, uint16_t hum_adc);

/**
 * @brief This internal API is used to calculate the
 * gas resistance value value in float format
 *
 * @param[in] dev        Structure instance of bme680_dev.
 * @param[in] gas_res_adc    Contains the Gas Resistance ADC value.
 * @param[in] gas_range        Contains the range of gas values.
 *
 * @return Calculated gas resistance.
 */
static uint32_t calc_gas_resistance(bme680_t *dev, uint16_t gas_res_adc,
                                 uint8_t gas_range);

/**
 * @brief This internal API is used to calculate the
 * heater resistance value in float format
 *
 * @param[in] temp    Contains the target temperature value.
 * @param[in] dev    Structure instance of bme680_dev.
 *
 * @return Calculated heater resistance.
 */
static uint8_t calc_heater_res(bme680_t *dev, uint16_t temp);

/**
 * @brief This internal API is used to calculate the field data of sensor.
 *
 * @param[out] data Structure instance to hold the data
 * @param[in] dev    Structure instance of bme680_dev.
 *
 *  @return int8_t result of the field data from sensor.
 */
static int8_t read_field_data(bme680_t *dev, bme680_data_t *data);

/****************** Global Function Definitions *******************************/
/**
 *@brief This API is the entry point.
 *It reads the chip-id and calibration data from the sensor.
 */
int8_t bme680_init(bme680_t *dev, const bme680_params_t *params)
{
    int8_t ret;
    uint8_t id;

    /* Retrieve stored parameters */
    dev->params = (bme680_params_t*)params;
    /* Soft reset device before init */
    ret = bme680_soft_reset(dev);
    if (ret != BME680_OK) {
        DEBUG("[bme680]: Cannot reset device\n");
        return -1;
    }
    /* Check device ID */
    ret = bme680_get_regs(dev, BME680_CHIP_ID_ADDR, &id, 1);
    if (ret != BME680_OK) {
        DEBUG("[bme680]: Cannot read device ID\n");
        return -BME680_COM_FAIL;
    }
    if (id != BME680_CHIP_ID) {
        DEBUG("[bme680]: Wrong ID %i instead of %i", id, BME680_CHIP_ID);
        return -BME680_BAD_ID;
    }

    /* Get the calibration data */
    ret = get_calib_data(dev);
    if (ret != BME680_OK) {
        DEBUG("[bme680]: Cannot get calibration data\n");
        return -BME680_COM_FAIL;
    }

    return ret;
}

/**
 * @brief This API reads the data from the given register address of the sensor.
 */
int8_t bme680_get_regs(bme680_t *dev, uint8_t reg_addr, uint8_t *reg_data,
                       uint16_t len )
{
    uint8_t ret;
    ret = i2c_read_regs(dev->params->i2c_dev, dev->params->i2c_addr, reg_addr,
                        (void*) reg_data, (uint16_t) len, 0);
    if(ret != 0) {
        DEBUG("[bme680]: Comm error\n");
        return -BME680_COM_FAIL;
    }
    else {
        return BME680_OK;
    }
}

/**
 * @brief This API writes the given data to the register address
 * of the sensor.
 */
int8_t bme680_set_regs(bme680_t *dev, const uint8_t reg_addr,
                       const uint8_t *reg_data, uint8_t len)
{
    uint8_t ret;

    ret = i2c_write_regs(dev->params->i2c_dev, dev->params->i2c_addr, reg_addr,
                          reg_data, (2 * len) , 0);
    if(ret != 0) {
        DEBUG("[bme680]: Comm error\n");
        return -BME680_COM_FAIL;
    }
    else {
        return BME680_OK;
    }
}

/**
 * @brief This API performs the soft reset of the sensor.
 */
int8_t bme680_soft_reset(bme680_t *dev)
{
    int8_t ret;
    uint8_t reg = BME680_SOFT_RESET_CMD;
    /* Reset the device */
    ret = bme680_set_regs(dev, BME680_SOFT_RESET_ADDR, &reg, 1);
    if (ret != 0) {
        DEBUG("[bme680]: Soft reset failed\n");
        return ret;
    }
    /* Wait for 5ms */
    xtimer_usleep(5 * 1000);
    return ret;
}

/**
 * @brief This API is used to set the oversampling, filter and T,P,H, gas
 * selection settings in the sensor.
 */
int8_t bme680_set_sensor_settings(bme680_t *dev, uint16_t flags)
{
    int8_t ret;
    uint8_t data = 0;

    if (flags & BME680_GAS_MEAS_SEL) {
        /* Set heater resistance */
        data = calc_heater_res(dev, dev->gas_sett.heatr_temp);
        ret = bme680_set_regs(dev, BME680_RES_HEAT0_ADDR, &data, 1);
        /* Set heating duration */
        data = calc_heater_dur(dev->gas_sett.heatr_dur);
        ret += bme680_set_regs(dev, BME680_GAS_WAIT0_ADDR, &data, 1);
        if (ret != BME680_OK) {
            DEBUG("[bme680]: Cannot set gas settings\n");
            return -BME680_COM_FAIL;
        }
        dev->gas_sett.nb_conv = 0;
    }

    ret = bme680_set_sensor_mode(dev, BME680_SLEEP_MODE);
    if (ret != BME680_OK) {
        DEBUG("[bme680]: Cannot change sensor mode\n");
        return -BME680_COM_FAIL;
    }
    /* Selecting the filter */
    if (flags & BME680_FILTER_SEL)
    {
        ret = bme680_get_regs(dev, BME680_CONF_ODR_FILT_ADDR, &data, 1);
        data |= dev->tph_sett.filter << BME680_FILTER_POS;
        ret += bme680_set_regs(dev, BME680_CONF_ODR_FILT_ADDR, &data, 1);
        if (ret != BME680_OK) {
            DEBUG("[bme680]: Cannot set filter\n");
            return -BME680_COM_FAIL;
        }
    }

    /* Selecting heater control for the sensor */
    if (flags & BME680_HCNTRL_SEL)
    {
        ret = bme680_get_regs(dev, BME680_CONF_HEAT_CTRL_ADDR, &data, 1);
        data |= dev->gas_sett.heatr_ctrl;
        ret += bme680_set_regs(dev, BME680_CONF_HEAT_CTRL_ADDR, &data, 1);
        if (ret != BME680_OK) {
            DEBUG("[bme680]: Cannot set heater control\n");
            return -BME680_COM_FAIL;
        }
    }

    /* Selecting heater T,P oversampling for the sensor */
    if (flags & (BME680_OST_SEL | BME680_OSP_SEL))
    {

        ret = bme680_get_regs(dev, BME680_CONF_T_P_MODE_ADDR, &data, 1);

        if (flags & BME680_OST_SEL)
        {
            data |= dev->tph_sett.os_temp << BME680_OST_POS;
        }
        if (flags & BME680_OSP_SEL)
        {
            data |= dev->tph_sett.os_pres << BME680_OSP_POS;

        }
        ret += bme680_set_regs(dev, BME680_CONF_T_P_MODE_ADDR, &data, 1);
        if (ret != BME680_OK) {
            DEBUG("[bme680]: Cannot set TP oversampling\n");
            return -BME680_COM_FAIL;
        }
    }

    /* Selecting humidity oversampling for the sensor */
    if (flags & BME680_OSH_SEL)
    {
        ret = bme680_get_regs(dev, BME680_CONF_OS_H_ADDR, &data, 1);
        /* No need to shift os_hum as its position is 0 */
        data |= dev->tph_sett.os_hum;
        ret += bme680_set_regs(dev, BME680_CONF_OS_H_ADDR, &data, 1);
        if (ret != BME680_OK) {
            DEBUG("[bme680]: Cannot set H oversampling\n");
            return -BME680_COM_FAIL;
        }

    }

    /* Selecting the runGas and NB conversion settings for the sensor */
    if (flags & (BME680_RUN_GAS_SEL | BME680_NBCONV_SEL))
    {
        ret = bme680_get_regs(dev, BME680_CONF_ODR_RUN_GAS_NBC_ADDR, &data, 1);
        if (flags & BME680_RUN_GAS_SEL) {
            data |= dev->gas_sett.run_gas << BME680_RUN_GAS_POS;
        }
        if (flags & BME680_NBCONV_SEL) {
            /* No need to shift as its position is 0 */
            data |= dev->gas_sett.nb_conv;
        }
        ret += bme680_set_regs(dev, BME680_CONF_ODR_RUN_GAS_NBC_ADDR, &data, 1);
        if (ret != BME680_OK) {
            DEBUG("[bme680]: Cannot set runGas and NB conv\n");
            return -BME680_COM_FAIL;
        }
    }

    return ret;
}

/**
 * @brief This API is used to get the oversampling, filter and T,P,H, gas
 * selection settings in the sensor.
 */
int8_t bme680_get_sensor_settings(bme680_t *dev)
{
    int8_t ret;
    /* starting address of the register array for burst read */
    uint8_t data_array[BME680_REG_BUFFER_LENGTH] = { 0 };

    ret = bme680_get_regs(dev, BME680_ADDR_SENS_CONF_START, &data_array[0], 1);
    if (ret != BME680_OK) {
        return ret;
    }
    dev->gas_sett.heatr_temp = data_array[0];
    ret = bme680_get_regs(dev, BME680_ADDR_GAS_CONF_START, &data_array[0], 1);
    if (ret != BME680_OK) {
        return ret;
    }
    dev->gas_sett.heatr_dur = data_array[0];

    ret = bme680_get_regs(dev, BME680_CONF_HEAT_CTRL_ADDR, data_array, BME680_REG_BUFFER_LENGTH);
    if (ret != BME680_OK) {
        return ret;
    }

    /* get the T,P,H ,Filter,ODR settings here */
    dev->tph_sett.filter = (data_array[BME680_REG_FILTER_INDEX] &
                           BME680_FILTER_MSK) >> BME680_FILTER_POS;


    dev->tph_sett.os_temp = (data_array[BME680_REG_TEMP_INDEX] & BME680_OST_MSK) >> BME680_OST_POS;
    dev->tph_sett.os_pres = (data_array[BME680_REG_PRES_INDEX] & BME680_OSP_MSK) >> BME680_OSP_POS;
    dev->tph_sett.os_hum = (data_array[BME680_REG_HUM_INDEX] & BME680_OSH_MSK);

    /* get the gas related settings */
    dev->gas_sett.heatr_ctrl = data_array[BME680_REG_HCTRL_INDEX] & BME680_HCTRL_MSK;
    dev->gas_sett.nb_conv = data_array[BME680_REG_NBCONV_INDEX] & BME680_NBCONV_MSK;
    dev->gas_sett.run_gas = (data_array[BME680_REG_RUN_GAS_INDEX] & BME680_RUN_GAS_MSK) >> BME680_RUN_GAS_POS;

    return ret;
}

/**
 * @brief This API is used to set the power mode of the sensor.
 */
int8_t bme680_set_sensor_mode(bme680_t *dev, uint8_t mode)
{
    int8_t ret;
    uint8_t curr_mode;

    ret = bme680_get_regs(dev, BME680_CONF_T_P_MODE_ADDR, &curr_mode, 1);
    if (ret != BME680_OK) {
        return ret;
    }
    if ((curr_mode & BME680_MODE_MSK) != mode) {
        /* set new mode */
        uint8_t tmp_mode = curr_mode;
        if (mode == BME680_SLEEP_MODE) {
            /* Only clear mode bits */
            tmp_mode &= ~BME680_MODE_MSK;
            ret = bme680_set_regs(dev, BME680_CONF_T_P_MODE_ADDR, &mode, 1);
            if (ret != BME680_OK) {
                return ret;
            }
            /* Loop until device goes to sleep */
            while(curr_mode != BME680_SLEEP_MODE) {
                ret = bme680_get_regs(dev, BME680_CONF_T_P_MODE_ADDR,
                                      &curr_mode, 1);
                if (ret != BME680_OK) {
                    return ret;
                }
                curr_mode &= BME680_MODE_MSK;
                xtimer_usleep(BME680_POLL_PERIOD_MS * US_PER_MS);
            }
            DEBUG("[bme680]: sleep mode\n");
        }
        else {
            tmp_mode |= BME680_FORCED_MODE;
            ret = bme680_set_regs(dev, BME680_CONF_T_P_MODE_ADDR, &tmp_mode, 1);
            if (ret != BME680_OK) {
                return ret;
            }
            DEBUG("[bme680]: forced mode\n");
        }
    }
    return ret;
}

/**
 * @brief This API is used to get the power mode of the sensor.
 */
int8_t bme680_get_sensor_mode(bme680_t *dev)
{
    int8_t ret;
    uint8_t mode;

    ret = bme680_get_regs(dev, BME680_CONF_T_P_MODE_ADDR, &mode, 1);
    if (ret == BME680_OK) {
        /* Masking the other register bit info */
        dev->power_mode = mode & BME680_MODE_MSK;
    }
    return ret;
}

/**
 * @brief This API is used to set the profile duration of the sensor.
 */
void bme680_set_profile_dur(bme680_t *dev, uint16_t duration)
{
    uint32_t tph_dur; /* Calculate in us */
    uint32_t meas_cycles;
    uint8_t os_to_meas_cycles[6] = {0, 1, 2, 4, 8, 16};

    meas_cycles = os_to_meas_cycles[dev->tph_sett.os_temp];
    meas_cycles += os_to_meas_cycles[dev->tph_sett.os_pres];
    meas_cycles += os_to_meas_cycles[dev->tph_sett.os_hum];

    /* TPH measurement duration */
    tph_dur = meas_cycles * (1963);
    tph_dur += (477 * 4);         /* TPH switching duration */
    tph_dur += (477 * 5);         /* Gas measurement duration */
    tph_dur += (500);             /* Get it to the closest whole number.*/
    tph_dur /= (1000);             /* Convert to ms */

    tph_dur += 1; /* Wake up duration of 1ms */
    /* The remaining time should be used for heating */
    dev->gas_sett.heatr_dur = duration - (uint16_t) tph_dur;
}

/**
 * @brief This API is used to get the profile duration of the sensor.
 */
void bme680_get_profile_dur(bme680_t *dev, uint32_t *duration)
{
    uint32_t tph_dur; /* Calculate in us */
    uint32_t meas_cycles;
    uint8_t os_to_meas_cycles[6] = {0, 1, 2, 4, 8, 16};

    meas_cycles = os_to_meas_cycles[dev->tph_sett.os_temp];
    meas_cycles += os_to_meas_cycles[dev->tph_sett.os_pres];
    meas_cycles += os_to_meas_cycles[dev->tph_sett.os_hum];

    /* TPH measurement duration */
    tph_dur = meas_cycles * 1963;
    tph_dur += 477 * 4;     /* TPH switching duration */
    tph_dur += 477 * 5;     /* Gas measurement duration */
    tph_dur += 500;         /* Get it to the closest whole number.*/
    tph_dur += 1000;         /* Wake up duration of 1ms */

    *duration = tph_dur;

    /* Get the gas duration only when the run gas is enabled */
    if (dev->gas_sett.run_gas) {
        /* The remaining time should be used for heating */
        *duration += dev->gas_sett.heatr_dur;
    }
}

/**
 * @brief This API reads the pressure, temperature and humidity and gas data
 * from the sensor, compensates the data and store it in the bme680_data
 * structure instance passed by the user.
 */
int8_t bme680_get_sensor_data(bme680_t *dev, bme680_data_t *data)
{
    return read_field_data(dev, data);
}

/**
 * @brief This internal API is used to read the calibrated data from the sensor.
 */
static int8_t get_calib_data(bme680_t *dev)
{
    int8_t ret;
    uint8_t coeff_array[BME680_COEFF_SIZE] = { 0 };
    uint8_t temp_var = 0; /* Temporary variable */

    ret = bme680_get_regs(dev, BME680_COEFF_ADDR1, coeff_array,
                          BME680_COEFF_ADDR1_LEN);
    /* Append the second half in the same array */
    ret += bme680_get_regs(dev, BME680_COEFF_ADDR2,
                           &coeff_array[BME680_COEFF_ADDR1_LEN],
                           BME680_COEFF_ADDR2_LEN);
    if (ret != BME680_OK) {
        return ret;
    }
    /* Temperature related coefficients */
    dev->calib.par_t1 = (uint16_t) (CONCAT_BYTES(coeff_array[BME680_T1_MSB_REG],
        coeff_array[BME680_T1_LSB_REG]));
    dev->calib.par_t2 = (int16_t) (CONCAT_BYTES(coeff_array[BME680_T2_MSB_REG],
        coeff_array[BME680_T2_LSB_REG]));
    dev->calib.par_t3 = (int8_t) (coeff_array[BME680_T3_REG]);

    /* Pressure related coefficients */
    dev->calib.par_p1 = (uint16_t) (CONCAT_BYTES(coeff_array[BME680_P1_MSB_REG],
        coeff_array[BME680_P1_LSB_REG]));
    dev->calib.par_p2 = (int16_t) (CONCAT_BYTES(coeff_array[BME680_P2_MSB_REG],
        coeff_array[BME680_P2_LSB_REG]));
    dev->calib.par_p3 = (int8_t) coeff_array[BME680_P3_REG];
    dev->calib.par_p4 = (int16_t) (CONCAT_BYTES(coeff_array[BME680_P4_MSB_REG],
        coeff_array[BME680_P4_LSB_REG]));
    dev->calib.par_p5 = (int16_t) (CONCAT_BYTES(coeff_array[BME680_P5_MSB_REG],
        coeff_array[BME680_P5_LSB_REG]));
    dev->calib.par_p6 = (int8_t) (coeff_array[BME680_P6_REG]);
    dev->calib.par_p7 = (int8_t) (coeff_array[BME680_P7_REG]);
    dev->calib.par_p8 = (int16_t) (CONCAT_BYTES(coeff_array[BME680_P8_MSB_REG],
        coeff_array[BME680_P8_LSB_REG]));
    dev->calib.par_p9 = (int16_t) (CONCAT_BYTES(coeff_array[BME680_P9_MSB_REG],
        coeff_array[BME680_P9_LSB_REG]));
    dev->calib.par_p10 = (uint8_t) (coeff_array[BME680_P10_REG]);

    /* Humidity related coefficients */
    dev->calib.par_h1 = (uint16_t) (((uint16_t) coeff_array[BME680_H1_MSB_REG] << BME680_HUM_REG_SHIFT_VAL)
                        | (coeff_array[BME680_H1_LSB_REG] & BME680_BIT_H1_DATA_MSK));
    dev->calib.par_h2 = (uint16_t) (((uint16_t) coeff_array[BME680_H2_MSB_REG] << BME680_HUM_REG_SHIFT_VAL)
                        | ((coeff_array[BME680_H2_LSB_REG]) >> BME680_HUM_REG_SHIFT_VAL));
    dev->calib.par_h3 = (int8_t) coeff_array[BME680_H3_REG];
    dev->calib.par_h4 = (int8_t) coeff_array[BME680_H4_REG];
    dev->calib.par_h5 = (int8_t) coeff_array[BME680_H5_REG];
    dev->calib.par_h6 = (uint8_t) coeff_array[BME680_H6_REG];
    dev->calib.par_h7 = (int8_t) coeff_array[BME680_H7_REG];

    /* Gas heater related coefficients */
    dev->calib.par_gh1 = (int8_t) coeff_array[BME680_GH1_REG];
    dev->calib.par_gh2 = (int16_t) (CONCAT_BYTES(coeff_array[BME680_GH2_MSB_REG],
        coeff_array[BME680_GH2_LSB_REG]));
    dev->calib.par_gh3 = (int8_t) coeff_array[BME680_GH3_REG];

    /* Other coefficients */
    ret = bme680_get_regs(dev, BME680_ADDR_RES_HEAT_RANGE_ADDR, &temp_var, 1);
    dev->calib.res_heat_range = ((temp_var & BME680_RHRANGE_MSK) / 16);
    if (ret == BME680_OK) {
        ret = bme680_get_regs(dev, BME680_ADDR_RES_HEAT_VAL_ADDR, &temp_var, 1);
        dev->calib.res_heat_val = (int8_t) temp_var;
        if (ret == BME680_OK) {
            ret = bme680_get_regs(dev, BME680_ADDR_RANGE_SW_ERR_ADDR, &temp_var, 1);
            dev->calib.range_sw_err = ((int8_t) temp_var & (int8_t) BME680_RSERROR_MSK) / 16;
        }
    }
    return ret;
}

/**
 * @brief This internal API is used to calculate the
 * temperature value in float format
 */
static int16_t calc_temperature(bme680_t *dev, uint32_t temp_adc)
{
    int64_t var1;
    int64_t var2;
    int64_t var3;
    int16_t calc_temp;

    var1 = ((int32_t) temp_adc >> 3) - ((int32_t) dev->calib.par_t1 << 1);
    var2 = (var1 * (int32_t) dev->calib.par_t2) >> 11;
    var3 = ((var1 >> 1) * (var1 >> 1)) >> 12;
    var3 = ((var3) * ((int32_t) dev->calib.par_t3 << 4)) >> 14;
    dev->calib.t_fine = (int32_t) (var2 + var3);
    calc_temp = (int16_t) (((dev->calib.t_fine * 5) + 128) >> 8);

    return calc_temp;
}

/**
 * @brief This internal API is used to calculate the
 * pressure value in float format
 */
static uint32_t calc_pressure(bme680_t *dev, uint32_t pres_adc)
{

    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t pres_comp;

    var1 = (((int32_t)dev->calib.t_fine) >> 1) - 64000;
    var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) *
        (int32_t)dev->calib.par_p6) >> 2;
    var2 = var2 + ((var1 * (int32_t)dev->calib.par_p5) << 1);
    var2 = (var2 >> 2) + ((int32_t)dev->calib.par_p4 << 16);
    var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) *
        ((int32_t)dev->calib.par_p3 << 5)) >> 3) +
        (((int32_t)dev->calib.par_p2 * var1) >> 1);
    var1 = var1 >> 18;
    var1 = ((32768 + var1) * (int32_t)dev->calib.par_p1) >> 15;
    pres_comp = 1048576 - pres_adc;
    pres_comp = (int32_t)((pres_comp - (var2 >> 12)) * ((uint32_t)3125));
    if (pres_comp >= BME680_MAX_OVERFLOW_VAL)
        pres_comp = ((pres_comp / var1) << 1);
    else
        pres_comp = ((pres_comp << 1) / var1);
    var1 = ((int32_t)dev->calib.par_p9 * (int32_t)(((pres_comp >> 3) *
        (pres_comp >> 3)) >> 13)) >> 12;
    var2 = ((int32_t)(pres_comp >> 2) *
        (int32_t)dev->calib.par_p8) >> 13;
    var3 = ((int32_t)(pres_comp >> 8) * (int32_t)(pres_comp >> 8) *
        (int32_t)(pres_comp >> 8) *
        (int32_t)dev->calib.par_p10) >> 17;

    pres_comp = (int32_t)(pres_comp) + ((var1 + var2 + var3 +
        ((int32_t)dev->calib.par_p7 << 7)) >> 4);

    return (uint32_t)pres_comp;
}

/**
 * @brief This internal API is used to calculate the
 * humidity value in float format
 */
static uint32_t  calc_humidity(bme680_t *dev, uint16_t hum_adc)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    int32_t var6;
    int32_t temp_scaled;
    int32_t calc_hum;

    temp_scaled = (((int32_t) dev->calib.t_fine * 5) + 128) >> 8;
    var1 = (int32_t) (hum_adc - ((int32_t) ((int32_t) dev->calib.par_h1 * 16)))
        - (((temp_scaled * (int32_t) dev->calib.par_h3) / ((int32_t) 100)) >> 1);
    var2 = ((int32_t) dev->calib.par_h2
        * (((temp_scaled * (int32_t) dev->calib.par_h4) / ((int32_t) 100))
            + (((temp_scaled * ((temp_scaled * (int32_t) dev->calib.par_h5) / ((int32_t) 100))) >> 6)
                / ((int32_t) 100)) + (int32_t) (1 << 14))) >> 10;
    var3 = var1 * var2;
    var4 = (int32_t) dev->calib.par_h6 << 7;
    var4 = ((var4) + ((temp_scaled * (int32_t) dev->calib.par_h7) / ((int32_t) 100))) >> 4;
    var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
    var6 = (var4 * var5) >> 1;
    calc_hum = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;

    if (calc_hum > 100000) /* Cap at 100%rH */
        calc_hum = 100000;
    else if (calc_hum < 0)
        calc_hum = 0;

return (uint32_t) calc_hum;
}

/**
 * @brief This internal API is used to calculate the
 * gas resistance value in float format
 */
static uint32_t calc_gas_resistance(bme680_t *dev, uint16_t gas_res_adc,
                                 uint8_t gas_range)
{
    int64_t var1;
    uint64_t var2;
    int64_t var3;
    uint32_t calc_gas_res;
    /* Look up table 1 for the possible gas range values */
    uint32_t lookupTable1[16] = { 2147483647, 2147483647, 2147483647,
                                  2147483647, 2147483647, 2126008810,
                                  2147483647, 2130303777, 2147483647,
                                  2147483647, 2143188679, 2136746228,
                                  2147483647, 2126008810, 2147483647,
                                  2147483647 };
    /* Look up table 2 for the possible gas range values */
    uint32_t lookupTable2[16] = { 4096000000, 2048000000, 1024000000, 512000000,
                                  255744255, 127110228, 64000000, 32258064,
                                  16016016, 8000000, 4000000, 2000000, 1000000,
                                  500000, 250000, 125000 };

    var1 = (int64_t) ((1340 + (5 * (int64_t) dev->calib.range_sw_err)) *
        ((int64_t) lookupTable1[gas_range])) >> 16;
    var2 = (((int64_t) ((int64_t) gas_res_adc << 15) -
           (int64_t) (16777216)) + var1);
    var3 = (((int64_t) lookupTable2[gas_range] * (int64_t) var1) >> 9);
    calc_gas_res = (uint32_t) ((var3 + ((int64_t) var2 >> 1)) / (int64_t) var2);

    return calc_gas_res;
}

/**
 * @brief This internal API is used to calculate the
 * heater resistance value in float format
 */
static uint8_t calc_heater_res(bme680_t *dev, uint16_t temp)
{
    uint8_t heatr_res;
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    int32_t heatr_res_x100;

    if (temp > 400) /* Cap temperature */
        temp = 400;

    var1 = (((int32_t) dev->amb_temp * dev->calib.par_gh3) / 1000) * 256;
    var2 = (dev->calib.par_gh1 + 784) * (((((dev->calib.par_gh2 + 154009) * temp * 5) / 100) + 3276800) / 10);
    var3 = var1 + (var2 / 2);
    var4 = (var3 / (dev->calib.res_heat_range + 4));
    var5 = (131 * dev->calib.res_heat_val) + 65536;
    heatr_res_x100 = (int32_t) (((var4 / var5) - 250) * 34);
    heatr_res = (uint8_t) ((heatr_res_x100 + 50) / 100);

    return heatr_res;
}

/**
 * @brief This internal API is used to calculate the Heat duration value.
 */
static uint8_t calc_heater_dur(uint16_t dur)
{
    uint8_t factor = 0;
    uint8_t durval;

    if (dur >= 0xfc0) {
        durval = 0xff; /* Max duration*/
    } else {
        while (dur > 0x3F) {
            dur = dur / 4;
            factor += 1;
        }
        durval = (uint8_t) (dur + (factor * 64));
    }

    return durval;
}

/**
 * @brief This internal API is used to calculate the field data of sensor.
 */
static int8_t read_field_data(bme680_t *dev, bme680_data_t *data)
{
    int8_t ret;
    uint8_t buff[BME680_FIELD_LENGTH] = { 0 };
    uint8_t gas_range;
    uint32_t adc_temp;
    uint32_t adc_pres;
    uint16_t adc_hum;
    uint16_t adc_gas_res;
    uint8_t tries = 10;

    do {
        ret = bme680_get_regs(dev, ((uint8_t) (BME680_FIELD0_ADDR)), buff,
                              (uint16_t) BME680_FIELD_LENGTH);

        if (ret != 0) {
            DEBUG("[bme680]: Cannot read data\n");
            return -BME680_COM_FAIL;
        }
        data->status = buff[0] & BME680_NEW_DATA_MSK;
        data->gas_index = buff[0] & BME680_GAS_INDEX_MSK;
        data->meas_index = buff[1];

        /* read the raw data from the sensor */
        adc_pres = (uint32_t) (((uint32_t) buff[2] * 4096) |
                   ((uint32_t) buff[3] * 16) | ((uint32_t) buff[4] / 16));
        adc_temp = (uint32_t) (((uint32_t) buff[5] * 4096) |
                   ((uint32_t) buff[6] * 16) | ((uint32_t) buff[7] / 16));
        adc_hum = (uint16_t) (((uint32_t) buff[8] * 256) | (uint32_t) buff[9]);
        adc_gas_res = (uint16_t) ((uint32_t) buff[13] * 4 |
                      (((uint32_t) buff[14]) / 64));
        gas_range = buff[14] & BME680_GAS_RANGE_MSK;

        data->status |= buff[14] & BME680_GASM_VALID_MSK;
        data->status |= buff[14] & BME680_HEAT_STAB_MSK;

        if (data->status & BME680_NEW_DATA_MSK) {
            data->temperature = calc_temperature(dev, adc_temp);
            data->pressure = calc_pressure(dev, adc_pres);
            data->humidity = calc_humidity(dev, adc_hum);
            data->gas_resistance = calc_gas_resistance(dev, adc_gas_res,
                                                       gas_range);
            break;
        }
        /* Delay to poll the data */
        xtimer_usleep(BME680_POLL_PERIOD_MS * US_PER_MS);
    } while (--tries);

    if (!tries) {
        DEBUG("[bme680]: No new data\n");
        return -BME680_NO_DATA;
    }

    return BME680_OK;
}
