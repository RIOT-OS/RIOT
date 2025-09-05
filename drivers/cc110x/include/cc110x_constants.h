/*
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Constants for the CC1100/CC1101 driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Command strobes of the CC1100/CC1101 transceiver
 *
 * See Table 42 on page 67 in the data sheet. Only values relevant to the
 * driver are listed.
 *
 * @{
 */

/**
 * @brief   Reset chip (SRES)
 */
#define CC110X_STROBE_RESET       0x30

/**
 * @brief   Calibrate frequency synthesizer and turn it off (SCAL)
 */
#define CC110X_STROBE_CALIBRATE   0x33

/**
 * @brief   Go to RX state (SRX)
 *
 * Requires frequency calibration first
 */
#define CC110X_STROBE_RX          0x34

/**
 * @brief   Go to TX state (STX)
 *
 * Requires frequency calibration first
 */
#define CC110X_STROBE_TX          0x35

/**
 * @brief   Go to IDLE state (SIDLE)
 */
#define CC110X_STROBE_IDLE        0x36

/**
 * @brief   Go to power down state once CS goes high (SPWD)
 */
#define CC110X_STROBE_OFF         0x39

/**
 * @brief   Flush RX fifo (SFRX)
 *
 * Only valid in IDLE or in RXFIO_OVERFLOW states
 */
#define CC110X_STROBE_FLUSH_RX    0x3A

/**
 * @brief   Flush TX fifo (SFTX)
 *
 * Only valid in IDLE or in TXFIO_OVERFLOW states
 */
#define CC110X_STROBE_FLUSH_TX    0x3B

/**
 * @brief   Get the status byte (SNOP)
 */
#define CC110X_STROBE_STATUS      0x3D

/** @} */

/**
 * @name    Access modifies for accessing configuration/status registers
 *
 * See Table 45 on pages 69ff in the data sheet. These modifies need to be
 * xor'ed with the address of the register.
 *
 * @{
 */

/**
 * @brief   Access modifier to write a single byte to a configuration register
 *
 * | read bit (`0x80`) | burst access bit (`0x40`) |
 * |-------------------|---------------------------|
 * | `0` (= write)     | `0` (= no burst access)   |
 */
#define CC110X_SINGLE_BYTE_WRITE  0x00

/**
 * @brief   Access modifier to write multiple bytes at once to configuration
 *          registers
 *
 * | read bit (`0x80`) | burst access bit (`0x40`) |
 * |-------------------|---------------------------|
 * | `0` (= write)     | `1` (= burst access)      |
 */
#define CC110X_BURST_WRITE        0x40

/**
 * @brief   Access modifier to read a single byte from a configuration register
 *
 * | read bit (`0x80`) | burst access bit (`0x40`) |
 * |-------------------|---------------------------|
 * | `1` (= read)      | `0` (= no burst access)   |
 */
#define CC110X_SINGLE_BYTE_READ   0x80

/**
 * @brief   Access modifier to read multiple bytes at once from configuration
 *          registers
 *
 * | read bit (`0x80`) | burst access bit (`0x40`) |
 * |-------------------|---------------------------|
 * | `1` (= read)      | `1` (= burst access)      |
 */
#define CC110X_BURST_READ         0xC0

/** @} */

/**
 * @name    "Multi byte registers" of the CC1100/CC1101 transceiver
 *
 * See Table 45 on pages 69ff in the data sheet. These multi byte registers
 * have a special semantics, which is documented for each multi byte register
 *
 * @{
 */

/**
 * @brief   Access to the PATABLE as multi byte register
 *
 * It is most convenient to read/write the whole 8 bytes of the PATABLE using a
 * burst access. The first single byte access after the CS pin is pulled low
 * will read from / write to the first byte, the second access the second byte,
 * and so on. As @ref cc110x_read and @ref cc110x_write pull the CS pin high
 * after the access, all but the first byte are only accessible using burst
 * access in this driver.
 */
#define CC110X_MULTIREG_PATABLE   0x3E

