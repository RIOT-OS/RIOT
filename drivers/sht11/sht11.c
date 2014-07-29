/******************************************************************************
 * Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

/**
 * @defgroup	sht11 SHT11
 * @ingroup		drivers
 * @brief		Driver for the Sensirion SHT11 humidity and temperature sensor
 * @{
 */

/**
 * @file
 * @brief		SHT11 Device Driver
 *
 * @version     $Revision: 2396 $
 *
 * @note		$Id: sht11.c 2396 2010-07-06 15:12:35Z ziegert $
 */

#include <stdio.h>
#include <stdint.h>

#include "hwtimer.h"
#include "mutex.h"
#include "sht11.h"
#include "sht11-board.h"
#include "bitarithm.h"

float sht11_temperature_offset;

/**
 * @brief   Perform measurement
 *
 * @param   p_value Measured value (14 or 12 bit -> 2 bytes)
 * @param   p_checksum Checksum of measurement
 * @param   mode The requestested measurement mode: temperature or humidity
 *
 * @return  1 on success, 0 otherwise
 */
static uint8_t measure(uint8_t *p_value, uint8_t *p_checksum, uint8_t mode);

/**
 * @brief   Write one byte
 *
 * @param   value The value to write
 *
 * @return  1 for acknowledged write, 0 otherwise
 */
static uint8_t write_byte(uint8_t value);

/**
 * @brief   Read ony byte
 *
 * @param   ack Set if the data read should be acknowledged
 *
 * @return  The read byte
 */
static uint8_t read_byte(uint8_t ack);

/**
 * @brief   Communication reset
 */
static void connection_reset(void);

/**
 * @brief   Send start of transmision sequence
 */
static void transmission_start(void);

/**
 * @brief   Toggle the clock line
 */
static inline void clk_signal(void);

/* mutex for exclusive measurement operation */
mutex_t sht11_mutex = MUTEX_INIT;

/*---------------------------------------------------------------------------*/
static inline void clk_signal(void)
{
    SHT11_SCK_HIGH;
    hwtimer_wait(SHT11_CLK_WAIT);
    SHT11_SCK_LOW;
    hwtimer_wait(SHT11_CLK_WAIT);
}

