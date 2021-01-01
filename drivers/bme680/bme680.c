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
 * @brief       Device driver implementation for the BME680
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
static int _read_reg(const bme680_t* dev, uint8_t reg, uint8_t* res);
static int _read_regs(const bme680_t* dev, uint8_t reg, uint32_t* res, uint8_t len);
static int _write_reg(const bme680_t* dev, uint8_t reg, const uint8_t* res);

int bme680_init(bme680_t* dev, const bme680_params_t* params)
{
    assert(dev && params);
    dev->params = *params;

    xtimer_init();

    uint8_t reset = BME680_RESET, reset_status = 0, chip_id = 0;

        DEBUG("write reset\n");
    /* reset device */
    if (_write_reg(dev, BME680_REGISTER_RESET, &reset) < 0){
        DEBUG("[bme680] error writing reset register\n");
        return -BME680_ERR_INTF;
    }

    DEBUG("check reset\n");
    /* check reset status */
    if (_read_reg(dev, BME680_REGISTER_RESET, &reset_status) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CHIP_ID register\n");
        return -BME680_ERR_INTF;
    }

    if (reset_status != 0){
        DEBUG("[bme680] error on reset, status: %d\n", reset_status);
        return -BME680_ERR_INTF;
    }

    uint8_t res = 0b00000000;
    
    /*int8_t tmp = spi_init_cs(DEV_SPI, NSS_PIN);

    printf("start spi with %d\n", tmp);
 
    if (tmp != SPI_OK) {
        puts("error: unable to initialize the given chip select line");
        return 1;
    }
    //unsigned int cpsr = irq_disable();
    tmp = spi_acquire(DEV_SPI, NSS_PIN, BME680_SPI_MODE, BME680_SPI_SPEED);
    printf("aquired\n");

    if (tmp == SPI_NOMODE) {
        puts("error: given SPI mode is not supported");
        return 1;
    }
    else if (tmp == SPI_NOCLK) {
        puts("error: targeted clock speed is not supported");
        return 1;
    }
    else if (tmp != SPI_OK) {
        puts("error: unable to acquire bus with given parameters");
        return 1;
    }


    //spi_transfer_regs(dev_spi, cs, 0x50, NULL, &res, 1);
    uint8_t written = 0;
    //gpio_clear(BME680_PARAM_SPI_NSS_PIN);
    spi_transfer_regs(DEV_SPI, NSS_PIN, 0x73, &res, NULL, 1);
    //gpio_set(BME680_PARAM_SPI_NSS_PIN);
    spi_transfer_regs(DEV_SPI, NSS_PIN, 0xF3, NULL, &written, 1);
    DEBUG("wrote in 0x73 %d and got %d\n", res, written);
    res = 0;
    xtimer_usleep(20);
    spi_transfer_regs(DEV_SPI, NSS_PIN, 0xD0, NULL, &res, 1);
    DEBUG("received in chip id %d\n", res);

    res = 0;
    spi_transfer_regs(DEV_SPI, NSS_PIN, 0xA6, NULL, &res, 1);
    spi_release(DEV_SPI);

    DEBUG("chip id: %d\n", chip_id);*/

    if (_read_reg(dev, BME680_REGISTER_CHIP_ID, &chip_id) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CHIP_ID register\n");
        return -BME680_ERR_NODEV;
    }

    if (chip_id != BME680_CHIP_ID){
        DEBUG("[bme680] wrong chip id: %d\n", chip_id);
        return -BME680_ERR_NODEV;
    }

    DEBUG("chip id: %d\n", chip_id);

    /* set humidity os */
    uint8_t setting = 0;
    if (_read_reg(dev, BME680_REGISTER_CTRL_HUM, &setting) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
        return -BME680_ERR_INTF;
    }
    setting = (setting & (0b11111000)) | dev->params.filter;
    if (_write_reg(dev, BME680_REGISTER_CTRL_HUM, &setting) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_HUM register\n");
        return -BME680_ERR_INTF;
    }

    DEBUG("check BME680_REGISTER_CTRL_HUM\n");
    if (_read_reg(dev, BME680_REGISTER_CTRL_HUM, &res) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CHIP_ID register\n");
        return -BME680_ERR_NODEV;
    }
     DEBUG("res %d with setting: %d\n", res, setting);

    /* set IIR Filter */
     setting = 0;
    if (_read_reg(dev, BME680_REGISTER_CONFIG, &setting) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
        return -BME680_ERR_INTF;
    }
    setting = (setting & (0b11100011)) | dev->params.filter;
    if (_write_reg(dev, BME680_REGISTER_CONFIG, &setting) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
        return -BME680_ERR_INTF;
    }
    
    if (dev->params.meas_gas){
        DEBUG("measure gas\n");
        /* calculate register values for gas heating temperature and duration */
        uint8_t res_heat_0 = 0, heat_duration = 0;
        
        _convert_res_heat(dev, &dev->params.gas_heating_temp, &dev->params.ambient_temp, &res_heat_0);
        calc_heater_dur(dev->params.gas_heating_time, &heat_duration);
    
        /* enable gas and select previously set heater settings */
        
        if (_read_reg(dev, BME680_REGISTER_CTRL_HUM, &setting) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_CONFIG register\n");
            return -BME680_ERR_INTF;
        }
        setting = (setting & (0b11100000)) | BME680_RUN_GAS;
        if (_write_reg(dev, BME680_REGISTER_CTRL_GAS_1, &setting) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_CTRL_GAS_L register\n");
            return -BME680_ERR_INTF;
        }

        /* set gas wait to calculated heat up duration */
    
        if (_write_reg(dev, BME680_REGISTER_GAS_WAIT_0, &heat_duration) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_GAS_WAIT_0 register\n");
            return -BME680_ERR_INTF;
        }

        /* set calculated heating temperature */ 
        
        if (_write_reg(dev, BME680_REGISTER_RES_HEAT_0, &res_heat_0) < 0){
            DEBUG("[bme680] error writing BME680_REGISTER_RES_HEAT_0 register\n");
            return -BME680_ERR_INTF;
        }
    }

    /* set temperature os and pressure os */
    uint8_t os_settings = (dev->params.temp_os << 5) | (dev->params.press_os << 2);
    if (_write_reg(dev, BME680_REGISTER_CTRL_MEAS, &os_settings) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_MEAS register\n");
        return -BME680_ERR_INTF;
    }

    /* read calibration parameters from sensor */

    bme680_calib_t calib_data;
    
    if (_read_reg(dev, BME680_REGISTER_PAR_G1, &calib_data.par_g1) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_G1 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_G2, (uint32_t*) &calib_data.par_g2, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_G2 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_G3, &calib_data.par_g3) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_G3 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_T1, (uint32_t*) &calib_data.par_t1, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_T1 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_T2, (uint32_t*) &calib_data.par_t2, 2) < 0){
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

    calib_data.par_h1 = ((uint16_t) par_h1_msb << 4) | (par_h1_h2_lsb >> 4);

    calib_data.par_h2 = ( (uint16_t) par_h2_msb << 4) | (par_h1_h2_lsb >> 4);

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

    if (_read_regs(dev, BME680_REGISTER_PAR_P2, (uint32_t*) &calib_data.par_p2, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P2 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_P3, &calib_data.par_p3) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P3 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_P4, (uint32_t*) &calib_data.par_p4, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P4 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_P5, (uint32_t*) &calib_data.par_p5, 2) < 0){
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

    if (_read_regs(dev, BME680_REGISTER_PAR_P8, (uint32_t*) &calib_data.par_p8, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P8 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_regs(dev, BME680_REGISTER_PAR_P9, (uint32_t*) &calib_data.par_p9, 2) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P9 register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PAR_P10, &calib_data.par_p10) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P10 register\n");
        return -BME680_ERR_INTF;
    }

    DEBUG("par p1: %d\n", calib_data.par_p1);
     DEBUG("par p2: %d\n", calib_data.par_p2);
      DEBUG("par p3: %d\n", calib_data.par_p3);
       DEBUG("par p4: %d\n", calib_data.par_p4);
        DEBUG("par p5: %d\n", calib_data.par_p5);
         DEBUG("par p6: %d\n", calib_data.par_p6);
          DEBUG("par p7: %d\n", calib_data.par_p7);
           DEBUG("par p8: %d\n", calib_data.par_p8);
            DEBUG("par p9: %d\n", calib_data.par_p9);
             DEBUG("par p10: %d\n", calib_data.par_p10);

    dev->calib = calib_data;

    return BME680_OK;
}

