/*
 * SPDX-FileCopyrightText: 2013 INRIA
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2018,2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_cc110x CC1100/CC1100e/CC1101 Sub-GHz transceiver driver
 * @ingroup     drivers_netdev
 *
 * This module contains the driver for the TI CC1100/CC110e/CC1101 Sub-GHz
 * transceivers.
 *
 * @warning How the CC1100/CC1101 operate can be configured quite sophistically.
 *          This has the drawback, that configurations breaking laws and rules
 *          are complete possible. Please make sure that the configured output
 *          power, duty cycle, frequency range, etc. conform to the rules,
 *          standards and laws that apply in your use case.
 *
 *
 * Supported Hardware and how to obtain
 * ====================================
 *
 * This driver has been developed for the CC1101 and the older CC1100
 * transceiver and tested for both. However, it should work with the CC1100e
 * as well - but this has *NOT* been tested at all.
 *
 * It is suggested to go for the CC1101 when considering to buy one of the
 * supported transceivers. The easiest way is to obtain CC1101 break out boards
 * with a complete antenna circuit & antenna that can be connected via jumper
 * wires using an 8 pin DIP pin header. These are sold in various flavours start
 * from less than 2€ at quantity one at your favourite Far East store. Beware
 * that while the CC1101 chip can operate a various base frequencies, the
 * antenna circuit will only work for a single frequency band. Most break out
 * boards will operate at 433 MHz, which is license free in many countries (but
 * verify that for your country before buying!). EU citizens might prefer the
 * 868 MHz band over the 433 MHz, as more license free bandwidth is available
 * in the 868 MHz band in the EU. (But when deploying only a few dozens of
 * devices, the 433 MHz band is also fine for EU citizens.) US citizens should
 * go for the 900 MHz band (as 868 MHz is not license free in the USA), which
 * even contains more bandwidth than the 868 MHz band. (However, the 900 MHz
 * band has not been tested, as using it would be illegal in the EU.)
 *
 *
 * Packet Format
 * =============
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Preamble (4 bytes, handled by hardware, see MDMCFG1)      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Sync Word (4 bytes, handled by hardware, see MDMCFG2)     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Length Field  |  Destination  |    Source     |  Payload...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  ... (see Length Field)         |  CRC (handled by hardware)    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * | Field        | Description                                                |
 * |--------------|------------------------------------------------------------|
 * | Preamble     | 4 bytes, handled by hardware                               |
 * | Sync Word    | 4 bytes, handled by hardware                               |
 * | Length Field | Handled by software & hardware, length of payload + 2      |
 * | Destination  | Handled by software & hardware, destination MAC address    |
 * | Source       | Handled by software only, source MAC address               |
 * | Payload      | Handled by software only, the payload to send              |
 * | CRC          | 2 bytes, handled by hardware                               |
 *
 * The Length Field contains the length of the driver supplied data in bytes,
 * not counting the Length Field. Thus, it contains the length of the payload
 * plus the length of the Destination and Source address.
 *
 *
 * Layer-2 Addresses
 * -----------------
 *
 * The layer 2 addresses of the CC110x transceivers is a single byte long and
 * the special value `0x00` for the destination address is used in broadcast
 * transmissions. The transceiver is configured by this driver to ignore all
 * packets unless the destination address matches the address of the transceiver
 * or the destination address is `0x00`.
 *
 * Please note that the layer 2 address by default is derived from the CPU ID.
 * Due to the birthday paradox with only 20 devices the probability of a
 * collision is already bigger than 50%. Thus, manual address assignment is
 * supported by providing an implementation of @ref cc1xxx_eui_get.
 *
 *
 * Base Band, Data Rate, Channel Bandwidth and Channel Map Configuration
 * =====================================================================
 *
 * This driver allows to configure the base band, the data rate and the channel
 * bandwidth using an @ref cc110x_config_t data structure. Default
 * configurations are supplied and name using the following scheme:
 * `cc110x_config_<BASE-BAND>_<DATA-RATE>_<CHANNEL-BANDWIDTH>`. (E.g.
 * @ref cc110x_config_868mhz_250kbps_300khz is the default configuration used by
 * the MSB-A2 and the MSB-IoT boards.)
 *
 * Using the @ref cc110x_chanmap_t data structure the channel layout can be
 * defined. This map contains 8 entries, each defines the offset from the base
 * frequency defined in the @ref cc110x_config_t data structure for each
 * channel in steps of 50kHz. E.g. @ref cc110x_chanmap_868mhz_lora provides
 * the LoRa channels 10 to 17 in the 868MHz band. (The RIOT channel numbers
 * will always start from 0, and currently only up to eight channels are
 * supported. A special value of 255 as offset from the base frequency in the
 * channel map is used mark the channel as disabled. This can be used if less
 * than 8 non-overlapping channels are possible in the license free band.)
 *
 * Please note that the channel map (@ref cc110x_chanmap_t) must match the
 * base configuration (@ref cc110x_config_t), as the channel map is relative
 * to the configured base frequency. Also, the distance between the channels
 * in the channel map should match the channel bandwidth of the configuration,
 * as otherwise channels could overlap.
 *
 * Both configuration and matching channel map can be applied using
 * @ref cc110x_apply_config. Please consider this as a slow operation, as the
 * transceiver needs to be calibrated for each channel in the channel map.
 *
 *
 * Calibration of the Frequency Generator
 * ======================================
 *
 * The CC110x transceivers use a voltage controlled oscillator (VCO) and a
 * phase locked loop (PLL) for frequency generation. However, they need to be
 * calibrated to work correctly with the given supply voltage and the current
 * temperature. The driver will perform this calibration during startup, but
 * when the supply voltage or the temperature is not stable, a recalibration is
 * required whenever the supply voltage of temperature has changed too much since
 * the last calibration. This can be done by calling
 * @ref cc110x_full_calibration. It is left to the application developer to
 * perform this calibration when needed. During a test of about 2 hours of
 * operation in an in-door environment with a stable temperature the CC1101 has
 * worked reliable without any calibration at all (except for the automatic
 * calibration at start up). So there are use cases which do not require any
 * recalibration at all.
 *
 *
 * Troubleshooting
 * ===============
 *
 * The Driver Does Not Initialize Properly
 * ---------------------------------------
 * Set `ENABLE_DEBUG` in `cc110x_netdev.c` to `1` to get debug output, which
 * will likely tell you what is going wrong. There are basically two things
 * that can fail:
 *
 * Upon initialization the driver will read out the part number and version of
 * the transceiver. If those do not match the ones expected for the CC1100,
 * CC1100E, or the CC1101 the driver will refuse to initialize. If this fails,
 * most likely incorrect values are read out and the SPI communication does not
 * work correctly. However, future revisions of the CC110X transceivers might
 * be produced and might have different values for the part number or version.
 * If this should happen and they remain compatible with the driver, their
 * part number & revision needs to be added to the driver.
 *
 * After uploading the configuration, the driver will read back the
 * configuration to verify it. If the SPI communication is not reliable (e.g.
 * sporadically bits flip), this will fail from time to time. E.g. on the
 * MSB-IoT boards this is the case when the SPI interface operates at a clock of
 * 5MHz, but it becomes reliable when clocked at 1MHz.
 *
 * The Driver Initializes, but Communication Is Impossible
 * -------------------------------------------------------
 * If two transceivers are too close to each other and TX power is at maximum,
 * the signal is just too strong to be received correctly. Reducing TX power
 * or increasing the distance (about half a meter should be fine) will solve
 * this issue.
 *
 * While the chips can operate at any base frequency offered by the driver,
 * the circuit the chip is connected to and the antenna are build for a single
 * base band. Check if your configuration matches the frequency range the
 * board is build for. E.g. most break out boards operate at 433MHz, but there
 * are also boards for 868MHz.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the CC1100/CC1101 driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdint.h>

#include "cc1xxx_common.h"
#include "mutex.h"
#include "net/gnrc/nettype.h"
#include "net/netdev.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of a layer 2 frame
 *
 * This does not include the preamble, sync word, CRC field, and length field.
 */
