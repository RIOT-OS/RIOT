/*
 * Copyright (C) 2015-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ieee802154_header IEEE 802.15.4 frame headers and definitions
 * @ingroup     net_ieee802154
 * @brief       IEEE 802.15.4 header definitions and utility functions
 * @{
 *
 * @file
 * @brief       IEEE 802.15.4 header definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_IEEE802154_H
#define NET_IEEE802154_H

#include <stdint.h>
#include <stdlib.h>

#include "byteorder.h"
#include "modules.h"
#include "net/eui64.h"
#include "time_units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default start frame delimiter
 */
#define IEEE802154_SFD                      (0xa7)

/**
 * @brief IEEE 802.15.4 address lengths
 * @{
 */
#define IEEE802154_SHORT_ADDRESS_LEN        (2U)    /**< short (16-bit) address */
#define IEEE802154_LONG_ADDRESS_LEN         (8U)    /**< long address (EUI-64) */
/**
 * @}
 */

/**
 * @brief IEEE802.15.4 FCF field definitions
 * @{
 */
#define IEEE802154_MAX_HDR_LEN              (23U)
#define IEEE802154_MIN_FRAME_LEN            (IEEE802154_FCF_LEN + sizeof(uint8_t))

#define IEEE802154_FCF_LEN                  (2U)
#define IEEE802154_FCS_LEN                  (2U)

#define IEEE802154_FCF_TYPE_MASK            (0x07)
#define IEEE802154_FCF_TYPE_BEACON          (0x00)
#define IEEE802154_FCF_TYPE_DATA            (0x01)
#define IEEE802154_FCF_TYPE_ACK             (0x02)
#define IEEE802154_FCF_TYPE_MACCMD          (0x03)

#define IEEE802154_FCF_SECURITY_EN          (0x08)  /**< enable security */
#define IEEE802154_FCF_FRAME_PEND           (0x10)  /**< follow-up frame is pending */
#define IEEE802154_FCF_ACK_REQ              (0x20)  /**< acknowledgement requested from receiver */
#define IEEE802154_FCF_PAN_COMP             (0x40)  /**< compress source PAN ID */

#define IEEE802154_FCF_DST_ADDR_MASK        (0x0c)
#define IEEE802154_FCF_DST_ADDR_VOID        (0x00)  /**< no destination address */
#define IEEE802154_FCF_DST_ADDR_RESV        (0x04)  /**< reserved address mode */
#define IEEE802154_FCF_DST_ADDR_SHORT       (0x08)  /**< destination address length is 2 */
#define IEEE802154_FCF_DST_ADDR_LONG        (0x0c)  /**< destination address length is 8 */

#define IEEE802154_FCF_VERS_MASK            (0x30)
#define IEEE802154_FCF_VERS_V0              (0x00)
#define IEEE802154_FCF_VERS_V1              (0x10)

#define IEEE802154_FCF_SRC_ADDR_MASK        (0xc0)
#define IEEE802154_FCF_SRC_ADDR_VOID        (0x00)  /**< no source address */
#define IEEE802154_FCF_SRC_ADDR_RESV        (0x40)  /**< reserved address mode */
#define IEEE802154_FCF_SRC_ADDR_SHORT       (0x80)  /**< source address length is 2 */
#define IEEE802154_FCF_SRC_ADDR_LONG        (0xc0)  /**< source address length is 8 */
/** @} */

/**
 * @brief   Channel ranges
 * @{
 */
#define IEEE802154_CHANNEL_MIN_SUBGHZ   (0U)    /**< Minimum channel for sub-GHz band */
#define IEEE802154_CHANNEL_MAX_SUBGHZ   (10U)   /**< Maximum channel for sub-GHz band */
#define IEEE802154_CHANNEL_MIN          (11U)   /**< Minimum channel for 2.4 GHz band */
#define IEEE802154_CHANNEL_MAX          (26U)   /**< Maximum channel for 2.4 GHz band */
/** @} */