uint16_t bme680_read(const bme680_t *dev, bme680_data_t *data)
{
    assert(dev && data);
    uint8_t new_data, gas_valid = 0;
    
    uint8_t reg_ctrl_meas = 0;

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

    xtimer_msleep(100);

    /* check if new data is available */
    do {
        if (_read_reg(dev, BME680_REGISTER_MEAS_STATUS_0, &new_data) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_MEAS_STATUS register\n");
        return -BME680_ERR_INTF;
    }
    } while (!(new_data & BME680_NEW_DATA));

    /* read sensor data */
        if (_calc_press(dev, &data->t_fine, &data->pressure) != BME680_OK){
        return -BME680_ERR_CALC_PRESS;
    }

    if (_calc_temp(dev, &data->t_fine, &data->temperature) != BME680_OK){
        return -BME680_ERR_CALC_TEMP;
    }
    xtimer_msleep(100);

    if (_calc_hum(dev, &data->temperature, &data->humidity) != BME680_OK){
        return -BME680_ERR_CALC_HUM;
    }
    xtimer_msleep(100);

    xtimer_msleep(100);
    if (dev->params.meas_gas){
        xtimer_msleep(dev->params.gas_heating_time);
        xtimer_msleep(dev->params.gas_heating_time);

    if (_read_reg(dev, BME680_REGISTER_GAS_R_LSB, &gas_valid) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_MEAS_STATUS register\n");
        return -BME680_ERR_INTF;
    }

    /* check if gas measurement was successful */
    if ((gas_valid & BME680_GAS_MEASUREMENT_SUCCESS) != BME680_GAS_MEASUREMENT_SUCCESS){
        DEBUG("gas measurement not successful: %u\n", new_data);
        data->gas_status = 0;
        //return -BME680_ERR_HEAT_GAS;
    }
    else if (_calc_gas(dev, &data->gas_resistance) != BME680_OK){
        data->gas_status = 0;
        return -BME680_ERR_CALC_GAS;
    }
    else {
        data->gas_status = 1;
        DEBUG("gas measurement successful\n");
    }
    }

    return BME680_OK;
}

