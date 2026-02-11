/*
 * Copyright (C) 2023 HAW Hamburg
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
 * @brief       Implementation of 802.15.4 Radio HAL interface for AT86RF2xx
 *              transceivers.
 *
 * @author      José I. Álamos <jose.alamos@haw-hamburg.de>
 * @}
 */

#include "at86rf2xx.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"
#include "macros/utils.h"
#include "net/ieee802154/radio.h"
#include "sched.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "thread.h"
#include "atomic_utils.h"
#if IS_USED(MODULE_AT86RF2XX_AES_SPI)
#include "at86rf2xx_aes.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

static const ieee802154_radio_ops_t at86rf2xx_ops;
static ieee802154_dev_t *at86rf2xx_periph;

#if IS_USED(MODULE_AT86RF2XX_AES_SPI) && \
    IS_USED(MODULE_IEEE802154_SECURITY)
/**
 * @brief   Pass the 802.15.4 encryption key to the transceiver hardware
 *
 * @param[in] dev               Abstract security device descriptor
 * @param[in] key               Encryption key to be used
 * @param[in] key_size          Size of the encryption key in bytes
 */
static void _at86rf2xx_set_key(ieee802154_sec_dev_t *dev,
                               const uint8_t *key, uint8_t key_size)
{
    (void)key_size;
    ieee802154_dev_t *hal = dev->ctx;
    at86rf2xx_aes_key_write_encrypt(hal->priv, key);
}

/**
 * @brief   Compute CBC-MAC from IEEE 802.15.4 security context
 *
 * @param[in]       dev         Abstract security device descriptor
 * @param[out]      cipher      Buffer to store cipher blocks
 * @param[in]       iv          Initial vector
 * @param[in]       plain       Input data blocks
 * @param[in]       nblocks     Number of blocks
 */
static void _at86rf2xx_cbc(const ieee802154_sec_dev_t *dev,
                           uint8_t *cipher,
                           uint8_t *iv,
                           const uint8_t *plain,
                           uint8_t nblocks)
{
    ieee802154_dev_t *hal = dev->ctx;

    at86rf2xx_aes_cbc_encrypt(hal->priv,
                              (aes_block_t *)cipher,
                              NULL,
                              iv,
                              (aes_block_t *)plain,
                              nblocks);
}

/**
 * @brief   Perform ECB encryption
 *
 * @param[in]       dev         Abstract security device descriptor
 * @param[out]      cipher      Output cipher blocks
 * @param[in]       plain       Plain blocks
 * @param[in]       nblocks     Number of blocks
 */
static void _at86rf2xx_ecb(const ieee802154_sec_dev_t *dev,
                           uint8_t *cipher,
                           const uint8_t *plain,
                           uint8_t nblocks)
{
    ieee802154_dev_t *hal = dev->ctx;

    at86rf2xx_aes_ecb_encrypt(hal->priv,
                              (aes_block_t *)cipher,
                              NULL,
                              (aes_block_t *)plain,
                              nblocks);

}
/**
 * @brief   Struct that contains IEEE 802.15.4 security operations
 *          which are implemented, using the transceiver´s hardware
 *          crypto capabilities
 */
static const ieee802154_radio_cipher_ops_t _at86rf2xx_cipher_ops = {
    .set_key = _at86rf2xx_set_key,
    .ecb = _at86rf2xx_ecb,
    .cbc = _at86rf2xx_cbc
};
#endif /* IS_USED(MODULE_AT86RF2XX_AES_SPI) && \
          IS_USED(MODULE_IEEE802154_SECURITY) */

