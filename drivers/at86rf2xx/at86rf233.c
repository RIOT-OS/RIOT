/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
 *               2019 OvGU Magdeburg
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
 * @brief       Implementation of AT86RF233 internal functions
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include "kernel_defines.h"
#if IS_USED(MODULE_AT86RF233)
#include "xtimer.h"
#include "at86rf2xx.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication_spi.h"
#include "at86rf233_internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

extern const int16_t _233_tx_pow_to_dbm[16];
extern const uint8_t _233_dbm_to_tx_pow[22];
extern const int16_t _233_rx_sens_to_dbm[16];
extern const uint8_t _233_dbm_to_rx_sens[50];

/**
 * @brief   Convert TX_PWR register value to actual transmission power in dbm
 *          for AT86RF233 transceivers
 */
static
int16_t at86rf233_tx_pow_to_dbm(const at86rf233_t *dev, uint8_t txpower)
{
    (void)dev;
    assert(txpower < ARRAY_SIZE(_233_tx_pow_to_dbm));
    return _233_tx_pow_to_dbm[txpower];
}

/**
 * @brief   Convert transmission power in dbm to TX_PWR register value
 *          for AT86RF233 transceivers
 */
static
uint8_t at86rf233_dbm_to_tx_pow(const at86rf233_t *dev, int16_t dbm)
{
    (void)dev;
    assert(dbm >= AT86RF233_TXPOWER_DBM_MIN &&
           dbm <= AT86RF233_TXPOWER_DBM_MAX);
    dbm += AT86RF233_TXPOWER_OFF;
    return _233_dbm_to_tx_pow[dbm];
}

/**
 * @brief   Convert RX_PDT_LEVEL register value to actual receiver sensibility in dbm
 *          for AT86RF233 transceivers
 */
static
int16_t at86rf233_rx_sens_to_dbm(const at86rf233_t *dev, uint8_t rxsens)
{
    (void)dev;
    assert(rxsens < ARRAY_SIZE(_233_rx_sens_to_dbm));
    return _233_rx_sens_to_dbm[rxsens];
}

/**
 * @brief   Convert receiver sensibility to RX_PDT_LEVEL register value
 *          for AT86RF233 transceivers
 */
static
uint8_t at86rf233_dbm_to_rxsens(const at86rf233_t *dev, int16_t dbm)
{
    (void)dev;
    assert(dbm >= AT86RF233_RX_SENSITIVITY_DBM_MIN &&
           dbm <= AT86RF233_RX_SENSITIVITY_DBM_MAX);
    dbm -= AT86RF233_RX_SENSITIVITY_DBM_MIN;
    return _233_dbm_to_rx_sens[dbm];
}

void at86rf233_setup(at86rf233_t *devs, const at86rf233_params_t *params,
                     uint8_t num)
{
    for (int i = 0; i < num; i++) {
        at86rf2xx_setup((at86rf2xx_t *)&devs[i]);
        devs[i].base.dev_type = AT86RF2XX_DEV_TYPE_AT86RF233;
        devs[i].params = params[i];
    }
}

void at86rf233_sleep(at86rf233_t *dev)
{
    if (dev->base.state != AT86RF2XX_STATE_SLEEP) {
        /* First go to TRX_OFF */
        if (dev->base.state != AT86RF2XX_STATE_TRX_OFF) {
            at86rf233_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
        }
        /* Discard all IRQ flags, framebuffer is lost anyway */
        at86rf2xx_spi_reg_read((at86rf2xx_t *)dev, AT86RF2XX_REG__IRQ_STATUS);
        gpio_set(dev->params.base_params.sleep_pin);
        dev->base.state = AT86RF2XX_STATE_SLEEP;
    }
}

