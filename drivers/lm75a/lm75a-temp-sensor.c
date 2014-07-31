/*
 * lm75a-temp-sensor.c - Driver for the LM75A temperature sensor based on the
 * i2c interface.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 *
 */

/**
 * @file
 * @internal
 * @brief       Driver for the LM75A temperature sensor.
 *              The communication between the LM75A and the MCU is
 *              based on the i2c interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @version     $Revision: 3855 $
 *
 * @note        $Id: lm75a-temp-sensor.c 3855 2013-09-05 13:53:49 kasmi $
 */

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "msg.h"
#include "thread.h"
#include "vtimer.h"
#include "timex.h"
#include "lpc2387.h"
#include "gpioint.h"
#include "i2c.h"
#include "lm75a-temp-sensor.h"
#include "hwtimer.h"
#include "board.h"

//declaration as volatile is important, otherwise no interrupt is triggered.
volatile bool my_alarm = false;

static uint16_t get_2s_complement(uint16_t num)
{
    return ~(num) + 1;
}

//Write: MC --> Sensor
static void to_bytes(float_t f, uint8_t *buff)
{
    int32_t i = (int32_t)(f * 2);
    buff[0] = (uint8_t)((i >> LM75A_BIT1) & 0xFF);//Most signif. byte
    buff[1] = (uint8_t)((i & LM75A_BIT1) << LM75A_BIT7);//Least signif. byte
}

static uint16_t to_uint16(uint8_t *buff)
{
    if (buff != NULL) {
        return (buff[0] << LM75A_BIT8) | buff[1];
    }
    else {
        return UINT16_MAX;
    }
}

//Read: Sensor --> MC
volatile static float_t to_float(uint8_t reg_addr, uint16_t reg_value)
{
    uint16_t sign = reg_value & LM75A_SIGN_BIT_MASK;
    float_t f_temp = 0.0;
    float_t factor = 0.0;

    switch (reg_addr) {
        case LM75A_OVER_TEMP_REG:
        case LM75A_THYST_REG:
            factor = 0.5;
            break;

        case LM75A_TEMPERATURE_REG:
            factor = 0.125;
    }

    if (sign) { //the number is negative
        f_temp = (get_2s_complement(reg_value) & LM75A_DATA_BITS_MASK)
                 * -factor;
    }
    else {   //the number is positive
        f_temp = reg_value * factor;
    }
    return f_temp;
}

static void set_register(uint8_t i2c_interface, uint8_t reg_addr, float_t value)
{
    bool status = false;
    uint8_t tx_buff[2];

    switch (reg_addr) {
        case LM75A_OVER_TEMP_REG:
        case LM75A_THYST_REG:
            to_bytes(value, tx_buff);
            status = i2c_write(i2c_interface, LM75A_ADDR, reg_addr, tx_buff, 2);
            break;

        case LM75A_CONFIG_REG:
            tx_buff[0] = (uint8_t) value;
            status = i2c_write(i2c_interface, LM75A_ADDR, reg_addr, tx_buff, 1);
    }

    if (!status) {
        puts(
            "[lm75a_tempSensorI2C/lm75A_setRegister]: Slave is not ready !!\n");
    }
}

void lm75A_set_over_temperature(float_t tos)
{
    set_register(LM75A_I2C_INTERFACE, LM75A_OVER_TEMP_REG, tos);
}

void lm75A_set_hysteresis_temperature(float_t thsyt)
{
    set_register(LM75A_I2C_INTERFACE, LM75A_THYST_REG, thsyt);
}

static uint16_t lm75A_get_register_value(uint8_t i2c_interface,
        uint8_t reg_addr,
        uint8_t reg_size)
{
    bool status = false;
    uint8_t rx_buff[reg_size];

    i2c_clear_buffer(rx_buff, reg_size);
    if ((reg_size > 0) && (reg_size < 3)) {
        status = i2c_read(i2c_interface, LM75A_ADDR, reg_addr, rx_buff,
                          reg_size);
        if (!status) { //Slave is not ready
            puts(
                "[lm75a_tempSensorI2C/lm75A_getConfigReg]: Slave is not\
                ready !\n");

            if (reg_size < 2) {
                return UCHAR_MAX;
            }
            else {
                return UINT16_MAX;
            }
        }
        else {   //Slave acknowledged
            if (reg_size < 2) {
                return rx_buff[0];
            }
            else {
                return to_uint16(rx_buff);
            }
        }
    }
    else {
        puts("the register size must be less than 2");
        return UINT16_MAX;
    }
}

uint8_t lm75A_get_config_reg(void)
{
    return lm75A_get_register_value(LM75A_I2C_INTERFACE, LM75A_CONFIG_REG, 1);
}

float_t lm75A_get_hysteresis_temperature(void)
{
    uint16_t hyst_reg_value = 0;
    hyst_reg_value = lm75A_get_register_value(LM75A_I2C_INTERFACE,
                     LM75A_THYST_REG, 2);
    hyst_reg_value = (hyst_reg_value >> LM75A_BIT7);
    return to_float(LM75A_THYST_REG, hyst_reg_value);
}