static int _write(ieee802154_dev_t *hal, const iolist_t *psdu)
{
    DEBUG("at86rf2xx_rf_ops: write\n");
    uint8_t len = 0;

    at86rf2xx_t *dev = hal->priv;
    mutex_lock(&dev->lock);
    /* load packet data into FIFO */
    for (const iolist_t *iol = psdu; iol; iol = iol->iol_next) {
        /* current packet data + FCS too long */
        if ((len + iol->iol_len + IEEE802154_FCS_LEN) > AT86RF2XX_MAX_PKT_LENGTH) {
            DEBUG("[at86rf2xx] error: packet too large (%" PRIuSIZE
                  " byte) to be send\n", (size_t)len + IEEE802154_FCS_LEN);

            mutex_unlock(&dev->lock);
            return -EOVERFLOW;
        }

        if (iol->iol_len) {
            at86rf2xx_sram_write(dev, len + 1, iol->iol_base, iol->iol_len);
            len += iol->iol_len;
        }
    }

    len += IEEE802154_FCS_LEN;
    assert(len <= IEEE802154_FRAME_LEN_MAX);
    /* write frame length field in FIFO */
    at86rf2xx_sram_write(dev, 0, &len, 1);
    mutex_unlock(&dev->lock);

    return 0;
}

static int _read(ieee802154_dev_t *hal, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    at86rf2xx_t *dev = hal->priv;
    uint8_t phr;
    size_t pkt_len;

    mutex_lock(&dev->lock);
    /* start frame buffer access */
    at86rf2xx_fb_start(dev);

    /* get the size of the received packet */
    phr = at86rf2xx_get_rx_len(dev);

    /* ignore MSB (refer p.80) and subtract length of FCS field */
    pkt_len = (phr & 0x7f) - IEEE802154_FCS_LEN;

    /* not enough space in buf */
    if (pkt_len > size) {
        at86rf2xx_fb_stop(dev);
        mutex_unlock(&dev->lock);
        return -ENOBUFS;
    }

    if (!buf) {
        at86rf2xx_fb_stop(dev);
        mutex_unlock(&dev->lock);
        return 0;
    }

    /* copy payload */
    at86rf2xx_fb_read(dev, (uint8_t *)buf, pkt_len);

    /* Ignore FCS but advance fb read - we must give a temporary buffer here,
     * as we are not allowed to issue SPI transfers without any buffer */
    uint8_t tmp[IEEE802154_FCS_LEN];
    at86rf2xx_fb_read(dev, tmp, IEEE802154_FCS_LEN);
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
        at86rf2xx_fb_read(dev, &(info->lqi), 1);

#if AT86RF2XX_HAVE_ED_REGISTER
        /* AT86RF231 does not provide ED at the end of the frame buffer, read
         * from separate register instead */
        at86rf2xx_fb_stop(dev);
        ed = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_ED_LEVEL);
#else
        at86rf2xx_fb_read(dev, &ed, 1);
        at86rf2xx_fb_stop(dev);
#endif
        info->rssi = RSSI_BASE_VAL + ed;
        DEBUG("[at86rf2xx] LQI:%d high is good, RSSI:%d high is either good or "
              "too much interference.\n", info->lqi, info->rssi);
#if AT86RF2XX_IS_PERIPH
        /* AT86RF2XX_IS_PERIPH means the MCU is ATmegaRFR2 that has symbol counter */
        {
            uint32_t rx_sc;
            rx_sc = at86rf2xx_get_sc(dev);

            /* convert counter value to ns */
            uint64_t timestamp = SC_TO_NS * (uint64_t)rx_sc;
            info->timestamp = timestamp;
            DEBUG("[at86rf2xx] CS: %" PRIu32 " timestamp: %" PRIu32 ".%09" PRIu32 " ",
                  rx_sc, (uint32_t)(info->timestamp / NS_PER_SEC),
                  (uint32_t)(info->timestamp % NS_PER_SEC));
        }
#endif
    }
    else {
        at86rf2xx_fb_stop(dev);
    }

    mutex_unlock(&dev->lock);
    return pkt_len;
}

static int _request_on(ieee802154_dev_t *hal)
{
    DEBUG("at86rf2xx_rf_ops: request_on\n");
    at86rf2xx_t *dev = hal->priv;
    mutex_lock(&dev->lock);
#if !AT86RF2XX_IS_PERIPH
    gpio_irq_enable(dev->params.int_pin);
#endif
    /* enable interrupts */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK,
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END | AT86RF2XX_IRQ_STATUS_MASK__RX_START);

