#include "at86rf2xx.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"
#include "net/ieee802154/radio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _set_trx_state(ieee802154_dev_t *dev, ieee802154_trx_state_t state);
static const ieee802154_radio_ops_t at86rf2xx_ops;
void at86rf2xx_schedule_task(void *dev);

static inline bool _is_sleep(ieee802154_dev_t *dev)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    return _dev->is_sleep;
}

static int _write(ieee802154_dev_t *dev, const iolist_t *pkt)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    uint8_t len = 0;

    /* Load packet data into FIFO. Size checks are handled by higher
     * layers */
    for (const iolist_t *iol = pkt; iol; iol = iol->iol_next) {
        if (iol->iol_len) {
            at86rf2xx_sram_write(_dev, len + 1, iol->iol_base, iol->iol_len);
            len += iol->iol_len;
        }
    }
    len += IEEE802154_FCS_LEN;

    /* write frame length field in FIFO */
    at86rf2xx_sram_write(_dev, 0, &len, 1);
    return 0;
}

static int _request_transmit(ieee802154_dev_t *dev)
{
    at86rf2xx_set_internal_state((at86rf2xx_t*) dev, AT86RF2XX_STATE_TX_ARET_ON);
    at86rf2xx_wait_trx_state_finish((at86rf2xx_t*) dev);
    at86rf2xx_tx_exec((at86rf2xx_t*) dev);
    return 0;
}

static int _len(ieee802154_dev_t *dev)
{
    uint8_t phr;

    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    /* Read the length from the Physical HDR (first byte) */
    at86rf2xx_sram_read(_dev, 0, &phr, 1);

    /* ignore MSB (refer p.80) and subtract length of FCS field */
    return (phr & 0x7f) - IEEE802154_FCS_LEN;
}

/* The radio should be woken up */
static int _indication_rx(ieee802154_dev_t *dev, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    uint8_t phr;
    size_t pkt_len;

    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;

    /* start frame buffer access */
    at86rf2xx_fb_start(_dev);

    /* get the size of the received packet */
    at86rf2xx_fb_read(_dev, &phr, 1);
    /* ignore MSB (refer p.80) and subtract length of FCS field */
    pkt_len = (phr & 0x7f) - IEEE802154_FCS_LEN;

    /* not enough space in buf */
    if (pkt_len > size) {
        /* Stop the frame buffer. The transceiver state is still RX_ON, but the
         * frame buffer protection is raised. So, it's possible to receive more
         * packets */
        at86rf2xx_fb_stop(_dev);
        return -ENOBUFS;
    }
    /* copy payload */
    at86rf2xx_fb_read(_dev, buf, pkt_len);

    /* Ignore FCS but advance fb read - we must give a temporary buffer here,
     * as we are not allowed to issue SPI transfers without any buffer */
    uint8_t tmp[2];
    at86rf2xx_fb_read(_dev, tmp, 2);
    (void)tmp;

    /* AT86RF212B RSSI_BASE_VAL + 1.03 * ED, base varies for diff. modulation and datarates
     * AT86RF232  RSSI_BASE_VAL + ED, base -91dBm
     * AT86RF233  RSSI_BASE_VAL + ED, base -94dBm
     * AT86RF231  RSSI_BASE_VAL + ED, base -91dBm
     * AT86RFA1   RSSI_BASE_VAL + ED, base -90dBm
     * AT86RFR2   RSSI_BASE_VAL + ED, base -90dBm
     *
     * AT86RF231 MAN. p.92, 8.4.3 Data Interpretation
     * AT86RF232 MAN. p.91, 8.4.3 Data Interpretation
     * AT86RF233 MAN. p.102, 8.5.3 Data Interpretation
     *
     * for performance reasons we ignore the 1.03 scale factor on the 212B,
     * which causes a slight error in the values, but the accuracy of the ED
     * value is specified as +/- 5 dB, so it should not matter very much in real
     * life.
     */
    if (info != NULL) {
        uint8_t ed = 0;
        at86rf2xx_fb_read(_dev, &(info->lqi), 1);

#if defined(MODULE_AT86RF231)
        /* AT86RF231 does not provide ED at the end of the frame buffer, read
         * from separate register instead */
        at86rf2xx_fb_stop(_dev);
        ed = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__PHY_ED_LEVEL);
#else
        at86rf2xx_fb_read(_dev, &ed, 1);
        at86rf2xx_fb_stop(_dev);
#endif
        info->rssi = RSSI_BASE_VAL + ed + IEEE802154_RADIO_RSSI_OFFSET;
        DEBUG("[at86rf2xx] LQI:%d high is good, RSSI:%d high is either good or"
              "too much interference.\n", info->lqi, info->rssi);
    }
    else {
        at86rf2xx_fb_stop(_dev);
    }

    /* At this point the transceiver state is RX_ON and the radio is able to
       receive more packets */

    return pkt_len;
}