float_t lm75A_get_over_temperature(void)
{
    uint16_t over_temp = 0;

    over_temp = lm75A_get_register_value(LM75A_I2C_INTERFACE,
                                         LM75A_OVER_TEMP_REG, 2);
    over_temp = (over_temp >> LM75A_BIT7);
    return to_float(LM75A_OVER_TEMP_REG, over_temp);
}

float_t lm75A_get_ambient_temperature(void)
{
    uint16_t amb_temp = 0;
    amb_temp = lm75A_get_register_value(LM75A_I2C_INTERFACE,
                                        LM75A_TEMPERATURE_REG, 2);
    amb_temp = (amb_temp >> LM75A_BIT5);
    return to_float(LM75A_TEMPERATURE_REG, amb_temp);
}

void lm75A_reset(void)
{
    lm75A_set_over_temperature(LM75A_DEFAULT_TOS);
    lm75A_set_hysteresis_temperature(LM75A_DEFAULT_THYST);
    set_register(LM75A_I2C_INTERFACE, LM75A_CONFIG_REG,
                 LM75A_DEFAULT_CONFIG_VALUE);
}

void lm75A_set_operation_mode(uint8_t op_mode)
{
    uint8_t config_reg = lm75A_get_config_reg();

    switch (op_mode) {
        case LM75A_NORMAL_OPERATION_MODE:
            config_reg &= ~(1 << LM75A_BIT0);
            break;

        case LM75A_SHUTDOWN_MODE:
            config_reg |= (1 << LM75A_BIT0);
            break;

        case LM75A_COMPARATOR_MODE:
            config_reg &= ~(1 << LM75A_BIT1);
            break;

        case LM75A_INTERRUPT_MODE:
            config_reg |= (1 << LM75A_BIT1);
            break;

        default:
            config_reg &= ~(1 << LM75A_BIT0);
    }
    set_register(LM75A_I2C_INTERFACE, LM75A_CONFIG_REG, config_reg);
}

bool lm75A_ext_irq_handler_register(int32_t port, uint32_t pin_bit_mask,
                                    int32_t flags, void *handler)
{
    return gpioint_set(port, pin_bit_mask, flags, handler);
}

bool lm75A_init(uint8_t i2c_interface, uint32_t baud_rate,
                void *external_interr_handler)
{
    if (i2c_initialize(i2c_interface, (uint32_t) I2CMASTER, 0, baud_rate, NULL)
        == false) { /* initialize I2C */
        puts("fatal error happened in i2c_initialize()\n");
        return false;
    }
    //i2c_enable_pull_up_resistor(i2c_interface);
    i2c_disable_pull_up_resistor(i2c_interface);

    if ((external_interr_handler != NULL)
        && lm75A_ext_irq_handler_register(2, BIT3, GPIOINT_FALLING_EDGE,
                                          external_interr_handler)) {
        printf("# %-70s%10s\n", "External interrupt handler registration",
               "...[OK]");
    }
    else {
        printf("# %-70s%10s\n", "External interrupt handler registration",
               "...[FAILED]");
        return false;
    }

    puts("################## Before reset ##################");
    printf("configReg = %u\n", lm75A_get_config_reg());
    printf("hystTemp = %f\n", lm75A_get_hysteresis_temperature());
    printf("overTemp = %f\n", lm75A_get_over_temperature());
    lm75A_reset();

    puts("\n################## After reset ##################");
    printf("configRegInitial = %u\n", lm75A_get_config_reg());
    printf("initialHystTemp = %f\n", lm75A_get_hysteresis_temperature());
    printf("initialOverTemp = %f\n", lm75A_get_over_temperature());

    puts("\n################## New configuration ##################");
    // set the hysteresis temperature
    lm75A_set_hysteresis_temperature(32.0);
    printf("hystTemp = %f\n", lm75A_get_hysteresis_temperature());
    lm75A_set_over_temperature(33.0);
    printf("overTemp = %f\n", lm75A_get_over_temperature());

    puts("\n################## Go to comparator mode ##################");
    lm75A_set_operation_mode(LM75A_COMPARATOR_MODE);
    printf("configReg = %u\n", lm75A_get_config_reg());
    //  puts("\n################## Go to interrupt mode ##################");
    //  lm75A_set_operation_mode(LM75A_INTERRUPT_MODE);
    //  printf("configReg = %u\n", lm75A_get_config_reg());

    return true;
}

void lm75A_set_in_alarm(bool b)
{
    my_alarm = b;
}

/*
 * Application entry point.
 */
void lm75A_start_sensor_sampling(void (*handler)(void))
{
    /*
     * Normal main() thread activity.
     */
    while (true) {
        //Toggle the green LED;
        LED_RED_TOGGLE;
        printf("amb_temp = %3.3f\n", lm75A_get_ambient_temperature());

        if (my_alarm && (handler != NULL)) {
            LED_GREEN_ON;
            handler();
            my_alarm = false;
        }
        hwtimer_wait(HWTIMER_TICKS(100000));
        LED_RED_TOGGLE;
        hwtimer_wait(HWTIMER_TICKS(100000));
    }
}
