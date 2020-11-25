/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_loramac LoRaMAC
 * @ingroup     net
 * @brief       LoRaMAC definitions
 *
 * @{
 *
 * @file
 * @brief       LoRaMAC header definitions
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef NET_LORAMAC_H
#define NET_LORAMAC_H

#include <stdint.h>
#include <stdlib.h>

#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_loramac_conf   LoRaMAC compile configurations
 * @ingroup config
 * @{
 */
/**
 * @brief   Default device EUI
 *
 * 8 bytes key, required for join procedure
 */
#ifndef CONFIG_LORAMAC_DEV_EUI_DEFAULT
#define CONFIG_LORAMAC_DEV_EUI_DEFAULT          "0000000000000000"
#endif

/**
 * @brief   Default application EUI
 *
 * 8 bytes key, required for join procedure
 */
#ifndef CONFIG_LORAMAC_APP_EUI_DEFAULT
#define CONFIG_LORAMAC_APP_EUI_DEFAULT          "0000000000000000"
#endif

/**
 * @brief   Default application key
 *
 * 16 bytes key, required for join procedure
 */
#ifndef CONFIG_LORAMAC_APP_KEY_DEFAULT
#define CONFIG_LORAMAC_APP_KEY_DEFAULT          "00000000000000000000000000000000"
#endif

/**
 * @brief   Default application session key
 *
 * 16 bytes key, only required for ABP join procedure type
 */
#ifndef CONFIG_LORAMAC_APP_SKEY_DEFAULT
#define CONFIG_LORAMAC_APP_SKEY_DEFAULT         "00000000000000000000000000000000"
#endif

/**
 * @brief   Default network session key
 *
 * 16 bytes key, only required for ABP join procedure type.
 */
#ifndef CONFIG_LORAMAC_NWK_SKEY_DEFAULT
#define CONFIG_LORAMAC_NWK_SKEY_DEFAULT         "00000000000000000000000000000000"
#endif

/**
 * @brief   Default device address
 */
#ifndef CONFIG_LORAMAC_DEV_ADDR_DEFAULT
#define CONFIG_LORAMAC_DEV_ADDR_DEFAULT         "00000000"
#endif

/**
 * @brief   Default LoRaWAN region
 */
#if !IS_ACTIVE(CONFIG_LORAMAC_REGION_EU_868) \
    && !IS_ACTIVE(CONFIG_LORAMAC_REGION_IN_865)
#define CONFIG_LORAMAC_REGION_EU_868           1
#endif

/**
 * @brief   Default device class (A, B or C)
 *
 * Configure the class of device. LoRaWAN supports three classes of operation
 * for end nodes namely Class A, B and C. All LoRaWAN devices are expected to
 * implement Class A, whereas Class B and Class C can be considered as
 * extensions to the specification of Class A devices.
 *
 * @see     [LoRaWAN Classes by TTN](https://www.thethingsnetwork.org/docs/lorawan/classes.html)
 *
 * @note    GNRC LoRaWAN only supports Class A
 */
#if IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DEVICE_CLASS_A)
#define CONFIG_LORAMAC_DEFAULT_DEVICE_CLASS     (LORAMAC_CLASS_A)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DEVICE_CLASS_B)
#define CONFIG_LORAMAC_DEFAULT_DEVICE_CLASS     (LORAMAC_CLASS_B)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DEVICE_CLASS_C)
#define CONFIG_LORAMAC_DEFAULT_DEVICE_CLASS     (LORAMAC_CLASS_C)
#endif

#ifndef CONFIG_LORAMAC_DEFAULT_DEVICE_CLASS
#define CONFIG_LORAMAC_DEFAULT_DEVICE_CLASS     (LORAMAC_CLASS_A)
#endif

/**
 * @brief   Default NetID (only valid with ABP join procedure)
 *
 * NETID is used by networks for assigning network-specific addresses to their
 * end-devices (i.e., DevAddr) so that uplink frames sent by those devices even
 * when they are roaming outside their home network can be forwarded to their
 * home network.
 */
#ifndef CONFIG_LORAMAC_DEFAULT_NETID
#define CONFIG_LORAMAC_DEFAULT_NETID            (1U)
#endif

/**
 * @brief Toggle network type (public or private)
 *
 * Set "1" to enable private network, set "0" to enable public network.
 * This configuration sets the sync word for LoRaWAN communication. This should
 * be in concordance with the gateway configuration. Public networks use `0x34`
 * while private networks use `0x12` as sync word.
 */
#ifdef DOXYGEN
#define CONFIG_LORAMAC_DEFAULT_PRIVATE_NETWORK
#endif

/**
 * @brief   Default network type (public or private)
 *
 * This configuration sets the sync word for LoRaWAN communication. This should
 * be in concordance with the gateway configuration. Public networks use `0x34`
 * while private networks use `0x12` as sync word.
 * @deprecated Use inverse @ref CONFIG_LORAMAC_DEFAULT_PRIVATE_NETWORK instead.
 * Will be removed after 2021.04 release.
 */
#ifndef LORAMAC_DEFAULT_PUBLIC_NETWORK
#if IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_PRIVATE_NETWORK)
#define LORAMAC_DEFAULT_PUBLIC_NETWORK          (false)
#else
#define LORAMAC_DEFAULT_PUBLIC_NETWORK          (true)
#endif
#endif

/**
 * @brief   Default datarate index
 *
 * Data rate combines two aspects, Bandwidth (BW) and Spreading Factor (SF). BW
 * depends on the region while SF contributes to the dwell time in any given
 * band which is limited by region. DR hence signifies difference combination on
 * BW and SF for different regions. Refer LoRaWAN 1.0.3 Regional Parameters for
 * more information.
 */
#if IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_0)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_0)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_1)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_1)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_2)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_2)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_3)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_3)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_4)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_4)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_5)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_5)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_6)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_6)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_7)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_7)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_8)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_8)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_9)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_9)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_10)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_10)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_11)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_11)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_12)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_12)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_13)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_13)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_14)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_14)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_DR_15)
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_15)
#endif

#ifndef CONFIG_LORAMAC_DEFAULT_DR
#define CONFIG_LORAMAC_DEFAULT_DR               (LORAMAC_DR_0)
#endif

/**
 * @brief   Default MAC TX power index
 *
 * TXPower index indicates power levels relative to the max EIRP level of the
 * end-device. Refer LoRaWAN 1.0.3 Regional Parameters for more information.
 */
#if IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_0)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_0)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_1)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_1)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_2)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_2)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_3)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_3)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_4)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_4)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_5)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_5)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_6)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_6)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_7)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_7)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_8)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_8)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_9)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_9)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_10)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_10)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_11)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_11)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_12)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_12)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_13)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_13)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_14)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_14)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_POWER_15)
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_15)
#endif

