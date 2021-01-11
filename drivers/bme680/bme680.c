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

#include "bme680.h"
#include "bme680_params.h"
#include "bme680_internals.h"
#include "stdbool.h"

#ifdef MODULE_BME680_I2C
#include "periph/i2c.h"
#endif

#ifdef MODULE_BME680_SPI
#include "periph/spi.h"
#endif

#include "xtimer.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#ifdef MODULE_BME680_I2C
#define DEV_ADDR    (dev->params.intf.i2c.addr)
#define DEV_I2C     (dev->params.intf.i2c.dev)
#endif

#ifdef MODULE_BME680_SPI
#define NSS_PIN    (dev->params.intf.spi.nss_pin)
#define DEV_SPI     (dev->params.intf.spi.dev)
#endif

static uint16_t _convert_res_heat(const bme680_t *dev, uint16_t* gas_heating_temp, uint8_t* ambient_temp, uint8_t* res);
static uint16_t _calc_temp(const bme680_t *dev, uint32_t* t_fine, uint32_t* res);
static uint16_t _calc_hum(const bme680_t *dev, const uint32_t *temp_comp, uint32_t* res);
static uint16_t _calc_press(const bme680_t *dev, uint32_t* t_fine, uint32_t* res);
static uint16_t _calc_gas(const bme680_t *dev, uint32_t* res);
static void calc_heater_dur(uint16_t dur, uint8_t* res);
#if MODULE_PERIPH_SPI
static uint16_t _set_memory_page(const bme680_t *dev, uint8_t reg);
#endif
static int _read_reg(const bme680_t* dev, uint8_t reg, void* res);
static int _read_regs(const bme680_t* dev, uint8_t reg, void* res, uint8_t len);
static int _write_reg(const bme680_t* dev, uint8_t reg, const uint8_t* res);

