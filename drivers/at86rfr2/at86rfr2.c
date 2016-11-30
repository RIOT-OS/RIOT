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

#include "include/at86rfr2_internal.h"
#include "include/at86rfr2_netdev.h"
#include "include/at86rfr2_registers.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

// saved device Pointer for Interrupt callback
static at86rf2xx_t* static_dev;

//static void _irq_handler(void *arg)
//{
//    netdev2_t *dev = (netdev2_t *) arg;
//
//    if (dev->event_callback) {
//        dev->event_callback(dev, NETDEV2_EVENT_ISR);
//    }
//}

/**
 * \brief ISR for transceiver's rx start interrupt
 *
 * By the time the SFD is detected, the hardware timestamps the
 * current frame in the SCTSR register.
 */
ISR(TRX24_RX_START_vect, ISR_BLOCK)
{
	__enter_isr();
	((netdev2_t*)static_dev)->event_callback((netdev2_t *)static_dev, NETDEV2_EVENT_RX_STARTED);
    __exit_isr();
}

/**
 * \brief ISR for transceiver's receive end interrupt
 */
ISR(TRX24_RX_END_vect, ISR_BLOCK)
{
	__enter_isr();
	((netdev2_t*)static_dev)->event_callback((netdev2_t *)static_dev, NETDEV2_EVENT_RX_COMPLETE);
    __exit_isr();
}

/**
 * \brief ISR for transceiver's TX_START interrupt
 */
ISR(TRX24_TX_START_vect)
{
	__enter_isr();
	((netdev2_t*)static_dev)->event_callback((netdev2_t *)static_dev, NETDEV2_EVENT_TX_STARTED);
    __exit_isr();
}

/**
 * \brief ISR for transceiver's transmit end interrupt
 */
ISR(TRX24_TX_END_vect, ISR_BLOCK)
{
	__enter_isr();
	((netdev2_t*)static_dev)->event_callback((netdev2_t *)static_dev, NETDEV2_EVENT_TX_COMPLETE);
    __exit_isr();
}

void at86rf2xx_setup(at86rf2xx_t *dev, const at86rf2xx_params_t *params)
{
	// in the rfr2 SOC device, no SPI access needed


	netdev2_t *netdev = (netdev2_t *)dev;
	netdev->driver = &at86rf2xx_driver;
	/* initialize device descriptor */
	memcpy(&dev->params, params, sizeof(at86rf2xx_params_t));
	dev->idle_state = AT86RF2XX_STATE_TRX_OFF;
	dev->state = AT86RF2XX_STATE_SLEEP;
	dev->pending_tx = 0;

	// save device pointer for Interrupt callback
	static_dev = dev;

	//TODO initialize interrupt
	// clear corresponding status bits
	// by writing a 1 to the Status register
	AT86RF2XX_REG__IRQ_STATUS =
			 AT86RF2XX_IRQ_STATUS_MASK__TRX_TX_END |
			 AT86RF2XX_IRQ_STATUS_MASK__TRX_END |
			 AT86RF2XX_IRQ_STATUS_MASK__RX_START;
	AT86RF2XX_REG__IRQ_STATUS1 =
			AT86RF2XX_IRQ_STATUS_MASK1__TX_START_EN;

	// set corresponding interrupt active
	AT86RF2XX_REG__IRQ_MASK =
			AT86RF2XX_IRQ_STATUS_MASK__TRX_TX_END |
			AT86RF2XX_IRQ_STATUS_MASK__TRX_END|
			AT86RF2XX_IRQ_STATUS_MASK__RX_START;
	AT86RF2XX_REG__IRQ_MASK1 =
			AT86RF2XX_IRQ_STATUS_MASK1__TX_START_EN;

	DEBUG("at86rf2xx_setup(): setup complete.\n");
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

#ifndef MODULE_AT86RFR2 // TODO replace for functionality RFR2
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
#endif

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
        netdev->event_callback(netdev, NETDEV2_EVENT_TX_STARTED);
    }
}
