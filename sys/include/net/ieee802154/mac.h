/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @{
 *
 * @file
 * @author Moritz Holzer <moritz.holzer@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ztimer.h"
#include "iolist.h"
#include "kernel_defines.h"

#include "net/ieee802154.h"
#include "net/ieee802154/submac.h"
#include "net/ieee802154/radio.h"

#define IEEE802154_MAC_INDIRECT_ENABLE

#if IS_USED(MODULE_ESP_IEEE802154)
#define IEEE802154_MAC_HAS_SRC_ADDR_MATCH
#endif

#ifdef IEEE802154_MAC_INDIRECT_ENABLE
#  ifndef IEEE802154_MAC_HAS_SRC_ADDR_MATCH
#    define IEEE802154_MAC_FORCE_SOFT_ACK
#  endif
#endif

#ifndef IEEE802154_SCAN_BEACON_PAYLOAD_MAX
#define IEEE802154_SCAN_BEACON_PAYLOAD_MAX  (IEEE802154_FRAME_LEN_MAX)
#endif

#ifndef CONFIG_IEEE802154_MAC_TXQ_LEN
#define CONFIG_IEEE802154_MAC_TXQ_LEN   (4U)
#endif
#ifndef IEEE802154_MAC_TXQ_LEN
#define IEEE802154_MAC_TXQ_LEN   CONFIG_IEEE802154_MAC_TXQ_LEN
#endif

#ifndef CONFIG_IEEE802154_MAC_TICK_INTERVAL_MS
#define CONFIG_IEEE802154_MAC_TICK_INTERVAL_MS     (1U)
#endif
#ifndef IEEE802154_MAC_TICK_INTERVAL_MS
#define IEEE802154_MAC_TICK_INTERVAL_MS     CONFIG_IEEE802154_MAC_TICK_INTERVAL_MS
#endif

/* In Symbols  then the timeout is symbol*symboltime in 2,4ghz this is 1s*/
#ifndef CONFIG_IEEE802154_MAC_FRAME_TIMEOUT
#define CONFIG_IEEE802154_MAC_FRAME_TIMEOUT        (125000U)
#endif
#ifndef IEEE802154_MAC_FRAME_TIMEOUT
#define IEEE802154_MAC_FRAME_TIMEOUT        CONFIG_IEEE802154_MAC_FRAME_TIMEOUT
#endif

#ifndef CONFIG_IEEE802154_MAC_TX_INDIRECTQ_SIZE
#define CONFIG_IEEE802154_MAC_TX_INDIRECTQ_SIZE     (4)
#endif
#ifndef IEEE802154_MAC_TX_INDIRECTQ_SIZE
#define IEEE802154_MAC_TX_INDIRECTQ_SIZE     CONFIG_IEEE802154_MAC_TX_INDIRECTQ_SIZE
#endif

#ifndef CONFIG_IEEE802154_MAC_ASSOC_TABLE_SIZE
#define CONFIG_IEEE802154_MAC_ASSOC_TABLE_SIZE      (IEEE802154_MAC_TX_INDIRECTQ_SIZE)
#endif
#ifndef IEEE802154_MAC_ASSOC_TABLE_SIZE
#define IEEE802154_MAC_ASSOC_TABLE_SIZE      CONFIG_IEEE802154_MAC_ASSOC_TABLE_SIZE
#endif

#ifndef CONFIG_IEEE802154_MAC_BASE_SLOT_DURATION
#define CONFIG_IEEE802154_MAC_BASE_SLOT_DURATION   (60U)
#endif
#ifndef IEEE802154_MAC_BASE_SLOT_DURATION
#define IEEE802154_MAC_BASE_SLOT_DURATION   CONFIG_IEEE802154_MAC_BASE_SLOT_DURATION
#endif

#ifndef CONFIG_IEEE802154_MAC_MAX_LOST_BEACONS
#define CONFIG_IEEE802154_MAC_MAX_LOST_BEACONS   (4U)
#endif
#ifndef IEEE802154_MAC_MAX_LOST_BEACONS
#define IEEE802154_MAC_MAX_LOST_BEACONS   CONFIG_IEEE802154_MAC_MAX_LOST_BEACONS
#endif

#ifndef CONFIG_IEEE802154_MAC_MIN_CAP_LENGTH
#define CONFIG_IEEE802154_MAC_MIN_CAP_LENGTH   (440U)
#endif
#ifndef IEEE802154_MAC_MIN_CAP_LENGTH
#define IEEE802154_MAC_MIN_CAP_LENGTH   CONFIG_IEEE802154_MAC_MIN_CAP_LENGTH
#endif

#ifndef CONFIG_IEEE802154_MAC_NUM_SUPERFRAME_SLOTS
#define CONFIG_IEEE802154_MAC_NUM_SUPERFRAME_SLOTS     (16)
#endif
#ifndef IEEE802154_MAC_NUM_SUPERFRAME_SLOTS
#define IEEE802154_MAC_NUM_SUPERFRAME_SLOTS     CONFIG_IEEE802154_MAC_NUM_SUPERFRAME_SLOTS
#endif

#ifndef IEEE802154_MAC_BASE_SUPERFRAME_DURATION
#define IEEE802154_MAC_BASE_SUPERFRAME_DURATION   (IEEE802154_MAC_BASE_SLOT_DURATION * \
                                                   IEEE802154_MAC_NUM_SUPERFRAME_SLOTS)