#define CC110X_MAX_FRAME_SIZE           0xFF

/**
 * @brief   Maximum (layer 2) payload size supported by the driver
 */
#define CC110X_MAX_PAYLOAD_SIZE         (CC110X_MAX_FRAME_SIZE - CC1XXX_HEADER_SIZE)

/**
 * @brief   Maximum number of channels supported by the driver
 */
#define CC110X_MAX_CHANNELS             8

/**
 * @brief   Default protocol for data that is coming in
 */
#ifdef MODULE_GNRC_SIXLOWPAN
#define CC110X_DEFAULT_PROTOCOL         (GNRC_NETTYPE_SIXLOWPAN)
#else
#define CC110X_DEFAULT_PROTOCOL         (GNRC_NETTYPE_UNDEF)
#endif

/**
 * @defgroup drivers_cc110x_config CC1100/CC1100e/CC1101 Sub-GHz transceiver driver
 *                                 compile time configuration
 * @ingroup config_drivers_netdev
 * @{
 */
/**
 * @brief The default channel to set up after initializing the device
 */
#ifndef CONFIG_CC110X_DEFAULT_CHANNEL
#define CONFIG_CC110X_DEFAULT_CHANNEL      (0U)
#endif
/** @} */

/**
 * @brief   The state of the CC1100/CC1101 transceiver
 *
 * The three least significant bytes match the representation of the matching
 * transceiver state given in the status byte of the hardware. See Table 32 on
 * page 31 in the data sheet for the possible states in the status byte.
 */
