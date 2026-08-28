/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Implementation of 802.15.4 Radio HAL interface for AT86RF215 driver.
 *
 * @author      Stepan Konoplev <stepan.konoplev@haw-hamburg.de>
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "event.h"
#include "iolist.h"
#include "sys/bus.h"
#include "byteorder.h"

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"

#include "at86rf215.h"
#include "at86rf215_internal.h"
#include "at86rf215_registers.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define AT86RF215_INVALID_ED 127

static const ieee802154_radio_ops_t at86rf215_sub_ghz_ops;
static const ieee802154_radio_ops_t at86rf215_24_ghz_ops;

/* if only one interface is active, put the other one to sleep */
static inline void _put_sibling_to_sleep(at86rf215_t *dev)
{
    int reg = is_subGHz(dev) ? RG_RF24_CMD : RG_RF09_CMD;

    at86rf215_reg_write(dev, reg, CMD_RF_SLEEP);
}

/* clear the other IRQ if the sibling is not ready yet */
static inline void _clear_sibling_irq(at86rf215_t *dev)
{
    if (is_subGHz(dev)) {
        at86rf215_reg_read(dev, RG_RF24_IRQS);
        at86rf215_reg_read(dev, RG_BBC1_IRQS);
    }
    else {
        at86rf215_reg_read(dev, RG_RF09_IRQS);
        at86rf215_reg_read(dev, RG_BBC0_IRQS);
    }
}

static inline void _clear_irq(at86rf215_t *dev)
{
    at86rf215_reg_read(dev, dev->BBC->RG_IRQS);
    at86rf215_reg_read(dev, dev->RF->RG_IRQS);
}

static int _off(ieee802154_dev_t *hal)
{
    at86rf215_t *dev = hal->priv;

    if (dev->state == AT86RF215_STATE_OFF) {
        return 0;
    }

    _clear_irq(dev);
    at86rf215_rf_cmd(dev, CMD_RF_TRXOFF);
    at86rf215_await_state(dev, RF_STATE_TRXOFF);

    at86rf215_rf_cmd(dev, CMD_RF_SLEEP);
    dev->state = AT86RF215_STATE_OFF;
    return 0;
}

static int _request_on(ieee802154_dev_t *hal)
{
    at86rf215_t *dev = hal->priv;

    if (dev->state != AT86RF215_STATE_OFF) {
        return 0;
    }

    at86rf215_rf_cmd(dev, CMD_RF_TRXOFF);
    return 0;
}

static int _confirm_on(ieee802154_dev_t *hal)
{
    at86rf215_t *dev = hal->priv;
    ieee802154_dev_t *sibling_hal = dev->sibling;
    at86rf215_t *sibling_dev = sibling_hal ? sibling_hal->priv : NULL;

    if (at86rf215_get_rf_state(dev) != RF_STATE_TRXOFF) {
        return -EAGAIN;
    }

    /* sleep mode resets all registers */
    at86rf215_reset(dev);

    _clear_irq(dev);
    _clear_sibling_irq(dev);

    /* If both transceivers were sleeping, the chip entered DEEP_SLEEP.
     * Waking one device in that mode wakes the other one too. */
    if (sibling_dev && (sibling_dev->state == AT86RF215_STATE_OFF)) {
        _put_sibling_to_sleep(dev);
    }

    dev->state = AT86RF215_STATE_IDLE;

    return 0;
}

static int _write(ieee802154_dev_t *hal, const iolist_t *psdu)
{
    at86rf215_t *dev = hal->priv;
    ssize_t len = 0;
    uint8_t *data = psdu->iol_base;

    /* Indicate if an ack was requested to handle auto ack */
    dev->tx_ack_req = (*data & IEEE802154_FCF_ACK_REQ) > 0;

    /* load packet data into FIFO */
    for (const iolist_t *iol = psdu; iol; iol = iol->iol_next) {
        /* current packet data + FCS too long */
        if ((len + iol->iol_len + IEEE802154_FCS_LEN) > AT86RF215_MAX_PKT_LENGTH) {
            DEBUG("[at86rf215] error: packet too large (%" PRIuSIZE
                  " byte) to be sent\n", len + IEEE802154_FCS_LEN);
            return -EOVERFLOW;
        }

        if (iol->iol_len) {
            at86rf215_reg_write_bytes(dev, dev->BBC->RG_FBTXS + len, iol->iol_base, iol->iol_len);
            len += iol->iol_len;
        }
    }

    len += IEEE802154_FCS_LEN;

    at86rf215_reg_write16(dev, dev->BBC->RG_TXFLL, len);

    return 0;
}

