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

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_AT86RF212B
/* See: Table 9-15. Recommended Mapping of TX Power, Frequency Band, and
 * PHY_TX_PWR (register 0x05), AT86RF212B data sheet. */
static const uint8_t dbm_to_tx_pow_868[] = { 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18,
                                             0x17, 0x15, 0x14, 0x13, 0x12, 0x11,
                                             0x10, 0x0f, 0x31, 0x30, 0x2f, 0x94,
                                             0x93, 0x91, 0x90, 0x29, 0x49, 0x48,
                                             0x47, 0xad, 0xcd, 0xcc, 0xcb, 0xea,
                                             0xe9, 0xe8, 0xe7, 0xe6, 0xe4, 0x80,
                                             0xa0 };
static const uint8_t dbm_to_tx_pow_915[] = { 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x17,
                                             0x16, 0x15, 0x14, 0x13, 0x12, 0x11,
                                             0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b,
                                             0x09, 0x91, 0x08, 0x07, 0x05, 0x27,
                                             0x04, 0x03, 0x02, 0x01, 0x00, 0x86,
                                             0x40, 0x84, 0x83, 0x82, 0x80, 0xc1,
                                             0xc0 };
static int16_t _tx_pow_to_dbm_212b(uint8_t channel, uint8_t page, uint8_t reg)
{
    if (page == 0 || page == 2) {
        const uint8_t *dbm_to_tx_pow;
        size_t nelem;

        if (channel == 0) {
            /* Channel 0 is 868.3 MHz */
            dbm_to_tx_pow = &dbm_to_tx_pow_868[0];
            nelem = ARRAY_SIZE(dbm_to_tx_pow_868);
        }
        else {
            /* Channels 1+ are 915 MHz */
            dbm_to_tx_pow = &dbm_to_tx_pow_915[0];
            nelem = ARRAY_SIZE(dbm_to_tx_pow_915);
        }

        for (size_t i = 0; i < nelem; ++i) {
            if (dbm_to_tx_pow[i] == reg) {
                return (i - AT86RF2XX_TXPOWER_OFF);
            }
        }
    }

    return 0;
}

#elif MODULE_AT86RF233
static const int16_t tx_pow_to_dbm[] = { 4, 4, 3, 3, 2, 2, 1,
                                         0, -1, -2, -3, -4, -6, -8, -12, -17 };
static const uint8_t dbm_to_tx_pow[] = { 0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                         0x0e, 0x0d, 0x0d, 0x0d, 0x0c, 0x0c,
                                         0x0b, 0x0b, 0x0a, 0x09, 0x08, 0x07,
                                         0x06, 0x05, 0x03, 0x00 };
#else
static const int16_t tx_pow_to_dbm[] = { 3, 3, 2, 2, 1, 1, 0,
                                         -1, -2, -3, -4, -5, -7, -9, -12, -17 };
static const uint8_t dbm_to_tx_pow[] = { 0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                         0x0e, 0x0d, 0x0d, 0x0c, 0x0c, 0x0b,
                                         0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06,
                                         0x05, 0x03, 0x00 };
#endif

void at86rf2xx_get_addr_short(const at86rf2xx_t *dev, network_uint16_t *addr)
{
    memcpy(addr, dev->netdev.short_addr, sizeof(*addr));
}

void at86rf2xx_set_addr_short(at86rf2xx_t *dev, const network_uint16_t *addr)
{
    memcpy(dev->netdev.short_addr, addr, sizeof(*addr));
#ifdef MODULE_SIXLOWPAN
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    dev->netdev.short_addr[0] &= 0x7F;
#endif
    /* device use lsb first, not network byte order */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_0,
                        dev->netdev.short_addr[1]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_1,
                        dev->netdev.short_addr[0]);
}

void at86rf2xx_get_addr_long(const at86rf2xx_t *dev, eui64_t *addr)
{
    memcpy(addr, dev->netdev.long_addr, sizeof(*addr));
}

