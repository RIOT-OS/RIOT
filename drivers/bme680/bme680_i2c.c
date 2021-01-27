/*
 * Copyright (C) 2020 OVGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_BME680
 * @{
 *
 * @file
 * @brief       Device driver implementation for the BME680 sensor.
 *
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdbool.h>

#include "bme680_i2c_params.h"
#include "bme680_internals.h"
#include "bme680_i2c.h"
#include "bme680_common.h"
#include "periph/i2c.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV_ADDR    (dev->i2c_intf.addr)
#define DEV_I2C     (dev->i2c_intf.dev)

static int _read_reg(const bme680_i2c_t *dev, uint8_t reg, void *res){

    i2c_acquire(DEV_I2C);
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, reg, res, 0) < 0){
        DEBUG("[bme680] error reading register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);
        return -EIO;
    }
    i2c_release(DEV_I2C);

    return 0;
}

static int _read_regs(const bme680_i2c_t *dev, uint8_t reg, void *res, uint8_t len){

    i2c_acquire(DEV_I2C);
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, reg, res, len, 0) < 0){
        DEBUG("[bme680] error reading register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);
        return -EIO;
    }
    i2c_release(DEV_I2C);

    return 0;
}

static int _write_reg(const bme680_i2c_t *dev, uint8_t reg, const uint8_t *res){

    i2c_acquire(DEV_I2C);
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, reg, *res, 0) < 0){
        DEBUG("[bme680] error writing register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);
        return -EIO;
    }
    i2c_release(DEV_I2C);

    return 0;
}

int bme680_i2c_init(bme680_i2c_t *dev, const bme680_i2c_params_t *params)
{
    assert(dev && params);
    dev->i2c_intf = params->i2c_params;
    dev->common.params = params->common_params;

    uint8_t reset = BME680_RESET, reset_status, chip_id, os_set, hum_set, filter_set;
    bme680_calib_t calib_data;

    /* reset device */
    if (_write_reg(dev, BME680_REGISTER_RESET, &reset) < 0){
        DEBUG("[bme680] error writing reset register\n");
        return -EIO;
    }

    /* check reset status */
    if (_read_reg(dev, BME680_REGISTER_RESET, &reset_status) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CHIP_ID register\n");
        return -EIO;
    }

    if (reset_status != 0){
        DEBUG("[bme680] error on reset\n");
        return -EIO;
    }

    if (_read_reg(dev, BME680_REGISTER_CHIP_ID, &chip_id) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CHIP_ID register\n");
        return -EIO;
    }

    if (chip_id != BME680_CHIP_ID){
        DEBUG("[bme680] wrong chip id\n");
        return -EIO;
    }

    /* set temperature os and pressure os */
    os_set = (dev->common.params.temp_os << 5) | (dev->common.params.press_os << 2);

    if (_write_reg(dev, BME680_REGISTER_CTRL_MEAS, &os_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_MEAS register\n");
        return -EIO;
    }

    if (_read_reg(dev, BME680_REGISTER_CTRL_HUM, &hum_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_HUM register\n");
        return -EIO;
    }
    hum_set = (hum_set & BME680_HUM_SETTINGS_MASK) | dev->common.params.hum_os;
    if (_write_reg(dev, BME680_REGISTER_CTRL_HUM, &hum_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_HUM register\n");
        return -EIO;
    }

    /* set IIR Filter */
    if (_read_reg(dev, BME680_REGISTER_CONFIG, &filter_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
        return -EIO;
    }
    filter_set = (filter_set & BME680_FILTER_SETTINGS_MASK) | (dev->common.params.filter << 2);

    if (_write_reg(dev, BME680_REGISTER_CONFIG, &filter_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
        return -EIO;
    }

    /* read calibration parameters from sensor */
    bme680_calib_chunk1_t calib_chunk1;
    bme680_calib_chunk2_t calib_chunk2;
    _read_regs(dev, BME680_REGISTER_CALIB_1, &calib_chunk1, sizeof(calib_chunk1));
    _read_regs(dev, BME680_REGISTER_CALIB_2, &calib_chunk2, sizeof(calib_chunk2));

    calib_data.par_t1 = calib_chunk1.par_t1;
    calib_data.par_t2 = calib_chunk2.par_t2;
    calib_data.par_h1 = (calib_chunk1.h1_msb << 4) | (calib_chunk1.h1_h2_lsb  & BME680_H1_H2_MASK);
    calib_data.par_h2 = (calib_chunk1.h2_msb << 4) | (calib_chunk1.h1_h2_lsb  & BME680_H1_H2_MASK);
    calib_data.par_t3 = calib_chunk2.par_t3;
    calib_data.par_p1 = calib_chunk2.par_p1;
    calib_data.par_p2 = calib_chunk2.par_p2;
    calib_data.par_p4 = calib_chunk2.par_p4;
    calib_data.par_p5 = calib_chunk2.par_p5;
    calib_data.par_p8 = calib_chunk2.par_p8;
    calib_data.par_p9 = calib_chunk2.par_p9;
    calib_data.par_g2 = calib_chunk1.par_g2;
    calib_data.par_h3 = calib_chunk1.par_h3;
    calib_data.par_h4 = calib_chunk1.par_h4;
    calib_data.par_h5 = calib_chunk1.par_h5;
    calib_data.par_h6 = calib_chunk1.par_h6;
    calib_data.par_h7 = calib_chunk1.par_h7;
    calib_data.par_p3 = calib_chunk2.par_p3;
    calib_data.par_p6 = calib_chunk2.par_p6;
    calib_data.par_p7 = calib_chunk2.par_p7;
    calib_data.par_p10 = calib_chunk2.par_p10;
    calib_data.par_g1 = calib_chunk1.par_g1;
    calib_data.par_g3 = calib_chunk1.par_g3;

    /* set gas settings and read gas calibration if enabled */
    if (dev->common.params.meas_gas){
        bme680_calib_chunk3_t calib_chunk3;
        _read_regs(dev, BME680_REGISTER_CALIB_3, &calib_chunk3, sizeof(calib_chunk3));

        calib_data.res_heat_val = calib_chunk3.res_heat_val;
        calib_data.res_heat_range = (calib_chunk3.res_heat_range & BME680_RES_HEAT_RANGE_MASK) >> 4;
        calib_data.range_sw_error = calib_chunk3.range_sw_error >> 4;

        dev->common.calib = calib_data;

        /* calculate register values for gas heating temperature and duration */
        uint8_t res_heat_0, heat_duration, set_gas;

        convert_res_heat(&dev->common, &res_heat_0);
        calc_heater_dur(dev->common.params.gas_heating_time, &heat_duration);

        /* set calculated heating temperature */
        if (_write_reg(dev, BME680_REGISTER_RES_HEAT_0, &res_heat_0) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_RES_HEAT_0 register\n");
            return -EIO;
        }

        /* set gas wait to calculated heat up duration */
        if (_write_reg(dev, BME680_REGISTER_GAS_WAIT_0, &heat_duration) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_GAS_WAIT_0 register\n");
            return -EIO;
        }

        /* enable gas and select heater settings */
        if (_read_reg(dev, BME680_REGISTER_CTRL_GAS_1, &set_gas) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_CTRL_GAS_1 register\n");
            return -EIO;
        }
        set_gas = (set_gas & (BME680_GAS_SETTINGS_MASK)) | BME680_RUN_GAS;
        if (_write_reg(dev, BME680_REGISTER_CTRL_GAS_1, &set_gas) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_CTRL_GAS_1 register\n");
            return -EIO;
        }
    }

    else{
        dev->common.calib = calib_data;
    }

    return 0;
}

int bme680_i2c_read(const bme680_i2c_t *dev, bme680_data_t *data)
{
    assert(dev && data);
    uint8_t new_data, reg_ctrl_meas;

    /* read os settings and set forced mode */
    if (_read_reg(dev, BME680_REGISTER_CTRL_MEAS, &reg_ctrl_meas) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CTRL_MEAS register\n");
        return -EIO;
    }

    reg_ctrl_meas = reg_ctrl_meas | BME680_FORCED_MODE;

    if (_write_reg(dev, BME680_REGISTER_CTRL_MEAS, &reg_ctrl_meas) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_MEAS register\n");
        return -EIO;
    }

    /* check if new data is available */
    xtimer_msleep(400);
    do {
        if (_read_reg(dev, BME680_REGISTER_MEAS_STATUS_0, &new_data) < 0){
            DEBUG("[bme680] error reading BME680_REGISTER_MEAS_STATUS register\n");
            return -EIO;
        }
        xtimer_msleep(10);
    } while (!(new_data & BME680_NEW_DATA));

    /* read uncompensated temperature, pressure, humidity */
    bme680_adc_readout_t adc_readout;
    if (_read_regs(dev, BME680_REGISTER_ADC, &adc_readout, sizeof(adc_readout)) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_ADC register\n");
        return -EIO;
    }

    uint32_t temp_adc, press_adc;
    uint16_t hum_adc;

    temp_adc = (uint32_t) (((uint32_t) adc_readout.temp_adc_msb << 12) | ((uint32_t) adc_readout.temp_adc_lsb << 4)
            | ((uint32_t) adc_readout.temp_adc_xlsb >> 4));

    hum_adc = (uint16_t) (((uint16_t) adc_readout.hum_adc_msb << 8) | (uint16_t) adc_readout.hum_adc_lsb);

    press_adc = (uint32_t) (((uint32_t) adc_readout.press_adc_msb << 12) | ((uint32_t) adc_readout.press_adc_lsb << 4)
            | ((uint32_t) adc_readout.press_adc_xlsb >> 4));


    /* calculate temperature, pressure, humidity */
    if (calc_temp(&dev->common, &data->temperature, &data->t_fine, temp_adc) != 0){
        DEBUG("[bme680] error calculating temperature\n");
        return -ERANGE;;
    }

    if (calc_press(&dev->common, &data->pressure, &data->t_fine, press_adc) != 0){
        DEBUG("[bme680] error calculating pressure\n");
        return -ERANGE;
    }

    if (calc_hum(&dev->common, &data->humidity, &data->temperature, hum_adc) != 0){
        DEBUG("[bme680] error calculating humidity\n");
        return -ERANGE;
    }

    /* evaluate gas data if required */
    if (dev->common.params.meas_gas){

        xtimer_msleep(dev->common.params.gas_heating_time);

        bme680_adc_readout_gas_t adc_readout_gas;
        if (_read_regs(dev, BME680_REGISTER_ADC_GAS, &adc_readout_gas, sizeof(adc_readout_gas)) < 0){
            DEBUG("[bme680] error reading BME680_REGISTER_ADC_GAS register\n");
            return -EIO;
        }

        /* check if gas measurement was successful */
        if ((adc_readout_gas.gas_adc_lsb & BME680_GAS_MEASUREMENT_SUCCESS) != BME680_GAS_MEASUREMENT_SUCCESS){
            DEBUG("[bme680] gas measurement not successful\n");
            data->gas_status = 0;
        }
        /* calculate gas */
        else{
            uint8_t gas_range = adc_readout_gas.gas_adc_lsb & BME680_GAS_RANGE_MASK;
            uint16_t gas_adc = (uint16_t) (uint16_t) (adc_readout_gas.gas_adc_msb << 2)
                |  ((uint16_t) adc_readout_gas.gas_adc_lsb >> 6);

            calc_gas(&dev->common, &data->gas_resistance, gas_range, gas_adc);
            data->gas_status = 1;
        }
    }

    return 0;
}
