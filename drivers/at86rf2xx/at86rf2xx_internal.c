/*
 * SPDX-FileCopyrightText: 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "ztimer.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"

#if !AT86RF2XX_IS_PERIPH
#include "periph/spi.h"
#include "periph/gpio.h"

#define SPIDEV          (dev->params.spi)
#define CSPIN           (dev->params.cs_pin)

static inline void getbus(const at86rf2xx_t *dev)
{
    spi_acquire(SPIDEV, CSPIN, SPI_MODE_0, dev->params.spi_clk);
}

void at86rf2xx_reg_write(const at86rf2xx_t *dev, uint8_t addr, uint8_t value)
{
    uint8_t reg = (AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_WRITE | addr);

    getbus(dev);
    spi_transfer_reg(SPIDEV, CSPIN, reg, value);
    spi_release(SPIDEV);
}

uint8_t at86rf2xx_reg_read(const at86rf2xx_t *dev, uint8_t addr)
{
    uint8_t reg = (AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_READ | addr);
    uint8_t value;

    getbus(dev);
    value = spi_transfer_reg(SPIDEV, CSPIN, reg, 0);
    spi_release(SPIDEV);

    return value;
}

void at86rf2xx_sram_read(const at86rf2xx_t *dev, uint8_t offset,
                         uint8_t *data, size_t len)
{
    uint8_t reg = (AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_READ);

    getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg);
    spi_transfer_byte(SPIDEV, CSPIN, true, offset);
    spi_transfer_bytes(SPIDEV, CSPIN, false, NULL, data, len);
    spi_release(SPIDEV);
}

void at86rf2xx_sram_write(const at86rf2xx_t *dev, uint8_t offset,
                          const uint8_t *data, size_t len)
{
    uint8_t reg = (AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_WRITE);

    getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg);
    spi_transfer_byte(SPIDEV, CSPIN, true, offset);
    spi_transfer_bytes(SPIDEV, CSPIN, false, data, NULL, len);
    spi_release(SPIDEV);
}

void at86rf2xx_fb_start(const at86rf2xx_t *dev)
{
    uint8_t reg = AT86RF2XX_ACCESS_FB | AT86RF2XX_ACCESS_READ;

    getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg);
}

void at86rf2xx_fb_read(const at86rf2xx_t *dev,
                       uint8_t *data, size_t len)
{
    spi_transfer_bytes(SPIDEV, CSPIN, true, NULL, data, len);
}

void at86rf2xx_fb_stop(const at86rf2xx_t *dev)
{
    /* transfer one byte (which we ignore) to release the chip select */
    spi_transfer_byte(SPIDEV, CSPIN, false, 1);
    spi_release(SPIDEV);
}

#endif /* SPI based transceiver */

uint8_t at86rf2xx_get_status(const at86rf2xx_t *dev)
{
    /* if sleeping immediately return state */
    if (dev->state == AT86RF2XX_STATE_SLEEP) {
        return dev->state;
    }

    return (at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
            & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS);
}

void at86rf2xx_assert_awake(at86rf2xx_t *dev)
{
    if (at86rf2xx_get_status(dev) == AT86RF2XX_STATE_SLEEP) {
        /* wake up and wait for transition to TRX_OFF */
#if AT86RF2XX_IS_PERIPH
        /* Setting SLPTR bit in TRXPR to 0 returns the radio transceiver
         * to the TRX_OFF state */
        *AT86RF2XX_REG__TRXPR &= ~(AT86RF2XX_TRXPR_SLPTR);
#else
        gpio_clear(dev->params.sleep_pin);
#endif
        ztimer_sleep(ZTIMER_USEC, AT86RF2XX_WAKEUP_DELAY);

        /* update state: on some platforms, the timer behind ztimer
         * may be inaccurate or the radio itself may take longer
         * to wake up due to extra capacitance on the oscillator.
         * Spin until we are actually awake
         */
        do {
            dev->state = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
                         & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
        } while (dev->state != AT86RF2XX_TRX_STATUS__TRX_OFF);
    }
}

void at86rf2xx_hardware_reset(at86rf2xx_t *dev)
{
    /* trigger hardware reset */
#if AT86RF2XX_IS_PERIPH
    /* set reset Bit */
    *(AT86RF2XX_REG__TRXPR) |= AT86RF2XX_TRXPR_TRXRST;
#else
    gpio_clear(dev->params.reset_pin);
    ztimer_sleep(ZTIMER_USEC, AT86RF2XX_RESET_PULSE_WIDTH);
    gpio_set(dev->params.reset_pin);
#endif
    ztimer_sleep(ZTIMER_USEC, AT86RF2XX_RESET_DELAY);

    /* update state: if the radio state was P_ON (initialization phase),
     * it remains P_ON. Otherwise, it should go to TRX_OFF
     */
    do {
        dev->state = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
                     & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    } while ((dev->state != AT86RF2XX_STATE_TRX_OFF)
             && (dev->state != AT86RF2XX_STATE_P_ON));
}

#if AT86RF2XX_RANDOM_NUMBER_GENERATOR
void at86rf2xx_get_random(at86rf2xx_t *dev, uint8_t *data, size_t len)
{
    at86rf2xx_disable_smart_idle(dev);
    for (size_t byteCount = 0; byteCount < len; ++byteCount) {
        uint8_t rnd = 0;
        for (uint8_t i = 0; i < 4; ++i) {
            /* bit 5 and 6 of the AT86RF2XX_REG__PHY_RSSI register contain the RND_VALUE */
            uint8_t regVal = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_RSSI)
                             & AT86RF2XX_PHY_RSSI_MASK__RND_VALUE;
            /* shift the two random bits first to the right and then to the correct position of the return byte */
            regVal = regVal >> 5;
            regVal = regVal << 2 * i;
            rnd |= regVal;
        }
        data[byteCount] = rnd;
    }
    at86rf2xx_enable_smart_idle(dev);
}
#endif

#if !AT86RF2XX_IS_PERIPH
void at86rf2xx_spi_init(at86rf2xx_t *dev, void (*irq_handler)(void *arg))
{
    /* initialize GPIOs */
    spi_init_cs(dev->params.spi, dev->params.cs_pin);
    gpio_init(dev->params.sleep_pin, GPIO_OUT);
    gpio_clear(dev->params.sleep_pin);
    gpio_init(dev->params.reset_pin, GPIO_OUT);
    gpio_set(dev->params.reset_pin);
    gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, irq_handler, dev);

    /* Intentionally check if bus can be acquired, if assertions are on */
    if (!IS_ACTIVE(NDEBUG)) {
        spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0, dev->params.spi_clk);
        spi_release(dev->params.spi);
    }
}
#endif