/* INTERNAL FUNCTIONS */


static int _read_reg(const bme680_t *dev, uint8_t reg, uint8_t* res){

    #if MODULE_PERIPH_SPI
    _set_memory_page(dev, reg);
    xtimer_usleep(20);
    reg = reg | (0b10000000);
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

static int _read_regs(const bme680_t *dev, uint8_t reg, uint32_t* res, uint8_t len){

    #if MODULE_PERIPH_SPI
    _set_memory_page(dev, reg);
    reg = reg | (0b10000000);
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
    reg = reg & (0b01111111);

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
    uint8_t mem_page = (reg & BME680_SPI_PAGE_0_MASK) ? 0b00000000 : 0b00010000;
    uint8_t tmp = spi_init_cs(DEV_SPI, NSS_PIN); 
    if (tmp != SPI_OK) {
        puts("error: unable to initialize the given chip select line\n");
        return 1;
    }
    spi_acquire(DEV_SPI, NSS_PIN, BME680_SPI_MODE, BME680_SPI_SPEED);

    /* read old register value */
    spi_transfer_regs(DEV_SPI, NSS_PIN, 0xF3, NULL, &prev, 1);
    mem_page = (prev & 0b11101111) | mem_page;
    //DEBUG("prev: %d, mem_page: %d\n", prev, mem_page);
    spi_transfer_regs(DEV_SPI, NSS_PIN, 0x73, &mem_page, NULL, 1);
    spi_release(DEV_SPI);

    return BME680_OK;
}
#endif

/* convert */
static uint16_t _convert_res_heat(const bme680_t *dev, uint16_t* gas_heating_temp, uint8_t* ambient_temp, uint8_t* res)
{
    uint8_t res_heat_val = 0;
    uint8_t res_heat_range = 0;

    if (_read_reg(dev, BME680_REGISTER_RES_HEAT_RANGE, &res_heat_range) < 0){
        DEBUG("[bme680] error reading BME60_REGISTER_GAS_R_LSB register\n");
        return -BME680_ERR_INTF;
    }

    res_heat_range = res_heat_range & BME680_RES_HEAT_RANGE_MASK;

      if (_read_reg(dev, BME680_REGISTER_RES_HEAT_VAL, &res_heat_val) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_RES_HEAT_VAL register\n");
        return -BME680_ERR_INTF;
      }

    /* calculate heater resistance using datasheet formula */
	uint8_t heatr_res;
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t var4;
	int32_t var5;
	int32_t heatr_res_x100;
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

    *res = temp_comp;

    DEBUG("temperature: %lu\n", temp_comp/100);

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
    hum_adc = (uint16_t) (((uint16_t) hum_adc_msb << 8) | (uint16_t) hum_adc_lsb);

	uint32_t var1 = (int32_t) (hum_adc - ((int32_t) ((int32_t) dev->calib.par_h1 << 4)))
		- ((((*temp_comp) * (int32_t) dev->calib.par_h3) / ((int32_t) 100)) >> 1);
	uint32_t var2 = ((int32_t) dev->calib.par_h2
		* ((((*temp_comp) * (int32_t) dev->calib.par_h4) / ((int32_t) 100))
			+ ((((*temp_comp) * (((*temp_comp) * (int32_t) dev->calib.par_h5) / ((int32_t) 100))) >> 6)
				/ ((int32_t) 100)) + (int32_t) (1 << 14))) >> 10;
	uint32_t var3 = var1 * var2;
	uint32_t var4 = (int32_t) dev->calib.par_h6 << 7;
	var4 = ((var4) + (((*temp_comp) * (int32_t) dev->calib.par_h7) / ((int32_t) 100))) >> 4;
	uint32_t var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
	uint32_t var6 = (var4 * var5) >> 1;
	uint32_t hum_comp = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;

    *res = hum_comp;
    DEBUG("humidity: %lu\n", hum_comp / 1000);
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
    DEBUG("[bme680] start calc press\n");
    uint32_t press_adc = 0;

    uint8_t press_adc_lsb, press_adc_msb, press_adc_xlsb = 0;

    if (_read_reg(dev, BME680_REGISTER_PRESS_ADC_XLSB, &press_adc_xlsb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PRESS_ADC_XSB register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PRESS_ADC_LSB, &press_adc_lsb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PRESS_ADC_LSB register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_PRESS_ADC_MSB, &press_adc_msb) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PRESS_ADC_MSB register\n");
        return -BME680_ERR_INTF;
    }

    //press_adc_msb = 150;

    press_adc = (uint32_t) (((uint32_t) press_adc_msb * 4096) | ((uint32_t) press_adc_lsb * 16)
				| ((uint32_t) press_adc_xlsb / 16));

    DEBUG("pressure adc msb: %d, lsb: %d, xlsb: %d\n", press_adc_lsb, press_adc_msb, press_adc_xlsb);


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

    *res = pressure_comp;

    DEBUG("pressure: %lu\n", pressure_comp);
  
    return BME680_OK;    
}

static uint16_t _calc_gas(const bme680_t *dev, uint32_t* res)
{
    uint16_t gas_adc = 0;
    uint8_t gas_range, range_switching_error, gas_adc_lsb, gas_adc_msb = 0;
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

    if (_read_reg(dev, BME680_REGISTER_GAS_RANGE, &gas_range) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_GAS_RANGE register\n");
        return -BME680_ERR_INTF;
    }

    if (_read_reg(dev, BME680_REGISTER_RANGE_SWITCHING_ERROR, &range_switching_error) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_GAS_RANGE_SWITCHING_ERROR register\n");
        return -BME680_ERR_INTF;
    }

    gas_range = gas_range & BME680_GAS_RANGE_MASK;

    gas_adc = (gas_adc_msb << 2) |  (gas_adc_lsb >> 6);

    /* calculate gas resistance using datasheet formula */

    int64_t var1 = (int64_t)(((1340 + (5 * (int64_t)range_switching_error)) * ((int64_t)const_array1_int[gas_range])) >> 16);
    int64_t var2 = (int64_t)(gas_adc << 15) - (int64_t)(1 << 24) + var1;
    int32_t gas_res = (int32_t)((((int64_t)(const_array2_int[gas_range] * (int64_t)var1) >> 9) + (var2 >> 1)) / var2);

    *res = gas_res;

    DEBUG("gas resistance: %lu\n", gas_res);

    return BME680_OK;
}