#ifndef CONFIG_LORAMAC_DEFAULT_TX_POWER
#define CONFIG_LORAMAC_DEFAULT_TX_POWER         (LORAMAC_TX_PWR_1)
#endif

/**
 * @brief   Default MAC TX port (from 1 to 223)
 *
 * The TX port denotes the port field `FPort` in the payload and is commonly
 * used on the destination side (Application Server) to distinguish different
 * payload formats. For example it can be used to identify different sensor
 * types/values from the same end-node and hence different payload formats can
 * be assigned to the different values based on value of port.
 */
#ifndef CONFIG_LORAMAC_DEFAULT_TX_PORT
#define CONFIG_LORAMAC_DEFAULT_TX_PORT          (2U)
#endif

/**
 * @brief   Default MAC TX mode (confirmable or unconfirmable)
 *
 * A confirmed uplink will expect an acknowledgment from the network server;
 * otherwise, the message will be retransmitted by the number indicated by
 * @ref  CONFIG_LORAMAC_DEFAULT_RETX , whereas an unconfirmed message will not
 * expect any acknowledgment back from the network server.
 */
#if IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_MODE_CNF)
#define CONFIG_LORAMAC_DEFAULT_TX_MODE          (LORAMAC_TX_CNF)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_TX_MODE_UNCNF)
#define CONFIG_LORAMAC_DEFAULT_TX_MODE          (LORAMAC_TX_UNCNF)
#endif