static int _len(ieee802154_dev_t *hal)
{
    at86rf215_t *dev = hal->priv;
    int16_t pkt_len;

    /* get the size of the received packet */
    at86rf215_reg_read_bytes(dev, dev->BBC->RG_RXFLL, &pkt_len, sizeof(pkt_len));

    /* subtract length of FCS field */
    return (pkt_len & 0x7ff) - IEEE802154_FCS_LEN;
}

static int _read(ieee802154_dev_t *hal, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    at86rf215_t *dev = hal->priv;
    int16_t pkt_len;

    pkt_len = _len(hal);

    if (buf == NULL) {
        return 0;
    }

    /* not enough space in buf */
    if (pkt_len > (int)size) {
        return -ENOBUFS;
    }

    /* copy payload */
    at86rf215_reg_read_bytes(dev, dev->BBC->RG_FBRXS, buf, pkt_len);

    if (info != NULL) {
        int8_t ed = (int8_t)at86rf215_reg_read(dev, dev->RF->RG_EDV);
        if (ed == AT86RF215_INVALID_ED) {
            info->rssi = 0;
        }
        else {
            info->rssi = (uint8_t)(ed - IEEE802154_RADIO_RSSI_OFFSET);
        }

#if IS_USED(MODULE_IEEE802154_RX_TIMESTAMP)
        uint32_t rx_timestamp;
        at86rf215_reg_read_bytes(dev, dev->BBC->RG_CNT0, &rx_timestamp,
                                 sizeof(rx_timestamp));

        /* convert counter value to ns */
        info->timestamp = rx_timestamp * 1000ULL / 32;
#endif
    }

    return pkt_len;
}

static int _request_rx(at86rf215_t *dev)
{
    at86rf215_state_t curr_state = dev->state;

    if (curr_state == AT86RF215_STATE_RX ||
        curr_state == AT86RF215_STATE_RX_START ||
        curr_state == AT86RF215_STATE_TX_WAIT_ACK) {
        return 0;
    }

    if (curr_state != AT86RF215_STATE_IDLE) {
        return -EBUSY;
    }

    at86rf215_rf_cmd(dev, CMD_RF_RX);

    return 0;

}

static int _request_idle(at86rf215_t *dev, bool force)
{
    if (!force && (dev->state == AT86RF215_STATE_TX          ||
                   dev->state == AT86RF215_STATE_TX_WAIT_ACK ||
                   dev->state == AT86RF215_STATE_RX_SEND_ACK ||
                   dev->state == AT86RF215_STATE_CCA_RX      ||
                   dev->state == AT86RF215_STATE_CCA_IDLE)) {
        return -EBUSY;
    }

    /* writing TXPREP aborts any ongoing operation (TX, auto-ACK, ED) */
    at86rf215_rf_cmd(dev, CMD_RF_TXPREP);
    return 0;
}

static int _request_cca(at86rf215_t *dev, at86rf215_state_t next_cca_state)
{
    at86rf215_state_t state = dev->state;

    if (state != AT86RF215_STATE_IDLE && state != AT86RF215_STATE_RX) {
        return -EBUSY;
    }
    at86rf215_disable_baseband(dev);
    at86rf215_disable_rpc(dev);

    if (state == AT86RF215_STATE_IDLE) {
        at86rf215_rf_cmd(dev, CMD_RF_RX);
    }
    /* start single ED measurement */
    dev->state = next_cca_state;
    at86rf215_reg_write(dev, dev->RF->RG_EDC, RF_EDSINGLE);

    return 0;
}

static int _request_tx(at86rf215_t *dev)
{
    if (dev->state != AT86RF215_STATE_IDLE) {
        return -EBUSY;
    }

    if (dev->tx_ack_req) {
        at86rf215_filter_ack_only(dev, true);
        at86rf215_set_auto_mode(dev, AT86RF215_AM_TX2RX, true);
    }

    if (dev->cca_tx) {
        return _request_cca(dev, AT86RF215_STATE_CCATX);
    }

    return at86rf215_tx_exec(dev);
}