#define IEEE802154_FRAME_LEN_MAX        (127U)  /**< maximum 802.15.4 frame length */
#define IEEE802154G_FRAME_LEN_MAX      (2047U)  /**< maximum 802.15.4g-2012 frame length */
#define IEEE802154_ACK_FRAME_LEN          (5U)  /**< ACK frame length */

/**
 * @brief Number of symbols to wait during Long Inter Frame Spacing
 */
#define IEEE802154_LIFS_SYMS            (40U)

/**
 * @brief Number of symbols to wait during Short Inter Frame Spacing
 */
#define IEEE802154_SIFS_SYMS            (12U)

/**
 * @brief Maximum frame size to consider a frame as short.
 */
#define IEEE802154_SIFS_MAX_FRAME_SIZE  (18U)

/**
 * @brief ACK Timeout period in symbols
 */
#define IEEE802154_ACK_TIMEOUT_SYMS     (54)

/**
 * @brief Symbol time for IEEE 802.15.4 MR-OFDM in µs
 */
#define IEEE802154_MR_OFDM_SYMBOL_TIME_US   (120)

/**
 * @brief Symbol time for IEEE 802.15.4 MR-FSK in µs
 *
 * symbol time is always 20 µs for MR-FSK (table 0, pg. 7)
 */
#define IEEE802154_MR_FSK_SYMBOL_TIME_US    (20)

/**
 * @brief value of measured power when RSSI is zero.
 *
 * This value is defined in the IEEE 802.15.4 standard
 */
#define IEEE802154_RADIO_RSSI_OFFSET        (-174)

#define IEEE802154_PHY_MR_FSK_PHR_LEN      (2)  /**< MR-FSK PHY header length */
#define IEEE802154_PHY_MR_FSK_2FSK_SFD_LEN (2)  /**< MR-FSK SFD length on Filtered 2-FSK */

/**
 * For the MR-FSK PHY, the SFD value when PHR + PSDU are coded/uncoded and with
 * phyMRFSKSFD = 0 or 1 respectively
 *
 * 802.15.4g, Table 131 (p. 51)
 *
 *  @{
 */
#define IEEE802154_PHY_MR_FSK_2FSK_CODED_SFD_0      (0x6F4E)
#define IEEE802154_PHY_MR_FSK_2FSK_CODED_SFD_1      (0x632D)
#define IEEE802154_PHY_MR_FSK_2FSK_UNCODED_SFD_0    (0x90E4)
#define IEEE802154_PHY_MR_FSK_2FSK_UNCODED_SFD_1    (0x7A0E)
/** @} */

/**
 * For the SUN PHYs, the value is 1 ms expressed in symbol periods, rounded
 * up to the next integer number of symbol periods using the ceiling() function.
 *
 * 802.15.4g, Table 70 (p. 43)
 */
#define IEEE802154G_ATURNAROUNDTIME_US          (1 * US_PER_MS)

/**
 * IEEE Std 802.15.4-2020
 * Table 11-1—PHY constants: The value is 12 for all other PHYs.
 */
#define IEEE802154_ATURNAROUNDTIME_IN_SYMBOLS   (12)

/**
 * IEEE Std 802.15.4-2020
 * Table 11-1—PHY constants: For all other PHYs¹, the duration of
 * 8 symbol periods.
 *
 * [1] all but MR-O-QPSK
 */
#define IEEE802154_CCA_DURATION_IN_SYMBOLS      (8)

/**
 * @brief   802.15.4 PHY modes
 */
typedef enum {
    IEEE802154_PHY_DISABLED,        /**< PHY disabled, no mode selected */
    IEEE802154_PHY_BPSK,            /**< Binary Phase Shift Keying */
    IEEE802154_PHY_ASK,             /**< Amplitude-Shift Keying */
    IEEE802154_PHY_OQPSK,           /**< Offset Quadrature Phase-Shift Keying */
    IEEE802154_PHY_MR_OQPSK,        /**< Multi-Rate Offset Quadrature Phase-Shift Keying */
    IEEE802154_PHY_MR_OFDM,         /**< Multi-Rate Orthogonal Frequency-Division Multiplexing */
    IEEE802154_PHY_MR_FSK,          /**< Multi-Rate Frequency Shift Keying */

    IEEE802154_PHY_NO_OP,           /**< don't change PHY configuration */
} ieee802154_phy_mode_t;

