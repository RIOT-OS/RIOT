/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_lorawan
 * @{
 *
 * @file
 * @brief   GNRC LoRaWAN internal header
 *
 * @author  Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef GNRC_LORAWAN_INTERNAL_H
#define GNRC_LORAWAN_INTERNAL_H

#include <stdio.h>
#include <string.h>
#include "iolist.h"
#include "net/lora.h"
#include "net/lorawan/hdr.h"
#include "net/gnrc/pktbuf.h"
#include "net/netdev.h"
#include "net/loramac.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSG_TYPE_TIMEOUT             (0x3457)           /**< Timeout message type */

#define MTYPE_MASK           0xE0                       /**< MHDR mtype mask */
#define MTYPE_JOIN_REQUEST   0x0                        /**< Join Request type */
#define MTYPE_JOIN_ACCEPT    0x1                        /**< Join Accept type */
#define MTYPE_UNCNF_UPLINK   0x2                        /**< Unconfirmed uplink type */
#define MTYPE_UNCNF_DOWNLINK 0x3                        /**< Unconfirmed downlink type */
#define MTYPE_CNF_UPLINK     0x4                        /**< Confirmed uplink type */
#define MTYPE_CNF_DOWNLINK   0x5                        /**< Confirmed downlink type */
#define MTYPE_REJOIN_REQ     0x6                        /**< Re-join request type */
#define MTYPE_PROPIETARY     0x7                        /**< Proprietary frame type */

#define MAJOR_MASK     0x3                              /**< Major mtype mask */
#define MAJOR_LRWAN_R1 0x0                              /**< LoRaWAN R1 version type */

#define MINOR_LRWAN    0x1                              /**< Minor LoRaWAN version of device */

#define JOIN_REQUEST_SIZE (23U)                         /**< Join Request size in bytes */
#define MIC_SIZE (4U)                                   /**< MIC size in bytes */
#define CFLIST_SIZE (16U)                               /**< Channel Frequency list size in bytes */

#define GNRC_LORAWAN_MAX_CHANNELS (16U)                 /**< Maximum number of channels */

#define LORAWAN_STATE_IDLE (0)                          /**< MAC state machine in idle */
#define LORAWAN_STATE_RX_1 (1)                          /**< MAC state machine in RX1 */
#define LORAWAN_STATE_RX_2 (2)                          /**< MAC state machine in RX2 */
#define LORAWAN_STATE_TX (3)                            /**< MAC state machine in TX */
#define LORAWAN_STATE_JOIN (4)                          /**< MAC state machine in Join */

#define GNRC_LORAWAN_DIR_UPLINK (0U)                    /**< uplink frame direction */
#define GNRC_LORAWAN_DIR_DOWNLINK (1U)                  /**< downlink frame direction */

#define GNRC_LORAWAN_BACKOFF_WINDOW_TICK (3600000000LL) /**< backoff expire tick in usecs (set to 1 second) */

#define GNRC_LORAWAN_BACKOFF_BUDGET_1   (36000000LL)    /**< budget of time on air during the first hour */
#define GNRC_LORAWAN_BACKOFF_BUDGET_2   (36000000LL)    /**< budget of time on air between 1-10 hours after boot */
#define GNRC_LORAWAN_BACKOFF_BUDGET_3   (8700000LL)     /**< budget of time on air every 24 hours */

#define GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ  (1 << 0) /**< Internal Link Check request flag */
#define GNRC_LORAWAN_MLME_OPTS_REKEY_IND_REQ   (1 << 1) /**< Internal Rekey Indication flag */

#define GNRC_LORAWAN_CID_SIZE (1U)                      /**< size of Command ID in FOps */
#define GNRC_LORAWAN_CID_LINK_CHECK_ANS (0x02)          /**< Link Check CID */

#define GNCR_LORAWAN_REKEY_IND_SIZE (1U)                /**< RekeyInd MAC command size */
#define GNCR_LORAWAN_CID_REKEY_CONF (0x0B)              /**< Reykey Confirmation CID */

#define GNRC_LORAWAN_FOPT_LINK_CHECK_ANS_SIZE (3U)      /**< size of Link check answer */
#define GNRC_LORAWAN_FOPT_REKEY_CONF_SIZE (2U)          /**< size of Rekey confirmation */

#define GNRC_LORAWAN_JOIN_DELAY_U32_MASK (0x1FFFFF)     /**< mask for detecting overflow in frame counter */

#define GNRC_LORAWAN_MAX_PAYLOAD_1 (59U)                /**< max MAC payload in DR0, DR1 and DR2 */
#define GNRC_LORAWAN_MAX_PAYLOAD_2 (123U)               /**< max MAC payload in DR3 */
#define GNRC_LORAWAN_MAX_PAYLOAD_3 (250U)               /**< max MAC payload above DR3 */

#define GNRC_LORAWAN_CFLIST_ENTRY_SIZE (3U)             /**< size of Channel Frequency list */
#define GNRC_LORAWAN_JOIN_ACCEPT_MAX_SIZE (33U)         /**< max size of Join Accept frame */

#define GNRC_LORAWAN_BACKOFF_STATE_1 (0U)               /**< backoff state during the first hour after boot */
#define GNRC_LORAWAN_BACKOFF_STATE_2 (1U)               /**< backoff state between 1-10 hours after boot */
#define GNRC_LORAWAN_BACKOFF_STATE_3 (2U)               /**< backoff state past 11 hours after boot */

#define GNRC_LORAWAN_BACKOFF_TIME_1 (1U)                /**< duration of first backoff state (in hours) */
#define GNRC_LORAWAN_BACKOFF_TIME_2 (10U)               /**< duration of second backoff state (in hours) */
#define GNRC_LORAWAN_BACKOFF_TIME_3 (24U)               /**< duration of third backoff state (in hours) */

#define GNRC_LORAWAN_APP_NONCE_SIZE (3U)                /**< App Nonce size */
#define GNRC_LORAWAN_JOIN_NONCE_SIZE (3U)               /**< Join nonce size */
#define GNRC_LORAWAN_NET_ID_SIZE (3U)                   /**< Net ID size */
#define GNRC_LORAWAN_DEV_NONCE_SIZE (2U)                /**< Dev Nonce size */

#define GNRC_LORAWAN_FOPTS_MAX_SIZE (15U)               /**< Maximum size of Fopts field */
#define GNRC_LORAWAN_FPORT_SIZE (1U)                    /**< Size of the Fport field */

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7)
#define GNRC_LORAWAN_STATE_FLASHPAGE_NUM (0x7)                      /**< Flashpage number where to store state */
#else
#define GNRC_LORAWAN_STATE_FLASHPAGE_NUM (FLASHPAGE_NUMOF - 0x4)    /**< Flashpage number where to store state */
#endif
#define GNRC_LORAWAN_INITIALIZED_MARKER (0x52f94f54U)               /**< Persistent state initialized marker */