#endif

#ifndef CONFIG_IEEE802154_MAC_RX_RETRY_MAX
#define CONFIG_IEEE802154_MAC_RX_RETRY_MAX   (4U)
#endif
#ifndef IEEE802154_MAC_RX_RETRY_MAX
#define IEEE802154_MAC_RX_RETRY_MAX   CONFIG_IEEE802154_MAC_RX_RETRY_MAX
#endif

/**
 * @brief IEEE 802.15.4 extended adress
 */
typedef eui64_t ieee802154_ext_addr_t;
/**
 * @brief IEEE 802.15.4 MAC short address
 */
typedef network_uint16_t ieee802154_short_addr_t;

/**
 * @brief IEEE 802.15.4 octet (bytes) with len.
 */
typedef struct {
    const uint8_t *ptr;
    size_t len;
} ieee802154_octets_t;

/**
 * @brief IEEE 802.15.4 MAC PIB.
 */
typedef struct {
    bool AOA_ENABLE;
    bool AUTO_REQUEST;
    bool BATT_LIFE_EXT;
    uint16_t BATT_LIFE_EXT_PERIODS;
    uint8_t BEACON_ORDER;
    ieee802154_octets_t BEACON_PAYLOAD;
    uint8_t BSN;
    ieee802154_ext_addr_t COORD_EXTENDED_ADDRESS;
    ieee802154_short_addr_t COORD_SHORT_ADDRESS;
    uint8_t DSN;
    eui64_t EXTENDED_ADDRESS;
    uint8_t FCS_TYPE;
    bool GROUP_RX_MODE;
    bool GTS_PERMIT;
    bool IMPLICIT_BROADCAST;
    uint16_t LIFS_PERIOD;
    uint8_t MAX_BE;
    uint8_t MAX_CSMA_BACKOFFS;
    bool NOTIFY_ALL_BEACONS;
    uint8_t MIN_BE;
    uint16_t PAN_ID;
    uint8_t RESPONSE_WAIT_TIME;
    bool RX_ON_WHEN_IDLE;
    bool SECURITY_ENABLED;
    network_uint16_t SHORT_ADDR;
    uint16_t SIFS_PERIOD;
    uint16_t SYNC_SYMBOL_OFFSET;
    bool TIMESTAMP_SUPPORTED;
    uint16_t TRANSACTION_PERSISTENCE_TIME;
    uint16_t UNIT_BACKOFF_PERIOD;
} ieee802154_pib_t;

/**
 * @brief IEEE 802.15.4 MAC PIB Attributes.
 */