#ifndef CONFIG_LORAMAC_DEFAULT_TX_MODE
#define CONFIG_LORAMAC_DEFAULT_TX_MODE          (LORAMAC_TX_CNF)
#endif

/**
 * @brief   Enable/disable adaptive datarate state
 *
 * If enabled the end node will inform the network server about the status of
 * ADR using the ADR field in uplink data packet. The network server will then
 * optimize the data rate and the transmission power of the end node based on
 * the information collected from the network.
 */
#ifdef DOXYGEN
#define CONFIG_LORAMAC_DEFAULT_ADR
#endif

/**
 * @brief   Default uplink retransmission
 *
 * Maximum number of retransmissions to be used for a confirmed uplink packet,
 * if no downlink acknowledgment is received from the network.
 */
#ifndef CONFIG_LORAMAC_DEFAULT_RETX
#define CONFIG_LORAMAC_DEFAULT_RETX             (5U)
#endif

/**
 * @brief   Default link check interval (in seconds)
 *
 * 0 means the link check process is disabled. The link check process is used by
 * an end-device to validate its connectivity with the network. The frame has
 * no payload. When a `LinkCheckReq` is received by the network server via one
 * or multiple gateways, it responds with a `LinkCheckAns` MAC command.
 */
#ifndef CONFIG_LORAMAC_DEFAULT_LINKCHK
#define CONFIG_LORAMAC_DEFAULT_LINKCHK          (0U)
#endif

/**
 * @brief   Default first RX window delay (in ms)
 *
 * This configuration specifies the delay in seconds to open the RX1 window
 * after the end of uplink modulation on the end-node.
 */
#ifndef CONFIG_LORAMAC_DEFAULT_RX1_DELAY
#define CONFIG_LORAMAC_DEFAULT_RX1_DELAY        (1000U)
#endif

/**
 * @brief   Default second RX (RX2) window datarate index
 *
 * This may be changed only if the network server can be configured with the
 * same datarate.
 */
#if IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_0)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_0)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_1)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_1)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_2)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_2)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_3)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_3)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_4)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_4)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_5)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_5)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_6)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_6)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_7)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_7)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_8)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_8)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_9)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_9)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_10)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_10)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_11)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_11)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_12)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_12)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_13)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_13)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_14)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_14)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_RX2_DR_15)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_15)
#endif

#ifndef CONFIG_LORAMAC_DEFAULT_RX2_DR
#if IS_ACTIVE(CONFIG_LORAMAC_REGION_EU_868)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_0)
#elif IS_ACTIVE(CONFIG_LORAMAC_REGION_IN_865)
#define CONFIG_LORAMAC_DEFAULT_RX2_DR           (LORAMAC_DR_2)
#endif
#endif

/**
 * @brief   Default second RX (RX2) window frequency (in Hz)
 */
#ifndef CONFIG_LORAMAC_DEFAULT_RX2_FREQ
#if IS_ACTIVE(CONFIG_LORAMAC_REGION_EU_868)
#define CONFIG_LORAMAC_DEFAULT_RX2_FREQ         (869525000UL)
#elif IS_ACTIVE(CONFIG_LORAMAC_REGION_IN_865)
#define CONFIG_LORAMAC_DEFAULT_RX2_FREQ         (866550000UL)
#endif
#endif