/**
 * @brief Size of the internal MHDR-MIC buffer
 */
#define MHDR_MIC_BUF_SIZE (sizeof(lorawan_hdr_t) + \
                           GNRC_LORAWAN_FOPTS_MAX_SIZE + \
                           GNRC_LORAWAN_FPORT_SIZE + \
                           MIC_SIZE)

/**
 * @brief buffer helper for parsing and constructing LoRaWAN packets.
 */
typedef struct {
    uint8_t *data;  /**< pointer to the beginning of the buffer holding data */
    uint8_t size;   /**< size of the buffer */
    uint8_t index;  /**< current index in the buffer */
} lorawan_buffer_t;

/**
 * @brief MLME Join Request data
 */
typedef struct {
    void *deveui;   /**< pointer to the Device EUI */
    void *joineui;  /**< pointer to the Join EUI */
    void *nwkkey;   /**< pointer to the Network Key. Mapped to App key if LoRaWAN 1.0x */
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    void *appkey;   /**< pointer to the App key */
#endif
    uint8_t dr;     /**< datarate for the Join Request */
} mlme_lorawan_join_t;

/**
 * @brief MLME Join Request type
 */
typedef enum {
    REJOIN_REQ_0,       /**< Rejoin-request type 0 */
    REJOIN_REQ_1,       /**< Rejoin-request type 1 */
    REJOIN_REQ_2,       /**< Rejoin-request type 2 */
    JOIN_REQ = 0xFF     /**< Join-request type */
} mlme_join_req_type_t;

