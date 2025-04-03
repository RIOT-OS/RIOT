/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_ASYMCUTE_H
#define NET_ASYMCUTE_H

/**
 * @defgroup    net_asymcute MQTT-SN Client (Asymcute)
 * @ingroup     net
 * @brief       Asymcute is an asynchronous MQTT-SN implementation
 *
 * # About
 * `Asymcute` is a asynchronous MQTT-SN client implementation, aiming at
 * providing the user a high degree of flexibility. It provides a flexible
 * interface that allows users to issue any number of concurrent requests to
 * one or more different gateways simultaneously.
 *
 * # Implementation state
 *
 * Implemented features:
 * - Connecting to multiple gateways simultaneously
 * - Registration of topic names
 * - Publishing of data (QoS 0 and QoS 1)
 * - Subscription to topics
 * - Pre-defined topic IDs as well as short and normal topic names
 *
 * Missing features:
 * - Gateway discovery process not implemented
 * - Last will feature not implemented
 * - No support for QoS level 2
 * - No support for QoS level -1
 * - No support for wildcard characters in topic names when subscribing
 * - Actual granted QoS level on subscription is ignored
 *
 * @{
 * @file
 * @brief       Asymcute MQTT-SN interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "assert.h"
#include "event/timeout.h"
#include "event/callback.h"
#include "net/mqttsn.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_asymcute_conf Asymcute (MQTT-SN Client) compile configurations
 * @ingroup net_mqtt_conf
 * @brief   Compile-time configuration options for Asymcute, an asynchronous
 *          MQTT-SN implementation based on the OASIS MQTT-SN protocol. It
 *          provides a flexible interface that allows users to issue any number
 *          of concurrent requests to one or more different gateways
 *          simultaneously.
 * @{
 */
/**
 * @brief   Default UDP port to listen on. Usage can be found in
 *          examples/networking/mqtt/asymcute_mqttsn. Application code is expected to use this
 *          macro to assign the default port.
 */
#ifndef CONFIG_ASYMCUTE_DEFAULT_PORT
#define CONFIG_ASYMCUTE_DEFAULT_PORT    (1883U)
#endif

/**
 * @brief   Default buffer size used for receive and request buffers
 */
#ifndef CONFIG_ASYMCUTE_BUFSIZE
#define CONFIG_ASYMCUTE_BUFSIZE         (128U)
#endif

/**
 * @brief   Maximum topic length
 *
 * @note    Must be less than (256 - 8) AND less than ( @ref CONFIG_ASYMCUTE_BUFSIZE - 8).
 */
#ifndef CONFIG_ASYMCUTE_TOPIC_MAXLEN
#define CONFIG_ASYMCUTE_TOPIC_MAXLEN    (32U)
#endif

/**
 * @brief   Keep alive interval [in s] communicated to the gateway
 *
 * Keep alive interval in seconds which is communicated to the gateway in the
 * CONNECT message. For more information, see MQTT-SN Spec v1.2, section 5.4.4.
 * For default values,see section 7.2 -> TWAIT: > 5 min.
 */
#ifndef CONFIG_ASYMCUTE_KEEPALIVE
#define CONFIG_ASYMCUTE_KEEPALIVE       (360)
#endif

/**
 * @brief   Interval to use for sending periodic ping messages
 *
 * The default behavior of this implementation is to send ping messages as soon
 * as three quarters of the keep alive interval have passed.
 *
 * @note    Must be less than @ref CONFIG_ASYMCUTE_KEEPALIVE
 */
#ifndef CONFIG_ASYMCUTE_KEEPALIVE_PING
#define CONFIG_ASYMCUTE_KEEPALIVE_PING  ((CONFIG_ASYMCUTE_KEEPALIVE / 4) * 3)
#endif

/**
 * @brief   Resend interval [in seconds]
 *
 * Interval used for timing the retry messages which are sent when the expected
 * reply from GW is not received. The retry timer is started by the client when
 * the message is sent and stopped when the expected reply from GW is received.
 * If the timer times out and the expected GW’s reply is not received, the
 * client retransmits the message. For more information, see MQTT-SN Spec v1.2,
 * section 6.13. For default values, see section 7.2 -> Tretry: 10 to 15 sec.
 */