/**
 * @brief   802.15.4 forward error correction schemes
 */
typedef enum {
    IEEE802154_FEC_NONE,            /**< no forward error correction */
    IEEE802154_FEC_NRNSC,           /**< non-recursive and non-systematic code */
    IEEE802154_FEC_RSC              /**< recursive and systematic code */
} ieee802154_mr_fsk_fec_t;

/**
 * @brief   802.15.4 MR-FSK symbol rates
 */
typedef enum {
    IEEE802154_MR_FSK_SRATE_50K,    /**< 50k Symbols/s  */
    IEEE802154_MR_FSK_SRATE_100K,   /**< 100k Symbols/s */
    IEEE802154_MR_FSK_SRATE_150K,   /**< 150k Symbols/s */
    IEEE802154_MR_FSK_SRATE_200K,   /**< 200k Symbols/s */
    IEEE802154_MR_FSK_SRATE_300K,   /**< 300k Symbols/s */
    IEEE802154_MR_FSK_SRATE_400K,   /**< 400k Symbols/s */
} ieee802154_mr_fsk_srate_t;

/**
 * @brief   802.15.4 MR-OQPSK chip rates
 */
typedef enum {
    IEEE802154_MR_OQPSK_CHIPS_100,  /**< 100 kChip/s  */
    IEEE802154_MR_OQPSK_CHIPS_200,  /**< 200 kChip/s  */
    IEEE802154_MR_OQPSK_CHIPS_1000, /**< 1000 kChip/s */
    IEEE802154_MR_OQPSK_CHIPS_2000, /**< 2000 kChip/s */
} ieee802154_mr_oqpsk_chips_t;

/**
 * @brief Get the minimum preamble length for a given symbol rate
 *
 * From IEEE 802.15.4g Table 6-64
 *
 * @param[in] srate     symbol rate
 * @return    preamble length in bytes
 */
static inline uint8_t ieee802154_mr_fsk_plen(ieee802154_mr_fsk_srate_t srate)
{
    switch (srate) {
    case IEEE802154_MR_FSK_SRATE_50K:
        return 2;
    case IEEE802154_MR_FSK_SRATE_100K:
        return 3;
    case IEEE802154_MR_FSK_SRATE_150K:
    case IEEE802154_MR_FSK_SRATE_200K:
    case IEEE802154_MR_FSK_SRATE_300K:
        return 8;
    case IEEE802154_MR_FSK_SRATE_400K:
        return 10;
    }

    return 0;
}

/**
 * @brief   Special address definitions
 * @{
 */
/**
 * @brief   Static initializer for broadcast address
 */
#define IEEE802154_ADDR_BCAST               { 0xff, 0xff }

/**
 * @brief   Length in byte of @ref IEEE802154_ADDR_BCAST
 */
#define IEEE802154_ADDR_BCAST_LEN           (IEEE802154_SHORT_ADDRESS_LEN)

/**
 * @brief   Broadcast address
 */
extern const uint8_t ieee802154_addr_bcast[IEEE802154_ADDR_BCAST_LEN];
/** @} */

/**
 * @defgroup net_ieee802154_conf    IEEE802.15.4 compile configurations
 * @ingroup  config
 * @{
 */
/**
 * @brief IEEE802.15.4 default PHY mode
 * @{
 */