/**
 * @brief MLME Link Check confirmation data
 */
typedef struct {
    uint8_t margin;         /**< demodulation margin (in dB) */
    uint8_t num_gateways;   /**< number of gateways */
} mlme_link_req_confirm_t;

/**
 * @brief MCPS data
 */
typedef struct {
    iolist_t *pkt;          /**< packet of the request */
    uint8_t port;           /**< port of the request */
    uint8_t dr;             /**< datarate of the request */
} mcps_data_t;

/**
 * @brief MCPS service access point descriptor
 */
typedef struct {
    uint32_t fcnt;                      /**< uplink framecounter */
    uint32_t fcnt_down;                 /**< downlink frame counter. Mapped to network downlink frame counter if using LoRaWAN 1.1 */
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    uint32_t afcnt_down;                /**< application downlink frame counter */
    uint32_t last_fcnt_down;            /**< last downlink frame counter used to decrypt frame. Needed to compute ConfFCnt */
#endif
    iolist_t *msdu;                     /**< current MSDU */
    int nb_trials;                      /**< holds the remaining number of retransmissions */
    int ack_requested;                  /**< whether the network server requested an ACK */
    int waiting_for_ack;                /**< true if the MAC layer is waiting for an ACK */
    uint8_t redundancy;                 /**< unconfirmed uplink redundancy */
    char mhdr_mic[MHDR_MIC_BUF_SIZE];   /**< internal retransmissions buffer */
} gnrc_lorawan_mcps_t;

/**
 * @brief MLME service access point descriptor
 */
typedef struct {
    uint8_t activation;     /**< Activation mechanism of the MAC layer */
    int pending_mlme_opts;  /**< holds pending mlme opts */
    uint32_t nid;           /**< current Network ID */
    int32_t backoff_budget; /**< remaining Time On Air budget */
    uint8_t dev_nonce[2];   /**< Device Nonce */
    uint8_t backoff_state;  /**< state in the backoff state machine */
} gnrc_lorawan_mlme_t;

/**
 * @brief GNRC LoRaWAN key context struct
 */
typedef struct {
    uint8_t *appskey;                               /**< pointer to Application SKey buffer */
    uint8_t *fnwksintkey;                           /**< pointer to Forwarding Network session integrity key */
    uint8_t *snwksintkey;                           /**< pointer to Serving Network session integrity key */
    uint8_t *nwksenckey;                            /**< pointer to Network session encryption key */
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    uint8_t *jsintkey;                              /**< pointer to join session integrity key */
    uint8_t *jsenckey;                              /**< pointer to join session encryption key */
#endif
} gnrc_lorawan_key_ctx_t;

/**
 * @brief GNRC LoRaWAN mac descriptor */
typedef struct {
    gnrc_lorawan_mcps_t mcps;                       /**< MCPS descriptor */
    gnrc_lorawan_mlme_t mlme;                       /**< MLME descriptor */
    void *mlme_buf;                                 /**< pointer to MLME buffer */
    void *mcps_buf;                                 /**< pointer to MCPS buffer */
    uint8_t *joineui;                               /**< pointer to Join EUI */
    gnrc_lorawan_key_ctx_t ctx;                     /**< GNRC LoRaWAN key context struct */
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    bool optneg;                                    /**< optneg bit */
#endif
    uint32_t channel[GNRC_LORAWAN_MAX_CHANNELS];    /**< channel array */
    uint16_t channel_mask;                          /**< channel mask */
    uint32_t toa;                                   /**< Time on Air of the last transmission */
    int busy;                                       /**< MAC busy  */
    int shutdown_req;                               /**< MAC Shutdown request */
    le_uint32_t dev_addr;                           /**< Device address */
    int state;                                      /**< state of MAC layer */
    uint8_t dl_settings;                            /**< downlink settings */
    uint8_t rx_delay;                               /**< Delay of first reception window */
    uint8_t dr_range[GNRC_LORAWAN_MAX_CHANNELS];    /**< Datarate Range for all channels */
    uint8_t last_dr;                                /**< datarate of the last transmission */
    uint8_t last_chan_idx;                          /**< index of channel used for last transmission */
} gnrc_lorawan_t;

