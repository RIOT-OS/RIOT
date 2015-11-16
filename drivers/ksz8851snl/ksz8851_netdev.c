/*
 * Copyright (C) 2015 Matt Poppe <matt@poppe.me>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_ksz8851snl
 * @{
 *
 * @file
 * @brief       Internal functions for the ksz8851snl driver
 *
 * @author       Matt Poppe <matt@poppe.me>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "mutex.h"
#include "ksz8851snl_regs.h"
#include "ksz8851snl.h"

#include "xtimer.h"

#include "net/netdev2.h"
#include "net/eui64.h"
#include "net/ethernet.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SECT_NAME "[KSZ8851 Driver]:"

static void lock(ksz8851snl_t *dev)
{
	mutex_lock(&dev->mutex);
}

static void unlock(ksz8851snl_t *dev)
{
	mutex_unlock(&dev->mutex);
}

static void ksz_spi_transfer(ksz8851snl_t *dev, char *out, char *in, int len)
{
	spi_acquire(dev->spi);
	gpio_clear(dev->cs);
	spi_transfer_bytes(dev->spi, out, in, len);
	gpio_set(dev->cs);
	spi_release(dev->spi);
}

static void reg_set(ksz8851snl_t *dev, uint16_t reg, uint16_t value)
{
    /* TODO: Make any changes that could be needed from Byte Order */
	uint16_t cmd = (reg << 2) & 0x3F0;

	if (reg & 0x02)
		cmd |= IO_REG_WRITE | KSZ8851_CMD_B3 | KSZ8851_CMD_B2;
	else
		cmd |= IO_REG_WRITE | KSZ8851_CMD_B1 | KSZ8851_CMD_B0;

	char cmdChain[4] = { (cmd >> 8), (cmd & 0xff), (value & 0xff), (value >> 8) };

	ksz_spi_transfer(dev, cmdChain, NULL, 4);
}

static uint16_t reg_get(ksz8851snl_t *dev, uint16_t reg)
{
	uint16_t cmd = (reg << 2) & 0x3F0;

	if (reg & 0x2)
		cmd |= IO_REG_READ | KSZ8851_CMD_B3 | KSZ8851_CMD_B2;
	else
		cmd |= IO_REG_READ | KSZ8851_CMD_B1 | KSZ8851_CMD_B0;

	char cmdChain[4] = { (cmd >> 8), (cmd & 0xff), 0, 0 };

	char result[4];

	ksz_spi_transfer(dev, cmdChain, result, 4);

	return result[2] | (result[3] << 8);
}

static void reg_bits_set(ksz8851snl_t *dev, uint16_t reg, uint16_t bit_mask)
{
	uint16_t reg_preValue = reg_get(dev, reg);
	uint16_t reg_value = reg_preValue | bit_mask;

	reg_set(dev, reg, reg_value);
}

static void reg_bits_clear(ksz8851snl_t *dev, uint16_t reg, uint16_t bit_mask)
{
	uint16_t reg_value;

	reg_value = reg_get(dev, reg);
	reg_value &= ~bit_mask;
	reg_set(dev, reg, reg_value);
}

static uint8_t reg_get8(ksz8851snl_t *dev, uint16_t reg)
{
	uint16_t cmd = (reg << 2) & 0x3F0;

	if (reg & 0x2)
		if (reg & 0x1)
			cmd |= IO_REG_READ | KSZ8851_CMD_B3;
		else
			cmd |= IO_REG_READ | KSZ8851_CMD_B2;

	else {
		if (reg & 0x1)
			cmd |= IO_REG_READ | KSZ8851_CMD_B1;
		else
			cmd |= IO_REG_READ | KSZ8851_CMD_B0;
	}
	char cmdChain[3] = { (cmd >> 8), (cmd & 0xff), 0 };

	char result[3];

	ksz_spi_transfer(dev, cmdChain, result, 3);

	return result[2];
}


static void ksz8851snl_get_mac_addr(netdev2_t *encdev, uint8_t* buf)
{
	ksz8851snl_t * dev = (ksz8851snl_t*)encdev;

	lock(dev);
	for (uint8_t s = 0; s < ETHERNET_ADDR_LEN; s++)
    {
		*buf++ = reg_get8(dev, 0x15 - s);
    }
    unlock(dev);
}

static int ksz8851snl_get_iid(netdev2_t *netdev, eui64_t *value, size_t max_len)
{
	if (max_len < sizeof(eui64_t))
    {
		return -EOVERFLOW;
    }

	uint8_t addr[ETHERNET_ADDR_LEN];
	ksz8851snl_get_mac_addr(netdev, addr);
	ethernet_get_iid(value, addr);

	return sizeof(eui64_t);
}

