#include "at86rf2xx.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"
#include "net/ieee802154/radio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

extern void at86rf2xx_irq_handler(void *arg);

static const ieee802154_radio_ops_t at86rf2xx_ops;

static int _write(ieee802154_dev_t *dev, const iolist_t *psdu)
{
    DEBUG("at86rf2xx_rf_ops: write\n");
    size_t len = 0;

    at86rf2xx_t *_dev = dev->priv;

    _dev->tx_frame_len = IEEE802154_FCS_LEN;

    /* load packet data into FIFO */
    for (const iolist_t *iol = psdu; iol; iol = iol->iol_next) {
        /* current packet data + FCS too long */
        if ((len + iol->iol_len + 2) > AT86RF2XX_MAX_PKT_LENGTH) {
            DEBUG("[at86rf2xx] error: packet too large (%u byte) to be send\n",
                  (unsigned)len + 2);
            return -EOVERFLOW;
        }

        if (iol->iol_len) {
            len = at86rf2xx_tx_load(_dev, iol->iol_base, iol->iol_len, len);
        }
    }
    return 0;
}

static int _request_transmit(ieee802154_dev_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: request_transmit\n");
    at86rf2xx_t *_dev = dev->priv;
    at86rf2xx_tx_exec(_dev);
    return 0;
}

static int _confirm_transmit(ieee802154_dev_t *dev, ieee802154_tx_info_t *info)
{
    DEBUG("at86rf2xx_rf_ops: confirm_transmit\n");
    at86rf2xx_t *_dev = dev->priv;
    uint8_t status = at86rf2xx_get_status(_dev);
    if (status == AT86RF2XX_STATE_BUSY_TX_ARET || status == AT86RF2XX_STATE_BUSY_TX) {
        return -EAGAIN;
    }

    if (info) {
        uint8_t trac_status = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__TRX_STATE) & AT86RF2XX_TRX_STATE_MASK__TRAC;
#if AT86RF2XX_HAVE_RETRIES && defined(AT86RF2XX_REG__XAH_CTRL_2)
        info->retrans = (at86rf2xx_reg_read(_dev, AT86RF2XX_REG__XAH_CTRL_2)
                       & AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_MASK) >>
                      AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_OFFSET;
#endif
        switch(trac_status) {
            case AT86RF2XX_TRX_STATE__TRAC_SUCCESS:
                info->status = TX_STATUS_SUCCESS;
                break;
            case AT86RF2XX_TRX_STATE__TRAC_SUCCESS_DATA_PENDING:
                info->status = TX_STATUS_FRAME_PENDING;
                break;
            case AT86RF2XX_TRX_STATE__TRAC_CHANNEL_ACCESS_FAILURE:
                info->status = TX_STATUS_MEDIUM_BUSY;
                break;
            case AT86RF2XX_TRX_STATE__TRAC_NO_ACK:
                info->status = TX_STATUS_NO_ACK;
                break;
            default:
                DEBUG("[at86rf2xx] Unhandled TRAC_STATUS: %d\n", trac_status >> 5);

        }
    }
    return 0;
}

static int _len(ieee802154_dev_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: len\n");
    uint8_t phr;
    at86rf2xx_t *_dev = dev->priv;
#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
    phr = TST_RX_LENGTH;
#else
    at86rf2xx_sram_read(_dev, 0, &phr, 1);
#endif
    return (phr & 0x7f) - 2;
}

/* The radio should be woken up */
static int _read(ieee802154_dev_t *dev, void *buf, size_t max_size, ieee802154_rx_info_t *info)
{
    DEBUG("at86rf2xx_rf_ops: read\n");
    uint8_t phr;
    size_t pkt_len;

    at86rf2xx_t *_dev = dev->priv;

    /* start frame buffer access */
    at86rf2xx_fb_start(_dev);

    /* get the size of the received packet */
#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
    phr = TST_RX_LENGTH;
#else
    at86rf2xx_fb_read(_dev, &phr, 1);
#endif

    /* ignore MSB (refer p.80) and subtract length of FCS field */
    pkt_len = (phr & 0x7f) - 2;

    /* not enough space in buf */
    if (pkt_len > max_size) {
        at86rf2xx_fb_stop(_dev);
        return -ENOBUFS;
    }

    if (!buf) {
        at86rf2xx_fb_stop(_dev);
        return 0;
    }
    /* copy payload */
    at86rf2xx_fb_read(_dev, (uint8_t *)buf, pkt_len);

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

#if defined(MODULE_AT86RF231) || defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
        /* AT86RF231 does not provide ED at the end of the frame buffer, read
         * from separate register instead */
        at86rf2xx_fb_stop(_dev);
        ed = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__PHY_ED_LEVEL);