/**
 * @brief LoRaWAN state that needs to be preserved across reboots
 */
typedef struct {
    uint32_t initialized_marker;    /**< state initialized marker */
    uint8_t dev_nonce[2];           /**< Device Nonce */
} gnrc_lorawan_persistent_state_t;

/**
 * @brief Encrypts LoRaWAN payload
 *
 * @note This function is also used for decrypting a LoRaWAN packet. The LoRaWAN server encrypts the packet using decryption, so the end device only needs to implement encryption
 *
 * @param[in] iolist pointer to the MSDU frame
 * @param[in] dev_addr device address
 * @param[in] fcnt frame counter
 * @param[in] dir direction of the packet (0 if uplink, 1 if downlink)
 * @param[in] appskey pointer to the Application Session Key
 */
void gnrc_lorawan_encrypt_payload(iolist_t *iolist, const le_uint32_t *dev_addr,
                                  uint32_t fcnt, uint8_t dir,
                                  const uint8_t *appskey);

/**
 * @brief Encrypts FOpts field
 *
 * @note This function is also used for decrypting the FOpts field. The LoRaWAN server encrypts the packet using decryption, so the end device only needs to implement encryption
 * @note This function is used only in LoRaWAN 1.1
 *
 * @param[in] fopts pointer to fopts
 * @param[in] len length of fopts
 * @param[in] dev_addr device address
 * @param[in] fcnt frame counter
 * @param[in] afcnt flag indicating if aFCntDown is used
 * @param[in] dir direction of the packet (0 if uplink, 1 if downlink)
 * @param[in] key pointer to key
 */
void gnrc_lorawan_encrypt_fopts(uint8_t *fopts, size_t len,
                                const le_uint32_t *dev_addr, uint32_t fcnt,
                                bool afcnt, uint8_t dir, const uint8_t *key);

/**
 * @brief Decrypts join accept message
 *
 * @param[in] key key to be used in the decryption
 * @param[in] pkt pointer to Join Accept MAC component (next byte after the MHDR)
 * @param[in] has_clist true if the Join Accept frame has CFList
 * @param[out] out buffer where the decryption is stored
 */
void gnrc_lorawan_decrypt_join_accept(const uint8_t *key, uint8_t *pkt,
                                      int has_clist, uint8_t *out);

/**
 * @brief Generate LoRaWAN session keys
 *
 * Intended to be called after a successful Join Request in order to generate
 * AppSKey, SNwkSIntKey, FNwkSIntKey and NwkSEncKey
 *
 * @param[in]       join_nonce pointer to the join_nonce of the Join Accept message
 * @param[in]       dev_nonce pointer to the dev_nonce buffer
 * @param[in]       joineui pointer to the Join EUI. NULL if LoRaWAN 1.0x
 * @param[in, out]  mac pointer to the LoRaWAN mac descriptor
 */
void gnrc_lorawan_generate_session_keys(const uint8_t *join_nonce,
                                        const uint8_t *dev_nonce,
                                        const uint8_t *joineui,
                                        gnrc_lorawan_t *mac);

/**
 * @brief Generate LoRaWAN 1.1x lifetime session keys
 *
 * Called in order to derive JSIntKey and JSEncKey.
 *
 * LoRaWAN specification 1.1 section 6.1.1.4
 *
 * @param[in] deveui pointer to the join_nonce of the Join Accept message
 * @param[in] nwkkey pointer to the Network key
 * @param[out] jsintkey pointer to the Join session integrity key
 * @param[out] jsenckey pointer to the Join session encryption key
 */
void gnrc_lorawan_generate_lifetime_session_keys(const uint8_t *deveui,
                                                 const uint8_t *nwkkey,
                                                 uint8_t *jsintkey,
                                                 uint8_t *jsenckey);
/**
 * @brief Set datarate for the next transmission
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] datarate desired datarate
 *
 * @return 0 on success
 * @return -EINVAL if datarate is not available in the current region
 */
int gnrc_lorawan_set_dr(gnrc_lorawan_t *mac, uint8_t datarate);

