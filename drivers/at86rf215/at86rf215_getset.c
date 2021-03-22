/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Getter and setter functions for the AT86RF215 driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <string.h>

#include "at86rf215.h"
#include "at86rf215_internal.h"
#include "periph/spi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* we can still go +3 dBm higher by increasing PA current */
#define PAC_DBM_MIN                             (-31)     /* dBm */

uint16_t at86rf215_get_addr_short(const at86rf215_t *dev, uint8_t filter)
{
    if (filter > 3) {
        return 0;
    }

    /* Each frame filter has a 2 byte short addr + 2 byte PAN ID, so we have to
       skip 4 bytes per filter */
    return ntohs(at86rf215_reg_read16(dev, dev->BBC->RG_MACSHA0F0 + (4 * filter)));
}

void at86rf215_set_addr_short(at86rf215_t *dev, uint8_t filter, uint16_t addr)
{
    /* do not overwrite node address for filters other than 0 */
    if (filter == 0) {
        dev->netdev.short_addr[0] = (uint8_t)(addr);
        dev->netdev.short_addr[1] = (uint8_t)(addr >> 8);
    }

    if (filter > 3) {
        return;
    }

    at86rf215_reg_write16(dev, dev->BBC->RG_MACSHA0F0 + (4 * filter), htons(addr));
}

bool at86rf215_get_framefilter_enabled(at86rf215_t *dev, uint8_t filter)
{
    return (at86rf215_reg_read(dev, dev->BBC->RG_AFC0) >> (AFC0_AFEN0_SHIFT + filter)) & 1;
}

void at86rf215_enable_framefilter(at86rf215_t *dev, uint8_t filter)
{
    at86rf215_reg_or(dev, dev->BBC->RG_AFC0, 1 << (AFC0_AFEN0_SHIFT + filter));
}

void at86rf215_disable_framefilter(at86rf215_t *dev, uint8_t filter)
{
    at86rf215_reg_and(dev, dev->BBC->RG_AFC0, 1 << (AFC0_AFEN0_SHIFT + filter));
}

uint64_t at86rf215_get_addr_long(const at86rf215_t *dev)
{
    uint64_t addr;
    at86rf215_reg_read_bytes(dev, dev->BBC->RG_MACEA0, &addr, sizeof(addr));
    return addr;
}

void at86rf215_set_addr_long(at86rf215_t *dev, uint64_t addr)
{
    memcpy(dev->netdev.long_addr, &addr, sizeof(addr));
    addr = ntohll(addr);
    at86rf215_reg_write_bytes(dev, dev->BBC->RG_MACEA0, &addr, sizeof(addr));
}

uint8_t at86rf215_get_chan(const at86rf215_t *dev)
{
    return at86rf215_reg_read16(dev, dev->RF->RG_CNL);
}

void at86rf215_set_trim(at86rf215_t *dev, uint8_t trim)
{
    assert(trim <= 0xF);
    at86rf215_reg_write(dev, RG_RF_XOC, trim | XOC_FS_MASK);
}

void at86rf215_set_clock_output(at86rf215_t *dev,
                                at86rf215_clko_cur_t cur, at86rf215_clko_freq_t freq)
{
    at86rf215_reg_write(dev, RG_RF_CLKO, cur | freq);
}

void at86rf215_set_chan(at86rf215_t *dev, uint16_t channel)
{
    at86rf215_await_state_end(dev, RF_STATE_TX);

    uint8_t old_state = at86rf215_get_rf_state(dev);

    /* frequency has to be updated in TRXOFF or TXPREP (datatsheet: 6.3.2) */
    if (old_state == RF_STATE_RX) {
        at86rf215_rf_cmd(dev, CMD_RF_TXPREP);
    }

    at86rf215_reg_write16(dev, dev->RF->RG_CNL, channel);
    dev->netdev.chan = channel;

    /* enable the radio again */
    if (old_state == RF_STATE_RX) {
        at86rf215_rf_cmd(dev, old_state);
    }
}

uint16_t at86rf215_get_channel_spacing(at86rf215_t *dev) {
    /* 25 kHz resolution */
    return 25 * at86rf215_reg_read(dev, dev->RF->RG_CS);
}

uint8_t at86rf215_get_phy_mode(at86rf215_t *dev)
{
    switch (at86rf215_reg_read(dev, dev->BBC->RG_PC) & PC_PT_MASK) {
    case 0x1: return IEEE802154_PHY_MR_FSK;
    case 0x2: return IEEE802154_PHY_MR_OFDM;
    case 0x3: return at86rf215_OQPSK_is_legacy(dev)
                ? IEEE802154_PHY_OQPSK
                : IEEE802154_PHY_MR_OQPSK;
    default:  return IEEE802154_PHY_DISABLED;
    }
}

