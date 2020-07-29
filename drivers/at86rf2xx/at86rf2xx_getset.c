/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
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
 * @brief       Getter and setter functions for the AT86RF2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <string.h>

#include "at86rf2xx.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"
#include "periph/spi.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if MODULE_AT86RF233
static const int16_t tx_pow_to_dbm[] = { 4, 4, 3, 3, 2, 2, 1,
                                         0, -1, -2, -3, -4, -6, -8, -12, -17 };
static const uint8_t dbm_to_tx_pow[] = { 0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                         0x0e, 0x0d, 0x0d, 0x0d, 0x0c, 0x0c,
                                         0x0b, 0x0b, 0x0a, 0x09, 0x08, 0x07,
                                         0x06, 0x05, 0x03, 0x00 };
static const int16_t rx_sens_to_dbm[] = { -101, -94, -91, -88, -85, -82, -79,
                                          -76, -73, -70, -67, -64, -61, -58, -55,
                                          -52 };
static const uint8_t dbm_to_rx_sens[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x01, 0x01, 0x01, 0x02, 0x02,
                                          0x02, 0x03, 0x03, 0x03, 0x04, 0x04,
                                          0x04, 0x05, 0x05, 0x05, 0x06, 0x06,
                                          0x06, 0x07, 0x07, 0x07, 0x08, 0x08,
                                          0x08, 0x09, 0x09, 0x09, 0x0a, 0x0a,
                                          0x0a, 0x0b, 0x0b, 0x0b, 0x0c, 0x0c,
                                          0x0c, 0x0d, 0x0d, 0x0d, 0x0e, 0x0e,
                                          0x0e, 0x0f };
#else
static const int16_t tx_pow_to_dbm[] = { 3, 3, 2, 2, 1, 1, 0,
                                         -1, -2, -3, -4, -5, -7, -9, -12, -17 };
static const uint8_t dbm_to_tx_pow[] = { 0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                         0x0e, 0x0d, 0x0d, 0x0c, 0x0c, 0x0b,
                                         0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06,
                                         0x05, 0x03, 0x00 };
static const int16_t rx_sens_to_dbm[] = { -101, -91, -88, -85, -82, -79, -76
                                          -73, -70, -67, -64, -61, -58, -55, -52,
                                          -49 };
static const uint8_t dbm_to_rx_sens[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                          0x01, 0x02, 0x02, 0x02, 0x03, 0x03,
                                          0x03, 0x04, 0x04, 0x04, 0x05, 0x05,
                                          0x05, 0x06, 0x06, 0x06, 0x07, 0x07,
                                          0x07, 0x08, 0x08, 0x08, 0x09, 0x09,
                                          0x09, 0x0a, 0x0a, 0x0a, 0x0b, 0x0b,
                                          0x0b, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d,
                                          0x0d, 0x0e, 0x0e, 0x0e, 0x0f };
#endif

int16_t at86rf2xx_get_txpower(const at86rf2xx_t *dev)
{
    uint8_t txpower = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_TX_PWR)
                      & AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR;
    return tx_pow_to_dbm[txpower];
}

void at86rf2xx_set_txpower(const at86rf2xx_t *dev, int16_t txpower)
{
    txpower += AT86RF2XX_TXPOWER_OFF;

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR,
                        dbm_to_tx_pow[txpower]);
}

int16_t at86rf2xx_get_rxsensitivity(const at86rf2xx_t *dev)
{
    uint8_t rxsens = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN)
                     & AT86RF2XX_RX_SYN__RX_PDT_LEVEL;
    return rx_sens_to_dbm[rxsens];
}

void at86rf2xx_set_rxsensitivity(const at86rf2xx_t *dev, int16_t rxsens)
{
    rxsens += MIN_RX_SENSITIVITY;

    if (rxsens < 0) {
        rxsens = 0;
    }
    else if (rxsens > MAX_RX_SENSITIVITY) {
        rxsens = MAX_RX_SENSITIVITY;
    }

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN);
    tmp &= ~(AT86RF2XX_RX_SYN__RX_PDT_LEVEL);
    tmp |= (dbm_to_rx_sens[rxsens] & AT86RF2XX_RX_SYN__RX_PDT_LEVEL);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RX_SYN, tmp);
}

uint8_t at86rf2xx_get_max_retries(const at86rf2xx_t *dev)
{
    return (at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0) >> 4);
}