/**
 * @brief build uplink frame
 *
 * @param[in] mac pointer to MAC descriptor
 * @param[in] payload packet containing payload
 * @param[in] confirmed_data true if confirmed frame
 * @param[in] port MAC port
 *
 * @return full LoRaWAN frame including payload
 * @return NULL if packet buffer is full. `payload` is released
 */
size_t gnrc_lorawan_build_uplink(gnrc_lorawan_t *mac, iolist_t *payload,
                                 int confirmed_data, uint8_t port);

/**
 * @brief pick a random available LoRaWAN channel
 *
 * @param[in] mac pointer to the MAC descriptor
 *
 * @return index of free channel inside channel array
 */
uint8_t gnrc_lorawan_pick_channel(gnrc_lorawan_t *mac);

/**
 * @brief Build fopts header
 *
 * @param[in] mac pointer to MAC descriptor
 * @param[out] buf destination buffer of fopts. If NULL, this function just returns
 *             the size of the expected fopts frame.
 *
 * @return size of the fopts frame
 */
uint8_t gnrc_lorawan_build_options(gnrc_lorawan_t *mac, lorawan_buffer_t *buf);

/**
 * @brief Process an fopts frame
 *
 * @param[in] mac pointer to MAC descriptor
 * @param[in] fopts pointer to fopts frame
 * @param[in] size size of fopts frame
 */
void gnrc_lorawan_process_fopts(gnrc_lorawan_t *mac, uint8_t *fopts,
                                size_t size);

/**
 * @brief Calculate join-request Message Integrity Code
 *
 * @param[in] buf pointer to the frame
 * @param[in] len length of the frame
 * @param[in] key key to be used to calculate the MIC
 * @param[out] out calculated MIC
 */
void gnrc_lorawan_calculate_join_req_mic(const uint8_t *buf, size_t len,
                                         uint8_t *key, le_uint32_t *out);

/**
 * @brief Calculate join-accept Message Integrity Code
 *
 * @param[in] buf pointer to the frame
 * @param[in] len length of the frame
 * @param[in] mac pointer to MAC descriptor
 * @param[out] out calculated MIC
 */
void gnrc_lorawan_calculate_join_acpt_mic(const uint8_t *buf, size_t len,
                                          gnrc_lorawan_t *mac, le_uint32_t *out);

/**
 * @brief Calculate Message Integrity Code for an uplink MCPS message
 *
 * @param[in] frame pointer to the PSDU frame (without MIC)
 * @param[in] conf_fcnt frame counter value of confirmed uplink frame. Always
 *                      zero if using LoRaWAN 1.0x
 * @param[in] mac pointer to MAC descriptor
 * @param[out] out calculated MIC
 */
void gnrc_lorawan_calculate_mic_uplink(iolist_t *frame, uint16_t conf_fcnt,
                                       gnrc_lorawan_t *mac, le_uint32_t *out);

/**
 * @brief Calculate Message Integrity Code for downlink MCPS message
 *
 * @param[in] dev_addr the Device Address
 * @param[in] fcnt frame counter
 * @param[in] conf_fcnt frame counter value of confirmed uplink frame
 * @param[in] frame pointer to the PSDU frame (without MIC)
 * @param[in] snwksintkey pointer to serving network session integrity key
 * @param[out] out calculated MIC
 */
void gnrc_lorawan_calculate_mic_downlink(const le_uint32_t *dev_addr,
                                         uint32_t fcnt, uint16_t conf_fcnt,
                                         iolist_t *frame,
                                         const uint8_t *snwksintkey,
                                         le_uint32_t *out);

/**
 * @brief Build a MCPS LoRaWAN header
 *
 * @param[in] mtype the MType of the header
 * @param[in] dev_addr the Device Address
 * @param[in] fcnt frame counter
 * @param[in] ack true if ACK bit is set
 * @param[in] fopts_length the length of the FOpts field
 * @param[out] buf destination buffer of the hdr
 *
 * @return the size of the header
 */
size_t gnrc_lorawan_build_hdr(uint8_t mtype, le_uint32_t *dev_addr,
                              uint32_t fcnt, uint8_t ack, uint8_t fopts_length,
                              lorawan_buffer_t *buf);

/**
 * @brief Process an MCPS downlink message (confirmable or non comfirmable)
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] psdu pointer to the downlink PSDU
 * @param[in] size size of the PSDU
 */
void  gnrc_lorawan_mcps_process_downlink(gnrc_lorawan_t *mac, uint8_t *psdu,
                                         size_t size);

/**
 * @brief Init regional channel settings.
 *
 *        Intended to be called upon initialization
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_channels_init(gnrc_lorawan_t *mac);

/**
 * @brief Reset MAC parameters
 *
 * @note This doesn't affect backoff timers variables.
 *
 * @param[in] mac pointer to the MAC layer
 */
void gnrc_lorawan_reset(gnrc_lorawan_t *mac);

/**
 * @brief Send a LoRaWAN packet
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] psdu the psdu frame to be sent
 * @param[in] dr the datarate used for the transmission
 * @param[in] chan the channel used for transmission
 */
void gnrc_lorawan_send_pkt(gnrc_lorawan_t *mac, iolist_t *psdu, uint8_t dr,
                           uint32_t chan);

/**
 * @brief Process join accept message
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] data the Join Accept packet
 * @param[in] size size of the Join Accept packet
 */
void gnrc_lorawan_mlme_process_join(gnrc_lorawan_t *mac, uint8_t *data,
                                    size_t size);

/**
 * @brief Inform the MAC layer that no packet was received during reception.
 *
 *        To be called when the radio reports "NO RX" after the second reception
 *        window
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_mlme_no_rx(gnrc_lorawan_t *mac);

/**
 * @brief Mac callback for no RX
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_event_no_rx(gnrc_lorawan_t *mac);

/**
 * @brief Mac callback for retransmission timeout event
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_event_retrans_timeout(gnrc_lorawan_t *mac);

/**
 * @brief Get the maximum MAC payload (M value) for a given datarate.
 *
 * @note This function is region specific
 *
 * @param[in] datarate datarate
 *
 * @return the maximum allowed size of the packet
 */
uint8_t gnrc_lorawan_region_mac_payload_max(uint8_t datarate);

/**
 * @brief MLME Backoff expiration tick
 *
 *        Must be called once an hour (right after calling @ref
 *        gnrc_lorawan_init) in order to maintain the Time On Air budget.
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_mlme_backoff_expire_cb(gnrc_lorawan_t *mac);

/**
 * @brief Process and dispatch a full LoRaWAN packet
 *
 *        Intended to be called right after reception from the radio
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] pkt the received packet
 */
void gnrc_lorawan_process_pkt(gnrc_lorawan_t *mac, iolist_t *pkt);

/**
 * @brief Open a reception window
 *
 *        This is called by the MAC layer on timeout event.
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_open_rx_window(gnrc_lorawan_t *mac);

/**
 * @brief save internal MAC state in non-volatile storage and shutdown
 *        the MAC layer gracefully.
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_perform_save(gnrc_lorawan_t *mac);

/**
 * @brief Acquire the MAC layer
 *
 * @param[in] mac pointer to the MAC descriptor
 *
 * @return true on success
 * @return false if MAC is already acquired
 */
static inline int gnrc_lorawan_mac_acquire(gnrc_lorawan_t *mac)
{
    int _c = mac->busy;

    mac->busy = true;
    return !_c;
}

/**
 * @brief Release the MAC layer
 *
 * @param[in] mac pointer to the MAC descriptor
 */
static inline void gnrc_lorawan_mac_release(gnrc_lorawan_t *mac)
{
    mac->busy = false;
}

/**
 * @brief Set the datarate of the second reception window
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] rx2_dr datarate of RX2
 */
void gnrc_lorawan_set_rx2_dr(gnrc_lorawan_t *mac, uint8_t rx2_dr);

/**
 * @brief Trigger the transmission of the Join Request packet.
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_trigger_join(gnrc_lorawan_t *mac);

/**
 * @brief Store DevNonce in flash memory
 *
 * @param[in] dev_nonce pointer to the DevNonce
 */
void gnrc_lorawan_store_dev_nonce(uint8_t *dev_nonce);

/**
 * @brief Check whether OptNeg bit is set
 *
 *  When using LoRaWAN 1.1x the OptNeg bit indicates whether the Network Server
 *  implements the LoRaWAN1.0 protocol version (unset) or 1.1 and later (set).
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mac pointer to the MAC descriptor
 * @return true if OptNeg bit is set
 * @return false if OptNeg bit is unset
 */