void at86rf233_assert_awake(at86rf233_t *dev)
{
    if (dev->base.state == AT86RF2XX_STATE_SLEEP) {
        gpio_clear(dev->params.base_params.sleep_pin);
        xtimer_usleep(AT86RF233_WAKEUP_DELAY);

        /* update state: on some platforms, the timer behind xtimer
         * may be inaccurate or the radio itself may take longer
         * to wake up due to extra capacitance on the oscillator.
         * Spin until we are actually awake
         */
        do {
            dev->base.state =
                at86rf2xx_spi_reg_read((at86rf2xx_t *)dev,
                                       AT86RF2XX_REG__TRX_STATUS)
                & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
        } while (dev->base.state != AT86RF2XX_TRX_STATUS__TRX_OFF);
    }
}

void at86rf233_hardware_reset(at86rf233_t *dev)
{
    gpio_clear(dev->params.base_params.reset_pin);
    xtimer_usleep(AT86RF2XX_RESET_PULSE_WIDTH);
    gpio_set(dev->params.base_params.reset_pin);
    xtimer_usleep(AT86RF2XX_RESET_DELAY);
    assert(at86rf233_get_status(dev) == AT86RF2XX_STATE_TRX_OFF);
    dev->base.state = AT86RF2XX_STATE_TRX_OFF;
}

void at86rf233_reset(at86rf233_t *dev)
{
    /* soft reset */
    at86rf2xx_reset((at86rf2xx_t *)dev);

    /* reconfigure channel */
    at86rf233_set_channel(dev, AT86RF233_DEFAULT_CHANNEL);

    /* reconfigure TX power */
    at86rf233_set_txpower(dev, AT86RF2XX_DEFAULT_TXPOWER);

    /* disable clock output to save power */
    at86rf233_set_clock_output(dev, false, AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF);

    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rf233_set_frame_buffer_protection(dev, true);

    /* enable smart receive technology */
    at86rf233_enable_smart_idle(dev);

    /* enable and clear interrupts */
    at86rf2xx_spi_reg_write((at86rf2xx_t *)dev, AT86RF2XX_REG__IRQ_MASK,
                            AT86RF2XX_IRQ_MASK_MASK__TRX_END);
    at86rf233_set_irq_mode(dev, true, false);
    at86rf2xx_spi_reg_read((at86rf2xx_t *)dev, AT86RF2XX_REG__IRQ_STATUS);

    DEBUG("[at86rf233] reset complete.\n");
}

int16_t at86rf233_get_txpower(const at86rf233_t *dev)
{
    uint8_t phy_tx_pwr = at86rf2xx_spi_reg_read((at86rf2xx_t *)dev,
                                                AT86RF2XX_REG__PHY_TX_PWR);

    phy_tx_pwr &= AT86RF233_PHY_TX_PWR_MASK__TX_PWR;
    DEBUG("[at86rf233] txpower value: %x\n", phy_tx_pwr);
    return at86rf233_tx_pow_to_dbm(dev, phy_tx_pwr);
}

void at86rf233_set_txpower(at86rf233_t *dev, int16_t dbm)
{
    if (dbm < AT86RF233_TXPOWER_DBM_MIN) {
        dbm = AT86RF233_TXPOWER_DBM_MIN;
    }
    else if (dbm > AT86RF233_TXPOWER_DBM_MAX) {
        dbm = AT86RF233_TXPOWER_DBM_MAX;
    }
    uint8_t txpower = at86rf233_dbm_to_tx_pow(dev, dbm);
    DEBUG("[at86rf233] txpower value: %x\n", txpower);
    uint8_t phy_tx_pwr = at86rf2xx_spi_reg_read((at86rf2xx_t *)dev,
                                                AT86RF2XX_REG__PHY_TX_PWR);
    phy_tx_pwr &= ~(AT86RF233_PHY_TX_PWR_MASK__TX_PWR);
    phy_tx_pwr |= (txpower & AT86RF233_PHY_TX_PWR_MASK__TX_PWR);
    at86rf2xx_spi_reg_write((at86rf2xx_t *)dev, AT86RF2XX_REG__PHY_TX_PWR,
                            phy_tx_pwr);
    dev->base.netdev.txpower = dbm;
}