static int set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    if (threshold < RSSI_BASE_VAL || threshold > (RSSI_BASE_VAL + 0x1F)) {
        return -EINVAL;
    }

    at86rf2xx_set_cca_threshold((at86rf2xx_t*) dev, threshold);
    return 0;
}

static int _config_phy(ieee802154_dev_t *dev, ieee802154_phy_conf_t *conf)
{
    uint8_t channel = conf->channel;
    int8_t pow = conf->pow;
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;

    if (pow < -AT86RF2XX_TXPOWER_OFF || pow > (-AT86RF2XX_TXPOWER_OFF + AT86RF2XX_TXPOWER_MAX)) {
        return -EINVAL;
    }

    uint8_t phy_cc_cca = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__PHY_CC_CCA);
    /* Clear previous configuration for channel number */
    phy_cc_cca &= ~(AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);

    /* Update the channel register */
    phy_cc_cca |= (channel & AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__PHY_CC_CCA, phy_cc_cca);

    /* Return to the state we had before reconfiguring */
    at86rf2xx_set_txpower((at86rf2xx_t*) dev, pow);

    return 0;
}

static int _set_trx_state(ieee802154_dev_t *dev, ieee802154_trx_state_t state)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    int radio_state = at86rf2xx_get_internal_state(_dev);
    int int_state;
    uint8_t mask = radio_state & 0xF;

    if (mask == 0x1) {
        return -EBUSY;
    }

    if (_dev->trx_state == state) {
        return -EALREADY;
    }

    /* Enable RX path */
    switch(state) {
        case IEEE802154_TRX_STATE_TRX_OFF:
            int_state = AT86RF2XX_STATE_TRX_OFF;
            break;
        case IEEE802154_TRX_STATE_RX_ON:
            int_state = (1 << 4) | 0x06;
            break;
        case IEEE802154_TRX_STATE_TX_ON:
            int_state = AT86RF2XX_STATE_PLL_ON;
            break;
        default:
            return -EINVAL;
    }

    _dev->trx_state = state;
    at86rf2xx_set_internal_state(_dev, int_state);

    return 0;
}

static int _off(ieee802154_dev_t *dev)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;

    at86rf2xx_sleep((at86rf2xx_t*) dev);
    _dev->is_sleep = true;
    return 0;
}

static bool _get_cap(ieee802154_dev_t *dev, ieee802154_rf_caps_t cap)
{
    (void) dev;
    /* Expose caps for Extended and Basic mode */
    switch(cap) {
        case IEEE802154_CAP_FRAME_RETRANS:
        case IEEE802154_CAP_AUTO_CSMA:
        case IEEE802154_CAP_24_GHZ:
        case IEEE802154_CAP_IRQ_TX_DONE:
        case IEEE802154_CAP_IRQ_CCA_DONE:
#if IS_ACTIVE(MODULE_AT86RF233)
        case IEEE802154_CAP_FRAME_RETRANS_INFO:
#endif
            return true;
        default:
            return false;
    }
}

