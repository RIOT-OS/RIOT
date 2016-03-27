/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
 *
 * @}
 */

#include "periph/spi.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"

void at86rf2xx_reg_write(const at86rf2xx_t *dev,
                         const uint8_t addr,
                         const uint8_t value)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_reg(dev->params.spi,
                     AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_WRITE | addr,
                     value, 0);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}

uint8_t at86rf2xx_reg_read(const at86rf2xx_t *dev, const uint8_t addr)
{
    char value;

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_reg(dev->params.spi,
                     AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_READ | addr,
                     0, &value);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);

    return (uint8_t)value;
}

void at86rf2xx_sram_read(const at86rf2xx_t *dev,
                         const uint8_t offset,
                         uint8_t *data,
                         const size_t len)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_reg(dev->params.spi,
                     AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_READ,
                     (char)offset, NULL);
    spi_transfer_bytes(dev->params.spi, NULL, (char *)data, len);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}

void at86rf2xx_sram_write(const at86rf2xx_t *dev,
                          const uint8_t offset,
                          const uint8_t *data,
                          const size_t len)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_reg(dev->params.spi,
                     AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_WRITE,
                     (char)offset, NULL);
    spi_transfer_bytes(dev->params.spi, (char *)data, NULL, len);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}

void at86rf2xx_fb_start(const at86rf2xx_t *dev)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_byte(dev->params.spi,
                      AT86RF2XX_ACCESS_FB | AT86RF2XX_ACCESS_READ,
                      NULL);
}

void at86rf2xx_fb_read(const at86rf2xx_t *dev,
                       uint8_t *data,
                       const size_t len)
{
    spi_transfer_bytes(dev->params.spi, NULL, (char *)data, len);
}

void at86rf2xx_fb_stop(const at86rf2xx_t *dev)
{
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}

uint8_t at86rf2xx_get_status(const at86rf2xx_t *dev)
{
    /* if sleeping immediately return state */
    if(dev->state == AT86RF2XX_STATE_SLEEP)
        return dev->state;

    return at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
                & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
}

void at86rf2xx_assert_awake(at86rf2xx_t *dev)
{
    if(at86rf2xx_get_status(dev) == AT86RF2XX_STATE_SLEEP) {

        /* wake up and wait for transition to TRX_OFF */
        gpio_clear(dev->params.sleep_pin);
        xtimer_usleep(AT86RF2XX_WAKEUP_DELAY);

        /* update state */
        dev->state = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
                         & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    }
}

void at86rf2xx_hardware_reset(at86rf2xx_t *dev)
{
    /* wake up from sleep in case radio is sleeping */
    at86rf2xx_assert_awake(dev);

    /* trigger hardware reset */
    gpio_clear(dev->params.reset_pin);
    xtimer_usleep(AT86RF2XX_RESET_PULSE_WIDTH);
    gpio_set(dev->params.reset_pin);
    xtimer_usleep(AT86RF2XX_RESET_DELAY);
}

void at86rf2xx_configure_phy(at86rf2xx_t *dev)
{
    /* make sure device is not sleeping */
    at86rf2xx_assert_awake(dev);

    uint8_t state;

    /* make sure ongoing transmissions are finished */
    do {
        state = at86rf2xx_get_status(dev);
    }
    while ((state == AT86RF2XX_STATE_BUSY_TX_ARET) || (state == AT86RF2XX_STATE_BUSY_RX_AACK));

    /* we must be in TRX_OFF before changing the PHY configuration */
    at86rf2xx_force_trx_off(dev);

#ifdef MODULE_AT86RF212B
    /* The TX power register must be updated after changing the channel if
     * moving between bands. */
    int16_t txpower = at86rf2xx_get_txpower(dev);

    uint8_t trx_ctrl2 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_2);
    uint8_t rf_ctrl0 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RF_CTRL_0);

    /* Clear previous configuration for PHY mode */
    trx_ctrl2 &= ~(AT86RF2XX_TRX_CTRL_2_MASK__FREQ_MODE);
    /* Clear previous configuration for GC_TX_OFFS */
    rf_ctrl0 &= ~AT86RF2XX_RF_CTRL_0_MASK__GC_TX_OFFS;

    if (dev->netdev.chan != 0) {
        /* Set sub mode bit on 915 MHz as recommended by the data sheet */
        trx_ctrl2 |= AT86RF2XX_TRX_CTRL_2_MASK__SUB_MODE;
    }

    if (dev->page == 0) {
        /* BPSK coding */
        /* Data sheet recommends using a +2 dB setting for BPSK */
        rf_ctrl0 |= AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__2DB;
    }
    else if (dev->page == 2) {
        /* O-QPSK coding */
        trx_ctrl2 |= AT86RF2XX_TRX_CTRL_2_MASK__BPSK_OQPSK;
        /* Data sheet recommends using a +1 dB setting for O-QPSK */
        rf_ctrl0 |= AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__1DB;
    }

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2, trx_ctrl2);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RF_CTRL_0, rf_ctrl0);
#endif

    uint8_t phy_cc_cca = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    /* Clear previous configuration for channel number */
    phy_cc_cca &= ~(AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);

    /* Update the channel register */
    phy_cc_cca |= (dev->netdev.chan & AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, phy_cc_cca);

#ifdef MODULE_AT86RF212B
    /* Update the TX power register to achieve the same power (in dBm) */
    at86rf2xx_set_txpower(dev, txpower);
#endif

    /* Return to the state we had before reconfiguring */
    at86rf2xx_set_state(dev, state);
}

void at86rf2xx_force_trx_off(const at86rf2xx_t *dev)
{
    at86rf2xx_reg_write(dev,
                        AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__FORCE_TRX_OFF);
    while (at86rf2xx_get_status(dev) != AT86RF2XX_STATE_TRX_OFF);
}