int bme680_init(bme680_t* dev, const bme680_params_t* params)
{

    assert(dev && params);
    dev->params = *params;

    xtimer_init();

    uint8_t reset = BME680_RESET, reset_status, chip_id, os_set, hum_set, filter_set;
    bme680_calib_t calib_data;

    /* reset device */
    if (_write_reg(dev, BME680_REGISTER_RESET, &reset) < 0){
        DEBUG("[bme680] error writing reset register\n");
        return -BME680_ERR_INTF;
    }

    /* check reset status */
    if (_read_reg(dev, BME680_REGISTER_RESET, &reset_status) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CHIP_ID register\n");
        return -BME680_ERR_INTF;
    }

    if (reset_status != 0){
        DEBUG("[bme680] error on reset, status: %d\n", reset_status);
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_CHIP_ID, &chip_id) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CHIP_ID register\n");
        return -BME680_ERR_NODEV;
    }

    if (chip_id != BME680_CHIP_ID){
        DEBUG("[bme680] wrong chip id: %d\n", chip_id);
        return -BME680_ERR_NODEV;
    }

    /* set temperature os and pressure os */
    os_set = (dev->params.temp_os << 5) | (dev->params.press_os << 2);

    if (_write_reg(dev, BME680_REGISTER_CTRL_MEAS, &os_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_MEAS register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_CTRL_HUM, &hum_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
        return -BME680_ERR_INTF;
    }
    hum_set = (hum_set & BME680_HUM_SETTINGS_MASK) | dev->params.hum_os;
    if (_write_reg(dev, BME680_REGISTER_CTRL_HUM, &hum_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_HUM register\n");
        return -BME680_ERR_INTF;
    }

    /* set IIR Filter */
    if (_read_reg(dev, BME680_REGISTER_CONFIG, &filter_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
        return -BME680_ERR_INTF;
    }
    filter_set = (filter_set & BME680_FILTER_SETTINGS_MASK) | (dev->params.filter << 2);

    if (_write_reg(dev, BME680_REGISTER_CONFIG, &filter_set) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
        return -BME680_ERR_INTF;
    }

    /* read calibration parameters from sensor */
    if (_read_reg(dev, BME680_REGISTER_PAR_G1, &calib_data.par_g1) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_G1 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_G2, (int32_t*) &calib_data.par_g2, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_G2 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_G3, &calib_data.par_g3) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_G3 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_T1, (int32_t*) &calib_data.par_t1, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_T1 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_T2, (int32_t*) &calib_data.par_t2, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_T2 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_T3, &calib_data.par_t3) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_T3 register\n");
        return -BME680_ERR_INTF;
    }
   
    uint8_t par_h1_h2_lsb = 0, par_h1_msb = 0, par_h2_msb = 0;

    if (_read_reg(dev, BME680_REGISTER_PAR_H1_H2_LSB, &par_h1_h2_lsb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H1_H2_LSB register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_H1_MSB, &par_h1_msb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H1_MSB register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_H2_MSB, &par_h2_msb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H2_MSB register\n");
        return -BME680_ERR_INTF;
    }

    calib_data.par_h1 = (par_h1_msb << 4) | (par_h1_h2_lsb  & BME680_H1_H2_MASK);
    calib_data.par_h2 = (par_h2_msb << 4) | (par_h1_h2_lsb & BME680_H1_H2_MASK);

    if (_read_reg(dev, BME680_REGISTER_PAR_H3, &calib_data.par_h3) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H2_MSB register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_H4, &calib_data.par_h4) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H4 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_H5, &calib_data.par_h5) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H5 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_H6, &calib_data.par_h6) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H6 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_H7, &calib_data.par_h7) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H6 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_P1, (uint32_t*) &calib_data.par_p1, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P1 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_P2, (int32_t*) &calib_data.par_p2, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P2 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_P3, &calib_data.par_p3) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P3 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_P4, (int32_t*) &calib_data.par_p4, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P4 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_P5, (int32_t*) &calib_data.par_p5, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P5 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_P6, &calib_data.par_p6) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P6 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_P7, &calib_data.par_p7) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P7 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_P8, (int32_t*) &calib_data.par_p8, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P8 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_P9, (int32_t*) &calib_data.par_p9, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P9 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_P10, &calib_data.par_p10) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P10 register\n");
        return -BME680_ERR_INTF;
    }

    dev->calib = calib_data;

        /* set gas settings if enabled */
       if (dev->params.meas_gas){

        /* calculate register values for gas heating temperature and duration */
        uint8_t res_heat_0, heat_duration, set_gas;
        
        _convert_res_heat(dev, &dev->params.gas_heating_temp, &dev->params.ambient_temp, &res_heat_0);
        calc_heater_dur(dev->params.gas_heating_time, &heat_duration);

        /* set calculated heating temperature */
        if (_write_reg(dev, BME680_REGISTER_RES_HEAT_0, &res_heat_0) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_RES_HEAT_0 register\n");
            return -BME680_ERR_INTF;
        }

        /* set gas wait to calculated heat up duration */
        if (_write_reg(dev, BME680_REGISTER_GAS_WAIT_0, &heat_duration) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_GAS_WAIT_0 register\n");
            return -BME680_ERR_INTF;
        }

        /* enable gas and select previously set heater settings */
        if (_read_reg(dev, BME680_REGISTER_CTRL_GAS_1, &set_gas) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
            return -BME680_ERR_INTF;
        }
        set_gas = (set_gas & (BME680_GAS_SETTINGS_MASK)) | BME680_RUN_GAS;
        if (_write_reg(dev, BME680_REGISTER_CTRL_GAS_1, &set_gas) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_CTRL_GAS_L register\n");
            return -BME680_ERR_INTF;
        }
    }

    return BME680_OK;
}