static int set_hw_addr_filter(ieee802154_dev_t *dev,
                              const network_uint16_t *short_addr,
                              const eui64_t *ext_addr,
                              uint16_t pan_id)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;

    le_uint16_t le_pan = byteorder_btols(byteorder_htons(pan_id));

    if (short_addr) {
        at86rf2xx_reg_write(_dev, AT86RF2XX_REG__SHORT_ADDR_0,
                            short_addr->u8[1]);
        at86rf2xx_reg_write(_dev, AT86RF2XX_REG__SHORT_ADDR_1,
                            short_addr->u8[0]);
    }

    if (ext_addr) {
        for (int i = 0; i < 8; i++) {
            at86rf2xx_reg_write(_dev, (AT86RF2XX_REG__IEEE_ADDR_0 + i),
                    ext_addr->uint8[IEEE802154_LONG_ADDRESS_LEN - 1 - i]);
        }
    }

    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__PAN_ID_0, le_pan.u8[0]);
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__PAN_ID_1, le_pan.u8[1]);
    return 0;
}

static int set_frame_retrans(ieee802154_dev_t *dev, uint8_t retrans)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    uint8_t tmp = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__XAH_CTRL_0);

    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_FRAME_RETRIES);
    tmp |= ((retrans > 7) ? 7 : retrans) << 4;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);

    return 0;
}

static int set_csma_params(ieee802154_dev_t *dev, ieee802154_csma_be_t *bd, int8_t retries)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;

    if (retries > 5) {
        return -EINVAL;
    }

    if(bd) {
        uint8_t max = bd->max;
        uint8_t min = bd->min;
        if (max > 8) {
            return -EINVAL;
        }
        max = (max > 8) ? 8 : max;
        min = (min > max) ? max : min;
        at86rf2xx_reg_write(_dev, AT86RF2XX_REG__CSMA_BE, (max << 4) | (min));
    }

    retries = (retries < 0) ? 7 : retries;  /* max < 0 => disable CSMA (set to 7) */
    uint8_t tmp = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES);
    tmp |= (retries << 1);
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);

    return 0;
}

int at86rf2xx_init(at86rf2xx_t *dev, const at86rf2xx_params_t *params, void (*isr)(void *arg))
{
    /* State to return after receiving or transmitting */
    dev->trx_state = IEEE802154_TRX_STATE_TRX_OFF;
    dev->dev.driver = &at86rf2xx_ops;

    /* initialize device descriptor */
    dev->params = *params;

    /* initialize GPIOs */
    spi_init_cs(dev->params.spi, dev->params.cs_pin);
    gpio_init(dev->params.sleep_pin, GPIO_OUT);
    gpio_clear(dev->params.sleep_pin);
    gpio_init(dev->params.reset_pin, GPIO_OUT);
    gpio_set(dev->params.reset_pin);
    gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, isr, dev);

    /* Intentionally check if bus can be acquired,
       since getbus() drops the return value */
    if (spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                                                dev->params.spi_clk) < 0) {
        DEBUG("[at86rf2xx] error: unable to acquire SPI bus\n");
        return -EIO;
    }
    spi_release(dev->params.spi);

    /* test if the device is responding */
    if (at86rf2xx_reg_read(dev, AT86RF2XX_REG__PART_NUM) != AT86RF2XX_PARTNUM) {
        DEBUG("[at86rf2xx] error: unable to read correct part number\n");
        return -ENOTSUP;
    }

    /* reset device to default values */
    at86rf2xx_sleep(dev);

    return 0;
}