#if AT86RF2XX_IS_PERIPH
    /* Setting SLPTR bit in TRXPR to 0 returns the radio transceiver
     * to the TRX_OFF state */
    *AT86RF2XX_REG__TRXPR &= ~(AT86RF2XX_TRXPR_SLPTR);
#else
    gpio_clear(dev->params.sleep_pin);
#endif

    mutex_unlock(&dev->lock);
    return 0;
}

static int _confirm_on(ieee802154_dev_t *hal)
{
    DEBUG("at86rf2xx_rf_ops: confirm_on\n");
    at86rf2xx_t *dev = hal->priv;
    mutex_lock(&dev->lock);
    int status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS)
                 & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;

    if (status != AT86RF2XX_TRX_STATUS__TRX_OFF) {
        mutex_unlock(&dev->lock);
        return -EAGAIN;
    }
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);
    mutex_unlock(&dev->lock);
    return 0;
}

static int _len(ieee802154_dev_t *hal)
{
    DEBUG("at86rf2xx_rf_ops: len\n");
    at86rf2xx_t *dev = hal->priv;
    mutex_lock(&dev->lock);
    uint8_t phr = at86rf2xx_peek_rx_len(dev);
    mutex_unlock(&dev->lock);
    return (phr & 0x7f) - 2;
}

static int _off(ieee802154_dev_t *hal)
{
    DEBUG("at86rf2xx_rf_ops: off\n");
    at86rf2xx_t *dev = hal->priv;
    mutex_lock(&dev->lock);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, AT86RF2XX_STATE_FORCE_TRX_OFF);

    /* Discard all IRQ flags, framebuffer is lost anyway */
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);
    /* Go to SLEEP mode from TRX_OFF */
#if AT86RF2XX_IS_PERIPH
    /* reset interrupts states in device */
    /* Setting SLPTR bit brings radio transceiver to sleep in in TRX_OFF*/
    *AT86RF2XX_REG__TRXPR |= (AT86RF2XX_TRXPR_SLPTR);
#else
    gpio_irq_disable(dev->params.int_pin);
    gpio_set(dev->params.sleep_pin);
#endif
    mutex_unlock(&dev->lock);
    return 0;
}
static int _request_set_rx(at86rf2xx_t *dev)
{
    DEBUG("at86r2xx_rf_ops: request_set_rx\n");
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, AT86RF2XX_PHY_STATE_RX);
    return 0;
}

static int _request_set_tx(at86rf2xx_t *dev, bool force)
{
    DEBUG("at86r2xx_rf_ops: request_set_tx\n");
    /* Prevent issue described in https://github.com/RIOT-OS/RIOT/pull/11256 */
    if (at86rf2xx_get_status(dev) == AT86RF2XX_PHY_STATE_RX_BUSY && !force) {
        return -EBUSY;
    }
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, AT86RF2XX_TRX_STATE__FORCE_PLL_ON);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, AT86RF2XX_PHY_STATE_TX);
    return 0;
}

static int _request_cca(at86rf2xx_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: request_cca\n");
    uint8_t reg;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, AT86RF2XX_TRX_STATE__FORCE_PLL_ON);
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_ON);

    reg = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_MASK);
    reg |= AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, reg);
    /* Perform CCA */
    reg = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    reg |= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, reg);
    return 0;
}

static int _request_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx)
{
    at86rf2xx_t *dev = hal->priv;

    int res = -ENOTSUP;

    mutex_lock(&dev->lock);
    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
#if AT86RF2XX_HAVE_RETRIES && AT86RF2XX_IS_PERIPH
        dev->tx_retries = -1;
#endif
        /* trigger sending of pre-loaded frame */
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                            AT86RF2XX_TRX_STATE__TX_START);
        res = 0;
        break;
    case IEEE802154_HAL_OP_SET_RX:
        res = _request_set_rx(dev);
        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        res = _request_set_tx(dev, *((bool *)ctx));
        break;
    case IEEE802154_HAL_OP_CCA:
        res = _request_cca(dev);
        break;
    default:
        assert(false);
        break;
    }
    mutex_unlock(&dev->lock);
    return res;
}