uint16_t bme680_read(const bme680_t *dev, bme680_data_t *data)
{
    assert(dev && data);
    uint8_t new_data, gas_valid, reg_ctrl_meas;

    /* read os settings and set forced mode */
    if (_read_reg(dev, BME680_REGISTER_CTRL_MEAS, &reg_ctrl_meas) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CTRL_MEAS register\n");
        return -BME680_ERR_INTF;
    }
    
    reg_ctrl_meas = reg_ctrl_meas | BME680_FORCED_MODE;

    if (_write_reg(dev, BME680_REGISTER_CTRL_MEAS, &reg_ctrl_meas) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_MEAS register\n");
        return -BME680_ERR_INTF;
    }

    /* check if new data is available */
    do {
        if (_read_reg(dev, BME680_REGISTER_MEAS_STATUS_0, &new_data) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_MEAS_STATUS register\n");
        return -BME680_ERR_INTF;
    }
    } while (!(new_data & BME680_NEW_DATA));

    /* read sensor data */
    if (_calc_temp(dev, &data->t_fine, &data->temperature) != BME680_OK){
        return -BME680_ERR_CALC_TEMP;
    }

    if (_calc_press(dev, &data->t_fine, &data->pressure) != BME680_OK){
        return -BME680_ERR_CALC_PRESS;
    }

    if (_calc_hum(dev, &data->temperature, &data->humidity) != BME680_OK){
        return -BME680_ERR_CALC_HUM;
    }

    if (dev->params.meas_gas){

        xtimer_msleep(dev->params.gas_heating_time);
        if (_read_reg(dev, BME680_REGISTER_GAS_R_LSB, &gas_valid) < 0){
            DEBUG("[bme680] error reading BME680_REGISTER_MEAS_STATUS register\n");
            return -BME680_ERR_INTF;
        }

        /* check if gas measurement was successful */
        if ((gas_valid & BME680_GAS_MEASUREMENT_SUCCESS) != BME680_GAS_MEASUREMENT_SUCCESS){

            data->gas_status = 0;
        }
        else if (_calc_gas(dev, &data->gas_resistance) != BME680_OK){
            data->gas_status = 0;
            return -BME680_ERR_CALC_GAS;
        }
        else {
            data->gas_status = 1;
        }
    }

    return BME680_OK;
}

/* INTERNAL FUNCTIONS */


static int _read_reg(const bme680_t *dev, uint8_t reg, void* res){

    #if MODULE_PERIPH_SPI
    _set_memory_page(dev, reg);
    reg = reg | (0x80);
    uint8_t tmp = spi_init_cs(DEV_SPI, NSS_PIN); 
    if (tmp != SPI_OK) {
        puts("error: unable to initialize the given chip select line\n");
        return -BME680_ERR_INTF;
    }

    spi_acquire(DEV_SPI, NSS_PIN, BME680_SPI_MODE, BME680_SPI_SPEED);
    spi_transfer_regs(DEV_SPI, NSS_PIN, reg, NULL, res, 1);
    spi_release(DEV_SPI);
    #endif

    #if MODULE_PERIPH_I2C
    i2c_acquire(DEV_I2C);
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, reg, res, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CHIP_ID register\n");
        // Release I2C device 
        i2c_release(DEV_I2C);

        return -BME680_ERR_INTF;
    }
    i2c_release(DEV_I2C);
    #endif

    return BME680_OK;
}

static int _read_regs(const bme680_t *dev, uint8_t reg, void* res, uint8_t len){

    #if MODULE_PERIPH_SPI
    _set_memory_page(dev, reg);
    reg = reg | (0x80);
    xtimer_usleep(20);
    uint8_t tmp = spi_init_cs(DEV_SPI, NSS_PIN); 
    if (tmp != SPI_OK) {
        puts("error: unable to initialize the given chip select line\n");
        return -BME680_ERR_INTF;
    }

    spi_acquire(DEV_SPI, NSS_PIN, BME680_SPI_MODE, BME680_SPI_SPEED);
    spi_transfer_regs(DEV_SPI, NSS_PIN, reg, NULL, res, len);
    spi_release(DEV_SPI);
    #endif

    #if MODULE_PERIPH_I2C
    i2c_acquire(DEV_I2C);
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, reg, res, len, 0) < 0){
        DEBUG("[bme680] error reading register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);
        return -BME680_ERR_INTF;
    }
    i2c_release(DEV_I2C);
    #endif

    return BME680_OK;
}

