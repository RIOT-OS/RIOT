/*
 * Copyright (C) 2023 Stefan Schmidt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_w5500
 * @{
 *
 * @file
 * @brief       Register definitions for W5500 devices
 *
 * @author      Stefan Schmidt <stemschmidt@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* The W5500 is accessed by sending a 16 Bit address first, then a 8 bit control byte
   which determines to which register (common or one of the 8 Sockets) this address shall be
   applied and finally the data.

   In order to simplify the functions to read and write to the W5500 via SPI the defined register
   addresses contain the control byte in the upper 5 bits and the actual register address in the
   lower 13 bits:
   0b00000 -> common register     (0x0xxx)
   0b00001 -> Socket 0 register   (0x08xx)

   The RX and TX buffers are accessed via separate functions in order to be able to access the full
   16 kB buffers.
 */

/**
 * @name    Common registers (with BSB 00000 in the upper 5 bits).
 * @{
 */
#define REG_MODE            (0x0000)    /**< Mode. */
#define REG_GAR0            (0x0001)    /**< Gateway address 0. */
#define REG_GAR1            (0x0002)    /**< Gateway address 1. */
#define REG_GAR2            (0x0003)    /**< Gateway address 2. */
#define REG_GAR3            (0x0004)    /**< Gateway address 3. */
#define REG_SUBR0           (0x0005)    /**< Subnet mask 0. */
#define REG_SUBR1           (0x0006)    /**< Subnet mask 1. */
#define REG_SUBR2           (0x0007)    /**< Subnet mask 2. */
#define REG_SUBR3           (0x0008)    /**< Subnet mask 3. */
#define REG_SHAR0           (0x0009)    /**< Source hardware address 0. */
#define REG_SHAR1           (0x000a)    /**< Source hardware address 1. */
#define REG_SHAR2           (0x000b)    /**< Source hardware address 2. */
#define REG_SHAR3           (0x000c)    /**< Source hardware address 3. */
#define REG_SHAR4           (0x000d)    /**< Source hardware address 4. */
#define REG_SHAR5           (0x000e)    /**< Source hardware address 5. */
#define REG_SIPR0           (0x000f)    /**< Source IP address 0. */
#define REG_SIPR1           (0x0010)    /**< Source IP address 1. */
#define REG_SIPR2           (0x0011)    /**< Source IP address 2. */
#define REG_SIPR3           (0x0012)    /**< Source IP address 3. */
#define REG_INTLEVEL0       (0x0013)    /**< Interrupt low level timer 0. */
#define REG_INTLEVEL1       (0x0014)    /**< Interrupt low level timer 1. */
#define REG_IR              (0x0015)    /**< Interrupt flags. */
#define REG_IMR             (0x0016)    /**< Interrupt masks. */
#define REG_SIR             (0x0017)    /**< Socket interrupt. */
#define REG_SIMR            (0x0018)    /**< Socket interrupt mask. */
#define REG_RTR0            (0x0019)    /**< Retry time 0. */
#define REG_RTR1            (0x001a)    /**< Retry time 1. */
#define REG_RCR             (0x001b)    /**< Retry count. */
#define REG_PTIMER          (0x001c)    /**< Ppp lcp request timer. */
#define REG_PMAGIC          (0x001d)    /**< Ppp lcp magic number. */
#define REG_PHAR0           (0x001e)    /**< Ppp destination mac address 0. */
#define REG_PHAR1           (0x001f)    /**< Ppp destination mac address 1. */
#define REG_PHAR2           (0x0020)    /**< Ppp destination mac address 2. */
#define REG_PHAR3           (0x0021)    /**< Ppp destination mac address 3. */
#define REG_PHAR4           (0x0022)    /**< Ppp destination mac address 4. */
#define REG_PHAR5           (0x0023)    /**< Ppp destination mac address 5. */
#define REG_PSID0           (0x0024)    /**< Ppp session identification 0. */
#define REG_PSID1           (0x0025)    /**< Ppp session identification 1. */
#define REG_PMRU0           (0x0026)    /**< Ppp maximum segment size 0. */
#define REG_PMRU1           (0x0027)    /**< Ppp maximum segment size 1. */
#define REG_UIPR0           (0x0028)    /**< Unreachable IP address 0. */
#define REG_UIPR1           (0x0029)    /**< Unreachable IP address 1. */
#define REG_UIPR2           (0x002a)    /**< Unreachable IP address 2. */
#define REG_UIPR3           (0x002b)    /**< Unreachable IP address 3. */
#define REG_UPORT0          (0x002c)    /**< Unreachable port 0. */
#define REG_UPORT1          (0x002d)    /**< Unreachable port 1. */
#define REG_PHYCFGR         (0x002e)    /**< Phy configuration. */
#define REG_VERSIONR        (0x0039)    /**< Chip version. */
/** @} */

/**
 * @name    Socket 0 register block (with BSB 00001 in the upper 5 bits).
 * @{
 */