typedef enum {
    CC110X_STATE_IDLE               = 0x00,   /**< IDLE state */
    /**
     * @brief   Frame received, waiting for upper layer to retrieve it
     *
     * Transceiver is in IDLE state.
     */
    CC110X_STATE_FRAME_READY        = 0x08,
    /**
     * @brief   Devices is powered down
     *
     * Transceiver is in SLEEP state. There is no matching representation in the
     * status byte, as reading the status byte will power up the transceiver in
     * bring it in the IDLE state. Thus, we set the three least significant bits
     * to the IDLE state
     */
    CC110X_STATE_OFF                = 0x10,
    CC110X_STATE_RX_MODE            = 0x01,   /**< Listening for frames */
    /**
     * @brief   Receiving a frame just now
     *
     * Transceiver is in RX state.
     */
    CC110X_STATE_RECEIVING          = 0x09,
    CC110X_STATE_TX_MODE            = 0x02,   /**< Transmit mode */
    /**
     * @brief   Waiting for transceiver to complete outgoing transmission
     *
     * Transceiver is in TX state
     */
    CC110X_STATE_TX_COMPLETING      = 0x0A,
    CC110X_STATE_FSTXON             = 0x03,   /**< Fast TX ready */
    CC110X_STATE_CALIBRATE          = 0x04,   /**< Device is calibrating */
    CC110X_STATE_SETTLING           = 0x05,   /**< PLL is settling */
    CC110X_STATE_RXFIFO_OVERFLOW    = 0x06,   /**< RX FIFO overflown */
    CC110X_STATE_TXFIFO_UNDERFLOW   = 0x07,   /**< TX FIFO underflown */
} cc110x_state_t;

/**
 * @brief Enumeration over the possible TX power settings the driver offers
 */
typedef enum {
    CC110X_TX_POWER_MINUS_30_DBM,                   /**< -30 dBm */
    CC110X_TX_POWER_MINUS_20_DBM,                   /**< -20 dBm */
    CC110X_TX_POWER_MINUS_15_DBM,                   /**< -15 dBm */
    CC110X_TX_POWER_MINUS_10_DBM,                   /**< -10 dBm */
    CC110X_TX_POWER_0_DBM,                          /**< 0 dBm */
    CC110X_TX_POWER_PLUS_5_DBM,                     /**< 5 dBm */
    CC110X_TX_POWER_PLUS_7_DBM,                     /**< 7 dBm */
    CC110X_TX_POWER_PLUS_10_DBM,                    /**< 10 dBm */
    CC110X_TX_POWER_NUMOF,                          /**< Number of TX power options */
} cc110x_tx_power_t;

/**
 * @brief   Structure that holds the PATABLE, which allows to configure the
 *          8 available output power levels using a magic number for each level.
 *
 * See Section "24 Output Power Programming" on page 59ff in the data sheet.
 * The values suggested in Table 39 on page 60 in the data sheet are already
 * available by this driver, but will only be linked in (8 bytes of ROM size)
 * when they are referenced.
 *
 * @see cc110x_patable_433mhz
 * @see cc110x_patable_868mhz
 * @see cc110x_patable_915mhz
 */
typedef struct {
    uint8_t data[8]; /**< Magic number to store in the configuration register */
} cc110x_patable_t;