static int _confirm_transmit(at86rf2xx_t *dev, ieee802154_tx_info_t *info)
{
    DEBUG("at86rf2xx_rf_ops: confirm_transmit\n");
    uint8_t status = at86rf2xx_get_status(dev);
    if (status == AT86RF2XX_STATE_BUSY_TX_ARET || status == AT86RF2XX_STATE_BUSY_TX) {
        return -EAGAIN;
    }

    if (info) {
        uint8_t trac_status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATE)
                              & AT86RF2XX_TRX_STATE_MASK__TRAC;
#if AT86RF2XX_HAVE_RETRIES && AT86RF2XX_HAVE_RETRIES_REG
        info->retrans = (at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_2)
                         & AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_MASK)
                        >> AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_OFFSET;
#endif
        switch (trac_status) {
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

static int _confirm_set_trx_state(at86rf2xx_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: confirm_set_trx_state\n");

    if (at86rf2xx_get_status(dev) == AT86RF2XX_STATE_IN_PROGRESS) {
        return -EAGAIN;
    }

    return 0;
}

static int _confirm_cca(at86rf2xx_t *dev)
{
    DEBUG("at86rf2xx_rf_ops: confirm_cca\n");
    uint8_t reg;
    reg = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS);
    if ((reg & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE) == 0) {
        return -EAGAIN;
    }

    /* Restore to default IRQs */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK,
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END | AT86RF2XX_IRQ_STATUS_MASK__RX_START);
    at86rf2xx_set_state(dev, AT86RF2XX_PHY_STATE_RX);
    return !!(reg & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS);
}

static int _confirm_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx)
{
    at86rf2xx_t *dev = hal->priv;
    int res = -ENOTSUP;

    mutex_lock(&dev->lock);
    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        res = _confirm_transmit(dev, ctx);
        break;
    case IEEE802154_HAL_OP_SET_RX:
        res = _confirm_set_trx_state(dev);
        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        res = _confirm_set_trx_state(dev);
        break;
    case IEEE802154_HAL_OP_CCA:
        res = _confirm_cca(dev);
        break;
    default:
        assert(false);
        break;
    }
    mutex_unlock(&dev->lock);
    return res;
}

static int _set_cca_threshold(ieee802154_dev_t *hal, int8_t threshold)
{
    DEBUG("at86rf2xx_rf_ops: set_cca_threshold to %i\n", threshold);
    at86rf2xx_t *dev = hal->priv;
    mutex_lock(&dev->lock);
    at86rf2xx_set_cca_threshold(dev, threshold);
    mutex_unlock(&dev->lock);
    return 0;
}

static int _set_cca_mode(ieee802154_dev_t *hal, ieee802154_cca_mode_t mode)
{
    (void)hal;
    if (mode != IEEE802154_CCA_MODE_ED_THRESHOLD) {
        DEBUG("at86rf2xx_rf_ops: CCA mode not supported\n");
        return -ENOTSUP;
    }
    DEBUG("at86rf2xx_rf_ops: set_cca_mode to ED Threshold\n");
    return 0;
}

static int _config_phy(ieee802154_dev_t *hal, const ieee802154_phy_conf_t *conf)
{
    at86rf2xx_t *dev = hal->priv;

    if (conf->pow < 0 || conf->pow > AT86RF2XX_TXPOWER_MAX) {
        return -EINVAL;
    }
    mutex_lock(&dev->lock);
    at86rf2xx_configure_phy(dev, conf->channel, conf->page, conf->pow);
    mutex_unlock(&dev->lock);
    return 0;
}

static int _set_csma_params(ieee802154_dev_t *hal, const ieee802154_csma_be_t *bd, int8_t retries)
{
    DEBUG("at86rf2xx_rf_ops: set_csma_params.");
    at86rf2xx_t *dev = hal->priv;
    retries = MIN(retries, 5);              /* valid values: 0-5 */
    retries = (retries < 0) ? 7 : retries;  /* max < 0 => disable CSMA (set to 7) */
    DEBUG("Retries: %i ", retries);
    mutex_lock(&dev->lock);
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES);
    tmp |= (retries << 1);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);

    if (bd) {
        uint8_t max = bd->max;
        uint8_t min = bd->min;
        DEBUG("max: %i ", max);
        DEBUG("min: %i ", min);
        max = MIN(8, max);
        min = MIN(min, max);
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_BE, (max << 4) | (min));
    }
    DEBUG("\n");

    mutex_unlock(&dev->lock);
    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *hal, ieee802154_af_cmd_t cmd, const void *value)
{

    at86rf2xx_t *dev = hal->priv;
    const uint16_t *pan_id = value;
    const network_uint16_t *short_addr = value;
    const eui64_t *ext_addr = value;

    DEBUG("at86rf2xx_rf_ops: set_hw_addr_filter.\n");
    mutex_lock(&dev->lock);
    switch (cmd) {
    case IEEE802154_AF_SHORT_ADDR:
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_0,
                            short_addr->u8[1]);
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_1,
                            short_addr->u8[0]);
        DEBUG("SHORT_ADDR: %04x\n", byteorder_ntohs(*short_addr));
        break;
    case IEEE802154_AF_EXT_ADDR:
        DEBUG(" EXT_ADDR: ");
        for (int i = 0; i < 8; i++) {
            at86rf2xx_reg_write(dev, (AT86RF2XX_REG__IEEE_ADDR_0 + i),
                                ext_addr->uint8[IEEE802154_LONG_ADDRESS_LEN - 1 - i]);
            DEBUG("%02x", ext_addr->uint8[i]);
        }
        DEBUG("\n");
        break;
    case IEEE802154_AF_PANID: {
        le_uint16_t le_pan = byteorder_btols(byteorder_htons(*pan_id));
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_0, le_pan.u8[0]);
        at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_1, le_pan.u8[1]);
        DEBUG("PANID: %04x\n", *pan_id);
    }
    break;
    case IEEE802154_AF_PAN_COORD:
        mutex_unlock(&dev->lock);
        return -ENOTSUP;
    }

    mutex_unlock(&dev->lock);
    return 0;
}

static int _config_src_addr_match(ieee802154_dev_t *hal, ieee802154_src_match_t cmd,
                                  const void *value)
{

    at86rf2xx_t *dev = hal->priv;
    int res;

    mutex_lock(&dev->lock);
    switch (cmd) {
    case IEEE802154_SRC_MATCH_EN: {
        const bool en = *((const bool *)value);
        at86rf2xx_set_option(dev, AT86RF2XX_OPT_ACK_PENDING, en);
        res = 0;
        break;
    }
    default:
        res = -ENOTSUP;
    }
    mutex_unlock(&dev->lock);
    return res;
}

static int _set_frame_filter_mode(ieee802154_dev_t *hal, ieee802154_filter_mode_t mode)
{

    at86rf2xx_t *dev = hal->priv;
    bool promisc = false;

    switch (mode) {
    case IEEE802154_FILTER_ACCEPT:
        promisc = false;
        break;
    case IEEE802154_FILTER_PROMISC:
        promisc = true;
        break;
    case IEEE802154_FILTER_ACK_ONLY:
    case IEEE802154_FILTER_SNIFFER:
        return -ENOTSUP;
    }

    DEBUG("[at86rf2xx] set promiscuous to %i\n", promisc);
    mutex_lock(&dev->lock);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_PROMISCUOUS, promisc);
    mutex_unlock(&dev->lock);
    return 0;
}

int at86rf2xx_init(at86rf2xx_t *dev, const at86rf2xx_params_t *params, ieee802154_dev_t *hal,
                   void (*cb)(void *), void *ctx)
{
    uint8_t tmp;

    (void)tmp;

    (void)at86rf2xx_periph;
    mutex_init(&dev->lock);
    mutex_lock(&dev->lock);
#if AT86RF2XX_IS_PERIPH
    (void)params;
    /* set all interrupts off */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, 0x00);
    at86rf2xx_periph = hal;
#else
    /* initialize device descriptor */
    dev->params = *params;
#endif

    if (!AT86RF2XX_IS_PERIPH) {
        at86rf2xx_spi_init(dev, cb, ctx);
    }

    at86rf2xx_hardware_reset(dev);

    /* test if the device is responding */
    if (at86rf2xx_reg_read(dev, AT86RF2XX_REG__PART_NUM) != AT86RF2XX_PARTNUM) {
        DEBUG("[at86rf2xx] error: unable to read correct part number\n");
        mutex_unlock(&dev->lock);
        return -ENOTSUP;
    }

    hal->driver = &at86rf2xx_ops;
    hal->priv = dev;

    if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
        at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK, true);
        at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA, true);
    }

    at86rf2xx_set_state(dev, AT86RF2XX_STATE_FORCE_TRX_OFF);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2,
                        AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);

#if !AT86RF2XX_IS_PERIPH
    /* don't populate masked interrupt flags to IRQ_STATUS register */
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_1);
    tmp &= ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_1, tmp);
#endif

    /* configure smart idle listening feature */
    at86rf2xx_enable_smart_idle(dev);

    /* disable clock output to save power */
    at86rf2xx_disable_clock_output(dev);

    /* enable interrupts */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK,
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END
                        | AT86RF2XX_IRQ_STATUS_MASK__RX_START);

    /* enable TX start interrupt for retry counter */
#if AT86RF2XX_HAVE_TX_START_IRQ
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK1,
                        AT86RF2XX_IRQ_STATUS_MASK1__TX_START);
#endif

    /* clear interrupt flags */
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

    /* Go to SLEEP mode from TRX_OFF */
#if AT86RF2XX_IS_PERIPH
    /* Setting SLPTR bit brings radio transceiver to sleep in in TRX_OFF*/
    *AT86RF2XX_REG__TRXPR |= (AT86RF2XX_TRXPR_SLPTR);
#else
    gpio_set(dev->params.sleep_pin);
#endif

    mutex_unlock(&dev->lock);
    DEBUG("[at86rf2xx] reset complete.\n");

    return 0;
}

static void _dispatch_event(ieee802154_dev_t *hal, ieee802154_trx_ev_t ev)
{
    at86rf2xx_t *dev = hal->priv;

    mutex_unlock(&dev->lock);
    hal->cb(hal, ev);
    mutex_lock(&dev->lock);
}

static inline void _isr_recv_complete(ieee802154_dev_t *hal)
{
    at86rf2xx_t *dev = hal->priv;

    if (IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {

        uint8_t phy_status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_RSSI);
        bool crc_ok = phy_status & AT86RF2XX_PHY_RSSI_MASK__RX_CRC_VALID;

        if (crc_ok) {
            _dispatch_event(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
        }
        else {
            _dispatch_event(hal, IEEE802154_RADIO_INDICATION_CRC_ERROR);
        }
    }
    else {
        _dispatch_event(hal, IEEE802154_RADIO_INDICATION_RX_DONE);
    }
}

void at86rf2xx_irq_handler(ieee802154_dev_t *hal)
{
    at86rf2xx_t *dev = hal->priv;
    uint8_t irq_mask;
    uint8_t state;

    /* Read the state before locking the mutex to capture the current state of the radio.
     * This operation should be fine, because it only performs a reading operation and doesn't change the radio state.
     * This solves the bug where the radio thread is blocked by the submac thread:
     * The radio is in RX and still hasn't received the IRQ for RX_DONE. When the radio
     * isn't busy anymore, it could happen that the submac is able to get the mutex before the
     * IRQ arrives and the radio thread is blocked because of the mutex even with higher priority. After the submac is done, the
     * radio thread is unlocked but the radio is in the TX state instead of RX. Therefore the radio
     * dispatches a CONFIRM_TX_DONE event which is actually an RX_DONE event, which causes an assertion in the submac.
     */
    state = at86rf2xx_get_status(dev);
    /* If transceiver is sleeping register access is impossible and frames are
     * lost anyway, so return immediately.
     */
    if (state == AT86RF2XX_STATE_SLEEP) {
        return;
    }
    mutex_lock(&dev->lock);

    /* read (consume) device status */
    irq_mask = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__RX_START) {
        _dispatch_event(hal, IEEE802154_RADIO_INDICATION_RX_START);
        DEBUG("[at86rf2xx] EVT - RX_START\n");
    }

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__TRX_END) {
        if ((state == AT86RF2XX_PHY_STATE_RX) || (state == AT86RF2XX_PHY_STATE_RX_BUSY)) {
            DEBUG("[at86rf2xx] EVT - RX_END\n");
            _isr_recv_complete(hal);
        }
        else if (state == AT86RF2XX_PHY_STATE_TX) {
            _dispatch_event(hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
        }
    }

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE) {
        _dispatch_event(hal, IEEE802154_RADIO_CONFIRM_CCA);
    }
    mutex_unlock(&dev->lock);
}