/**
 * @brief   Access to the TX and RX FIFO as multi byte register
 *
 * A single byte read using @ref cc110x_read from the FIFO multi byte register
 * will retrieve and remove the next byte from the RX FIFO. A burst of *n* bytes
 * using @ref cc110x_burst_read will retrieve and remove the next *n* bytes.
 *
 * A single byte write using @ref cc110x_write will push one byte of data into
 * the TX FIFO. A multi byte write of *n* byte using @ref cc110x_burst_write
 * will push *n* bytes into the TX FIFO.
 *
 * @warning Reading the last byte from the RX-FIFO results in data corruption,
 *          unless the whole frame was received. Thus, read all but the last
 *          byte from the FIFO until the whole frame was received.
 */
#define CC110X_MULTIREG_FIFO      0x3F

/** @} */

/**
 * @name    Configuration registers of the CC1100/CC1101 transceiver
 *
 * See Table 43 on pages 68ff in the data sheet. Only values relevant to the
 * driver are listed.
 *
 * @{
 */

/**
 * @brief   First configuration register on the transceiver, used for burst
 *          access to the whole configuration
 */
#define CC110X_CONF_START         0x00

/**
 * @brief   GDO2 output pin configuration
 */
#define CC110X_REG_IOCFG2         0x00

/**
 * @brief   GDO1 output pin configuration
 */
#define CC110X_REG_IOCFG1         0x01

/**
 * @brief   GDO0 output pin configuration
 */
#define CC110X_REG_IOCFG0         0x02

/**
 * @brief   PKTCTRL1 configuration register
 *
 * This register contains multiple configuration settings.
 *
 * Layout:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+
 * | PQT |U|C|S|ADR|
 * +-+-+-+-+-+-+-+-+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - PQT (bits 7-5): Preabmle quality estimator threshold
 * - U (bit 4): unused, always 0
 * - C (bit 3): Auto-clear RX FIFO on CRC mismatch (if frame fits in FIFO)
 * - S (bit 2): Append status
 * - ADR (bits 1-0): Address check setting
 */
#define CC110X_REG_PKTCTRL1       0x07

/**
 * @brief   Device address
 */
#define CC110X_REG_ADDR           0x09

/**
 * @brief   Channel number
 */
#define CC110X_REG_CHANNR         0x0A

/**
 * @brief   Intermediate frequency to use
 */
#define CC110X_REG_FSCTRL1        0x0B

/**
 * @brief   Frequency control word, high byte
 */
#define CC110X_REG_FREQ2          0x0D

/**
 * @brief   Frequency control word, middle byte
 */
#define CC110X_REG_FREQ1          0x0E

/**
 * @brief   Frequency control word, low byte
 */
#define CC110X_REG_FREQ0          0x0F

/**
 * @brief   Modem configuration (channel filter bandwidth and data rate)
 */
#define CC110X_REG_MDMCFG4        0x10

/**
 * @brief   Modem configuration (data rate)
 */
#define CC110X_REG_MDMCFG3        0x11

/**
 * @brief   Modem deviation setting
 */
#define CC110X_REG_DEVIATN        0x15

/**
 * @brief   Front End TX Configuration
 *
 * Least three significant bits contain the current PA power setting.
 */
#define CC110X_REG_FREND0         0x22

/**
 * @brief   Charge pump current calibration
 *
 * This value depends on the environment (e.g. temperature, supply voltage,
 * etc.), but not on the frequency. Thus, this value does not become obsolete
 * when changing the channel.
 */
#define CC110X_REG_FSCAL3         0x23

/**
 * @brief   VCO current calibration
 *
 * This value depends on the environment (e.g. temperature, supply voltage,
 * etc.), but not on the frequency. Thus, this value does not become obsolete
 * when changing the channel.
 */
#define CC110X_REG_FSCAL2         0x24

/**
 * @brief   VCO capacitance calibration
 *
 * This value is frequency depended. Thus, for fast channel hopping it has to
 * be obtained for each channel (by performing a calibration on that channel
 * and reading it out). Than the stored calibration data can be written to the
 * register when changing the channel.
 */