typedef enum {
    IEEE802154_PIB_AOA_ENABLE,
    IEEE802154_PIB_AUTO_REQUEST,
    IEEE802154_PIB_BATT_LIFE_EXT,
    IEEE802154_PIB_BATT_LIFE_EXT_PERIODS,
    IEEE802154_PIB_BEACON_ORDER,
    IEEE802154_PIB_BEACON_PAYLOAD,
    IEEE802154_PIB_BSN,
    IEEE802154_PIB_COORD_EXTENDED_ADDRESS,
    IEEE802154_PIB_COORD_SHORT_ADDRESS,
    IEEE802154_PIB_DSN,
    IEEE802154_PIB_EXTENDED_ADDRESS,
    IEEE802154_PIB_FCS_TYPE,
    IEEE802154_PIB_GROUP_RX_MODE,
    IEEE802154_PIB_GTS_PERMIT,
    IEEE802154_PIB_IMPLICIT_BROADCAST,
    IEEE802154_PIB_LIFS_PERIOD,
    IEEE802154_PIB_MAX_BE,
    IEEE802154_PIB_MAX_CSMA_BACKOFFS,
    IEEE802154_PIB_NOTIFY_ALL_BEACONS,
    IEEE802154_PIB_MIN_BE,
    IEEE802154_PIB_PAN_ID,
    IEEE802154_PIB_RESPONSE_WAIT_TIME,
    IEEE802154_PIB_RX_ON_WHEN_IDLE,
    IEEE802154_PIB_SECURITY_ENABLED,
    IEEE802154_PIB_SHORT_ADDR,
    IEEE802154_PIB_SIFS_PERIOD,
    IEEE802154_PIB_SYNC_SYMBOL_OFFSET,
    IEEE802154_PIB_TIMESTAMP_SUPPORTED,
    IEEE802154_PIB_TRANSACTION_PERSISTENCE_TIME,
    IEEE802154_PIB_UNIT_BACKOFF_PERIOD,
    IEEE802154_PIB_ATTR_COUNT
} ieee802154_pib_attr_t;

/**
 * @brief IEEE 802.15.4 MAC PIB data types.
 */
typedef enum {
    IEEE802154_PIB_TYPE_BOOL,   /**< bool */
    IEEE802154_PIB_TYPE_U8,     /**< uint8_t */
    IEEE802154_PIB_TYPE_U16,    /**< uint16_t */
    IEEE802154_PIB_TYPE_EUI64,  /**< ext address in ieee802154_ext_addr_t */
    IEEE802154_PIB_TYPE_NUI16,  /**< short address in ieee802154_short_addr_t */
    IEEE802154_PIB_TYPE_BYTES   /**< octets in  ieee802154_octets_t*/
} ieee802154_pib_type_t;

/**
 * @brief IEEE 802.15.4 device type identifier.
 */
typedef enum {
    IEEE802154_DEV_TYPE_CC2538_RF,
    IEEE802154_DEV_TYPE_NRF802154,
    IEEE802154_DEV_TYPE_SOCKET_ZEP,
    IEEE802154_DEV_TYPE_KW2XRF,
    IEEE802154_DEV_TYPE_MRF24J40,
    IEEE802154_DEV_TYPE_ESP_IEEE802154,
    IEEE802154_DEV_TYPE_INVALID,
} ieee802154_dev_type_t;

/**
 * @brief IEEE 802.15.4 address mode.
 */
typedef enum {
    IEEE802154_ADDR_MODE_NONE,
    IEEE802154_ADDR_MODE_SHORT,
    IEEE802154_ADDR_MODE_EXTENDED
} ieee802154_addr_mode_t;

/**
 * @brief IEEE 802.15.4 address container.
 */
typedef struct {
    ieee802154_addr_mode_t type;
    union {
        ieee802154_ext_addr_t ext_addr;
        ieee802154_short_addr_t short_addr;
    } v;
} ieee802154_addr_t;

/**
 * @brief IEEE 802.15.4 PIB attribute value.
 */
typedef struct {
    ieee802154_pib_type_t type;
    union {
        bool b;
        uint8_t u8;
        uint16_t u16;
        ieee802154_octets_t bytes;
        eui64_t ext_addr;
        network_uint16_t short_addr;
    } v;
} ieee802154_pib_value_t;

/**
 * @brief IEEE 802.15.4 PIB attribute access type.
 */
typedef enum {
    IEEE802154_PIB_ACC_RW,
    IEEE802154_PIB_ACC_RO
} ieee802154_pib_access_t;

/**
 * @brief IEEE 802.15.4 PIB attribute entry metadata.
 */
typedef struct {
    ieee802154_pib_type_t type;
    ieee802154_pib_access_t access;

    uint16_t offset;
    uint16_t size;

    ieee802154_pib_value_t def;
    ieee802154_pib_value_t min;
    ieee802154_pib_value_t max;
} ieee802154_pib_attr_entry_t;

/**
 * @brief IEEE 802.15.4 MAC MCPS-DATA.confirm callback.
 */
typedef void (*ieee802154_mcps_data_confirm_cb_t)(void *mac, uint8_t handle, int status);

/**
 * @brief IEEE 802.15.4 MAC MCPS-DATA.indication callback.
 */
typedef void (*ieee802154_mcps_data_indication_cb_t)(void *mac, iolist_t *msdu,
                                                     const ieee802154_rx_info_t *info);

/**
 * @brief IEEE 802.15.4 scan result entry.
 */
typedef struct {
    uint16_t channel;
    uint16_t pan_id;
    ieee802154_addr_t coord_addr;
    uint8_t lqi;
    uint8_t rssi;
    uint8_t beacon_payload_len;
    uint8_t beacon_payload[IEEE802154_SCAN_BEACON_PAYLOAD_MAX];
} ieee802154_scan_result_t;

/**
 * @brief IEEE 802.15.4 scan request parameters.
 */
typedef struct {
    const uint16_t *channels;
    uint8_t channel_count;
    uint32_t duration;
    ieee802154_scan_result_t *results;
    size_t results_len;
    size_t *results_used;
} ieee802154_mlme_scan_req_t;

/**
 * @brief IEEE 802.15.4 association capability information.
 */
typedef union {
    uint8_t u8;
    struct {
        uint8_t reserved0 : 1;
        uint8_t device_type : 1;
        uint8_t power_source : 1;
        uint8_t rx_on_when_idle : 1;
        uint8_t association_type : 1;
        uint8_t suspendable_csma_ca : 1;
        uint8_t security_capability : 1;
        uint8_t allocate_address : 1;
    } bits;
} ieee802154_assoc_capability_t;

/**
 * @brief IEEE 802.15.4 association status.
 */
typedef enum {
    IEEE802154_ASSOC_STATUS_SUCCESS = 0,
    IEEE802154_ASSOC_STATUS_PAN_AT_CAPACITY = 1,
    IEEE802154_ASSOC_STATUS_PAN_ACCESS_DENIED = 2,
} ieee802154_assoc_status_t;

/**
 * @brief IEEE 802.15.4 MAC MLME-SCAN.confirm callback.
 */
typedef void (*ieee802154_mlme_scan_confirm_cb_t)(void *mac, int status,
                                                  ieee802154_mlme_scan_req_t *request);

/* === MLME confirm callback types === */

/**
 * @brief IEEE 802.15.4 MAC MLME-SET.confirm callback.
 */
typedef void (*ieee802154_mlme_set_confirm_cb_t)(void *mac,
                                                 uint8_t handle,
                                                 int status,
                                                 ieee802154_pib_attr_t attr);

/**
 * @brief IEEE 802.15.4 MAC MLME-GET.confirm callback.
 */
typedef void (*ieee802154_mlme_get_confirm_cb_t)(void *mac,
                                                 uint8_t handle,
                                                 int status,
                                                 ieee802154_pib_attr_t attr,
                                                 ieee802154_pib_value_t value);

/**
 * @brief IEEE 802.15.4 MAC MLME-START.confirm callback.
 */
typedef void (*ieee802154_mlme_start_confirm_cb_t)(void *mac,
                                                   uint8_t handle,
                                                   int status);

/**
 * @brief IEEE 802.15.4 MAC MLME-ASSOCIATE.indication callback.
 */
typedef void (*ieee802154_mlme_associate_indication_cb_t)(void *mac,
                                                          const uint8_t *device_addr,
                                                          uint8_t device_addr_len,
                                                          ieee802154_addr_mode_t device_addr_mode,
                                                          ieee802154_assoc_capability_t cap);

/**
 * @brief IEEE 802.15.4 MAC MLME-ASSOCIATE.confirm callback.
 */
typedef void (*ieee802154_mlme_associate_confirm_cb_t)(void *mac,
                                                       int status,
                                                       uint16_t short_addr);

/**
 * @brief IEEE 802.15.4 MAC ACK timeout callback.
 */
