/*
 * Copyright (C) 2023 Fabian Hüßler ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_wifi_manager Wi-Fi manager
 * @ingroup     net
 * @brief       Provides the Wi-Fi manager interface
 * @{
 *
 * @file
 * @brief       Definitions for the Wi-Fi manager
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef NET_WIFI_MANAGER_H
#define NET_WIFI_MANAGER_H

#include "modules.h"
#include "list.h"
#include "net/netif.h"
#include "net/wifi.h"
#include "net/netdev.h"
#include "net/netdev/wifi.h"
#include "ztimer/periodic.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CONFIG_WIFI_MANAGER_MSG_QUEUE_SIZE) || defined(DOXYGEN)
/**
 * @brief   Wi-Fi manager message queue size
 */
#define CONFIG_WIFI_MANAGER_MSG_QUEUE_SIZE          4
#endif

/**
 * @brief   @ref CONFIG_WIFI_MANAGER_MSG_QUEUE_SIZE
 */
#define WIFI_MANAGER_MSG_QUEUE_SIZE                 CONFIG_WIFI_MANAGER_MSG_QUEUE_SIZE

#if !defined(CONFIG_WIFI_MANAGER_STACK_SIZE) || defined(DOXYGEN)
/**
 * @brief   Wi-Fi manager stack size
 */
#define CONFIG_WIFI_MANAGER_STACK_SIZE              THREAD_STACKSIZE_MEDIUM
#endif

/**
 * @brief   @ref CONFIG_WIFI_MANAGER_STACK_SIZE
 */
#define WIFI_MANAGER_STACK_SIZE                     CONFIG_WIFI_MANAGER_STACK_SIZE

#if !defined(CONFIG_WIFI_MANAGER_SCAN_LIST_SIZE) || defined(DOXYGEN)
/**
 * @brief   Wi-Fi manager scan list size
 */
#define CONFIG_WIFI_MANAGER_SCAN_LIST_SIZE          3
#endif

/**
 * @brief   @ref CONFIG_WIFI_MANAGER_SCAN_LIST_SIZE
 */
#define WIFI_MANAGER_SCAN_LIST_SIZE                 CONFIG_WIFI_MANAGER_SCAN_LIST_SIZE

#if !defined(CONFIG_WIFI_MANAGER_NETIF_CTX_SIZE) || defined(DOXYGEN)
/**
 * @brief   Number of Wi-Fi interfaces that can be managed
 */
#define CONFIG_WIFI_MANAGER_NETIF_CTX_SIZE          1
#endif

/**
 * @brief   @ref CONFIG_WIFI_MANAGER_NETIF_CTX_SIZE
 */
#define WIFI_MANAGER_NETIF_CTX_SIZE                 CONFIG_WIFI_MANAGER_NETIF_CTX_SIZE

#if !defined(CONFIG_WIFI_MANAGER_AUTOSCAN_TIMEOUT_SEC) || defined(DOXYGEN)
/**
 * @brief   Regular timeout after which an unconnected Wi-Fi interface scans for
 *          access points and tries to automatically connect
 */
#define CONFIG_WIFI_MANAGER_AUTOSCAN_TIMEOUT_SEC    30
#endif

/**
 * @brief   @ref CONFIG_WIFI_MANAGER_AUTOSCAN_TIMEOUT_SEC
 */
#define WIFI_MANAGER_AUTOSCAN_TIMEOUT_SEC           CONFIG_WIFI_MANAGER_AUTOSCAN_TIMEOUT_SEC

/**
 * @brief   Wi-Fi manager event type enumeration
 */
typedef enum {
    WIFI_MANAGER_EVENT_DO_SCAN                      = 1u << 0,   /**< Triggers a scan */
    WIFI_MANAGER_EVENT_SCAN_DONE                    = 1u << 1,   /**< Indicates a complete scan */
    WIFI_MANAGER_EVENT_CONNECTED                    = 1u << 2,   /**< Successfully connected */
    WIFI_MANAGER_EVENT_DISCONNECTED                 = 1u << 3,   /**< Disconnected */
    WIFI_MANAGER_EVENT_ENABLE                       = 1u << 4,   /**< Enable management */
    WIFI_MANAGER_EVENT_DISABLE                      = 1u << 5,   /**< Disable management */
} wifi_manager_event_t;

/**
 * @brief   Wi-Fi manager event type
 */
typedef struct wifi_manager_event_ctx {
    wifi_manager_event_t event;                                 /**< Event type */
    union {
        wifi_scan_result_t scan[WIFI_MANAGER_SCAN_LIST_SIZE];   /**< Scan result */
        wifi_connect_result_t connect;                          /**< Connected result */
        wifi_disconnect_result_t disconnect;                    /**< Disconnected result */
    } result;                                                   /**< Union of events */
} wifi_manager_event_ctx_t;

/**
 * @brief   Application connection hint type
 */