static int _write_reg(const bme680_t *dev, uint8_t reg, const uint8_t* res){
    #if MODULE_PERIPH_SPI
    _set_memory_page(dev, reg);
    reg = reg & (0x7F);

    uint8_t tmp = spi_init_cs(DEV_SPI, NSS_PIN); 
    if (tmp != SPI_OK) {
        puts("error: unable to initialize the given chip select line\n");
        return -BME680_ERR_INTF;
    }

    spi_acquire(DEV_SPI, NSS_PIN, BME680_SPI_MODE, BME680_SPI_SPEED);
    spi_transfer_regs(DEV_SPI, NSS_PIN, reg, res, NULL, 1);
    spi_release(DEV_SPI);
    #endif

    #if MODULE_PERIPH_I2C
    i2c_acquire(DEV_I2C);
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, reg, *res, 0) < 0){
        DEBUG("[bme680] error writing register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_INTF;
    }
    i2c_release(DEV_I2C);
    #endif

    return BME680_OK;
}


// for SPI communication
#if MODULE_PERIPH_SPI
static uint16_t _set_memory_page(const bme680_t *dev, uint8_t reg){

    uint8_t prev = 0;
    uint8_t mem_page = (reg & BME680_SPI_PAGE_0_MASK) ? BME680_SPI_PAGE_0 : BME680_SPI_PAGE_1;
    uint8_t tmp = spi_init_cs(DEV_SPI, NSS_PIN); 
    if (tmp != SPI_OK) {
        puts("error: unable to initialize the given chip select line\n");
        return 1;
    }
    spi_acquire(DEV_SPI, NSS_PIN, BME680_SPI_MODE, BME680_SPI_SPEED);

    /* read old register value */
    spi_transfer_regs(DEV_SPI, NSS_PIN, 0xF3, NULL, &prev, 1);
    mem_page = (prev & 0b11101111) | mem_page;
    spi_transfer_regs(DEV_SPI, NSS_PIN, 0x73, &mem_page, NULL, 1);
    spi_release(DEV_SPI);

    return BME680_OK;
}
#endif

/* convert heating config to register value */
static uint16_t _convert_res_heat(const bme680_t *dev, uint16_t* gas_heating_temp, uint8_t* ambient_temp, uint8_t* res)
{
    uint8_t res_heat_val, heatr_res;
    uint8_t res_heat_range = 0;
    int32_t var1, var2, var3, var4, var5, heatr_res_x100;

    if (_read_reg(dev, BME680_REGISTER_RES_HEAT_RANGE, &res_heat_range) < 0){
        DEBUG("[bme680] error reading BME60_REGISTER_GAS_R_LSB register\n");
        return -BME680_ERR_INTF;
    }

    res_heat_range = (res_heat_range & BME680_RES_HEAT_RANGE_MASK) >> 4;

      if (_read_reg(dev, BME680_REGISTER_RES_HEAT_VAL, &res_heat_val) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_RES_HEAT_VAL register\n");
        return -BME680_ERR_INTF;
      }

    /* calculate heater resistance using datasheet formula */

    var1 = (((int32_t) (*ambient_temp) * dev->calib.par_g3) / 1000) * 256;
	var2 = (dev->calib.par_g1 + 784) * (((((dev->calib.par_g2 + 154009) * (*gas_heating_temp) * 5) / 100) + 3276800) / 10);
	var3 = var1 + (var2 / 2);
	var4 = (var3 / ((res_heat_range) + 4));
	var5 = (131 * res_heat_val) + 65536;
	heatr_res_x100 = (int32_t) (((var4 / var5) - 250) * 34);
	heatr_res = (uint8_t) ((heatr_res_x100 + 50) / 100);

    *res = heatr_res;

    return BME680_OK;
}

