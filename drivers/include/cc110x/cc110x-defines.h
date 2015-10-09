/*
 * Copyright (C) 2008 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Driver internal constants for CC110x chip configuration
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef CC110X_DEFINES_H
#define CC110X_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Variable packet length PKTCTRL0 bit configuration
 *
 * If variable packet length is configured in PKTCTRL0 the
 * first byte after the synch word determines the packet length.
 */
#define VARIABLE_PKTLEN     (0x01)

/**
 * @name    Bitmasks for reading out status register values
 * @{
 */

/**
 * @brief   Bitmask (=10000000) for reading CRC_OK.
 *
 * If CRC_OK == 1: CRC for received data OK (or CRC disabled).
 * If CRC_OK == 0: CRC error in received data.
 */
#define CRC_OK              (0x80)
/**
 * @brief  Bitmask (=01111111) for reading LQI_EST.
 *
 * The Link Quality Indicator estimates how easily a received signal can be demodulated.
 */
#define LQI_EST                 (0x7F)
#define I_RSSI                  (0x00)  /**< Index 0 contains RSSI information (from optionally appended packet status bytes). */
#define I_LQI                   (0x01)  /**< Index 1 contains LQI & CRC_OK information (from optionally appended packet status bytes). */
#define MARC_STATE              (0x1F)  /**< Bitmask (=00011111) for reading MARC_STATE in MARCSTATE status register. */
#define PKTSTATUS_CS            (0x40)  /**< Bitmask (=01000000) for reading CS (Carrier Sense) in PKTSTATUS status register. */
#define PKTSTATUS_PQT_REACHED   (0x20)  /**< Bitmask (=00100000) for reading PQT_REACHED (Preamble Quality reached) in PKTSTATUS status register. */
#define PKTSTATUS_CCA           (0x10)  /**< Bitmask (=00010000) for reading CCA (clear channel assessment) in PKTSTATUS status register. */
#define PKTSTATUS_SFD           (0x08)  /**< Bitmask (=00001000) for reading SFD (Sync word found) in PKTSTATUS status register. */
#define PKTSTATUS_GDO2          (0x04)  /**< Bitmask (=00000100) for reading GDO2 (current value on GDO2 pin) in PKTSTATUS status register. */
#define PKTSTATUS_GDO1          (0x02)  /**< Bitmask (=00000010) for reading GDO1 (current value on GDO1 pin) in PKTSTATUS status register. */
#define PKTSTATUS_GDO0          (0x01)  /**< Bitmask (=00000001) for reading GDO0 (current value on GDO0 pin) in PKTSTATUS status register. */
#define TXFIFO_UNDERFLOW        (0x80)  /**< Bitmask (=10000000) for reading TXFIFO_UNDERFLOW in TXBYTES status register. */
#define BYTES_IN_TXFIFO         (0x7F)  /**< Bitmask (=01111111) for reading NUM_TXBYTES in TXBYTES status register. */
#define RXFIFO_OVERFLOW         (0x80)  /**< Bitmask (=10000000) for reading RXFIFO_OVERFLOW in RXBYTES status register. */
#define BYTES_IN_RXFIFO         (0x7F)  /**< Bitmask (=01111111) for reading NUM_RXBYTES in RXBYTES status register. */
/** @} */

/**
 * @name    Bitmasks for reading out configuration register values
 * @{
 */
#define PKT_LENGTH_CONFIG   (0x03)      /**< Bitmask (=00000011) for reading LENGTH_CONFIG in PKTCTRL0 configuration register. */
/** @} */

/**
 * @name    Definitions to support burst/single access
 * @{
 */
#define CC110X_WRITE_BURST  (0x40) /**< Offset for burst write. */
#define CC110X_READ_SINGLE  (0x80) /**< Offset for read single byte. */
#define CC110X_READ_BURST   (0xC0) /**< Offset for read burst. */
#define CC110X_NOBYTE       (0xFF) /**< No command (for reading). */
/** @} */

/**
 * @name    Configuration Registers (47x)
 * @{
 */
