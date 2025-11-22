/*
 * SPDX-FileCopyrightText: 2024 CNRS, France
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_abp2
 * @{
 *
 * @file
 * @brief       Honeywell ABP2 series pressure and temperature sensor driver
 *
 * @author      David Picard <david.picard@clermont.in2p3.fr>
 * @}
 */

#include <assert.h>
#include "abp2.h"
#include "abp2_params.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "ztimer.h"
#include <errno.h>
#include "debug.h"

#define ENABLE_DEBUG 0
#define MAX_LOOPS_TIMEOUT   (10)            /**< Timeout (ms). */
#define ABP2_CMD_MEAS       (0xAA)          /**< Start a measurement. */
#define ABP2_CMD_NOP        (0xF0)          /**< NOP command. */
#define ABP2_RX_BUF_LEN     (8)             /**< Length of the receive buffer. */

/** Data to send in order to start a measurement. */
static const uint8_t data_tx_meas_start[] = { ABP2_CMD_MEAS, 0x00, 0x00 };

/** Data to send in order to start a measurement and read the data of the previous measurement. */
static const uint8_t data_tx_meas_read[] = { ABP2_CMD_MEAS, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/** Data to send in order to read the data of the previous measurement. */
static const uint8_t data_tx_nop_read[] = { ABP2_CMD_NOP, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const int32_t cntPMax = 15099494;    /**< Output at maximum pressure (counts). */
static const int32_t cntPMin = 1677722;     /**< Output at minimum pressure (counts). */

int abp2_init(abp2_t *dev, const abp2_params_t *params)
{
    uint8_t status = 0xFF;      /* sensor status byte */
    int res = 0;
    int count = 10;

    assert(dev && params);
    dev->params = params;

#if defined(MODULE_ABP2_SPI)
    res = spi_init_cs(params->spi, params->cs);
#else
#pragma message("implement I2C code here")
#endif
    if (res) {
        return -EIO;
    }

    /* test status byte: the busy flag should clear in about 5ms */
    while ((status & ABP2_STATUS_BUSY) && count) {
#if defined(MODULE_ABP2_SPI)
        spi_acquire(params->spi, params->cs, SPI_MODE_0, params->clk);
        status = spi_transfer_byte(params->spi, params->cs, false, ABP2_CMD_NOP);
        spi_release(params->spi);
#else
#pragma message("implement I2C code here")
#endif
        ztimer_sleep(ZTIMER_USEC, 1000);
        count--;
    }
    /* the busy flag should be clear */
    if (status & ABP2_STATUS_BUSY) {
        res = -ETIMEDOUT;
    }

    return res;
}

int abp2_measure(const abp2_t *dev)
{
    uint8_t status = 0xFF;      /* sensor status byte */
    int res = 0;
    uint8_t data_rx[ABP2_RX_BUF_LEN];

#if defined(MODULE_ABP2_SPI)
    spi_acquire(dev->params->spi, dev->params->cs, SPI_MODE_0, dev->params->clk);
    spi_transfer_bytes(dev->params->spi, dev->params->cs, false, data_tx_meas_start, data_rx,
                       sizeof(data_tx_meas_start));
    spi_release(dev->params->spi);
#else
#pragma message("implement I2C code here")
#endif
    status = data_rx[0];
    if (status & ABP2_STATUS_MASK) {
        return -ENODATA;
    }

    return res;
}

int abp2_read(const abp2_t *dev, int32_t *press, int32_t *temp)
{
    int res = 0;
    uint8_t status = 0xFF;      /* sensor status byte */
    int loopsTimeout = MAX_LOOPS_TIMEOUT;
    abp2_raw_t rawData;

    res = abp2_measure(dev);
    if (res) {
        return res;
    }
    /* wait until the busy flag clears or until timeout: */
    while (((status = abp2_getstatus(dev)) & ABP2_STATUS_BUSY) && loopsTimeout) {
        loopsTimeout--;
        ztimer_sleep(ZTIMER_USEC, 1000);    /* wait 1ms */
    }
    if (!loopsTimeout) {
        return -ETIMEDOUT;
    }
    /* check sensor errors: */
    if (status & ABP2_STATUS_MASK) {
        return -ENODATA;
    }
    /* read ADC conversion results: */
    res = abp2_read_raw(dev, &rawData);
    if (res) {
        return res;
    }
    /* convert to physical quantities: */
    *press = abp2_pressure(dev, &rawData);
    *temp = abp2_temperature(dev, &rawData);

    return 0;
}

int abp2_read_nb(const abp2_t *dev, int32_t *press, int32_t *temp)
{
    int res = 0;
    abp2_raw_t rawData;

    /* start an ADC conversion and read previous results: */
    res = abp2_measure_read(dev, &rawData);
    if (res) {
        return res;
    }
    /* convert to physical quantities: */
    *press = abp2_pressure(dev, &rawData);
    *temp = abp2_temperature(dev, &rawData);

    return 0;
}

uint8_t abp2_getstatus(const abp2_t *dev)
{
    uint8_t status = 0xFF;      /* sensor status byte */

#if defined(MODULE_ABP2_SPI)
    spi_acquire(dev->params->spi, dev->params->cs, SPI_MODE_0, dev->params->clk);
    status = spi_transfer_byte(dev->params->spi, dev->params->cs, false, ABP2_CMD_NOP);
    spi_release(dev->params->spi);
#else
#pragma message("implement I2C code here")
#endif

    return status;
}

int abp2_read_raw(const abp2_t *dev, abp2_raw_t *raw_values)
{
    uint8_t status = 0xFF;      /* sensor status byte */
    uint8_t data_rx[ABP2_RX_BUF_LEN];

#if defined(MODULE_ABP2_SPI)
    spi_acquire(dev->params->spi, dev->params->cs, SPI_MODE_0, dev->params->clk);
    spi_transfer_bytes(dev->params->spi, dev->params->cs, false, data_tx_nop_read, data_rx,
                       sizeof(data_tx_nop_read));
    spi_release(dev->params->spi);
#else
#pragma message("implement I2C code here")
#endif
    status = data_rx[0];
    if (status & ABP2_STATUS_MASK) {
        return -ENODATA;
    }
    raw_values->cntPress = (uint32_t)data_rx[1] << 16 | (uint32_t)data_rx[2] << 8 | data_rx[3];
    raw_values->cntTemp = (uint32_t)data_rx[4] << 16 | (uint32_t)data_rx[5] << 8 | data_rx[6];

    return 0;
}

int abp2_measure_read(const abp2_t *dev, abp2_raw_t *raw_values)
{
    uint8_t status = 0xFF;     /* sensor status byte */
    int res = 0;
    uint8_t data_rx[ABP2_RX_BUF_LEN];

#if defined(MODULE_ABP2_SPI)
    spi_acquire(dev->params->spi, dev->params->cs, SPI_MODE_0, dev->params->clk);
    spi_transfer_bytes(dev->params->spi, dev->params->cs, false, data_tx_meas_read, data_rx,
                       sizeof(data_tx_meas_read));
    spi_release(dev->params->spi);
#else
#pragma message("implement I2C code here")
#endif
    status = data_rx[0];
    if (status & ABP2_STATUS_MASK) {
        return -ENODATA;
    }
    raw_values->cntPress = (uint32_t)data_rx[1] << 16 | (uint32_t)data_rx[2] << 8 | data_rx[3];
    raw_values->cntTemp = (uint32_t)data_rx[4] << 16 | (uint32_t)data_rx[5] << 8 | data_rx[6];

    return res;
}

int32_t abp2_pressure(const abp2_t *dev, const abp2_raw_t *raw_values)
{
    int32_t press = 0;

    /* int64_t in intermediate calculation prevents overflow */
    int64_t diffCnt = (raw_values->cntPress - cntPMin);
    int64_t diffRng = (dev->params->rangeMax - dev->params->rangeMin);
    int64_t diffCntRng = cntPMax - cntPMin;
    int64_t numerator = diffCnt * diffRng;

    press = numerator / diffCntRng + dev->params->rangeMin;

    return press;
}

int32_t abp2_temperature(const abp2_t *dev, const abp2_raw_t *raw_values)
{
    (void)(dev); /* keep unused parameter to homogenize function prototypes */
    /* 64-bit constant avoids integer overflow */
    return ((raw_values->cntTemp * 200000LL) >> 24) - 50000;
}
