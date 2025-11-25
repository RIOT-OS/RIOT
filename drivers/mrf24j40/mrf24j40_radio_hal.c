#include "mrf24j40.h"
#include "mrf24j40_params.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"
#include "net/ieee802154/radio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static const ieee802154_radio_ops_t mrf24j40_ops;

void mrf24j40_radio_irq_handler(void *ctx)
{
    ieee802154_dev_t *hal = ctx;
    mrf24j40_t *dev = hal->priv;

    /* update pending bits */
    mrf24j40_update_tasks(dev);
    DEBUG("[mrf24j40] INTERRUPT (pending: %x),\n", dev->pending);
    /* Transmit interrupt occurred */
    if (dev->pending & MRF24J40_TASK_TX_READY) {
        hal->cb(hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
    }

    /* Receive interrupt occurred */
    if (dev->pending & MRF24J40_TASK_RX_READY) {
        DEBUG("[mrf24j40] EVT - RX_END\n");
        dev->pending &= ~(MRF24J40_TASK_RX_READY);
        /* Prevent race condition if there is an ongoing transmission */
        if (dev->tx_pending) {
            mrf24j40_flush_rx(dev);
        }
        else {
            hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
    }
    DEBUG("[mrf24j40] END IRQ\n");
}

static int _write(ieee802154_dev_t *hal, const iolist_t *iolist)
{
    uint8_t len = 0;
    mrf24j40_t *dev = hal->priv;
    /* load packet data into FIFO */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        /* Check if there is data to copy, prevents assertion failure in the
         * SPI peripheral if there is no data to copy */
        if (iol->iol_len) {
            /* current packet data + FCS too long */
            len = mrf24j40_tx_load(dev, iol->iol_base, iol->iol_len, len);
        }
        /* only on first iteration: */
        if (iol == iolist) {
            /* Grab the FCF bits from the frame header */
            dev->fcf_low = *(uint8_t*)(iol->iol_base);
        }
    }

    /* write frame length field in FIFO */
    mrf24j40_tx_normal_fifo_write(dev, MRF24J40_TX_NORMAL_FIFO + 1, &len, 1);

    return 0;
}

int mrf24j40_init(mrf24j40_t *dev, const mrf24j40_params_t *params, ieee802154_dev_t *hal,
                   gpio_cb_t cb, void *ctx)
{
    hal->driver = &mrf24j40_ops;
    hal->priv = dev;
    dev->params = params;

    /* initialize GPIOs */
    spi_init_cs(dev->params->spi, dev->params->cs_pin);
    gpio_init(dev->params->reset_pin, GPIO_OUT);
    gpio_set(dev->params->reset_pin);
    gpio_init_int(dev->params->int_pin, GPIO_IN, GPIO_RISING, cb, ctx);

    /* reset device to default values */
    if (mrf24j40_reset(dev)) {
        return -ENODEV;
    }

    /* Set device to SLEEP */
    mrf24j40_sleep(dev);
    return 0;
}

static int _read(ieee802154_dev_t *hal, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    uint8_t phr;
    size_t pkt_len;
    int res = -ENOBUFS;
    mrf24j40_t *dev = hal->priv;

    phr = mrf24j40_reg_read_long(dev, MRF24J40_RX_FIFO);
    pkt_len = (phr & 0x7f) - IEEE802154_FCS_LEN;
    if (buf) {
        if (pkt_len > size) {
            mrf24j40_flush_rx(dev);
            return -ENOBUFS;
        }
        mrf24j40_rx_fifo_read(dev, 1, (uint8_t *)buf, pkt_len);

        if (info != NULL) {
            uint8_t rssi_scalar = 0;
            /* Read LQI and RSSI values from the RX fifo */
            mrf24j40_rx_fifo_read(dev, phr + 1, &(info->lqi), 1);
            mrf24j40_rx_fifo_read(dev, phr + 2, &(rssi_scalar), 1);
            info->rssi = mrf24j40_dbm_from_reg(rssi_scalar);
        }
        res = pkt_len;
    }
    else {
        mrf24j40_flush_rx(dev);
    }

    /* Return -ENOBUFS if a too small buffer is supplied. Return packet size
     * otherwise */
    return res;
}

static int _len(ieee802154_dev_t *hal)
{
    mrf24j40_t *dev = hal->priv;
    return (mrf24j40_reg_read_long(dev, MRF24J40_RX_FIFO) & 0x7f) - IEEE802154_FCS_LEN;
}

static int _off(ieee802154_dev_t *hal)
{
    mrf24j40_t *dev = hal->priv;
    mrf24j40_sleep(dev);
    return -ENOTSUP;
}

static int _request_on(ieee802154_dev_t *hal)
{
    mrf24j40_t *dev = hal->priv;
    mrf24j40_wake_up(dev);
    return 0;
}

static int _confirm_on(ieee802154_dev_t *hal)
{
    (void) hal;
    /* Nothing to do here */
    return 0;
}

static int _request_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx)
{
    int res = -EBUSY;
    mrf24j40_t *dev = hal->priv;
    (void) ctx;
    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        if (dev->fcf_low & IEEE802154_FCF_ACK_REQ) {
            mrf24j40_reg_write_short(dev, MRF24J40_REG_TXNCON, MRF24J40_TXNCON_TXNACKREQ | MRF24J40_TXNCON_TXNTRIG);
            mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG1, 0x00);
        }
        else {
            mrf24j40_reg_write_short(dev, MRF24J40_REG_TXNCON, MRF24J40_TXNCON_TXNTRIG);
        }
        dev->tx_pending = true;
        res = 0;
        break;
    case IEEE802154_HAL_OP_SET_RX:
        mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG1, 0x00);
        res = 0;
        mrf24j40_reg_write_short(dev, MRF24J40_REG_INTCON, ~(MRF24J40_INTCON_RXIE));
        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG1, MRF24J40_BBREG1_RXDECINV );
        mrf24j40_reg_write_short(dev, MRF24J40_REG_INTCON, ~(MRF24J40_INTCON_TXNIE));
        res = 0;
        break;
    case IEEE802154_HAL_OP_CCA:
        mrf24j40_enable_lna(dev);
        mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG6, MRF24J40_BBREG6_RSSIMODE1);
        mrf24j40_reg_write_short(dev, MRF24J40_REG_INTCON, 0xFF);
        res = 0;
        break;
    }

    return res;
}

