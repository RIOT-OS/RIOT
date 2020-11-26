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

#include <math.h> //remove
#include <stdint.h> //remove

#include "bme680.h"
#include "bme680_params.h"
#include "bme680_internals.h"
#include "periph/i2c.h"
#include "xtimer.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define DEV_ADDR    (dev->params.i2c_addr)
#define DEV_I2C     (dev->params.i2c_dev)

static uint16_t _convert_res_heat(const bme680_t *dev, uint8_t* res);
static uint16_t _calc_temp(const bme680_t *dev, uint32_t* t_fine, uint16_t* res);
//static uint16_t _calc_temp_old(const bme680_t *dev, uint32_t* t_fine, uint16_t* res);
static uint16_t _calc_hum(const bme680_t *dev, const uint16_t *temp_comp, uint16_t* res);
static uint16_t _calc_press(const bme680_t *dev, uint32_t* t_fine, uint16_t* res);

int bme680_init(bme680_t *dev, const bme680_params_t *params)
{
    assert(dev && params);
    dev->params = *params;

    xtimer_init();
    
    /*Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t temp = 0;
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_TEMP_ADC, &temp, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_TEMP_ADC register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    DEBUG("[bme680] temperature before reset: %u\n", temp);


    DEBUG("[bme680] acquired\n");
    /* reset device */
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_RESET, BME680_RESET, 0) < 0){
        DEBUG("[bme680] error writing reset register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_WRITE;
    }
    DEBUG("[bme680] reset\n");

    xtimer_msleep(20);

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_RESET, &temp, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_RESET register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    DEBUG("[bme680] after reset: %u\n", temp);

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_GAS_L, &temp, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_CTRL_GAS_L register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    DEBUG("[bme680] ctrl gas after reset: %u\n", temp);

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_HUM_ADC, &temp, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_HUM_ADC register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    DEBUG("[bme680] hum adc after reset: %u\n", temp);

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PRESS_ADC, &temp, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PRESS_ADC register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    DEBUG("[bme680] press adc after reset: %u\n", temp);

    /* set humidity oversampling */
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_HUM, 0b100, 0) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_HUM register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_WRITE;
    }

    uint8_t res_heat_0 = 0;
    _convert_res_heat(dev, &res_heat_0);

    DEBUG("[bme680] heating temp\n");

    /* set gas wait to 100 ms heat up duration */
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_GAS_WAIT_0, 0x59, 0) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_GAS_WAIT_0 register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_WRITE;
    }

    /* set calculated heating temperature */
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_RES_HEAT_0, res_heat_0, 0) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_RES_HEAT_0 register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_WRITE;
    }

    /* enable gas and select previously set heater settings */
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_GAS_L, (1 << 4), 0) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_GAS_L register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_WRITE;
    }
    DEBUG("[bme680] heater settings\n");
    /* set temperature oversampling, pressure oversampling and set forced mode */
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_MEAS, (0b010 << 5) | (0b100 << 2) | (0b01), 0) < 0){
        DEBUG("[bme680] error writing BME680_REGISTER_CTRL_MEAS register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_WRITE;
    }
    DEBUG("[bme680] wrote all registers\n");
    return 0;
}

uint16_t bme680_read(const bme680_t *dev, bme680_data_t *data)
{
    assert(dev);
    uint16_t status = 0;
    DEBUG("[bme680] wait\n");
    xtimer_msleep(100);
    do {
        i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_MEAS_STATUS_0, &status, 0);
    } while (! (status & (1<<7)));
    DEBUG("[bme680] completed waiting\n");

    /* check if gas measurement was successful */

    uint8_t success = 0;
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME60_REGISTER_GAS_R_LSB, &success, 0) < 0){
        DEBUG("[bme680] error reading BME60_REGISTER_GAS_R_LSB register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_WRITE;
    }

    DEBUG("gas measurement: %u\n", success);
    if ((success & 0b00110000) != 0b00110000){
        DEBUG("gas measurement not successful: %u\n", success);
        /* Release I2C device */
        i2c_release(DEV_I2C);

        //return -BME680_ERR_I2C_WRITE;
    }

    if (!_calc_temp(dev, &data->t_fine, &data->temperature)){
        return -BME680_ERR_CALC_TEMP;
    }
    if (!_calc_hum(dev, &data->temperature, &data->humidity)){
        return -BME680_ERR_CALC_HUM;
    }
    if (!_calc_press(dev, &data->t_fine, &data->pressure)){
        return -BME680_ERR_CALC_PRESS;
    }      
    i2c_release(DEV_I2C);
    return 0;
}

/* INTERNAL FUNCTIONS */