#ifndef CONFIG_ASYMCUTE_T_RETRY
#define CONFIG_ASYMCUTE_T_RETRY         (10U)
#endif

/**
 * @brief   Number of retransmissions until requests time out
 *
 * Maximum number of retransmissions in the event that the retry timer times
 * out. After 'CONFIG_ASYMCUTE_N_RETRY' number of retransmissions, the client
 * aborts the procedure and assumes that its MQTT-SN connection to the gateway
 * is disconnected. For more information, see MQTT-SN Spec v1.2, section 6.13.
 * For default values, see section 7.2 -> Nretry: 3-5.
 */
#ifndef CONFIG_ASYMCUTE_N_RETRY
#define CONFIG_ASYMCUTE_N_RETRY         (3U)
#endif
/** @} */

#ifndef ASYMCUTE_HANDLER_PRIO
/**
 * @brief   Default priority for Asymcute's handler thread
 */
#define ASYMCUTE_HANDLER_PRIO           (THREAD_PRIORITY_MAIN - 2)
#endif

#ifndef ASYMCUTE_HANDLER_STACKSIZE
/**
 * @brief   Default stack size for Asymcute's handler thread
 */
#define ASYMCUTE_HANDLER_STACKSIZE      (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Return values used by public Asymcute functions
 */
enum {
    ASYMCUTE_OK         =  0,       /**< all is good */
    ASYMCUTE_OVERFLOW   = -1,       /**< error: insufficient buffer space */
    ASYMCUTE_GWERR      = -2,       /**< error: bad gateway connection state */
    ASYMCUTE_NOTSUP     = -3,       /**< error: feature not supported */
    ASYMCUTE_BUSY       = -4,       /**< error: context already in use */
    ASYMCUTE_REGERR     = -5,       /**< error: registration invalid */
    ASYMCUTE_SUBERR     = -6,       /**< error: subscription invalid */
    ASYMCUTE_SENDERR    = -7,       /**< error: unable to sent packet */
};

/**
 * @brief   Possible event types passed to the event callback
 */
enum {
    ASYMCUTE_TIMEOUT,               /**< request timed out */
    ASYMCUTE_CANCELED,              /**< request was canceled */
    ASYMCUTE_REJECTED,              /**< request was rejected */
    ASYMCUTE_CONNECTED,             /**< connected to gateway */
    ASYMCUTE_DISCONNECTED,          /**< connection got disconnected */
    ASYMCUTE_REGISTERED,            /**< topic was registered */
    ASYMCUTE_PUBLISHED,             /**< data was published */
    ASYMCUTE_SUBSCRIBED,            /**< client was subscribed to topic */
    ASYMCUTE_UNSUBSCRIBED,          /**< client was unsubscribed from topic */
};

/**
 * @brief   Forward type declaration for connections contexts
 */
typedef struct asymcute_con asymcute_con_t;

/**
 * @brief   Forward type declaration for request contexts
 */
typedef struct asymcute_req asymcute_req_t;

/**
 * @brief   Forward type declaration for subscription contexts
 */
typedef struct asymcute_sub asymcute_sub_t;

/**
 * @brief   Forward type declaration for topic definitions
 */
typedef struct asymcute_topic asymcute_topic_t;

/**
 * @brief   Forward type declaration for last will definitions
 */
typedef struct asymcute_will asymcute_will_t;

/**
 * @brief   Event callback used for communicating connection and request related
 *          events to the user
 *
 * @param[in] req       pointer to the request context that triggered the event,
 *                      may be NULL of unsolicited events
 * @param[in] evt_type  type of the event
 */
typedef void(*asymcute_evt_cb_t)(asymcute_req_t *req, unsigned evt_type);

/**
 * @brief   Callback triggered on events for active subscriptions
 *
 * @param[in] sub       pointer to subscription context triggering this event
 * @param[in] evt_type  type of the event
 * @param[in] data      incoming data for PUBLISHED events, may be NULL
 * @param[in] len       length of @p data in bytes
 * @param[in] arg       user supplied argument
 */