static int _request_cca(ieee802154_dev_t *dev)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;

    if (_dev->trx_state != IEEE802154_TRX_STATE_RX_ON) {
        return -EINVAL;
    }

    uint8_t reg;
    uint8_t int_state = 0x06;
    uint8_t rx_syn = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__RX_SYN);
    rx_syn = rx_syn | AT86RF2XX_RX_SYN__RX_PDT_DIS;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__RX_SYN, rx_syn);
    at86rf2xx_set_internal_state(_dev, int_state);
    reg = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__IRQ_MASK) | AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__IRQ_MASK, reg);
    at86rf2xx_wait_trx_state_finish(_dev);
    /* Perform CCA */
    reg = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__PHY_CC_CCA);
    reg |= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__PHY_CC_CCA, reg);
    return 0;
}

static int _confirm_cca(ieee802154_dev_t *dev)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    uint8_t trx_status = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__TRX_STATUS);
    if (!(trx_status & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE)) {
        return -EAGAIN;
    }
    uint8_t rx_syn = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__RX_SYN);
    rx_syn = rx_syn & ~AT86RF2XX_RX_SYN__RX_PDT_DIS;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__RX_SYN, rx_syn);
    uint8_t reg = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__IRQ_MASK) & ~AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__IRQ_MASK, reg);
    bool ret = !!(trx_status & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS);
    return ret;
}

static int _request_set_trx_state(ieee802154_dev_t *dev, ieee802154_trx_state_t state)
{
    return _set_trx_state(dev, state);

}
static int _confirm_set_trx_state(ieee802154_dev_t *dev)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    if (at86rf2xx_get_internal_state(_dev) == AT86RF2XX_STATE_IN_PROGRESS) {
        return -EAGAIN;
    }
    return 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    at86rf2xx_assert_awake(_dev);

    return 0;
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    uint8_t trx_status = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__TRX_STATUS);
    if ((trx_status & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS) != AT86RF2XX_TRX_STATUS__TRX_OFF)
    {
        return -EAGAIN;
    }

    /* Go to sleep. The HAL might take some time to call the `start` function
     * and we don't want to waste power */
    at86rf2xx_reset(_dev);
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__IRQ_MASK,
                        AT86RF2XX_IRQ_STATUS_MASK__RX_START |
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END);

    at86rf2xx_reg_read(_dev, AT86RF2XX_REG__IRQ_STATUS);
    _dev->trx_state = IEEE802154_TRX_STATE_TRX_OFF;
    return 0;
}

