/*
 * Copyright (C) 2025 David Picard
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_max31865
 * @{
 *
 * @file
 * @brief       Device driver implementation for the drivers_sensors
 *
 * @author      David Picard
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "ztimer.h"

#include "byteorder.h"
#include "log.h"

#include "max31865.h"
#include "max31865_internal.h"
#include "max31865_params.h"

/*
 * Single data byte read transfer:
 *
 * CS   |\_____________________/|
 * SCK  | 8 periods | 8 periods |
 * MOSI | addr byte |   idle    |
 * MISO |   idle    | data byte |
 */

/* ****************************************************************************
 * PRIVATE FUNCTIONS
 * ****************************************************************************/

/**
 * @brief Convert raw data to temperature
 * @param[in] dev Device descriptor of the driver
 * @param[in] raw_data Code used to set the RTD low and RTD high registers
 * @param[out] temp Temperature in Celsius centi-degrees (0.01°C)
 */
int _raw_to_temperature(max31865_t *dev, uint16_t raw_data, int32_t *temperature)
{
    assert(dev->params->lut);
    assert(temperature);

    if (raw_data < (*dev->params->lut)[0][MAX31865_LUTCOL_CODE]) {
        LOG_ERROR("%s() >> ERROR: raw_data too small 0x%04X < 0x%04X\n", __FUNCTION__, raw_data,
                  (uint16_t)(*dev->params->lut)[0][MAX31865_LUTCOL_CODE]);
        return -EINVAL;
    }
    if (raw_data > (*dev->params->lut)[dev->params->lut_numlines - 1][MAX31865_LUTCOL_CODE]) {
        LOG_ERROR("%s() >> ERROR: raw_data too big 0x%04X > 0x%04X\n", __FUNCTION__, raw_data,
                  (uint16_t)(*dev->params->lut)[dev->params->lut_numlines -
                                                1][MAX31865_LUTCOL_CODE]);
        return -EINVAL;
    }
    /* walk the LUT to find the appropriate coefficients for linear interpolation: */
    int i = 0;
    for (i = 0 ; i < dev->params->lut_numlines ; i++) {
        if (raw_data < (*dev->params->lut)[i][MAX31865_LUTCOL_CODE]) {
            break;
        }
    }
    i--;
    LOG_DEBUG("%s() >> i = %d\n", __FUNCTION__, i);
    LOG_DEBUG("%s() >> a0 = %ld, a1 = %ld, raw_data = 0x%04X\n", __FUNCTION__,
              (*dev->params->lut)[i][MAX31865_LUTCOL_A0],
              (*dev->params->lut)[i][MAX31865_LUTCOL_A1],
              (uint16_t)(*dev->params->lut)[i][MAX31865_LUTCOL_CODE]);
    /* calculate T in µ°C by linear interpolation with the coefficients from the LUT: */
    int32_t temp_uc = (*dev->params->lut)[i][MAX31865_LUTCOL_A0]
                      + (*dev->params->lut)[i][MAX31865_LUTCOL_A1] * raw_data;
    /* convert µ°C to c°C: */
    int32_t temp_cc = temp_uc / 10000;
    LOG_DEBUG("%s() >> T (°µC) = %ld\n", __FUNCTION__, temp_uc);
    LOG_DEBUG("%s() >> T (°C) = %ld.%02d\n", __FUNCTION__, temp_cc / 100, abs(temp_cc) % 100);

    /* convert to centi degC */
    *temperature = temp_uc / 10000;

    return 0;
}

/**
 * @brief Convert temperature to raw data
 * @param[in] dev Device descriptor of the driver
 * @param[in] temp Temperature in Celsius centi-degrees (0.01°C)
 * @param[out] raw_data Code used to set the RTD low and RTD high registers
 * @return 0 on success.
 * @return -EINVAL if temperature is out of LUT range.
 */
int _temp_to_raw(max31865_t *dev, int32_t temp, uint16_t *raw_data)
{
    int32_t temp_uc = temp * 10000;     // c°C --> µ°C

    assert(raw_data);
    assert(dev->params->lut_numlines);
    if ((temp_uc < (*dev->params->lut)[0][MAX31865_LUTCOL_TEMP])
        || (temp_uc > (*dev->params->lut)[dev->params->lut_numlines - 1][MAX31865_LUTCOL_TEMP])) {
        return -EINVAL;
    }

    int i = 0;
    for (i = 0 ; i < dev->params->lut_numlines ; i++) {
        if (temp_uc < (*dev->params->lut)[i][MAX31865_LUTCOL_TEMP]) {
            break;
        }
    }
    i--;
    *raw_data = (temp_uc - (*dev->params->lut)[i][MAX31865_LUTCOL_A0]) /
                (*dev->params->lut)[i][MAX31865_LUTCOL_A1];

    LOG_DEBUG("%s() >> i = %d, T = %lu c°C, ADC code = 0x%04u\n", __FUNCTION__, i, temp, *raw_data);

    return 0;
}

