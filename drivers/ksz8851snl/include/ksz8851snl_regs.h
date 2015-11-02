/*
 * Copyright (C) 2015
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
 * @brief       ksz8851snl Register Definitions
 *
 * @author      Matt Poppe <matt@poppe.me>
 */

#ifndef KSZ8851SNL_REGS_H
#define KSZ8851SNL_REGS_H


#define IO_REG_READ   (0<<14)
#define IO_REG_WRITE  (1<<14)
#define FIFO_RX_READ  (2 << 6)
#define FIFO_TX_WRITE (3 << 6)
#define KSZ8851_CMD_B0        0x400
#define KSZ8851_CMD_B1        0x800
#define KSZ8851_CMD_B2        0x1000
#define KSZ8851_CMD_B3        0x2000

#define CHIP_ID_REG         0xC0
#define CHIP_ID_8851_16     0x8870

#define KSZ8851_MAHTR0_REG 		0xA0
#define KSZ8851_MAHTR1_REG		0XA2
#define KSZ8851_MAHTR2_REG		0XA4
#define KSZ8851_MAHTR3_REG		0XA6
/* Registers I use */
#define KSZ8851_P1SR_REG      0Xf8


#	define P1SR_LINK_GOOD		(1<<5)
#define KSZ8851_INT_STATUS_REG  0x90
#define KSZ8851_INT_EN_REG      0x92

/* General Control Reg */
#define KSZ8851_P1CR_REG            0xF6
    #define P1CR_AN_ENABLE          (1<<13)
#	define P1CR_HP_MDIX				(1<<5)
#	define P1CR_FORCE_DUPLEX		(1<<5)

#define KSZ8851_GRR_REG				0x26
#	define GRR_QUEUE_RESET			(1<<1)
#	define GRR_DEV_RESET			(1<<0)

#define KSZ8851_P1MBCR_REG          0xE4
    #define P1MBCR_LLB              (1<<14)
    #define P1MBCR_FORCE_100        (1<<13)
    #define P1MBCR_AN_ENABLE        (1<<12)
    #define P1MBCR_RESTART_AN       (1<<9)
    #define P1MBCR_FORCE_DUPLEX     (1<<8)
    #define P1MBCR_HP_MDIX          (1<<5)
    #define P1MBCR_FORCE_MDIX       (1<<4)

/* SPI Clock */
#define KSZ8851_OBCR_REG        0x20
#define   BUS_CLOCK_166               0x0004    /* 166 MHz on-chip bus clock (defaul is 125MHz) */
#define   BUS_CLOCK_DIVIDEDBY_5       0x0003    /* Bus clock devided by 5 */
#define   BUS_CLOCK_DIVIDEDBY_3       0x0002    /* Bus clock devided by 3 */
#define   BUS_CLOCK_DIVIDEDBY_2       0x0001    /* Bus clock devided by 2 */
#define   BUS_CLOCK_DIVIDEDBY_1       0x0000    /* Bus clock devided by 1 */
#define   BUS_CLOCK_DIVIDED_MASK      0x0003    /* Bus clock devider mask */

/* Energy Managment */
#define KSZ8851_PMECR_REG       0xD4
#	define PMECR_NORMAL			(1<<6)

/* Interrupt Registers */
#define KSZ8851_IER_REG         0x90
#define KSZ8851_ISR_REG         0x92

#define IER_LCIE                        (1<<15)
#define IER_TXIE                        (1<<14)
#define IER_RXIE                        (1<<13)
#define IER_RX_OVERRUN_IRQ_EN			(1<<11)
#define IER_RX_PROC_STOPPED_IRQ_EN		(1<<8)
#define IER_TX_PROC_STOPPED_IRQ_EN		(1<<9)



#define ISR_LCIS                        (1<<15)
#define ISR_TXIS                        (1<<14)
#define ISR_RXIS                        (1<<13)
#define ISR_RXOIS                       (1<<11)
#define ISR_TXPSIS                      (1<<9)
#define ISR_RXPSIS                      (1<<8)
#define ISR_TXSAIS                      (1<<6)
#define ISR_RXWFDIS                     (1<<5)
#define ISR_RXMPDIS                     (1<<4)
#define ISR_LDIS                        (1<<3)
#define ISR_EDIS                        (1<<2)
#define ISR_SPIBEIS                     (1<<1)

/* Rx regusters pg 29 of the manual deistribes all Rx Registers */

#define KSZ8851_RXFCTR_REG      0x9C
#define KSZ8851_RXDTTR_REG		0x8C
#define KSZ8851_RXDBCTR_REG		0x8E