static int _confirm_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx)
{
    int res = -EAGAIN;
    mrf24j40_t *dev = hal->priv;
    uint8_t tmp_ccaedth;
    uint8_t tmp_rssi;

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        if (dev->pending & MRF24J40_TASK_TX_READY) {
            dev->pending &= ~(MRF24J40_TASK_TX_READY);
            if (ctx) {
                uint8_t txstat = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXSTAT);
                ieee802154_tx_info_t *info = ctx;
                if (txstat & MRF24J40_TXSTAT_TXNSTAT) {
                    info->status = (txstat & MRF24J40_TXSTAT_CCAFAIL)
                                   ? TX_STATUS_MEDIUM_BUSY
                                   : TX_STATUS_NO_ACK;
                }
                else {
                    info->status = TX_STATUS_SUCCESS;
                }
                info->retrans = txstat >> MRF24J40_TXSTAT_MAX_FRAME_RETRIES_SHIFT;
            }
            mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG1, MRF24J40_BBREG1_RXDECINV);
            dev->tx_pending = false;
            return 0;
        }
        break;
    case IEEE802154_HAL_OP_SET_RX:
    case IEEE802154_HAL_OP_SET_IDLE:
        res = 0;
        break;
    case IEEE802154_HAL_OP_CCA:
        if ((mrf24j40_reg_read_short(dev, MRF24J40_REG_BBREG6) & MRF24J40_BBREG2_RSSIRDY)) {
            tmp_ccaedth = mrf24j40_reg_read_short(dev, MRF24J40_REG_CCAEDTH);       /* Energy detection threshold */
            tmp_rssi = mrf24j40_reg_read_long(dev, MRF24J40_REG_RSSI);
            mrf24j40_enable_auto_pa_lna(dev);

            *((bool*) ctx) = (tmp_rssi < tmp_ccaedth) ? true : false;
            res = 0;
        }
        break;
    }

    return res;
}