#else
        at86rf2xx_fb_read(_dev, &ed, 1);
        at86rf2xx_fb_stop(_dev);
#endif
        info->rssi = RSSI_BASE_VAL + ed;
        DEBUG("[at86rf2xx] LQI:%d high is good, RSSI:%d high is either good or"
              "too much interference.\n", info->lqi, info->rssi);
    }
    else {
        at86rf2xx_fb_stop(_dev);
    }

    return pkt_len;
}

static int _request_cca(ieee802154_dev_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: request_cca\n");
    at86rf2xx_t *_dev = dev->priv;
    uint8_t reg;
    /* TODO: Check basic mode */
    at86rf2xx_set_state(_dev, AT86RF2XX_STATE_RX_ON);
    /* Perform CCA */
    reg = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__PHY_CC_CCA);
    reg |= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__PHY_CC_CCA, reg);
    return 0;
}

static int _confirm_cca(ieee802154_dev_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: confirm_cca\n");
    uint8_t reg;
    at86rf2xx_t *_dev = dev->priv;
    reg = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__TRX_STATUS);
    if ((reg & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE) == 0) {
        return -EAGAIN;
    }
    at86rf2xx_set_state(_dev, AT86RF2XX_STATE_RX_AACK_ON);
    return !!(reg & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS);
}

static int _set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    DEBUG("at86rf2xx_rf_ops: set_cca_threshold to %i\n", threshold);
    at86rf2xx_t *_dev = dev->priv;
    at86rf2xx_set_cca_threshold(_dev, threshold);
    //phy_cc_cca |= (channel & AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    //at86rf2xx_reg_write(_dev, AT86RF2XX_REG__PHY_CC_CCA, phy_cc_cca);
    return 0;
}

int at86rf2xx_config_phy_alt(at86rf2xx_t *dev, uint8_t channel,
        uint8_t page, int8_t txpower);

static int _config_phy(ieee802154_dev_t *dev, const ieee802154_phy_conf_t *conf)
{
    DEBUG("at86rf2xx_rf_ops: config_phy to page %i, channel %i and txpow %i\n", conf->page, conf->channel, conf->pow);
    at86rf2xx_t *_dev = dev->priv;
    return at86rf2xx_config_phy_alt(_dev, conf->channel, conf->page, conf->pow);
}

static int _request_set_trx_state(ieee802154_dev_t *dev, ieee802154_trx_state_t state)
{
    /* TODO: Fix transition between RX and TX!! */
    /* TODO: Add basic mode support */
    DEBUG("at86r2xx_rf_ops: request_set_trx_state to ");
    at86rf2xx_t *_dev = dev->priv;

    /* TODO: Implement for periph version */

    /* Prevent issue described in https://github.com/RIOT-OS/RIOT/pull/11256 */
    if ((at86rf2xx_get_status(_dev) == AT86RF2XX_PHY_STATE_RX_BUSY) || _dev->pending_irq) {
        return -EBUSY;
    }

    int int_state;
    switch(state) {
        case IEEE802154_TRX_STATE_TRX_OFF:
            int_state = AT86RF2XX_STATE_TRX_OFF;
            DEBUG("TRX_OFF");
            break;
        case IEEE802154_TRX_STATE_RX_ON:
            int_state = AT86RF2XX_STATE_RX_AACK_ON;
            DEBUG("RX_ON");
            /*TODO: Disable race condition */
            break;
        case IEEE802154_TRX_STATE_TX_ON:
            DEBUG("TX_ON");
            int_state = AT86RF2XX_STATE_TX_ARET_ON;
            break;
        default:
            return -EINVAL;
    }

    DEBUG("\n");


    /* This is also required to prevent https://github.com/RIOT-OS/RIOT/pull/11256.
     * Disabling the RX path (RX_SYN) is not enough to prevent the race condition :/
     * If getting the internal transceiver status and checking if there are
     * pending IRQs doesn't 
     */
    if (state != IEEE802154_TRX_STATE_TRX_OFF) {
        at86rf2xx_reg_write(_dev, AT86RF2XX_REG__TRX_STATE, AT86RF2XX_TRX_STATE__FORCE_PLL_ON);
    }

    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__TRX_STATE, int_state);
    return 0;
}