typedef void(*asymcute_sub_cb_t)(const asymcute_sub_t *sub, unsigned evt_type,
                                 const void *data, size_t len, void *arg);

/**
 * @brief   Context specific timeout callback, only used internally
 *
 * @internal
 *
 * @param[in] con       connection context for this timeout
 * @param[in] req       request that timed out
 *
 * @return  Event type to communicate to the user
 */
typedef unsigned(*asymcute_to_cb_t)(asymcute_con_t *con, asymcute_req_t *req);

/**
 * @brief   Asymcute request context
 */
struct asymcute_req {
    mutex_t lock;                   /**< synchronization lock */
    struct asymcute_req *next;      /**< the requests list entry */
    asymcute_con_t *con;            /**< connection the request is using */
    asymcute_to_cb_t cb;            /**< internally used callback */
    void *arg;                      /**< internally used additional state */
    event_callback_t to_evt;        /**< timeout event */
    event_timeout_t to_timer;       /**< timeout timer */
    uint8_t data[CONFIG_ASYMCUTE_BUFSIZE]; /**< buffer holding the request's data */
    size_t data_len;                /**< length of the request packet in byte */
    uint16_t msg_id;                /**< used message id for this request */
    uint8_t retry_cnt;              /**< retransmission counter */
};

/**
 * @brief   Asymcute connection context
 */
struct asymcute_con {
    mutex_t lock;                       /**< synchronization lock */
    sock_udp_t sock;                    /**< socket used by a connections */
    asymcute_req_t *pending;            /**< list holding pending requests */
    asymcute_sub_t *subscriptions;      /**< list holding active subscriptions */
    asymcute_evt_cb_t user_cb;          /**< event callback provided by user */
    event_callback_t keepalive_evt;     /**< keep alive event */
    event_timeout_t keepalive_timer;    /**< keep alive timer */
    uint16_t last_id;                   /**< last used message ID for this
                                         *   connection */
    uint8_t keepalive_retry_cnt;        /**< keep alive transmission counter */
    uint8_t state;                      /**< connection state */
    uint8_t rxbuf[CONFIG_ASYMCUTE_BUFSIZE];    /**< connection specific receive buf */
    char cli_id[MQTTSN_CLI_ID_MAXLEN + 1];  /**< buffer to store client ID */
};

/**
 * @brief   Data-structure for holding topics and their registration status
 */
struct asymcute_topic {
    asymcute_con_t *con;        /**< connection used for registration */
    char name[CONFIG_ASYMCUTE_TOPIC_MAXLEN + 1];   /**< topic string (ASCII only) */
    uint8_t flags;              /**< normal, short, or pre-defined */
    uint16_t id;                /**< topic id */
};

/**
 * @brief   Data-structure holding the state of subscriptions
 */
struct asymcute_sub {
    asymcute_sub_t *next;       /**< the subscriptions list entry */
    asymcute_topic_t *topic;    /**< topic we subscribe to */
    asymcute_sub_cb_t cb;       /**< called on incoming data */
    void *arg;                  /**< user supplied callback argument */
};

/**
 * @brief   Data structure for defining a last will
 */
struct asymcute_will {
    const char *topic;          /**< last will topic */
    void *msg;                  /**< last will message content */
    size_t msg_len;             /**< length of last will message content */
};

/**
 * @brief   Check if a given request context is currently used
 *
 * @param[in] req       request context to check
 *
 * @return  true if context is currently used
 * @return  false if context is not used
 */
static inline bool asymcute_req_in_use(const asymcute_req_t *req)
{
    assert(req);
    return (req->con != NULL);
}

/**
 * @brief   Check if a given subscription is currently active
 *
 * @param[in] sub       subscription context to check
 *
 * @return  true if subscription is active
 * @return  false if subscription is not active
 */
static inline bool asymcute_sub_active(const asymcute_sub_t *sub)
{
    assert(sub);
    return (sub->topic != NULL);
}

/**
 * @brief   Reset the given topic
 *
 * @warning Make sure that the given topic is not used by any subscription or
 *          last will when calling this function
 *
 * @param[out] topic    topic to reset
 */