static int ksz8851snl_get(netdev2_t *dev, netopt_t opt, void *value, size_t max_len)
{
	int res = -EINVAL;
	uint16_t *tgt = (uint16_t *)value;

	switch (opt) {
		case NETOPT_ADDRESS: //2
			if (max_len < ETHERNET_ADDR_LEN)
				res = -EINVAL;
			else {
				ksz8851snl_get_mac_addr(dev, (uint8_t*)value);
				res = ETHERNET_ADDR_LEN;
			}
			break;
		case NETOPT_ADDR_LEN: //4
		case NETOPT_SRC_LEN: //5
			assert(max_len == 2);
			*tgt = 6;
			res = sizeof(uint16_t);
			break;
		case NETOPT_IPV6_IID: //7
			return ksz8851snl_get_iid(dev, value, max_len);
		case NETOPT_MAX_PACKET_SIZE: //9
            *tgt = 1500;
            res=sizeof(uint16_t);
            break;
		case NETOPT_STATE: //15
			res = 0;
			*tgt = NETOPT_STATE_IDLE;
			break;
		case NETOPT_IS_WIRED: //24
			res = 1;
			break;
		case NETOPT_DEVICE_TYPE: //25
			*tgt = NETDEV2_TYPE_ETHERNET;
			res = 2;
		 	break;
		default:
			DEBUG(SECT_NAME"Device Get Opt Called and not handled: %d\n", opt);
			res = -ENOTSUP;
			break;
	}
	return res;
}

static int ksz8851snl_set(netdev2_t *dev, netopt_t opt, void *value, size_t value_len)
{
	int res = 0;

	switch (opt) {
	default:
		printf("Device Set Opt Called: %d\n", opt);
		return -ENOTSUP;
	}

	return res;
}

/*
 * Process for transmitting packet
 * 1. Check if the send buffer can hold the packet register: TXMIR
 * 1b. TODO: If there is not room on the KSZ buffer you can setup an interrupt
 *           for when there is room left
 * 2. Write a "1" to RXQCR[3] to enable TXQ write access
 *   a. control word
 *   b. byte count
 *   c. pkt data
 * 3. Write a "0" to RXQCR[3] to end TXQ write access
 * 4. Write a "1" to TXQCR[0] to issue a tx command
 * 5. (optional) read ISR[14] to verify the Tx is complete then write "1" to ISR[14] to clear
 */

static int ksz8851snl_send(netdev2_t *netdev, const struct iovec *vector, int count)
{
	ksz8851snl_t * dev = (ksz8851snl_t*)netdev;

	uint16_t frame_len = 0;
	uint16_t len_avail;

	/* Lock the device and clear IRQ */
	lock(dev);
    /* TODO: Decide if it is better to clear IRQ Enable on interface or disable IRQ Pin */
	reg_set(dev, KSZ8851_IER_REG, 0x0000);

	/* Get total len of frame */
	for (int i = 0; i < count; i++) {
		frame_len += vector[i].iov_len;
	}

	/* Check if there is available room addition 8 bytes is needed for KSZ header */
	len_avail = reg_get(dev, KSZ8851_TXMIR_REG);
 	if (len_avail >= frame_len + 8)
    {
 		char tx[2];
 		/* Write fifo command */
		tx[0] = 0xc0;

		/* Enable access the the Tx Queue */
		reg_bits_set(dev, KSZ8851_RXQCR_REG, RXQCR_QUEUE_ACCESS_EN);

		spi_acquire(dev->spi);
		gpio_clear(dev->cs);

		/* Send Write fifo command */
		spi_transfer_bytes(dev->spi, tx, NULL, 1);

		/* Send Command Frame */
		tx[0] = 0x00;
		tx[1] = 0x80;
		spi_transfer_bytes(dev->spi, tx, NULL, 2);

		/* Send Write Len */
		tx[0] = frame_len & 0xff;
		tx[1] = (frame_len >> 8) & 0x07;
		spi_transfer_bytes(dev->spi, tx, NULL, 2);

		for (int i = 0; i < count; i++) {
			spi_transfer_bytes(dev->spi, vector[i].iov_base, NULL, vector[i].iov_len);
		}

        spi_transfer_bytes(dev->spi, NULL, NULL, frame_len % 4);

		gpio_set(dev->cs);
		spi_release(dev->spi);

	} else
    {
		DEBUG(SECT_NAME"Tx buffer to small to handle request\n");
	}

	/* Disable queue write access and enable Tx of Queue */
    reg_bits_clear(dev, KSZ8851_RXQCR_REG, RXQCR_QUEUE_ACCESS_EN);
	reg_bits_set(dev, KSZ8851_TXQCR_REG, TXQCR_TX_CMD);

	unlock(dev);
	reg_set(dev, KSZ8851_IER_REG, IER_DEFAULT_CONFIG);

	return frame_len;
}