static int _confirm_set_trx_state(ieee802154_dev_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: confirm_set_trx_state\n");
    at86rf2xx_t *_dev = dev->priv;
    assert(at86rf2xx_get_status(_dev) != AT86RF2XX_PHY_STATE_RX_BUSY);
    if (at86rf2xx_get_status(_dev) == AT86RF2XX_STATE_IN_PROGRESS) {
        return -EAGAIN;
    }
    return 0;
}

static int _set_hw_addr_filter(ieee802154_dev_t *dev, const network_uint16_t *short_addr, const eui64_t *ext_addr, const uint16_t *pan_id)
{
    DEBUG("at86rf2xx_rf_ops: set_hw_addr_filter. ");
    at86rf2xx_t *_dev = dev->priv;
    if (pan_id) {
        le_uint16_t le_pan = byteorder_btols(byteorder_htons(*pan_id));
        at86rf2xx_reg_write(_dev, AT86RF2XX_REG__PAN_ID_0, le_pan.u8[0]);
        at86rf2xx_reg_write(_dev, AT86RF2XX_REG__PAN_ID_1, le_pan.u8[1]);
        DEBUG("PANID: %04x ", *pan_id);

    }

    if (short_addr) {
        at86rf2xx_reg_write(_dev, AT86RF2XX_REG__SHORT_ADDR_0,
                            short_addr->u8[1]);
        at86rf2xx_reg_write(_dev, AT86RF2XX_REG__SHORT_ADDR_1,
                            short_addr->u8[0]);
        DEBUG("SHORT_ADDR: %04x ", byteorder_ntohs(*short_addr));
    }

    if (ext_addr) {
        DEBUG(" EXT_ADDR: ");
        for (int i = 0; i < 8; i++) {
            at86rf2xx_reg_write(_dev, (AT86RF2XX_REG__IEEE_ADDR_0 + i),
                    ext_addr->uint8[IEEE802154_LONG_ADDRESS_LEN - 1 - i]);
                DEBUG("%02x", ext_addr->uint8[i]);
        }
        DEBUG("\n");
    }

    return 0;
}

static int _set_frame_retrans(ieee802154_dev_t *dev, uint8_t retries)
{
    DEBUG("at86rf2xx_rf_ops: set_frame_retrans to %i\n", retries);
    at86rf2xx_t *_dev = dev->priv;
    uint8_t tmp = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__XAH_CTRL_0);

    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_FRAME_RETRIES);
    tmp |= ((retries > 7) ? 7 : retries) << 4;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);
    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd, int8_t retries)
{
    DEBUG("at86rf2xx_rf_ops: set_csma_params. ");
    at86rf2xx_t *_dev = dev->priv;
    retries = (retries > 5) ? 5 : retries;  /* valid values: 0-5 */
    retries = (retries < 0) ? 7 : retries;  /* max < 0 => disable CSMA (set to 7) */
    DEBUG("Retries: %i ", retries);
    uint8_t tmp = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES);
    tmp |= (retries << 1);
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);

    if(bd) {
        uint8_t max = bd->max;
        uint8_t min = bd->min;
        DEBUG("max: %i ", max);
        DEBUG("min: %i ", min);
        max = (max > 8) ? 8 : max;
        min = (min > max) ? max : min;
        at86rf2xx_reg_write(_dev, AT86RF2XX_REG__CSMA_BE, (max << 4) | (min));
    }
    DEBUG("\n");

    return 0;
}

static int _off(ieee802154_dev_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: off\n");
    at86rf2xx_t *_dev = dev->priv;
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__TRX_STATE, AT86RF2XX_STATE_FORCE_TRX_OFF);
    /* TODO: Disable interrupts */
    /* Discard all IRQ flags, framebuffer is lost anyway */
    at86rf2xx_reg_read(_dev, AT86RF2XX_REG__IRQ_STATUS);
    /* Go to SLEEP mode from TRX_OFF */
#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
    /* reset interrupts states in device */
    _dev->irq_status = 0;
    /* Setting SLPTR bit brings radio transceiver to sleep in in TRX_OFF*/
    *AT86RF2XX_REG__TRXPR |= (AT86RF2XX_TRXPR_SLPTR);
#else
    gpio_set(_dev->params.sleep_pin);
#endif
    return 0;
}