#define CC110X_REG_FSCAL1         0x25

/**
 * @brief   Undocumented frequency calibration value
 *
 * For fast channel hopping this value can be ignored (see page 64ff in the
 * data sheet) - so it has to be frequency independent.
 */
#define CC110X_REG_FSCAL0         0x26

/**
 * @brief   Unlock the temperature sensor by writing 0xBF to it
 *
 * Intended for production test, but who would complain about getting an
 * temperature sensor for free :-)
 *
 * @see     @ref CC110X_GDO0_ANALOG_TEMPERATURE for details
 */
#define CC110X_REG_PTEST          0x2A

/**
 * @brief   Magic value obtained with SmartRF Studio software
 */
#define CC110X_REG_TEST2          0x2C

/**
 * @brief   Magic value obtained with SmartRF Studio software
 */
#define CC110X_REG_TEST1          0x2D

/**
 * @brief   Magic value obtained with SmartRF Studio software
 */
#define CC110X_REG_TEST0          0x2E

/** @} */

/**
 * @name    Status registers of the CC1100/CC1101 transceiver
 *
 * See Table 43 on pages 68ff in the data sheet. Only values relevant to the
 * driver are listed.
 *
 * @warning The burst access bit of these registers has to be set to distinguish
 *          between command strobes and status registers. Thus, no burst access
 *          to status registers is possible.
 *
 * @{
 */

/**
 * @brief   Part number
 *
 * @warning Not accessible using burst reads
 */
#define CC110X_REG_PARTNUM        (0x30 | 0x40)

/**
 * @brief   Version
 *
 * @warning Not accessible using burst reads
 */
#define CC110X_REG_VERSION        (0x31 | 0x40)

/**
 * @brief   Estimated link quality
 *
 * @warning Not accessible using burst reads
 */
#define CC110X_REG_LQI            (0x33 | 0x40)

/**
 * @brief   Received signal strength indication
 *
 * @warning Not accessible using burst reads
 */
#define CC110X_REG_RSSI           (0x34 | 0x40)

/**
 * @brief   Packet status, GDOx status
 *
 * @warning Not accessible using burst reads
 */
#define CC110X_REG_PKTSTATUS      (0x38 | 0x40)

/**
 * @brief   Number of bytes in the TX FIFO
 *
 * @warning Not accessible using burst reads
 * @warning The received value could be corrupted when reading it while it is
 *          updated. Reading it out twice until both reads return the same
 *          value is the suggested workaround for this hardware bug.
 */
#define CC110X_REG_TXBYTES        (0x3A | 0x40)

/**
 * @brief   Number of bytes available in the RX FIFO
 *
 * @warning Not accessible using burst reads
 * @warning The received value could be corrupted when reading it while it is
 *          updated. Reading it out twice until both reads return the same
 *          value is the suggested workaround for this hardware bug.
 */
#define CC110X_REG_RXBYTES        (0x3B | 0x40)

/** @} */

/**
 * @name    Possible values for the IOCFG2, IOCFG1, and IOCFG0 configuration
 *          registers
 *
 * See Table 41 on page 62 in the data sheet. Only values relevant to the
 * driver are listed.
 *
 * @{
 */

/**
 * @brief   GDOx goes HIGH when data has to be read from RX FIFO or when a
 *          packet is fully received
 *
 * Depends on the threshold set for the RX-FIFO in the FIFOTHR configuration
 * register
 */
#define CC110X_GDO_ON_RX_DATA           0x01

/**
 * @brief   GDOx goes LOW when data should be written to the TX FIFO
 *
 * Depends on the threshold set for the TX-FIFO in the FIFOTHR configuration
 * register
 */
#define CC110X_GDO_ON_TX_DATA           0x02

/**
 * @brief   GDOx goes HIGH when a packet is received/send and back LOW when the
 *          transmission is completed/aborted (e.g. wrong destination address)
 */
#define CC110X_GDO_ON_TRANSMISSION      0x06