static inline bool gnrc_lorawan_optneg_is_set(const gnrc_lorawan_t *mac)
{
    (void)mac;
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    return mac->optneg;
#else
    return 1; /* NO-OP */
#endif
}

/**
 * @brief Set OptNeg bit in the MAC descriptor
 *
 *  When using LoRaWAN 1.1x the OptNeg bit indicates whether the Network Server
 *  implements the LoRaWAN1.0 protocol version (unset) or 1.1 and later (set).
 *
 *  This setter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mac       pointer to the MAC descriptor
 * @param[in] optneg    OptNeg bit
 */
static inline void gnrc_lorawan_set_optneg(gnrc_lorawan_t *mac, uint8_t optneg)
{
    (void)mac;
    (void)optneg;

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    mac->optneg = optneg;
#endif
}

/**
 * @brief Get the join session integrity key from the MAC descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mac       pointer to the MAC descriptor
 * @return pointer to join session integrity key
 */
static inline uint8_t *gnrc_lorawan_get_jsintkey(const gnrc_lorawan_t *mac)
{
    (void)mac;
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    return mac->ctx.jsintkey;
#else
    return NULL; /* NO-OP */
#endif
}

/**
 * @brief Get the join session encryption key from the MAC descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mac       pointer to the MAC descriptor
 * @return pointer to join session encryption key
 */
static inline uint8_t *gnrc_lorawan_get_jsenckey(const gnrc_lorawan_t *mac)
{
    (void)mac;
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    return mac->ctx.jsenckey;
#else
    return NULL; /* NO-OP */
#endif
}

/**
 * @brief Get the application downlink frame counter from the MAC descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mac       pointer to the MAC descriptor
 * @return application downlink frame counter
 */
static inline uint32_t gnrc_lorawan_get_afcnt_down(const gnrc_lorawan_t *mac)
{
    (void)mac;
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    return mac->mcps.afcnt_down;
#else
    return 0; /* NO-OP */
#endif
}

/**
 * @brief Set the application downlink frame counter in the MAC descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mac       pointer to the MAC descriptor
 * @param[in] afcnt_down application downlink frame counter
 */
static inline void gnrc_lorawan_set_afcnt_down(gnrc_lorawan_t *mac, uint32_t afcnt_down)
{
    (void)mac;
    (void)afcnt_down;

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    mac->mcps.afcnt_down = afcnt_down;
#endif
}

/**
 * @brief Get the last downlink frame counter from the MAC descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mac       pointer to the MAC descriptor
 */
static inline uint32_t gnrc_lorawan_get_last_fcnt_down(const gnrc_lorawan_t *mac)
{
    (void)mac;
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    return mac->mcps.last_fcnt_down;
#else
    return 0; /* NO-OP */
#endif
}

/**
 * @brief Set the last downlink frame counter in the MAC descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mac       pointer to the MAC descriptor
 * @param[in] last_fcnt_down last downlink frame counter
 */
static inline void gnrc_lorawan_set_last_fcnt_down(gnrc_lorawan_t *mac, uint32_t last_fcnt_down)
{
    (void)mac;
    (void)last_fcnt_down;
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    mac->mcps.last_fcnt_down = last_fcnt_down;
#endif
}

/**
 * @brief Get the app key from the MLME join request data
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mlme_join      pointer to the MLME join request data
 * @return pointer to the app key
 */
static inline uint8_t * gnrc_lorawan_mlme_join_get_appkey(const mlme_lorawan_join_t *mlme_join)
{
    (void)mlme_join;

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    return mlme_join->appkey;
#else
    return NULL; /* NO-OP */
#endif
}

/**
 * @brief Se the app key in the MLME join request data
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] mlme_join       pointer to the MLME join request data
 * @param[in] key       pointer to the app key
 */
static inline void gnrc_lorawan_mlme_join_set_appkey(mlme_lorawan_join_t *mlme_join, uint8_t *key)
{
    (void)mlme_join;
    (void)key;

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    mlme_join->appkey = key;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* GNRC_LORAWAN_INTERNAL_H */
/** @} */
