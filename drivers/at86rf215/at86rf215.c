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
 * @brief       Implementation of public functions for AT86RF215 driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "unaligned.h"
#include "at86rf215_internal.h"
#include "at86rf215_netdev.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _setup_interface(at86rf215_t *dev, const at86rf215_params_t *params, uint8_t index)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &at86rf215_driver;
    dev->params = *params;
    dev->state = AT86RF215_STATE_OFF;

    netdev_register(netdev, NETDEV_AT86RF215, index);
}

void at86rf215_setup(at86rf215_t *dev_09, at86rf215_t *dev_24, const at86rf215_params_t *params, uint8_t index)
{
    /* configure the sub-GHz interface */
    if (dev_09) {
        dev_09->RF = &RF09_regs;
        dev_09->BBC = &BBC0_regs;
        _setup_interface(dev_09, params, 2 * index);
        dev_09->sibling = dev_24;
    }

    /* configure the 2.4 GHz interface */
    if (dev_24) {
        dev_24->RF = &RF24_regs;
        dev_24->BBC = &BBC1_regs;
        _setup_interface(dev_24, params, 2 * index + 1);
        dev_24->sibling = dev_09;
    }
}

void at86rf215_reset_and_cfg(at86rf215_t *dev)
{
    netdev_ieee802154_reset(&dev->netdev);

    /* set device address */
    netdev_ieee802154_setup(&dev->netdev);

    if (is_subGHz(dev)) {
        dev->netdev.chan = CONFIG_AT86RF215_DEFAULT_SUBGHZ_CHANNEL;
    } else {
        dev->netdev.chan = CONFIG_AT86RF215_DEFAULT_CHANNEL;
    }

    dev->netdev.pan = CONFIG_IEEE802154_DEFAULT_PANID;

    /* set default options */
    dev->retries_max      = AT86RF215_RETRIES_MAX_DEFAULT;
    dev->csma_retries_max = AT86RF215_CSMA_RETRIES_MAX_DEFAULT;
    dev->csma_maxbe       = AT86RF215_CSMA_MAX_BE_DEFAULT;
    dev->csma_minbe       = AT86RF215_CSMA_MIN_BE_DEFAULT;

    dev->flags |= AT86RF215_OPT_AUTOACK
               |  AT86RF215_OPT_CSMA
#if CONFIG_AT86RF215_RPC
               |  AT86RF215_OPT_RPC
#endif
               ;

    /* apply the configuration */
    at86rf215_reset(dev);

    /* default to requesting ACKs, just like at86rf2xx */
    const netopt_enable_t enable = NETOPT_ENABLE;
    netdev_ieee802154_set(&dev->netdev, NETOPT_ACK_REQ, &enable, sizeof(enable));

    /* enable RX start IRQs */
    at86rf215_reg_or(dev, dev->BBC->RG_IRQM, BB_IRQ_RXAM);
}