uint16_t at86rf215_get_pan(const at86rf215_t *dev, uint8_t filter)
{
    if (filter > 3) {
        return 0;
    }

    return at86rf215_reg_read16(dev, dev->BBC->RG_MACPID0F0 + (4 * filter));
}

void at86rf215_set_pan(at86rf215_t *dev, uint8_t filter, uint16_t pan)
{
    if (filter == 0) {
        dev->netdev.pan = pan;
    }

    if (filter > 3) {
        return;
    }

    at86rf215_reg_write16(dev, dev->BBC->RG_MACPID0F0 + (4 * filter), pan);
}

// TODO: take modulation into account
int16_t at86rf215_get_txpower(const at86rf215_t *dev)
{
    uint8_t pac = at86rf215_reg_read(dev, dev->RF->RG_PAC);

    /* almost linear, each PACUR step adds ~1 dBm */
    return PAC_DBM_MIN + (pac & PAC_TXPWR_MASK) +
           ((pac & PAC_PACUR_MASK) >> PAC_PACUR_SHIFT);
}

// TODO: take modulation into account
void at86rf215_set_txpower(const at86rf215_t *dev, int16_t txpower)
{
    uint8_t pacur = 0;

    txpower -= PAC_DBM_MIN;

    if (txpower < 0) {
            txpower = 0;
    }

    if (txpower > PAC_TXPWR_MASK) {
        switch (txpower - PAC_TXPWR_MASK) {
            case 1:
                pacur = 1 << PAC_PACUR_SHIFT;
                break;
            case 2:
                pacur = 2 << PAC_PACUR_SHIFT;
                break;
            default:
                pacur = 3 << PAC_PACUR_SHIFT;
                break;
        }

        txpower = PAC_TXPWR_MASK;
    }

    at86rf215_reg_write(dev, dev->RF->RG_PAC, pacur | txpower);
}

int8_t at86rf215_get_cca_threshold(const at86rf215_t *dev)
{
    return dev->csma_ed;
}

void at86rf215_set_cca_threshold(at86rf215_t *dev, int8_t value)
{
    dev->csma_ed = value;
    at86rf215_reg_write(dev, dev->BBC->RG_AMEDT, value);
}

int8_t at86rf215_get_ed_level(at86rf215_t *dev)
{
    return at86rf215_reg_read(dev, dev->RF->RG_EDV);
}

void at86rf215_enable_rpc(at86rf215_t *dev)
{
    if (!(dev->flags & AT86RF215_OPT_RPC) || !CONFIG_AT86RF215_RPC_EN) {
        return;
    }

    /* no Reduced Power mode available for OFDM */

#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
    if (dev->fsk_pl) {
        /* MR-FSK */
        at86rf215_reg_or(dev, dev->BBC->RG_FSKRPC, FSKRPC_EN_MASK);
        return;
    }
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
    {
        /* MR-O-QPSK */
        at86rf215_reg_or(dev, dev->BBC->RG_OQPSKC2, OQPSKC2_RPC_MASK);
    }
#endif
}

void at86rf215_disable_rpc(at86rf215_t *dev)
{
    if (!(dev->flags & AT86RF215_OPT_RPC) || !CONFIG_AT86RF215_RPC_EN) {
        return;
    }

    /* no Reduced Power mode available for OFDM */

#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
    if (dev->fsk_pl) {
        /* MR-FSK */
        at86rf215_reg_and(dev, dev->BBC->RG_FSKRPC, ~FSKRPC_EN_MASK);
        return;
    }
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
    {
        /* MR-O-QPSK */
        at86rf215_reg_and(dev, dev->BBC->RG_OQPSKC2, ~OQPSKC2_RPC_MASK);
    }
#endif
}

void at86rf215_set_option(at86rf215_t *dev, uint16_t option, bool state)
{
    /* set option field */
    dev->flags = (state) ? (dev->flags |  option)
                         : (dev->flags & ~option);

    switch (option) {
        case AT86RF215_OPT_PROMISCUOUS:
            if (state) {
                at86rf215_reg_or(dev, dev->BBC->RG_AFC0, AFC0_PM_MASK);
            } else {
                at86rf215_reg_and(dev, dev->BBC->RG_AFC0, ~AFC0_PM_MASK);
            }

            break;
        case AT86RF215_OPT_AUTOACK:
            if (state) {
                at86rf215_reg_or(dev, dev->BBC->RG_AMCS, AMCS_AACK_MASK);
            } else {
                at86rf215_reg_and(dev, dev->BBC->RG_AMCS, ~AMCS_AACK_MASK);
            }

            break;
        case AT86RF215_OPT_CCATX:
            if (state){
                at86rf215_reg_or(dev, dev->BBC->RG_AMCS, AMCS_CCATX_MASK);
            } else {
                at86rf215_reg_and(dev, dev->BBC->RG_AMCS, ~AMCS_CCATX_MASK);
            }

            break;
        case AT86RF215_OPT_RPC:
            if (state) {
                at86rf215_enable_rpc(dev);
            } else {
                at86rf215_disable_rpc(dev);
            }

            break;
        default:
            /* do nothing */
            break;
    }
}