int16_t at86rf233_get_rxsensitivity(const at86rf233_t *dev)
{
    uint8_t rx_syn = at86rf2xx_spi_reg_read((at86rf2xx_t *)dev,
                                            AT86RF2XX_REG__RX_SYN)
                     & AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL;

    return at86rf233_rx_sens_to_dbm(dev, rx_syn);
}

void at86rf233_set_rxsensitivity(const at86rf233_t *dev, int16_t dbm)
{
    if (dbm < AT86RF233_RX_SENSITIVITY_DBM_MIN) {
        dbm = AT86RF233_RX_SENSITIVITY_DBM_MIN;
    }
    else if (dbm > AT86RF233_RX_SENSITIVITY_DBM_MAX) {
        dbm = AT86RF233_RX_SENSITIVITY_DBM_MAX;
    }
    uint8_t rxsens = at86rf233_dbm_to_rxsens(dev, dbm);
    DEBUG("[at86rf233] rxsens value: %x\n", rxsens);
    uint8_t rx_syn = at86rf2xx_spi_reg_read((at86rf2xx_t *)dev,
                                            AT86RF2XX_REG__RX_SYN);
    rx_syn &= ~(AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL);
    rx_syn |= (rxsens & AT86RF2XX_RX_SYN_MASK__RX_PDT_LEVEL);
    at86rf2xx_spi_reg_write((at86rf2xx_t *)dev, AT86RF2XX_REG__RX_SYN, rx_syn);
}

int8_t at86rf233_get_cca_threshold(const at86rf233_t *dev)
{
    uint8_t thresh = at86rf2xx_get_cca_threshold((const at86rf2xx_t *)dev);

    /* multiply by 2 because precesion is 2 dbm */
    return AT86RF233_RSSI_BASE_VAL + (thresh << 1);
}

void at86rf233_set_cca_threshold(const at86rf233_t *dev, int8_t thresh)
{
    /* ensure the given value is negative, since a CCA threshold > 0 is
       just impossible: thus, any positive value given is considered
       to be the absolute value of the actually wanted threshold */
    if (thresh > 0) {
        thresh = -thresh;
    }
    /* transform the dBm value in the form
       that will fit in the AT86RF2XX_REG__CCA_THRES register */
    thresh -= AT86RF233_RSSI_BASE_VAL;
    thresh >>= 1; /* divide by 2 because precesion is 2 dbm */
    thresh &= AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;

    return at86rf2xx_set_cca_threshold((const at86rf2xx_t *)dev,
                                       (uint8_t)thresh);
}

int8_t at86rf233_get_ed_level(const at86rf233_t *dev)
{
    uint8_t phy_ed_level =
        at86rf2xx_get_ed_level((const at86rf2xx_t *)dev);

    return (int8_t)(phy_ed_level + AT86RF233_RSSI_BASE_VAL);
}

void at86rf233_set_rx_tx_timestamping(const at86rf233_t *dev, bool timestamping)
{
    uint8_t xah_ctrl_1 = at86rf2xx_spi_reg_read((at86rf2xx_t *)dev,
                                                AT86RF2XX_REG__XAH_CTRL_1);

    /* high if receiving or transmitting (TX_ARET) */
    xah_ctrl_1 = (timestamping)
                 ? (xah_ctrl_1 | AT86RF2XX_XAH_CTRL_1_MASK__ARET_TX_TS_EN)
                 : (xah_ctrl_1 & ~(AT86RF2XX_XAH_CTRL_1_MASK__ARET_TX_TS_EN));

    at86rf2xx_spi_reg_write((at86rf2xx_t *)dev, AT86RF2XX_REG__TRX_CTRL_1,
                            xah_ctrl_1);
}