#define REG_S0_MR               (0x0800)    /**< Socket 0 mode */
#define REG_S0_CR               (0x0801)    /**< Socket 0 command */
#define REG_S0_IR               (0x0802)    /**< Socket 0 interrupt */
#define REG_S0_SR               (0x0803)    /**< Socket 0 status */
#define REG_S0_PORT0            (0x0804)    /**< Socket 0 Source port 0 */
#define REG_S0_PORT1            (0x0805)    /**< Socket 0 Source port 1 */
#define REG_S0_DHAR0            (0x0806)    /**< Socket 0 destination hardware address 0 */
#define REG_S0_DHAR1            (0x0807)    /**< Socket 0 destination hardware address 1 */
#define REG_S0_DHAR2            (0x0808)    /**< Socket 0 destination hardware address 2 */
#define REG_S0_DHAR3            (0x0809)    /**< Socket 0 destination hardware address 3 */
#define REG_S0_DHAR4            (0x080a)    /**< Socket 0 destination hardware address 4 */
#define REG_S0_DHAR5            (0x080b)    /**< Socket 0 destination hardware address 5 */
#define REG_S0_DIPR0            (0x080c)    /**< Socket 0 destination IP address 0 */
#define REG_S0_DIPR1            (0x080d)    /**< Socket 0 destination IP address 1 */
#define REG_S0_DIPR2            (0x080e)    /**< Socket 0 destination IP address 2 */
#define REG_S0_DIPR3            (0x080f)    /**< Socket 0 destination IP address 3 */
#define REG_S0_DPORT0           (0x0810)    /**< Socket 0 destination port 0 */
#define REG_S0_DPORT1           (0x0811)    /**< Socket 0 destination port 1 */
#define REG_S0_MSSR0            (0x0812)    /**< Socket 0 maximum segment size 0 */
#define REG_S0_MSSR1            (0x0813)    /**< Socket 0 maximum segment size 1 */
#define REG_S0_TOS              (0x0815)    /**< Socket 0 IP TOS */
#define REG_S0_TTL              (0x0816)    /**< Socket 0 IP TTL */
#define REG_S0_RXBUF_SIZE       (0x081e)    /**< Socket 0 receive buffer size */
#define REG_S0_TXBUF_SIZE       (0x081f)    /**< Socket 0 transmit buffer size */
#define REG_S0_TX_FSR0          (0x0820)    /**< Socket 0 tx free size 0 */
#define REG_S0_TX_FSR1          (0x0821)    /**< Socket 0 tx free size 1 */
#define REG_S0_TX_RD0           (0x0822)    /**< Socket 0 tx read pointer 0 */
#define REG_S0_TX_RD1           (0x0823)    /**< Socket 0 tx read pointer 1 */
#define REG_S0_TX_WR0           (0x0824)    /**< Socket 0 tx write pointer 0 */
#define REG_S0_TX_WR1           (0x0825)    /**< Socket 0 tx write pointer 1 */
#define REG_S0_RX_RSR0          (0x0826)    /**< Socket 0 rx received size 0 */
#define REG_S0_RX_RSR1          (0x0827)    /**< Socket 0 rx received size 1 */
#define REG_S0_RX_RD0           (0x0828)    /**< Socket 0 rx read pointer 0 */
#define REG_S0_RX_RD1           (0x0829)    /**< Socket 0 rx read pointer 1 */
#define REG_S0_RX_WR0           (0x082a)    /**< Socket 0 rx write pointer 0 */
#define REG_S0_RX_WR1           (0x082b)    /**< Socket 0 rx write pointer 1 */
#define REG_S0_IMR              (0x082c)    /**< Socket 0 interrupt mask */
#define REG_S0_FRAG0            (0x082d)    /**< Socket 0 fragment offset in IP header 0 */
#define REG_S0_FRAG1            (0x082e)    /**< Socket 0 fragment offset in IP header 1 */
#define REG_S0_KPALVTR          (0x082f)    /**< Socket 0 keep alive timer */
/** @} */

#define Sn_RXBUF_SIZE_BASE      (0x001E)    /**< Register to configure a sockets RX buffer size */
#define Sn_TXBUF_SIZE_BASE      (0x001F)    /**< Register to configure a sockets TX buffer size */

/**
 * @name    Some selected bitfield definitions.
 * @{
 */
/* Common definitions. */
#define MODE_RESET              (0x80)      /**< Device mode: reset. */
#define PHY_LINK_UP             (0x01)      /**< Link up indication. */
#define IMR_S0_INT              (0x01)      /**< Global Socket 0 interrupt mask. */
#define SPI_CONF                SPI_MODE_0  /**< Configure SPI MODE 0 */
#define CHIP_VERSION            (0x04)      /**< Chip version we expect to read from the device */

#define ENABLE_MAC_FILTER       (0x80)      /**< Enable hardware MAC filter for raw mode */
#define ENABLE_BROADCAST_FILTER (0x40)      /**< Enable Broadcast blocking */
#define ENABLE_MULTICAST_FILTER (0x20)      /**< Enable Multicast blocking */
#define MR_MACRAW               (0x04)      /**< Socket mode: raw Ethernet */

#define CR_OPEN                 (0x01)      /**< Socket command: open */
#define CR_SEND                 (0x20)      /**< Socket command: send */
#define CR_RECV                 (0x40)      /**< Socket command: receive new data */

#define IR_RECV                 (0x04)      /**< Socket interrupt: data received */
#define IR_SEND_OK              (0x10)      /**< Socket interrupt: send ok */

#define CMD_READ                (0x00)      /**< Define for the read command */
#define CMD_WRITE               (0x04)      /**< Define for the write command */
#define SOCKET0_RX_BUFFER       (0x18)      /**< BSB for Socket 0 Receive Buffer. */
#define SOCKET0_TX_BUFFER       (0x10)      /**< BSB for Socket 0 Transmit Buffer. */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
