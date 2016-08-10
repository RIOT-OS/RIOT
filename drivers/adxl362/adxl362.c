/*
 * Copyright (C) 2015-2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_adxl362
 * @{
 *
 * @file
 * @brief       Device driver implementation for Analog Devices ADXL362
 *              Micropower, 3-Axis, ±2 g/±4 g/±8 g Digital Output MEMS Accelerometer
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <errno.h>

#include "adxl362.h"
#include "log.h"
#include "xtimer.h"
#include "timex.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Expected device identification values */
#define ADXL362_EXPECTED_DEVID_AD  (0xAD)
#define ADXL362_EXPECTED_DEVID_MST (0x1D)

/* Temperature sensor parameters, taken straight from the data sheet. */
/* 25 degrees Celsius equals this many LSB */
#define ADXL362_TEMP_25CEL_BIAS      (390)
/* Slope, milli-Celsius per LSB */
#define ADXL362_TEMP_SLOPE_MULT      (65)
/* To clarify how the above constants will be used: */
/* temp = (adc_val - bias) * slope + 25000; */

/* Command set */
enum {
    ADXL362_CMD_WRITE_REG = 0x0A,
    ADXL362_CMD_READ_REG  = 0x0B,
    ADXL362_CMD_READ_FIFO = 0x0D,
};

/* Register map */
enum {
    ADXL362_REG_DEVID_AD       = 0x00,
    ADXL362_REG_DEVID_MST      = 0x01,
    ADXL362_REG_PARTID         = 0x02,
    ADXL362_REG_REVID          = 0x03,
    ADXL362_REG_XDATA          = 0x08,
    ADXL362_REG_YDATA          = 0x09,
    ADXL362_REG_ZDATA          = 0x0A,
    ADXL362_REG_STATUS         = 0x0B,
    ADXL362_REG_FIFO_ENTRIES_L = 0x0C,
    ADXL362_REG_FIFO_ENTRIES_H = 0x0D,
    ADXL362_REG_XDATA_L        = 0x0E,
    ADXL362_REG_XDATA_H        = 0x0F,
    ADXL362_REG_YDATA_L        = 0x10,
    ADXL362_REG_YDATA_H        = 0x11,
    ADXL362_REG_ZDATA_L        = 0x12,
    ADXL362_REG_ZDATA_H        = 0x13,
    ADXL362_REG_TEMP_L         = 0x14,
    ADXL362_REG_TEMP_H         = 0x15,
    ADXL362_REG_RESERVED1      = 0x16,
    ADXL362_REG_RESERVED2      = 0x17,
    ADXL362_REG_SOFT_RESET     = 0x1F,
    ADXL362_REG_THRESH_ACT_L   = 0x20,
    ADXL362_REG_THRESH_ACT_H   = 0x21,
    ADXL362_REG_TIME_ACT       = 0x22,
    ADXL362_REG_THRESH_INACT_L = 0x23,
    ADXL362_REG_THRESH_INACT_H = 0x24,
    ADXL362_REG_TIME_INACT_L   = 0x25,
    ADXL362_REG_TIME_INACT_H   = 0x26,
    ADXL362_REG_ACT_INACT_CTL  = 0x27,
    ADXL362_REG_FIFO_CONTROL   = 0x28,
    ADXL362_REG_FIFO_SAMPLES   = 0x29,
    ADXL362_REG_INTMAP1        = 0x2A,
    ADXL362_REG_INTMAP2        = 0x2B,
    ADXL362_REG_FILTER_CTL     = 0x2C,
    ADXL362_REG_POWER_CTL      = 0x2D,
    ADXL362_REG_SELF_TEST      = 0x2E,
};

/* The values for the axis field of the FIFO buffer data */
enum {
    ADXL362_AXIS_X             = 0x00,
    ADXL362_AXIS_Y             = 0x01,
    ADXL362_AXIS_Z             = 0x02,
    ADXL362_AXIS_TEMP          = 0x03, /* built-in temperature sensor */
};