void at86rf2xx_irq_handler(void *dev)
{
    uint8_t irq_mask;
    uint8_t state;

    at86rf2xx_t *at86rf2xx_dev = dev;
    ieee802154_dev_t *_dev = dev;
    /* If transceiver is sleeping register access is impossible and frames are
     * lost anyway, so return immediately.
     */
    if (_is_sleep(_dev)) {
        return;
    }

    state = at86rf2xx_dev->trx_state;
    /* read (consume) device status */
    irq_mask = at86rf2xx_clear_irq_flags(at86rf2xx_dev);

    if (at86rf2xx_irq_has_rx_start(irq_mask)) {
        _dev->cb(_dev, IEEE802154_RADIO_INDICATION_RX_START);
    }

    if (at86rf2xx_irq_has_trx_end(irq_mask)) {
        if (state == IEEE802154_TRX_STATE_RX_ON) {
            _dev->cb(_dev, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        else {
            _dev->cb(_dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
        }
    }

    if (at86rf2xx_irq_has_awake_end(irq_mask)) {

        _dev->cb(_dev, IEEE802154_RADIO_CONFIRM_CCA);
    }
}

static int _confirm_transmit(ieee802154_dev_t *dev, ieee802154_tx_info_t *info)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    int radio_state = at86rf2xx_get_internal_state(_dev);
    uint8_t mask = radio_state & 0xF;
    uint8_t tx_status = TX_STATUS_SUCCESS;

    if (mask == 0x2) {
        return -EAGAIN;
    }

    if (info == 0) {
        return 0;
    }

    uint8_t trac_status;
    trac_status = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__TRX_STATE)
                  & AT86RF2XX_TRX_STATE_MASK__TRAC;
    switch (trac_status) {
                case AT86RF2XX_TRX_STATE__TRAC_SUCCESS_DATA_PENDING:
                    tx_status = TX_STATUS_FRAME_PENDING;
                    /* FALL-THRU */
                case AT86RF2XX_TRX_STATE__TRAC_SUCCESS:
                    DEBUG("[at86rf2xx] TX SUCCESS\n");
                    break;
                case AT86RF2XX_TRX_STATE__TRAC_NO_ACK:
                    DEBUG("[at86rf2xx] TX NO_ACK\n");
                        tx_status = TX_STATUS_NO_ACK;
                        break;
                case AT86RF2XX_TRX_STATE__TRAC_CHANNEL_ACCESS_FAILURE:
                    DEBUG("[at86rf2xx] TX_CHANNEL_ACCESS_FAILURE\n");
                        tx_status = TX_STATUS_MEDIUM_BUSY;
                        break;
                default:
                    DEBUG("[at86rf2xx] Unhandled TRAC_STATUS: %d\n",
                          trac_status >> 5);
                    return -EINVAL;
            }
    info->status = tx_status;
    if (IS_ACTIVE(AT86RF2XX_HAVE_RETRIES)) {
        info->retrans = (at86rf2xx_reg_read(_dev, AT86RF2XX_REG__XAH_CTRL_2)
                           & AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_MASK) >>
                          AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_OFFSET;
    }
    return 0;
}

static int _set_rx_mode(ieee802154_dev_t *dev, ieee802154_rx_mode_t mode)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;

    bool ack = true;
    bool promisc = false;
    switch (mode) {
        case IEEE802154_RX_AACK_DISABLED:
            ack = false;
            break;
        case IEEE802154_RX_AACK_ENABLED:
            at86rf2xx_set_frame_pending(_dev, false);
            break;
        case IEEE802154_RX_AACK_FRAME_PENDING:
            at86rf2xx_set_frame_pending(_dev, true);
            break;
        case IEEE802154_RX_PROMISC:
            ack = false;
            promisc = true;
            break;
        default:
            return -ENOTSUP;
    }

    at86rf2xx_set_auto_ack(_dev, ack);
    at86rf2xx_set_promiscuous(_dev, promisc);
    return 0;
}

static int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    at86rf2xx_t *_dev = (at86rf2xx_t*) dev;
    uint8_t phy_cc_cca = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__PHY_CC_CCA);

    uint8_t tmp = 0;

    switch (mode) {
        case IEEE802154_CCA_MODE_ED_THRESHOLD:
            tmp = 1;
            break;
        case IEEE802154_CCA_MODE_CARRIER_SENSING:
            tmp = 2;
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_AND_CS:
            tmp = 3;
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_OR_CS:
            tmp = 0;
            break;
    }

    phy_cc_cca &= ~AT86RF2XX_PHY_CC_CCA_MASK__CCA_MODE;
    phy_cc_cca |= tmp << AT86RF2XX_PHY_CC_CCA_POS__CCA_MODE;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__PHY_CC_CCA, phy_cc_cca); 

    printf("%02x\n", phy_cc_cca);
    printf("%02x\n", at86rf2xx_reg_read(_dev, AT86RF2XX_REG__CCA_THRES));

    return 0;
}

static const ieee802154_radio_ops_t at86rf2xx_ops = {
    .write = _write,
    .request_transmit = _request_transmit,
    .len = _len,
    .indication_rx = _indication_rx,
    .set_cca_threshold = set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .off = _off,
    .get_cap = _get_cap,
    .set_hw_addr_filter = set_hw_addr_filter,
    .set_frame_retrans = set_frame_retrans,
    .set_csma_params = set_csma_params,
    .confirm_transmit = _confirm_transmit,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .request_cca = _request_cca,
    .confirm_cca = _confirm_cca,
    .request_set_trx_state = _request_set_trx_state,
    .confirm_set_trx_state = _confirm_set_trx_state,
    .set_rx_mode = _set_rx_mode,
};
