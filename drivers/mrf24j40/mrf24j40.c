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
    dev->idle_state = MRF24J40_PSEUDO_STATE_SLEEP;
    dev->state = MRF24J40_PSEUDO_STATE_RX_AACK_ON;
    dev->pending_tx = 0;
    /* initialise SPI */
    spi_init_master(dev->params.spi, SPI_CONF_FIRST_RISING, params->spi_speed);
}

void mrf24j40_reset(mrf24j40_t *dev)
{
	uint8_t tmp_rxmcr;
	uint8_t tmp_txmcr;

#if CPUID_LEN
/* make sure that the buffer is always big enough to store a 64bit value */
#   if CPUID_LEN < IEEE802154_LONG_ADDRESS_LEN
    uint8_t cpuid[IEEE802154_LONG_ADDRESS_LEN];
#   else
    uint8_t cpuid[CPUID_LEN];
#endif
    eui64_t addr_long;
#endif


    mrf24j40_hardware_reset(dev);

    /* Reset state machine to ensure a known state */
    mrf24j40_software_reset(dev);

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

    /* Here starts init-process as described on MRF24J40 Manual Chap. 3.2 */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_PACON2, 0x98);	/* Initialize FIFOEN = 1 and TXONTS = 0x6 */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXSTBL, 0x95);	/* Initialize RFSTBL = 0x9. */
    /* set default channel */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON0, 0x03);	/* Initialize RFOPT = 0x03. */

    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON1, 0x01);	/* VCO optimization */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON2, 0x80);	/* Bit 7 = PLL Enable */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON6, 0x90);	/* Filter control / clk recovery ctrl. / Battery monitor */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON7, 0x80);	/* Sleep clock selection -> int. 100kHz clk */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON8, 0x10);	/* VCO control */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_SLPCON1, 0x21);	/* CLKOUT pin enable  / sleep clk divider */

    /* Configuration for nonbeacon-enabled device */
    tmp_rxmcr = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);	/* read modify write */
    tmp_rxmcr &= 0b11110111;
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RXMCR, tmp_rxmcr);
    tmp_txmcr = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);	/* read modify write */
    tmp_txmcr &= 0b11011111;
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, tmp_txmcr);
	/* End nonbeacon-enabled config. */

    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG2, 0x80);		/* CCA Mode 1: energy above threshold / clear CCA bits */
//    mrf24j40_reg_write_short(dev, MRF24J40_REG_CCAEDTH, 0x60);	/* Energy detection threshold */
    mrf24j40_set_cca_threshold(dev, -69);							/* Energy detection threshold -69dBm */
//    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG6, 0x40);		/* Append RSSI to RXFIFO */

    /* configure Immediate Sleep and Wake-Up mode */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RXFLUSH, 0x1);		/* Reset RXFIFO pointer , cleared by hardware*/
    mrf24j40_reg_write_short(dev, MRF24J40_REG_WAKECON, 0x80);		/* enable Immediate Wake-up mode */


//    mrf24j40_configure_phy(dev);
    /* set default channel */
    mrf24j40_set_chan(dev, MRF24J40_DEFAULT_CHANNEL);				/* Set Channel - bits[7:4] + RF Optimization */
																	/* 0000 = chan 11, 0001 = chan 12 .....1111 = chan 26 */


    /* set default TX power */
	mrf24j40_set_txpower(dev, MRF24J40_DEFAULT_TXPOWER);

    /* reset RF state machine */
	mrf24j40_reset_state_machine(dev);

    /* set default options */
    mrf24j40_set_option(dev, NETDEV2_IEEE802154_PAN_COMP, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_AUTOACK, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_CSMA, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_TELL_RX_START, false);
    mrf24j40_set_option(dev, MRF24J40_OPT_TELL_RX_END, true);
    mrf24j40_set_option(dev, MRF24J40_OPT_PROMISCUOUS, false);
#ifdef MODULE_NETSTATS_L2
    mrf24j40_set_option(dev, MRF24J40_OPT_TELL_TX_END, true);
#endif

    /* set default protocol */
#ifdef MODULE_GNRC_SIXLOWPAN
    dev->netdev.proto = GNRC_NETTYPE_SIXLOWPAN;
#elif MODULE_GNRC
    dev->netdev.proto = GNRC_NETTYPE_UNDEF;
#endif

    /* set interrupt pin polarity / active high */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_SLPCON0, 0x3);
    /* clear interrupt flags (cleared by read) */
    mrf24j40_reg_read_short(dev, MRF24J40_REG_INTSTAT));		
    /* mrf24j40_set_interrupts (transmit and receive-irq) */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_INTCON, 0b11110110);

    /* go into RX state */
    mrf24j40_set_state(dev, MRF24J40_PSEUDO_STATE_RX_AACK_ON);

    DEBUG("mrf24j40_reset(): reset complete.\n");
}


bool mrf24j40_cca(mrf24j40_t *dev)
{
    uint8_t tmp;
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
  	tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_CCAEDTH);		/* Energy detection threshold */
  	printf("\nmrf24j40_cca : Threshold-Value = %x\n", (int)tmp);
  	tmp_rssi = mrf24j40_reg_read_long(dev, MRF24J40_REG_RSSI);
  	printf("mrf24j40_cca : Measured Value = %x\n", (int)tmp_rssi);
    if (tmp_rssi < tmp)
    {
    	return true;			
    }
    else
    {
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
    } while (state == MRF24J40_PSEUDO_STATE_BUSY_TX_ARET);

    if (state != MRF24J40_PSEUDO_STATE_TX_ARET_ON) {
        dev->idle_state = state;
    }
    mrf24j40_set_state(dev, MRF24J40_PSEUDO_STATE_TX_ARET_ON);
    dev->tx_frame_len = IEEE802154_FCS_LEN;
}



size_t mrf24j40_tx_load(mrf24j40_t *dev, uint8_t *data, size_t len, size_t offset)
{
	uint8_t i;

    dev->tx_frame_len += (uint8_t)len;

    mrf24j40_tx_normal_fifo_write(dev, offset + 1, data, len);

    return offset + len;
}

void mrf24j40_tx_exec(mrf24j40_t *dev)
{
	uint8_t tmp;

    netdev2_t *netdev = (netdev2_t *)dev;

    /* write frame length field in FIFO */
    mrf24j40_tx_normal_fifo_write(dev, 0, &(dev->tx_frame_len), 1);

    /* trigger sending of pre-loaded frame / read-modify-write */
    tmp = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXNCON);
    tmp |= MRF24J40_TXNCON_MASK__TXNTRIG;

    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXNCON, tmp);

    if (netdev->event_callback && (dev->netdev.flags & MRF24J40_OPT_TELL_TX_START))
    {
        netdev->event_callback(netdev, NETDEV2_EVENT_TX_STARTED);
    }
}