typedef struct wifi_manager_connection_hint {
    const char *ssid;                                   /**< Prefer this SSID */
    const wifi_security_mode_t *sec;                    /**< Use these credentials */
} wifi_manager_connection_hint_t;

/**
 * @brief   Managed Wi-Fi interface type
 */
typedef struct wifi_manager_netif_ctx {
    kernel_pid_t netif_pid;                             /**< ID of the managed Wi-Fi interface */
    const wifi_manager_connection_hint_t *hint;         /**< Connection hints for this interface */
    ztimer_periodic_t timer;                            /**< Network scan timer */
    bool no_periodic_scan;                              /**< True if the periodic scan timer should not be activated */
} wifi_manager_netif_ctx_t;

/**
 * @brief   Wi-Fi manager callback on network scan
 *
 * @param[in]           netif           Interface handle
 * @param[in]           scan            List of scan results
 */
typedef void (*wifi_manager_on_scan_cb) (void *netif, const wifi_scan_list_t *scan);

/**
 * @brief   Wi-Fi manager callback on network connect
 *
 * @param[in]           netif           Interface handle
 * @param[in]           connect         Connect result
 */
typedef void (*wifi_manager_connect_cb) (void *netif, const wifi_connect_result_t *connect);

/**
 * @brief   Wi-Fi manager callback on network disconnect
 *
 * @param[in]           netif           Interface handle
 * @param[in]           connect         Connect result
 */
typedef void (*wifi_manager_disconnect_cb) (void *netif, const wifi_disconnect_result_t *disconnect);

/**
 * @brief   Wi-Fi manager event listener type
 */
typedef struct wifi_manager_listener {
    list_node_t node;                                   /**< List node */
    wifi_manager_on_scan_cb on_scan;                    /**< Listener scan callback */
    wifi_manager_connect_cb on_connect;                 /**< Listener Connect callback */
    wifi_manager_disconnect_cb on_disconnect;           /**< Listener disconnect callback */
} wifi_manager_listener_t;

/**
 * @brief   Start the Wi-Fi manager thread
 */
void wifi_manager_start(void);

/**
 * @brief   Enable management of added Wi-Fi interfaces
 *
 * This enables the internal timer for periodic scanning and connection attempts
 */
void wifi_manager_enable(void);

/**
 * @brief   Pause management of added Wi-Fi interfaces
 *
 * This cancels the internal timer for periodic scanning and connection attempts
 */
void wifi_manager_disable(void);

/**
 * @brief   Get a managed Wi-Fi interface added with @ref wifi_manager_netif_add_by_pid
 *          by its ID
 *
 * @param[in]           netif_pid           Interface ID
 *
 * @return  Read-only reference to the managed interface
 */
const wifi_manager_netif_ctx_t *wifi_manager_netif_get_by_pid(kernel_pid_t netif_pid);

/**
 * @brief   Add a Wi-Fi interface to be managed by the Wi-Fi manager
 *
 * @param[in]           netif_pid           Interface ID
 * @param[in]           hint                Connection hint for this interface
 * @param[in]           periodi_scan        True if a periodic scan should be initiated
 *                                          to try to automatically connect o saved APs
 *
 * @return  Read-only reference to the managed interface
 */
const wifi_manager_netif_ctx_t *wifi_manager_netif_add_by_pid(kernel_pid_t netif_pid,
                                                              const wifi_manager_connection_hint_t *hint,
                                                              bool periodic_scan);

/**
 * @brief   Register a listener for Wi-Fi events
 *
 * @param[in, out]      listener            Listener with event callbacks
 */
void wifi_manager_register_listener(wifi_manager_listener_t *listener);

/**
 * @brief   Deregister a listener for Wi-Fi events
 *
 * @param[in, out]      listener            Listener with event callbacks
 */
void wifi_manager_deregister_listener(wifi_manager_listener_t *listener);

/**
 * @brief   Trigger a network scan of the Wi-Fi interface with the ID @p netif_pid
 *
 * @param[in]           netif_pid           Interface ID
 *
 * @return  0 on success
 */
int wifi_manager_scan(kernel_pid_t netif_pid);

/**
 * @brief   Try to connect to a network on the Wi-Fi interface with the ID @p netif_pid
 *
 * @param[in]           netif_pid           Interface ID
 * @param[in]           ssid                SSID of the access point to connect to
 * @param[in]           credentials         Credentials
 * @param[in]           channel             Access point channel
 *
 * @return  0 on success
 */
int wifi_manager_connect(kernel_pid_t netif_pid,
                         const char *ssid,
                         const wifi_security_mode_t *credentials,
                         unsigned channel);

/**
 * @brief   Disconnect from a network on the Wi-Fi interface with the ID @p netif_pid
 *
 * @param[in]           netif_pid           Interface ID
 *
 * @return  0 on success
 */
int wifi_manager_disconnect(kernel_pid_t netif_pid);

#ifdef __cplusplus
}
#endif

#endif /* NET_WIFI_MANAGER_H */
/** @} */