#define ADXL362_FIFO_AXIS_SHIFT            (14)
#define ADXL362_FIFO_AXIS_MASK             (0x03 << ADXL362_FIFO_AXIS_SHIFT)
#define ADXL362_FIFO_DATA_SHIFT            (0)
#define ADXL362_FIFO_DATA_MASK             (0x3fff << ADXL362_FIFO_DATA_SHIFT)
#define ADXL362_FIFO_DATA_WIDTH            (14)
#define ADXL362_FIFO_ENTRIES_MASK          (0x3ff)

#define ADXL362_POWER_CTL_MEASURE          (0x02) /* set Measurement mode, see data sheet for details */

#define ADXL362_STATUS_DATA_READY_MASK     (0x01)
#define ADXL362_STATUS_FIFO_READY_MASK     (0x02)
#define ADXL362_STATUS_FIFO_WATERMARK_MASK (0x04)
#define ADXL362_STATUS_FIFO_OVERRUN_MASK   (0x08)
#define ADXL362_STATUS_ACT_MASK            (0x10)
#define ADXL362_STATUS_INACT_MASK          (0x20)
#define ADXL362_STATUS_AWAKE_MASK          (0x40)
#define ADXL362_STATUS_ERR_USER_REGS_MASK  (0x80)

#define ADXL362_FILTER_RANGE_SHIFT         (6)
#define ADXL362_FILTER_RANGE_MASK          (0x03 << ADXL362_FILTER_RANGE_SHIFT)
#define ADXL362_FILTER_HALF_BW_SHIFT       (4)
#define ADXL362_FILTER_HALF_BW_MASK        (0x01 << ADXL362_FILTER_HALF_BW_SHIFT)
#define ADXL362_FILTER_ODR_SHIFT           (0)
#define ADXL362_FILTER_ODR_MASK            (0x07 << ADXL362_FILTER_ODR_SHIFT)

#define ADXL362_FIFO_CONTROL_TEMP_SHIFT    (2)
#define ADXL362_FIFO_CONTROL_TEMP_MASK     (0x01 << ADXL362_FILTER_RANGE_SHIFT)
#define ADXL362_FIFO_CONTROL_AH_SHIFT      (3)
#define ADXL362_FIFO_CONTROL_AH_MASK       (0x01 << ADXL362_FILTER_HALF_BW_SHIFT)
#define ADXL362_FIFO_CONTROL_MODE_SHIFT    (0)
#define ADXL362_FIFO_CONTROL_MODE_MASK     (0x03 << ADXL362_FILTER_ODR_SHIFT)

#define ADXL362_SELF_TEST_MASK             (0x01)
/* Time to wait during self test until values have stabilized */
#define ADXL362_SELF_TEST_SLEEP            (100 * MS_IN_USEC)
/* Self test expected values */
#define ADXL362_SELF_TEST_XOFF_MIN         (450)
#define ADXL362_SELF_TEST_XOFF_MAX         (710)
#define ADXL362_SELF_TEST_YOFF_MIN         (-710)
#define ADXL362_SELF_TEST_YOFF_MAX         (-450)
#define ADXL362_SELF_TEST_ZOFF_MIN         (350)
#define ADXL362_SELF_TEST_ZOFF_MAX         (650)
/* The self-test offset is dependent on the supply voltage, see data sheet:
 * Table 22. Self Test Output Scale Factors for Different Supply Voltages, Vs */
/* represented as a power of two fraction (FACTOR / 2**SHIFT) */
/* Choose this shift so that the 32 bit intermediate variable does not overflow,
 * 16 bits should be safe for the whole sensor range and still close to optimum */
#define ADXL362_SELF_TEST_VSS_SCALE_SHIFT  (16)    /* denominator exponent */
/* The values below were interpolated from the given table for a 3.3 V supply
 * voltage (2.95x is the resulting factor) */
/* The division by 100 is placed here with only numeric constants to allow the
 * compiler to optimize away the division completely */
#define ADXL362_SELF_TEST_VSS_SCALE_FACTOR ((295UL << ADXL362_SELF_TEST_VSS_SCALE_SHIFT) / 100)
/* convenience macro */
#define ADXL362_SELF_TEST_RESCALE_LIMIT(x) \
    (((x) * ADXL362_SELF_TEST_VSS_SCALE_FACTOR) >> ADXL362_SELF_TEST_VSS_SCALE_SHIFT)

