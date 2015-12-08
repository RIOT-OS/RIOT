/*
 * Copyright (C) 2015 Matt Poppe <matt@poppe.me>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_ksz8851snl ksz8851snl
 * @ingroup     drivers
 * @brief       Driver for the ksz8851snl Ethernet Adapter
 * @{
 *
 * @file
 * @brief       ksz8851snl Configuration File
 *
 * @author      Matt Poppe <matt@poppe.me>
 */


/*
 * TODO: Currently placed inside driver file just for initial convenance will
 * Get moved to the board folder in the future
 */

#ifndef KSZ8851SNL_CONFIG_H
#define KSZ8851SNL_CONFIG_H

#define KSZ8851SNL_SPI      SPI_1
#define KSZ8851SNL_CS       GPIO_PIN(PB, 3)
#define KSZ8851SNL_INT      GPIO_PIN(PA, 22)
#define KSZ8851SNL_RST      GPIO_PIN(PA, 28)
#define KSZ8851SNL_EEIRQ    GPIO_PIN(PA, 13)
#define KSZ8851SNL_DBGTRG   GPIO_PIN(PA, 8)
//#define
#define KSZ8851SNL_SPI_SPEED SPI_SPEED_5MHZ

#define KSZ8851SNL_RST_DELAY (100000U)
#define KSZ8851SNL_BOOT_DELAY (100000U)

#define KSZ8851_MAC_LOW  	0x1a3e
#define KSZ8851_MAC_MID		0x2519
#define KSZ8851_MAC_HIGH	0x0004
#define KSZ_TX_BLOCKING_EN  0 /* This may get deleted */

#define KSZ8851_DEFAULT_FRAME_CNT_IRQ_THRES     (1U)
#define KSZ8851_DEFAULT_RX_DURATION_TIMER_IRQ   (1000U)
#define KSZ8851_DEFAULT_BYTE_CNT_IRQ            (4096U)

#define IER_DEFAULT_CONFIG	 (IER_LCIE | IER_RXIE)

#define RXQCR_DEFAULT_CONFIG   (RXQCR_TWO_BYTE_OFFSET | RXQCR_AUTO_DEQUE_EN | RXQCR_FRAME_CNT_IRQ_EN)

#define TXCR_DEFAULT_CONFIG     (TXCR_TCGICMP | TX_CTRL_UDP_CHECKSUM | \
            TXCR_TCGTP | TXCR_TCGIP | TXCR_TXFCPE | TXCR_TXPE | TXCR_TXCE)

#define RXCR1_DEFAULT_CONFIG    (RXCR1_UDP_CS_EN | RXCR1_TCP_CS_EN | \
            RXCR1_IP_CS_EN | RXCR1_MAC_ADDR_FILT_EN | RXCR1_FLOW_CTRL_EN | RXCR1_RX_ALL_EN | \
            RXCR1_RX_MULTICAST_ADDR_FILT_EN | RXCR1_RX_BROADCAST_EN | RXCR1_RX_ALL_MULTICAST | RXCR1_RX_UNICAST_EN)

#define RXCR2_DEFAULT_CONFIG    (RXCR2_RX_ICMP_CS_EN | RXCR2_RX_UDP_LITE_EN | \
            RXCR2_RX_CHK_SUM_ZERO_EN | RXCR2_RX_CTRL_BURST_LEN_FRAME)

#endif
