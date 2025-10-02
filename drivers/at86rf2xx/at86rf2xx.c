/*
 * SPDX-FileCopyrightText: 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#if IS_USED(MODULE_AT86RF2XX_AES_SPI)
#include "at86rf2xx_aes.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

static void at86rf2xx_disable_clock_output(at86rf2xx_t *dev)
{
#if AT86RF2XX_IS_PERIPH
    (void) dev;
#else
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_0);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL);
    tmp |= (AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_0, tmp);
#endif
}

void at86rf2xx_enable_smart_idle(at86rf2xx_t *dev)
{
#if AT86RF2XX_SMART_IDLE_LISTENING
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_RPC);
    tmp |= AT86RF2XX_TRX_RPC_MASK__RX_RPC__SMART_IDLE;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_RPC, tmp);
    at86rf2xx_set_rxsensitivity(dev, RSSI_BASE_VAL);
#else
    (void) dev;
#endif
}

void at86rf2xx_disable_smart_idle(at86rf2xx_t *dev)
{
#if AT86RF2XX_SMART_IDLE_LISTENING
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_RPC);
    tmp &= ~AT86RF2XX_TRX_RPC_MASK__RX_RPC__SMART_IDLE;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_RPC, tmp);
#else
    (void) dev;
#endif
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
    uint8_t tmp;

    /* Reset state machine to ensure a known state */
    if (dev->state == AT86RF2XX_STATE_P_ON) {
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_FORCE_TRX_OFF);
    }

    /* set default channel, page and TX power */
    at86rf2xx_configure_phy(dev, AT86RF2XX_DEFAULT_CHANNEL, AT86RF2XX_DEFAULT_PAGE, AT86RF2XX_DEFAULT_TXPOWER);

    /* Record the default channel page in the device descriptor */
#if AT86RF2XX_HAVE_SUBGHZ
    dev->page = AT86RF2XX_DEFAULT_PAGE;
#endif

    /* set default options */

    if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
        at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK, true);
        at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA, true);
    }

    /* enable safe mode (protect RX FIFO until reading data starts) */
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
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END);

    /* enable TX start interrupt for retry counter */
#if AT86RF2XX_HAVE_TX_START_IRQ
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK1,
                             AT86RF2XX_IRQ_STATUS_MASK1__TX_START);
#endif

    /* clear interrupt flags */
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

    /* State to return after receiving or transmitting */
    dev->idle_state = AT86RF2XX_PHY_STATE_RX;
    /* go into RX state */
    at86rf2xx_set_state(dev, AT86RF2XX_PHY_STATE_RX);

    /* Enable RX start IRQ */
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_MASK);
    tmp |= AT86RF2XX_IRQ_STATUS_MASK__RX_START;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, tmp);

    DEBUG("at86rf2xx_reset(): reset complete.\n");
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
#if AT86RF2XX_HAVE_RETRIES
    dev->tx_retries = -1;
#endif

    /* write frame length field in FIFO */
    at86rf2xx_sram_write(dev, 0, &(dev->tx_frame_len), 1);
    /* trigger sending of pre-loaded frame */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__TX_START);
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