/**
 * @brief   Configuration of the transceiver to use
 *
 * @warning Two transceivers with different configurations will be unable
 *          to communicate.
 *
 * The data uploaded into configuration registers are stored in
 * @ref cc110x_conf. Most of them cannot be changed, as the driver relies on
 * their values. However, the base frequency, the symbol rate (which equals
 * the bit rate for the chosen modulation and error correction) and the
 * channel bandwidth can be configured using this data structure.
 *
 * Please note that while the CC1100/CC1101 chip is compatible with a huge
 * frequency range (300 MHz - 928 MHz), the complete circuit is optimized to
 * a narrow frequency band. So make sure the configured base frequency is within
 * that frequency band that is compatible with that circuit. (Most break out
 * board will operate at the 433 MHz band. In the EU the 868 MHz band would be
 * more interesting, but 433 MHz is license free as well. In the USA the 915 MHz
 * band is license free.
 *
 * Please verify that the driver is configured in a way that allows legal
 * operation according to rules and laws that apply for you.
 */
typedef struct {
    uint8_t base_freq[3]; /**< Base frequency to use */
    /**
     * @brief   FSCTRL1 configuration register value that affects the
     *          intermediate frequency of the transceiver to use
     * @note    The 4 most significant bits have to be 0.
     *
     * Assuming a 26 MHz crystal the IF is calculated as follows (in kHz):
     *
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * double intermediate_frequency = 26000 / 1024 * fsctrl1;
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    uint8_t fsctrl1;
    /**
     * @brief   MDMCFG4 configuration register value that affects channel filter
     *          bandwidth and the data rate
     *
     * See page 76 in the data sheet.
     *
     * Assuming a 26 MHz crystal the channel filter bandwidth is calculated
     * as follows (in kHz):
     *
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * uint8_t exponent = mdmcfg4 >> 6;
     * uint8_t mantissa = (mdmcfg4 >> 4) & 0x03;
     * double bandwidth = 26000.0 / (8 * (4 + mantissa) * (1L << exponent));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    uint8_t mdmcfg4;
    /**
     * @brief   MDMCFG3 configuration register value that affects the data rate
     *
     * @see     cc110x_config_t::mdmcfg4
     *
     * See page 76 in the data sheet.
     *
     * Assuming a 26 MHz crystal the symbol rate of the transceiver is calculated
     * as follows (in kBaud):
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * uint8_t exponent = mdmcfg4 & 0x0f;
     * int32_t mantissa = mdmcfg3;
     * double baudrate = (256 + mantissa) * 26000.0 / (1L << (28 - exponent));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    uint8_t mdmcfg3;
    /**
     * @brief   DEVIANT configuration register that affects the amount by which
     *          the radio frequency is shifted in FSK/GFSK modulation
     *
     * @see cc110x_config_t::mdmcfg4
     *
     * See page 79 in the data sheet.
     *
     * In an ideal world the channel bandwidth would be twice the channel
     * deviation. In the real world the used channel bandwidth is higher.
     * Assuming a 26 MHz crystal and GFSK modulation (the driver will configure
     * the transceiver to use GFSK) the deviation
     *
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * uint8_t exponent = (deviatn >> 4) & 0x07;
     * int32_t mantissa = deviatn & 0x07;
     * double deviation = (8 + mantissa) * 26000.0 / (1L << (17 - exponent));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *
     * For reliable operation at high symbol rates, the deviation has to be
     * increased as well.
     */
    uint8_t deviatn;
} cc110x_config_t;

/**
 * @brief   Structure to hold mapping between virtual and physical channel numbers
 *
 * This driver will provide "virtual" channel numbers 0 to 7, which will be
 * translated to "physical" channel numbers before being send to the
 * transceiver. This is used to overcome the following limitations:
 *
 * - The transceiver does not support channel maps with varying distance between
 *   channels. However, e.g. the LoRa channels 10 - 16 in the 868 MHz band have
 *   a distance of 300 kHz, but channel 16 and 17 have a distance of 1 MHz.
 * - The transceiver does not supports channel distances higher than 405.46 kHz.
 *
 * This mapping overcomes both limitations be using 50kHz physical channel
 * spacing and use the map to translate to the correct physical channel. This
 * also allows to keep the same MDMCFG1 and MDMCFG0 configuration register
 * values for all channel layouts. Finally, different channel sets can be
 * used by different groups of IoT device in the same environment to limit
 * collisions between those groups - assuming that enough non-overlapping
 * channels are available.
 *
 * The "virtual" channel (the channel number presented to RIOT) will be used
 * as index in @ref cc110x_chanmap_t::map, the value in there will give the
 * corresponding "physical" channel number, or 255 if this virtual channel
 * number is not available.
 */