/**
 * @internal
 * @brief Read registers from the device
 *
 * @param[in]  dev   pointer to device descriptor
 * @param[in]  addr  register address
 * @param[out] buf   output buffer
 * @param[in]  count number of bytes to read
 *
 * @return 0 on success
 * @return <0 on error
 */
static int adxl362_read_regs(const adxl362_t *dev, uint8_t addr, uint8_t *buf, size_t count)
{
    DEBUG("adxl362_read_regs: %p, 0x%02x, %p, %lu\n", (void *)dev,
        (unsigned int)addr, (void *)buf, (unsigned long)count);
    spi_acquire(dev->spi);
    gpio_clear(dev->cs);
    int res;
    res = spi_transfer_byte(dev->spi, ADXL362_CMD_READ_REG, NULL);
    if (res < 0) {
        DEBUG("adxl362_read_regs: SPI ERR!\n");
        return res;
    }
    res = spi_transfer_regs(dev->spi, addr, NULL, (char *)buf, count);
    if (res < 0) {
        DEBUG("adxl362_read_regs: SPI ERR!\n");
        return res;
    }
    gpio_set(dev->cs);
    spi_release(dev->spi);
    return 0;
}

/**
 * @internal
 * @brief Read registers from the device
 *
 * @param[in]  dev   pointer to device descriptor
 * @param[in]  addr  register address
 * @param[in]  val   register value
 *
 * @return 0 on success
 * @return <0 on error
 */
static int adxl362_write_reg(const adxl362_t *dev, uint8_t addr, uint8_t val)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs);
    int res;
    res = spi_transfer_byte(dev->spi, ADXL362_CMD_WRITE_REG, NULL);
    if (res < 0) {
        return res;
    }
    res = spi_transfer_reg(dev->spi, addr, val, NULL);
    if (res < 0) {
        return res;
    }
    gpio_set(dev->cs);
    spi_release(dev->spi);
    return 0;
}

/**
 * @internal
 * @brief Read FIFO contents from the device
 *
 * @param[in]  dev     pointer to device descriptor
 * @param[out] buf     output buffer
 * @param[in]  bufsize size of output buffer, in bytes
 *
 * @return number of read FIFO values on success
 * @return <0 on error
 */
static int adxl362_read_fifo(const adxl362_t *dev, uint8_t *buf, size_t bufsize)
{
    DEBUG("adxl362_read_fifo: %p, %p, %lu\n", (void *)dev, (void *)buf, (unsigned long) bufsize);
    int res;
    /* Check how many values are available in the FIFO */
    union {
        uint16_t u16;
        uint8_t u8[2];
    } tmp;
    res = adxl362_read_regs(dev, ADXL362_REG_FIFO_ENTRIES_L, tmp.u8, sizeof(tmp.u8));
    DEBUG("adxl362: FIFO_ENTRIES=%02x %02x\n", (unsigned int)tmp.u8[0], (unsigned int)tmp.u8[1]);
    /* FIFO_ENTRIES is stored little endian on the device, the below or+shift
     * should be a no-op on LE machines (byteorder.h is missing
     * host-to-little-endian functions, 2016-08-10) */
    uint16_t nentries = ((tmp.u8[1] << 8) | tmp.u8[0]) & ADXL362_FIFO_ENTRIES_MASK;
    DEBUG("adxl362: nfifo=%u\n", (unsigned int) nentries);
    if (nentries == 0) {
        return 0;
    }
    /* nentries is the number of 16 bit values in the FIFO */
    if (nentries > (bufsize / sizeof(uint16_t))) {
        nentries = bufsize / sizeof(uint16_t);
    }
    spi_acquire(dev->spi);
    gpio_clear(dev->cs);
    res = spi_transfer_regs(dev->spi, ADXL362_CMD_READ_FIFO, NULL, (char *)buf, nentries * sizeof(uint16_t));
    if (res < 0) {
        return res;
    }
    gpio_set(dev->cs);
    spi_release(dev->spi);
    DEBUG("adxl362_read_fifo: read=%lu\n", (unsigned long) nentries);
    return nentries;
}