static uint16_t _convert_res_heat(const bme680_t *dev, uint8_t* res){

    int8_t res_heat_val = 0;
    uint8_t par_g1, par_g3, res_heat_range = 0;
    uint16_t par_g2 = 0;

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_G1, &par_g1, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_G1 register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_G2, &par_g2, 2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_G2 register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_G3, &par_g3, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_G3 register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_RES_HEAT_RANGE, &res_heat_range, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_RES_HEAT_RANGE register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    res_heat_range = res_heat_range & 0b00110000;

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_RES_HEAT_VAL, &res_heat_val, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_RES_HEAT_VAL register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    uint8_t target_temp = dev->params.gas_heating_temp;
    uint8_t amb_temp = dev->params.ambient_temp;
    uint8_t var1 = (((int32_t)amb_temp * par_g3) / 1000) * 256;
    uint8_t var2 = (par_g1 + 784) * (((((par_g2 + 154009) * target_temp * 5) / 100) + 3276800) / 10);
    uint8_t var3 = var1 + (var2 >> 1);
    uint8_t var4 = (var3 / (res_heat_range + 4));
    uint8_t var5 = (131 * res_heat_val) + 65536;
    uint8_t res_heat_x100 = (int32_t)(((var4 / var5) - 250) * 34);
    uint8_t res_heat_x = (uint8_t)((res_heat_x100 + 50) / 100);

    *res = res_heat_x;

    return 1;
}




static uint16_t _calc_temp(const bme680_t *dev, uint32_t* t_fine, uint16_t* res){

    /* read uncompensated values */
    uint32_t temp_adc = 0;
    uint16_t par_t1, par_t2 = 0;
    uint8_t par_t3 = 0;
    //uint8_t temp_adc_lsb, temp_adc_msb, temp_adc_xsb = 0;

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_T1, &par_t1, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_T1 register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_T2, &par_t2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_T2 register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    /*
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, 0x23, &temp_adc_lsb, 0) < 0){
        DEBUG("[bme680] error writing reset register\n");

        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, 0x22, &temp_adc_msb, 0) < 0){
        DEBUG("[bme680] error writing reset register\n");

        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, 0x24, &temp_adc_xsb, 0) < 0){
        DEBUG("[bme680] error writing reset register\n");

        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    temp_adc = 0;
    temp_adc = (temp_adc_msb << 15) | (temp_adc_lsb << 7) | (temp_adc_xsb & 0b11110000);*/


    printf("temp adc manuell: %lu\n", temp_adc);
    temp_adc = 0;
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_TEMP_ADC, &temp_adc, 3, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_TEMP_ADC register\n");
        
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    temp_adc = temp_adc & 0xFFFFF0;
    // desired value: temp_adc = 8432752;

    printf("temp adc automatic: %lu\n", temp_adc);

    DEBUG("[bme680] read uncompensated\n");

    /* calculate compensated temperature using data sheet formula */
    int32_t var1 = ((int32_t)temp_adc >> 3) - ((int32_t)par_t1 << 1);
    int32_t var2 = (var1 * (int32_t)par_t2) >> 11;
    int32_t var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)par_t3 << 4)) >> 14;
    *t_fine = var2 + var3;
    int32_t temp_comp = ((*t_fine * 5) + 128) >> 8;
    DEBUG("[bme680] temperature compensated: %lu\n", temp_comp/100); 

    *res = temp_comp;

    return 1;
}

/*static uint16_t _calc_temp_old(const bme680_t *dev, uint32_t* t_fine, uint16_t* res){
    
    uint32_t temp_adc = 0;
    uint16_t par_t1, par_t2 = 0;
    uint8_t par_t3 = 0;

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_TEMP, &temp_adc, 3, 0) < 0){
        DEBUG("[bme680] error writing reset register\n");
        
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    printf("temp adc: %lu\n", temp_adc);

    temp_adc = temp_adc & 0xFFFFF0;

    printf("temp adc: %lu\n", temp_adc);

    DEBUG("[bme680] read uncompensated\n");
    
        
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_T1, &par_t1, 2, 0) < 0){
        DEBUG("[bme680] error writing reset register\n");
            
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    DEBUG("[bme680] read compensation params 1\n");

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_T2, &par_t2, 2, 0) < 0){
        DEBUG("[bme680] error writing reset register\n");
            
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    DEBUG("[bme680] read compensation params 2\n");

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_T3, &par_t3, 0) < 0){
        DEBUG("[bme680] error writing reset register\n");
            
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }
    DEBUG("[bme680] read compensation params 3\n");

        
    int32_t var1 = ((int32_t)temp_adc >> 3) - ((int32_t)par_t1 << 1);
    int32_t var2 = (var1 * (int32_t)par_t2) >> 11;
    int32_t var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)par_t3 << 4)) >> 14;
    *t_fine = var2 + var3;
    int32_t temp_comp = ((*t_fine * 5) + 128) >> 8;
    DEBUG("[bme680] temperature compensated: %lu\n", temp_comp);

    *res = temp_comp;

    return 1;
}*/