#if IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_PHY_BPSK)
#define CONFIG_IEEE802154_DEFAULT_PHY_MODE          IEEE802154_PHY_BPSK
#elif IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_PHY_ASK)
#define CONFIG_IEEE802154_DEFAULT_PHY_MODE          IEEE802154_PHY_ASK
#elif IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_PHY_OQPSK)
#define CONFIG_IEEE802154_DEFAULT_PHY_MODE          IEEE802154_PHY_OQPSK
#elif IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_PHY_MR_OQPSK)
#define CONFIG_IEEE802154_DEFAULT_PHY_MODE          IEEE802154_PHY_MR_OQPSK
#elif IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_PHY_MR_OFDM)
#define CONFIG_IEEE802154_DEFAULT_PHY_MODE          IEEE802154_PHY_MR_OFDM
#elif IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_PHY_MR_FSK)
#define CONFIG_IEEE802154_DEFAULT_PHY_MODE          IEEE802154_PHY_MR_FSK
#endif

#ifndef CONFIG_IEEE802154_DEFAULT_PHY_MODE
#define CONFIG_IEEE802154_DEFAULT_PHY_MODE          IEEE802154_PHY_OQPSK
#endif
/** @} */
/**
 * @brief IEEE802.15.4 default sub-GHZ channel
 */
#ifndef CONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL
#define CONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL   (5U)
#endif

/**
 * @brief IEEE802.15.4 default channel
 */
#ifndef CONFIG_IEEE802154_DEFAULT_CHANNEL
#define CONFIG_IEEE802154_DEFAULT_CHANNEL          (26U)
#endif

/**
 * @brief IEEE802.15.4 default sub-GHZ page
 */
#ifndef CONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE
#define CONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE      (2U)
#endif

/**
 * @brief IEEE802.15.4 MR-OQPSK default chip rate
 */
#ifndef CONFIG_IEEE802154_MR_OQPSK_DEFAULT_CHIPS
#define CONFIG_IEEE802154_MR_OQPSK_DEFAULT_CHIPS    IEEE802154_MR_OQPSK_CHIPS_1000
#endif

/**
 * @brief IEEE802.15.4 MR-OQPSK default rate mode
 */
#ifndef CONFIG_IEEE802154_MR_OQPSK_DEFAULT_RATE
#define CONFIG_IEEE802154_MR_OQPSK_DEFAULT_RATE     (2U)
#endif

/**
 * @brief IEEE802.15.4 MR-OFDM default modulation option
 */
#ifndef CONFIG_IEEE802154_MR_OFDM_DEFAULT_OPTION
#define CONFIG_IEEE802154_MR_OFDM_DEFAULT_OPTION    (2U)
#endif

/**
 * @brief IEEE802.15.4 MR-OFDM default Modulation & Coding Scheme
 */
#ifndef CONFIG_IEEE802154_MR_OFDM_DEFAULT_SCHEME
#define CONFIG_IEEE802154_MR_OFDM_DEFAULT_SCHEME    (2U)
#endif

/**
 * @brief IEEE802.15.4 MR-FSK default symbol rate
 */
#ifndef CONFIG_IEEE802154_MR_FSK_DEFAULT_SRATE
#define CONFIG_IEEE802154_MR_FSK_DEFAULT_SRATE      IEEE802154_MR_FSK_SRATE_200K
#endif

/**
 * @brief IEEE802.15.4 MR-FSK default modulation index, fraction of 64
 */
#ifndef CONFIG_IEEE802154_MR_FSK_DEFAULT_MOD_IDX
#define CONFIG_IEEE802154_MR_FSK_DEFAULT_MOD_IDX    (64U)
#endif

/**
 * @brief IEEE802.15.4 MR-FSK default modulation order
 */
#ifndef CONFIG_IEEE802154_MR_FSK_DEFAULT_MOD_ORD
#define CONFIG_IEEE802154_MR_FSK_DEFAULT_MOD_ORD    (2U)
#endif

/**
 * @brief IEEE802.15.4 MR-FSK default error correction mode
 */
#ifndef CONFIG_IEEE802154_MR_FSK_DEFAULT_FEC
#define CONFIG_IEEE802154_MR_FSK_DEFAULT_FEC        IEEE802154_FEC_NONE
#endif

/**
 * @brief IEEE802.15.4 default PANID
 */
#ifndef CONFIG_IEEE802154_DEFAULT_PANID
#define CONFIG_IEEE802154_DEFAULT_PANID            (0x0023U)
#endif