static int _set_cca_threshold(ieee802154_dev_t *hal, int8_t threshold)
{
    mrf24j40_t *dev = hal->priv;
    mrf24j40_set_cca_threshold(dev, threshold);
    return 0;
}

static int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    (void) dev;
    (void) mode;
    if (mode != IEEE802154_CCA_MODE_ED_THRESHOLD) {
        return -ENOTSUP;
    }
    return 0;
}

static int _config_phy(ieee802154_dev_t *hal, const ieee802154_phy_conf_t *conf)
{
    mrf24j40_t *dev = hal->priv;
    int8_t pow = conf->pow;
    uint8_t channel = conf->channel;
    if (pow < MRF24J40_MIN_TXPOWER && pow > MRF24J40_MAX_TXPOWER) {
        return -EINVAL;
    }
    mrf24j40_set_txpower(dev, pow);
    mrf24j40_set_chan(dev, channel);
    return 0;
}

static int _set_csma_params(ieee802154_dev_t *hal, const ieee802154_csma_be_t *bd, int8_t retries)
{
    mrf24j40_t *dev = hal->priv;

    if (bd->min > MRF24J40_MAX_MINBE) {
        return -EINVAL;
    }

    /* This radio ignores max_be */
    mrf24j40_set_csma_max_retries(dev, retries);
    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *hal, ieee802154_af_cmd_t cmd, const void *value)
{
    mrf24j40_t *dev = hal->priv;
    switch (cmd) {
        case IEEE802154_AF_SHORT_ADDR:
            mrf24j40_set_addr_short(dev, *((const uint16_t *)value));
            break;
        case IEEE802154_AF_EXT_ADDR:
            mrf24j40_set_addr_long(dev, value);
            break;
        case IEEE802154_AF_PANID:
            mrf24j40_set_pan(dev, *((const uint16_t *)value));
            break;
        case IEEE802154_AF_PAN_COORD:
            return -ENOTSUP;
    }

    return 0;
}

static int _config_src_addr_match(ieee802154_dev_t *hal, ieee802154_src_match_t cmd, const void *value)
{
    mrf24j40_t *dev = hal->priv;
    uint8_t tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_ACKTMOUT) & ~MRF24J40_ACKTMOUT_DRPACK;
    bool en;
    switch (cmd) {
    case IEEE802154_SRC_MATCH_EN:
        en = *((bool*) value);
        tmp = en ? tmp | MRF24J40_ACKTMOUT_DRPACK : tmp;
        mrf24j40_reg_write_short(dev, MRF24J40_REG_ACKTMOUT, tmp);
        return 0;
    default:
        return -ENOTSUP;
    }
}

static int _set_frame_filter_mode(ieee802154_dev_t *hal, ieee802154_filter_mode_t mode)
{
    mrf24j40_t *dev = hal->priv;
    switch (mode) {
        case IEEE802154_FILTER_ACCEPT:
            mrf24j40_set_option(dev, MRF24J40_OPT_PROMISCUOUS, false);
            break;
        case IEEE802154_FILTER_PROMISC:
            mrf24j40_set_option(dev, MRF24J40_OPT_PROMISCUOUS, true);
            break;
        case IEEE802154_FILTER_ACK_ONLY:
        default:
            return -ENOTSUP;
    }

    return 0;
}

int _set_frame_retrans(ieee802154_dev_t *hal, uint8_t retrans)
{
    (void) hal;
    (void) retrans;

    /* This radio does not allow to set the number of retransmission, but this
     * must still be defined because this radio declares
     * IEEE802154_CAP_FRAME_RETRANS */

    return -ENOTSUP;
}

static const ieee802154_radio_ops_t mrf24j40_ops = {
    .caps =  IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_FRAME_RETRANS
          | IEEE802154_CAP_FRAME_RETRANS_INFO
          | IEEE802154_CAP_PHY_OQPSK
          | IEEE802154_CAP_REG_RETENTION,
    .write = _write,
    .read = _read,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .len = _len,
    .off = _off,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_csma_params = _set_csma_params,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_frame_filter_mode = _set_frame_filter_mode,
    .set_frame_retrans = _set_frame_retrans,
};