typedef struct {
    uint8_t map[CC110X_MAX_CHANNELS]; /**< "Physical" channel numbers */
} cc110x_chanmap_t;

/**
 * @brief   Structure holding all parameter for driver initialization
 */
typedef struct {
    const cc110x_patable_t *patable;  /**< Pointer to the PATABLE to use */
    /**
     * @brief   Pointer to the configuration of the base frequency, data rate
     *          and channel bandwidth; or `NULL` to keep the default.
     */
    const cc110x_config_t *config;
    const cc110x_chanmap_t *channels;   /**< Pointer to the default channel map */
    spi_t spi;                          /**< SPI bus connected to the device */
    spi_clk_t spi_clk;                  /**< SPI clock to use (max 6.5 MHz) */
    spi_cs_t cs;                        /**< GPIO pin connected to chip select */
    gpio_t gdo0;                        /**< GPIO pin connected to GDO0 */
    gpio_t gdo2;                        /**< GPIO pin connected to GDO2 */
} cc110x_params_t;

/**
 * @brief   Structure holding the calibration data of the frequency synthesizer
 */
typedef struct {
    /**
     * @brief   VCO capacitance calibration, which depends on the frequency and,
     *          thus, has to be stored for each channel
     */
    char fscal1[CC110X_MAX_CHANNELS];
    char fscal2;    /**< VCO current calibration, independent of channel */
    char fscal3;    /**< charge pump current calibration, independent of channel */
} cc110x_fs_calibration_t;

/**
 * @brief   Buffer to temporary store incoming/outgoing packet
 *
 * The CC1100/CC1101 transceiver's FIFO sadly is only 64 bytes in size. To
 * support frames bigger than that, chunks of the frame have to be
 * transferred between the MCU and the CC1100/CC1101 transceiver while the
 * frame is in transit.
 */
typedef struct __attribute__((packed)) {
    uint8_t len;            /**< Length of the frame in bytes */
    /**
     * @brief   The payload data of the frame
     */
    uint8_t data[CC110X_MAX_FRAME_SIZE];
    /**
     * @brief   Index of the next @ref cc110x_framebuf_t::data element to
     *          transfer
     *
     * In RX mode: Index of the next @ref cc110x_framebuf_t::data element to
     * store data read from the RX-FIFO into.
     *
     * In TX mode: Index of the next @ref cc110x_framebuf_t::data element to
     * write to the TX-FIFO.
     */
    uint8_t pos;
} cc110x_framebuf_t;

/**
 * @brief   Device descriptor for CC1100/CC1101 transceivers
 */
typedef struct {
    netdev_t netdev;                    /**< RIOT's interface to this driver */
    uint8_t addr;                       /**< Layer 2 address of this device */
    /* Keep above in sync with cc1xx_t members, as they must overlap! */
    cc110x_state_t state;               /**< State of the transceiver */
    cc110x_tx_power_t tx_power;         /**< TX power of the receiver */
    uint8_t channel;                    /**< Currently tuned (virtual) channel */
    /* Struct packing:  addr, state, tx_power and channel add up to 32 bit */
    const cc110x_chanmap_t *channels;   /**< Pointer to the channel map to use. */
    cc110x_params_t params;             /**< Configuration of the driver */
    cc110x_framebuf_t buf;              /**< Temporary frame buffer */
    /**
     * @brief   RSSI and LQI of the last received frame
     */
    cc1xxx_rx_info_t rx_info;
    /**
     * @brief   Frequency synthesizer calibration data
     */
    cc110x_fs_calibration_t fscal;
    /**
     * @brief   Use mutex to block during TX and unblock from ISR when ISR
     *          needs to be handled from thread-context
     *
     * Blocking during TX within the driver prevents the upper layers from
     * calling @ref netdev_driver_t::send while already transmitting a frame.
     */
    mutex_t isr_signal;
    uint8_t rssi_offset;                /**< dBm to subtract from raw RSSI data */
} cc110x_t;