/**
 * @brief   Default LoRaMAC join procedure
 *
 * There are two options, Over The Air Activation (OTAA) results in device
 * sending join request to the network whereas in the case of Activation By
 * Personalization (ABP) the user enters the activation keys manually. OTAA is
 * the preferred and most secure way to connect to a LoRaWAN network. For quick
 * testing ABP is preferred as the device can transmit right away without
 * waiting for the network to provision the keys.
 */
#if IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE_OTAA)
#define CONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE   (LORAMAC_JOIN_OTAA)
#elif IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE_ABP)
#define CONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE   (LORAMAC_JOIN_ABP)
#endif

#ifndef CONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE
#define CONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE   (LORAMAC_JOIN_OTAA)
#endif

/**
 * @brief   Default LoRaMAC join accept delay 1 (in seconds)
 *
 * Maximum wait time in end node to receive the `join-accept` message sent by
 * the network in the first receive window. This is similar to the operation of
 * RX1 window.
 */
#ifndef CONFIG_LORAMAC_DEFAULT_JOIN_DELAY1
#define CONFIG_LORAMAC_DEFAULT_JOIN_DELAY1      (5U)
#endif

/**
 * @brief   Default LoRaMAC join accept delay 2
 *
 * Maximum wait time in end node to receive the `join-accept` message sent by
 * the network in the second receive window. This is similar to the operation of
 * RX2 window.
 */
#ifndef CONFIG_LORAMAC_DEFAULT_JOIN_DELAY2
#define CONFIG_LORAMAC_DEFAULT_JOIN_DELAY2      (6U)
#endif

/**
 * @brief   Default max FCNT gap
 *
 * Frame counter deviation is the difference between the frame counter values
 * transmitted by the End Node and the value stored in Network Server (NS).
 */
#ifndef CONFIG_LORAMAC_DEFAULT_MAX_FCNT_GAP
#define CONFIG_LORAMAC_DEFAULT_MAX_FCNT_GAP     (16384U)
#endif

/**
 * @brief   Default maximum system overall timing error
 *
 * This configuration is used to increase the RX window to account for timer
 * drift. This may be decreased if the system clock is accurate, for eg: if the
 * system clock is from a TCXO.
 */
#ifndef CONFIG_LORAMAC_DEFAULT_SYSTEM_MAX_RX_ERROR
#define CONFIG_LORAMAC_DEFAULT_SYSTEM_MAX_RX_ERROR  (50)
#endif

/**
 * @brief   Default minimum RX symbols to detect a frame
 *
 * This configuration is used to arrive at `T_RX_late` which is used in RX
 * window calculation. This may be increased to account for inaccuracies in
 * system timer. Refer SX1276_settings_for_LoRaWAN_v2p2.pdf (AN1200.24)
 * from Semtech for more information.
 */
#ifndef CONFIG_LORAMAC_DEFAULT_MIN_RX_SYMBOLS
#define CONFIG_LORAMAC_DEFAULT_MIN_RX_SYMBOLS   (12)
#endif
/** @} */

/**
 * @brief   Default adaptive datarate ACK limit (in s)
 *
 * @note This feature is not yet supported.
 */
#ifndef LORAMAC_DEFAULT_ADR_ACK_LIMIT
#define LORAMAC_DEFAULT_ADR_ACK_LIMIT           (64U)
#endif

/**
 * @brief   Default adaptive datarate ACK delay (in s)
 *
 * @note This feature is not yet supported.
 */
#ifndef LORAMAC_DEFAULT_ADR_ACK_DELAY
#define LORAMAC_DEFAULT_ADR_ACK_DELAY           (32U)
#endif

/**
 * @brief   Default adaptive datarate timeout
 *
 * @note This feature is not yet supported.
 */
#ifndef LORAMAC_DEFAULT_ADR_TIMEOUT
#define LORAMAC_DEFAULT_ADR_TIMEOUT             (3U)
#endif

/**
 * @brief   Default second RX window delay (in ms)
 */
#define LORAMAC_DEFAULT_RX2_DELAY               (1000U + CONFIG_LORAMAC_DEFAULT_RX1_DELAY)

