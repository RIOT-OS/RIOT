/*
 * Copyright (C) 2015 Eistec AB
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lis3dh
 * @{
 *
 * @file
 * @brief       Implementation of LIS3DH SPI driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stddef.h>
#include <stdint.h>
#include "periph/gpio.h"
#include "periph/spi.h"
#include "lis3dh.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SPI_MODE            SPI_MODE_3

#define DEV_SPI        (dev->params.spi)
#define DEV_CS         (dev->params.cs)
#define DEV_CLK        (dev->params.clk)
#define DEV_SCALE      (dev->params.scale)

static inline int lis3dh_write_bits(const lis3dh_t *dev, const uint8_t reg,
                                    const uint8_t mask,  const uint8_t values);
static int lis3dh_write_reg(const lis3dh_t *dev, const uint8_t reg,
                            const uint8_t value);
static int lis3dh_read_regs(const lis3dh_t *dev, const uint8_t reg,
                            const uint8_t len, uint8_t *buf);

int lis3dh_init(lis3dh_t *dev, const lis3dh_params_t *params)
{
    dev->params = *params;

    uint8_t test;

    /* initialize the chip select line */
    if (spi_init_cs(DEV_SPI, DEV_CS) != SPI_OK) {
        DEBUG("[lis3dh] error while initializing CS pin\n");
        return -1;
    }

    /* test connection to the device */
    lis3dh_read_regs(dev, LIS3DH_REG_WHO_AM_I, 1, &test);
    if (test != LIS3DH_WHO_AM_I_RESPONSE) {
        /* chip is not responding correctly */
        DEBUG("[lis3dh] error reading the who am i reg [0x%02x]\n", (int)test);
        return -1;
    }

    /* Clear all settings */
    lis3dh_write_reg(dev, LIS3DH_REG_CTRL_REG1, LIS3DH_CTRL_REG1_XYZEN_MASK);
    /* Disable HP filter */
    lis3dh_write_reg(dev, LIS3DH_REG_CTRL_REG2, 0);
    /* Disable INT1 interrupt sources */
    lis3dh_write_reg(dev, LIS3DH_REG_CTRL_REG3, 0);
    /* Set block data update and little endian, set Normal mode (LP=0, HR=1) */
    lis3dh_write_reg(dev, LIS3DH_REG_CTRL_REG4,
                     (LIS3DH_CTRL_REG4_BDU_ENABLE |
                      LIS3DH_CTRL_REG4_BLE_LITTLE_ENDIAN |
                      LIS3DH_CTRL_REG4_HR_MASK));
    /* Disable FIFO */
    lis3dh_write_reg(dev, LIS3DH_REG_CTRL_REG5, 0);
    /* Reset INT2 settings */
    lis3dh_write_reg(dev, LIS3DH_REG_CTRL_REG6, 0);

    /* Configure scale */
    lis3dh_set_scale(dev, DEV_SCALE);

    return 0;
}

int lis3dh_read_xyz(const lis3dh_t *dev, lis3dh_data_t *acc_data)
{
    uint8_t i;
    /* Set READ MULTIPLE mode */
    static const uint8_t addr = (LIS3DH_REG_OUT_X_L | LIS3DH_SPI_READ_MASK |
                                 LIS3DH_SPI_MULTI_MASK);

    /* Acquire exclusive access to the bus. */
    spi_acquire(DEV_SPI, DEV_CS, SPI_MODE, DEV_CLK);
    /* Perform the transaction */
    spi_transfer_regs(DEV_SPI, DEV_CS, addr,
                      NULL, acc_data, sizeof(lis3dh_data_t));
    /* Release the bus for other threads. */
    spi_release(DEV_SPI);

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
    return lis3dh_read_regs(dev, LIS3DH_REG_OUT_AUX_ADC1_L,
                            LIS3DH_ADC_DATA_SIZE, (uint8_t *)out);
}

int lis3dh_read_aux_adc2(const lis3dh_t *dev, int16_t *out)
{
    return lis3dh_read_regs(dev, LIS3DH_REG_OUT_AUX_ADC2_L,
                            LIS3DH_ADC_DATA_SIZE, (uint8_t *)out);
}

int lis3dh_read_aux_adc3(const lis3dh_t *dev, int16_t *out)
{
    return lis3dh_read_regs(dev, LIS3DH_REG_OUT_AUX_ADC3_L,
                            LIS3DH_ADC_DATA_SIZE, (uint8_t *)out);
}

int lis3dh_set_aux_adc(const lis3dh_t *dev, const uint8_t enable,
                       const uint8_t temperature)
{
    return lis3dh_write_bits(dev, LIS3DH_REG_TEMP_CFG_REG,
                             LIS3DH_TEMP_CFG_REG_ADC_PD_MASK,
                             (enable ? LIS3DH_TEMP_CFG_REG_ADC_PD_MASK : 0) |
                             (temperature ? LIS3DH_TEMP_CFG_REG_TEMP_EN_MASK : 0));
}