/**
 * @brief   GDOx goes HIGH when channel is clear for sending
 *
 * Depends on the CCA_MODE setting in the MCSM1 configuration register
 */
#define CC110X_GDO_ON_CHANNEL_CLEAR     0x09

/**
 * @brief   GDOx goes HIGH when PLL is in lock
 *
 */
#define CC110X_GDO_ON_PLL_IN_LOCK       0x0A

/**
 * @brief   GDOx remains constantly LOW
 */
#define CC110X_GDO_CONSTANT_LOW         0x2F

/**
 * @brief   GDOx remains constantly HIGH
 */
#define CC110X_GDO_CONSTANT_HIGH        0x6F

/**
 * @brief   Repurpose GDO0 as analog temperature sensor in IDLE state
 *
 * This only works with GDO0 and only in IDLE state! Additionally, 0xBF has
 * to be written to configuration register PTEST when in IDLE state. Before
 * leaving IDLE state, PTEST should be restored to 0x7F.
 */
#define CC110X_GDO0_ANALOG_TEMPERATURE  0x80

/** @} */

/**
 * @name    Bitmasks to access entries in the PKTSTATUS status register
 *
 * See page 94 in the data sheet.
 *
 * @{
 */
/**
 * @brief   Bitmask to get the GDO0 state from the PKTSTATUS status register
 *          value
 */
#define CC110X_PKTSTATUS_GDO0           0x01
/**
 * @brief   Bitmask to get the GDO2 state from the PKTSTATUS status register
 *          value
 */
#define CC110X_PKTSTATUS_GDO2           0x04
/**
 * @brief   Bitmask to get the SFD bit from the PKTSTATUS status register value
 *          which is set while receiving a frame
 */
#define CC110X_PKTSTATUS_RECEIVING      0x08
/**
 * @brief   Bitmask to get the CCA bit from the PKTSTATUS status register value
 */
#define CC110X_PKTSTATUS_CCA            0x10
/**
 * @brief   Bitmask to get the Carrier Sense bit from the PKTSTATUS status
 *          register value
 */
#define CC110X_PKTSTATUS_CS             0x40
/** @} */

/**
 * @name    Values to write into the PTEST configuration register
 *
 * See page 91 in the data sheet. Only the two documented values are specified.
 *
 * @{
 */
/**
 * @brief   Default value of the PTEST configuration register.
 */
#define CC110X_PTEST_DEFAULT            0x7F
/**
 * @brief   Value to write in PTEST when reading the temperature.
 *
 * @see     CC110X_GDO0_ANALOG_TEMPERATURE
 */
#define CC110X_PTEST_TEMPERATURE        0xBF
/** @} */

/**
 * @brief   Size of the RX and TX FIFO
 */
#define CC110X_FIFO_SIZE                64

/**
 * @brief   Value of the bits 7-2 of the PKTCTRL1 configuration register used
 *          in this driver
 */
#define CC110X_PKTCTRL1_VALUE           0x00

/**
 * @name    Possible address matching policies
 *
 * See page 73 in the data sheet. The policy should be combined with
 * @ref CC110X_PKTCTRL1_VALUE via bitwise or. (Only modes compatible with the
 * driver are defined.)
 *
 * @{
 */
/**
 * @brief   Accept incoming frames regardless of address
 */
#define CC110X_PKTCTRL1_ADDR_ALL        0x00
/**
 * @brief   Accept frames with matching address or broadcast address
 */
#define CC110X_PKTCTRL1_ADDR_MATCH      0x02
/**
 * @brief   Bitmask to access address matching mode of the CC110x from the
 *          PKTCTRL1 register
 *
 * Apply this using bitwise and to the value of the PKTCTRL1 register to get the
 * address matching mode currently used.
 */
#define CC110X_PKTCTRL1_GET_ADDR_MODE   0x03
/** @} */

/**
 * @brief   Time in micro seconds the CC110X takes to wake up from SLEEP state
 */
#define CC110X_WAKEUP_TIME_US           150

#ifdef __cplusplus
}
#endif

/** @} */