void at86rf2xx_set_addr_long(at86rf2xx_t *dev, const eui64_t *addr)
{
    memcpy(dev->netdev.long_addr, addr, sizeof(*addr));
    for (int i = 0; i < 8; i++) {
        /* device use lsb first, not network byte order */
        at86rf2xx_reg_write(dev, (AT86RF2XX_REG__IEEE_ADDR_0 + i),
                dev->netdev.long_addr[IEEE802154_LONG_ADDRESS_LEN - 1 - i]);
    }
}

uint8_t at86rf2xx_get_chan(const at86rf2xx_t *dev)
{
    return dev->netdev.chan;
}

void at86rf2xx_set_chan(at86rf2xx_t *dev, uint8_t channel)
{
    if ((channel > AT86RF2XX_MAX_CHANNEL)
#if AT86RF2XX_MIN_CHANNEL /* is zero for sub-GHz */
       || (channel < AT86RF2XX_MIN_CHANNEL)
#endif
        ) {
        return;
    }

    dev->netdev.chan = channel;

    at86rf2xx_configure_phy(dev);
}

uint8_t at86rf2xx_get_page(const at86rf2xx_t *dev)
{
#ifdef MODULE_AT86RF212B
    return dev->page;
#else
    (void) dev;
    return 0;
#endif
}

void at86rf2xx_set_page(at86rf2xx_t *dev, uint8_t page)
{
#ifdef MODULE_AT86RF212B
    if ((page == 0) || (page == 2)) {
        dev->page = page;
        at86rf2xx_configure_phy(dev);
    }
#else
    (void) dev;
    (void) page;
#endif
}

uint8_t at86rf2xx_get_phy_mode(at86rf2xx_t *dev)
{
#ifdef MODULE_AT86RF212B
    uint8_t ctrl2;
    ctrl2 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_2);
    if (ctrl2 & AT86RF2XX_TRX_CTRL_2_MASK__BPSK_OQPSK) {
        return IEEE802154_PHY_OQPSK;
    } else {
        return IEEE802154_PHY_BPSK;
    }
#else
    (void) dev;
    return IEEE802154_PHY_OQPSK;
#endif
}

int at86rf2xx_set_rate(at86rf2xx_t *dev, uint8_t rate)
{
    uint8_t ctrl2;

    if (rate > 3) {
        return -ERANGE;
    }

    ctrl2 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_2);
    ctrl2 &= ~AT86RF2XX_TRX_CTRL_2_MASK__OQPSK_DATA_RATE;
    ctrl2 |= rate;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2, ctrl2);

    return 0;
}

uint8_t at86rf2xx_get_rate(at86rf2xx_t *dev)
{
    uint8_t rate;

    rate = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_2);
    rate &= AT86RF2XX_TRX_CTRL_2_MASK__OQPSK_DATA_RATE;

    return rate;
}

uint16_t at86rf2xx_get_pan(const at86rf2xx_t *dev)
{
    return dev->netdev.pan;
}

void at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan)
{
    le_uint16_t le_pan = byteorder_htols(pan);

    DEBUG("pan0: %u, pan1: %u\n", le_pan.u8[0], le_pan.u8[1]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_0, le_pan.u8[0]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_1, le_pan.u8[1]);
}

int16_t at86rf2xx_get_txpower(const at86rf2xx_t *dev)
{
#ifdef MODULE_AT86RF212B
    uint8_t txpower = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_TX_PWR);
    DEBUG("txpower value: %x\n", txpower);
    return _tx_pow_to_dbm_212b(dev->netdev.chan, dev->page, txpower);
#else
    uint8_t txpower = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_TX_PWR)
                      & AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR;
    return tx_pow_to_dbm[txpower];
#endif
}

void at86rf2xx_set_txpower(const at86rf2xx_t *dev, int16_t txpower)
{
    txpower += AT86RF2XX_TXPOWER_OFF;

    if (txpower < 0) {
        txpower = 0;
    }
    else if (txpower > AT86RF2XX_TXPOWER_MAX) {
        txpower = AT86RF2XX_TXPOWER_MAX;
    }
#ifdef MODULE_AT86RF212B
    if (dev->netdev.chan == 0) {
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR,
                            dbm_to_tx_pow_868[txpower]);
    }
    else if (dev->netdev.chan < 11) {
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR,
                            dbm_to_tx_pow_915[txpower]);
    }
#else
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR,
                        dbm_to_tx_pow[txpower]);
#endif
}

int8_t at86rf2xx_get_rxsensitivity(const at86rf2xx_t *dev)
{
    uint8_t rxsens = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN)
                     & AT86RF2XX_RX_SYN__RX_PDT_LEVEL;
    /* From datasheet (see below) */
    return rxsens > 0 ? RSSI_BASE_VAL + ((rxsens - 1) * 3) : MIN_RX_SENSITIVITY;
}

void at86rf2xx_set_rxsensitivity(const at86rf2xx_t *dev, int8_t rxsens)
{
    uint8_t hwval;
    /* From datasheet, rxsens = @ref RSSI_BASE_VAL + (3 * (RX_PDT_LEVEL-1)).
     * If rxsens < @ref RSSI_BASE_VAL, the RX sensitivity is equal to @ref
     * MIN_RX_SENSITIVITY.
     */
    if (rxsens < RSSI_BASE_VAL) {
        hwval = 0;
    }
    else if (rxsens > RSSI_BASE_VAL + (3 * (AT86RF2XX_RX_SYN__RX_PDT_LEVEL)) - 1) {
        hwval = AT86RF2XX_RX_SYN__RX_PDT_LEVEL;
    }
    else {
        hwval = ((rxsens - RSSI_BASE_VAL + 3) / 3);
    }

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN);
    tmp &= ~(AT86RF2XX_RX_SYN__RX_PDT_LEVEL);
    tmp |= (hwval & AT86RF2XX_RX_SYN__RX_PDT_LEVEL);
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
    /* ensure the given value is negative, since a CCA threshold > 0 is
       just impossible: thus, any positive value given is considered
       to be the absolute value of the actually wanted threshold */
    if (value > 0) {
        value = -value;
    }
    /* transform the dBm value in the form
       that will fit in the AT86RF2XX_REG__CCA_THRES register */
    value -= RSSI_BASE_VAL;
    value >>= 1;
    value &= AT86RF2XX_CCA_THRES_MASK__CCA_ED_THRES;
    value |= AT86RF2XX_CCA_THRES_MASK__RSVD_HI_NIBBLE;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CCA_THRES, value);
}

int8_t at86rf2xx_get_ed_level(at86rf2xx_t *dev)
{
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_ED_LEVEL);

#if MODULE_AT86RF212B
    /* AT86RF212B has different scale than the other variants */
    int8_t ed = (int8_t)(((int16_t)tmp * 103) / 100) + RSSI_BASE_VAL;
#else
    int8_t ed = (int8_t)tmp + RSSI_BASE_VAL;
#endif
    return ed;
}