static inline void asymcute_topic_reset(asymcute_topic_t *topic)
{
    assert(topic);
    memset(topic, 0, sizeof(asymcute_topic_t));
}

/**
 * @brief   Check if a given topic is currently registered with a gateway
 *
 * @param[in] topic     topic to check
 *
 * @return  true if topic is registered
 * @return  false if topic is not registered
 */
static inline bool asymcute_topic_is_reg(const asymcute_topic_t *topic)
{
    assert(topic);
    return (topic->con != NULL);
}

/**
 * @brief   Check if a given topic is a short topic
 *
 * @param[in] topic     topic to check
 *
 * @return  true if topic is a short topic
 * @return  false if topic is not short topic
 */
static inline bool asymcute_topic_is_short(const asymcute_topic_t *topic)
{
    assert(topic);
    return ((topic->flags & MQTTSN_TIT_SHORT) != 0);
}

/**
 * @brief   Check if a given topic is a pre-defined topic
 *
 * @param[in] topic     topic to check
 *
 * @return  true if topic is pre-defined
 * @return  false if topic is not pre-defined
 */
static inline bool asymcute_topic_is_predef(const asymcute_topic_t *topic)
{
    assert(topic);
    return ((topic->flags & MQTTSN_TIT_PREDEF) != 0);
}

/**
 * @brief   Check if a given topic is initialized
 *
 * @param[in] topic     topic to check
 *
 * @return  true if topic is initialized
 * @return  false if topic is not initialized
 */
static inline bool asymcute_topic_is_init(const asymcute_topic_t *topic)
{
    assert(topic);
    return (topic->name[0] != '\0');
}

/**
 * @brief   Compare two given topics and check if they are equal
 *
 * @param[in] a     topic A
 * @param[in] b     topic B
 *
 * @return  true if both topics are equal
 * @return  false if topics differ
 */
static inline bool asymcute_topic_equal(const asymcute_topic_t *a,
                                        const asymcute_topic_t *b)
{
    assert(a);
    assert(b);

    return ((a->flags == b->flags) && (a->id == b->id));
}

/**
 * @brief   Initialize the given topic
 *
 * @param[out] topic        topic to initialize
 * @param[in] topic_name    topic name (ASCII), may be NULL if topic should use
 *                          a pre-defined topic ID
 * @param[in] topic_id      pre-defined topic ID, or don't care if @p topic_name
 *                          is given
 *
 * @return  ASYMCUTE_OK on success
 * @return  ASYMCUTE_REGERR if topic is already registered
 * @return  ASYMCUTE_OVERFLOW if topic name does not fit into buffer or if pre-
 *          defined topic ID is invalid
 */
int asymcute_topic_init(asymcute_topic_t *topic, const char *topic_name,
                        uint16_t topic_id);

/**
 * @brief   Start the global Asymcute handler thread for processing timeouts and
 *          keep alive events
 *
 * This function is typically called during system initialization.
 */
void asymcute_handler_run(void);

/**
 * @brief   Check if the given connection context is connected to a gateway
 *
 * @param[in] con       connection to check
 *
 * @return  true if context is connected
 * @return  false if not connected
 */
bool asymcute_is_connected(const asymcute_con_t *con);

/**
 * @brief   Connect to the given MQTT-SN gateway
 *
 * @param[in,out] con   connection to use
 * @param[in,out] req   request context to use for CONNECT procedure
 * @param[in] server    UDP endpoint of the target gateway
 * @param[in] cli_id    client ID to register with the gateway
 * @param[in] clean     set `true` to start a clean session
 * @param[in] will      last will (currently not implemented)
 * @param[in] callback  user callback triggered on defined events
 *
 * @return  ASYMCUTE_OK if CONNECT message has been sent
 * @return  ASYMCUTE_NOTSUP if last will was given (temporary until implemented)
 * @return  ASYMCUTE_OVERFLOW if @p cli_id is larger than ASYMCUTE_ID_MAXLEN
 * @return  ASYMCUTE_GWERR if initializing the socket for the connection failed
 * @return  ASYMCUTE_BUSY if the connection or the request context are in use
 */