static int _request_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx)
{
    at86rf215_t *dev = hal->priv;

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        return _request_tx(dev);
    case IEEE802154_HAL_OP_SET_RX:
        return _request_rx(dev);
    case IEEE802154_HAL_OP_SET_IDLE:
        {
            bool force = *((bool *)ctx);
            return _request_idle(dev, force);
            break;
        }
    case IEEE802154_HAL_OP_CCA:
        {
            at86rf215_state_t next_cca_state = (dev->state == AT86RF215_STATE_IDLE) ?
                AT86RF215_STATE_CCA_IDLE : AT86RF215_STATE_CCA_RX;

            return _request_cca(dev, next_cca_state);
            break;
        }
    default:
        return -ENOTSUP;
    }
}

static int _confirm_tx(at86rf215_t *dev, ieee802154_tx_info_t *info)
{
    if (dev->state == AT86RF215_STATE_TX || dev->state == AT86RF215_STATE_CCATX) {
        return -EAGAIN;
    }

    if (info) {
        info->status = dev->cca_busy ? TX_STATUS_MEDIUM_BUSY : TX_STATUS_SUCCESS;
    }
    return 0;
}

static int _confirm_rx(at86rf215_t *dev)
{
    if (at86rf215_get_rf_state(dev) != RF_STATE_RX) {
        return -EAGAIN;
    }
    dev->state = AT86RF215_STATE_RX;
    return 0;
}

static int _confirm_idle(at86rf215_t *dev)
{
    if (at86rf215_get_rf_state(dev) != RF_STATE_TXPREP) {
        return -EAGAIN;
    }

    /* An aborted operation produces no completion IRQ (no TXFE for an
     * aborted TX/ACK, possibly no EDC for an aborted CCA) - clean up
     * the software state here so we don't get stuck. If the abort
     * happened during a CCA, re-enable what request_cca() disabled. */
    switch (dev->state) {
    case AT86RF215_STATE_CCA_RX:
    case AT86RF215_STATE_CCA_IDLE:
        at86rf215_enable_baseband(dev);
        at86rf215_enable_rpc(dev);
        break;
    case AT86RF215_STATE_TX:
    case AT86RF215_STATE_RX_SEND_ACK:
        at86rf215_tx_done(dev);
        dev->tx_ack_req = false;
        break;
    default:
        break;
    }

    dev->state = AT86RF215_STATE_IDLE;
    return 0;
}

static int _confirm_cca(at86rf215_t *dev, bool *clear)
{
    at86rf215_state_t state = dev->state;

    if (state == AT86RF215_STATE_CCA_RX || state == AT86RF215_STATE_CCA_IDLE) {
        return -EAGAIN;
    }

    at86rf215_enable_baseband(dev);
    at86rf215_enable_rpc(dev);
    *clear = !dev->cca_busy;
    return 0;
}

static int _confirm_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx)
{
    at86rf215_t *dev = hal->priv;

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        return _confirm_tx(dev, ctx);
    case IEEE802154_HAL_OP_SET_RX:
        return _confirm_rx(dev);
    case IEEE802154_HAL_OP_SET_IDLE:
        return _confirm_idle(dev);
    case IEEE802154_HAL_OP_CCA:
        return _confirm_cca(dev, ctx);
    default:
        return -ENOTSUP;
    }
    return 0;
}

static int _set_cca_threshold(ieee802154_dev_t *hal, int8_t threshold)
{
    at86rf215_t *dev = hal->priv;

    at86rf215_set_cca_threshold(dev, threshold);
    return 0;
}

static int _set_cca_mode(ieee802154_dev_t *hal, ieee802154_cca_mode_t mode)
{
    (void)hal;
    if (mode != IEEE802154_CCA_MODE_ED_THRESHOLD) {
        DEBUG("at86rf215_rf_ops: CCA mode not supported\n");
        return -ENOTSUP;
    }
    DEBUG("at86rf215_rf_ops: set_cca_mode to ED Threshold\n");
    return 0;
}