/**
 * @internal
 * @brief Read the status register from the device
 *
 * @param[in]  dev  pointer to device descriptor
 *
 * @return status byte
 * @return 0xff on error
 */
static uint8_t adxl362_get_status(const adxl362_t *dev)
{
    uint8_t buf;
    int res = adxl362_read_regs(dev, ADXL362_REG_STATUS, &buf, 1);
    if (res < 0) {
        buf = 0xff;
    }
    return buf;
}

int adxl362_init(adxl362_t *dev, spi_t spi, gpio_t cs)
{
    DEBUG("adxl362_init %p\n", (void *)dev);
    if (dev == NULL) {
        return -EINVAL;
    }
    dev->spi = spi;
    dev->cs = cs;

    gpio_init(dev->cs, GPIO_OUT);
    gpio_set(dev->cs);

    /* Read chip identification */
    uint8_t buf[4];
    DEBUG("adxl362: Checking device identification\n");
    int res = adxl362_read_regs(dev, ADXL362_REG_DEVID_AD, buf, sizeof(buf));
    if (res < 0) {
        DEBUG("adxl362: SPI error!\n");
        return -EIO;
    }
    DEBUG("adxl362: ID: AD=0x%02x MST=0x%02x PART=0%03o REV=0x%02x\n",
        (unsigned int)buf[0], (unsigned int)buf[1],
        (unsigned int)buf[2], (unsigned int)buf[3]);

    if ((buf[0] != ADXL362_EXPECTED_DEVID_AD) ||
        (buf[1] != ADXL362_EXPECTED_DEVID_MST)) {
        /* PARTID is 0362 (octal) for the ADXL362, but this driver might work
         * with other variants as well (untested), so there's no reason to
         * further restrict us right now. */
        LOG_ERROR("adxl362: ID: AD=0x%02x MST=0x%02x PART=0%03o REV=0x%02x\n",
            (unsigned int)buf[0], (unsigned int)buf[1],
            (unsigned int)buf[2], (unsigned int)buf[3]);
        LOG_ERROR("adxl362: Unknown ID, expected AD=0x%02x MST=0x%02x\n",
            (unsigned int)ADXL362_EXPECTED_DEVID_AD,
            (unsigned int)ADXL362_EXPECTED_DEVID_MST);
        return -ENODEV;
    }

    /* Perform a soft reset */
    res = adxl362_write_reg(dev, ADXL362_REG_SOFT_RESET, 'R');
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }
    /* There's no mention of it in the data sheet, but empirical evidence
     * suggests that there has to be a short delay between writing soft reset,
     * and writing other registers, or else the second write is sometimes
     * ignored. 5 ms seem to work fine (picked at random during testing) */
    xtimer_usleep(5 * MS_IN_USEC);

    /* Set some valid default filter settings */
    res = adxl362_set_filter(dev, ADXL362_RANGE_4G, ADXL362_ODR_100HZ, 1);
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }

    res = adxl362_set_fifo(dev, false, ADXL362_FIFO_MODE_DISABLED, ADXL362_FIFO_WATERMARK_DEFAULT);
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }

    uint8_t power_ctl = ADXL362_POWER_CTL_MEASURE;

    DEBUG("adxl362: write POWER_CTL=0x%02x\n", power_ctl);
    res = adxl362_write_reg(dev, ADXL362_REG_POWER_CTL, power_ctl);
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }

    return 0;
}

int adxl362_set_filter(adxl362_t *dev, adxl362_range_t range, adxl362_odr_t odr, bool half_bw)
{
    DEBUG("adxl362_set_filter %p, range=%u (%u g), half_bw=%u, odr=%u (%u Hz)\n",
        (void *)dev, (unsigned int)range, (2U << (unsigned int)range), (unsigned int)half_bw,
        (unsigned int)odr, (400U >> (ADXL362_ODR_400HZ - (unsigned int)odr)));
    uint8_t filter_ctl =
        ((range << ADXL362_FILTER_RANGE_SHIFT) & ADXL362_FILTER_RANGE_MASK) |
        ((half_bw << ADXL362_FILTER_HALF_BW_SHIFT) & ADXL362_FILTER_HALF_BW_MASK) |
        ((odr << ADXL362_FILTER_ODR_SHIFT) & ADXL362_FILTER_ODR_MASK);
    DEBUG("adxl362: write FILTER_CTL=0x%02x\n", filter_ctl);
    int res = adxl362_write_reg(dev, ADXL362_REG_FILTER_CTL, filter_ctl);
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }
    /* The hardware values for the valid range settings are the number of left
     * shifts needed to go from 2 to the chosen range */
    dev->scale_shift = range;
    return 0;
}

