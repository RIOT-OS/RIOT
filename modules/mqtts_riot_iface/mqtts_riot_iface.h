#ifndef MQTTS_RIOT_IFACE_H
#define MQTTS_RIOT_IFACE_H

#include <gnrc_wolfssl_tls.h>

struct MQTTSContext; // forward declaration
typedef struct MQTTSContext MQTTSContext;

typedef void (*MqttsMessageArrivalCallback)(char *topic, int topic_len, char *payload,
                                            int payload_len);
void mqtts_set_message_arrival_callback(MqttsMessageArrivalCallback _callback);

/**
 * @file mqtts_riot_iface.h
 * @brief Header file for the MQTTS RIOT Interface.
 */

/**
 * @brief Initialize the MQTTS module.
 *
 * This function should be called once at the beginning to initialize the MQTTS module.
 *
 * @param mqtts_ctx Pointer to the MQTTS context structure.
 * @param writebuf Pointer to the write buffer for MQTTS communication.
 * @param writebuf_size Size of the write buffer.
 * @param readbuf Pointer to the read buffer for MQTTS communication.
 * @param readbuf_size Size of the read buffer.
 * @return 0 on success, negative value on failure.
 */
int mqtts_init(MQTTSContext *mqtts_ctx, unsigned char *writebuf, int writebuf_size,
               unsigned char *readbuf, int readbuf_size);

/**
 * @brief Create a new MQTTS context.
 *
 * This function creates a new MQTTS context.
 *
 * @param log_callback Function pointer for logging.
 * @return A pointer to the MQTTS context on success, NULL on failure.
 */
MQTTSContext *mqtts_create_context(void (*log_callback)(const char *message));

/**
 * @brief Set the TLS context for the MQTTS context.
 *
 * @param mqtts_ctx The MQTTS context.
 * @param tls_ctx The TLS context.
 * @return 0 on success, negative value on failure.
 */
int mqtts_set_tls_context(MQTTSContext *mqtts_ctx, TLSContext *tls_ctx);

/**
 * @brief Connect to the MQTT broker.
 *
 * @param mqtts_ctx The MQTTS context.
 * @param remoteAddress The remote address of the MQTT broker.
 * @param clientID The client identifier for MQTT.
 * @param username The username for MQTT authentication.
 * @param password The password for MQTT authentication.
 * @return 0 on success, negative value on failure.
 */
int mqtts_connect(MQTTSContext *mqtts_ctx, char *remoteAddress, char *clientID, char *username,
                  char *password);

/**
 * @brief Publish a message to an MQTT topic.
 *
 * @param mqtts_ctx The MQTTS context.
 * @param topic The MQTT topic to publish to.
 * @param payload The message payload.
 * @param qos The quality of service level.
 * @param retained Flag to indicate if the message should be retained.
 * @return 0 on success, negative value on failure.
 */
int mqtts_publish(MQTTSContext *mqtts_ctx, const char *topic, unsigned char *payload, int qos,
                  int retained);

/**
 * @brief Subscribe to an MQTT topic.
 *
 * @param mqtts_ctx The MQTTS context.
 * @param topic The MQTT topic to subscribe to.
 * @param qos The quality of service level.
 * @return 0 on success, negative value on failure.
 */
int mqtts_subscribe(MQTTSContext *mqtts_ctx, const char *topic, int qos);
int mqtts_unsubscribe(MQTTSContext *mqtts_ctx, const char *topic);

/**
 * @brief Disconnect from the MQTT broker.
 *
 * @param mqtts_ctx The MQTTS context.
 */
int mqtts_disconnect(MQTTSContext *mqtts_ctx);

/**
 * @brief Cleanup and deinitialize the MQTTS module.
 *
 * This function should be called once at the end to cleanup MQTTS resources.
 */
void mqtts_cleanup(void);

/**
 * @brief Set the certificate paths for the MQTTS context.
 *
 * @param ctx The MQTTS context.
 * @param ca_cert_path Path to the CA certificate.
 * @param cert_chain_path Path to the certificate chain.
 * @param private_key_path Path to the private key.
 * @return 0 on success, negative value on failure.
 */
int mqtts_set_certificate(MQTTSContext *ctx, const char *ca_cert_path, const char *cert_chain_path,
                          const char *private_key_path);


#endif