/* ****************************************************************************
 * PUBLIC FUNCTIONS
 * ****************************************************************************/

int max31865_init(max31865_t *dev, const max31865_params_t *params)
{
    assert(dev);
    assert(params);

    uint8_t data_tx[2] = { 0 };     /* data sent by MCU to device on the MOSI line */
    uint16_t th_code;               /* high/low threshold ADC code */

    dev->params = params;

    int ret = spi_init_cs(dev->params->spi, dev->params->cs_pin);
    if (ret < 0) {
        LOG_ERROR("Failed to initialize MAX31865\n");
        return ret;
    }

    /* The MAX31865 supports SPI modes 1 and 3 (datasheet p16, section "Serial interface") */
    spi_acquire(dev->params->spi, dev->params->cs_pin, SPI_MODE_1, SPI_CLK_5MHZ);

    /* write the configuration register: */
    spi_transfer_reg(dev->params->spi, dev->params->cs_pin, MAX31865_ADDR_CFG_W, params->cfg_byte);

    /* write the high threshold register; LSB's address is MSB's address + 1 */
    if (_temp_to_raw(dev, dev->params->temp_high_threshold, &th_code) == 0) {
        data_tx[0] = th_code >> 8;          /* MSB */
        data_tx[1] = th_code & 0x00FF;      /* LSB */
        spi_transfer_regs(dev->params->spi, dev->params->cs_pin, MAX31865_ADDR_RTD_HTHRES_MSB_W,
                          data_tx, NULL, 2);
    }

    /* write the low threshold register; LSB's address is MSB's address + 1 */
    if (_temp_to_raw(dev, dev->params->temp_low_threshold, &th_code) == 0) {
        data_tx[0] = th_code >> 8;          /* MSB */
        data_tx[1] = th_code & 0x00FF;      /* LSB */
        spi_transfer_regs(dev->params->spi, dev->params->cs_pin, MAX31865_ADDR_RTD_LTHRES_MSB_W,
                          data_tx, NULL, 2);
    }

    spi_release(dev->params->spi);

    return 0;
}

void max31865_clear_fault(max31865_t *dev, const max31865_params_t *params, uint8_t *config)
{
    assert(dev);
    assert(params);
    // assert(config);

    uint8_t cfg_byte = 0;
    uint8_t clr_byte = 0;

    spi_acquire(dev->params->spi, dev->params->cs_pin, SPI_MODE_1, SPI_CLK_5MHZ);
    cfg_byte = spi_transfer_reg(dev->params->spi, dev->params->cs_pin, MAX31865_ADDR_CFG_R, 0);
    if (config) {
        *config = cfg_byte;
    }
    clr_byte = cfg_byte | MAX31865_CFG_CLEAR_FAULT;
    spi_transfer_reg(dev->params->spi, dev->params->cs_pin, MAX31865_ADDR_CFG_W, clr_byte);
    spi_release(dev->params->spi);
}

int max31865_read_raw(max31865_t *dev, uint16_t *raw_data)
{
    assert(dev);
    assert(raw_data);

    uint8_t data_tx[3] = { 0 };     /* data sent by MCU to device on the MOSI line */
    uint8_t data_rx[3] = { 0 };     /* data received by MCU from device on the MISO line */

    data_tx[0] = MAX31865_ADDR_RTD_MSB;
    spi_acquire(dev->params->spi, dev->params->cs_pin, SPI_MODE_1, SPI_CLK_5MHZ);
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data_tx, data_rx,
                       sizeof(data_tx));
    spi_release(dev->params->spi);

    *raw_data = byteorder_bebuftohs(data_rx + 1);

    /* test error bit b0: */
    if (*raw_data & 0x0001) {
        return -EIO;
    }

    return 0;
}

void max31865_raw_to_data(max31865_t *dev, uint32_t raw_data, max31865_data_t *data)
{
    assert(data);

    _raw_to_temperature(dev, raw_data, &data->rtd_temperature_cdegc);
    data->fault = MAX31865_FAULT_NO_FAULT;
}

int max31865_read(max31865_t *dev, max31865_data_t *data)
{
    assert(dev && data);

    uint16_t raw_data;
    if (max31865_read_raw(dev, &raw_data) == -EIO) {
        max31865_detect_fault(dev, &data->fault);
        return -EIO;
    }
    else {
        max31865_raw_to_data(dev, raw_data, data);
    }

    return 0;
}