static void _wake_from_sleep(at86rf215_t *dev)
{
    /* wake the transceiver */
    at86rf215_rf_cmd(dev, CMD_RF_TRXOFF);
    at86rf215_await_state(dev, RF_STATE_TRXOFF);

    /* config is lost after SLEEP */
    at86rf215_reset(dev);

    /* if both transceivers were sleeping, the chip entered DEEP_SLEEP.
       Waking one device in that mode wakes the other one too. */
    if (dev->sibling && dev->sibling->state == AT86RF215_STATE_SLEEP) {
        at86rf215_rf_cmd(dev->sibling, CMD_RF_SLEEP);
    }
}

bool at86rf215_set_rx_from_idle(at86rf215_t *dev, uint8_t *state)
{
    if (dev->state == AT86RF215_STATE_SLEEP) {
        _wake_from_sleep(dev);
    }

    uint8_t s;
    while ((s = at86rf215_get_rf_state(dev)) == RF_STATE_TRANSITION) {}

    if (state) {
        *state = s;
    }

    if (s == RF_STATE_RESET) {
        at86rf215_rf_cmd(dev, CMD_RF_TRXOFF);
        at86rf215_await_state(dev, RF_STATE_TRXOFF);
        s = RF_STATE_TRXOFF;
    }

    if (s == RF_STATE_TRXOFF) {
        at86rf215_rf_cmd(dev, CMD_RF_RX);
        at86rf215_await_state(dev, RF_STATE_RX);
        s = RF_STATE_RX;
    }

    if (s == RF_STATE_RX) {
        return true;
    }

    return false;
}

bool at86rf215_set_idle_from_rx(at86rf215_t *dev, uint8_t state)
{
    if (state == RF_STATE_TX || state == CMD_RF_TXPREP) {
        return false;
    }

    if (dev->state == AT86RF215_STATE_SLEEP) {
        if (state == CMD_RF_SLEEP) {
            return true;
        }

        _wake_from_sleep(dev);
    }

    uint8_t s;
    while ((s = at86rf215_get_rf_state(dev)) == RF_STATE_TRANSITION) {}

    if (s != RF_STATE_RX) {
        return false;
    }

    if (state == RF_STATE_RX) {
        return true;
    }

    at86rf215_rf_cmd(dev, CMD_RF_TRXOFF);
    at86rf215_await_state(dev, RF_STATE_TRXOFF);

    if (state == RF_STATE_TRXOFF) {
        return true;
    }

    /* clear IRQ */
    at86rf215_reg_read(dev, dev->BBC->RG_IRQS);
    at86rf215_reg_read(dev, dev->RF->RG_IRQS);
    at86rf215_rf_cmd(dev, CMD_RF_SLEEP);

    dev->state = AT86RF215_STATE_SLEEP;

    if (state == RF_STATE_RESET) {
        return true;
    }

    return false;
}

int at86rf215_enable_batmon(at86rf215_t *dev, unsigned voltage)
{
    uint8_t bmdvc;

    /* only configure BATMON on one interface */
    if (!is_subGHz(dev) && dev->sibling != NULL) {
        dev = dev->sibling;
    }

    /* ensure valid range */
    if (voltage < 1700 || voltage > 3675) {
        return -ERANGE;
    }

    if (voltage > 2500) {
        /* high range */
        bmdvc = (voltage - 2550 + 37) / 75;
        DEBUG("[at86rf215] BATMON set to %u mV\n", 2550 + 75 * bmdvc);

        bmdvc |= BMDVC_BMHR_MASK;
    } else {
        /* low range */
        bmdvc  = (voltage - 1700 + 25) / 50;
        DEBUG("[at86rf215] BATMON set to %u mV\n", 1700 + 50 * bmdvc);
    }

    /* set batmon threshold */
    at86rf215_reg_write(dev, RG_RF_BMDVC, bmdvc);

    /* enable interrupt */
    at86rf215_reg_or(dev, dev->RF->RG_IRQM, RF_IRQ_BATLOW);

    return 0;
}

void at86rf215_disable_batmon(at86rf215_t *dev)
{
    /* only configure BATMON on one interface */
    if (!is_subGHz(dev) && dev->sibling != NULL) {
        dev = dev->sibling;
    }

    /* disable interrupt */
    at86rf215_reg_and(dev, dev->RF->RG_IRQM, ~RF_IRQ_BATLOW);
}
