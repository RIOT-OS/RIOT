/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
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
 *
 * @}
 */

#include "periph/cpuid.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


void at86rf2xx_setup(at86rf2xx_t *dev, const at86rf2xx_params_t *params)
{
    netdev2_t *netdev = (netdev2_t *)dev;

    netdev->driver = &at86rf2xx_driver;
    /* initialize device descriptor */
    memcpy(&dev->params, params, sizeof(at86rf2xx_params_t));
    dev->idle_state = AT86RF2XX_STATE_TRX_OFF;
    dev->state = AT86RF2XX_STATE_SLEEP;
    dev->pending_tx = 0;
    /* initialise SPI */
    spi_init_master(dev->params.spi, SPI_CONF_FIRST_RISING, params->spi_speed);
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
#if CPUID_LEN
/* make sure that the buffer is always big enough to store a 64bit value */
#   if CPUID_LEN < IEEE802154_LONG_ADDRESS_LEN
    uint8_t cpuid[IEEE802154_LONG_ADDRESS_LEN];
#   else
    uint8_t cpuid[CPUID_LEN];
#endif
    eui64_t addr_long;
#endif

    at86rf2xx_hardware_reset(dev);

    /* Reset state machine to ensure a known state */
    at86rf2xx_reset_state_machine(dev);

    /* reset options and sequence number */
    dev->netdev.seq = 0;
    dev->netdev.flags = 0;
    /* set short and long address */
#if CPUID_LEN
    /* in case CPUID_LEN < 8, fill missing bytes with zeros */
    memset(cpuid, 0, CPUID_LEN);

    cpuid_get(cpuid);

#if CPUID_LEN > IEEE802154_LONG_ADDRESS_LEN
    for (int i = IEEE802154_LONG_ADDRESS_LEN; i < CPUID_LEN; i++) {
        cpuid[i & 0x07] ^= cpuid[i];
    }
#endif

    /* make sure we mark the address as non-multicast and not globally unique */
    cpuid[0] &= ~(0x01);
    cpuid[0] |= 0x02;
    /* copy and set long address */
    memcpy(&addr_long, cpuid, IEEE802154_LONG_ADDRESS_LEN);
    at86rf2xx_set_addr_long(dev, NTOHLL(addr_long.uint64.u64));
    at86rf2xx_set_addr_short(dev, NTOHS(addr_long.uint16[0].u16));
#else
    at86rf2xx_set_addr_long(dev, AT86RF2XX_DEFAULT_ADDR_LONG);
    at86rf2xx_set_addr_short(dev, AT86RF2XX_DEFAULT_ADDR_SHORT);
#endif
    /* set default PAN id */
    at86rf2xx_set_pan(dev, AT86RF2XX_DEFAULT_PANID);
    /* set default channel */
    at86rf2xx_set_chan(dev, AT86RF2XX_DEFAULT_CHANNEL);
    /* set default TX power */
    at86rf2xx_set_txpower(dev, AT86RF2XX_DEFAULT_TXPOWER);
    /* set default options */
    at86rf2xx_set_option(dev, NETDEV2_IEEE802154_PAN_COMP, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_START, false);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_END, true);
#ifdef MODULE_NETSTATS_L2
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_TX_END, true);
#endif
    /* set default protocol */
#ifdef MODULE_GNRC_SIXLOWPAN
    dev->netdev.proto = GNRC_NETTYPE_SIXLOWPAN;
#elif MODULE_GNRC
    dev->netdev.proto = GNRC_NETTYPE_UNDEF;
#endif
    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2,
                        AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);
#ifdef MODULE_AT86RF212B
    at86rf2xx_set_page(dev, 0);
#endif

    /* don't populate masked interrupt flags to IRQ_STATUS register */
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_1);
    tmp &= ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_1, tmp);

    /* disable clock output to save power */
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_0);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL);
    tmp |= (AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_0, tmp);

    /* enable interrupts */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK,
                        AT86RF2XX_IRQ_STATUS_MASK__TRX_END);
    /* clear interrupt flags */
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

    /* go into RX state */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_AACK_ON);

    DEBUG("at86rf2xx_reset(): reset complete.\n");
}

bool at86rf2xx_cca(at86rf2xx_t *dev)
{
    uint8_t tmp;
    uint8_t status;

    at86rf2xx_assert_awake(dev);

    /* trigger CCA measurment */
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    tmp &= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, tmp);
    /* wait for result to be ready */
    do {
        status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS);
    } while (!(status & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE));
    /* return according to measurement */
    if (status & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS) {
        return true;
    }
    else {
        return false;
    }
}

size_t at86rf2xx_send(at86rf2xx_t *dev, uint8_t *data, size_t len)
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
    /* make sure ongoing transmissions are finished */
    do {
        state = at86rf2xx_get_status(dev);
    } while (state == AT86RF2XX_STATE_BUSY_RX_AACK ||
             state == AT86RF2XX_STATE_BUSY_TX_ARET);
    if (state != AT86RF2XX_STATE_TX_ARET_ON) {
        dev->idle_state = state;
    }
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_TX_ARET_ON);
    dev->tx_frame_len = IEEE802154_FCS_LEN;
}

size_t at86rf2xx_tx_load(at86rf2xx_t *dev, uint8_t *data,
                         size_t len, size_t offset)
{
    dev->tx_frame_len += (uint8_t)len;
    at86rf2xx_sram_write(dev, offset + 1, data, len);
    return offset + len;
}

void at86rf2xx_tx_exec(at86rf2xx_t *dev)
{
    netdev2_t *netdev = (netdev2_t *)dev;

    /* write frame length field in FIFO */
    at86rf2xx_sram_write(dev, 0, &(dev->tx_frame_len), 1);
    /* trigger sending of pre-loaded frame */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__TX_START);
    if (netdev->event_callback &&
        (dev->netdev.flags & AT86RF2XX_OPT_TELL_TX_START)) {
        netdev->event_callback(netdev, NETDEV2_EVENT_TX_STARTED, NULL);
    }
}

size_t at86rf2xx_rx_len(at86rf2xx_t *dev)
{
    uint8_t phr;

    at86rf2xx_fb_read(dev, &phr, 1);

    /* ignore MSB (refer p.80) and substract length of FCS field */
    return (size_t)((phr & 0x7f) - 2);
}

void at86rf2xx_rx_read(at86rf2xx_t *dev, uint8_t *data, size_t len,
                       size_t offset)
{
    /* when reading from SRAM, the different chips from the AT86RF2xx family
     * behave differently: the AT86F233, the AT86RF232 and the ATRF86212B return
     * frame length field (PHR) at position 0 and the first data byte at
     * position 1.
     * The AT86RF231 does not return the PHR field and return
     * the first data byte at position 0.
     */
#ifndef MODULE_AT86RF231
    at86rf2xx_sram_read(dev, offset + 1, data, len);
#else
    at86rf2xx_sram_read(dev, offset, data, len);
#endif
}