/**
 * @brief IEEE802.15.4 Broadcast PANID
 */
#ifndef IEEE802154_PANID_BCAST
#define IEEE802154_PANID_BCAST              { 0xff, 0xff }
#endif

/**
 * @brief IEEE802.15.4 default TX power (in dBm)
 */
#ifndef CONFIG_IEEE802154_DEFAULT_TXPOWER
#define CONFIG_IEEE802154_DEFAULT_TXPOWER          (0)
#endif
/** @} */

/**
 * @brief IEEE802.15.4 default value for minimum backoff exponent
 */
#ifndef CONFIG_IEEE802154_DEFAULT_CSMA_CA_MIN_BE
#define CONFIG_IEEE802154_DEFAULT_CSMA_CA_MIN_BE   (3U)
#endif

/**
 * @brief IEEE802.15.4 default value for maximum number of CSMA-CA retries.
 */
#ifndef CONFIG_IEEE802154_DEFAULT_CSMA_CA_RETRIES
#define CONFIG_IEEE802154_DEFAULT_CSMA_CA_RETRIES  (4U)
#endif

/**
 * @brief IEEE802.15.4 default value for maximum backoff exponent
 */
#ifndef CONFIG_IEEE802154_DEFAULT_CSMA_CA_MAX_BE
#define CONFIG_IEEE802154_DEFAULT_CSMA_CA_MAX_BE   (5U)
#endif

/**
 * @brief IEEE802.15.4 default value for CCA threshold (in dBm)
 */
#ifndef CONFIG_IEEE802154_CCA_THRESH_DEFAULT
#define CONFIG_IEEE802154_CCA_THRESH_DEFAULT       (-70)
#endif

/**
 * @brief IEEE802.15.4 default value for maximum frame retries.
 */
#ifndef CONFIG_IEEE802154_DEFAULT_MAX_FRAME_RETRANS
#define CONFIG_IEEE802154_DEFAULT_MAX_FRAME_RETRANS     (4U)
#endif

/**
 * @brief Disable Auto ACK support
 */
#ifdef DOXYGEN
#define CONFIG_IEEE802154_AUTO_ACK_DISABLE 0
#endif

/**
 * @brief Request ACKs by default
 */
#ifndef CONFIG_IEEE802154_DEFAULT_ACK_REQ
#define CONFIG_IEEE802154_DEFAULT_ACK_REQ          1
#endif

/**
 * @brief   Enable DSME CAP reduction
 *
 * @note    For the moment, this is only valid for @ref pkg_opendsme
 */
#ifndef CONFIG_IEEE802154_DSME_CAP_REDUCTION
#define CONFIG_IEEE802154_DSME_CAP_REDUCTION             0
#endif

/**
 * @brief   Set the maximum DSME MAC response wait time
 *
 * This configuration sets the maximum wait times for MAC commands (association,
 * DSME GTS allocation, etc).
 * The unit is "base superframe duration" (60 * symbol_time_us).
 *
 * @note    For the moment, this is only valid for @ref pkg_opendsme
 */
#ifndef CONFIG_IEEE802154_DSME_MAC_RESPONSE_WAIT_TIME
#define CONFIG_IEEE802154_DSME_MAC_RESPONSE_WAIT_TIME    (244U)
#endif

/**
 * @brief   Set the scan duration
 *
 * Set the scan duration for each channel to `60 * symbol_time_us *
 * (2^scanDuration + 1)`
 *
 * @note    For the moment, this is only valid for @ref pkg_opendsme
 */
#ifndef CONFIG_IEEE802154_DSME_SCAN_DURATION
#define CONFIG_IEEE802154_DSME_SCAN_DURATION    (4U)
#endif

/**
 * @brief   Set IEEE 802.15.4 DSME Superframe Order (SO)
 * The SO sets the slot duration to `60 * symbol_time_us * 2^SO` usecs.
 * E.g for O-QPSK an SO=3 renders a slot duration of `60 * 16 * 8` = 7.6 ms and
 * a superframe duration of 122.88 ms.
 *
 * Settings this value to 3 allows to transmit full IEEE 802.15.4 O-QPSK frames
 * (127 bytes).
 *
 * @note    For the moment, this is only valid for @ref pkg_opendsme
 */