int max31865_detect_fault(max31865_t *dev, max31865_fault_t *flt_code)
{
    assert(dev);
    assert(flt_code);

    uint8_t data_tx[2] = { 0 };     /* data sent by MCU to device on the MOSI line */
    uint8_t data_rx[2] = { 0 };     /* data received by MCU from device on the MISO line */
    uint8_t cfg;                    /* value of the config register */
    uint8_t fault;                  /* value of the fault register */

    spi_acquire(dev->params->spi, dev->params->cs_pin, SPI_MODE_1, SPI_CLK_5MHZ);

    /* read current config: */
    data_tx[0] = MAX31865_ADDR_CFG_R;
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data_tx, data_rx,
                       sizeof(data_tx));

    /* start automatic fault detection: */
    cfg = data_rx[1] | MAX31865_CFG_FLTDET_AUTO_START;
    data_tx[0] = MAX31865_ADDR_CFG_W;
    data_tx[1] = cfg;
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data_tx, NULL,
                       sizeof(data_tx));

    /* wait for completion and check actual completion; datasheet states 100µs: */
    ztimer_sleep(ZTIMER_USEC, 200);
    data_tx[0] = MAX31865_ADDR_CFG_R;
    data_tx[1] = 0;
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data_tx, data_rx,
                       sizeof(data_tx));
    cfg = data_rx[1];
    if ((cfg & MAX31865_CFG_FLTDET_MASK) != MAX31865_CFG_FLTDET_IDLE) {
        spi_release(dev->params->spi);
        return -EIO;
    }

    /* read fault code: */
    data_tx[0] = MAX31865_ADDR_FAULT;
    data_tx[1] = 0;
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data_tx, data_rx,
                       sizeof(data_tx));
    fault = data_rx[1];

    spi_release(dev->params->spi);

    if (fault & (MAX31865_FLT_REF_FC | MAX31865_FLT_REF_FO | MAX31865_FLT_RTD_FO)) {
        *flt_code = MAX31865_FAULT_CIRCUIT;
    }
    else if (fault & MAX31865_FLT_THRESHIGH) {
        *flt_code = MAX31865_FAULT_RTD_HIGH;
    }
    else if (fault & MAX31865_FLT_THRESLOW) {
        *flt_code = MAX31865_FAULT_RTD_LOW;
    }
    else if (fault & MAX31865_FLT_VOLTAGE) {
        *flt_code = MAX31865_FAULT_VOLTAGE;
    }
    else {
        *flt_code = MAX31865_FAULT_NO_FAULT;
    }

    return 0;
}

void max31865_switch_vbias(max31865_t *dev, bool enable)
{
    assert(dev);

    uint8_t data_tx[2] = { 0 };     /* data sent by MCU to device on the MOSI line */
    uint8_t data_rx[2] = { 0 };     /* data received by MCU from device on the MISO line */
    uint8_t cfg;                    /* value of the config register */

    spi_acquire(dev->params->spi, dev->params->cs_pin, SPI_MODE_1, SPI_CLK_5MHZ);

    /* read current config: */
    data_tx[0] = MAX31865_ADDR_CFG_R;
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data_tx, data_rx,
                       sizeof(data_tx));
    cfg = data_rx[1];

    /* switch Vbias on or off: */
    if (enable) {
        cfg = cfg | MAX31865_CFG_VBIAS_ON;      /* switch on */
    }
    else {
        cfg = cfg & ~MAX31865_CFG_VBIAS_ON;     /* switch off */
    }
    data_tx[0] = MAX31865_ADDR_CFG_W;
    data_tx[1] = cfg;
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data_tx, NULL,
                       sizeof(data_tx));

    spi_release(dev->params->spi);
}

void max31865_oneshot(max31865_t *dev)
{
    assert(dev);

    uint8_t data_tx[2] = { 0 };     /* data sent by MCU to device on the MOSI line */
    uint8_t data_rx[2] = { 0 };     /* data received by MCU from device on the MISO line */
    uint8_t cfg;                    /* value of the config register */

    spi_acquire(dev->params->spi, dev->params->cs_pin, SPI_MODE_1, SPI_CLK_5MHZ);

    /* read current config: */
    data_tx[0] = MAX31865_ADDR_CFG_R;
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data_tx, data_rx,
                       sizeof(data_tx));
    cfg = data_rx[1];

    /* switch Vbias on or off: */
    cfg = cfg | MAX31865_CFG_1SHOT;
    data_tx[0] = MAX31865_ADDR_CFG_W;
    data_tx[1] = cfg;
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data_tx, NULL,
                       sizeof(data_tx));

    spi_release(dev->params->spi);
}