static uint16_t _calc_temp(const bme680_t *dev, uint32_t* t_fine, uint32_t* res)
{
    /* read uncompensated values */
    uint32_t temp_adc = 0;
    uint8_t temp_adc_lsb = 0, temp_adc_msb = 0, temp_adc_xlsb = 0;

    if (_read_reg(dev, BME680_REGISTER_TEMP_ADC_XLSB, &temp_adc_xlsb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_RES_HEAT_VAL register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_TEMP_ADC_LSB, &temp_adc_lsb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_RES_HEAT_VAL register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_TEMP_ADC_MSB, &temp_adc_msb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_RES_HEAT_VAL register\n");
        return -BME680_ERR_INTF;
    }

    temp_adc = (uint32_t) (((uint32_t) temp_adc_msb << 12) | ((uint32_t) temp_adc_lsb << 4)
                | ((uint32_t) temp_adc_xlsb >> 4));

    /* calculate compensated temperature using data sheet formula */
    int32_t var1 = ((int32_t)temp_adc >> 3) - ((int32_t)dev->calib.par_t1 << 1);
    int32_t var2 = (var1 * (int32_t)dev->calib.par_t2) >> 11;
    int32_t var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)dev->calib.par_t3 << 4)) >> 14;
    *t_fine = var2 + var3;
    int32_t temp_comp = ((*t_fine * 5) + 128) >> 8;

    if ((temp_comp < (-4000)) | (temp_comp > 8500)){
        return -BME680_ERR_CALC_TEMP;
    }

    *res = temp_comp;
    return BME680_OK;
}

static uint16_t _calc_hum(const bme680_t *dev, const uint32_t *temp_comp, uint32_t* res)
{
    uint16_t hum_adc = 0;

    uint8_t hum_adc_msb, hum_adc_lsb = 0;

    if (_read_reg(dev, BME680_REGISTER_HUM_ADC_MSB, &hum_adc_msb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_HUM_ADC register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_HUM_ADC_LSB, &hum_adc_lsb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_HUM_ADC register\n");
        return -BME680_ERR_INTF;
    }

    /* calculate humdity using datasheet formula */
    hum_adc = (uint16_t) (((uint16_t) hum_adc_msb * 256) | (uint16_t) hum_adc_lsb);

	int32_t var1 = (int32_t) (hum_adc - ((int32_t) ((int32_t) dev->calib.par_h1 << 4)))
		- (((((int32_t) *temp_comp) * (int32_t) dev->calib.par_h3) / ((int32_t) 100)) >> 1);
	int32_t var2 = ((int32_t) dev->calib.par_h2
		* ((((*temp_comp) * (int32_t) dev->calib.par_h4) / ((int32_t) 100))
			+ (((((int32_t) *temp_comp) * ((((int32_t) *temp_comp) * (int32_t) dev->calib.par_h5) / ((int32_t) 100))) >> 6)
				/ ((int32_t) 100)) + (int32_t) (1 << 14))) >> 10;
	int32_t var3 = var1 * var2;
	int32_t var4 = (int32_t) dev->calib.par_h6 << 7;
	var4 = ((var4) + ((((int32_t) *temp_comp) * (int32_t) dev->calib.par_h7) / ((int32_t) 100))) >> 4;
	int32_t var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
	int32_t var6 = (var4 * var5) >> 1;
	int32_t hum_comp = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;

    if ((hum_comp < 0) || (hum_comp > 100000)){
        return -BME680_ERR_CALC_HUM;
    }

    *res = (uint32_t) hum_comp;
    return BME680_OK;  
}


/* translate heating duration to register value */
static void calc_heater_dur(uint16_t dur, uint8_t* res)
{
	uint8_t factor = 0;

	if (dur >= 0xfc0) {
		*res = 0xff; /* Max duration */
	} else {
		while (dur > 0x3F) {
			dur = dur / 4;
			factor += 1;
		}
		*res = (uint8_t) (dur + (factor * 64));
	}
}

static uint16_t _calc_press(const bme680_t *dev, uint32_t* t_fine, uint32_t* res)
{
    uint32_t press_adc = 0;

    uint8_t press_adc_lsb = 0, press_adc_msb = 0, press_adc_xlsb = 0;

    if (_read_reg(dev, BME680_REGISTER_PRESS_ADC_MSB, &press_adc_msb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PRESS_ADC_MSB register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PRESS_ADC_LSB, &press_adc_lsb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PRESS_ADC_LSB register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PRESS_ADC_XLSB, &press_adc_xlsb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PRESS_ADC_XSB register\n");
        return -BME680_ERR_INTF;
    }

    press_adc = (uint32_t) (((uint32_t) press_adc_msb * 4096) | ((uint32_t) press_adc_lsb * 16)
				| ((uint32_t) press_adc_xlsb / 16));

    /* calculate pressure using datasheet formula */
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t pressure_comp;

    var1 = (((int32_t)* t_fine) >> 1) - 64000;
	var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)dev->calib.par_p6) >> 2;
	var2 = var2 + ((var1 * (int32_t) dev->calib.par_p5) << 1);
	var2 = (var2 >> 2) + ((int32_t) dev->calib.par_p4 << 16);
	var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) *
		((int32_t) dev->calib.par_p3 << 5)) >> 3) + (((int32_t) dev->calib.par_p2 * var1) >> 1);
	var1 = var1 >> 18;
	var1 = ((32768 + var1) * (int32_t) dev->calib.par_p1) >> 15;
	pressure_comp = 1048576 - press_adc;
	pressure_comp = (uint32_t)((pressure_comp - (var2 >> 12)) * ((uint32_t)3125));
    if (pressure_comp >= (1 << 30)){
        pressure_comp = ((pressure_comp / (uint32_t)var1) << 1);
    }
    else {
	    pressure_comp = ((pressure_comp << 1) / (uint32_t)var1);
    }
	var1 = ((int32_t) dev->calib.par_p9 * (int32_t)(((pressure_comp >> 3) *
		(pressure_comp >> 3)) >> 13)) >> 12;
	var2 = ((int32_t)(pressure_comp >> 2) *
		(int32_t) dev->calib.par_p8) >> 13;
	var3 = ((int32_t)(pressure_comp >> 8) * (int32_t)(pressure_comp >> 8) *
		(int32_t)(pressure_comp >> 8) *
		(int32_t) dev->calib.par_p10) >> 17;

	pressure_comp = (int32_t)(pressure_comp) + ((var1 + var2 + var3 +
		((int32_t) dev->calib.par_p7 << 7)) >> 4); 

    if ((pressure_comp < 30000) | (pressure_comp > 110000)){
        return -BME680_ERR_CALC_PRESS;
    }

    *res = pressure_comp;
  
    return BME680_OK;    
}

