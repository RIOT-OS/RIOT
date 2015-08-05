/*
 * Copyright (C) 2014 Milan Babel <babel@inf.fu-berlin.de> and INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
  * @ingroup CC2420
  * @{
  * @file
  * @brief Definitions and settings for the CC2420
  * @author Milan Babel <babel@inf.fu-berlin.de>
  * @author Kévin Roussel <Kevin.Roussel@inria.fr>
  *
  */
#ifndef CC2420_SETTINGS_H
#define CC2420_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CC2420 access mode values.
 * @brief Values defining the command you want to give to the CC2420:
 *        read or write access to registers or RAM.
 *        These values are to be OR-ed to the address you want to access.
 * @see CC2420 datasheet section 13.2 page 31.
 * @{
 */
/** @brief Read-Register Mode */
#define CC2420_READ_ACCESS      0x40
/** @brief Write Register Mode */
#define CC2420_WRITE_ACCESS     0x00
/**
 * @brief RAM Access Modifier (to use with @c CC2420_RAM_READ_ACCESS
 *        or @c CC2420_RAM_WRITE_ACCESS)
 */
#define CC2420_RAM_ACCESS       0x80
/** @brief Read-Memory Mode (to use with @c CC2420_RAM_ACCESS) */
#define CC2420_RAM_READ_ACCESS  0x20
/** @brief Write-Memory Mode (to use with @c CC2420_RAM_ACCESS) */
#define CC2420_RAM_WRITE_ACCESS 0x00

/**
 * @}
 */


/**
 * @name Strobe command addresses
 * @brief Addresses of "strobe" commands in CC2420 address space.
 * @see CC2420 datasheet section 37 pages 61--62.
 * @{
 */
/** @brief NO Operation @see NOBYTE  */
#define CC2420_STROBE_NOP       0x00
/** @brief Turn transceiver (internal oscillator) on */
#define CC2420_STROBE_XOSCON    0x01
/** @brief Calibrate TX frequency, then put transceiver in wait (inactive) mode */
#define CC2420_STROBE_TXCAL     0x02
/** @brief Put transceiver in RX mode */
#define CC2420_STROBE_RXON      0x03
/** @brief Put transceiver in TX mode */
#define CC2420_STROBE_TXON      0x04
/** @brief Put transceiver in TX mode after checking media availability */
#define CC2420_STROBE_TXONCCA   0x05
/** @brief Put transceiver in idle mode */
#define CC2420_STROBE_RFOFF     0x06
/** @brief Turn transceiver (internal oscillator) off -> power-down mode */
#define CC2420_STROBE_XOSCOFF   0x07
/** @brief Flush transceiver's RX buffer */
#define CC2420_STROBE_FLUSHRX   0x08
/** @brief Flush transceiver's TX buffer */
#define CC2420_STROBE_FLUSHTX   0x09
/** @brief Send an ACK frame with pending field cleared */
#define CC2420_STROBE_ACK       0x0A
/** @brief Send an ACK frame with pending field set */
#define CC2420_STROBE_ACKPEND   0x0B
/** @brief Start RXFIFO in-line decryption/verification */
#define CC2420_STROBE_RXDEC     0x0C
/** @brief Start TXFIFO in-line encryption/authentication */
#define CC2420_STROBE_TXENC     0x0D
/** @brief Start standalone AES encryption */
#define CC2420_STROBE_AES       0x0E

/**
 * @}
 */



/**
 * @name CC2420 Configuration Register addresses.
 * @brief Addresses of configuration registers in CC2420 address space.
 * @see CC2420 datasheet section 37 pages 61 to 80.
 * @{
 */
/** @brief Main Control Register  */
#define CC2420_REG_MAIN         0x10
/**
 * @brief Modem Control Register 0
 * @{
 */
#define CC2420_REG_MDMCTRL0     0x11
    /** @brief Address Decode enable flag */
    #define CC2420_ADR_DECODE          0x800
    /** @brief Reserved Frame accept enable flag */
    #define CC2420_RES_FRM_MODE        0x2000
    /** @brief PAN Coordinator mode enable flag */
    #define CC2420_PAN_COORD           0x1000
    /** @brief Automatic CRC computation/verification enable flag */
    #define CC2420_AUTOCRC             0x20
    /** @brief Automatic ACK response enable flag */
    #define CC2420_AUTOACK             0x10
/** @} */
/** @brief Modem Control Register 1 */
#define CC2420_REG_MDMCTRL1     0x12
/**
 * @brief RSSI and CCA Status and Control Register
 * @{
 */
#define CC2420_REG_RSSI         0x13
    /** @brief CCA Threshold value mask */
    #define CC2420_CCATHR_MASK         0xFF00
    /** @brief RSSI estimate value mask */
    #define CC2420_RSSI_MASK           0xFF
/** @} */
/** @brief Sync Word  */
#define CC2420_REG_SYNCWORD     0x14
/**
 * @brief Transmit Control Register
 * @{
 */
#define CC2420_REG_TXCTRL       0x15
    /** @brief Output PA Level value mask */
    #define CC2420_PALEVEL_MASK        0x1F
/** @} */
/** @brief Receive Control Register 0 */
#define CC2420_REG_RXCTRL0      0x16
/**
 * @brief Receive Control Register 1
 * @{
 */
#define CC2420_REG_RXCTRL1      0x17
    /** @brief RX BandPass Filter Bias Current mode flag */
    #define CC2420_RXBPF_LOCUR         0x2000
/** @} */
/**
 * @brief Frequency Synthesizer Control and Status Register
 * @{
 */
#define CC2420_REG_FSCTRL       0x18
    /** @brief RF Operating Frequency mask */
    #define CC2420_FREQ_MASK           0x3FF
/** @} */
/**
 * @brief Security Control Register 0
 * @{
 */
#define CC2420_REG_SECCTRL0     0x19
    /** @brief RXFIFO Protection enable flag */
    #define CC2420_RXFIFO_PROTECTION   0x200
/** @} */
/** @brief Security Control Register 1 */
#define CC2420_REG_SECCTRL1     0x1A
/** @brief Battery Monitor Control Register */
#define CC2420_REG_BATTMON      0x1B
/**
 * @brief I/O Configuration Register 0
 * @{
 */
#define CC2420_REG_IOCFG0       0x1C
    /** @brief FIFOP Activation Threshold mask */
    #define CC2420_FIFOPTHR_MASK       0x7F
/** @} */
/** @brief I/O Configuration Register 1 */
#define CC2420_REG_IOCFG1       0x1D
/** @brief Manufacturer ID (low word) */
#define CC2420_REG_MANFIDL      0x1E
/** @brief Manufacturer ID (high word) */
#define CC2420_REG_MANFIDH      0x1F
/** @brief Finite State Machine Time Constants */
#define CC2420_REG_FSMTC        0x20
/** @brief Manual signal AND override Register */
#define CC2420_REG_MANAND       0x21
/** @brief Manual signal OR override Register */
#define CC2420_REG_MANOR        0x22
/** @brief AGC Control Register  */
#define CC2420_REG_AGCCTRL      0x23
/** @brief AGC Test Register 0 */
#define CC2420_REG_AGCTST0      0x24
/** @brief AGC Test Register 1 */
#define CC2420_REG_AGCTST1      0x25
/** @brief AGC Test Register 2 */
#define CC2420_REG_AGCTST2      0x26
/** @brief Frequency Synthesizer Test Register 0 */
#define CC2420_REG_FSTST0       0x27
/** @brief Frequency Synthesizer Test Register 1 */
#define CC2420_REG_FSTST1       0x28
/** @brief Frequency Synthesizer Test Register 2 */
#define CC2420_REG_FSTST2       0x29
/** @brief Frequency Synthesizer Test Register 3 */
#define CC2420_REG_FSTST3       0x2A
/** @brief Receiver Bandpass Filters Test Register  */
#define CC2420_REG_RXBPFTST     0x2B
/** @brief Finite State Machine Information Register  */
#define CC2420_REG_FSMSTATE     0x2C
/** @brief ADC Test Register */
#define CC2420_REG_ADCTST       0x2D
/** @brief DAC Test Register */
#define CC2420_REG_DACTST       0x2E
/** @brief Top-level Test Register */
#define CC2420_REG_TOPTST       0x2F

/** @brief Transmit FIFO Byte Register */
#define CC2420_REG_TXFIFO       0x3E
/** @brief Receive FIFO Byte Register */
#define CC2420_REG_RXFIFO       0x3F

/**
 * @}
 */



/**
 * @name CC2420 Configuration RAM addresses.
 * @brief Addresses of configuration zones in CC2420 RAM address space.
 * @see CC2420 datasheet section 13.5 page 31.
 * @{
 */
/** @brief Short (16-bit) address of the system into its current PAN. */
#define CC2420_RAM_SHORTADR     0x16A
/** @brief 16-bit ID of the PAN into which the system is associated. */
#define CC2420_RAM_PANID        0x168
/** @brief IEEE long (64-bit) address of the system. */
#define CC2420_RAM_IEEEADR      0x160

/**
 * @}
 */


/**
 * @name Status byte flags
 * @brief Bits of the status byte returned by CC2420 adter each command.
 * @see CC2420 datasheet section 13.3 page 29.
 * @{
 */

/**
 * @brief Bit XOSC16M_STABLE of the status register.
 * @details This bit indicates whether the 16-MHz oscillator
 *          is running (value 1), or if the transceiver
 *          is in power-down mode (value 0).
 */
#define CC2420_STATUS_XOSC16M_STABLE   0x40
/**
 * @brief Bit TX_UNDERFLOW of the status register.
 * @details This bit indicates whether the latest transmission has failed
 *          because the TX buffer didn't contain enough data (error bit).
 */
#define CC2420_STATUS_TX_UNDERFLOW     0x20
/**
 * @brief Bit ENC_BUSY of the status register.
 * @details This bit indicates whether the encryption module
 *          is currently busy (value 1), or not (value 0).
 */
#define CC2420_STATUS_ENC_BUSY         0x10
/**
 * @brief Bit TX_ACTIVE of the status register.
 * @details This bit indicates whether a RF transmission
 *          is currently in progress (value 1), or not (value 0).
 */
#define CC2420_STATUS_TX_ACTIVE        0x08
/**
 * @brief Bit PLL_LOCK of the status register.
 * @details This bit indicates whether the frequency synthesizer PLL
 *          is currently locked (value 1), or not (value 0).
 */
#define CC2420_STATUS_PLL_LOCK         0x04
/**
 * @brief Bit RSSI_VALID of the status register.
 * @details This bit indicates whether the RSSI value
 *          is valid (value 1), or not (value 0).
 *          The RSSI is always valid when the CC2420
 *          has been in RX mode for at least 128 µs),
 */
#define CC2420_STATUS_RSSI_VALID       0x02

/**
 * @}
 */



/**
 * @brief Null command strobe/parameter.
 * @details Basically a strobe address used to perform a NOP on CC2420;
 *          This is useful to just query the status byte, or more generally
 *          passively receive bytes from the transceiver on its SPI link.
 */
#define NOBYTE 0x0


/* Various configuration settings for the CC2420 drivers  */
#define CC2420_DEFAULT_CHANNR   (26U)
#define CC2420_SYNC_WORD_TX_TIME 900000
#define CC2420_RX_BUF_SIZE      3
#define CC2420_WAIT_TIME        500

#ifdef __cplusplus
}
#endif

#endif