/**
 * @name    LoRaMAC constants
 * @{
 */
/**
 * @brief   Device EUI length in bytes
 */
#define LORAMAC_DEVEUI_LEN                      (8U)

/**
 * @brief   Device address length in bytes
 */
#define LORAMAC_DEVADDR_LEN                     (4U)

/**
 * @brief   Application EUI length in bytes
 */
#define LORAMAC_APPEUI_LEN                      (8U)

/**
 * @brief   Application key length in bytes
 */
#define LORAMAC_APPKEY_LEN                      (16U)

/**
 * @brief   Application session key length in bytes
 */
#define LORAMAC_APPSKEY_LEN                     (16U)

/**
 * @brief   Network session key length in bytes
 */
#define LORAMAC_NWKSKEY_LEN                     (16U)

/**
 * @brief   Minimum port value
 */
#define LORAMAC_PORT_MIN                        (1U)

/**
 * @brief   Maximmu port value
 */
#define LORAMAC_PORT_MAX                        (223U)

/**
 * @brief Application Nonce length in bytes
 */
#define LORAMAC_APP_NONCE_LEN                   (3U)

/**
 * @brief Network ID length in bytes
 */
#define LORAMAC_NETWORK_ID_LEN                  (3U)

/** @} */

/**
 * @name    LoRaMAC parameters indexes
 */

/**
 * @brief   Device class
 */
typedef enum {
    LORAMAC_CLASS_A,                   /**< Class A device */
    LORAMAC_CLASS_B,                   /**< Class B device */
    LORAMAC_CLASS_C,                   /**< Class C device */
} loramac_class_t;

/**
 * @brief   LoRaMAC network join procedure type
 */
typedef enum {
    LORAMAC_JOIN_OTAA,                 /**< Other-the-air activation */
    LORAMAC_JOIN_ABP,                  /**< Activation by personnalization */
} loramac_join_mode_t;

/**
 * @brief   LoRaMAC transmission mode
 */
typedef enum {
    LORAMAC_TX_CNF,                    /**< Confirmable transmission mode */
    LORAMAC_TX_UNCNF,                  /**< Unconfirmable transmission mode */
} loramac_tx_mode_t;

/**
 * @brief   LoRaMAC datarate indexes
 *
 *          Each index corresponds to a different modulation, spreading factor
 *          and bandwidth depending on the regions.
 */