#ifndef CONFIG_IEEE802154_DSME_SUPERFRAME_ORDER
#define CONFIG_IEEE802154_DSME_SUPERFRAME_ORDER      (3U)
#endif

/**
 * @brief   Set IEEE 802.15.4 DSME Multisuperframe Order (MO)
 *
 * The MO sets the number of superframes per multisuperframe to `2^(MO-SO)`.
 *
 * @see @ref CONFIG_IEEE802154_DSME_SUPERFRAME_ORDER
 *
 * @note    For the moment, this is only valid for @ref pkg_opendsme
 */
#ifndef CONFIG_IEEE802154_DSME_MULTISUPERFRAME_ORDER
#define CONFIG_IEEE802154_DSME_MULTISUPERFRAME_ORDER (3U)
#endif

/**
 * @brief   Set IEEE 802.15.4 DSME Beacon Order (BO)
 *
 * The BO sets the beacon interval to `2^(BO-SO)` superframes.
 *
 * @see @ref CONFIG_IEEE802154_DSME_SUPERFRAME_ORDER
 *
 * @note    For the moment, this is only valid for @ref pkg_opendsme
 */
#ifndef CONFIG_IEEE802154_DSME_BEACON_ORDER
#define CONFIG_IEEE802154_DSME_BEACON_ORDER          (3U)
#endif

/**
 * @brief   Use static GTS allocation
 *
 * When set, the MAC implementation will not use scheduling functions.
 * This requires that the upper layer allocates slots manually using @ref NETOPT_GTS_ALLOC.
 *
 * @see @ref CONFIG_IEEE802154_DSME_STATIC_GTS
 *
 * @note    For the moment, this is only valid for @ref pkg_opendsme
 */
#ifdef DOXYGEN
#define CONFIG_IEEE802154_DSME_STATIC_GTS            0
#endif

/**
 * @brief   Set expiration time of DSME GTS slot
 *
 * Sets the expiration time of DSME GTS slot (in number of idle slots). If DSME
 * detects no activity, it will deallocate the GTS slot.
 *
 * @note    For the moment, this is only valid for @ref pkg_opendsme
 */
#ifndef CONFIG_IEEE802154_DSME_GTS_EXPIRATION
#define CONFIG_IEEE802154_DSME_GTS_EXPIRATION         (16U)
#endif

/**
 * @brief   Set the minimum LQI to consider a beacon from a coordinator valid.
 *
 * @note    For the moment, this is only valid for @ref pkg_opendsme
 */
#ifndef CONFIG_IEEE802154_DSME_MIN_COORD_LQI
#define CONFIG_IEEE802154_DSME_MIN_COORD_LQI         (100U)
#endif

