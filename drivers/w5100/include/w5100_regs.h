/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_w5100
 * @{
 *
 * @file
 * @brief       Register definitions for W5100 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI commands
 * @{
 */
#define CMD_READ            (0x0f)
#define CMD_WRITE           (0xf0)
/** @} */

/**
 * @name    Common registers
 * @{
 */
#define REG_MODE            (0x0000)    /**< mode */
#define REG_GAR0            (0x0001)    /**< gateway address 0 */
#define REG_GAR1            (0x0002)    /**< gateway address 1 */
#define REG_GAR2            (0x0003)    /**< gateway address 2 */
#define REG_GAR3            (0x0004)    /**< gateway address 3 */
#define REG_SUB0            (0x0005)    /**< subnet mask 0 */
#define REG_SUB1            (0x0006)    /**< subnet mask 1 */
#define REG_SUB2            (0x0007)    /**< subnet mask 2 */
#define REG_SUB3            (0x0008)    /**< subnet mask 3 */
#define REG_SHAR0           (0x0009)    /**< source hardware address 0 */
#define REG_SHAR1           (0x000a)    /**< source hardware address 1 */
#define REG_SHAR2           (0x000b)    /**< source hardware address 2 */
#define REG_SHAR3           (0x000c)    /**< source hardware address 3 */
#define REG_SHAR4           (0x000d)    /**< source hardware address 4 */
#define REG_SHAR5           (0x000e)    /**< source hardware address 5 */
#define REG_SIPR0           (0x000f)    /**< source IP address 0 */
#define REG_SIPR1           (0x0010)    /**< source IP address 1 */
#define REG_SIPR2           (0x0011)    /**< source IP address 2 */
#define REG_SIPR3           (0x0012)    /**< source IP address 3 */
#define REG_IR              (0x0015)    /**< interrupt flags */
#define REG_IMR             (0x0016)    /**< interrupt masks */
#define REG_RTR0            (0x0017)    /**< retry time 0 */
#define REG_RTR1            (0x0018)    /**< retry time 1 */
#define REG_RCR             (0x0019)    /**< retry count */
#define REG_RMSR            (0x001a)    /**< RX memory size */
#define REG_TMSR            (0x001b)    /**< TX memory size */
#define REG_PATR0           (0x001c)    /**< PPPoE auth type 0 */
#define REG_PATR1           (0x001d)    /**< PPPoE auth type 1 */
#define REG_PTIMER          (0x0028)    /**< PPP LCP request timer */
#define REG_PMAGIC          (0x0029)    /**< PPP LCP magic number */
#define REG_UIPR0           (0x002a)    /**< unreachable IP address 0 */
#define REG_UIPR1           (0x002b)    /**< unreachable IP address 1 */
#define REG_UIPR2           (0x002c)    /**< unreachable IP address 2 */
#define REG_UIPR3           (0x002d)    /**< unreachable IP address 3 */
#define REG_UPORT0          (0x00fe)    /**< unreachable port 0 */
#define REG_UPORT1          (0x002f)    /**< unreachable port 1 */
/** @} */

/**
 * @name    Socket 0 registers
 *
 * As we are using the device in MACRAW mode, we only need socket 0.
 * @{
 */
#define S0_MR               (0x0400)    /**< mode */
#define S0_CR               (0x0401)    /**< control */
#define S0_IR               (0x0402)    /**< interrupt flags */
#define S0_SR               (0x0403)    /**< state */
#define S0_DHAR0            (0x0406)    /**< destination hardware address 0 */
#define S0_DHAR1            (0x0407)    /**< destination hardware address 1 */
#define S0_DHAR2            (0x0408)    /**< destination hardware address 2 */
#define S0_DHAR3            (0x0409)    /**< destination hardware address 3 */
#define S0_DHAR4            (0x040a)    /**< destination hardware address 4 */
#define S0_DHAR5            (0x040b)    /**< destination hardware address 5 */
#define S0_DIPR0            (0x040c)    /**< destination IP address 0 */
#define S0_DIPR1            (0x040d)    /**< destination IP address 1 */
#define S0_DIPR2            (0x040e)    /**< destination IP address 2 */
#define S0_DIPR3            (0x040f)    /**< destination IP address 3 */
#define S0_DPORT0           (0x0410)    /**< destination port 0 */
#define S0_DPORT1           (0x0411)    /**< destination port 1 */
#define S0_MSSR0            (0x0412)    /**< maximum segment size 0 */
#define S0_MSSR1            (0x0413)    /**< maximum segment size 1 */
#define S0_PROTO            (0x0414)    /**< protocol in IP raw mode */
#define S0_TOS              (0x0415)    /**< IP TOS */
#define S0_TTL              (0x0416)    /**< IP TTL */
#define S0_TX_FSR0          (0x0420)    /**< TX free size 0 */
#define S0_TX_FSR1          (0x0421)    /**< TX free size 1 */
#define S0_TX_RD0           (0x0422)    /**< TX read pointer 0 */
#define S0_TX_RD1           (0x0423)    /**< TX read pointer 1 */
#define S0_TX_WR0           (0x0424)    /**< TX write pointer 0 */
#define S0_TX_WR1           (0x0425)    /**< TX write pointer 1 */
#define S0_RX_RSR0          (0x0426)    /**< RX receive size 0 */
#define S0_RX_RSR1          (0x0427)    /**< RX receive size 1 */
#define S0_RX_RD0           (0x0428)    /**< RX read pointer 0 */
#define S0_RX_RD1           (0x0429)    /**< RX read pointer 1 */
/** @} */

/**
 * @name    Some selected bitfield definitions
 * @{
 */
#define MODE_RESET          (0x80)      /**< device mode: reset */

#define RMSR_8KB_TO_S0      (0x03)      /**< receive memory size: 8kib */
#define TMSR_8KB_TO_S0      (0x03)      /**< transmit memory size: 8kib */

#define IMR_S0_INT          (0x01)      /**< global socket 0 interrupt mask */

#define MR_UDP              (0x02)      /**< socket mode: UDP */
#define MR_MACRAW           (0x04)      /**< socket mode: raw Ethernet */

#define CR_OPEN             (0x01)      /**< socket command: open */
#define CR_CLOSE            (0x10)      /**< socket command: close */
#define CR_SEND_MAC         (0x21)      /**< socket command: send raw */
#define CR_RECV             (0x40)      /**< socket command: receive new data */

#define IR_SEND_OK          (0x10)      /**< socket interrupt: send ok */
#define IR_RECV             (0x04)      /**< socket interrupt: data received */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