typedef enum {
    /**
     * - ISM EU863-870: LoRa modulation, SF12, BW125 (250bit/s)
     * - ISM US902-928: LoRa modulation, SF10, BW125 (980bit/s)
     * - ISM CN779-787: LoRa modulation, SF12, BW125 (250bit/s)
     * - ISM EU433: LoRa modulation, SF12, BW125 (250bit/s)
     * - ISM AU915-928: LoRa modulation, SF10, BW125 (980bit/s)
     * - ISM CN470-510: LoRa modulation, SF12, BW125 (250bit/s)
     * - ISM AS923: LoRa modulation, SF12, BW125 (250bit/s)
     * - ISM KR920-923: LoRa modulation, SF12, BW125 (250bit/s)
     *
     * Default value used.
     */
    LORAMAC_DR_0 = 0,
    /**
     * - ISM EU863-870: LoRa modulation, SF11, BW125 (440bit/s)
     * - ISM US902-928: LoRa modulation, SF9, BW125 (1760bit/s)
     * - ISM CN779-787: LoRa modulation, SF11, BW125 (440bit/s)
     * - ISM EU433: LoRa modulation, SF11, BW125 (440bit/s)
     * - ISM AU915-928: LoRa modulation, SF9, BW125 (1760bit/s)
     * - ISM CN470-510: LoRa modulation, SF11, BW125 (440bit/s)
     * - ISM AS923: LoRa modulation, SF11, BW125 (440bit/s)
     * - ISM KR920-923: LoRa modulation, SF11, BW125 (440bit/s)
     */
    LORAMAC_DR_1,
    /**
     * - ISM EU863-870: LoRa modulation, SF10, BW125 (980bit/s)
     * - ISM US902-928: LoRa modulation, SF8, BW125 (3125bit/s)
     * - ISM CN779-787: LoRa modulation, SF10, BW125 (980bit/s)
     * - ISM EU433: LoRa modulation, SF10, BW125 (980bit/s)
     * - ISM AU915-928: LoRa modulation, SF8, BW125 (3125bit/s)
     * - ISM CN470-510: LoRa modulation, SF10, BW125 (980bit/s)
     * - ISM AS923: LoRa modulation, SF10, BW125 (980bit/s)
     * - ISM KR920-923: LoRa modulation, SF10, BW125 (980bit/s)
     */
    LORAMAC_DR_2,
    /**
     * - ISM EU863-870: LoRa modulation, SF9, BW125 (1760bit/s)
     * - ISM US902-928: LoRa modulation, SF7, BW125 (5470bit/s)
     * - ISM CN779-787: LoRa modulation, SF9, BW125 (1760bit/s)
     * - ISM EU433: LoRa modulation, SF9, BW125 (1760bit/s)
     * - ISM AU915-928: LoRa modulation, SF7, BW125 (5470bit/s)
     * - ISM CN470-510: LoRa modulation, SF9, BW125 (1760bit/s)
     * - ISM AS923: LoRa modulation, SF9, BW125 (1760bit/s)
     * - ISM KR920-923: LoRa modulation, SF9, BW125 (1760bit/s)
     */
    LORAMAC_DR_3,
    /**
     * - ISM EU863-870: LoRa modulation, SF8, BW125 (3125bit/s)
     * - ISM US902-928: LoRa modulation, SF8, BW500 (12500bit/s)
     * - ISM CN779-787: LoRa modulation, SF8, BW125 (3125bit/s)
     * - ISM EU433: LoRa modulation, SF8, BW125 (3125bit/s)
     * - ISM AU915-928: LoRa modulation, SF8, BW500 (12500bit/s)
     * - ISM CN470-510: LoRa modulation, SF8, BW125 (3125bit/s)
     * - ISM AS923: LoRa modulation, SF8, BW125 (3125bit/s)
     * - ISM KR920-923: LoRa modulation, SF8, BW125 (3125bit/s)
     */
    LORAMAC_DR_4,
    /**
     * - ISM EU863-870: LoRa modulation, SF7, BW125 (5470bit/s)
     * - ISM US902-928: reserved for future use
     * - ISM CN779-787: LoRa modulation, SF7, BW125 (5470bit/s)
     * - ISM EU433: LoRa modulation, SF7, BW125 (5470bit/s)
     * - ISM AU915-928: reserved for future use
     * - ISM CN470-510: LoRa modulation, SF7, BW125 (5470bit/s)
     * - ISM AS923: LoRa modulation, SF7, BW125 (5470bit/s)
     * - ISM KR920-923: LoRa modulation, SF7, BW125 (5470bit/s)
     */
    LORAMAC_DR_5,
    /**
     * - ISM EU863-870: LoRa modulation, SF7, BW250 (11000bit/s)
     * - ISM US902-928: reserved for future use
     * - ISM CN779-787: LoRa modulation, SF7, BW250 (11000bit/s)
     * - ISM EU433 : LoRa modulation, SF7, BW250 (11000bit/s)
     * - ISM AU915-928: reserved for future use
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: LoRa modulation, SF7, BW250 (11000bit/s)
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_6,
    /**
     * - ISM EU863-870: FSK modulation (50000bit/s)
     * - ISM US902-928: reserved for future use
     * - ISM CN779-787: FSK modulation (50000bit/s)
     * - ISM EU433: FSK modulation (50000bit/s)
     * - ISM AU915-928: reserved for future use
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: FSK modulation (50000bit/s)
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_7,
    /**
     * - ISM EU863-870: reserved for future use
     * - ISM US902-928: LoRa modulation, SF12, BW500 (980bit/s)
     * - ISM CN779-787: reserved for future use
     * - ISM EU433: reserved for future use
     * - ISM AU915-928: LoRa modulation, SF12, BW500 (980bit/s)
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: reserved for future use
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_8,
    /**
     * - ISM EU863-870: reserved for future use
     * - ISM US902-928: LoRa modulation, SF11, BW500 (1760bit/s)
     * - ISM CN779-787: reserved for future use
     * - ISM EU433: reserved for future use
     * - ISM AU915-928: LoRa modulation, SF11, BW500 (1760bit/s)
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: reserved for future use
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_9,
    /**
     * - ISM EU863-870: reserved for future use
     * - ISM US902-928: LoRa modulation, SF10, BW500 (3900bit/s)
     * - ISM CN779-787: reserved for future use
     * - ISM EU433: reserved for future use
     * - ISM AU915-928: LoRa modulation, SF10, BW500 (3900bit/s)
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: reserved for future use
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_10,
    /**
     * - ISM EU863-870: reserved for future use
     * - ISM US902-928: LoRa modulation, SF9, BW500 (7000bit/s)
     * - ISM CN779-787: reserved for future use
     * - ISM EU433: reserved for future use
     * - ISM AU915-928: LoRa modulation, SF9, BW500 (7000bit/s)
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: reserved for future use
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_11,
    /**
     * - ISM EU863-870: reserved for future use
     * - ISM US902-928: LoRa modulation, SF8, BW500 (12500bit/s)
     * - ISM CN779-787: reserved for future use
     * - ISM EU433: reserved for future use
     * - ISM AU915-928: LoRa modulation, SF8, BW500 (12500bit/s)
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: reserved for future use
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_12,
    /**
     * - ISM EU863-870: reserved for future use
     * - ISM US902-928: LoRa modulation, SF7, BW500 (21900bit/s)
     * - ISM CN779-787: reserved for future use
     * - ISM EU433: reserved for future use
     * - ISM AU915-928: LoRa modulation, SF7, BW500 (21900bit/s)
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: reserved for future use
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_13,
    /**
     * - ISM EU863-870: reserved for future use
     * - ISM US902-928: reserved for future use
     * - ISM CN779-787: reserved for future use
     * - ISM EU433: reserved for future use
     * - ISM AU915-928: reserved for future use
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: reserved for future use
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_14,
    /**
     * - ISM EU863-870: reserved for future use
     * - ISM US902-928: reserved for future use
     * - ISM CN779-787: reserved for future use
     * - ISM EU433: reserved for future use
     * - ISM AU915-928: reserved for future use
     * - ISM CN470-510: reserved for future use
     * - ISM AS923: reserved for future use
     * - ISM KR920-923: reserved for future use
     */
    LORAMAC_DR_15,
} loramac_dr_idx_t;

