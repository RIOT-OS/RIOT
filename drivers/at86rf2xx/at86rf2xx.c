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


#include "luid.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_netdev.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#ifdef MODULE_AT86RFR2

/*TODO port all functions from at86rf2xx_netdev.c _isr() */

/* All Interrupts are liste here to test them.*/

/*TODO Remove not needed interupt vector routines */

	#include "avr/interrupt.h"
	// saved device Pointer for Interrupt callback
	static netdev_t* static_dev;

	/**
	 * \brief ISR for transceiver's transmit end interrupt
	 */
	ISR(TRX24_PLL_LOCK_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TRX24_PLL_LOCK_vect\n");
		LED_PORT &= ~GREEN;
		__exit_isr();
	}

	/**
	 * \brief ISR for transceiver's transmit end interrupt
	 */
	ISR(TRX24_PLL_UNLOCK_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TRX24_PLL_UNLOCK_vect\n");
		LED_PORT |= GREEN;
		__exit_isr();
	}

	/**
	 * \brief ISR for transceiver's rx start interrupt
	 *
	 * By the time the SFD is detected, the hardware timestamps the
	 * current frame in the SCTSR register.
	 */
	ISR(TRX24_RX_START_vect, ISR_BLOCK)
	{
		__enter_isr();
		LED_PORT &= ~GREEN;
		static_dev->event_callback(static_dev, NETDEV_EVENT_RX_STARTED);
		__exit_isr();
	}

	/**
	 * \brief ISR for transceiver's receive end interrupt
	 */
	ISR(TRX24_RX_END_vect, ISR_BLOCK)
	{
		__enter_isr();
		static_dev->event_callback(static_dev,NETDEV_EVENT_RX_COMPLETE);
	    LED_PORT |= GREEN;
		__exit_isr();
	}

	ISR(TRX24_CCA_ED_DONE_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TRX24_CCA_ED_DONE \n");
		__exit_isr();
	}
	ISR(TRX24_XAH_AMI_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TRX24_XAH_AMI  \n");
		__exit_isr();
	}

	/**
	 * \brief ISR for transceiver's transmit end interrupt
	 */
	ISR(TRX24_TX_END_vect, ISR_BLOCK)
	{
		__enter_isr();
		// DEBUG("TRX24_TX_END_vect\n");
		static_dev->event_callback(static_dev, NETDEV_EVENT_TX_COMPLETE);
		LED_PORT |= RED;
		__exit_isr();

		/* set transceiver back to receiving state*/
		at86rf2xx_set_state((at86rf2xx_t*)static_dev, AT86RF2XX_TRX_STATE__RX_AACK_ON);
	}

	/**
	 * \brief ISR for transceiver's transmit end interrupt
	 */
	ISR(TRX24_AWAKE_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TRX24_AWAKE_vect\n");
		LED_PORT |= RED;
		__exit_isr();
	}

	ISR(SCNT_CMP1_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("SCNT_CMP1   \n");
		__exit_isr();
	}
	ISR(SCNT_CMP2_vect, ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("SCNT_CMP 2 \n");
		__exit_isr();
	}
	ISR(SCNT_CMP3_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("SCNT_CMP3   \n");
		__exit_isr();
	}
	ISR(SCNT_OVFL_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("SCNT_OVFL \n");
		__exit_isr();
	}
	ISR(SCNT_BACKOFF_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("SCNT_BACKOFF\n");
		__exit_isr();
	}
	ISR(AES_READY_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("AES_READY \n");
		__exit_isr();
	}
	ISR(BAT_LOW_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("BAT_LOW \n");
		__exit_isr();
	}

	/**
	 * \brief ISR for transceiver's TX_START interrupt
	 */
	ISR(TRX24_TX_START_vect)
	{
		__enter_isr();
		// DEBUG("TRX24_TX_START_vect\n");
		LED_PORT &= ~RED;
		static_dev->event_callback(static_dev, NETDEV_EVENT_TX_STARTED);
		__exit_isr();
	}

	ISR(TRX24_AMI0_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TRX24_AMI0  \n");
		__exit_isr();
	}
	ISR(TRX24_AMI1_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TRX24_AMI1  \n");
		__exit_isr();
	}
	ISR(TRX24_AMI2_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TRX24_AMI2  \n");
		__exit_isr();
	}
	ISR(TRX24_AMI3_vect , ISR_BLOCK)
	{
		__enter_isr();
		DEBUG("TRX24_AMI3  \n");
		__exit_isr();
	}