#define CC110X_IOCFG2       (0x00)      /**< GDO2 output pin configuration */
#define CC110X_IOCFG1       (0x01)      /**< GDO1 output pin configuration */
#define CC110X_IOCFG0       (0x02)      /**< GDO0 output pin configuration */
#define CC110X_FIFOTHR      (0x03)      /**< RX FIFO and TX FIFO thresholds */
#define CC110X_SYNC1        (0x04)      /**< Sync word, high byte */
#define CC110X_SYNC0        (0x05)      /**< Sync word, low byte */
#define CC110X_PKTLEN       (0x06)      /**< Packet length */
#define CC110X_PKTCTRL1     (0x07)      /**< Packet automation control */
#define CC110X_PKTCTRL0     (0x08)      /**< Packet automation control */
#define CC110X_ADDR         (0x09)      /**< Device address */
#define CC110X_CHANNR       (0x0A)      /**< Channel number */
#define CC110X_FSCTRL1      (0x0B)      /**< Frequency synthesizer control */
#define CC110X_FSCTRL0      (0x0C)      /**< Frequency synthesizer control */
#define CC110X_FREQ2        (0x0D)      /**< Frequency control word, high byte */
#define CC110X_FREQ1        (0x0E)      /**< Frequency control word, middle byte */
#define CC110X_FREQ0        (0x0F)      /**< Frequency control word, low byte */
#define CC110X_MDMCFG4      (0x10)      /**< Modem configuration */
#define CC110X_MDMCFG3      (0x11)      /**< Modem configuration */
#define CC110X_MDMCFG2      (0x12)      /**< Modem configuration */
#define CC110X_MDMCFG1      (0x13)      /**< Modem configuration */
#define CC110X_MDMCFG0      (0x14)      /**< Modem configuration */
#define CC110X_DEVIATN      (0x15)      /**< Modem deviation setting */
#define CC110X_MCSM2        (0x16)      /**< Main Radio Control State Machine configuration */
#define CC110X_MCSM1        (0x17)      /**< Main Radio Control State Machine configuration */
#define CC110X_MCSM0        (0x18)      /**< Main Radio Control State Machine configuration */
#define CC110X_FOCCFG       (0x19)      /**< Frequency Offset Compensation configuration */
#define CC110X_BSCFG        (0x1A)      /**< Bit Synchronization configuration */
#define CC110X_AGCCTRL2     (0x1B)      /**< AGC control */
#define CC110X_AGCCTRL1     (0x1C)      /**< AGC control */
#define CC110X_AGCCTRL0     (0x1D)      /**< AGC control */
#define CC110X_WOREVT1      (0x1E)      /**< High byte Event 0 timeout */
#define CC110X_WOREVT0      (0x1F)      /**< Low byte Event 0 timeout */
#define CC110X_WORCTRL      (0x20)      /**< Wake On Radio control */
#define CC110X_FREND1       (0x21)      /**< Front end RX configuration */
#define CC110X_FREND0       (0x22)      /**< Front end TX configuration */
#define CC110X_FSCAL3       (0x23)      /**< Frequency synthesizer calibration */
#define CC110X_FSCAL2       (0x24)      /**< Frequency synthesizer calibration */
#define CC110X_FSCAL1       (0x25)      /**< Frequency synthesizer calibration */
#define CC110X_FSCAL0       (0x26)      /**< Frequency synthesizer calibration */
#define CC110X_RCCTRL1      (0x27)      /**< RC oscillator configuration */
#define CC110X_RCCTRL0      (0x28)      /**< RC oscillator configuration */
#define CC110X_FSTEST       (0x29)      /**< Frequency synthesizer calibration control */
#define CC110X_PTEST        (0x2A)      /**< Production test */
#define CC110X_AGCTEST      (0x2B)      /**< AGC test */
#define CC110X_TEST2        (0x2C)      /**< Various test settings */
#define CC110X_TEST1        (0x2D)      /**< Various test settings */
#define CC110X_TEST0        (0x2E)      /**< Various test settings */
/** @} */

/**
 * @name    Strobe commands (14x)
 * @{
 */
#define CC110X_SRES         (0x30)      /**< Reset chip. */
/**
 * @brief   Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
 *
 * If in RX/TX: Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
 */
#define CC110X_SFSTXON      (0x31)
#define CC110X_SXOFF        (0x32)      /**< Turn off crystal oscillator. */
#define CC110X_SCAL         (0x33)      /**< Calibrate frequency synthesizer and turn it off (enables quick start). */
#define CC110X_SRX          (0x34)      /**< Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1. */
/**
 * In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
 * If in RX state and CCA is enabled: Only go to TX if channel is clear.
 */
#define CC110X_STX          (0x35)
#define CC110X_SIDLE        (0x36)      /**< Exit RX / TX, turn off frequency synthesizer and exit WOR mode if applicable. */
#define CC110X_SAFC         (0x37)      /**< Perform AFC adjustment of the frequency synthesizer */
#define CC110X_SWOR         (0x38)      /**< Start automatic RX polling sequence (Wake-on-Radio) */
#define CC110X_SPWD         (0x39)      /**< Enter power down mode when CSn goes high. */
#define CC110X_SFRX         (0x3A)      /**< Flush the RX FIFO buffer (CC110X should be in IDLE state). */
#define CC110X_SFTX         (0x3B)      /**< Flush the TX FIFO buffer (CC110X should be in IDLE state). */
#define CC110X_SWORRST      (0x3C)      /**< Reset real time clock. */
#define CC110X_SNOP         (0x3D)      /**< No operation. May be used to pad strobe commands to two bytes for simpler software. */
/** @} */

/**
 * @name    Status registers (12x)
 * @{
 */
#define CC110X_PARTNUM      (0x30)      /**< Part number of CC110X. */
#define CC110X_VERSION      (0x31)      /**< Current version number. */
#define CC110X_FREQEST      (0x32)      /**< Frequency Offset Estimate. */
#define CC110X_LQI          (0x33)      /**< Demodulator estimate for Link Quality. */
#define CC110X_RSSI         (0x34)      /**< Received signal strength indication. */
#define CC110X_MARCSTATE    (0x35)      /**< Control state machine state. */
#define CC110X_WORTIME1     (0x36)      /**< High byte of WOR timer. */
#define CC110X_WORTIME0     (0x37)      /**< Low byte of WOR timer. */
#define CC110X_PKTSTATUS    (0x38)      /**< Current GDOx status and packet status. */
#define CC110X_VCO_VC_DAC   (0x39)      /**< Current setting from PLL calibration module. */
#define CC110X_TXBYTES      (0x3A)      /**< Underflow and number of bytes in the TX FIFO. */
#define CC110X_RXBYTES      (0x3B)      /**< Overflow and number of bytes in the RX FIFO. */
/** @} */

/**
 * @name    Multi byte registers
 * @{
 */
/**
 * @brief   Register for eight user selected output power settings.
 *
 * 3-bit FREND0.PA_POWER value selects the PATABLE entry to use.
 */
#define CC110X_PATABLE      (0x3E)
#define CC110X_TXFIFO       (0x3F)      /**< TX FIFO: Write operations write to the TX FIFO (SB: +0x00; BURST: +0x40) */
#define CC110X_RXFIFO       (0x3F)      /**< RX FIFO: Read operations read from the RX FIFO (SB: +0x80; BURST: +0xC0) */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif
