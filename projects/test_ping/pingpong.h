#ifndef PING_H
#define PING_H

#include <stdint.h>

#define SECOND   (1000*1000)

/**
 * The different message types a ping_packet_t message can have:
 *      PING        means it is a normal pingmessage.
 *      PING_ACK    means it is an ACK to one of those normal PING messages.
 *      PING_BCST   means it is a broadcasted ping that doesn't care about ACKs.
 */
enum ping_message_type {
    PING,     //!< PING
    PING_ACK, //!< PING_ACK
    PING_BCST,//!< PING_BCST
};

/**
 * The struckt describing a ping-packet. A pingpacket has a sequencenumber and
 * a type.
 */
typedef struct __attribute__((packed)) {
    uint16_t seq_nr;
    uint8_t  type ;
} ping_packet_t;

/**
 * @brief Sends a broadcast for duration seconds.
 *
 * This method sends a broadcast for duration seconds with a delay of 500ms
 * between each message.
 *
 * @param duration The duration for this broadcast to continue, in seconds.
 */
void broadcast_without_ack(uint16_t duration);

/**
 * @brief Handles incoming broadcast messages.
 */
void broadcast_incoming();

/**
 * @brief The internal method for sending a broadcast message.
 */
void send_broadcast();

#endif /* PING_H */
