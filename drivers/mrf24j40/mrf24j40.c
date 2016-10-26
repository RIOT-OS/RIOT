/*
 * Copyright (C) 2016 Hochschule für Angewandte Wissenschaften Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Implementation of public functions for MRF24J40 drivers
 *
 * @author      <neo@nenaco.de>
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 * @}
 */

#include "periph/cpuid.h"
#include "byteorder.h"
#include "net/gnrc.h"
#include "mrf24j40_registers.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_netdev.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

uint8_t state;

void mrf24j40_setup(mrf24j40_t *dev, const mrf24j40_params_t *params)
{
    netdev2_t *netdev = (netdev2_t *)dev;

    netdev->driver = &mrf24j40_driver;
    /* initialize device descriptor */
    memcpy(&dev->params, params, sizeof(mrf24j40_params_t));
    dev->idle_state = MRF24J40_PSEUDO_STATE_RX_AACK_ON;
    dev->state = MRF24J40_PSEUDO_STATE_SLEEP;
    dev->pending_tx = 0;
    /* initialise SPI */
    spi_init_master(dev->params.spi, SPI_CONF_FIRST_RISING, params->spi_speed);

}

void mrf24j40_reset(mrf24j40_t *dev)
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

    mrf24j40_init(dev);

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
    mrf24j40_set_addr_long(dev, NTOHLL(addr_long.uint64.u64));
    mrf24j40_set_addr_short(dev, NTOHS(addr_long.uint16[0].u16));
#else
    mrf24j40_set_addr_long(dev, MRF24J40_DEFAULT_ADDR_LONG);
    mrf24j40_set_addr_short(dev, MRF24J40_DEFAULT_ADDR_SHORT);
#endif
    /* set default PAN id */
    mrf24j40_set_pan(dev, MRF24J40_DEFAULT_PANID);

    /* configure Immediate Sleep and Wake-Up mode */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_WAKECON, MRF24J40_WAKECON_MASK__IMMWAKE);            /* enable Immediate Wake-up mode */

    /* set default options */
    mrf24j40_set_option(dev, NETDEV2_IEEE802154_PAN_COMP, true);
    mrf24j40_set_option(dev, NETDEV2_IEEE802154_SRC_MODE_LONG, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_AUTOACK, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_CSMA, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_TELL_RX_START, false);
    mrf24j40_set_option(dev, MRF24J40_OPT_TELL_RX_END, true);
#ifdef MODULE_NETSTATS_L2
    mrf24j40_set_option(dev, MRF24J40_OPT_TELL_TX_END, true);
#endif

    /* set default protocol */
#ifdef MODULE_GNRC_SIXLOWPAN
    dev->netdev.proto = GNRC_NETTYPE_SIXLOWPAN;
#elif MODULE_GNRC
    dev->netdev.proto = GNRC_NETTYPE_UNDEF;
#endif

    /* go into RX state */
    mrf24j40_set_state(dev, MRF24J40_PSEUDO_STATE_RX_AACK_ON);
    DEBUG("mrf24j40_reset(): reset complete.\n");
}

bool mrf24j40_cca(mrf24j40_t *dev)
{
    uint8_t tmp_ccaedth;
    uint8_t status;
    uint8_t tmp_rssi;

    mrf24j40_assert_awake(dev);

    /* trigger CCA measurment */
    /* take a look onto datasheet chapter 3.6.1 */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG6, MRF24J40_BBREG6_MASK__RSSIMODE1);
    /* wait for result to be ready */
    do {
        status = mrf24j40_reg_read_short(dev, MRF24J40_REG_BBREG6);
    } while (!(status & MRF24J40_BBREG2_MASK__RSSIRDY));
    /* return according to measurement */
    tmp_ccaedth = mrf24j40_reg_read_short(dev, MRF24J40_REG_CCAEDTH);       /* Energy detection threshold */
    tmp_rssi = mrf24j40_reg_read_long(dev, MRF24J40_REG_RSSI);
    if (tmp_rssi < tmp_ccaedth) {
        /* channel is clear */
        return true;            /* idle */
    }
    else {
        /* channel is busy */
        return false;
    }
}

size_t mrf24j40_send(mrf24j40_t *dev, uint8_t *data, size_t len)
{
    /* check data length */
    if (len > MRF24J40_MAX_PKT_LENGTH) {
        DEBUG("[mrf24j40] Error: data to send exceeds max packet size\n");
        return 0;
    }
    mrf24j40_tx_prepare(dev);
    mrf24j40_tx_load(dev, data, len, 0);
    mrf24j40_tx_exec(dev);
    return len;
}

void mrf24j40_tx_prepare(mrf24j40_t *dev)
{
    uint8_t state;

    dev->pending_tx++;

    /* make sure ongoing transmissions are finished */
    do {
        state = mrf24j40_get_status(dev);
    } while (state == MRF24J40_PSEUDO_STATE_BUSY_RX_AACK ||
             state == MRF24J40_PSEUDO_STATE_BUSY_TX_ARET);

    if (state != MRF24J40_PSEUDO_STATE_TX_ARET_ON) {
        dev->idle_state = state;
    }
    mrf24j40_set_state(dev, MRF24J40_PSEUDO_STATE_TX_ARET_ON);
    dev->tx_frame_len = IEEE802154_FCS_LEN;
}

size_t mrf24j40_tx_load(mrf24j40_t *dev, uint8_t *data, size_t len, size_t offset)
{

    dev->tx_frame_len += (uint8_t)len;

    mrf24j40_tx_normal_fifo_write(dev, MRF24J40_TX_NORMAL_FIFO + offset + 2, data, len);
    return offset + len;
}

void mrf24j40_tx_exec(mrf24j40_t *dev)
{
    netdev2_t *netdev = (netdev2_t *)dev;

    dev->tx_frame_len = dev->tx_frame_len - IEEE802154_FCS_LEN;
    /* write frame length field in FIFO */
    mrf24j40_tx_normal_fifo_write(dev, MRF24J40_TX_NORMAL_FIFO + 1, &(dev->tx_frame_len), 1);

    /* write header length to FIFO address 0x00 */
    /* from figure 3-11 datasheet: header length =      2 Bytes Frame Control
     *                                              +	1 Byte Seq. No.
     *                                              +	4 to 20 Bytes Addressing Fields
     */
    mrf24j40_reg_write_long(dev, MRF24J40_TX_NORMAL_FIFO, dev->header_len);

    /* trigger sending of pre-loaded frame */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXNCON, 0x5);  //transmit packet with ACK requested
    if (netdev->event_callback && (dev->netdev.flags & MRF24J40_OPT_TELL_TX_START)) {
        netdev->event_callback(netdev, NETDEV2_EVENT_TX_STARTED);
    }
}