void at86rf215_reset(at86rf215_t *dev)
{
    uint8_t reg;
    dev->state = AT86RF215_STATE_OFF;

    /* Reset state machine to ensure a known state */
    at86rf215_rf_cmd(dev, CMD_RF_TRXOFF);
    at86rf215_await_state(dev, RF_STATE_TRXOFF);

    if (!dev->sibling) {
        /* disable 2.4-GHz IRQs if the interface is not enabled */
        if (is_subGHz(dev)) {
            at86rf215_reg_write(dev, RG_BBC1_IRQM, 0);
            at86rf215_reg_write(dev, RG_RF24_IRQM, 0);
            at86rf215_reg_write(dev, RG_RF24_CMD, CMD_RF_SLEEP);

        /* disable sub-GHz IRQs if the interface is not enabled */
        } else {
            at86rf215_reg_write(dev, RG_BBC0_IRQM, 0);
            at86rf215_reg_write(dev, RG_RF09_IRQM, 0);
            at86rf215_reg_write(dev, RG_RF09_CMD, CMD_RF_SLEEP);
        }
    }

    /* disable clock output */
if (!IS_ACTIVE(CONFIG_AT86RF215_USE_CLOCK_OUTPUT)){
    at86rf215_reg_write(dev, RG_RF_CLKO, 0);
}
    /* allow to configure board-specific trim */
#ifdef CONFIG_AT86RF215_TRIM_VAL
    at86rf215_set_trim(dev, CONFIG_AT86RF215_TRIM_VAL);
#endif

    /* enable TXFE & RXFE IRQ */
    at86rf215_reg_write(dev, dev->BBC->RG_IRQM, BB_IRQ_TXFE | BB_IRQ_RXFE);

    /* enable EDC IRQ */
    at86rf215_reg_write(dev, dev->RF->RG_IRQM, RF_IRQ_EDC | RF_IRQ_TRXRDY);

    /* set energy detect threshold to -84 dBm */
    at86rf215_set_cca_threshold(dev, AT86RF215_EDT_DEFAULT);

    /* enable address filter 0 */
    at86rf215_reg_write(dev, dev->BBC->RG_AFC0, AFC0_AFEN0_MASK );
    at86rf215_reg_write(dev, dev->BBC->RG_AMAACKPD, AMAACKPD_PD0_MASK);

    /* enable auto-ACK with Frame Checksum & Data Rate derived from RX frame */
    reg = AMCS_AACKFA_MASK | AMCS_AACKDR_MASK;
    if (dev->flags & AT86RF215_OPT_AUTOACK) {
        reg |= AMCS_AACK_MASK;
    }

    at86rf215_reg_write(dev, dev->BBC->RG_AMCS, reg);

    if (CONFIG_AT86RF215_DEFAULT_PHY_MODE == IEEE802154_PHY_OQPSK) {
        at86rf215_configure_legacy_OQPSK(dev, CONFIG_AT86RF215_DEFAULT_OQPSK_RATE);
    }
    if (CONFIG_AT86RF215_DEFAULT_PHY_MODE == IEEE802154_PHY_MR_OQPSK) {
        at86rf215_configure_OQPSK(dev, CONFIG_AT86RF215_DEFAULT_MR_OQPSK_CHIPS,
                                       CONFIG_AT86RF215_DEFAULT_MR_OQPSK_RATE);
    }
    if (CONFIG_AT86RF215_DEFAULT_PHY_MODE == IEEE802154_PHY_MR_OFDM) {
        at86rf215_configure_OFDM(dev, CONFIG_AT86RF215_DEFAULT_MR_OFDM_OPT,
                                      CONFIG_AT86RF215_DEFAULT_MR_OFDM_MCS);
    }
    if (CONFIG_AT86RF215_DEFAULT_PHY_MODE == IEEE802154_PHY_MR_FSK) {
        at86rf215_configure_FSK(dev, CONFIG_AT86RF215_DEFAULT_MR_FSK_SRATE,
                                     CONFIG_AT86RF215_DEFAULT_MR_FSK_MOD_IDX,
                                     CONFIG_AT86RF215_DEFAULT_MR_FSK_MORD,
                                     CONFIG_AT86RF215_DEFAULT_MR_FSK_FEC);
    }

    /* set default channel */
    at86rf215_set_chan(dev, dev->netdev.chan);

    /* set short and long address */
    uint64_t long_addr;
    memcpy(&long_addr, dev->netdev.long_addr, sizeof(long_addr));
    at86rf215_set_addr_long(dev, long_addr);
    at86rf215_set_addr_short(dev, 0, unaligned_get_u16(dev->netdev.short_addr));

    /* set default PAN id */
    at86rf215_set_pan(dev, 0, dev->netdev.pan);

    /* set default TX power */
    at86rf215_set_txpower(dev, CONFIG_AT86RF215_DEFAULT_TXPOWER);

    /* start listening for incoming packets */
    at86rf215_rf_cmd(dev, CMD_RF_RX);
    at86rf215_await_state(dev, RF_STATE_RX);

    dev->state = AT86RF215_STATE_IDLE;
}

ssize_t at86rf215_send(at86rf215_t *dev, const void *data, size_t len)
{
    /* check data length */
    if (len > AT86RF215_MAX_PKT_LENGTH) {
        DEBUG("[at86rf215] Error: data to send exceeds max packet size\n");
        return -EOVERFLOW;
    }

    if (at86rf215_tx_prepare(dev)) {
        return -EBUSY;
    }

    at86rf215_tx_load(dev, data, len, 0);
    at86rf215_tx_exec(dev);
    return len;
}

void at86rf215_tx_done(at86rf215_t *dev)
{
    uint8_t amcs = at86rf215_reg_read(dev, dev->BBC->RG_AMCS);

    /* re-enable AACK, disable TX2RX */
    amcs &= ~AMCS_TX2RX_MASK;
    if (dev->flags & AT86RF215_OPT_AUTOACK) {
        amcs |= AMCS_AACK_MASK;
    }

    at86rf215_reg_write(dev, dev->BBC->RG_AMCS, amcs);
}

static bool _tx_ongoing(at86rf215_t *dev)
{
    if (dev->flags & AT86RF215_OPT_TX_PENDING) {
        return true;
    }

    /* we can still fill the TX buffer and queue TX
       when in AT86RF215_STATE_RX_SEND_ACK */
    if (dev->state == AT86RF215_STATE_TX ||
        dev->state == AT86RF215_STATE_TX_WAIT_ACK) {
        return true;
    }

    return false;
}

