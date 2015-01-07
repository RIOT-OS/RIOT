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

#ifndef CC110X_INTERNAL_H
#define CC110X_INTERNAL_H

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
#define LQI_EST             (0x7F)
#define I_RSSI              (0x00)      ///< Index 0 contains RSSI information (from optionally appended packet status bytes).
#define I_LQI               (0x01)      ///< Index 1 contains LQI & CRC_OK information (from optionally appended packet status bytes).
#define MARC_STATE          (0x1F)      ///< Bitmask (=00011111) for reading MARC_STATE in MARCSTATE status register.
#define CS                  (0x40)      ///< Bitmask (=01000000) for reading CS (Carrier Sense) in PKTSTATUS status register.
#define PQT_REACHED         (0x20)      ///< Bitmask (=00100000) for reading PQT_REACHED (Preamble Quality reached) in PKTSTATUS status register.
#define CCA                 (0x10)      ///< Bitmask (=00010000) for reading CCA (clear channel assessment) in PKTSTATUS status register.
#define SFD                 (0x08)      ///< Bitmask (=00001000) for reading SFD (Sync word found) in PKTSTATUS status register.
#define GDO2                (0x04)      ///< Bitmask (=00000100) for reading GDO2 (current value on GDO2 pin) in PKTSTATUS status register.
#define GDO1                (0x02)      ///< Bitmask (=00000010) for reading GDO1 (current value on GDO1 pin) in PKTSTATUS status register.
#define GDO0                (0x01)      ///< Bitmask (=00000001) for reading GDO0 (current value on GDO0 pin) in PKTSTATUS status register.
#define TXFIFO_UNDERFLOW    (0x80)      ///< Bitmask (=10000000) for reading TXFIFO_UNDERFLOW in TXBYTES status register.
#define BYTES_IN_TXFIFO     (0x7F)      ///< Bitmask (=01111111) for reading NUM_TXBYTES in TXBYTES status register.
#define RXFIFO_OVERFLOW     (0xBF)      ///< Bitmask (=10000000) for reading RXFIFO_OVERFLOW in RXBYTES status register.
#define BYTES_IN_RXFIFO     (0xFF)      ///< Bitmask (=01111111) for reading NUM_RXBYTES in RXBYTES status register.
/** @} */

/**
 * @name    Bitmasks for reading out configuration register values
 * @{
 */
#define PKT_LENGTH_CONFIG   (0x03)      ///< Bitmask (=00000011) for reading LENGTH_CONFIG in PKTCTRL0 configuration register.
/** @} */

/**
 * @name    Definitions to support burst/single access
 * @{
 */
#define CC1100_WRITE_BURST  (0x40) ///< Offset for burst write.
#define CC1100_READ_SINGLE  (0x80) ///< Offset for read single byte.
#define CC1100_READ_BURST   (0xC0) ///< Offset for read burst.
#define CC1100_NOBYTE       (0xFF) ///< No command (for reading).
/** @} */

/**
 * @name    Configuration Registers (47x)
 * @{
 */
#define CC1100_IOCFG2       (0x00)      ///< GDO2 output pin configuration
#define CC1100_IOCFG1       (0x01)      ///< GDO1 output pin configuration
#define CC1100_IOCFG0       (0x02)      ///< GDO0 output pin configuration
#define CC1100_FIFOTHR      (0x03)      ///< RX FIFO and TX FIFO thresholds
#define CC1100_SYNC1        (0x04)      ///< Sync word, high byte
#define CC1100_SYNC0        (0x05)      ///< Sync word, low byte
#define CC1100_PKTLEN       (0x06)      ///< Packet length
#define CC1100_PKTCTRL1     (0x07)      ///< Packet automation control
#define CC1100_PKTCTRL0     (0x08)      ///< Packet automation control
#define CC1100_ADDR         (0x09)      ///< Device address
#define CC1100_CHANNR       (0x0A)      ///< Channel number
#define CC1100_FSCTRL1      (0x0B)      ///< Frequency synthesizer control
#define CC1100_FSCTRL0      (0x0C)      ///< Frequency synthesizer control
#define CC1100_FREQ2        (0x0D)      ///< Frequency control word, high byte
#define CC1100_FREQ1        (0x0E)      ///< Frequency control word, middle byte
#define CC1100_FREQ0        (0x0F)      ///< Frequency control word, low byte
#define CC1100_MDMCFG4      (0x10)      ///< Modem configuration
#define CC1100_MDMCFG3      (0x11)      ///< Modem configuration
#define CC1100_MDMCFG2      (0x12)      ///< Modem configuration
#define CC1100_MDMCFG1      (0x13)      ///< Modem configuration
#define CC1100_MDMCFG0      (0x14)      ///< Modem configuration
#define CC1100_DEVIATN      (0x15)      ///< Modem deviation setting
#define CC1100_MCSM2        (0x16)      ///< Main Radio Control State Machine configuration
#define CC1100_MCSM1        (0x17)      ///< Main Radio Control State Machine configuration
#define CC1100_MCSM0        (0x18)      ///< Main Radio Control State Machine configuration
#define CC1100_FOCCFG       (0x19)      ///< Frequency Offset Compensation configuration
#define CC1100_BSCFG        (0x1A)      ///< Bit Synchronization configuration
#define CC1100_AGCCTRL2     (0x1B)      ///< AGC control
#define CC1100_AGCCTRL1     (0x1C)      ///< AGC control
#define CC1100_AGCCTRL0     (0x1D)      ///< AGC control
#define CC1100_WOREVT1      (0x1E)      ///< High byte Event 0 timeout
#define CC1100_WOREVT0      (0x1F)      ///< Low byte Event 0 timeout
#define CC1100_WORCTRL      (0x20)      ///< Wake On Radio control
#define CC1100_FREND1       (0x21)      ///< Front end RX configuration
#define CC1100_FREND0       (0x22)      ///< Front end TX configuration
#define CC1100_FSCAL3       (0x23)      ///< Frequency synthesizer calibration
#define CC1100_FSCAL2       (0x24)      ///< Frequency synthesizer calibration
#define CC1100_FSCAL1       (0x25)      ///< Frequency synthesizer calibration
#define CC1100_FSCAL0       (0x26)      ///< Frequency synthesizer calibration
#define CC1100_RCCTRL1      (0x27)      ///< RC oscillator configuration
#define CC1100_RCCTRL0      (0x28)      ///< RC oscillator configuration
#define CC1100_FSTEST       (0x29)      ///< Frequency synthesizer calibration control
#define CC1100_PTEST        (0x2A)      ///< Production test
#define CC1100_AGCTEST      (0x2B)      ///< AGC test
#define CC1100_TEST2        (0x2C)      ///< Various test settings
#define CC1100_TEST1        (0x2D)      ///< Various test settings
#define CC1100_TEST0        (0x2E)      ///< Various test settings
/** @} */