static void _event_handler(event_t *event)
{
    at86rf2xx_bhp_ev_t *bhp = container_of(event, at86rf2xx_bhp_ev_t, ev);
    ieee802154_dev_t *hal = bhp->hal;

    at86rf2xx_irq_handler(hal);
}

void _irq_handler(void *ctx)
{
    at86rf2xx_bhp_ev_t *bhp = ctx;

    event_post(bhp->evq, &bhp->ev);
}

int at86rf2xx_init_event(at86rf2xx_bhp_ev_t *bhp, const at86rf2xx_params_t *params,
                         ieee802154_dev_t *hal, event_queue_t *evq)
{
    bhp->hal = hal;
    bhp->evq = evq;
    bhp->ev.handler = _event_handler;

    return at86rf2xx_init(&bhp->dev, params, hal, _irq_handler, bhp);
}

static const ieee802154_radio_ops_t at86rf2xx_ops = {
    .caps =  IEEE802154_CAP_24_GHZ
            | IEEE802154_CAP_IRQ_CRC_ERROR
            | IEEE802154_CAP_IRQ_RX_START
            | IEEE802154_CAP_IRQ_TX_DONE
#if ! IS_ACTIVE(AT86RF2XX_BASIC_MODE)
            | IEEE802154_CAP_FRAME_RETRANS
#if AT86RF2XX_HAVE_RETRIES
            | IEEE802154_CAP_FRAME_RETRANS_INFO
#endif
#endif

#if AT86RF2XX_IS_PERIPH
            | IEEE802154_CAP_RX_TIMESTAMP
#endif
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
#if IS_USED(MODULE_AT86RF2XX_AES_SPI) && IS_USED(MODULE_IEEE802154_SECURITY)
    .cipher_ops = &_at86rf2xx_cipher_ops,
#endif
};

#if AT86RF2XX_IS_PERIPH

/**
 * @brief ISR for transceiver's TX_START interrupt
 *
 * In procedure TX_ARET the TRX24_TX_START interrupt is issued separately for every
 * frame transmission and frame retransmission.
 * Indicates the frame start of a transmitted acknowledge frame in procedure RX_AACK.
 *
 * Flow Diagram Manual p. 52 / 63
 */
#if AT86RF2XX_HAVE_RETRIES
ISR(TRX24_TX_START_vect){
    /* __enter_isr(); is not necessary as there is nothing which causes a
     * thread_yield and the interrupt can not be interrupted by an other ISR */
    at86rf2xx_t *dev = at86rf2xx_periph->priv;

    dev->tx_retries ++;
}
#endif

/**
 * @brief  Transceiver PLL Lock
 *
 *  Is triggered when PLL locked successfully.
 *
 * Manual p. 40
 */
static inline void txr24_pll_lock_handler(void)
{
    /* __enter_isr(); is not necessary as there is nothing which causes a
     * thread_yield and the interrupt can not be interrupted by an other ISR */
    DEBUG("TRX24_PLL_LOCK\n");
}
AVR8_ISR(TRX24_PLL_LOCK_vect, txr24_pll_lock_handler);

/**
 * @brief  Transceiver PLL Unlock
 *
 *  Is triggered when PLL unlocked unexpectedly.
 *
 * Manual p. 89
 */