/*---------------------------------------------------------------------------*/
static uint8_t write_byte(uint8_t value)
{
    uint8_t i;
    uint8_t ack;

    SHT11_DATA_OUT;

    /* send value bit by bit to sht11 */
    for (i = 0; i < 8; i++) {
        if (value & BIT7) {
            SHT11_DATA_HIGH;
            hwtimer_wait(SHT11_DATA_WAIT);
        }
        else {
            SHT11_DATA_LOW;
            hwtimer_wait(SHT11_DATA_WAIT);
        }

        /* trigger clock signal */
        clk_signal();

        /* shift value to write next bit */
        value = value << 1;
    }

    /* wait for ack */
    SHT11_DATA_IN;
    hwtimer_wait(SHT11_CLK_WAIT);
    ack = SHT11_DATA;

    clk_signal();

    return ack;
}
/*---------------------------------------------------------------------------*/
static uint8_t read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t value = 0;

    SHT11_DATA_IN;
    hwtimer_wait(SHT11_DATA_WAIT);

    /* read value bit by bit */
    for (i = 0; i < 8; i++) {
        value = value << 1;
        SHT11_SCK_HIGH;
        hwtimer_wait(SHT11_CLK_WAIT);

        if (SHT11_DATA) {
            /* increase data by one when DATA is high */
            value++;
        }

        SHT11_SCK_LOW;
        hwtimer_wait(SHT11_CLK_WAIT);
    }

    /* send ack if necessary */
    SHT11_DATA_OUT;

    if (ack) {
        SHT11_DATA_LOW;
        hwtimer_wait(SHT11_DATA_WAIT);
    }
    else {
        SHT11_DATA_HIGH;
        hwtimer_wait(SHT11_DATA_WAIT);
    }

    clk_signal();

    /* release data line */
    SHT11_DATA_IN;

    return value;
}
/*---------------------------------------------------------------------------*/
static void transmission_start(void)
{
    /*       _____         ________
       DATA:      |_______|
                 ___     ___
       SCK : ___|   |___|   |______
    */
    SHT11_DATA_OUT;

    /* set initial state */
    SHT11_DATA_HIGH;
    hwtimer_wait(SHT11_DATA_WAIT);
    SHT11_SCK_LOW;
    hwtimer_wait(SHT11_CLK_WAIT);

    SHT11_SCK_HIGH;
    hwtimer_wait(SHT11_CLK_WAIT);

    SHT11_DATA_LOW;
    hwtimer_wait(SHT11_DATA_WAIT);

    SHT11_SCK_LOW;
    hwtimer_wait(SHT11_CLK_WAIT);

    SHT11_SCK_HIGH;
    hwtimer_wait(SHT11_CLK_WAIT);

    SHT11_DATA_HIGH;
    hwtimer_wait(SHT11_DATA_WAIT);

    SHT11_SCK_LOW;
    hwtimer_wait(SHT11_CLK_WAIT);
}
/*---------------------------------------------------------------------------*/
static void connection_reset(void)
{
    /*       _____________________________________________________         ____
       DATA:                                                      |_______|
                _    _    _    _    _    _    _    _    _        ___     ___
       SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |__
    */
    uint8_t i;
    SHT11_DATA_HIGH;
    hwtimer_wait(SHT11_DATA_WAIT);
    SHT11_SCK_LOW;
    hwtimer_wait(SHT11_CLK_WAIT);

    for (i = 0; i < 9; i++) {
        clk_signal();
    }

    transmission_start();
}
/*---------------------------------------------------------------------------*/
static uint8_t measure(uint8_t *p_value, uint8_t *p_checksum, uint8_t mode)
{
    uint8_t error = 0;
    uint8_t ack = 1;
    uint16_t i;

    transmission_start();
    error = write_byte(mode);

    hwtimer_wait(HWTIMER_TICKS(1000));

    /* wait untile sensor has finished measurement or timeout */
    for (i = 0; (i < SHT11_MEASURE_TIMEOUT) && (!error); i++) {
        ack = SHT11_DATA;

        if (!ack) {
            break;
        }

        hwtimer_wait(HWTIMER_TICKS(1000));
    }

    error += ack;

    /* read MSB */
    *(p_value + 1) = read_byte(SHT11_ACK);
    /* read LSB */
    *(p_value) = read_byte(SHT11_ACK);
    /* read checksum */
    *p_checksum = read_byte(SHT11_NO_ACK);

    return (!error);
}
/*---------------------------------------------------------------------------*/
void sht11_init(void)
{
    sht11_temperature_offset = 0;
    SHT11_INIT;
    hwtimer_wait(11 * HWTIMER_TICKS(1000));
}
/*---------------------------------------------------------------------------*/
uint8_t sht11_read_status(uint8_t *p_value, uint8_t *p_checksum)
{
    uint8_t error = 0;

    transmission_start();
    error |= write_byte(SHT11_STATUS_REG_R);
    *p_value = read_byte(SHT11_ACK);
    *p_checksum = read_byte(SHT11_NO_ACK);
    return (!error);
}
/*---------------------------------------------------------------------------*/
uint8_t sht11_write_status(uint8_t *p_value)
{
    uint8_t error = 0;

    transmission_start();
    error += write_byte(SHT11_STATUS_REG_W);
    error += write_byte(*p_value);
    return (!error);
}
/*---------------------------------------------------------------------------*/
uint8_t sht11_read_sensor(sht11_val_t *value, sht11_mode_t mode)
{
    uint8_t error = 0;
    uint8_t checksum;
    uint16_t humi_int, temp_int;

    /* Temperature arithmetic where S0(T) is read value
     * T = D1 + D2 * S0(T) */
    const float D1 = -39.6;
    const float D2 = 0.01;

    /* Arithmetic for linear humdity where S0(RH) is read value
     * HL = C1 + C2 * S0(RH) + C3 * SO(RH)^2 */
    const float C1 = -4.0;
    const float C2 = +0.0405;
    const float C3 = -0.0000028;

    /* Arithmetic for temperature compesated relative humdity
     * HT = (T-25) * ( T1 + T2 * SO(RH) ) + HL */
    const float T1 = +0.01;
    const float T2 = +0.00008;

    /* check for valid buffer */
    if (value == NULL) {
        return 0;
    }

    value->temperature = 0;
    value->relhum = 0;
    value->relhum_temp = 0;

    mutex_lock(&sht11_mutex);
    connection_reset();

    /* measure humidity */
    if (mode & HUMIDITY) {
        error += (!measure((uint8_t *) &humi_int, &checksum, SHT11_MEASURE_HUMI));
    }

    /* measure temperature */
    if (mode & TEMPERATURE) {
        error += (!measure((uint8_t *) &temp_int, &checksum, SHT11_MEASURE_TEMP));
    }

    /* break on error */
    if (error != 0) {
        connection_reset();
        mutex_unlock(&sht11_mutex);
        return 0;
    }

    if (mode & TEMPERATURE) {
        value->temperature = D1 + (D2 * ((float) temp_int)) + sht11_temperature_offset;
    }

    if (mode & HUMIDITY) {
        value->relhum = C1 + (C2 * ((float) humi_int)) + (C3 * ((float) humi_int) * ((float) humi_int));

        if (mode & TEMPERATURE) {
            value->relhum_temp = (value->temperature - 25) * (T1 + (T2 * (float) humi_int)) + value->relhum;
        }
    }

    mutex_unlock(&sht11_mutex);
    return 1;
}

/** @} */