static uint16_t _calc_gas(const bme680_t *dev, uint32_t* res)
{
    uint16_t gas_adc = 0;
    uint8_t gas_adc_lsb = 0, gas_adc_msb = 0;
    uint8_t gas_range = 0, range_switching_error = 0;
    int64_t var1;
	uint64_t var2;
	uint32_t gas_res;

    uint32_t const_array1_int[] = CONST_ARRAY1_INT;
    uint32_t const_array2_int[] = CONST_ARRAY2_INT;

    if (_read_reg(dev, BME680_REGISTER_GAS_ADC_MSB, &gas_adc_msb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_GAS_ADC_MSB register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_GAS_ADC_LSB, &gas_adc_lsb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_GAS_ADC_LSB register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_RANGE_SW_ERR, &range_switching_error) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_GAS_RANGE_SWITCHING_ERROR register\n");
        return -BME680_ERR_INTF;
    }

    gas_range = gas_adc_lsb & BME680_GAS_RANGE_MASK;

    gas_adc = (uint16_t) ((uint16_t) gas_adc_msb << 2) |  ((uint16_t) gas_adc_lsb >> 4);

    /* calculate gas resistance using datasheet formula */
    var1 = (int64_t)(((1340 + (5 * (int64_t)range_switching_error)) * ((int64_t)const_array1_int[gas_range])) >> 16);
    var2 = (int64_t)(gas_adc << 15) - (int64_t)(1 << 24) + var1;
    gas_res = (int32_t)((((int64_t)(const_array2_int[gas_range] * (int64_t)var1) >> 9) + (var2 >> 1)) / var2);
    
    *res = gas_res;

    return BME680_OK;
}