typedef void (*ieee802154_mac_ack_timeout_fired_cb_t)(void *mac);

/**
 * @brief IEEE 802.15.4 MAC bottom-half request callback.
 */
typedef void (*ieee802154_mac_bh_request_cb_t)(void *mac);

/* === RADIO callbacks === */
/**
 * @brief IEEE 802.15.4 MAC radio event callback.
 */
typedef void (*ieee802154_radio_cb_request_t)(ieee802154_dev_t *dev, ieee802154_trx_ev_t st);

/* === Transmission callbacks === */
/**
 * @brief IEEE 802.15.4 MAC tick callback.
 */
typedef void (*ieee802154_mac_tick_t)(void *mac);
/**
 * @brief IEEE 802.15.4 MAC scan timer request callback.
 */
typedef void (*ieee802154_mac_scan_timer_request_t)(void *mac);
/**
 * @brief IEEE 802.15.4 MAC allocate request callback.
 */
typedef void (*ieee802154_mac_allocate_request_t)(void *mac, size_t len);
/**
 * @brief Schedule @ref ieee802154_mac_rx_request_process in thread context.
 *
 * @param[in] mac Pointer to the IEEE 802.15.4 MAC descriptor.
 */
typedef void (*ieee802154_mac_rx_request_t)(void *mac);
/**
 * @brief IEEE 802.15.4 MAC deallocation request callback.
 */
typedef void (*ieee802154_mac_dealloc_request_t)(void *mac, iolist_t *iolist);

/**
 * @brief IEEE 802.15.4 MAC callbacks.
 */
typedef struct {
    ieee802154_mcps_data_confirm_cb_t data_confirm;             /**< MCPS-DATA.confirm callback*/
    ieee802154_mcps_data_indication_cb_t data_indication;       /**< MCPS-DATA.indication callback*/
    ieee802154_mlme_scan_confirm_cb_t mlme_scan_confirm;        /**< MLME-SCAN.confirm callback */
    ieee802154_mlme_start_confirm_cb_t mlme_start_confirm;      /**< MLME-START.confirm callback */
    ieee802154_mlme_associate_indication_cb_t mlme_associate_indication; /**< MLME-ASSOCIATE.indication callback */
    ieee802154_mlme_associate_confirm_cb_t mlme_associate_confirm; /**< MLME-ASSOCIATE.confirm callback */
    ieee802154_mac_ack_timeout_fired_cb_t ack_timeout;          /**< ieee802154_mac_ack_timeout_fired() should be dispatched */
    ieee802154_radio_cb_request_t radio_cb_request;             /**< ieee802154_mac_handle_radio() should be dispatched */
    ieee802154_mac_scan_timer_request_t scan_timer_request;     /**< ieee802154_mac_scan_timer_process() should be dispatched */
    ieee802154_mac_tick_t tick_request;                         /**< ieee802154_mac_tick() should be dispatched */
    ieee802154_mac_allocate_request_t allocate_request;         /**< allocate TX queue entry ieee802154_mac_tx_process() should be dispatched */
    ieee802154_mac_dealloc_request_t dealloc_request;           /**< release TX queue entry */
    ieee802154_mac_rx_request_t rx_request;                     /**< RX request from MAC */
    void *mac;
} ieee802154_mac_cbs_t;

/**
 * @brief IEEE 802.15.4 MAC global state.
 */
typedef enum {
    IEEE802154_MAC_STATE_IDLE,
    IEEE802154_MAC_STATE_SCAN_ACTIVE,
    IEEE802154_MAC_STATE_ASSOCIATING,
    IEEE802154_MAC_STATE_COORDINATOR,
    IEEE802154_MAC_STATE_DEVICE,
    IEEE802154_MAC_STATE_SLEEP,
    IEEE802154_MAC_STATE_INVALID,
} ieee802154_mac_state_t;

/**
 * @brief IEEE 802.15.4 MAC TX descriptor state.
 */
typedef enum {
    IEEE802154_TX_STATE_QUEUED,
    IEEE802154_TX_STATE_IN_PROGRESS,
    IEEE802154_TX_STATE_DONE,
} ieee802154_tx_state_t;

/**
 * @brief IEEE 802.15.4 MAC TX descriptor.
 */