int adxl362_set_fifo(const adxl362_t *dev, bool store_temp, adxl362_fifo_mode_t mode, unsigned int watermark)
{
    DEBUG("adxl362_set_fifo %p, store_temp=%u, mode=%u, size=%u\n",
        (void *)dev, (unsigned int)store_temp, (unsigned int)mode, watermark);
    if (watermark >= ADXL362_FIFO_SIZE) {
        watermark = ADXL362_FIFO_SIZE - 1;
    }

    uint8_t watermark_lsb = watermark & 0xff;
    uint8_t fifo_control =
        ((store_temp << ADXL362_FIFO_CONTROL_TEMP_SHIFT) & ADXL362_FIFO_CONTROL_TEMP_MASK) |
        ((mode << ADXL362_FIFO_CONTROL_MODE_SHIFT) & ADXL362_FIFO_CONTROL_MODE_MASK) |
        (((watermark >> 8) << ADXL362_FIFO_CONTROL_AH_SHIFT) & ADXL362_FIFO_CONTROL_AH_MASK);
    int res;
    DEBUG("adxl362: write FIFO_CONTROL=0x%02x\n", fifo_control);
    res = adxl362_write_reg(dev, ADXL362_REG_FIFO_CONTROL, fifo_control);
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }
    DEBUG("adxl362: write FIFO_SAMPLES=0x%02x\n", watermark_lsb);
    res = adxl362_write_reg(dev, ADXL362_REG_FIFO_SAMPLES, watermark_lsb);
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }
    return 0;
}

int adxl362_read_acc_fifo(const adxl362_t *dev, adxl362_data_t *acc, size_t count)
{
    DEBUG("adxl362_read_acc %p, %p, %lu\n", (void *)dev, (void *)acc, (unsigned long)count);
    /* Check status */
    uint8_t status = adxl362_get_status(dev);
    if (status == 0xff) {
        LOG_ERROR("adxl362: Status error!\n");
        return -EIO;
    }
    DEBUG("adxl362: status=0x%02x\n", (unsigned int)status);
    if ((status & ADXL362_STATUS_FIFO_READY_MASK) == 0) {
        DEBUG("adxl362: FIFO ready is not set\n");
    }
    if (ENABLE_DEBUG != 0) {
        uint8_t tmp = 0;
        adxl362_read_regs(dev, ADXL362_REG_FILTER_CTL, &tmp, 1);
        DEBUG("adxl362: filter=0x%02x\n", tmp);
        adxl362_read_regs(dev, ADXL362_REG_FIFO_CONTROL, &tmp, 1);
        DEBUG("adxl362:   fifo=0x%02x\n", tmp);
    }
    if (count == 0) {
        return 0;
    }

    /* Unpacking/decoding values in-place to reduce memory usage */
    uint8_t *buf = (uint8_t*)acc;
    size_t bufsize = count * sizeof(*acc);
    int nentries = adxl362_read_fifo(dev, buf, bufsize);
    if (nentries < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }
    bool found_beginning = false;
    int ret = 0;
    while (nentries > 0) {
        /* Pop a FIFO value off the beginning of the buffer */
        /* Data is stored little endian on the device */
        uint16_t data = (buf[1] << 8) | buf[0];
        buf += sizeof(data);
        --nentries;

        /* Decode axis identification */
        unsigned int axis = (data & ADXL362_FIFO_AXIS_MASK) >> ADXL362_FIFO_AXIS_SHIFT;
        if (!found_beginning) {
            /* Data order on the device is X1, Y1, Z1, (Temp1, ) X2, Y2, Z2,
             * (Temp2, ) etc. Skip values until we are at an X axis value, to
             * get the measurements in sync */
            if (axis != ADXL362_AXIS_X) {
                DEBUG("adxl362: Skipping value axis=%u\n", axis);
                continue;
            }
            found_beginning = true;
        }
        int32_t value = ((data & ADXL362_FIFO_DATA_MASK) >> ADXL362_FIFO_DATA_SHIFT);
        /* Sign extend */
        value = (int32_t)(value << (sizeof(value) * 8 - ADXL362_FIFO_DATA_WIDTH))
            >> (sizeof(value) * 8 - ADXL362_FIFO_DATA_WIDTH);
        /* rescale to milli-g */
        value <<= dev->scale_shift;

        switch(axis) {
            case ADXL362_AXIS_X:
                DEBUG("adxl362: %4d X=%6" PRId32 "\n", ret, value);
                acc->acc_x = (int16_t)value;
                break;
            case ADXL362_AXIS_Y:
                DEBUG("adxl362: %4d Y=%6" PRId32 "\n", ret, value);
                acc->acc_y = (int16_t)value;
                break;
            case ADXL362_AXIS_Z:
                DEBUG("adxl362: %4d Z=%6" PRId32 "\n", ret, value);
                acc->acc_z = (int16_t)value;
                /* Z should be the last value in the sequence, move to next point */
                ++acc;
                /* We only return the number of complete readings */
                ++ret;
                break;
            default:
                /* Ignoring temperature for now */
                break;
        }
    }
    return ret;
}