/**
 * @brief   Initializes an IEEE 802.15.4 MAC frame header in @p buf.
 *
 * @pre Resulting header must fit in memory allocated at @p buf.
 *
 * @see IEEE Std 802.15.4-2011, 5.2.1 General MAC frame format.
 *
 * If @p dst is NULL the IEEE802154_FCF_ACK_REQ will be unset to prevent
 * flooding the network.
 *
 * @param[out] buf      Target memory for frame header.
 * @param[in] src       Source address for frame in network byteorder.
 *                      May be NULL if @ref IEEE802154_FCF_SRC_ADDR_VOID is set
 *                      in @p flags.
 * @param[in] src_len   Length of @p src. Legal values are:
 *                      * 0 (will set @ref IEEE802154_FCF_SRC_ADDR_VOID in MHR)
 *                      * 2 (will set @ref IEEE802154_FCF_SRC_ADDR_SHORT in MHR)
 *                      * 8 (will set @ref IEEE802154_FCF_SRC_ADDR_LONG in MHR)
 * @param[in] dst       Destination address for frame in network byteorder.
 *                      May be NULL if @ref IEEE802154_FCF_SRC_ADDR_VOID is set
 *                      in @p flags.
 * @param[in] dst_len   Length of @p dst. Legal values are:
 *                      * 0 (will set @ref IEEE802154_FCF_DST_ADDR_VOID in MHR)
 *                      * 2 (will set @ref IEEE802154_FCF_DST_ADDR_SHORT in MHR)
 *                      * 8 (will set @ref IEEE802154_FCF_DST_ADDR_LONG in MHR)
 * @param[in] src_pan   Source PAN ID in little-endian. May be 0 if
 *                      @ref IEEE802154_FCF_PAN_COMP is set in @p flags.
 *                      Otherwise, it will be ignored, when
 *                      @ref IEEE802154_FCF_PAN_COMP is set.
 * @param[in] dst_pan   Destination PAN ID in little-endian.
 * @param[in] flags     Flags for the frame. These are interchangeable with the
 *                      first byte of the IEEE 802.15.4 FCF. This means that
 *                      it encompasses the type values,
 *                      @ref IEEE802154_FCF_SECURITY_EN,
 *                      @ref IEEE802154_FCF_FRAME_PEND, and
 *                      @ref IEEE802154_FCF_ACK_REQ.
 * @param[in] seq       Sequence number for frame.
 *
 * The version field in the FCF will be set implicitly to version 1.
 *
 * @return  Size of frame header on success.
 * @return  0, on error (flags set to unexpected state).
 */
size_t ieee802154_set_frame_hdr(uint8_t *buf, const uint8_t *src, size_t src_len,
                                const uint8_t *dst, size_t dst_len,
                                le_uint16_t src_pan, le_uint16_t dst_pan,
                                uint8_t flags, uint8_t seq);

/**
 * @brief   Get length of MAC header.
 *
 * @todo include security header implications
 *
 * @param[in] mhr   MAC header.
 *
 * @return  Length of MAC header on success.
 * @return  0, on error (source mode or destination mode set to reserved).
 */
size_t ieee802154_get_frame_hdr_len(const uint8_t *mhr);

/**
 * @brief   Gets source address from MAC header.
 *
 * @pre (@p src != NULL) && (@p src_pan != NULL)
 *
 * @param[in] mhr       MAC header.
 * @param[out] src      Source address in network byte order in MAC header.
 * @param[out] src_pan  Source PAN little-endian byte order in MAC header.
 *
 * @return   Length of source address.
 * @return  -EINVAL, if @p mhr contains unexpected flags.
 */
int ieee802154_get_src(const uint8_t *mhr, uint8_t *src, le_uint16_t *src_pan);

/**
 * @brief   Gets destination address from MAC header.
 *
 * @pre (@p dst != NULL) && (@p dst_pan != NULL)
 *
 * @param[in] mhr       MAC header.
 * @param[out] dst      Destination address in network byte order in MAC header.
 * @param[out] dst_pan  Destination PAN in little-endian byte order in MAC header.
 *
 * @return   Length of destination address.
 * @return  -EINVAL, if @p mhr contains unexpected flags.
 */
int ieee802154_get_dst(const uint8_t *mhr, uint8_t *dst, le_uint16_t *dst_pan);

/**
 * @brief  Check whether a frame pass the IEEE 802.15.4 frame filter.
 *
 * A frame passes the frame filter only if:
 * - The PAN ID matches the PAN ID of the frame filter or the broadcast PAN ID
 * - Either the Short or Extended Address matches the frame filter OR the
 *   Short Address is the broadcast address.
 *
 * @param[in] mhr           MAC header (PSDU)
 * @param[in] pan           PAN ID of the frame filter.
 * @param[in] short_addr    Short Address of the frame filter.
 * @param[in] ext_addr      Extended Address of the frame filter.
 *
 * @return 0            if frame passes the frame filter.
 * @return 1            if frame doesn't pass the frame filter.
 */
int ieee802154_dst_filter(const uint8_t *mhr, uint16_t pan,
                          network_uint16_t short_addr, const eui64_t *ext_addr);

