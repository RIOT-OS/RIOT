/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       On-chip settings for the TI CC1100/CC1101 transceiver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include "cc110x.h"
#include "cc110x_internal.h"

const uint8_t cc110x_conf[CC110X_CONF_SIZE] = {
    /*
     * IOCFG2; default: 0x29 (CHIP_RDYn)
     * Invert GDO2: off,
     * GDO2: Go high when RX data should be read
     *
     * Why not default?
     * GDO2 will be used to be notified about FIFO events (e.g. refilling TX
     * FIFO is needed during transmission, reading from RX FIFO is needed
     * during reception)
     */
    CC110X_GDO_ON_RX_DATA,
    /*
     * IOCFG1; default: 0x2E (3-state)
     * Invert GDO1: off,
     * GDO1: 3-state (required when SPI interface is shared with other devices)
     */
    0x2E,
    /*
     * IOCFG0; default: 0x3F (CLK_XOSC/192)
     * Invert GDO0: off,
     * GDO0: Go high on PLL in lock
     *
     * Why not default?
     * GDO0 will be used to be notified when a packet is coming while in RX
     * mode (will go high during transmission) and when sending is completed
     * while in TX (remains high during transmission and will go back low when
     * done).
     */
    CC110X_GDO_ON_TRANSMISSION,
    /*
     * FIFOTHR; default: 0x07
     * TEST1 = 0x31 and TEST2 = 0x88 when waking up from SLEEP,
     * 0dB RX attenuation,
     * threshold for FIFOs: TX FIFO = 33, RX FIFO = 32
     */
    0x07,
    /*
     * SYNC1, SYNC0; defaults: 0xD3, 0x91
     * Use 0xD3,0x91 as sync word
     */
    0xD3,   /**< SYNC1 */
    0x91,   /**< SYNC0 */
    /*
     * PKTLEN; default: 0xFF
     * Packet length in bytes in fixed length mode, else maximum length
     */
    0xff,
    /*
     * PKTCTRL1; default: 0x04
     * PQT: Accept all sync words, regardless of preamble quality
     * CRC_AUTOFLUSH: Do not auto-flush RX FIFO on incorrect CRC
     * APPEND_STATUS: Do not add 2 bytes of status information in RX FIFO
     * ADDR_CHK:  Filter incoming frames in hardware by address: Only frames
     *            with destination address 0x00 (broadcast) or with with the
     *            layer-2 address of the transceiver are accepted.
     *
     * Why not default?
     * - The RSSI, LQI and CRC info are also available via status registers.
     *   Thus, it is not worth to sacrifice two bytes of RX FIFO for it.
     * - Hardware address filtering could reduce the number IRQs generated
     *   (e.g. a huge frame is dropped before it fully received) which reduces
     *   the system's load. Thus, it is enabled.
     */
    CC110X_PKTCTRL1_VALUE | CC110X_PKTCTRL1_ADDR_MATCH,
    /*
     * PKTCTRL0; default: 0x45
     * Data whitening enabled, use RX/TX FIFOs, CRC enabled,
     * variable packet length
     */
    0x45,
    /*
     * ADDR; default: 0x00
     * Address will overwritten later
     *
     * Why not default?
     * 0x00 is used as broadcast address. Using it would increase chance to
     * receive message during device initialization and thus power consumption.
     */
    0xFF,
    /*
     * CHANNR; default: 0x00
     * Channel number 0 by default
     */
    0x00,
    /*
     * FSCTRL1; default: 0x0C
     * Intermediate frequency: 0x0C * 26MHz / 1024 = 304.7kHz
     *
     * Why not defaults?
     * See MDMCFG4, MDMCFG3
     */
    0x0C,
    /*
     * FSCTRL0; default: 0x00
     * Frequency offset to base frequency: 0kHz
     */
    0x00,
    /*
     * FREQ2, FREQ1, FREQ0; defaults: 0x1E, 0xC4, 0xEC
     * 0x2146E4 * 26MHz / 65536 = 865.1998 MHz (LoRa Channel 10)
     *
     * Why not defaults?
     * Default is 800.000 MHz, which is not in a license free frequency band.
     * Using LoRa channel 10 instead.
     */
    0x21,   /**< FREQ2 */
    0x46,   /**< FREQ1 */
    0xE4,   /**< FREQ0 */
    /*
     * MDMCFG4, MDMCFG3; defaults: 0x8C, 0x22
     * 541.67 kHz channel filter bandwidth,
     * 249.94 kBaud symbol rate
     *
     * Why not defaults?
     * Using 250 kBaud (==> 250kBit/s when 2-FSK/GFSK/ASK) to compete with
     * 802.15.4 in data rate.
     * Using settings listed in Table 7 (pages 12ff in the data sheet):
     *  - 250 kBaud
     *  - GFSK modulation
     *  - 304 kHz IF frequency
     *  - 540 kHz channel filter bandwidth
     *  - 127 kHz deviation
     */
    0x2D,   /**< MDMCFG4 */
    0x3B,   /**< MDMCFG3 */
    /*
     * MDMCFG2; default: 0x02
     * DC blocking filter on,
     * GFSK modulation,
     * no manchester code,
     * Enable RX when 30 bits of the 32 bits sync word are received correctly
     *
     * Why not default?
     * Default expects all 16 bits of a two byte sync word to be correctly
     * received. The data sheet recommends to expect 30 bits of a four byte
     * sync word to be correctly received instead (see section 15 on page 37),
     * so we go for this.
     *
     * Using GFSK instead of 2-FSK reduces the modulated spectrum width and is
     * suggested in Table 7 of the datasheet, see MDMCFG4, MDMCFG3
     */
    0x13,
    /*
     * MDMCFG1, MDMCFG0; defaults: 0x22, 0xF8
     * FEC disabled,
     * 4 preamble bytes,
     * 49.99 kHz distance between channel centre frequencies (closest to 50kHz)
     *
     * Why not defaults?
     * This driver uses an translation layer between physical channels (with
     * 50 kHz distance) and "virtual" channel numbers as seen outside of the
     * driver. This allows to set the frequency in a range of 12.75 MHz with
     * a resolution of 50kHz - this seems to allow to configure all desired
     * channel layouts.
     */
    0x20,   /**< MDMCFG1 */
    0xF8,   /**< MDMCFG0 */
    /*
     * DEVIATN; default: 0x47
     * Deviation of frequency to encode data: +- 126.953kHz in 2-FSK/4-FSK/GFSK
     *
     * Why not default?
     * Higher deviation required for reliable operation at 250 kbps data rate.
     */
    0x62,
    /*
     * MCSM2; default: 0x07
     * No direct RX termination on RSSI measurement (only for ASK/OOK),
     * on RX timeout check for sync word and ignore PQI,
     * no RX timeout
     */
    0x07,
    /*
     * MCSM1; default: 0x30
     * CCA: Enter TX even when channel is detected busy
     * go to idle after packet received,
     * go to idle after packet sent
     *
     * Why not default?
     * By default the transceiver refuses to enter TX when the channel is
     * detected busy. While this is desired, checking if TX was successfully
     * entered is too slow and generated interrupts on the GDO2 pin are easily
     * missed. However, reading the carrier sense value in the PKTSTATUS
     * registers allows to implement this feature in software in a faster
     * way. In addition to not missing GDO2 interrupts, this allows the send
     * function to give feedback right away when the channel is busy.
     */
    0x00,
    /*
     * MCSM0; default: 0x04
     * Auto calibration: Disabled, driver has to manage
     * delay to allow supply voltage to stabilize: 149-155 microseconds
     * pin radio control option is off,
     * oscillator is off in sleep state
     *
     * Why not default?
     * Using 149µs-155µs instead of the default 37µs-39µs as PO_TIMEOUT is
     * encouraged by the data sheet for robust operation
     */
    0x08,
    /*
     * FOCCFG; default: 0x36
     * Freeze frequency offset compensation etc until CS high: yes
     * frequency compensation loop gain before sync word: 3K
     * frequency compensation loop gain after sync word: K/2
     * saturation point for frequency offset compensation: 25% channel bandwidth
     * (incompatible with ASK/OOK)
     */
    0x36,
    /*
     * BSCFG; default: 0x6C
     * Clock recovery feedback loop integral gain before sync word: 2K_i,
     * clock recovery feedback loop proportional gain before sync word: 3K_p,
     * clock recovery feedback loop integral gain after sync word: K_i/2
     * clock recovery feedback loop proportional gain after sync word: K_p,
     * data rate offset compensation: Disabled
     */
    0x6C,
    /*
     * AGCCTRL2; default: 0x03
     * Maximum allowable DVGA gain: No limitation, maximum DVGA gain can be used
     * Maximum allowable LNA + LNA2 gain: No limitation
     * target amplitude from channel filter: 33 dB (default)
     */
    0x03,
    /*
     * AGCCTRL1; default: 0x40
     * LNA priority: Decrease LNA gain first, start decreasing LNA2 gain when
     *               LNA gain reached minimum
     * Relative carrier sense threshold: Disabled
     * Absolute carrier sense threshold: At MAGN_TARGET
     */
    0x40,
    /*
     * AGCCTRL0; default: 0x91
     * HYST_LEVEL: Medium hysteresis, medium asymmetric dead zone, medium gain
     * Adjust gain after how many channel filter samples: After 16 samples
     * Freeze AGC gain: Never; perform gain adjustments as required
     * FILTER_LENGTH:
     *     - 16 channel filter samples when 2-FSK, 4-FSK, or MSK is used
     *     - 8 dB decision baundry when OOK/ASK is used
     */
    0x91,
    /*
     * WOREVT1, WOREVT0, WORCTRL; defaults: 0x87, 0x6B, 0xF8
     * Event0 Timeout: 1.000 seconds
     * RC_PD: 1 (The datasheet is quite cryptic regarding this setting)
     * Event1 Timeout: 1.333ms - 1.385ms (depending on crystal frequency)
     * RC oscillator calibration: Enabled
     * WOR_RES: 0 (Relevant for Event0 resolution and maximum timeout)
     */
    0x87,   /**< WOREVT1 */
    0x6B,   /**< WOREVT0 */
    0xF8,   /**< WORCTRL */
    /*
     * FREND1; default: 0x56
     * LNA_CURRENT: 0b01
     * LNA2MIX_CURRENT: 0b01
     * LODIV_BUF_CURRENT_RW: 0b01
     * MIX_CURRENT: 0b10
     */
    0x56,
    /*
     * FREND0; default: 0x10
     * LODIV_BUF_CURRENT_TX: 0b01
     * Index in PA_POWER table (in 0..7, default is 0): 4 (0dBm)
     *
     * Why not default:
     * Use a reasonable TX power level instead of the lowest.
     */
    0x10 | CC110X_TX_POWER_0_DBM,
    /*
     * FSCAL3, FSCAL2, FSCAL1, FSCAL0; defaults: 0xA9, 0x0A, 0x20, 0x0d
     * These values store calibration date of the CC1100/CC1101 transceiver.
     * Once the transceiver performs a calibration, those registers are updated
     * with the new calibration data. In a "stable" environment (e.g. constant
     * channel/frequency, stable humidity, temperature, supply voltage etc.)
     * the obtained values could be written to the transceiver and calibration
     * could be turned off completely.
     *
     * Fast channel hopping could be performed by obtaining the FSCAL1
     * calibration data for each channel and storing it in the MCU's RAM.
     * The other calibration values is frequency independent according to the
     * data sheet, but depends on temperature etc.
     *
     * Once the FSCAL1 values for each channel are stored, the calibration can
     * be disabled and the stored FSCAL1 data can be uploaded for each channel
     * hop. A re-calibration from time to time is suggested to cope with changes
     * in the environment, e.g. in temperature or supply voltage.
     *
     * Why not defaults?
     * Using "magic" values obtained with SmartRF Studio software for 868 MHz
     * band.
     */
    0xEA,   /**< FSCAL3: charge pump current calibration, frequency independent */
    0x2A,   /**< FSCAL2: VCO current calibration, frequency independent */
    0x00,   /**< FSCAL1: VCO capacitance calibration, frequency dependent */
    0x1F,   /**< FSCAL0: "Magic number", use SmartRF Studio to obtain */
    /*
     * RCCTRL1, RCCTRL0; defaults: 0x41, 0x00
     * RC oscillator configuration, no explanation given in data sheet.
     */
    0x41,   /**< RCCTRL1 */
    0x00,   /**< RCCTRL0 */
};

const uint8_t cc110x_magic_registers[3] = { 0x88, 0x31, 0x09 };
