/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc2420
 * @{
 *
 * @file
 * @brief       Register and command definitions for CC2420
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef CC2420_REGISTERS_H
#define CC2420_REGISTERS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Internal device option flags
 * @{
 */
#define CC2420_OPT_AUTOACK          (0x0001)    /**< auto ACKs active */
#define CC2420_OPT_CSMA             (0x0002)    /**< CSMA active */
#define CC2420_OPT_PROMISCUOUS      (0x0004)    /**< promiscuous mode active */
#define CC2420_OPT_PRELOADING       (0x0008)    /**< preloading enabled */
/** @} */

/**
 * @name    Possible device state change commands
 * @{
 */
enum {
    CC2420_GOTO_PD,         /**< power down */
    CC2420_GOTO_IDLE,       /**< idle */
    CC2420_GOTO_RX,         /**< receive state */
    CC2420_GOTO_TXON,       /**< transmit packet without CCA */
    CC2420_GOTO_TXONCCA     /**< transmit packet using CCA */
};

/**
 * @name    (Selected) device states
 */
enum {
    CC2420_STATE_PD          = 0,   /**< power down */
    CC2420_STATE_IDLE        = 1,   /**< idle state */
    CC2420_STATE_TX_PRE      = 34,  /**< transmitting preamble */
    CC2420_STATE_RX_SEARCH   = 6,   /**< receive SFD search */
    CC2420_STATE_RX_OVERFLOW = 17   /**< receive buffer overflow */
};

/**
 * @name    CC2420 SPI commands
 * @{
 */
#define CC2420_REG_WRITE            (0x00)  /**< read register value */
#define CC2420_REG_READ             (0x40)  /**< write register value */
#define CC2420_RAM                  (0x80)  /**< access the internal RAM */
#define CC2420_RAM_WRITE            (0x00)  /**< write to RAM */
#define CC2420_RAM_READ             (0x20)  /**< read from RAM */
#define CC2420_FIFO_READ            (CC2420_REG_RXFIFO | CC2420_REG_READ)
#define CC2420_FIFO_WRITE           (CC2420_REG_TXFIFO | CC2420_REG_WRITE)
/** @} */

/**
 * @name    CC2420 strobe commands
 * @see     Datasheet section 37, pages 61--62
 * @{
 */
#define CC2420_STROBE_NOP           (0x00)  /**< no operation */
#define CC2420_STROBE_XOSCON        (0x01)  /**< turn transceiver on */
#define CC2420_STROBE_TXCAL         (0x02)  /**< calibrate TX freq and wait */
#define CC2420_STROBE_RXON          (0x03)  /**< switch to RX mode */
#define CC2420_STROBE_TXON          (0x04)  /**< switch to TX mode */
#define CC2420_STROBE_TXONCCA       (0x05)  /**< switch to TX after CCA*/
#define CC2420_STROBE_RFOFF         (0x06)  /**< switch to IDLE mode */
#define CC2420_STROBE_XOSCOFF       (0x07)  /**< power down */
#define CC2420_STROBE_FLUSHRX       (0x08)  /**< flush RX FIFO */
#define CC2420_STROBE_FLUSHTX       (0x09)  /**< flush TX FIFO */
#define CC2420_STROBE_ACK           (0x0A)  /**< send ACK with pending cleared */
#define CC2420_STROBE_ACKPEND       (0x0B)  /**< send ACK with pending set */
#define CC2420_STROBE_RXDEC         (0x0C)  /**< start RX FIFO decrypt/verify */
#define CC2420_STROBE_TXENC         (0x0D)  /**< start TX FIFO encrypt/auth */
#define CC2420_STROBE_AES           (0x0E)  /**< start AES encryption */
/** @} */

/**
 * @name    CC2420 configuration registers
 * @see     Datasheet section 37, pages 61 to 80
 * @{
 */
#define CC2420_REG_MAIN             (0x10)  /**< main control */
#define CC2420_REG_MDMCTRL0         (0x11)  /**< modem control 0 */
#define CC2420_REG_MDMCTRL1         (0x12)  /**< modem control 1 */
#define CC2420_REG_RSSI             (0x13)  /**< RSSI and CCA control */
#define CC2420_REG_SYNCWORD         (0x14)  /**< synchronization word control */
#define CC2420_REG_TXCTRL           (0x15)  /**< transmit control */
#define CC2420_REG_RXCTRL0          (0x16)  /**< receive control 0 */
#define CC2420_REG_RXCTRL1          (0x17)  /**< receive control 1 */
#define CC2420_REG_FSCTRL           (0x18)  /**< freq synthesizer control */
#define CC2420_REG_SECCTRL0         (0x19)  /**< security control 0 */
#define CC2420_REG_SECCTRL1         (0x1A)  /**< security control 1 */
#define CC2420_REG_BATTMON          (0x1B)  /**< battery monitor control */
#define CC2420_REG_IOCFG0           (0x1C)  /**< I/O control 0 */
#define CC2420_REG_IOCFG1           (0x1D)  /**< I/O control 1 */
#define CC2420_REG_MANFIDL          (0x1e)  /**< manufacturer ID low */
#define CC2420_REG_MANFIDH          (0x1F)  /**< manufacturer ID high */
#define CC2420_REG_FSMTC            (0x20)  /**< FSM timer constants */
#define CC2420_REG_MANAND           (0x21)  /**< manual signal AND override */
#define CC2420_REG_MANOR            (0x22)  /**< manual signal OR override */
#define CC2420_REG_AGCCTRL          (0x23)  /**< AGC control */
#define CC2420_REG_AGCTST0          (0x24)  /**< AGC test 0 */
#define CC2420_REG_AGCTST1          (0x25)  /**< AGC test 1 */
#define CC2420_REG_AGCTST2          (0x26)  /**< AGC test 2 */
#define CC2420_REG_FSTST0           (0x27)  /**< freq synthesizer test 0 */
#define CC2420_REG_FSTST1           (0x28)  /**< freq synthesizer test 1 */
#define CC2420_REG_FSTST2           (0x29)  /**< freq synthesizer test 2 */
#define CC2420_REG_FSTST3           (0x2A)  /**< freq synthesizer test 3 */
#define CC2420_REG_RXBPFTST         (0x2B)  /**< RX bandpass filter test */
#define CC2420_REG_FSMSTATE         (0x2C)  /**< FSM status */
#define CC2420_REG_ADCTST           (0x2D)  /**< ADC test */
#define CC2420_REG_DACTST           (0x2E)  /**< DAC test */
#define CC2420_REG_TOPTST           (0x2F)  /**< top level test */
#define CC2420_REG_TXFIFO           (0x3E)  /**< TX FIFO byte */
#define CC2420_REG_RXFIFO           (0x3F)  /**< RX FIFO byte */
/** @} */

/**
 * @name    CC2420 section address in RAM
 * @see     Datasheet section 13.5 page 31.
 * @{
 */
#define CC2420_RAM_TXFIFO           (0x0000)
#define CC2420_RAM_RXFIFO           (0x0080)
#define CC2420_RAM_KEY0             (0x0100)
#define CC2420_RAM_RXNONCE          (0x0110)
#define CC2420_RAM_RXCTR            (0x0110)
#define CC2420_RAM_SABUF            (0x0120)
#define CC2420_RAM_KEY1             (0x0130)
#define CC2420_RAM_TXNONCE          (0x0140)
#define CC2420_RAM_TXCTR            (0x0140)
#define CC2420_RAM_CBCSTATE         (0x0150)
#define CC2420_RAM_IEEEADR          (0x0160)
#define CC2420_RAM_PANID            (0x0168)
#define CC2420_RAM_SHORTADR         (0x016A)
/** @} */

/**
 * @name    Status byte bit fields
 * @see     Datasheet section 13.3, page 29
 * @{
 */
#define CC2420_STATUS_XOSC_STABLE   (0x40)
#define CC2420_STATUS_TX_UNDERFLOW  (0x20)
#define CC2420_STATUS_ENC_BUSY      (0x10)
#define CC2420_STATUS_TX_ACTIVE     (0x08)
#define CC2420_STATUS_PLL_LOCK      (0x04)
#define CC2420_STATUS_RSSI_VALID    (0x02)
/** @} */

/**
 * @name    Modem control 0 register bitfields
 * @{
 */
#define CC2420_MDMCTRL0_RES_FRM     (0x2000
#define CC2420_MDMCTRL0_ADR_DECODE  (0x0800)
#define CC2420_MDMCTRL0_PAN_COORD   (0x1000)
#define CC2420_MDMCTRL0_AUTOCRC     (0x0020)
#define CC2420_MDMCTRL0_AUTOACK     (0x0010)
#define CC2420_MDMCTRL0_PREAMBLE_M  (0x000f)
#define CC2420_MDMCTRL0_PREAMBLE_3B (0x0002)
/** @} */

/**
 * @brief  CRC/Correlation bit masks
 * @{
 */
#define CC2420_CRCCOR_CRC_MASK      (0x80)
#define CC2420_CRCCOR_COR_MASK      (0x7F)
/** @} */

/**
 * @name    Transmit control register bitfields
 * @{
 */
#define CC2420_TXCTRL_PA_MASK       (0x001f)
/** @} */

/**
 * @name    Receive control register 1 bitfields
 * @{
 */
#define CC2420_RXCTRL1_RXBPF_LOCUR  (0x2000)
/** @} */

/**
 * @name    Frequency synthesizer control and status register bitfields
 * @{
 */
#define CC2420_FSCTRL_LOCK_THR_MASK (0xc000)
#define CC2420_FSCTRL_CAL_DONE      (0x2000)
#define CC2420_FSCTRL_CAL_RUNNING   (0x1000)
#define CC2420_FSCTRL_LOCK_LENGTH   (0x0800)
#define CC2420_FSCTRL_LOCK_STATUS   (0x0400)
#define CC2420_FSCTRL_FREQ_MASK     (0x03ff)
/** @} */

/**
 * @name    Security control register 0 bitfields
 * @{
 */
#define CC2420_SECCTRL0_RXFIFO_PROT (0x0200)
/** @} */

/**
 * @name    Manufacturer ID low register value
 * @{
 */
#define CC2420_MANFIDL_VAL          (0x233d)
/** @} */

/**
 * @name   Manufacturer ID high register value
 * @{
 */
#define CC2420_MANFIDH_VAL          (0x3000)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CC2420_REGISTERS_H */
/** @} */