#endif

void at86rf2xx_setup(at86rf2xx_t *dev, const at86rf2xx_params_t *params)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &at86rf2xx_driver;
    /* initialize device descriptor */
    memcpy(&dev->params, params, sizeof(at86rf2xx_params_t));
    dev->idle_state = AT86RF2XX_STATE_TRX_OFF;
    dev->state = AT86RF2XX_STATE_SLEEP;
    dev->pending_tx = 0;

#ifdef MODULE_AT86RFR2
    // save device pointer for interrupts
    static_dev= netdev;
#endif
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
    eui64_t addr_long;

    at86rf2xx_hardware_reset(dev);

    /* Reset state machine to ensure a known state */
    at86rf2xx_reset_state_machine(dev);

    /* reset options and sequence number */
    dev->netdev.seq = 0;
    dev->netdev.flags = 0;

    /* get an 8-byte unique ID to use as hardware address */
    luid_get(addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN);
    /* make sure we mark the address as non-multicast and not globally unique */
    addr_long.uint8[0] &= ~(0x01);
    addr_long.uint8[0] |=  (0x02);
    /* set short and long address */
    at86rf2xx_set_addr_long(dev, NTOHLL(addr_long.uint64.u64));
    at86rf2xx_set_addr_short(dev, NTOHS(addr_long.uint16[0].u16));

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

#ifdef MODULE_AT86RFR2
    /* no spi,
     * no clock out pin
     *  TODO replace for functionality for RFR2? */

    /* clear interrupt flags by writing corresponding bit */
	at86rf2xx_reg_write( dev, AT86RF2XX_REG__IRQ_STATUS,  0xff );
	at86rf2xx_reg_write( dev, AT86RF2XX_REG__IRQ_STATUS1, 0xff );

	/* TODO now only enable necessary interrupts,
	   maybe other interrupts could be useful */

	/* enable interrupts IRQ_MASK*/
	at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK,
			// AT86RF2XX_IRQ_STATUS_MASK__AWAKE
			 AT86RF2XX_IRQ_STATUS_MASK__TX_END_EN
			 //| AT86RF2XX_IRQ_STATUS_MASK__AMI_EN
			 //| AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE_EN
			| AT86RF2XX_IRQ_STATUS_MASK__RX_END_EN
			| AT86RF2XX_IRQ_STATUS_MASK__RX_START_EN
			//| AT86RF2XX_IRQ_STATUS_MASK__PLL_UNLOCK
			//| AT86RF2XX_IRQ_STATUS_MASK__PLL_LOCK
						);

	/* enable interrupts IRQ_MASK1*/
	   at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK1,
			   AT86RF2XX_IRQ_STATUS_MASK1__TX_START_EN
			   //| AT86RF2XX_IRQ_STATUS_MASK1__MAF_0_AMI_EN
			   //| AT86RF2XX_IRQ_STATUS_MASK1__MAF_1_AMI_EN
			   //| AT86RF2XX_IRQ_STATUS_MASK1__MAF_2_AMI_EN
			   //| AT86RF2XX_IRQ_STATUS_MASK1__MAF_3_AMI_EN
						);
   /* set PLL on */
	   at86rf2xx_set_state(dev, AT86RF2XX_STATE_PLL_ON);
#else
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
#endif

	/* go into RX state */
	at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_AACK_ON);

    DEBUG("at86rf2xx_reset(): reset complete.\n");
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
    netdev_t *netdev = (netdev_t *)dev;

    /* write frame length field in FIFO */
    at86rf2xx_sram_write(dev, 0, &(dev->tx_frame_len), 1);
    /* trigger sending of pre-loaded frame */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE,
                        AT86RF2XX_TRX_STATE__TX_START);

    /* TODO MODULE_AT86RFR2 change to interrupt
     * or remove complete upper layer does not handle TX_start interrupt
     */

    if (netdev->event_callback &&
        (dev->netdev.flags & AT86RF2XX_OPT_TELL_TX_START)) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
    }
}
