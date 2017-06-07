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

#include "net/gnrc/netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_AT86RFR2

#include "thread.h"
#include "msg.h"



// char at86rfr2_interrupt_thread_stack[256];
// static msg_t _int_msg_queue[16]; // must be a power of 2

/*TODO port all functions from at86rf2xx_netdev.c _isr() */

	#include "avr/interrupt.h"
	// saved device Pointer for Interrupt callback
	static netdev_t* static_dev;
	// static kernel_pid_t at86rfr2_interrupt_thread_pid;

//	/*
//	* Send massage to gnrc_netdev containing interrupt reason.
//	*/
//	static inline void at86rfr2_isr_handler(uint8_t vect_num)
//	{
//		gnrc_netdev_t *gnrc_netdev = (gnrc_netdev_t*) static_dev->context;
//
//		msg_t msg;
//		msg.type = NETDEV_MSG_TYPE_EVENT;
//		msg.content.ptr = gnrc_netdev;
//		msg.content.value = (uint32_t) vect_num;
//
//		if (msg_send(&msg, gnrc_netdev->pid) <= 0) {
//			puts("gnrc_netdev: possibly lost interrupt.");
//		}
//	}
//		msg_t m;
//		m.content.value = vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//
//	    if (sched_context_switch_request) {
//	        thread_yield();
//	    }
//	}
//
//	void *at86rfr2_interrupt_thread(void *arg)
//	{
//		(void) arg;
//
//		// DEBUG("at86rfr2_interrupt_thread started, pid: %" PRIkernel_pid "\n", thread_getpid());
//
//		/* for the thread running the shell */
//		msg_init_queue(_int_msg_queue, sizeof(_int_msg_queue));
//
//		msg_t m;
//
//		while (1)
//		{
//
//			msg_receive(&m);
//
//			printf("[at86rf2xx] at86rfr2_interrupt_thread got massage from Interrupt Vector: %u\n", m.content.value);
//
//			/*
//			 * If transceiver is sleeping there should be not interrupts triggered.
//			 */
//			at86rf2xx_t *dev = (at86rf2xx_t *) static_dev;
//			uint8_t state = at86rf2xx_get_status(dev);
//
//			if (state == AT86RF2XX_STATE_SLEEP)
//			{
//				printf("[at86rf2xx] Error at86rfr2_interrupt_thread: Interrupt triggered while transceiver is in sleep.\n");
//			}
//			else if ( !(static_dev->event_callback) )
//			{
//				printf("[at86rf2xx] Error at86rfr2_interrupt_thread: No callback available.\n");
//			}
//			else
//			{
//				/*
//				 * Check which Interrupt send a massage.
//				 * Send events to upper layer if configured else don't
//				 */
//
//
//				switch( vect_num )// m.content.value )
//				{
//				case TRX24_PLL_LOCK_vect_num:
//						printf("[at86rf2xx] at86rfr2_interrupt_thread: PLL Locked.\n");
//					break;
//					case TRX24_RX_START_vect_num:
//							static_dev->event_callback(static_dev, NETDEV_EVENT_RX_STARTED);
//							DEBUG("[at86rf2xx] EVT - RX_START\n");
//						break;
//					case TRX24_RX_END_vect_num:
//							if (state == AT86RF2XX_STATE_RX_AACK_ON ||
//								state == AT86RF2XX_STATE_BUSY_RX_AACK)
//							{
//								DEBUG("[at86rf2xx] EVT - RX_END\n");
//								if ( !( ((at86rf2xx_t*)static_dev)->netdev.flags & AT86RF2XX_OPT_TELL_RX_END) )
//								{
//									break;
//								}
//								static_dev->event_callback(static_dev, NETDEV_EVENT_RX_COMPLETE);
//							}
//							else if ( state == AT86RF2XX_STATE_TX_ARET_ON ||
//									  state == AT86RF2XX_STATE_BUSY_TX_ARET )
//							{
//								/* check for more pending TX calls and return to idle state if
//								 * there are none */
//								assert( dev->pending_tx != 0);
//								if ((--dev->pending_tx) == 0)
//								{
//									at86rf2xx_set_state(dev, dev->idle_state);
//									DEBUG("[at86rf2xx] return to state 0x%x\n", dev->idle_state);
//								}
//							}
//							DEBUG("[at86rf2xx] EVT - TX_END\n");
//						break;
//					case TRX24_TX_END_vect_num:
//							/*
//							 * If interrupt is triggered check if upper layer wants to be informed.
//							 * If not give error then the interrupt could be disabled.
//							 */
//							if ( dev->netdev.flags & AT86RF2XX_OPT_TELL_TX_END)
//							{
//								uint8_t trac_status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATE) &
//											  AT86RF2XX_TRX_STATE_MASK__TRAC;
//
//								switch (trac_status)
//								{
//									case AT86RF2XX_TRX_STATE__TRAC_SUCCESS:
//									case AT86RF2XX_TRX_STATE__TRAC_SUCCESS_DATA_PENDING:
//										static_dev->event_callback(static_dev, NETDEV_EVENT_TX_COMPLETE);
//										DEBUG("[at86rf2xx] TX SUCCESS\n");
//										break;
//									case AT86RF2XX_TRX_STATE__TRAC_NO_ACK:
//										static_dev->event_callback(static_dev, NETDEV_EVENT_TX_NOACK);
//										DEBUG("[at86rf2xx] TX NO_ACK\n");
//										break;
//									case AT86RF2XX_TRX_STATE__TRAC_CHANNEL_ACCESS_FAILURE:
//										static_dev->event_callback(static_dev, NETDEV_EVENT_TX_MEDIUM_BUSY);
//										DEBUG("[at86rf2xx] TX_CHANNEL_ACCESS_FAILURE\n");
//										break;
//									default:
//										DEBUG("[at86rf2xx] Unhandled TRAC_STATUS: %d\n",trac_status >> 5);
//								}
//							}
//							/* set transceiver back to receiving state*/
//							// do it after the packet was read in _rec() function
//							//at86rf2xx_set_state((at86rf2xx_t*)static_dev, AT86RF2XX_TRX_STATE__RX_AACK_ON);
//						break;
//					default:
//							printf("[at86rf2xx] Error: Unhandled Interrupt %u\n",  m.content.value );
//						break;
//				}
//			}
//		}
//	}