int _request_on(ieee802154_dev_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: request_on\n");
    at86rf2xx_t *_dev = dev->priv;
    gpio_irq_enable(_dev->params.int_pin);
    /* enable interrupts */
    at86rf2xx_reg_write(_dev, AT86RF2XX_REG__IRQ_MASK,
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END | AT86RF2XX_IRQ_STATUS_MASK__RX_START);


#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
    /* Setting SLPTR bit in TRXPR to 0 returns the radio transceiver
     * to the TRX_OFF state */
    *AT86RF2XX_REG__TRXPR &= ~(AT86RF2XX_TRXPR_SLPTR);
#else
    gpio_clear(_dev->params.sleep_pin);
#endif
    return 0;
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: confirm_on\n");
    at86rf2xx_t *_dev = dev->priv;
    int status = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__TRX_STATUS)
                     & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;

     if (status != AT86RF2XX_TRX_STATUS__TRX_OFF)
     {
         return -EAGAIN;

     }
    at86rf2xx_reg_read(_dev, AT86RF2XX_REG__IRQ_STATUS);
    _dev->pending_irq = false;
    return 0;
}

static int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    (void) dev;
    /* TODO: */
    if (mode != IEEE802154_CCA_MODE_ED_THRESHOLD) {
        DEBUG("at86rf2xx_rf_ops: CCA mode not supported\n");
        return -ENOTSUP;
     }
    DEBUG("at86rf2xx_rf_ops: set_cca_mode to ED Threshold\n");
    return 0;
}

static int _set_rx_mode(ieee802154_dev_t *dev, ieee802154_rx_mode_t mode)
{
    DEBUG("at86rf2xx_rf_ops: set_rx_mode to ");
    at86rf2xx_t *_dev = dev->priv;
    bool ack_pending = false;
    bool auto_ack = false;
    bool promisc = false;
    switch(mode) {
        case IEEE802154_RX_AACK_ENABLED:
            DEBUG("IEEE802154_RX_AACK_ENABLED\n");
            auto_ack = true;
            break;
        case IEEE802154_RX_AACK_FRAME_PENDING:
            DEBUG("IEEE802154_RX_AACK_FRAME_PENDING\n");
            auto_ack = true;
            ack_pending = true;
            break;
        case IEEE802154_RX_PROMISC:
            DEBUG("IEEE802154_RX_PROMISC\n");
            promisc = true;
            break;
        case IEEE802154_RX_WAIT_FOR_ACK:
            DEBUG("IEEE802154_RX_WAIT_FOR_ACK\n");
            return -ENOTSUP;
            break;
        case IEEE802154_RX_AACK_DISABLED:
            DEBUG("IEEE802154_RX_AACK_DISABLED\n");
            break;
    }

    at86rf2xx_set_option(_dev, AT86RF2XX_OPT_AUTOACK, auto_ack);
    at86rf2xx_set_option(_dev, AT86RF2XX_OPT_ACK_PENDING, ack_pending);
    at86rf2xx_set_option(_dev, AT86RF2XX_OPT_PROMISCUOUS, promisc);
    return 0;
}

void at86rf2xx_init_hal(at86rf2xx_t *dev, ieee802154_dev_t *hal)
{
    hal->driver = &at86rf2xx_ops;
    hal->priv = dev;
}

/* TODO */
int at86rf2xx_init(at86rf2xx_t *dev, const at86rf2xx_params_t *params, void *arg)
{
#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
    (void) params;
    /* set all interrupts off */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, 0x00);
#else
    /* initialize device descriptor */
    dev->params = *params;
#endif

#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
    at86rfmega_dev = dev;
#else
    /* initialize GPIOs */
    spi_init_cs(dev->params.spi, dev->params.cs_pin);
    gpio_init(dev->params.sleep_pin, GPIO_OUT);
    gpio_clear(dev->params.sleep_pin);
    gpio_init(dev->params.reset_pin, GPIO_OUT);
    gpio_set(dev->params.reset_pin);
    gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, at86rf2xx_irq_handler, arg);
    gpio_irq_disable(dev->params.int_pin);
    /* DEBUG PINS */
    gpio_init(ISR_PIN, GPIO_OUT);
    gpio_init(TX_PIN, GPIO_OUT);
    gpio_init(RX_PIN, GPIO_OUT);
    gpio_init(STATE_PIN, GPIO_OUT);

    /* Intentionally check if bus can be acquired,
       since getbus() drops the return value */
    if (spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                                                dev->params.spi_clk) < 0) {
        DEBUG("[at86rf2xx] error: unable to acquire SPI bus\n");
        return -EIO;
    }
    spi_release(dev->params.spi);
