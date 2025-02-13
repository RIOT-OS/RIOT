/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_emcute MQTT-SN Client (emCute)
 * @ingroup     net
 * @brief       emCute, the MQTT-SN implementation for RIOT
 *
 * # About
 * emCute is the implementation of the OASIS MQTT-SN protocol for RIOT. It is
 * designed with a focus on small memory footprint and usability.
 *
 *
 * # Design Decisions and Restrictions
 * * emCute is designed to run on top of UDP only, making use of
 * @ref net_sock_udp. The design is not intended to be used with any other
 * transport.
 *
 * The implementation is based on a 2-thread model: emCute needs one thread of
 * its own, in which receiving of packets and sending of ping messages are
 * handled. All 'user space functions' have to run from (a) different (i.e.
 * user) thread(s). emCute uses thread flags to synchronize between threads.
 *
 * Further know restrictions are:
 * - ASCII topic names only (no support for UTF8 names, yet)
 * - topic length is restricted to fit in a single length byte (248 byte max)
 * - no support for wildcards in topic names. This feature requires more
 *   elaborate internal memory management, supposedly at the cost of quite
 *   increased ROM and RAM usage
 * - no retransmit when receiving a REJ_CONG (reject, reason congestion). when
 *   getting a REJ_CONG (reject, reason congestion), the spec tells us to resend
 *   the original message after T_WAIT (default: >5min). This is not supported,
 *   as this would require to block to calling thread (or keep state) for long
 *   periods of time and is (in Hauke's opinion) not feasible for constrained
 *   nodes.
 *
 *
 * # Error Handling
 * This implementation tries minimize parameter checks to a minimum, checking as
 * many parameters as feasible using assertions. For the sake of run-time
 * stability and usability, typical overflow checks are always done during run-
 * time and explicit error values returned in case of errors.
 *
 *
 * # Implementation state
 * In the current state, emCute supports:
 * - connecting to a gateway
 * - disconnecting from gateway
 * - registering a last will topic and message during connection setup
 * - registering topic names with the gateway (obtaining topic IDs)
 * - subscribing to topics
 * - unsubscribing from topics
 * - updating will topic
 * - updating will message
 * - sending out periodic PINGREQ messages
 * - handling re-transmits
 *
 * The following features are however still missing (but planned):
 * @todo        Gateway discovery (so far there is no support for handling
 *              ADVERTISE, GWINFO, and SEARCHGW). Open question to answer here:
 *              how to put / how to encode the IPv(4/6) address AND the port of
 *              a gateway in the GwAdd field of the GWINFO message
 * @todo        QOS level 2
 * @todo        QOS level -1
 * @todo        put the node to sleep (send DISCONNECT with duration field set)
 * @todo        handle DISCONNECT messages initiated by the broker/gateway
 * @todo        support for pre-defined and short topic IDs
 * @todo        handle (previously) active subscriptions on reconnect/disconnect
 * @todo        handle re-connect/disconnect from unresponsive gateway (in case
 *              a number of ping requests are unanswered)
 * @todo        react only to incoming ping requests that are actually send by
 *              the gateway we are connected to
 *
 * @{
 * @file
 * @brief       emCute MQTT-SN interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_EMCUTE_H
#define NET_EMCUTE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_emcute_conf EmCute (MQTT-SN Client) compile configurations
 * @ingroup net_mqtt_conf
 * @brief   Compile-time configuration options for emCute, an implementation
 *          of the OASIS MQTT-SN protocol for RIOT. It is designed with a focus
 *          on small memory footprint and usability
 * @{
 */
/**
 * @brief   Default UDP port to listen on (also used as SRC port). Usage can be
 *          found in examples/networking/mqtt/emcute_mqttsn. Application code is expected to use
 *          this macro to assign the default port.
 */
#ifndef CONFIG_EMCUTE_DEFAULT_PORT
#define CONFIG_EMCUTE_DEFAULT_PORT          (1883U)
#endif

/**
 * @brief   Buffer size used for emCute's transmit and receive buffers
 *
 * @note    The buffer size MUST be less than 32768 on 16-bit and 8-bit
 *          platforms to prevent buffer overflows.
 *
 * The overall buffer size used by emCute is this value time two (Rx + Tx).
 */
#ifndef CONFIG_EMCUTE_BUFSIZE
#define CONFIG_EMCUTE_BUFSIZE               (512U)
#endif

/**
 * @brief   Maximum topic length
 *
 * @note    **Must** be less than (256 - 6) AND less than ( @ref CONFIG_EMCUTE_BUFSIZE - 6 )
 */
#ifndef CONFIG_EMCUTE_TOPIC_MAXLEN
#define CONFIG_EMCUTE_TOPIC_MAXLEN          (196U)
#endif

/**
 * @brief   Keep-alive interval [in seconds] communicated to the gateway
 *
 * Keep alive interval in seconds which is communicated to the gateway in the
 * CONNECT message. For more information, see MQTT-SN Spec v1.2, section 5.4.4.
 * For default values, see section 7.2 -> TWAIT: > 5 min.
 */
#ifndef CONFIG_EMCUTE_KEEPALIVE
#define CONFIG_EMCUTE_KEEPALIVE             (360)       /* -> 6 min*/
#endif

/**
 * @brief   Re-send interval [in seconds]
 *
 * Interval used for timing the retry messages which are sent when the expected
 * reply from GW is not received. The retry timer is started by the client when
 * the message is sent and stopped when the expected reply from GW is received.
 * If the timer times out and the expected GW’s reply is not received, the
 * client retransmits the message. For more information, see MQTT-SN Spec v1.2,
 * section 6.13. For default values, see section 7.2 -> Tretry: 10 to 15 sec.
 */
#ifndef CONFIG_EMCUTE_T_RETRY
#define CONFIG_EMCUTE_T_RETRY               (15U)       /* -> 15 sec */
#endif

/**
 * @brief   Number of retransmissions until requests time out
 *
 * Maximum number of retransmissions in the event that the retry timer times
 * out. After 'CONFIG_EMCUTE_N_RETRY' number of retransmissions, the client
 * aborts the procedure and assumes that its MQTT-SN connection to the gateway
 * is disconnected. For more information, see MQTT-SN Spec v1.2, section 6.13.
 * For default values, see section 7.2 -> Nretry: 3-5.
 */
#ifndef CONFIG_EMCUTE_N_RETRY
#define CONFIG_EMCUTE_N_RETRY               (3U)
#endif
/** @} */

/**
 * @brief   MQTT-SN flags
 *
 * All MQTT-SN functions only support a sub-set of the available flags. It is up
 * to the user to only supply valid/supported flags to a function. emCute will
 * trigger assertion fails on the use of unsupported flags (if compiled with
 * DEVELHELP).
 *
 * Refer to the MQTT-SN spec section 5.3.4 for further information.
 */
enum {
    EMCUTE_DUP        = 0x80,   /**< duplicate flag */
    EMCUTE_QOS_MASK   = 0x60,   /**< QoS level mask */
    EMCUTE_QOS_2      = 0x40,   /**< QoS level 2 */
    EMCUTE_QOS_1      = 0x20,   /**< QoS level 1 */
    EMCUTE_QOS_0      = 0x00,   /**< QoS level 0 */
    EMCUTE_RETAIN     = 0x10,   /**< retain flag */
    EMCUTE_WILL       = 0x08,   /**< will flag, used during CONNECT */
    EMCUTE_CS         = 0x04,   /**< clean session flag */
    EMCUTE_TIT_MASK   = 0x03,   /**< topic ID type mask */
    EMCUTE_TIT_SHORT  = 0x02,   /**< topic ID: short */
    EMCUTE_TIT_PREDEF = 0x01,   /**< topic ID: pre-defined */
    EMCUTE_TIT_NORMAL = 0x00    /**< topic ID: normal */
};

/**
 * @brief   Possible emCute return values
 */
enum {
    EMCUTE_OK       =  0,       /**< everything went as expect */
    EMCUTE_NOGW     = -1,       /**< error: not connected to a gateway */
    EMCUTE_REJECT   = -2,       /**< error: operation was rejected by broker */
    EMCUTE_OVERFLOW = -3,       /**< error: ran out of buffer space */
    EMCUTE_TIMEOUT  = -4,       /**< error: timeout */
    EMCUTE_NOTSUP   = -5        /**< error: feature not supported */
};

/**
 * @brief   MQTT-SN topic
 */
typedef struct {
    const char *name;           /**< topic string (currently ASCII only) */
    uint16_t id;                /**< topic id, as assigned by the gateway */
} emcute_topic_t;

/**
 * @brief   Signature for callbacks fired when publish messages are received
 *
 * @param[in] topic     topic the received data was published on
 * @param[in] data      published data, can be NULL
 * @param[in] len       length of @p data in bytes
 */
typedef void(*emcute_cb_t)(const emcute_topic_t *topic, void *data, size_t len);

/**
 * @brief   Data-structure for keeping track of topics we register to
 */
typedef struct emcute_sub {
    struct emcute_sub *next;    /**< next subscription (saved in a list) */
    emcute_topic_t topic;       /**< topic we subscribe to */
    emcute_cb_t cb;             /**< function called when receiving messages */
    void *arg;                  /**< optional custom argument */
} emcute_sub_t;

/**
 * @brief   Connect to a given MQTT-SN gateway (CONNECT)
 *
 * When called while already connected to a gateway, call emcute_discon() first
 * to disconnect from the current gateway.
 *
 * @param[in] remote        address and port of the target MQTT-SN gateway
 * @param[in] clean         set to true to start a clean session
 * @param[in] will_topic    last will topic name, no last will will be
 *                          configured if set to NULL
 * @param[in] will_msg      last will message content, will be ignored if
 *                          @p will_topic is set to NULL
 * @param[in] will_msg_len  length of @p will_msg in byte
 * @param[in] flags         flags used for the last will, allowed are retain and
 *                          QoS
 *
 * @return  EMCUTE_OK on success
 * @return  EMCUTE_NOGW if already connected to a gateway
 * @return  EMCUTE_REJECT on connection refused by gateway
 * @return  EMCUTE_TIMEOUT on connection timeout
 */
int emcute_con(sock_udp_ep_t *remote, bool clean, const char *will_topic,
               const void *will_msg, size_t will_msg_len, unsigned flags);

/**
 * @brief   Disconnect from the gateway we are currently connected to
 *
 * @return  EMCUTE_OK on success
 * @return  EMCUTE_GW if not connected to a gateway
 * @return  EMCUTE_TIMEOUT on response timeout
 */
int emcute_discon(void);

/**
 * @brief   Get a topic ID for the given topic name from the gateway
 *
 * @param[in,out] topic     topic to register, topic.name **must not** be NULL
 *
 * @return  EMCUTE_OK on success
 * @return  EMCUTE_NOGW if not connected to a gateway
 * @return  EMCUTE_OVERFLOW if length of topic name exceeds
 *          @ref CONFIG_EMCUTE_TOPIC_MAXLEN
 * @return  EMCUTE_TIMEOUT on connection timeout
 */
int emcute_reg(emcute_topic_t *topic);

/**
 * @brief   Publish data on the given topic
 *
 * @param[in] topic     topic to send data to, topic **must** be registered
 *                      (topic.id **must** populated).
 * @param[in] buf       data to publish
 * @param[in] len       length of @p data in bytes
 * @param[in] flags     flags used for publication, allowed are QoS and retain
 *
 * @return  EMCUTE_OK on success
 * @return  EMCUTE_NOGW if not connected to a gateway
 * @return  EMCUTE_REJECT if publish message was rejected (QoS > 0 only)
 * @return  EMCUTE_OVERFLOW if length of data exceeds @ref CONFIG_EMCUTE_BUFSIZE
 * @return  EMCUTE_TIMEOUT on connection timeout (QoS > 0 only)
 * @return  EMCUTE_NOTSUP on unsupported flag values
 */
int emcute_pub(emcute_topic_t *topic, const void *buf, size_t len,
               unsigned flags);

/**
 * @brief   Subscribe to the given topic
 *
 * When calling this function, @p sub->topic.name and @p sub->cb **must** be
 * set.
 *
 * @param[in,out] sub   subscription context, @p sub->topic.name and @p sub->cb
 *                      **must** not be NULL.
 * @param[in] flags     flags used when subscribing, allowed are QoS, DUP, and
 *                      topic ID type
 *
 * @return  EMCUTE_OK on success
 * @return  EMCUTE_NOGW if not connected to a gateway
 * @return  EMCUTE_OVERFLOW if length of topic name exceeds
 *          @ref CONFIG_EMCUTE_TOPIC_MAXLEN
 * @return  EMCUTE_TIMEOUT on connection timeout
 */
int emcute_sub(emcute_sub_t *sub, unsigned flags);

/**
 * @brief   Unsubscripbe the given topic
 *
 * @param[in] sub       subscription context
 *
 * @return  EMCUTE_OK on success
 * @return  EMCUTE_NOGW if not connected to a gateway
 * @return  EMCUTE_TIMEOUT on connection timeout
 */
int emcute_unsub(emcute_sub_t *sub);

/**
 * @brief   Update the last will topic
 *
 * @param[in] topic     new last will topic
 * @param[in] flags     flags used for the topic, allowed are QoS and retain
 *
 * @return  EMCUTE_OK on success
 * @return  EMCUTE_NOGW if not connected to a gateway
 * @return  EMCUTE_OVERFLOW if length of topic name exceeds
 *          @ref CONFIG_EMCUTE_TOPIC_MAXLEN
 * @return  EMCUTE_REJECT on rejection by the gateway
 * @return  EMCUTE_TIMEOUT on response timeout
 */
int emcute_willupd_topic(const char *topic, unsigned flags);

/**
 * @brief   Update the last will message
 *
 * @param[in] data      new message to send on last will
 * @param[in] len       length of @p data in bytes
 *
 * @return  EMCUTE_OK on success
 * @return  EMCUTE_NOGW if not connected to a gateway
 * @return  EMCUTE_OVERFLOW if length of the given message exceeds
 *          @ref CONFIG_EMCUTE_BUFSIZE
 * @return  EMCUTE_REJECT on rejection by the gateway
 * @return  EMCUTE_TIMEOUT on response timeout
 */
int emcute_willupd_msg(const void *data, size_t len);

/**
 * @brief   Run emCute, will 'occupy' the calling thread
 *
 * This function will run the emCute message receiver. It will block the thread
 * it is running in.
 *
 * @param[in] port      UDP port used for listening (default: 1883)
 * @param[in] id        client ID (should be unique)
 */
void emcute_run(uint16_t port, const char *id);

/**
 * @brief   Return the string representation of the given type value
 *
 * This function is for debugging purposes.
 *
 * @param[in] type      MQTT-SN message type
 *
 * @return  string representation of the given type
 * @return  'UNKNOWN' on invalid type value
 */
const char *emcute_type_str(uint8_t type);

#ifdef __cplusplus
}
#endif

#endif /* NET_EMCUTE_H */
/** @} */