/**
 * @name    Strobe commands (14x)
 * @{
 */
#define CC1100_SRES         (0x30)      ///< Reset chip.
/**
 * @brief   Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
 *
 * If in RX/TX: Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
 */
#define CC1100_SFSTXON      (0x31)
#define CC1100_SXOFF        (0x32)      ///< Turn off crystal oscillator.
#define CC1100_SCAL         (0x33)      ///< Calibrate frequency synthesizer and turn it off (enables quick start).
#define CC1100_SRX          (0x34)      ///< Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1.
/**
 * In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
 * If in RX state and CCA is enabled: Only go to TX if channel is clear.
 */
#define CC1100_STX          (0x35)
#define CC1100_SIDLE        (0x36)      ///< Exit RX / TX, turn off frequency synthesizer and exit WOR mode if applicable.
#define CC1100_SAFC         (0x37)      ///< Perform AFC adjustment of the frequency synthesizer
#define CC1100_SWOR         (0x38)      ///< Start automatic RX polling sequence (Wake-on-Radio)
#define CC1100_SPWD         (0x39)      ///< Enter power down mode when CSn goes high.
#define CC1100_SFRX         (0x3A)      ///< Flush the RX FIFO buffer (CC1100 should be in IDLE state).
#define CC1100_SFTX         (0x3B)      ///< Flush the TX FIFO buffer (CC1100 should be in IDLE state).
#define CC1100_SWORRST      (0x3C)      ///< Reset real time clock.
#define CC1100_SNOP         (0x3D)      ///< No operation. May be used to pad strobe commands to two bytes for simpler software.
/** @} */

/**
 * @name    Status registers (12x)
 * @{
 */
#define CC1100_PARTNUM      (0x30)      ///< Part number of CC1100.
#define CC1100_VERSION      (0x31)      ///< Current version number.
#define CC1100_FREQEST      (0x32)      ///< Frequency Offset Estimate.
#define CC1100_LQI          (0x33)      ///< Demodulator estimate for Link Quality.
#define CC1100_RSSI         (0x34)      ///< Received signal strength indication.
#define CC1100_MARCSTATE    (0x35)      ///< Control state machine state.
#define CC1100_WORTIME1     (0x36)      ///< High byte of WOR timer.
#define CC1100_WORTIME0     (0x37)      ///< Low byte of WOR timer.
#define CC1100_PKTSTATUS    (0x38)      ///< Current GDOx status and packet status.
#define CC1100_VCO_VC_DAC   (0x39)      ///< Current setting from PLL calibration module.
#define CC1100_TXBYTES      (0x3A)      ///< Underflow and number of bytes in the TX FIFO.
#define CC1100_RXBYTES      (0x3B)      ///< Overflow and number of bytes in the RX FIFO.
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
#define CC1100_PATABLE      (0x3E)
#define CC1100_TXFIFO       (0x3F)      ///< TX FIFO: Write operations write to the TX FIFO (SB: +0x00; BURST: +0x40)
#define CC1100_RXFIFO       (0x3F)      ///< RX FIFO: Read operations read from the RX FIFO (SB: +0x80; BURST: +0xC0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif
