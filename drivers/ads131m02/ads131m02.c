/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ads131m02
 * @{
 *
 * @file
 * @brief       ADS131M02 Analog-to-digital converter driver
 *
 * @author     Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "ads131m02.h"
#include "byteorder.h"
#include "time_units.h"
#include "xtimer.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define SPI_PARAM(dev) dev->params->spi, dev->params->cs_pin
#define SPI_ACQUIRE(dev) spi_acquire(dev->params->spi, dev->params->cs_pin, SPI_MODE_1, dev->params->spi_clk);
#define SPI_RELEASE(dev) spi_release(dev->params->spi)

#define BIT(X) (1<<(X))


#define ADS131M02_DEVICE_ID     0x22

/* Device settings Registers */
#define ADS131M02_ID_REG        0x00
#define ADS131M02_STATUS_REG    0x01

/* Global settings Registers */
#define ADS131M02_MODE_REG      0x02
#define ADS131M02_CLOCK_REG     0x03
#define ADS131M02_GAIN_REG      0x04
#define ADS131M02_CFG_REG       0x06
#define ADS131M02_THRESH_MSB_REG    0x07
#define ADS131M02_THRESH_LSB_REG    0x08

/* Channel 0 settings Registers */
#define ADS131M02_CH0_CFG_REG       0x09
#define ADS131M02_CH0_OCAL_MSB_REG  0x0A
#define ADS131M02_CH0_OCAL_LSB_REG  0x0B
#define ADS131M02_CH0_GCAL_MSB_REG  0x0C
#define ADS131M02_CH0_GCAL_LSB_REG  0x0D

/* Channel 1 settings Registers */
#define ADS131M02_CH1_CFG_REG       0x0E
#define ADS131M02_CH1_OCAL_MSB_REG  0x0F
#define ADS131M02_CH1_OCAL_LSB_REG  0x10
#define ADS131M02_CH1_GCAL_MSB_REG  0x11
#define ADS131M02_CH1_GCAL_LSB_REG  0x12

/* Register Map CRC Registers */
#define ADS131M02_REGMAP_CRC_REG    0x3E

#define ADS131M02_REF_INTERNAL      1200
#define ADS131M02_RESOLUTION        24

/* ADS131M02 cmds */
#define ADS131M02_NULL_CMD      0x0000
#define ADS131M02_RESET_CMD     0x0011
#define ADS131M02_STANDBY_CMD       0x0022
#define ADS131M02_WAKEUP_CMD        0x0033
#define ADS131M02_LOCK_CMD      0x0555
#define ADS131M02_UNLOCK_CMD        0x0655
#define ADS131M02_RREG_CMD      0xA000
#define ADS131M02_WREG_CMD      0x6000

#define ADS131M02_RESET_RSP     0xFF22

#define ADS131M02_GAIN0_MASK        GENMASK(2, 0)
#define ADS131M02_GAIN1_MASK        GENMASK(6, 4)
#define ADS131M02_CHANNEL0_ENABLE   BIT(8)
#define ADS131M02_CHANNEL1_ENABLE   BIT(9)
#define ADS131M02_DRDY_CH0_MASK     BIT(0)
#define ADS131M02_DRDY_CH1_MASK     BIT(1)
#define ADS131M02_OSR_256_MASK      BIT(2)
#define ADS131M02_OSR_512_MASK      BIT(3)
#define ADS131M02_OSR_1024_MASK     BIT(3) | BIT(2)
#define ADS131M02_OSR_2048_MASK     BIT(4)
#define ADS131M02_OSR_4096_MASK     BIT(4) | BIT(2)
#define ADS131M02_OSR_8192_MASK     BIT(4) | BIT(3)
#define ADS131M02_OSR_16384_MASK    BIT(4) | BIT(3) | BIT(2)
#define ADS131M02_GC_MODE_MASK      BIT(8)
#define ADS131M02_GC_DELAY_MASK     GENMASK(12, 9)
#define ADS131M02_PWR_HR        BIT(1) | BIT(0)
#define ADS131M02_PWR_LP        BIT(0)

#define ADS131M02_DISABLE_ADC       0x000E
#define ADS131M02_RESET_DELAY       100

/* generous upper bound on the conversion period so ads131m02_sample()
 * does not block forever if DRDY never arrives (e.g. missing CLKIN) */
#define ADS131M02_DRDY_TIMEOUT_US   (100U * US_PER_MS)

#define ADS131M02_GAIN_1        0
#define ADS131M02_GAIN_2        1
#define ADS131M02_GAIN_4        2
#define ADS131M02_GAIN_8        3
#define ADS131M02_GAIN_16       4
#define ADS131M02_GAIN_32       5
#define ADS131M02_GAIN_64       6
#define ADS131M02_GAIN_128      7


static inline uint32_t sys_get_be24(const uint8_t src[3])
{
    return ((uint32_t)src[0] << 16) | byteorder_bebuftohs(&src[1]);
}

static int _read_sample(ads131m02_t *dev, uint32_t *chan0, uint32_t *chan1)
{
    uint8_t rx_buf[12];

    /* clock out a NULL command while reading back the previous
     * conversion's status + channel data in the same full-duplex frame */
    spi_transfer_bytes(SPI_PARAM(dev), false, NULL, rx_buf, sizeof(rx_buf));

    /* the STATUS register (16 bit) is returned left-justified in the
     * 24-bit response word, i.e. in its upper two bytes */
    uint16_t status = byteorder_bebuftohs(&rx_buf[0]);
    if (chan0) {
        *chan0  = sys_get_be24(&rx_buf[3]);
    }
    if (chan1) {
        *chan1  = sys_get_be24(&rx_buf[6]);
    }

    return status & 0x3;
}

static int _read_reg(ads131m02_t *dev, uint8_t addr)
{
    /* every SPI frame is always 4 words (response + CH0 + CH1 + CRC/zero),
     * regardless of the command - so both the command and the response
     * read must always transfer a full 12-byte frame. */
    uint8_t tx_buf[12] = {0};
    uint8_t rx_buf[12] = {0};
    byteorder_htobebufs(&tx_buf[0], ADS131M02_RREG_CMD | (addr << 7));

    /* the response to a register command is only returned in the
     * *following* SPI frame, so CS must be deasserted between the
     * command and the response read to mark the frame boundary. */
    spi_transfer_bytes(SPI_PARAM(dev), false, &tx_buf, NULL, sizeof(tx_buf));
    spi_transfer_bytes(SPI_PARAM(dev), false, NULL, rx_buf, sizeof(rx_buf));

    /* the 16-bit register value is returned left-justified in the
     * 24-bit response word (upper two bytes), the last byte is zero
     * padding - only take the first two bytes to get the real value. */
    return byteorder_bebuftohs(rx_buf);
}

static int _write_reg(ads131m02_t *dev, uint8_t addr, uint16_t write_data)
{
    uint8_t tx_buf[12] = {0};
    uint8_t rx_buf[12];
    byteorder_htobebufs(&tx_buf[0], ADS131M02_WREG_CMD | (addr << 7));
    byteorder_htobebufs(&tx_buf[3], write_data);

    /* same as above: the write confirmation is only returned in the
     * following SPI frame, and each frame is always a full 12 bytes. */
    spi_transfer_bytes(SPI_PARAM(dev), false, &tx_buf, NULL, sizeof(tx_buf));
    spi_transfer_bytes(SPI_PARAM(dev), false, NULL, rx_buf, sizeof(rx_buf));

    return 0;
}

static int ads131m02_device_reset(ads131m02_t *dev)
{
    int res;
    uint8_t tx_buf[12] = {0};
    uint8_t rx_buf[12] = {0};

    byteorder_htobebufs(tx_buf, ADS131M02_RESET_CMD);

    /* the RESET command is not latched by the device until the entire
     * (4-word/12-byte) frame is complete - sending a shorter frame
     * makes the device respond with 0x0011 instead of resetting. */
    spi_transfer_bytes(SPI_PARAM(dev), false, tx_buf, NULL, sizeof(tx_buf));

    /* the device needs tREGACQ (up to ~4ms @ 8.192 MHz CLKIN) to reload
     * its registers to their default values before the RESET
     * acknowledgment (and any further communication) is valid. */
    xtimer_msleep(ADS131M02_RESET_DELAY);

    /* clock a NULL command to read back the RESET acknowledgment,
     * which is returned in the frame following the RESET command. */
    spi_transfer_bytes(SPI_PARAM(dev), false, NULL, rx_buf, sizeof(rx_buf));

    res = byteorder_bebuftohs(rx_buf);

    if (res != ADS131M02_RESET_RSP) {
        return -EIO;
    }

    return 0;
}

static void _unlock(void *ctx)
{
    mutex_unlock(ctx);
}

void __attribute__((weak)) board_ads131m02_clk_ctrl(ads131m02_t *dev, bool on)
{
    (void)dev;
    (void)on;
}

int ads131m02_init(ads131m02_t *dev, const ads131m02_params_t *params)
{
    int res;

    dev->params = params;
    mutex_init_locked(&dev->ready);

    board_ads131m02_clk_ctrl(dev, true);

    spi_init_cs(params->spi, params->cs_pin);
    /* DRDY is active low - a new conversion result is signaled by a
     * high-to-low transition. (The one-time low-to-high transition
     * after reset, which indicates the SPI interface being ready, is
     * not relied upon here - we wait out tREGACQ instead, see
     * ads131m02_device_reset()) */
    res = gpio_init_int(params->ready_pin, GPIO_IN, GPIO_FALLING, _unlock, &dev->ready);
    if (res) {
        board_ads131m02_clk_ctrl(dev, false);
        return res;
    }

    SPI_ACQUIRE(dev);

    res = _read_reg(dev, ADS131M02_ID_REG);
    /* the upper byte of the ID register holds the fixed device ID,
     * the lower byte is reserved/revision info */
    if ((res >> 8) != ADS131M02_DEVICE_ID) {
        DEBUG("ads131m02: Device ID mismatch %x\n", res);
        res = -EINVAL;
        goto out;
    }

    res = ads131m02_device_reset(dev);

    /* disable ADC until we start sampling */
    _write_reg(dev, ADS131M02_CLOCK_REG, ADS131M02_DISABLE_ADC);

out:
    SPI_RELEASE(dev);

    board_ads131m02_clk_ctrl(dev, false);
    return res;
}

static int _setup_adc(ads131m02_t *dev, uint8_t channels)
{
    int enable = _read_reg(dev, ADS131M02_CLOCK_REG);
    if (enable < 0) {
        return enable;
    }
    enable |= (channels & (ADS131M02_CHANNEL_0 | ADS131M02_CHANNEL_1)) << 8;

    enable &= ~(ADS131M02_OSR_16384_MASK);
    enable |=  ADS131M02_OSR_1024_MASK; // TODO: configurable

    _write_reg(dev, ADS131M02_CLOCK_REG, enable);

    return 0;
}

int ads131m02_sample(ads131m02_t *dev, uint8_t channels, uint32_t *chan0, uint32_t *chan1, size_t len)
{
    int res;
    size_t num_samples = 0;

    /* only pass along buffers for the channels that were requested */
    uint32_t *chan0_buf = (channels & ADS131M02_CHANNEL_0) ? chan0 : NULL;
    uint32_t *chan1_buf = (channels & ADS131M02_CHANNEL_1) ? chan1 : NULL;

    board_ads131m02_clk_ctrl(dev, true);

    SPI_ACQUIRE(dev);

    /* drain any stale/spurious DRDY edge so the first mutex_lock()
     * below only returns once a fresh conversion result is ready */
    mutex_trylock(&dev->ready);

    res = _setup_adc(dev, channels);
    if (res < 0) {
        goto out;
    }

    while (num_samples < len) {
        res = xtimer_mutex_lock_timeout(&dev->ready, ADS131M02_DRDY_TIMEOUT_US);
        if (res < 0) {
            /* DRDY did not arrive in time (e.g. missing CLKIN) */
            res = -ETIMEDOUT;
            break;
        }
        res = _read_sample(dev, chan0_buf, chan1_buf);
        if (res < 0) {
            break;
        }

        if ((res & ADS131M02_DRDY_CH0_MASK) && chan0_buf) {
            ++chan0_buf;
        }
        if ((res & ADS131M02_DRDY_CH1_MASK) && chan1_buf) {
            ++chan1_buf;
        }

        ++num_samples;
    }

    /* return the error if we could not even get a single sample,
     * otherwise return the number of samples that were read */
    res = (num_samples == 0 && res < 0) ? res : (int)num_samples;

out:
    /* sampling done - disable ADC */
    _write_reg(dev, ADS131M02_CLOCK_REG, ADS131M02_DISABLE_ADC);

    SPI_RELEASE(dev);

    board_ads131m02_clk_ctrl(dev, false);

    return res;
}