typedef struct {
    bool in_use;                                    /**< wheather ring buffer element is in use */
    uint8_t handle;                                 /**< the MSDU handle */
    uint8_t type;
    bool indirect;
    bool ack;
    ieee802154_tx_state_t tx_state;                 /**< current TX state */
    uint32_t deadline_tick;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];            /**< persistent header storage */
    iolist_t iol_mhr;                               /**< persistent mhr */
    iolist_t *iol_msdu;                             /**< iolist nodes */
} ieee802154_mac_tx_desc_t;

/**
 * @brief IEEE 802.15.4 MAC TX queue.
 */
typedef struct {
    ieee802154_mac_tx_desc_t q[IEEE802154_MAC_TXQ_LEN]; /**< outgoing queue */
    ieee802154_ext_addr_t dst_ext_addr;                 /**< key or destination extended addr */
    network_uint16_t dst_short_addr;                    /**< destination short addr (network order) */
    ieee802154_addr_mode_t key_mode;                    /**< key type for queue matching */
    ieee802154_addr_mode_t dst_mode;                    /**< destination addr mode for frame */
    bool has_dst_addr;
    uint8_t head;                                       /**< queue head */
    uint8_t tail;                                       /**< queue tail */
    uint8_t cnt;                                        /**< queue count */
    uint32_t *deadline_tick;
} ieee802154_mac_txq_t;

/**
 * @brief IEEE 802.15.4 MAC indirect transmission queue.
 */
typedef struct {
    ieee802154_mac_txq_t q[IEEE802154_MAC_TX_INDIRECTQ_SIZE];
    ieee802154_mac_txq_t *current_txq;
    unsigned current_slot;
    bool busy;
    uint32_t tick;
    uint32_t free_mask;
    mutex_t lock;
} ieee802154_mac_indirect_q_t;

/**
 * @brief IEEE 802.15.4 association address alias entry.
 */
typedef struct {
    ieee802154_ext_addr_t ext_addr;
    ieee802154_short_addr_t short_addr;
    bool valid;
} ieee802154_mac_assoc_entry_t;

/**
 * @brief IEEE 802.15.4 MAC descriptor
 */
typedef struct {
    bool is_coordinator;
    ieee802154_mac_state_t state;
    ieee802154_mac_state_t state_history;
    ieee802154_dev_type_t dev_type;
    ieee802154_pib_t pib;                                       /**< PIB of the MAC */
    mutex_t pib_lock;
    ieee802154_submac_t submac;                                 /**< SubMAC descriptor */
    mutex_t submac_lock;
    ieee802154_mac_cbs_t cbs;                                   /**< callbacks for the SubMAC */
    ztimer_t ack_timer;                                         /**< timer for ACK timeout */
    ztimer_t tick;                                              /**< tick for frame timeouts */
    uint8_t cmd_buf[IEEE802154_FRAME_LEN_MAX];                  /**< receiving buf */
    iolist_t cmd;
    ieee802154_mac_indirect_q_t indirect_q;
    ieee802154_mac_assoc_entry_t assoc_table[IEEE802154_MAC_ASSOC_TABLE_SIZE];
    uint16_t sym_us;
    ieee802154_mlme_scan_req_t *scan_req;
    bool scan_active;
    uint8_t scan_idx;
    ztimer_t scan_timer;
    bool scan_timer_pending;
    bool assoc_pending;
    uint32_t assoc_deadline_tick;
    bool poll_rx_active;
    uint32_t poll_rx_deadline;
    uint8_t rx_retry_count;
    bool rx_retry_pending;
} ieee802154_mac_t;

typedef enum {
    IEEE802154_SCAN_ACTIVE,
    IEEE802154_SCAN_PASSIVE,
    IEEE802154_SCAN_ED,
} ieee802154_scan_type_t;

/* === Function that has to be dispatched and called on callbacks */

/**
 * @brief Has to be called from thread context when ieee802154_mac_ack_timeout_fired_cb_t is called.
 */
void ieee802154_mac_ack_timeout_fired(ieee802154_mac_t *mac);

/**
 * @brief Has to be called from thread context when ieee802154_radio_cb_request_t is called.
 */
void ieee802154_mac_handle_radio(ieee802154_dev_t *dev, ieee802154_trx_ev_t st);

/**
 * @brief Process pending TX queue entries.
 */