/**
 * @brief   Setup the CC1100/CC1101 driver, but perform no initialization
 *
 * @ref netdev_driver_t::init can be used after this call to initialize the
 * transceiver.
 *
 * @param   dev     Device descriptor to use
 * @param   params  Parameter of the device to setup
 * @param   index   Index of @p params in a global parameter struct array.
 *                  If initialized manually, pass a unique identifier instead.
 *
 * @retval  0       Device successfully set up
 * @retval  -EINVAL @p dev or @p params is `NULL`, or @p params is invalid
 */
int cc110x_setup(cc110x_t *dev, const cc110x_params_t *params, uint8_t index);

/**
 * @brief Apply the given configuration and the given channel map and performs
 *        a recalibration
 *
 * @param   dev     Device descriptor of the transceiver
 * @param   conf    Configuration to apply or `NULL` to only change channel map
 * @param   chanmap Channel map to apply (must be compatible with @p conf)
 * @param   channel The channel to tune in after applying the config
 *
 * @retval  0       Success
 * @retval  -EINVAL Called with invalid argument
 * @retval  -EIO    Communication with the transceiver failed
 * @retval  -ERANGE Channel out of range or not supported by channel map
 *
 * @pre     The application developer checked in the documentation that the
 *          channel map in @p chanmap is compatible with the configuration in
 *          @p conf
 *
 * Because the configuration (potentially) changes the channel bandwidth, the
 * old channel map is rendered invalid. This API therefore asks for both to make
 * sure an application developer does not forget to update the channel map.
 * Because the old calibration data is also rendered invalid,
 * @ref cc110x_full_calibration is called to update it.
 */
int cc110x_apply_config(cc110x_t *dev, const cc110x_config_t *conf,
                        const cc110x_chanmap_t *chanmap, uint8_t channel);

/**
 * @brief   Perform a calibration of the frequency generator for each supported
 *          channel
 *
 * @param   dev     Device descriptor of the transceiver
 *
 * @retval  0       Success
 * @retval  -EINVAL Called with invalid argument
 * @retval  -EAGAIN Current state prevents deliberate calibration
 * @retval  -EIO    Communication with the transceiver failed
 *
 * Tunes in each supported channel and calibrates the transceiver. The
 * calibration data is stored so that @ref cc110x_set_channel can skip the
 * calibration phase and use the stored calibration data instead.
 */
int cc110x_full_calibration(cc110x_t *dev);

/**
 * @brief Hops to the specified channel
 *
 * @param   dev     Device descriptor of the transceiver
 * @param   channel Channel to hop to
 *
 * @retval  0       Success
 * @retval  -EINVAL Called with `NULL` as @p dev
 * @retval  -ERANGE Channel out of range or not supported by channel map
 * @retval  -EAGAIN Currently in a state that does not allow hopping, e.g.
 *                  sending/receiving a packet, calibrating or handling
 *                  transmission errors
 * @retval  -EIO    Communication with the transceiver failed
 *
 * This function implements the fact channel hopping approach outlined in
 * section 28.2 on page 64 in the data sheet, which skips the calibration phase
 * by storing the calibration date for each channel in the driver.
 */
int cc110x_set_channel(cc110x_t *dev, uint8_t channel);

/**
 * @brief   Set the TX power to the specified value
 *
 * @param   dev     Device descriptor of the transceiver
 * @param   power   Output power to apply
 *
 * @retval  0       Success
 * @retval  -EINVAL Called with `NULL` as @p dev
 * @retval  -ERANGE Called with an invalid value for @p power
 * @retval  -EAGAIN Changing the TX power is in the current state not possible
 * @retval  -EIO    Communication with the transceiver failed
 */
int cc110x_set_tx_power(cc110x_t *dev, cc110x_tx_power_t power);

/**
 * @brief   Wakes the transceiver from SLEEP mode and enters RX mode
 *
 * @retval  0       Success
 * @retval  -EIO    Communication with the transceiver failed
 */
int cc110x_wakeup(cc110x_t *dev);

/**
 * @brief   Sets the transceiver into SLEEP mode.
 *
 * Only @ref cc110x_wakeup can awake the device again.
 */
void cc110x_sleep(cc110x_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