static int _config_phy(ieee802154_dev_t *hal, const ieee802154_phy_conf_t *conf)
{
    at86rf215_t *dev = hal->priv;

    switch (conf->phy_mode) {
#ifdef MODULE_IEEE802154_PHY_OQPSK
    case IEEE802154_PHY_OQPSK:
        at86rf215_configure_legacy_OQPSK(dev, at86rf215_OQPSK_get_mode_legacy(dev));
        break;
#endif
#ifdef MODULE_IEEE802154_PHY_MR_OQPSK
    case IEEE802154_PHY_MR_OQPSK: {
        const ieee802154_mr_oqpsk_conf_t *oqpsk = (const ieee802154_mr_oqpsk_conf_t *)conf;
        at86rf215_configure_OQPSK(dev, oqpsk->chips, oqpsk->rate_mode);
        break;
    }
#endif
#ifdef MODULE_IEEE802154_PHY_MR_OFDM
    case IEEE802154_PHY_MR_OFDM: {
        const ieee802154_mr_ofdm_conf_t *ofdm = (const ieee802154_mr_ofdm_conf_t *)conf;
        at86rf215_configure_OFDM(dev, ofdm->option, ofdm->scheme);
        break;
    }
#endif
#ifdef MODULE_IEEE802154_PHY_MR_FSK
    case IEEE802154_PHY_MR_FSK: {
        const ieee802154_mr_fsk_conf_t *fsk = (const ieee802154_mr_fsk_conf_t *)conf;
        at86rf215_configure_FSK(dev, fsk->srate, fsk->mod_idx, (fsk->mod_ord == 4), fsk->fec);
        break;
    }
#endif
    default:
        return -EINVAL;
    }

    at86rf215_set_chan(dev, conf->channel);
    at86rf215_set_txpower(dev, conf->pow);
    return 0;
}

static int _set_csma_params(ieee802154_dev_t *hal, const ieee802154_csma_be_t *bd,
                            int8_t retries)
{
    (void)bd;
    at86rf215_t *dev = hal->priv;
    if (retries > 0) {
        return -ENOTSUP;
    }
    dev->cca_tx = retries == 0;
    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *hal, ieee802154_af_cmd_t cmd, const void *value)
{
    at86rf215_t *dev = hal->priv;
    const uint16_t *pan_id = value;
    const network_uint16_t *short_addr = value;
    const eui64_t *ext_addr = value;

    switch (cmd) {
    case IEEE802154_AF_SHORT_ADDR:
        at86rf215_set_addr_short(dev, 0, byteorder_ntohs(*short_addr));
        break;
    case IEEE802154_AF_EXT_ADDR:
        at86rf215_set_addr_long(dev, ext_addr->uint64.u64);
        break;
    case IEEE802154_AF_PANID:
        at86rf215_set_pan(dev, 0, *pan_id);
        break;
    case IEEE802154_AF_PAN_COORD:
    default:
        return -ENOTSUP;
    }

    return 0;
}

static int _config_src_addr_match(ieee802154_dev_t *hal, ieee802154_src_match_t cmd,
                                  const void *value)
{
    at86rf215_t *dev = hal->priv;

    switch (cmd) {
    case IEEE802154_SRC_MATCH_EN:
        {
            bool enable = *(const bool *)value;
            if (enable) {
                at86rf215_reg_write(dev, dev->BBC->RG_AMAACKPD, 0x0F);
                at86rf215_reg_or(dev, dev->BBC->RG_IRQM, BB_IRQ_RXAM);
            }
            else {
                at86rf215_reg_write(dev, dev->BBC->RG_AMAACKPD, 0);
                at86rf215_reg_and(dev, dev->BBC->RG_IRQM, ~BB_IRQ_RXAM);
            }
            break;
        }
    default:
        return -ENOTSUP;
    }

    return 0;
}

static int _set_frame_filter_mode(ieee802154_dev_t *hal, ieee802154_filter_mode_t mode)
{
    at86rf215_t *dev = hal->priv;
    switch (mode) {
    case IEEE802154_FILTER_ACCEPT:
        at86rf215_set_promisc(dev, false);
        at86rf215_filter_ack_only(dev, false);
        break;

    case IEEE802154_FILTER_PROMISC:
        at86rf215_set_promisc(dev, true);
        break;

    case IEEE802154_FILTER_ACK_ONLY:
        at86rf215_set_promisc(dev, false);
        at86rf215_filter_ack_only(dev, true);
        break;

    case IEEE802154_FILTER_SNIFFER:
        at86rf215_set_promisc(dev, true);
        at86rf215_reg_and(dev, dev->BBC->RG_PC, ~PC_FCSFE_MASK);
        break;

    default:
        return -ENOTSUP;
    }

    if ((dev->filter_mode == IEEE802154_FILTER_SNIFFER)
        && (mode != IEEE802154_FILTER_SNIFFER)) {
        /* enable fcs */
        at86rf215_reg_or(dev, dev->BBC->RG_PC, PC_FCSFE_MASK);
    }

    dev->filter_mode = mode;
    return 0;
}

static int _get_frame_filter_mode(ieee802154_dev_t *hal, ieee802154_filter_mode_t *mode)
{
    at86rf215_t *dev = hal->priv;

    *mode = dev->filter_mode;
    return 0;
}

static void _handle_txrx_done(ieee802154_dev_t *hal, ieee802154_trx_ev_t event)
{
    at86rf215_t *dev = hal->priv;
    /* After RXFE or TXFE the device switches automatically to TXPREP make sure to be sync
     * with HW State */
    dev->state = AT86RF215_STATE_IDLE;
    if (hal->cb) {
        hal->cb(hal, event);
    }
}

static void _handle_edc(ieee802154_dev_t *hal)
{
    at86rf215_t *dev = hal->priv;
    at86rf215_state_t current_state = dev->state;

    int8_t ed_value = at86rf215_get_ed_level(dev);

    dev->cca_busy = ed_value > at86rf215_get_cca_threshold(dev);

    at86rf215_enable_baseband(dev);
    at86rf215_enable_rpc(dev);

    if (current_state == AT86RF215_STATE_CCATX) {
        if (dev->cca_busy) {
            at86rf215_tx_done(dev);
            at86rf215_rf_cmd(dev, CMD_RF_TXPREP);
            _handle_txrx_done(hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
            return;
        }
        /* channel clear -> TX */
        dev->state = AT86RF215_STATE_TX;
        at86rf215_rf_cmd(dev, CMD_RF_TXPREP);
        return;
    }

    dev->state = (current_state == AT86RF215_STATE_CCA_IDLE) ?
                 AT86RF215_STATE_IDLE : AT86RF215_STATE_RX;

    if (current_state == AT86RF215_STATE_CCA_IDLE) {
        at86rf215_rf_cmd(dev, CMD_RF_TXPREP);
        _handle_txrx_done(hal, IEEE802154_RADIO_CONFIRM_CCA);
        return;
    }

    dev->state = AT86RF215_STATE_RX;
    if (hal->cb) {
        hal->cb(hal, IEEE802154_RADIO_CONFIRM_CCA);
    }
}

static void _tx_end(ieee802154_dev_t *hal)
{
    at86rf215_t *dev = hal->priv;

    DEBUG("[at86rf215] _tx_end: SW=%s HW=%s\n",
          at86rf215_sw_state2a(dev->state),
          at86rf215_hw_state2a(at86rf215_get_rf_state(dev)));

    at86rf215_tx_done(dev);

    dev->state = dev->tx_ack_req ? AT86RF215_STATE_TX_WAIT_ACK : AT86RF215_STATE_IDLE;
    if (hal->cb) {
        DEBUG("[at86rf215] _tx_end: cb CONFIRM_TX_DONE\n");
        hal->cb(hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
    }
}

static void at86rf215_handle_common_irq(ieee802154_dev_t *hal)
{
    at86rf215_t *dev = hal->priv;
    uint8_t fcf0 = 0;
    uint8_t bb_irqs_enabled = BB_IRQ_RXFS | BB_IRQ_RXFE | BB_IRQ_TXFE | BB_IRQ_AGCR;
    uint8_t rf_irqs_enabled = RF_IRQ_EDC | RF_IRQ_TRXRDY | RF_IRQ_BATLOW;
    uint8_t bb_irq_mask = at86rf215_reg_read(dev, dev->BBC->RG_IRQS);
    uint8_t rf_irq_mask = at86rf215_reg_read(dev, dev->RF->RG_IRQS);

    /* exit early if the interrupt was not for this interface */
    bb_irq_mask &= bb_irqs_enabled;
    rf_irq_mask &= rf_irqs_enabled;

    if (!bb_irq_mask && !rf_irq_mask) {
        return;
    }
    /* Handle Low Battery IRQ */
#if MODULE_AT86RF215_BATMON
    if ((rf_irq_mask & RF_IRQ_BATLOW)) {
        msg_bus_t *bus = sys_bus_get(SYS_BUS_POWER);
        msg_bus_post(bus, SYS_BUS_POWER_EVENT_LOW_VOLTAGE, NULL);
    }
#endif

#ifdef MODULE_IEEE802154_PHY_MR_FSK
    /* listen for short preamble in RX */
    if (bb_irq_mask & BB_IRQ_TXFE && dev->fsk_pl) {
        at86rf215_FSK_prepare_rx(dev);
    }
#endif /* MODULE_IEEE802154_PHY_MR_FSK */

    int iter = 0;
    while (bb_irq_mask || (rf_irq_mask & RF_IRQ_EDC)) {

        /* This should never happen */
        if (++iter > 3) {
            puts("AT86RF215: stuck in ISR");
            LOG_ERROR("\tnum_channels: %d\n", dev->num_chans);
            LOG_ERROR("\tHW: %s\n", at86rf215_hw_state2a(at86rf215_get_rf_state(dev)));
            LOG_ERROR("\tSW: %s\n", at86rf215_sw_state2a(dev->state));
            LOG_ERROR("\trf_irq_mask: %x\n", rf_irq_mask);
            LOG_ERROR("\tbb_irq_mask: %x\n", bb_irq_mask);
            break;
        }

        DEBUG("[at86rf215] ISR loop iter=%d bb=0x%02x rf=0x%02x SW=%s HW=%s\n",
              iter, bb_irq_mask, rf_irq_mask,
              at86rf215_sw_state2a(dev->state),
              at86rf215_hw_state2a(at86rf215_get_rf_state(dev)));

        switch (dev->state) {
        case AT86RF215_STATE_RX:
        case AT86RF215_STATE_RX_START:
        case AT86RF215_STATE_IDLE:

            /* assume this comes from sending auto ack; don't indicate rx_done here*/
            bb_irq_mask &= ~BB_IRQ_TXFE;

            if (bb_irq_mask & BB_IRQ_RXFS) {
                DEBUG("[at86rf215] ISR IDLE/RX: -> RX_START cb\n");
                bb_irq_mask &= ~BB_IRQ_RXFS;
                dev->state = AT86RF215_STATE_RX_START;
                if (hal->cb) {
                    hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_START);
                }
            }

            /* to avoid getting stuck in RX_START go back to rx after gain control is released */
            if (bb_irq_mask & BB_IRQ_AGCR) {
                bb_irq_mask &= ~BB_IRQ_AGCR;

                if (dev->state == AT86RF215_STATE_RX_START) {
                    dev->state = AT86RF215_STATE_RX;
                }
            }

            if (!(bb_irq_mask & BB_IRQ_RXFE)) {
                break;
            }

            bb_irq_mask &= ~BB_IRQ_RXFE;

            fcf0 = at86rf215_reg_read(dev, dev->BBC->RG_FBRXS);

            if (fcf0 & IEEE802154_FCF_ACK_REQ && (dev->auto_mode == AT86RF215_AM_AUTO_ACK)) {
                DEBUG("[at86rf215] ISR IDLE/RX: RX_ACK_REQ set -> RX_SEND_ACK\n");
                dev->state = AT86RF215_STATE_RX_SEND_ACK;
                break;
            }

            DEBUG("[at86rf215] ISR IDLE/RX: no ACK_REQ -> RX_DONE cb\n");
            _handle_txrx_done(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
            break;

        case AT86RF215_STATE_RX_SEND_ACK:
            /* this can happen when receiving an RXFE IRQ but the AGCR is not indicated yet */
            bb_irq_mask &= ~BB_IRQ_AGCR;

            if (!(bb_irq_mask & BB_IRQ_TXFE)) {
                break;
            }

            bb_irq_mask &= ~BB_IRQ_TXFE;
            DEBUG("[at86rf215] ISR RX_SEND_ACK: TXFE (ACK sent) -> RX_DONE cb\n");
            _handle_txrx_done(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
            break;

        case AT86RF215_STATE_TX:
            /* start transmitting the frame */
            if (rf_irq_mask & RF_IRQ_TRXRDY) {
                DEBUG("[at86rf215] ISR: TX_PENDING + TRXRDY -> starting TX\n");
#ifdef MODULE_IEEE802154_PHY_MR_FSK
                /* send long preamble in TX */
                if (dev->fsk_pl) {
                    at86rf215_FSK_prepare_tx(dev);
                }
#endif
                /* switch to state TX */
                at86rf215_rf_cmd(dev, CMD_RF_TX);
                return;
            }

            if (!(bb_irq_mask & BB_IRQ_TXFE)) {
                DEBUG("[at86rf215] ISR TX: unexpected irq 0x%02x\n", bb_irq_mask);
                break;
            }

            bb_irq_mask &= ~BB_IRQ_TXFE;
            DEBUG("[at86rf215] ISR TX: TXFE\n");
            _tx_end(hal);
            break;

        case AT86RF215_STATE_TX_WAIT_ACK:

            /* received a frame passing fcs */
            if (bb_irq_mask & BB_IRQ_RXFS) {
                bb_irq_mask &= ~BB_IRQ_RXFS;
            }

            if (bb_irq_mask & BB_IRQ_AGCR) {
                bb_irq_mask &= ~BB_IRQ_AGCR;
            }

            if (!(bb_irq_mask & BB_IRQ_RXFE)) {
                DEBUG("TX_WAIT_ACK: only RXFE (%x)\n", bb_irq_mask);
                break;
            }

            bb_irq_mask &= ~BB_IRQ_RXFE;

            DEBUG("[at86rf215] TX_WAIT_ACK: ack recv -> rx_done_cb\n");
            _handle_txrx_done(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
            break;

        case AT86RF215_STATE_CCA_RX:
        case AT86RF215_STATE_CCA_IDLE:
        case AT86RF215_STATE_CCATX:
            /* Start ED or handle result */
            if (rf_irq_mask & RF_IRQ_EDC) {
                rf_irq_mask &= ~RF_IRQ_EDC;
                _handle_edc(hal);
            }
            break;
        case AT86RF215_STATE_OFF:
        default:
            DEBUG("[at86rf215] ISR: unhandled state %s bb=0x%02x\n",
                  at86rf215_sw_state2a(dev->state), bb_irq_mask);
            break;
        }
    }
}

static void at86rf215_irq_handler(ieee802154_dev_t *hal)
{
    at86rf215_t *dev = hal->priv;
    ieee802154_dev_t *sibling_hal = dev->sibling;
    at86rf215_t *sibling_dev = sibling_hal ? sibling_hal->priv : NULL;

    do {
        at86rf215_handle_common_irq(hal);

        if (sibling_dev && sibling_dev->state != AT86RF215_STATE_OFF) {
            at86rf215_handle_common_irq(sibling_hal);
        }
        else {
            _clear_sibling_irq(dev);
        }
    } while (gpio_read(dev->params.int_pin));
}

static void _event_handler(event_t *event)
{
    at86rf215_bhp_ev_t *bhp = container_of(event, at86rf215_bhp_ev_t, ev);

    if (bhp->hal_09) {
        at86rf215_irq_handler(bhp->hal_09);
    }
    else {
        at86rf215_irq_handler(bhp->hal_24);
    }
}

static int _init_hardware(at86rf215_t *dev, void (*cb)(void *), void *ctx)
{
    int res = -1;

    /* don't call HW init for both radios */
    if (is_subGHz(dev) || dev->sibling == NULL) {
        /* initialize GPIOs */
        spi_init_cs(dev->params.spi, dev->params.cs_pin);
        gpio_init(dev->params.reset_pin, GPIO_OUT);
        gpio_set(dev->params.reset_pin);

        /* reset the entire chip */
        if ((res = at86rf215_hardware_reset(dev))) {
            return res;
        }

        /* turn off unused interface */
        if (dev->sibling == NULL) {
            _put_sibling_to_sleep(dev);
        }

        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, cb, ctx);
    }

    res = at86rf215_reg_read(dev, RG_RF_PN);
    if ((res != AT86RF215_PN) && (res != AT86RF215M_PN)) {
        DEBUG("[at86rf215] error: unable to read correct part number: %x\n", res);
        return -ENOTSUP;
    }

    dev->state = AT86RF215_STATE_OFF;
    at86rf215_rf_cmd(dev, CMD_RF_SLEEP);
    return 0;
}

static void _irq_handler(void *ctx)
{
    at86rf215_bhp_ev_t *bhp = ctx;
    event_post(bhp->evq, &bhp->ev);
}

int at86rf215_init(at86rf215_t *dev_09,
                   at86rf215_t *dev_24,
                   ieee802154_dev_t *hal_09,
                   ieee802154_dev_t *hal_24,
                   const at86rf215_params_t *params,
                   void *ctx)
{

    /* configure the sub-GHz interface */
    if (dev_09) {
        dev_09->RF = &RF09_regs;
        dev_09->BBC = &BBC0_regs;
        dev_09->sibling = hal_24;
        dev_09->params = *params;
        dev_09->state = AT86RF215_STATE_OFF;
        hal_09->driver = &at86rf215_sub_ghz_ops;
        hal_09->priv = dev_09;
        _init_hardware(dev_09, _irq_handler, ctx);
    }

    /* configure the 2.4 GHz interface */
    if (dev_24) {
        dev_24->RF = &RF24_regs;
        dev_24->BBC = &BBC1_regs;
        dev_24->sibling = hal_09;
        dev_24->params = *params;
        dev_24->state = AT86RF215_STATE_OFF;
        hal_24->driver = &at86rf215_24_ghz_ops;
        hal_24->priv = dev_24;
        _init_hardware(dev_24, _irq_handler, ctx);
    }

    return 0;
}

int at86rf215_init_event(at86rf215_bhp_ev_t *bhp, ieee802154_dev_t *hal_09,
                         ieee802154_dev_t *hal_24, event_queue_t *evq)
{
    bhp->hal_09 = hal_09;
    bhp->hal_24 = hal_24;
    bhp->evq = evq;
    bhp->ev.handler = _event_handler;

    return 0;
}

#define AT86RF215_COMMON_CAPS \
          ((IS_USED(MODULE_IEEE802154_PHY_OQPSK)    ? IEEE802154_CAP_PHY_OQPSK    : 0) \
         | (IS_USED(MODULE_IEEE802154_PHY_MR_OQPSK) ? IEEE802154_CAP_PHY_MR_OQPSK : 0) \
         | (IS_USED(MODULE_IEEE802154_PHY_MR_OFDM)  ? IEEE802154_CAP_PHY_MR_OFDM  : 0) \
         | (IS_USED(MODULE_IEEE802154_PHY_MR_FSK)   ? IEEE802154_CAP_PHY_MR_FSK   : 0) \
         | (IS_USED(MODULE_IEEE802154_RX_TIMESTAMP) ? IEEE802154_CAP_RX_TIMESTAMP : 0) \
         | IEEE802154_CAP_AUTO_ACK \
         | IEEE802154_CAP_IRQ_TX_DONE \
         | IEEE802154_CAP_IRQ_RX_START \
         | IEEE802154_CAP_AUTO_TX2RX \
         | IEEE802154_CAP_IRQ_CCA_DONE)

#define AT86RF215_OPS(band)                              \
{                                                        \
        .caps = (band) | AT86RF215_COMMON_CAPS,          \
        .write                 = _write,                 \
        .read                  = _read,                  \
        .request_on            = _request_on,            \
        .confirm_on            = _confirm_on,            \
        .len                   = _len,                   \
        .off                   = _off,                   \
        .request_op            = _request_op,            \
        .confirm_op            = _confirm_op,            \
        .set_cca_threshold     = _set_cca_threshold,     \
        .set_cca_mode          = _set_cca_mode,          \
        .config_phy            = _config_phy,            \
        .set_csma_params       = _set_csma_params,       \
        .config_addr_filter    = _config_addr_filter,    \
        .config_src_addr_match = _config_src_addr_match, \
        .set_frame_filter_mode = _set_frame_filter_mode, \
        .get_frame_filter_mode = _get_frame_filter_mode, \
}

static const ieee802154_radio_ops_t at86rf215_sub_ghz_ops = AT86RF215_OPS(IEEE802154_CAP_SUB_GHZ);
static const ieee802154_radio_ops_t at86rf215_24_ghz_ops = AT86RF215_OPS(IEEE802154_CAP_24_GHZ);