#endif

    /* reset hardware into a defined state */
    at86rf2xx_hardware_reset(dev);

    /* test if the device is responding */
    if (at86rf2xx_reg_read(dev, AT86RF2XX_REG__PART_NUM) != AT86RF2XX_PARTNUM) {
        DEBUG("[at86rf2xx] error: unable to read correct part number\n");
        return -ENOTSUP;
    }

    /* reset device to default values and put it into RX state */
    //at86rf2xx_reset(dev);

    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2,
                        AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);

#if !defined(MODULE_AT86RFA1) && !defined(MODULE_AT86RFR2)
    /* don't populate masked interrupt flags to IRQ_STATUS register */
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_1);
    tmp &= ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_1, tmp);
#endif

    /* configure smart idle listening feature */
    at86rf2xx_enable_smart_idle(dev);

    /* disable clock output to save power */
    at86rf2xx_disable_clock_output(dev);

    /* TODO!! */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_FORCE_TRX_OFF);
    /* TODO: ??? */
    /* enable TX start interrupt for retry counter */
#ifdef AT86RF2XX_REG__IRQ_MASK1
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK1,
                             AT86RF2XX_IRQ_STATUS_MASK1__TX_START);
#endif

    /* TODO: ???? */
#if IS_USED(MODULE_IEEE802154_SECURITY) && \
    IS_USED(MODULE_AT86RF2XX_AES_SPI)
    dev->netdev.sec_ctx.dev.cipher_ops = &_at86rf2xx_cipher_ops;
    dev->netdev.sec_ctx.dev.ctx = dev;
    /* All configurations of the security module, the SRAM content,
       and keys are reset during DEEP_SLEEP or RESET state. */
    at86rf2xx_aes_key_write_encrypt(dev,
        dev->netdev.sec_ctx.cipher.context.context);
#endif

    return 0;
}

void at86rf2xx_task_handler(ieee802154_dev_t *dev)
{
    uint8_t irq_mask;
    uint8_t state;

    at86rf2xx_t *_dev = dev->priv;
    /* If transceiver is sleeping register access is impossible and frames are
     * lost anyway, so return immediately.
     */
    /* TODO: Clear flags on sleep... */
    state = at86rf2xx_get_status(_dev);
    if (state == AT86RF2XX_STATE_SLEEP) {
        return;
    }

    /* read (consume) device status */
#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
    irq_mask = _dev->irq_status;
    _dev->irq_status = 0;
#else
    irq_mask = at86rf2xx_reg_read(_dev, AT86RF2XX_REG__IRQ_STATUS);
    _dev->pending_irq = false;
#endif

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__RX_START) {
        DEBUG("[at86rf2xx] EVT - RX_START\n");
        dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_START);
    }

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__TRX_END) {
        /* TODO: */
       if ((state == AT86RF2XX_PHY_STATE_RX)
            || (state == AT86RF2XX_PHY_STATE_RX_BUSY)) {
            DEBUG("[at86rf2xx] EVT - RX_END\n");
            dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);

        }
        else if (state == AT86RF2XX_PHY_STATE_TX) {
            /* check for more pending TX calls and return to idle state if
             * there are none */
            DEBUG("[at86rf2xx] EVT - TX_END");
            dev->cb(dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
        }
        else {
        }
    }
}

static const ieee802154_radio_ops_t at86rf2xx_ops = {
    .caps =  IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_PHY_OQPSK
          | IEEE802154_CAP_FRAME_RETRANS_INFO
          | IEEE802154_CAP_REG_RETENTION
          | IEEE802154_CAP_FRAME_RETRANS,

    .write = _write,
    .read = _read,
    .request_transmit = _request_transmit,
    .confirm_transmit = _confirm_transmit,
    .len = _len,
    .off = _off,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .request_set_trx_state = _request_set_trx_state,
    .confirm_set_trx_state = _confirm_set_trx_state,
    .request_cca = _request_cca,
    .confirm_cca = _confirm_cca,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_hw_addr_filter = _set_hw_addr_filter,
    .set_csma_params = _set_csma_params,
    .set_rx_mode = _set_rx_mode, /* OK! */
    .set_frame_retrans = _set_frame_retrans,
};
