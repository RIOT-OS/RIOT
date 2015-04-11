/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stddef.h>
#include <stdint.h>
#include "periph/gpio.h"
#include "periph/spi.h"
#include "lis3dh.h"

/**
 * @ingroup     drivers_lis3dh
 * @{
 *
 * @file
 * @brief  Implementation of LIS3DH SPI driver
 *
 * @author Joakim Gebart <joakim.gebart@eistec.se>
 */


static inline int lis3dh_write_bits(const lis3dh_t *dev, const lis3dh_reg_t reg, const uint8_t mask,
                                    const uint8_t values);
static int lis3dh_write_reg(const lis3dh_t *dev, const lis3dh_reg_t reg, const uint8_t value);
static int lis3dh_read_regs(const lis3dh_t *dev, const lis3dh_reg_t reg, const uint8_t len,
                            uint8_t *buf);


int lis3dh_init(lis3dh_t *dev, spi_t spi, gpio_t cs_pin, gpio_t int1_pin, gpio_t int2_pin, lis3dh_scale_t scale)
{
    uint8_t in;

    dev->spi = spi;
    dev->cs = cs_pin;
    dev->int1 = int1_pin;
    dev->int2 = int2_pin;
    dev->scale = 0;

    /* CS */
    gpio_init_out(dev->cs, GPIO_NOPULL);
    gpio_set(dev->cs);

    if (lis3dh_read_regs(dev, LIS3DH_REG_WHO_AM_I, 1, &in) < 0) {
        /* Communication error */
        return -1;
    }

    if (in != LIS3DH_WHO_AM_I_RESPONSE) {
        /* Chip is not responding correctly */
        return -1;
    }

    /* Set block data update and little endian mode. */
    lis3dh_write_reg(dev, LIS3DH_REG_CTRL_REG4,
                     (LIS3DH_CTRL_REG4_BDU_ENABLE |
                      LIS3DH_CTRL_REG4_BLE_LITTLE_ENDIAN));
    lis3dh_set_scale(dev, scale);
    return 0;
}

int lis3dh_read_xyz(const lis3dh_t *dev, lis3dh_data_t *acc_data)
{
    uint8_t i;
    /* Set READ MULTIPLE mode */
    static const uint8_t addr = (LIS3DH_REG_OUT_X_L | LIS3DH_SPI_READ_MASK | LIS3DH_SPI_MULTI_MASK);

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Perform the transaction */
    gpio_clear(dev->cs);

    if (spi_transfer_regs(dev->spi, addr, NULL, (char *)acc_data,
                          sizeof(lis3dh_data_t)) != sizeof(lis3dh_data_t)) {
        /* Transfer error */
        gpio_set(dev->cs);
        /* Release the bus for other threads. */
        spi_release(dev->spi);
        return -1;
    }

    gpio_set(dev->cs);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    /* Scale to milli-G */
    for (i = 0; i < 3; ++i) {
        int32_t tmp = (int32_t)(((int16_t *)acc_data)[i]);
        tmp *= dev->scale;
        tmp /= 32768;
        (((int16_t *)acc_data)[i]) = (int16_t)tmp;
    }

    return 0;
}

int lis3dh_read_aux_adc1(const lis3dh_t *dev, int16_t *out)
{
    return lis3dh_read_regs(dev, LIS3DH_REG_OUT_AUX_ADC1_L, LIS3DH_ADC_DATA_SIZE, (uint8_t *)out);
}

int lis3dh_read_aux_adc2(const lis3dh_t *dev, int16_t *out)
{
    return lis3dh_read_regs(dev, LIS3DH_REG_OUT_AUX_ADC2_L, LIS3DH_ADC_DATA_SIZE, (uint8_t *)out);
}

int lis3dh_read_aux_adc3(const lis3dh_t *dev, int16_t *out)
{
    return lis3dh_read_regs(dev, LIS3DH_REG_OUT_AUX_ADC3_L, LIS3DH_ADC_DATA_SIZE, (uint8_t *)out);
}

int lis3dh_set_aux_adc(lis3dh_t *dev, const uint8_t enable, const uint8_t temperature)
{
    return lis3dh_write_bits(dev, LIS3DH_REG_TEMP_CFG_REG, LIS3DH_TEMP_CFG_REG_ADC_PD_MASK,
                             (enable ? LIS3DH_TEMP_CFG_REG_ADC_PD_MASK : 0) |
                             (temperature ? LIS3DH_TEMP_CFG_REG_TEMP_EN_MASK : 0));
}