/*
 *  Steps that need to be taken after we get a Rx interrupt
 * 1. Read what the source of the intrrupt is from in TXQCR[10:12]
 * 2. Read the total frame count in RXFCTR
 * 3. Read frame header status RXFHSR
 * 4. Read byte count for the frame RXFHBCR
 * 5. Write 0x000 to RXFDPR[0:10] to clear rx buffer pointer
 * 6. Write a "1" to RXQCR[3] to enable access to Rx queue
 * 7. Read all the buffer count from step 4
 * 8. Write a "0" to RXQCR[3] to disable access to the Rx queue
 *
 */

static int ksz8851snl_recv(netdev2_t *netdev, char* buf, int len)
{
	ksz8851snl_t * dev = (ksz8851snl_t*)netdev;

	uint16_t frame_size = 0;
	uint16_t frame_status;

	lock(dev);

	/* Read frame header status */
	if (len == 0 && (!buf)) {

		frame_status = reg_get(dev, KSZ8851_RXFHSR_REG);

		if ((frame_status & RXFHSR_RX_VALID_FRAME) != RXFHSR_RX_VALID_FRAME) {

			reg_set(dev, KSZ8851_RXQCR_REG, (RXQCR_RELEASE_RX_ERR | RXQCR_DEFAULT_CONFIG));
			DEBUG(SECT_NAME"Frame Rx Error: 0x%04X\n", frame_status);

			/* Release device IRQ will be returned back by irq receive handler */
			unlock(dev);
			return 0;
		}
		/*
		 * read the size of the frame return with the value then this function will be called
		 * with a valid buffer
		 */
		frame_size = reg_get(dev, KSZ8851_RXFHBCR_REG) & 0x7FF;

	} else {
		/* use the buffer length as the frame size to keep from having to make another KSZ call */
		frame_size = len;
		/* Reset rx buffer pointer */
		reg_set(dev, KSZ8851_RXFDPR_REG, RXFDPR_RESET_PTR_AUTO_INC);

		/* enable access to Rx Queue */
		reg_bits_set(dev, KSZ8851_RXQCR_REG, RXQCR_QUEUE_ACCESS_EN);

		/* Form Read packet */
		char tx_cmd[4];
		tx_cmd[0] = 0x2 << 6;

		/* Acquire SPI and SNL Control */
		spi_acquire(dev->spi);
		gpio_clear(dev->cs);

		/* Send Read fifo command */
		spi_transfer_bytes(dev->spi, tx_cmd, NULL, 1);

		/* Dummy eight byte read to avoid dummy bytes - status word - length word */
		spi_transfer_bytes(dev->spi, NULL, NULL, 8);

		/* 2 bytes to align to word */
		spi_transfer_bytes(dev->spi, NULL, NULL, 2);

		/* Transfer Data */
		spi_transfer_bytes(dev->spi, NULL, buf, (len - 6));

		/* Maintain alignment to 4-byte boundaries */
		spi_transfer_bytes(dev->spi, NULL, NULL, len % 4);

		/* Release SNL and SPI control */
		gpio_set(dev->cs);
		spi_release(dev->spi);

		/* disable access to the Rx queue */
		reg_bits_clear(dev, KSZ8851_RXQCR_REG, RXQCR_QUEUE_ACCESS_EN);
	}
	unlock(dev);
	return frame_size;
}

/* This is IRQ Handler for KSZ8851SNL: privileged IRQ keep short */
static void ksz8851snl_pIsr(void *netdev)
{
	ksz8851snl_t *dev = (ksz8851snl_t*)netdev;

	/* disable interrupt line */
	gpio_irq_disable(dev->int_pin);

	/* call netdev2 even to init frame receive process */
	dev->netdev.event_callback((netdev2_t*)dev, NETDEV2_EVENT_ISR, NULL);
}