/**
 * @brief   LoRaMAC transmission power indexes
 *
 *          Each index corresponds to a different modulation, spreading factor
 *          and bandwidth depending on the regions.
 */
typedef enum {
    /**
     * - EU863-870: 20dBm (if supported)
     * - US902-928: 30dBm (if supported)
     * - CN779-787: 10dBm
     * - EU433: 10dBm
     * - AU915-928: 30dBm
     * - CN470-510: 17dBm
     * - ISM AS923: 14dBm
     * - ISM KR920-923: 20dBm
     */
    LORAMAC_TX_PWR_0 = 0,
    /**
     * - EU863-870: 14dBm
     * - US902-928: 28dBm
     * - CN779-787: 7dBm
     * - EU433: 7dBm
     * - AU915-928: 28dBm
     * - CN470-510: 16dBm
     * - ISM AS923: 12dBm
     * - ISM KR920-923: 14dBm
     *
     * Default value used
     */
    LORAMAC_TX_PWR_1,
    /**
     * - EU863-870: 11dBm
     * - US902-928: 26dBm
     * - CN779-787: 4dBm
     * - EU433: 4dBm
     * - AU915-928: 26dBm
     * - CN470-510: 14dBm
     * - ISM AS923: 10dBm
     * - ISM KR920-923: 10dBm
     */
    LORAMAC_TX_PWR_2,
    /**
     * - EU863-870: 8dBm
     * - US902-928: 24dBm
     * - CN779-787: 1dBm
     * - EU433: 1dBm
     * - AU915-928: 24dBm
     * - CN470-510: 12dBm
     * - ISM AS923: 8dBm
     * - ISM KR920-923: 8dBm
     */
    LORAMAC_TX_PWR_3,
    /**
     * - EU863-870: 5dBm
     * - US902-928: 22dBm
     * - CN779-787: -2dBm
     * - EU433: -2dBm
     * - AU915-928: 22dBm
     * - CN470-510: 10dBm
     * - ISM AS923: 6dBm
     * - ISM KR920-923: 5dBm
     */
    LORAMAC_TX_PWR_4,
    /**
     * - EU863-870: 2dBm
     * - US902-928: 20dBm
     * - CN779-787: -5dBm
     * - EU433: -5dBm
     * - AU915-928: 20dBm
     * - CN470-510: 7dBm
     * - ISM AS923: 4dBm
     * - ISM KR920-923: 2dBm
     */
    LORAMAC_TX_PWR_5,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: 18dBm
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: 18dBm
     * - CN470-510: 5dBm
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: 0dBm
     */
    LORAMAC_TX_PWR_6,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: 16dBm
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: 16dBm
     * - CN470-510: 2dBm
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: Reserved for future use
     */
    LORAMAC_TX_PWR_7,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: 14dBm
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: 14dBm
     * - CN470-510: Reserved for future use
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: Reserved for future use
     */
    LORAMAC_TX_PWR_8,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: 12dBm
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: 12dBm
     * - CN470-510: Reserved for future use
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: Reserved for future use
     */
    LORAMAC_TX_PWR_9,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: 10dBm
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: 10dBm
     * - CN470-510: Reserved for future use
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: Reserved for future use
     */
    LORAMAC_TX_PWR_10,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: Reserved for future use
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: Reserved for future use
     * - CN470-510: Reserved for future use
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: Reserved for future use
     */
    LORAMAC_TX_PWR_11,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: Reserved for future use
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: Reserved for future use
     * - CN470-510: Reserved for future use
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: Reserved for future use
     */
    LORAMAC_TX_PWR_12,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: Reserved for future use
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: Reserved for future use
     * - CN470-510: Reserved for future use
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: Reserved for future use
     */
    LORAMAC_TX_PWR_13,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: Reserved for future use
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: Reserved for future use
     * - CN470-510: Reserved for future use
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: Reserved for future use
     */
    LORAMAC_TX_PWR_14,
    /**
     * - EU863-870: Reserved for future use
     * - US902-928: Reserved for future use
     * - CN779-787: Reserved for future use
     * - EU433: Reserved for future use
     * - AU915-928: Reserved for future use
     * - CN470-510: Reserved for future use
     * - ISM AS923: Reserved for future use
     * - ISM KR920-923: Reserved for future use
     */
    LORAMAC_TX_PWR_15,
} loramac_tx_pwr_idx_t;
/** @} */

/**
 * @brief   A LoRaMAC network channel
 */
 typedef struct {
    uint32_t freq;                     /**< Center frequency in Hz */
    uint8_t index;                     /**< Channel index in defined list */
    uint8_t bw;                        /**< Bandwidth index */
    uint8_t dr_min;                    /**< Minimum datarate index */
    uint8_t dr_max;                    /**< Maximum datarate index */
    uint8_t dcycle;                    /**< Duty cycle to use on this channel (1 to 100) */
} loramac_channel_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_LORAMAC_H */
/** @} */
