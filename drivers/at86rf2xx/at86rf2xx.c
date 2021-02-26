/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
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
 * @brief       Implementation of public functions for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include "kernel_defines.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#if IS_USED(IEEE802154_SECURITY)
#include "net/ieee802154_security.h"
#endif
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_netdev.h"
#if IS_USED(MODULE_AT86RF2XX_AES_SPI)
#include "at86rf2xx_aes.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

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
    at86rf2xx_aes_key_write_encrypt((at86rf2xx_t *)dev->ctx, key);
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
    at86rf2xx_aes_cbc_encrypt((at86rf2xx_t *)dev->ctx,
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
    at86rf2xx_aes_ecb_encrypt((at86rf2xx_t *)dev->ctx,
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

void at86rf2xx_setup(at86rf2xx_t *dev, const at86rf2xx_params_t *params, uint8_t index)
{
    netdev_t *netdev = &dev->netdev.netdev;

    netdev->driver = &at86rf2xx_driver;
    /* State to return after receiving or transmitting */
    dev->idle_state = AT86RF2XX_STATE_TRX_OFF;
    /* radio state is P_ON when first powered-on */
    dev->state = AT86RF2XX_STATE_P_ON;
    dev->pending_tx = 0;

#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
    (void) params;
    /* set all interrupts off */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, 0x00);
#else
    /* initialize device descriptor */
    dev->params = *params;
#endif

    netdev_register(netdev, NETDEV_AT86RF2XX, index);
    /* set device address */
    netdev_ieee802154_setup(&dev->netdev);
}

static void at86rf2xx_disable_clock_output(at86rf2xx_t *dev)
{
#if defined(MODULE_AT86RFA1) || defined(MODULE_AT86RFR2)
    (void) dev;
#else
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_0);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL);
    tmp |= (AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_0, tmp);
#endif
}

static void at86rf2xx_enable_smart_idle(at86rf2xx_t *dev)
{
#if AT86RF2XX_SMART_IDLE_LISTENING
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_RPC);
    tmp |= (AT86RF2XX_TRX_RPC_MASK__RX_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__PDT_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__PLL_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__XAH_TX_RPC_EN |
            AT86RF2XX_TRX_RPC_MASK__IPAN_RPC_EN);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_RPC, tmp);
    at86rf2xx_set_rxsensitivity(dev, RSSI_BASE_VAL);
#else
    (void) dev;
#endif
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
    netdev_ieee802154_reset(&dev->netdev);

    /* Reset state machine to ensure a known state */
    if (dev->state == AT86RF2XX_STATE_P_ON) {
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_FORCE_TRX_OFF);
    }

    /* set short and long address */
    at86rf2xx_set_addr_long(dev, (eui64_t *)dev->netdev.long_addr);
    at86rf2xx_set_addr_short(dev, (network_uint16_t *)dev->netdev.short_addr);

    /* set default channel */
    at86rf2xx_set_chan(dev, AT86RF2XX_DEFAULT_CHANNEL);
    /* set default TX power */
    at86rf2xx_set_txpower(dev, AT86RF2XX_DEFAULT_TXPOWER);
    /* set default options */

    if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
        at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK, true);
        at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA, true);

        static const netopt_enable_t enable = NETOPT_ENABLE;
        netdev_ieee802154_set(&dev->netdev, NETOPT_ACK_REQ,
                              &enable, sizeof(enable));
    }

    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2,
                        AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);
#ifdef MODULE_AT86RF212B
    at86rf2xx_set_page(dev, AT86RF2XX_DEFAULT_PAGE);
#endif

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

    /* enable interrupts */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK,
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END);

    /* enable TX start interrupt for retry counter */
#ifdef AT86RF2XX_REG__IRQ_MASK1
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK1,
                             AT86RF2XX_IRQ_STATUS_MASK1__TX_START);
#endif

    /* clear interrupt flags */
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

#if IS_USED(MODULE_IEEE802154_SECURITY) && \
    IS_USED(MODULE_AT86RF2XX_AES_SPI)
    dev->netdev.sec_ctx.dev.cipher_ops = &_at86rf2xx_cipher_ops;
    dev->netdev.sec_ctx.dev.ctx = dev;
    /* All configurations of the security module, the SRAM content,
       and keys are reset during DEEP_SLEEP or RESET state. */
    at86rf2xx_aes_key_write_encrypt(dev,
        dev->netdev.sec_ctx.cipher.context.context);
#endif

    /* State to return after receiving or transmitting */
    dev->idle_state = AT86RF2XX_PHY_STATE_RX;
    /* go into RX state */
    at86rf2xx_set_state(dev, AT86RF2XX_PHY_STATE_RX);

    DEBUG("at86rf2xx_reset(): reset complete.\n");
}

size_t at86rf2xx_send(at86rf2xx_t *dev, const uint8_t *data, size_t len)
{
    /* check data length */
    if (len > AT86RF2XX_MAX_PKT_LENGTH) {
        DEBUG("[at86rf2xx] Error: data to send exceeds max packet size\n");
        return 0;
    }
    at86rf2xx_tx_prepare(dev);
    at86rf2xx_tx_load(dev, data, len, 0);
    at86rf2xx_tx_exec(dev);
    return len;
}

void at86rf2xx_tx_prepare(at86rf2xx_t *dev)
{
    uint8_t state;

    dev->pending_tx++;
    state = at86rf2xx_set_state(dev, AT86RF2XX_PHY_STATE_TX);
    if (state != AT86RF2XX_PHY_STATE_TX) {
        dev->idle_state = state;
    }
    dev->tx_frame_len = IEEE802154_FCS_LEN;
}

size_t at86rf2xx_tx_load(at86rf2xx_t *dev, const uint8_t *data,
                         size_t len, size_t offset)
{
    dev->tx_frame_len += (uint8_t)len;
    at86rf2xx_sram_write(dev, offset + 1, data, len);
    return offset + len;
}

void at86rf2xx_tx_exec(at86rf2xx_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;

#if AT86RF2XX_HAVE_RETRIES
    dev->tx_retries = -1;
#endif

    /* write frame length field in FIFO */
    at86rf2xx_sram_write(dev, 0, &(dev->tx_frame_len), 1);
    /* trigger sending of pre-loaded frame */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__TX_START);
    if (netdev->event_callback &&
        (dev->flags & AT86RF2XX_OPT_TELL_TX_START)) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    }
}

bool at86rf2xx_cca(at86rf2xx_t *dev)
{
    uint8_t reg;
    uint8_t old_state = at86rf2xx_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    /* Disable RX path */
    uint8_t rx_syn = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN);

    reg = rx_syn | AT86RF2XX_RX_SYN__RX_PDT_DIS;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RX_SYN, reg);
    /* Manually triggered CCA is only possible in RX_ON (basic operating mode) */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_ON);
    /* Perform CCA */
    reg = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    reg |= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, reg);
    /* Spin until done (8 symbols + 12 µs = 128 µs + 12 µs for O-QPSK)*/
    do {
        reg = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS);
    } while ((reg & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE) == 0);
    /* return true if channel is clear */
    bool ret = !!(reg & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS);
    /* re-enable RX */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RX_SYN, rx_syn);
    /* Step back to the old state */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    at86rf2xx_set_state(dev, old_state);
    return ret;
}