static inline void txr24_pll_unlock_handler(void)
{
    /* __enter_isr(); is not necessary as there is nothing which causes a
     * thread_yield and the interrupt can not be interrupted by an other ISR */
    DEBUG("TRX24_PLL_UNLOCK\n");
}
AVR8_ISR(TRX24_PLL_UNLOCK_vect, txr24_pll_unlock_handler);

/**
 * @brief ISR for transceiver's receive start interrupt
 *
 *  Is triggered when valid PHR is detected.
 *  Save IRQ status and inform upper layer of data reception.
 *
 * Flow Diagram Manual p. 52 / 63
 */
static inline void txr24_rx_start_handler(void)
{
    uint8_t status = *AT86RF2XX_REG__TRX_STATE & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_RX_START 0x%x\n", status);
    at86rf2xx_periph->cb(at86rf2xx_periph, IEEE802154_RADIO_INDICATION_RX_START);
}
AVR8_ISR(TRX24_RX_START_vect, txr24_rx_start_handler);

/**
 * @brief ISR for transceiver's receive end interrupt
 *
 *  Is triggered when valid data is received. FCS check passed.
 *  Save IRQ status and inform upper layer of data reception.
 *
 * Flow Diagram Manual p. 52 / 63
 */
static inline void txr24_rx_end_handler(void)
{
    uint8_t status = *AT86RF2XX_REG__TRX_STATE & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_RX_END 0x%x\n", status);
    at86rf2xx_periph->cb(at86rf2xx_periph, IEEE802154_RADIO_INDICATION_RX_DONE);
}
AVR8_ISR(TRX24_RX_END_vect, txr24_rx_end_handler);

/**
 * @brief  Transceiver CCAED Measurement finished
 *
 *  Is triggered when CCA or ED measurement completed.
 *
 * Manual p. 74 and p. 76
 */
static inline void txr24_cca_ed_done_handler(void)
{
    DEBUG("TRX24_CCA_ED_DONE\n");
    at86rf2xx_periph->cb(at86rf2xx_periph, IEEE802154_RADIO_CONFIRM_CCA);
}
AVR8_ISR(TRX24_CCA_ED_DONE_vect, txr24_cca_ed_done_handler);

/**
 * @brief  Transceiver Frame Address Match, indicates incoming frame
 *
 *  Is triggered when Frame with valid Address is received.
 *  Can be used to wake up MCU from sleep, etc.
 *
 * Flow Diagram Manual p. 52 / 63
 */
static inline void txr24_xah_ami_handler(void)
{
    /* __enter_isr(); is not necessary as there is nothing which causes a
     * thread_yield and the interrupt can not be interrupted by an other ISR */
    DEBUG("TRX24_XAH_AMI\n");
}
AVR8_ISR(TRX24_XAH_AMI_vect, txr24_xah_ami_handler)

/**
 * @brief ISR for transceiver's transmit end interrupt
 *
 *  Is triggered when data or when acknowledge frames where send.
 *
 * Flow Diagram Manual p. 52 / 63
 */
static inline void txr24_tx_end_handler(void)
{

    uint8_t status = *AT86RF2XX_REG__TRX_STATE & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_TX_END 0x%x\n", status);

    /* only inform upper layer when a transmission was done,
     * not for sending acknowledge frames if data was received. */
    if (status != AT86RF2XX_PHY_STATE_RX) {
        at86rf2xx_periph->cb(at86rf2xx_periph, IEEE802154_RADIO_CONFIRM_TX_DONE);
    }


}
AVR8_ISR(TRX24_TX_END_vect, txr24_tx_end_handler);
/**
 * @brief ISR for transceiver's wakeup finished interrupt
 *
 *  Is triggered when transceiver module reset is finished.
 *  Save IRQ status and inform upper layer the transceiver is operational.
 *
 * Manual p. 40
 */
static inline void txr24_awake_handler(void)
{
    /* __enter_isr(); is not necessary as there is nothing which causes a
     * thread_yield and the interrupt can not be interrupted by an other ISR */
    DEBUG("TRX24_AWAKE\n");
}
AVR8_ISR(TRX24_AWAKE_vect, txr24_awake_handler);

#endif /* AT86RF2XX_IS_PERIPH */