void at86rf233_set_channel(at86rf233_t *dev, uint8_t channel)
{
    if (channel < AT86RF233_CHANNEL_MIN) {
        channel = AT86RF233_CHANNEL_MIN;
    }
    else if (channel > AT86RF233_CHANNEL_MAX) {
        channel = AT86RF233_CHANNEL_MAX;
    }

    uint8_t prev_state = at86rf233_set_state(dev, AT86RF2XX_STATE_TRX_OFF);

    at86rf2xx_set_channel((at86rf2xx_t *)dev, channel);

    /* Return to the state we had before reconfiguring */
    at86rf233_set_state(dev, prev_state);
}

void at86rf233_configure_phy(at86rf233_t *dev, at86rf233_phy_mode_t mode)
{
    uint8_t trx_ctrl_2 = at86rf2xx_spi_reg_read((at86rf2xx_t *)dev,
                                                AT86RF2XX_REG__TRX_CTRL_2);

    uint8_t mode_mask = AT86RF2XX_TRX_CTRL_2_MASK__OQPSK_SCRAM_EN |
                        AT86RF233_TRX_CTRL_2_MASK__OQPSK_DATA_RATE;

    trx_ctrl_2 &= ~(mode_mask);
    trx_ctrl_2 |= (mode & mode_mask);

    at86rf2xx_spi_reg_write((at86rf2xx_t *)dev, AT86RF2XX_REG__TRX_CTRL_2,
                            trx_ctrl_2);
}

void at86rf233_enable_smart_idle(at86rf233_t *dev)
{
    uint8_t trc_rpc = at86rf2xx_spi_reg_read((at86rf2xx_t *)dev,
                                             AT86RF2XX_REG__TRX_RPC);

    trc_rpc |= (AT86RF2XX_TRX_RPC_MASK__RX_RPC_EN |
                AT86RF2XX_TRX_RPC_MASK__PDT_RPC_EN |
                AT86RF2XX_TRX_RPC_MASK__PLL_RPC_EN |
                AT86RF2XX_TRX_RPC_MASK__XAH_TX_RPC_EN |
                AT86RF2XX_TRX_RPC_MASK__IPAN_RPC_EN);
    at86rf2xx_spi_reg_write((at86rf2xx_t *)dev, AT86RF2XX_REG__TRX_RPC,
                            trc_rpc);
    at86rf233_set_rxsensitivity(dev, AT86RF233_RSSI_BASE_VAL);
}

uint8_t at86rf233_fb_read_phr(const at86rf233_t *dev)
{
    uint8_t phr;

    at86rf2xx_spi_sram_read((at86rf2xx_t *)dev, 0, &phr, 1);
    return phr;
}

ssize_t at86rf233_fb_read(const at86rf233_t *dev, at86rf233_fb_t *fb,
                          void *buf, size_t buf_size)
{
    at86rf2xx_spi_fb_start((at86rf2xx_t *)dev);
    at86rf2xx_spi_fb_read((at86rf2xx_t *)dev, &fb->phr, 1);
    int payload_length = (fb->phr & 0x7F) - IEEE802154_FCS_LEN;
    if (payload_length > 0) {
        if ((size_t)payload_length > buf_size) {
            return -ENOBUFS;
        }
        at86rf2xx_spi_fb_read((at86rf2xx_t *)dev, buf, payload_length);
        fb->payload = buf;
        at86rf2xx_spi_fb_read((at86rf2xx_t *)dev, &fb->fcs, 2);
        at86rf2xx_spi_fb_read((at86rf2xx_t *)dev, &fb->lqi, 1);
        at86rf2xx_spi_fb_read((at86rf2xx_t *)dev, &fb->ed, 1);
        at86rf2xx_spi_fb_read((at86rf2xx_t *)dev, &fb->rx_status, 1);
    }
    at86rf2xx_spi_fb_stop((at86rf2xx_t *)dev);
    return payload_length;
}
#endif /* IS_USED(MODULE_AT86RF233) */