static uint16_t _calc_hum(const bme680_t *dev, const uint16_t *temp_comp, uint16_t* res){
    DEBUG("[bme680] start calc hum\n");
    uint16_t par_h1, par_h2, hum_adc = 0;
    uint8_t par_h3, par_h4, par_h5, par_h6, par_h7 = 0;
    uint8_t par_h2_msb, par_h2_lsb = 0;
   if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_H1, &par_h1, 2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H1 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    par_h1 = par_h1 & 0xFFF0;

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_H2, &par_h2_lsb, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H2 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, 0xE1, &par_h2_msb, 0) < 0){
        DEBUG("[bme680] error reading 0xE1 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    par_h2 = (par_h2_msb << 7) | (par_h2_lsb & 0xF0);

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_H3, &par_h3, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H3 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_H4, &par_h4, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H4 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_H5, &par_h5, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H5 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_H6, &par_h6, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H6 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_H7, &par_h7, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_H7 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_HUM_ADC, &hum_adc, 2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_HUM_ADC register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    int32_t temp_scaled = (int32_t)temp_comp;
    int32_t var1 = (int32_t)hum_adc - (int32_t)((int32_t)par_h1 << 4) - (((temp_scaled * (int32_t)par_h3) / ((int32_t)100)) >> 1);
    int32_t var2 = ((int32_t)par_h2 * (((temp_scaled * (int32_t)par_h4) / ((int32_t)100)) + (((temp_scaled * ((temp_scaled * (int32_t)par_h5) /
                ((int32_t)100))) >> 6) / ((int32_t)100)) + ((int32_t)(1 << 14)))) >> 10;
    int32_t var3 = var1 * var2;
    int32_t var4 = (((int32_t)par_h6 << 7) + ((temp_scaled * (int32_t)par_h7) / ((int32_t)100))) >> 4;
    int32_t var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
    int32_t var6 = (var4 * var5) >> 1;
    int32_t hum_comp = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;

    if (res){
        printf("test");
    }

    DEBUG("[bme680] humidity compensated: %lu\n", hum_comp/1000);  
    return 1;  
}

static uint16_t _calc_press(const bme680_t *dev, uint32_t* t_fine, uint16_t* res){
    DEBUG("[bme680] start calc press\n");
    uint32_t press_adc = 0;
    uint16_t par_p1, par_p2, par_p4, par_p5, par_p8, par_p9 = 0;
    uint8_t par_p3, par_p6, par_p7, par_p10 = 0;
   if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P1, &par_p1, 2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P1 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P2, &par_p2, 2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P2 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P3, &par_p3, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P3 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P4, &par_p4, 2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P4 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P5, &par_p5, 2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P5 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P6, &par_p6, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P6 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P7, &par_p7, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P7 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P8, &par_p8, 2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P8 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P9, &par_p9, 2, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P9 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_PAR_P10, &par_p10, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PAR_P10 register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BME680_REGISTER_PRESS_ADC, &press_adc, 3, 0) < 0){
        DEBUG("[bme680] error reading BME680_REGISTER_PRESS_ADC register\n");
         /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C_READ;
    }

    press_adc = press_adc & 0xFFFFF0;

    uint32_t var1 = ((int32_t) *t_fine >> 1) - 64000;
    uint32_t var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)par_p6) >> 2;
    var2 = var2 + ((var1 * (int32_t)par_p5) << 1);
    var2 = (var2 >> 2) + ((int32_t)par_p4 << 16);
    var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) * ((int32_t)par_p3 << 5)) >> 3) + (((int32_t)par_p2 * var1) >> 1);
    var1 = var1 >> 18;
    var1 = ((32768 + var1) * (int32_t)par_p1) >> 15;
    uint32_t press_comp = 1048576 - press_adc;
    press_comp = (uint32_t)((press_comp - (var2 >> 12)) * ((uint32_t)3125));
    if (press_comp >= (1 << 30)){
        press_comp = ((press_comp / (uint32_t)var1) << 1);
    }
    else{
        press_comp = ((press_comp << 1) / (uint32_t)var1);
    }
    var1 = ((int32_t)par_p9 * (int32_t)(((press_comp >> 3) *
    (press_comp >> 3)) >> 13)) >> 12;
    var2 = ((int32_t)(press_comp >> 2) * (int32_t)par_p8) >> 13;
    uint32_t var3 = ((int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) * (int32_t)par_p10) >> 17;
    press_comp = (int32_t)(press_comp) + ((var1 + var2 + var3 + ((int32_t)par_p7 << 7)) >> 4);

    if (res){
        printf("test");
    }

    DEBUG("[bme680] pressure compensated: %lu\n", press_comp);  
    return 1;    
}


//static uint16_t _calc_gas(const bme680_t *dev, uint32_t* t_fine, uint16_t* res){