int adxl362_read_acc_now(const adxl362_t *dev, adxl362_data_t *acc)
{
    DEBUG("adxl362_read_acc_now: %p, %p\n", (void *)dev, (void *)acc);
    uint8_t buf[sizeof(*acc)];
    /* The acceleration data registers are located next to each other in the
     * register map, get all of them in one transaction */
    if (ENABLE_DEBUG != 0) {
        uint8_t tmp = 0;
        adxl362_read_regs(dev, ADXL362_REG_FILTER_CTL, &tmp, 1);
        DEBUG("adxl362: filter=0x%02x\n", tmp);
        adxl362_read_regs(dev, ADXL362_REG_FIFO_CONTROL, &tmp, 1);
        DEBUG("adxl362:   fifo=0x%02x\n", tmp);
    }
    uint8_t status = adxl362_get_status(dev);
    DEBUG("adxl362: status=0x%02x\n", (unsigned int)status);
    if ((status & ADXL362_STATUS_DATA_READY_MASK) == 0) {
        LOG_WARNING("adxl362: no data available\n");
        return -EAGAIN;
    }
    DEBUG("adxl362: dev->scale_shift=%u\n", (unsigned int)dev->scale_shift);
    int res = adxl362_read_regs(dev, ADXL362_REG_XDATA_L, buf, sizeof(buf));
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }
    DEBUG("adxl362: buf: %02x %02x %02x %02x %02x %02x\n",
        (unsigned int)buf[0], (unsigned int)buf[1],
        (unsigned int)buf[2], (unsigned int)buf[3],
        (unsigned int)buf[4], (unsigned int)buf[5]);

    acc->acc_x = ((buf[1] << 8) | buf[0]) << dev->scale_shift;
    acc->acc_y = ((buf[3] << 8) | buf[2]) << dev->scale_shift;
    acc->acc_z = ((buf[5] << 8) | buf[4]) << dev->scale_shift;

    return 0;
}

int adxl362_read_temp(const adxl362_t *dev, int32_t *temp)
{
    uint8_t buf[2];
    /* TEMP_L and TEMP_H are located next to each other in the register map, get
     * both using a single 2 byte read transaction */
    int res = adxl362_read_regs(dev, ADXL362_REG_TEMP_L, buf, sizeof(buf));
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }

    /* byte ordering and alignment */
    int32_t adc_val = (buf[1] << 8) | buf[0];

    (*temp) = (adc_val - ADXL362_TEMP_25CEL_BIAS) * ADXL362_TEMP_SLOPE_MULT + 25000;
    return 0;
}

static bool adxl362_self_test_check(int16_t value, int16_t min, int16_t max)
{
    bool pass = true;
    DEBUG("adxl362_self_test_check:   [%6d, %6d]  ", (int)min, (int)max);
    if (value > max) {
        pass = false;
    }
    if (value < min) {
        pass = false;
    }
    DEBUG("%s", (pass ? "OK\n" : "<--- NOT OK\n"));
    return pass;
}