static void ksz8851snl_isr(netdev2_t *netdev)
{
	ksz8851snl_t *dev = (ksz8851snl_t*)netdev;
	uint16_t isr_mask;

	lock(dev);

	/* Read interrupt status register */
	isr_mask = reg_get(dev, KSZ8851_ISR_REG);//TODO: DO I need to do this before I reset the isr

	/** Interrupt Handlers **/
	/*  IRQ Linkup Detect IRQ */
	if (isr_mask & ISR_LDIS) {
		uint16_t temp_irq = reg_get(dev, KSZ8851_PMECR_REG);
		temp_irq |= (1 << 3);
		temp_irq &= (~(0x000D << 2));
		reg_set(dev, KSZ8851_PMECR_REG, temp_irq);
	}

	/* Link Change IRQ */
	if (isr_mask & ISR_LCIS) {
		/* Check and report what the current status of the link */
		uint16_t link = reg_get(dev, KSZ8851_P1SR_REG);

        /* TODO Send link is up after grnc_netdev2 supports the call
		netdev2_event_t event = (link & P1SR_LINK_GOOD) ?
					NETDEV2_EVENT_LINK_UP :
					NETDEV2_EVENT_LINK_DOWN;
        dev->netdev.event_callback(netdev, event, NULL);
        */
		if (link & P1SR_LINK_GOOD)
			DEBUG(SECT_NAME "Link Status is Up\n");
		else
			DEBUG(SECT_NAME "Link Status is Down\n");

		reg_set(dev, KSZ8851_ISR_REG, ISR_LCIS);

	}

	/* Packet Received IRQ */
	if (isr_mask & ISR_RXIS) {
		/* Reset the IRQ Flag */
		reg_set(dev, KSZ8851_ISR_REG, ISR_RXIS);

		uint16_t frames_avail = (reg_get(dev, KSZ8851_RXFCTR_REG) >> 8);
		while (frames_avail) {
			unlock(dev);
			netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE, NULL);
			lock(dev);
			frames_avail--;
		}
	}
	/* Restore IRQs and Release Mutex */
    unlock(dev);
	gpio_irq_enable(dev->int_pin);

}

static int ksz8851snl_reset(ksz8851snl_t *dev)
{
	uint16_t dev_id;

	/* First attempt reset is a soft reset */
	reg_set(dev, KSZ8851_GRR_REG, GRR_DEV_RESET);
	xtimer_usleep(1000);
	reg_set(dev, KSZ8851_GRR_REG, 0);

	/* Check for a valid Chip */
	dev_id = reg_get(dev, CHIP_ID_REG);

    DEBUG(SECT_NAME"Chip id: 0x%04X\n", dev_id);

	if ((dev_id & 0xFFF0) == CHIP_ID_8851_16)
		return 0;

	/* Second attempt reset is a hard reset */
	gpio_clear(dev->rst);
	xtimer_usleep(KSZ8851SNL_RST_DELAY);
	gpio_set(dev->rst);
	xtimer_usleep(KSZ8851SNL_BOOT_DELAY);

	/* Check for a valid Chip */
	dev_id = reg_get(dev, CHIP_ID_REG);

	if ((dev_id & 0xFFF0) == CHIP_ID_8851_16)
		return 0;

	DEBUG(SECT_NAME "Reset Failed\n");
	return 0;
}