//	if (((at86rf2xx_t*)static_dev)->netdev.flags & AT86RF2XX_OPT_TELL_RX_END)
//	{
//
//	}

	/**
	 * \brief ISR for transceiver's PLL Lock interrupt
	 */
	ISR(TRX24_PLL_LOCK_vect, ISR_BLOCK)
	{
		__enter_isr();
		LED_PORT &= ~GREEN;
		((at86rf2xx_t*)static_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__PLL_LOCK_EN;
		static_dev->event_callback(static_dev, NETDEV_EVENT_ISR);
		DEBUG("TRX24_PLL_LOCK\n");
		__exit_isr();
	}

	/**
	 * \brief ISR for transceiver's PLL unlock interrupt
	 */
	ISR(TRX24_PLL_UNLOCK_vect, ISR_BLOCK)
	{
		__enter_isr();
		((at86rf2xx_t*)static_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__PLL_UNLOCK_EN;
		static_dev->event_callback(static_dev, NETDEV_EVENT_ISR);
		LED_PORT |= GREEN;
		DEBUG("TRX24_PLL_UNLOCK\n");
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
		((at86rf2xx_t*)static_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__RX_START_EN;
		static_dev->event_callback(static_dev, NETDEV_EVENT_ISR);
		DEBUG("TRX24_RX_START\n");
		__exit_isr();
	}

	/**
	 * \brief ISR for transceiver's receive end interrupt
	 */
	ISR(TRX24_RX_END_vect, ISR_BLOCK)
	{
		__enter_isr();
		((at86rf2xx_t*)static_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__RX_END_EN;
		/*
		 * p. 99, 9.8.7 Dynamic Frame Buffer Protection
		 * Enabled in reset function to Protect content in
		 * frame from being overwritten by  new frames which are received
		 * */

		static_dev->event_callback(static_dev, NETDEV_EVENT_ISR);
		DEBUG("TRX24_RX_END\n");
		__exit_isr();

//		__ENTER_ISR();
//		IF ( ((AT86RF2XX_T*)STATIC_DEV)->NETDEV.FLAGS & AT86RF2XX_OPT_TELL_RX_END)
//		{
//			STATIC_DEV->EVENT_CALLBACK(STATIC_DEV, NETDEV_EVENT_RX_COMPLETE);
//		}
//		DEBUG("[AT86RF2XX] EVT - RX_END\N");
//
//		/* TRANSCEIVER WILL BE IN RX_AACK_ON OR BUSY_RX_AACK STATE
//		 *
//		 * PACKET IS READ IN THE EVENT_CALLBACK AND FRAME BUFFER PROTECTION WILL BE RELEASED THEN.
//		 * NEW DATA WILL NOT OVERRIDE THE OLD IN FRAME BUFFER.
//		 */
//		/* SHOULD NOT BE NECESSARY*/
//		// AT86RF2XX_SET_STATE(DEV, AT86RF2XX_TRX_STATE__RX_AACK_ON);
//		__EXIT_ISR();
	}

//	ISR(TRX24_CCA_ED_DONE_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = TRX24_CCA_ED_DONE_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}
	/* Transceiver Frame Address Match
	 * Indicates address matching
	 *
	 * */
	ISR(TRX24_XAH_AMI_vect , ISR_BLOCK)
	{
		__enter_isr();
		((at86rf2xx_t*)static_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__AMI_EN;
		static_dev->event_callback(static_dev, NETDEV_EVENT_ISR);
		DEBUG("TRX24_XAH_AMI\n");
		__exit_isr();
	}

	/**
	 * \brief ISR for transceiver's transmit end interrupt
	 */
	ISR(TRX24_TX_END_vect, ISR_BLOCK)
	{
		__enter_isr();
		((at86rf2xx_t*)static_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__TX_END_EN;
		static_dev->event_callback(static_dev, NETDEV_EVENT_ISR);
		/* don't set transceiver back to receiving state
		* it will be in TX_ARET_ON
		*/
		DEBUG("TRX24_TX_END\n");
		__exit_isr();
	}

//	/**
//	 * \brief ISR for transceiver's transmit end interrupt
//	 */
//	ISR(TRX24_AWAKE_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = TRX24_AWAKE_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		LED_PORT |= RED;
//		__exit_isr();
//	}

//	ISR(SCNT_CMP1_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = SCNT_CMP1_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}
//	ISR(SCNT_CMP2_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = SCNT_CMP2_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}
//	ISR(SCNT_CMP3_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = SCNT_CMP3_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}
//	ISR(SCNT_OVFL_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = SCNT_OVFL_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}
//	ISR(SCNT_BACKOFF_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = SCNT_BACKOFF_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}
//	ISR(AES_READY_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = AES_READY_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}

	/**
	 * \brief ISR for transceiver's TX_START interrupt
	 */
	ISR(TRX24_TX_START_vect)
	{
		__enter_isr();
		((at86rf2xx_t*)static_dev)->irq_status1 |= AT86RF2XX_IRQ_STATUS_MASK1__TX_START_EN;
		static_dev->event_callback(static_dev, NETDEV_EVENT_ISR);
		LED_PORT &= ~RED;
		DEBUG("TRX24_TX_START\n");
		__exit_isr();
	}
//
//	/* address match interrupt from address filter #0,
//	 * enabled bit AMI0 in register IRQ_MASK1 is set.
//	 *
//	 */
//	ISR(TRX24_AMI0_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = TRX24_AMI0_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}
//	ISR(TRX24_AMI1_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = TRX24_AMI1_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}
//	ISR(TRX24_AMI2_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = TRX24_AMI2_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}
//	ISR(TRX24_AMI3_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		msg_t m;
//		m.content.value = TRX24_AMI3_vect_num;
//		msg_send_int(&m, at86rfr2_interrupt_thread_pid);
//		__exit_isr();
//	}

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
    static_dev= (netdev_t *)dev;

//    /* add thread to handle interrupt massages */
//    at86rfr2_interrupt_thread_pid = thread_create(at86rfr2_interrupt_thread_stack, sizeof(at86rfr2_interrupt_thread_stack),
//                                2, THREAD_CREATE_STACKTEST,
//								at86rfr2_interrupt_thread, NULL, "at86rfr2_int");

    /* set all interrupts off */

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, 0x00);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK1,0x00);

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
    at86rf2xx_set_addr_long(dev, ntohll(addr_long.uint64.u64));
    at86rf2xx_set_addr_short(dev, ntohs(addr_long.uint16[0].u16));

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

#ifdef MODULE_AT86RFR2
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_TX_END, true);
#endif

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
    /* no spi,s
     *
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
			// | AT86RF2XX_IRQ_STATUS_MASK__AMI_EN
			//| AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE_EN
			| AT86RF2XX_IRQ_STATUS_MASK__RX_END_EN
			//| AT86RF2XX_IRQ_STATUS_MASK__RX_START_EN
			//| AT86RF2XX_IRQ_STATUS_MASK__PLL_UNLOCK_EN
			// | AT86RF2XX_IRQ_STATUS_MASK__PLL_LOCK_EN
						);

	/* enable interrupts IRQ_MASK1*/
	 at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK1, 0x00);
//			    AT86RF2XX_IRQ_STATUS_MASK1__TX_START_EN
//			   | AT86RF2XX_IRQ_STATUS_MASK1__MAF_0_AMI_EN
//			   | AT86RF2XX_IRQ_STATUS_MASK1__MAF_1_AMI_EN
//			   | AT86RF2XX_IRQ_STATUS_MASK1__MAF_2_AMI_EN
//			   | AT86RF2XX_IRQ_STATUS_MASK1__MAF_3_AMI_EN
//						);

	/* clear frame buffer protection */
	*AT86RF2XX_REG__TRX_CTRL_2 &= ~(1<<RX_SAFE_MODE);

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
						AT86RF2XX_IRQ_STATUS_MASK__TRX_END_EN);

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