int adxl362_self_test(const adxl362_t *dev)
{
    /*
     * Algorithm outline (from data sheet):
     *
     * 1. Read acceleration data for the x-, y-, and z-axes.
     * 2. Assert self test by setting the ST bit in the SELF_TEST register
     * 3. Wait 1/ODR for the output to settle to its new value.
     * 4. Read acceleration data for the x-, y-, and z-axes.
     * 5. Compare to the values from Step 1, and convert the difference from LSB
     *    to mg by multiplying by the sensitivity. If the observed difference
     *    falls within the self test output change specification listed in
     *    Table 1, then the device passes self test and is deemed operational.
     * 6. Deassert self test by clearing the ST bit in the SELF_TEST register
     */
    DEBUG("adxl362_self_test: %p\n", (void *)dev);
    /* Let values stabilize */
    xtimer_usleep(ADXL362_SELF_TEST_SLEEP);
    /* read current values of X, Y, Z */
    adxl362_data_t baseline;
    int res;
    res = adxl362_read_acc_now(dev, &baseline);
    if (res < 0) {
        return res;
    }

    DEBUG("adxl362_self_test:    baseline: %6d %6d %6d\n",
        (int)baseline.acc_x, (int)baseline.acc_y, (int)baseline.acc_z);
    DEBUG("adxl362_self_test: enable self-test bit\n");
    res = adxl362_write_reg(dev, ADXL362_REG_SELF_TEST, ADXL362_SELF_TEST_MASK);
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }
    /* Let values stabilize */
    xtimer_usleep(ADXL362_SELF_TEST_SLEEP);
    /* read new values of X, Y, Z */
    adxl362_data_t self_test_data;
    res = adxl362_read_acc_now(dev, &self_test_data);
    if (res < 0) {
        return res;
    }

    DEBUG("adxl362_self_test: disable self-test bit\n");
    res = adxl362_write_reg(dev, ADXL362_REG_SELF_TEST, 0);
    if (res < 0) {
        LOG_ERROR("adxl362: SPI error!\n");
        return -EIO;
    }

    DEBUG("adxl362_self_test: with ST bit: %6d %6d %6d\n",
        (int)self_test_data.acc_x, (int)self_test_data.acc_y, (int)self_test_data.acc_z);

    self_test_data.acc_x -= baseline.acc_x;
    self_test_data.acc_y -= baseline.acc_y;
    self_test_data.acc_z -= baseline.acc_z;
    DEBUG("adxl362_self_test:  difference: %6d %6d %6d\n",
        (int)self_test_data.acc_x, (int)self_test_data.acc_y, (int)self_test_data.acc_z);

    bool pass = true;
    bool check;
    DEBUG("adxl362_self_test: Check X:\n");
    check = adxl362_self_test_check(self_test_data.acc_x,
        ADXL362_SELF_TEST_RESCALE_LIMIT(ADXL362_SELF_TEST_XOFF_MIN),
        ADXL362_SELF_TEST_RESCALE_LIMIT(ADXL362_SELF_TEST_XOFF_MAX));
    if (!check) {
        pass = false;
    }
    DEBUG("adxl362_self_test: Check Y:\n");
    check = adxl362_self_test_check(self_test_data.acc_y,
        ADXL362_SELF_TEST_RESCALE_LIMIT(ADXL362_SELF_TEST_YOFF_MIN),
        ADXL362_SELF_TEST_RESCALE_LIMIT(ADXL362_SELF_TEST_YOFF_MAX));
    if (!check) {
        pass = false;
    }
    DEBUG("adxl362_self_test: Check Z:\n");
    check = adxl362_self_test_check(self_test_data.acc_z,
        ADXL362_SELF_TEST_RESCALE_LIMIT(ADXL362_SELF_TEST_ZOFF_MIN),
        ADXL362_SELF_TEST_RESCALE_LIMIT(ADXL362_SELF_TEST_ZOFF_MAX));
    if (!check) {
        pass = false;
    }
    return (pass ? 1 : 0);
}