static int ksz8851snl_init(netdev2_t *encdev)
{
	ksz8851snl_t *dev = (ksz8851snl_t*)encdev;
	lock(dev);

	/*** Setup IO ****/

	/* Setup Chip Select */
	gpio_init(dev->cs, GPIO_DIR_OUT, GPIO_PULLUP);
	gpio_set(dev->cs);

	/* Setup IRQ */
	gpio_init(dev->int_pin, GPIO_DIR_IN, GPIO_PULLUP);
	gpio_init_int(dev->int_pin, GPIO_PULLUP, GPIO_FALLING, ksz8851snl_pIsr, (void*)dev);

	/* Setup RST */
	gpio_init(dev->rst, GPIO_DIR_OUT, GPIO_PULLUP);
	gpio_set(dev->rst);

	/*** Setup SPI ***/
	if (spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, KSZ8851SNL_SPI_SPEED) < 0) return -1;

	/* Soft reset and if that fails hard reset */
	ksz8851snl_reset(dev);

	/* Set Power Mode */
	reg_set(dev, KSZ8851_PMECR_REG, PMECR_NORMAL);

	/* Setup MAC from config file TODO: Create a better way to setup MAC*/
	reg_set(dev, KSZ8851_MARL_REG, KSZ8851_MAC_LOW);
	reg_set(dev, KSZ8851_MARM_REG, KSZ8851_MAC_MID);
	reg_set(dev, KSZ8851_MARH_REG, KSZ8851_MAC_HIGH);

	#if ENABLE_DEBUG
	uint16_t mac_low = reg_get(dev, KSZ8851_MARL_REG);
	uint16_t mac_middle = reg_get(dev, KSZ8851_MARM_REG);
	uint16_t mac_high = reg_get(dev, KSZ8851_MARH_REG);

	DEBUG(SECT_NAME "Mac address %02x:%02x:%02x:%02x:%02x:%02x\n", mac_high >> 8, mac_high & 0xff, mac_middle >> 8, mac_middle & 0xff, mac_low >> 8, mac_low & 0xff);
	#endif

	/*** SETUP Tx ***/
	/* Init step5: enable QMU Transmit Frame Data Pointer Auto Increment. */
	reg_set(dev, KSZ8851_TXFDPR_REG, TXFDPR_TXFPAI);

	/* Init step6: configure QMU transmit control register. */
	reg_set(dev, KSZ8851_TXCR_REG, TXCR_DEFAULT_CONFIG);

	/*** Setup Rx ***/

	/* Init step7: enable QMU Receive Frame Data Pointer Auto Increment. */
	reg_set(dev, KSZ8851_RXFDPR_REG, RXFDPR_RXFPAI);

	/* Setup Rx QMU IRQ Thresholds */
	reg_set(dev, KSZ8851_RXFCTR_REG, KSZ8851_DEFAULT_FRAME_CNT_IRQ_THRES);
	reg_set(dev, KSZ8851_RXDTTR_REG, KSZ8851_DEFAULT_RX_DURATION_TIMER_IRQ);
	reg_set(dev, KSZ8851_RXDBCTR_REG, KSZ8851_DEFAULT_BYTE_CNT_IRQ);

	/* Setup Rx QMU CR1. Defaults are defined in Config file*/
	reg_set(dev, KSZ8851_RXCR1_REG, RXCR1_DEFAULT_CONFIG);

	/* Setup RX QMU CR2. Defaults are defined in Config file*/
	reg_set(dev, KSZ8851_RXCR2_REG, RXCR2_DEFAULT_CONFIG);

	/* Setup QMU IRQ types Defaults are defined in Config file */
	reg_set(dev, KSZ8851_RXQCR_REG, RXQCR_DEFAULT_CONFIG);

	/* Adjust data output clock TODO Make sure this is needed */
	reg_set(dev, KSZ8851_OBCR_REG, BUS_CLOCK_166 | BUS_CLOCK_DIVIDEDBY_1);

	/* Put in half duplex if not in AN or if AN fails */
	reg_bits_clear(dev, KSZ8851_P1CR_REG, P1CR_FORCE_DUPLEX);
	reg_bits_set(dev, KSZ8851_P1CR_REG, P1CR_AN_ENABLE);

	/* Clear all interrupts */
	reg_set(dev, KSZ8851_ISR_REG, 0xFFFF);

	/* Setup KSZ8851 IRQs Defaults are defined in Config file*/
	reg_set(dev, KSZ8851_IER_REG, IER_DEFAULT_CONFIG);
	reg_set(dev, KSZ8851_RXFDPR_REG, RXFDPR_RESET_PTR_AUTO_INC);

	/* enable Tx & Rx QMU. */
	reg_bits_set(dev, KSZ8851_TXCR_REG, TXCR_TXE);
    reg_bits_set(dev, KSZ8851_RXCR1_REG, RXCR1_RXE);

	DEBUG(SECT_NAME "initialization complete.\n");

	unlock(dev);
	gpio_irq_enable(dev->int_pin);

	return 0;
}

const static netdev2_driver_t netdev2_driver_ksz8851snl = {
	.send	= ksz8851snl_send,
	.recv	= ksz8851snl_recv,
	.init	= ksz8851snl_init,
	.isr	= ksz8851snl_isr,
	.get	= ksz8851snl_get,
	.set	= ksz8851snl_set,
};

void ksz8851snl_setup(ksz8851snl_t *dev, spi_t spi, gpio_t cs, gpio_t int_pin,
		      gpio_t rst_pin)
{
	dev->netdev.driver = &netdev2_driver_ksz8851snl;
	dev->spi = spi;
	dev->cs = cs;
	dev->rst = rst_pin;
	dev->int_pin = int_pin;

	mutex_init(&dev->mutex);
}