#define KSZ8851_RXCR1_REG       0x74
    #define RXCR1_FLUSH_RX_Q          			(1<<15)
    #define RXCR1_UDP_CS_EN             		(1<<14)
    #define RXCR1_TCP_CS_EN             		(1<<13)
    #define RXCR1_IP_CS_EN             			(1<<12)
    #define RXCR1_MAC_ADDR_FILT_EN             	(1<<11)
    #define RXCR1_FLOW_CTRL_EN         			(1<<10)
    #define RXCR1_RX_ERR_FRAME_EN           	(1<<9)
    #define RXCR1_RX_MULTICAST_ADDR_FILT_EN     (1<<8)
    #define RXCR1_RX_BROADCAST_EN              	(1<<7)
    #define RXCR1_RX_ALL_MULTICAST              (1<<6)
    #define RXCR1_RX_UNICAST_EN             	(1<<5)
    #define RXCR1_RX_ALL_EN              		(1<<4)
    #define RXCR1_RX_INVERSE_FILTER_EN          (1<<1)
    #define RXCR1_RX_EN              			(1<<0)
    #define RXCR1_RXE              				(1<<0)
    #define RXCR1_RXAE             				(1<<4)

#define KSZ8851_RXCR2_REG       0x76
    #define RXCR2_RX_ERR_FRAME_EN           (1<<9)
    #define RXCR2_RX_SRC_ADDR_FILT_EN              (1<<0)
    #define RXCR2_RX_ICMP_CS_EN              (1<<1)
    #define RXCR2_RX_UDP_LITE_EN              (1<<2)
    #define RXCR2_RX_CHK_SUM_ZERO_EN             (1<<3)
    #define RXCR2_RX_FRAG_PASS_EN              (1<<4)
    #define RXCR2_RX_CTRL_BURST_LEN_4         0x0000
    #define RXCR2_RX_CTRL_BURST_LEN_8         (1<<5)
    #define RXCR2_RX_CTRL_BURST_LEN_16        (1<<6)
    #define RXCR2_RX_CTRL_BURST_LEN_32        (1<<6|1<<5)
    #define RXCR2_RX_CTRL_BURST_LEN_FRAME     (1<<7)


#define KSZ8851_RXFHSR_REG      0x7C
#	define RXFHSR_RX_VALID_FRAME		(1<<15)
#	define RXFHSR_ICMP_ERR				(1<<13)
#	define RXFHSR_IP_ERR				(1<<12)
#	define RXFHSR_TCP_ERR				(1<<11)
#	define RXFHSR_UDP_ERR				(1<<10)
#	define RXFHSR_BROADCAST				(1<<7)
#	define RXFHSR_MULTICAST				(1<<6)
#	define RXFHSR_UNICAST				(1<<5)
#	define RXFHSR_PHY_ERR				(1<<4)
#	define RXFHSR_FRAME_ETHER			(1<<3)
#	define RXFHSR_FRAME_LEN_ERR			(1<<2)
#	define RXFHSR_RUNT_ERR				(1<<1)
#	define RXFHSR_BAD_CRC_ERR			(1<<0)
#	define RXFHSR_ANY_ERR			\
								(RXFHSR_BAD_CRC_ERR \
								| RXFHSR_RUNT_ERR \
								| RXFHSR_FRAME_LEN_ERR \
								| RXFHSR_PHY_ERR \
								| RXFHSR_UDP_ERR \
								| RXFHSR_TCP_ERR \
								| RXFHSR_IP_ERR \
								| RXFHSR_ICMP_ERR)


#define KSZ8851_RXFHBCR_REG     0x7E

#define KSZ8851_RXQCR_REG       0x82
#	define 	RXQCR_DURATION_TIMER_EN       (1<<7) /* RXDBCTS */
    #define RXQCR_FRAME_CNT_IRQ_EN        (1<<5)
    #define RXQCR_AUTO_DEQUE_EN           (1<<4)
	#define RXQCR_TWO_BYTE_OFFSET	      (1<<9)
#   define  RXQCR_QUEUE_ACCESS_EN         (1<<3)

#define 	RXQCR_RELEASE_RX_ERR	(1<<0) /* RRXERF */


#define KSZ8851_RXFDPR_REG      0x86
    #define RXFDPR_RXFPAI           (1<<14)
#	define RXFDPR_RESET_PTR_AUTO_INC	0x4000


/* Tx registers pg   of the manual discribes all the Tx Registers */

#define KSZ8851_TXCR_REG        0x70
    #define TXCR_TCGICMP            (1<<8)
	#define TX_CTRL_UDP_CHECKSUM	(1<<7)
    #define TXCR_TCGTP              (1<<6)
    #define TXCR_TCGIP              (1<<5)
    #define TXCR_FTXQ               (1<<4)
    #define TXCR_TXFCPE             (1<<3)
    #define TXCR_TXPE               (1<<2)
    #define TXCR_TXCE               (1<<1)
    #define TXCR_TXE                (1<<0)
#define KSZ8851_TXMIR_REG       0x78
#define KSZ8851_TXQCR_REG       0x80
    #define TXQCR_TX_CMD        (1<<0)

#define KSZ8851_TXFDPR_REG      0x84
    #define TXFDPR_TXFPAI           (1<<14)
/* rename following */

/* */

/* MAC */
#define KSZ8851_MARL_REG        0x10
#define KSZ8851_MARM_REG        0x12
#define KSZ8851_MARH_REG        0x14

#endif /* KSZ8851SNL_REGS_H */