/*
 * As there is no packet queue in RIOT we have to block in send()
 * when the device is busy sending a previous frame.
 *
 * Since both _send() and _isr() are running in the same thread
 * we have to service radio events while waiting in order to
 * advance the previous transmission.
 */
static void _block_while_busy(at86rf215_t *dev)
{
    gpio_irq_disable(dev->params.int_pin);

    do {
        if (gpio_read(dev->params.int_pin) || dev->timeout) {
            at86rf215_driver.isr((netdev_t *) dev);
        }
        /* allow the other interface to process events */
        thread_yield();
    } while (_tx_ongoing(dev));

    gpio_irq_enable(dev->params.int_pin);
}

static void at86rf215_block_while_busy(at86rf215_t *dev)
{
    if (!IS_ACTIVE(MODULE_AT86RF215_BLOCKING_SEND)) {
        return;
    }

    if (_tx_ongoing(dev)) {
        DEBUG("[at86rf215] Block while TXing\n");
        _block_while_busy(dev);
    }
}

int at86rf215_tx_prepare(at86rf215_t *dev)
{
    if (dev->state == AT86RF215_STATE_SLEEP) {
        return -EAGAIN;
    }

    if (!IS_ACTIVE(MODULE_AT86RF215_BLOCKING_SEND) && _tx_ongoing(dev)) {
        return -EBUSY;
    } else {
        at86rf215_block_while_busy(dev);
    }

    dev->tx_frame_len = IEEE802154_FCS_LEN;

    return 0;
}

size_t at86rf215_tx_load(at86rf215_t *dev, const uint8_t *data,
                         size_t len, size_t offset)
{
    /* set bit if ACK was requested and retransmission is enabled */
    if (offset == 0 && (data[0] & IEEE802154_FCF_ACK_REQ) && dev->retries_max) {
        dev->flags |= AT86RF215_OPT_ACK_REQUESTED;
    }

    at86rf215_reg_write_bytes(dev, dev->BBC->RG_FBTXS + offset, data, len);
    dev->tx_frame_len += (uint16_t) len;

    return offset + len;
}

int at86rf215_tx_exec(at86rf215_t *dev)
{
    /* write frame length */
    at86rf215_reg_write16(dev, dev->BBC->RG_TXFLL, dev->tx_frame_len);

    dev->retries = dev->retries_max;
    dev->csma_retries = dev->csma_retries_max;

    dev->flags |= AT86RF215_OPT_TX_PENDING;

    if ((dev->flags & AT86RF215_OPT_CSMA) && !(dev->flags & AT86RF215_OPT_CCATX)) {
        dev->flags |= AT86RF215_OPT_CCA_PENDING;
    }

    if (dev->state == AT86RF215_STATE_IDLE) {
        at86rf215_rf_cmd(dev, CMD_RF_TXPREP);
    } else {
        DEBUG("[at86rf215] will TX after %s\n", at86rf215_sw_state2a(dev->state));
    }

    return 0;
}

void at86rf215_tx_abort(at86rf215_t *dev)
{
    dev->flags &= ~(AT86RF215_OPT_CCA_PENDING | AT86RF215_OPT_TX_PENDING);

    at86rf215_tx_done(dev);
    at86rf215_enable_baseband(dev);
    at86rf215_rf_cmd(dev, CMD_RF_RX);

    dev->state = AT86RF215_STATE_IDLE;
}

bool at86rf215_cca(at86rf215_t *dev)
{
    bool clear;
    uint8_t old_state;

    if (dev->state != AT86RF215_STATE_IDLE) {
        return false;
    }

    if (dev->flags & AT86RF215_OPT_TX_PENDING) {
        return false;
    }

    if (!at86rf215_set_rx_from_idle(dev, &old_state)) {
        return false;
    }

    /* disable ED IRQ, baseband */
    at86rf215_reg_and(dev, dev->RF->RG_IRQM, ~(RF_IRQ_EDC | RF_IRQ_TRXRDY));
    at86rf215_reg_and(dev, dev->BBC->RG_PC, ~PC_BBEN_MASK);

    at86rf215_disable_rpc(dev);

    /* start energy detect */
    at86rf215_reg_write(dev, dev->RF->RG_EDC, RF_EDSINGLE);
    while (!(at86rf215_reg_read(dev, dev->RF->RG_IRQS) & RF_IRQ_EDC)) {}

    clear = !(at86rf215_reg_read(dev, dev->BBC->RG_AMCS) & AMCS_CCAED_MASK);

    /* enable ED IRQ, baseband */
    at86rf215_reg_or(dev, dev->RF->RG_IRQM, RF_IRQ_EDC | RF_IRQ_TRXRDY);
    at86rf215_reg_or(dev, dev->BBC->RG_PC, PC_BBEN_MASK);

    at86rf215_enable_rpc(dev);
    at86rf215_set_idle_from_rx(dev, old_state);

    return clear;
}