int asymcute_connect(asymcute_con_t *con, asymcute_req_t *req,
                     sock_udp_ep_t *server, const char *cli_id, bool clean,
                     asymcute_will_t *will, asymcute_evt_cb_t callback);

/**
 * @brief   Close the given connection
 *
 * @param[in,out] con   connection to close
 * @param[in,out] req   request context to use for DISCONNECT procedure
 *
 * @return  ASYMCUTE_OK if DISCONNECT message has been sent
 * @return  ASYMCUTE_GWERR if connection context is not connected
 * @return  ASYMCUTE_BUSY if the given request context is already in use
 */
int asymcute_disconnect(asymcute_con_t *con, asymcute_req_t *req);

/**
 * @brief   Register a given topic with the connected gateway
 *
 * @param[in] con       connection to use
 * @param[in,out] req   request context to use for REGISTER procedure
 * @param[in,out] topic topic to register
 *
 * @return  ASYMCUTE_OK if REGISTER message has been sent
 * @return  ASYMCUTE_REGERR if topic is already registered
 * @return  ASYMCUTE_GWERR if not connected to a gateway
 * @return  ASYMCUTE_BUSY if the given request context is already in use
 */
int asymcute_register(asymcute_con_t *con, asymcute_req_t *req,
                      asymcute_topic_t *topic);

/**
 * @brief   Publish the given data to the given topic
 *
 * @param[in] con       connection to use
 * @param[in,out] req   request context used for PUBLISH procedure
 * @param[in] topic     publish data to this topic
 * @param[in] data      actual payload to send
 * @param[in] data_len  size of @p data in bytes
 * @param[in] flags     additional flags (QoS level, DUP, and RETAIN)
 *
 * @return  ASYMCUTE_OK if PUBLISH message has been sent
 * @return  ASYMCUTE_NOTSUP if unsupported flags have been set
 * @return  ASYMCUTE_OVERFLOW if data does not fit into transmit buffer
 * @return  ASYMCUTE_REGERR if given topic is not registered
 * @return  ASYMCUTE_GWERR if not connected to a gateway
 * @return  ASYMCUTE_BUSY if the given request context is already in use
 */
int asymcute_publish(asymcute_con_t *con, asymcute_req_t *req,
                     const asymcute_topic_t *topic,
                     const void *data, size_t data_len, uint8_t flags);

/**
 * @brief   Subscribe to a given topic
 *
 * @param[in] con       connection to use
 * @param[in,out] req   request context used for SUBSCRIBE procedure
 * @param[out] sub      subscription context to store subscription state
 * @param[in,out] topic topic to subscribe to, must be initialized (see
 *                      asymcute_topic_init())
 * @param[in] callback  user callback triggered on events for this subscription
 * @param[in] arg       user supplied argument passed to the event callback
 * @param[in] flags     additional flags (QoS level and DUP)
 *
 * @return  ASYMCUTE_OK if SUBSCRIBE message has been sent
 * @return  ASYMCUTE_NOTSUP if invalid or unsupported flags have been set
 * @return  ASYMCUTE_REGERR if topic is not initialized
 * @return  ASYMCUTE_GWERR if not connected to a gateway
 * @return  ASYMCUTE_SUBERR if already subscribed to the given topic
 * @return  ASYMCUTE_BUSY if the given request context is already in use
 */
int asymcute_subscribe(asymcute_con_t *con, asymcute_req_t *req,
                       asymcute_sub_t *sub, asymcute_topic_t *topic,
                       asymcute_sub_cb_t callback, void *arg, uint8_t flags);

/**
 * @brief   Cancel an active subscription
 *
 * @param[in] con       connection to use
 * @param[in,out] req   request context used for UNSUBSCRIBE procedure
 * @param[in,out] sub   subscription to cancel
 *
 * @return  ASYMCUTE_OK if UNSUBSCRIBE message has been sent
 * @return  ASYMCUTE_SUBERR if subscription is not currently active
 * @return  ASYMCUTE_GWERR if not connected to a gateway
 * @return  ASYMCUTE_BUSY if the given request context is already in use
 */
int asymcute_unsubscribe(asymcute_con_t *con, asymcute_req_t *req,
                         asymcute_sub_t *sub);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_ASYMCUTE_H */