/**
 * @brief   Gets sequence number from MAC header.
 *
 * @pre length of allocated space at @p mhr > 3
 *
 * @param[in] mhr   MAC header.
 *
 * @return  The sequence number in @p mhr.
 */
static inline uint8_t ieee802154_get_seq(const uint8_t *mhr)
{
    return mhr[2];
}

/**
 * @brief   Generates an IPv6 interface identifier from an IEEE 802.15.4 address.
 *
 * @pre (@p eui64 != NULL) && (@p addr != NULL)
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-6">
 *          RFC 4944, section 6
 *      </a>
 * @see <a href="https://tools.ietf.org/html/rfc6282#section-3.2.2">
 *          RFC 6282, section 3.2.2
 *      </a>
 *
 * @param[out] eui64    The resulting EUI-64.
 * @param[in] addr      An IEEE 802.15.4 address.
 * @param[in] addr_len  The length of @p addr. Must be 2 (short address),
 *                      4 (PAN ID + short address), or 8 (long address).
 *
 * @return Copy of @p eui64 on success.
 * @return NULL, if @p addr_len was of illegal length.
 */
static inline eui64_t *ieee802154_get_iid(eui64_t *eui64, const uint8_t *addr,
                                          size_t addr_len)
{
    int i = 0;

    eui64->uint8[0] = eui64->uint8[1] = 0;

    switch (addr_len) {
        case 8:
            eui64->uint8[0] = addr[i++] ^ 0x02;
            eui64->uint8[1] = addr[i++];
            eui64->uint8[2] = addr[i++];
            eui64->uint8[3] = addr[i++];
            eui64->uint8[4] = addr[i++];
            eui64->uint8[5] = addr[i++];
            eui64->uint8[6] = addr[i++];
            eui64->uint8[7] = addr[i++];
            break;

        case 4:
            eui64->uint8[0] = addr[i++] ^ 0x02;
            eui64->uint8[1] = addr[i++];

            /* Falls through. */
        case 2:
            eui64->uint8[2] = 0;
            eui64->uint8[3] = 0xff;
            eui64->uint8[4] = 0xfe;
            eui64->uint8[5] = 0;
            eui64->uint8[6] = addr[i++];
            eui64->uint8[7] = addr[i++];
            break;

        default:
            return NULL;
    }

    return eui64;
}

/**
 * @brief Convert from RSSI scale to dBm.
 *
 * The RSSI calculation is based on the IEEE 802.15.4 2020 specification and
 * it's represented as one octet of integer.
 *
 * RSSI equal to zero maps to -174 dBm and has the same scale as dBm (1 RSSI
 * step == 1 dBm). Therefore an RSSI value of 254 maps to +80 dBm.
 *
 * @note RSSI == 255 is defined as reserved by the standard and ignored by this
 * function.
 *
 * @param[in] rssi RF power in RSSI scale
 *
 * @return RF power in dBm scale
 */
static inline int16_t ieee802154_rssi_to_dbm(uint8_t rssi)
{
    return rssi + IEEE802154_RADIO_RSSI_OFFSET;
}

/**
 * @brief Convert from dBm scale to RSSI.
 *
 * The RSSI calculation is based on the IEEE 802.15.4 2020 specification and
 * it's represented as one octet of integer.
 *
 * RSSI equal to zero maps to -174 dBm and has the same scale as dBm (1 RSSI
 * step == 1 dBm). Therefore an RSSI value of 254 maps to +80 dBm.
 *
 * @param[in] dbm RF power in dBm scale.
 *
 * @return RF power in RSSI scale.
 */
static inline uint8_t ieee802154_dbm_to_rssi(int16_t dbm)
{
    const int min = IEEE802154_RADIO_RSSI_OFFSET;
    const int max = min + (UINT8_MAX - 1);

    int val = dbm <= min ? min : (dbm >= max ? max : dbm);
    return val - IEEE802154_RADIO_RSSI_OFFSET;
}

#ifdef __cplusplus
}
#endif

#endif /* NET_IEEE802154_H */
/** @} */