void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state)
{
    uint8_t tmp;

    DEBUG("set option %i to %i\n", option, state);

    /* set option field */
    dev->flags = (state) ? (dev->flags |  option)
                         : (dev->flags & ~option);
    /* trigger option specific actions */
    switch (option) {
        case AT86RF2XX_OPT_CSMA:
            if (state) {
                DEBUG("[at86rf2xx] opt: enabling CSMA mode" \
                      "(4 retries, min BE: 3 max BE: 5)\n");
                /* Initialize CSMA seed with hardware address */
                at86rf2xx_set_csma_seed(dev, dev->netdev.long_addr);
                at86rf2xx_set_csma_max_retries(dev, 4);
                at86rf2xx_set_csma_backoff_exp(dev, 3, 5);
            }
            else {
                DEBUG("[at86rf2xx] opt: disabling CSMA mode\n");
                /* setting retries to -1 means CSMA disabled */
                at86rf2xx_set_csma_max_retries(dev, -1);
            }
            break;
        case AT86RF2XX_OPT_PROMISCUOUS:
            DEBUG("[at86rf2xx] opt: %s PROMISCUOUS mode\n",
                  (state ? "enable" : "disable"));
            /* disable/enable auto ACKs in promiscuous mode */
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            tmp = (state) ? (tmp |  AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK)
                          : (tmp & ~AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
            /* enable/disable promiscuous mode */
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_1);
            tmp = (state) ? (tmp |  AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE)
                          : (tmp & ~AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_1, tmp);
            break;
        case AT86RF2XX_OPT_AUTOACK:
            DEBUG("[at86rf2xx] opt: %s auto ACKs\n",
                  (state ? "enable" : "disable"));
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            tmp = (state) ? (tmp & ~AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK)
                          : (tmp |  AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
            break;
        case AT86RF2XX_OPT_ACK_PENDING:
            DEBUG("[at86rf2xx] opt: enabling pending ACKs\n");
            tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            tmp = (state) ? (tmp |  AT86RF2XX_CSMA_SEED_1__AACK_SET_PD)
                          : (tmp & ~AT86RF2XX_CSMA_SEED_1__AACK_SET_PD);
            at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
            break;
        default:
            /* do nothing */
            break;
    }
}

/**
 * @brief Internal function to change state
 * @details For all cases but AT86RF2XX_STATE_FORCE_TRX_OFF state and
 *          cmd parameter are the same.
 *
 * @param dev       device to operate on
 * @param state     target state
 * @param cmd       command to initiate state transition
 */

static inline void _set_state(at86rf2xx_t *dev, uint8_t state, uint8_t cmd)
{
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, cmd);

    /* To prevent a possible race condition when changing to
     * RX_AACK_ON state the state doesn't get read back in that
     * case. See discussion
     * in https://github.com/RIOT-OS/RIOT/pull/5244
     */
    if (state != AT86RF2XX_STATE_RX_AACK_ON) {
        while (at86rf2xx_get_status(dev) != state) {}
    }
    /* Although RX_AACK_ON state doesn't get read back,
     * at least make sure if state transition is in progress or not
     */
    else {
        while (at86rf2xx_get_status(dev) == AT86RF2XX_STATE_IN_PROGRESS) {}
    }

    dev->state = state;
}

uint8_t at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state)
{
    uint8_t old_state;

    /* make sure there is no ongoing transmission, or state transition already
     * in progress */
    do {
        old_state = at86rf2xx_get_status(dev);
    } while (old_state == AT86RF2XX_STATE_BUSY_RX_AACK ||
             old_state == AT86RF2XX_STATE_BUSY_TX_ARET ||
             old_state == AT86RF2XX_STATE_BUSY_RX      ||
             old_state == AT86RF2XX_STATE_BUSY_TX      ||
             old_state == AT86RF2XX_STATE_IN_PROGRESS);

    if (state == AT86RF2XX_STATE_FORCE_TRX_OFF) {
        _set_state(dev, AT86RF2XX_STATE_TRX_OFF, state);
    }
    else if (state != old_state) {
        /* we need to go via PLL_ON if we are moving between RX_AACK_ON <-> TX_ARET_ON */
        if ((old_state == AT86RF2XX_STATE_RX_AACK_ON &&
             state == AT86RF2XX_STATE_TX_ARET_ON) ||
            (old_state == AT86RF2XX_STATE_TX_ARET_ON &&
             state == AT86RF2XX_STATE_RX_AACK_ON)) {
            _set_state(dev, AT86RF2XX_STATE_PLL_ON, AT86RF2XX_STATE_PLL_ON);
        }
        /* check if we need to wake up from sleep mode */
        if (state == AT86RF2XX_STATE_SLEEP) {
            /* First go to TRX_OFF */
            _set_state(dev, AT86RF2XX_STATE_TRX_OFF,
                       AT86RF2XX_STATE_FORCE_TRX_OFF);
            /* Discard all IRQ flags, framebuffer is lost anyway */
            at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);
            /* Go to SLEEP mode from TRX_OFF */
#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
            /* reset interrupts states in device */
            dev->irq_status = 0;
            /* Setting SLPTR bit brings radio transceiver to sleep in in TRX_OFF*/
            *AT86RF2XX_REG__TRXPR |= (AT86RF2XX_TRXPR_SLPTR);
#else
            gpio_set(dev->params.sleep_pin);
#endif
            dev->state = state;
        }
        else {
            if (old_state == AT86RF2XX_STATE_SLEEP) {
                DEBUG("at86rf2xx: waking up from sleep mode\n");
                at86rf2xx_assert_awake(dev);
            }
            _set_state(dev, state, state);
        }
    }

    return old_state;
}