int lis3dh_set_axes(lis3dh_t *dev, const uint8_t axes)
{
    return lis3dh_write_bits(dev, LIS3DH_REG_CTRL_REG1, LIS3DH_CTRL_REG1_XYZEN_MASK, axes);
}

int lis3dh_set_fifo_mode(lis3dh_t *dev, const lis3dh_fifo_mode_t mode)
{
    return lis3dh_write_bits(dev, LIS3DH_REG_FIFO_CTRL_REG, LIS3DH_FIFO_CTRL_REG_FM_MASK,
                             mode);
}

int lis3dh_set_fifo(lis3dh_t *dev, const uint8_t enable)
{
    return lis3dh_write_bits(dev, LIS3DH_REG_CTRL_REG5, LIS3DH_CTRL_REG5_FIFO_EN_MASK,
                             (enable ? LIS3DH_CTRL_REG5_FIFO_EN_MASK : 0));
}

int lis3dh_set_odr(lis3dh_t *dev, const lis3dh_odr_t odr)
{
    return lis3dh_write_bits(dev, LIS3DH_REG_CTRL_REG1, LIS3DH_CTRL_REG1_ODR_MASK,
                             odr);
}

int lis3dh_set_scale(lis3dh_t *dev, const lis3dh_scale_t scale)
{
    /* Sensor full range is -32768 -- +32767 */
    /*  => Scale factor is scale/32768 */
    switch (scale)
    {
        case LIS3DH_SCALE_2G:
            dev->scale = 2000;
            break;
        case LIS3DH_SCALE_4G:
            dev->scale = 4000;
            break;
        case LIS3DH_SCALE_8G:
            dev->scale = 8000;
            break;
        case LIS3DH_SCALE_16G:
            dev->scale = 16000;
            break;
        default:
            return -1;
    }
    return lis3dh_write_bits(dev, LIS3DH_REG_CTRL_REG4, LIS3DH_CTRL_REG4_FS_MASK,
                             scale);
}


/**
 * @brief Read sequential registers from the LIS3DH.
 *
 * @param[in]  dev          Device descriptor
 * @param[in]  reg          The source register starting address
 * @param[in]  len          Number of bytes to read
 * @param[out] buf          The values of the source registers will be written here
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
static int lis3dh_read_regs(const lis3dh_t *dev, const lis3dh_reg_t reg, const uint8_t len,
                            uint8_t *buf)
{
    /* Set READ MULTIPLE mode */
    uint8_t addr = (reg & LIS3DH_SPI_ADDRESS_MASK) | LIS3DH_SPI_READ_MASK | LIS3DH_SPI_MULTI_MASK;

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Perform the transaction */
    gpio_clear(dev->cs);

    if (spi_transfer_regs(dev->spi, addr, NULL, (char *)buf, len) < 0) {
        /* Transfer error */
        gpio_set(dev->cs);
        /* Release the bus for other threads. */
        spi_release(dev->spi);
        return -1;
    }

    gpio_set(dev->cs);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    return 0;
}

/**
 * @brief Write a value to an 8 bit register in the LIS3DH.
 *
 * @param[in]  reg          The target register.
 * @param[in]  value        The value to write.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
static int lis3dh_write_reg(const lis3dh_t *dev, const lis3dh_reg_t reg, const uint8_t value)
{
    /* Set WRITE SINGLE mode */
    uint8_t addr = (reg & LIS3DH_SPI_ADDRESS_MASK) | LIS3DH_SPI_WRITE_MASK | LIS3DH_SPI_SINGLE_MASK;

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Perform the transaction */
    gpio_clear(dev->cs);

    if (spi_transfer_reg(dev->spi, addr, value, NULL) < 0) {
        /* Transfer error */
        gpio_set(dev->cs);
        /* Release the bus for other threads. */
        spi_release(dev->spi);
        return -1;
    }

    gpio_set(dev->cs);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
    return 0;
}

/**
 * @brief Write (both set and clear) bits of an 8-bit register on the LIS3DH.
 *
 * @param[in]  addr         Register address on the LIS3DH.
 * @param[in]  mask         Bitmask for the bits to modify.
 * @param[in]  values       The values to write to the masked bits.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
static inline int
lis3dh_write_bits(const lis3dh_t *dev, const lis3dh_reg_t reg, const uint8_t mask,
                  const uint8_t values)
{
    uint8_t tmp;

    if (lis3dh_read_regs(dev, reg, 1, &tmp) < 0) {
        /* Communication error */
        return -1;
    }

    tmp &= ~mask;
    tmp |= (values & mask);

    if (lis3dh_write_reg(dev, reg, tmp) < 0) {
        /* Communication error */
        return -1;
    }

    return 0;
}

/** @} */