int lis3dh_set_axes(const lis3dh_t *dev, const uint8_t axes)
{
    return lis3dh_write_bits(dev, LIS3DH_REG_CTRL_REG1,
                             LIS3DH_CTRL_REG1_XYZEN_MASK, axes);
}

int lis3dh_set_fifo(const lis3dh_t *dev, const uint8_t mode, const uint8_t watermark)
{
    int status;
    uint8_t reg;
    reg = (watermark << LIS3DH_FIFO_CTRL_REG_FTH_SHIFT)
            & LIS3DH_FIFO_CTRL_REG_FTH_MASK;
    reg |= mode;
    status = lis3dh_write_reg(dev, LIS3DH_REG_FIFO_CTRL_REG, reg);
    if (status < 0) {
        /* communication error */
        return status;
    }
    if (mode != 0x00) {
        status = lis3dh_write_bits(dev, LIS3DH_REG_CTRL_REG5,
            LIS3DH_CTRL_REG5_FIFO_EN_MASK, LIS3DH_CTRL_REG5_FIFO_EN_MASK);
    } else {
        status = lis3dh_write_bits(dev, LIS3DH_REG_CTRL_REG5,
            LIS3DH_CTRL_REG5_FIFO_EN_MASK, 0);
    }
    return status;
}

int lis3dh_set_odr(const lis3dh_t *dev, const uint8_t odr)
{
    return lis3dh_write_bits(dev, LIS3DH_REG_CTRL_REG1,
        LIS3DH_CTRL_REG1_ODR_MASK, odr);
}

int lis3dh_set_scale(lis3dh_t *dev, const uint8_t scale)
{
    uint8_t scale_reg;
    /* Sensor full range is -32768 -- +32767 (measurements are left adjusted) */
    /*  => Scale factor is scale/32768 */
    switch (scale)
    {
        case 2:
            dev->scale = 2000;
            scale_reg = LIS3DH_CTRL_REG4_SCALE_2G;
            break;
        case 4:
            dev->scale = 4000;
            scale_reg = LIS3DH_CTRL_REG4_SCALE_4G;
            break;
        case 8:
            dev->scale = 8000;
            scale_reg = LIS3DH_CTRL_REG4_SCALE_8G;
            break;
        case 16:
            dev->scale = 16000;
            scale_reg = LIS3DH_CTRL_REG4_SCALE_16G;
            break;
        default:
            return -1;
    }
    return lis3dh_write_bits(dev, LIS3DH_REG_CTRL_REG4,
                             LIS3DH_CTRL_REG4_FS_MASK, scale_reg);
}

int lis3dh_set_int1(const lis3dh_t *dev, const uint8_t mode)
{
    return lis3dh_write_reg(dev, LIS3DH_REG_CTRL_REG3, mode);
}

int lis3dh_get_fifo_level(const lis3dh_t *dev)
{
    uint8_t reg;
    int level;

    if (lis3dh_read_regs(dev, LIS3DH_REG_FIFO_SRC_REG, 1, &reg) != 0) {
        return -1;
    }
    level = (reg & LIS3DH_FIFO_SRC_REG_FSS_MASK) >> LIS3DH_FIFO_SRC_REG_FSS_SHIFT;
    return level;
}


/**
 * @brief Read sequential registers from the LIS3DH.
 *
 * @param[in]  dev          Device descriptor
 * @param[in]  reg          The source register starting address
 * @param[in]  len          Number of bytes to read
 * @param[out] buf          The values of the source registers will be written
 *                          here
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
static int lis3dh_read_regs(const lis3dh_t *dev, const uint8_t reg,
                            const uint8_t len, uint8_t *buf)
{
    /* Set READ MULTIPLE mode */
    uint8_t addr = (reg & LIS3DH_SPI_ADDRESS_MASK) | LIS3DH_SPI_READ_MASK |
                    LIS3DH_SPI_MULTI_MASK;

    /* Acquire exclusive access to the bus. */
    spi_acquire(DEV_SPI, DEV_CS, SPI_MODE, DEV_CLK);
    /* Perform the transaction */
    spi_transfer_regs(DEV_SPI, DEV_CS, addr, NULL, buf, (size_t)len);
    /* Release the bus for other threads. */
    spi_release(DEV_SPI);

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
static int lis3dh_write_reg(const lis3dh_t *dev, const uint8_t reg,
                            const uint8_t value)
{
    /* Set WRITE SINGLE mode */
    uint8_t addr = ((reg & LIS3DH_SPI_ADDRESS_MASK) | LIS3DH_SPI_WRITE_MASK |
                    LIS3DH_SPI_SINGLE_MASK);

    /* Acquire exclusive access to the bus. */
    spi_acquire(DEV_SPI, DEV_CS, SPI_MODE, DEV_CLK);
    /* Perform the transaction */
    spi_transfer_reg(DEV_SPI, DEV_CS, addr, value);
    /* Release the bus for other threads. */
    spi_release(DEV_SPI);

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
static inline int lis3dh_write_bits(const lis3dh_t *dev, const uint8_t reg,
                                    const uint8_t mask, const uint8_t values)
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