void at86rf2xx_set_max_retries(const at86rf2xx_t *dev, uint8_t max)
{
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);

    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_FRAME_RETRIES);
    tmp |= ((max > 7) ? 7 : max) << 4;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);
}

uint8_t at86rf2xx_get_csma_max_retries(const at86rf2xx_t *dev)
{
    uint8_t tmp;

    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES;
    tmp >>= 1;
    return tmp;
}

void at86rf2xx_set_csma_max_retries(const at86rf2xx_t *dev, int8_t retries)
{
    retries = (retries > 5) ? 5 : retries;  /* valid values: 0-5 */
    retries = (retries < 0) ? 7 : retries;  /* max < 0 => disable CSMA (set to 7) */
    DEBUG("[at86rf2xx] opt: Set CSMA retries to %u\n", retries);

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES);
    tmp |= (retries << 1);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);
}

void at86rf2xx_set_csma_backoff_exp(const at86rf2xx_t *dev,
                                    uint8_t min, uint8_t max)
{
    max = (max > 8) ? 8 : max;
    min = (min > max) ? max : min;
    DEBUG("[at86rf2xx] opt: Set min BE=%u, max BE=%u\n", min, max);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_BE, (max << 4) | (min));
}

void at86rf2xx_set_csma_seed(const at86rf2xx_t *dev, const uint8_t entropy[2])
{
    if (entropy == NULL) {
        DEBUG("[at86rf2xx] opt: CSMA seed entropy is nullpointer\n");
        return;
    }
    DEBUG("[at86rf2xx] opt: Set CSMA seed to 0x%x 0x%x\n", entropy[0], entropy[1]);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_0, entropy[0]);

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
    tmp &= ~(AT86RF2XX_CSMA_SEED_1__CSMA_SEED_1);
    tmp |= entropy[1] & AT86RF2XX_CSMA_SEED_1__CSMA_SEED_1;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
}

int8_t at86rf2xx_get_cca_threshold(const at86rf2xx_t *dev)
{
    int8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CCA_THRES);

    tmp &= AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;
    tmp <<= 1;
    return (RSSI_BASE_VAL + tmp);
}

void at86rf2xx_set_cca_threshold(const at86rf2xx_t *dev, int8_t value)
{
    /* transform the dBm value in the form
       that will fit in the AT86RF2XX_REG__CCA_THRES register */
    value -= RSSI_BASE_VAL;
    value >>= 1;
    value &= AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CCA_THRES, value);
}

int8_t at86rf2xx_get_ed_level(at86rf2xx_t *dev)
{
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_ED_LEVEL);

    int8_t ed = (int8_t)tmp + RSSI_BASE_VAL;
    return ed;
}

void at86rf2xx_set_promiscuous(at86rf2xx_t *dev, bool enable)
{
    uint8_t tmp;
    DEBUG("[at86rf2xx] opt: %s PROMISCUOUS mode\n",
          (enable ? "enable" : "disable"));
    /* enable/disable promiscuous mode */
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_1);
    tmp = (enable) ? (tmp |  AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE)
                  : (tmp & ~AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_1, tmp);
}

void at86rf2xx_set_auto_ack(at86rf2xx_t *dev, bool enable)
{
    uint8_t tmp;
    DEBUG("[at86rf2xx] opt: %s auto ACKs\n",
          (enable ? "enable" : "disable"));
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
    tmp = (enable) ? (tmp & ~AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK)
                  : (tmp |  AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
}

void at86rf2xx_set_frame_pending(at86rf2xx_t *dev, bool pending)
{
    uint8_t tmp;
    DEBUG("[at86rf2xx] opt: enabling pending ACKs\n");
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
    tmp = (pending) ? (tmp |  AT86RF2XX_CSMA_SEED_1__AACK_SET_PD)
                  : (tmp & ~AT86RF2XX_CSMA_SEED_1__AACK_SET_PD);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
}

void at86rf2xx_sleep(at86rf2xx_t *dev)
{
    at86rf2xx_set_internal_state(dev, AT86RF2XX_STATE_FORCE_TRX_OFF);
    at86rf2xx_wait_trx_state_finish(dev);
    /* Discard all IRQ flags, framebuffer is lost anyway */
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);
    /* Go to SLEEP mode from TRX_OFF */
    gpio_set(dev->params.sleep_pin);
}