void ieee802154_mac_rx_process(ieee802154_mac_t *mac, iolist_t *buf);

/**
 * @brief Handle periodic MAC tick.
 */
void ieee802154_mac_tick(ieee802154_mac_t *mac);

/**
 * @brief Process the active scan timer in thread context.
 */
void ieee802154_mac_scan_timer_process(ieee802154_mac_t *mac);

/**
 * @brief Process an RX request in thread context.
 *
 * Reschedule busy RX requests through the RX request callback, up to
 * IEEE802154_MAC_RX_RETRY_MAX times per recovery attempt.
 *
 * @param[in] mac Pointer to the MAC descriptor.
 */
void ieee802154_mac_rx_request_process(ieee802154_mac_t *mac);

/**
 * @brief Init the IEEE 802.15.4 MAC
 */
void ieee802154_mac_init(ieee802154_mac_t *mac,
                         const ieee802154_mac_cbs_t *cbs);

/**
 * @brief Initialize IEEE 802.15.4 MAC with explicit radio type
 *
 * @param[in] mac       pointer to MAC descriptor
 * @param[in] cbs       callback set
 * @param[in] dev_type  radio type to initialize
 */
void ieee802154_mac_init_with_devtype(ieee802154_mac_t *mac,
                                      const ieee802154_mac_cbs_t *cbs,
                                      ieee802154_dev_type_t dev_type);

/**
 * @brief Issue a MAC scan request.
 */
int ieee802154_mac_mlme_scan_request(ieee802154_mac_t *mac, ieee802154_scan_type_t type,
                                     ieee802154_mlme_scan_req_t *req);
/**
 * @brief Issue a MAC MLME-SET request.
 */
void ieee802154_mac_mlme_set_request(ieee802154_mac_t *mac,
                                     ieee802154_pib_attr_t attr,
                                     const ieee802154_pib_value_t *in);
/**
 * @brief Issue a MAC MLME-GET request.
 */
void ieee802154_mac_mlme_get_request(ieee802154_mac_t *mac,
                                     ieee802154_pib_attr_t attr,
                                     ieee802154_pib_value_t *out);
/**
 * @brief Issue a MAC MLME-START request.
 */
int ieee802154_mlme_start_request(ieee802154_mac_t *mac, uint16_t channel);
/**
 * @brief Issue a MAC MCPS-DATA request.
 */
int ieee802154_mcps_data_request(ieee802154_mac_t *mac,
                                 ieee802154_addr_mode_t src_mode,
                                 ieee802154_addr_mode_t dst_mode,
                                 uint16_t dst_panid,
                                 const void *dst_addr,
                                 iolist_t *msdu,
                                 uint8_t msdu_handle,
                                 bool ack_req,
                                 bool indirect);
/**
 * @brief Issue a MAC MLME-ASSOCIATE request.
 *
 * @note The coordinator address is expected in network byte order.
 */
int ieee802154_mac_mlme_associate_request(ieee802154_mac_t *mac,
                                          const ieee802154_addr_t *coord_addr,
                                          uint16_t channel_num,
                                          uint16_t coord_panid,
                                          ieee802154_assoc_capability_t capability);
/**
 * @brief Issue a MAC MLME-ASSOCIATE response (coordinator).
 *
 * @note The destination address is expected in network byte order.
 */
int ieee802154_mac_mlme_associate_response(ieee802154_mac_t *mac,
                                           const ieee802154_addr_t *dst_addr,
                                           ieee802154_assoc_status_t status,
                                           uint16_t short_addr);
/**
 * @brief Issue a MAC MLME-POLL request.
 */
int ieee802154_mac_mlme_poll(ieee802154_mac_t *mac, ieee802154_addr_mode_t coord_mode,
                             uint16_t coord_panid, const void *coord_addr);
/**
 * @brief Issue a MAC MCPS-DATA request using the queued descriptor.
 */
int ieee802154_mac_mcps_data(void);
/**
 * @brief Issue a MAC MCPS-RESET request.
 */
int ieee802154_mac_mcps_reset(void);
/**
 * @brief Issue a MAC MLME-PURGE request.
 */
int ieee802154_mac_mlme_purge(void);

#ifdef __cplusplus
}
#endif

/** @} */
