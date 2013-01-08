#ifndef PING_H
#define PING_H

#include <stdint.h>

#define XS_STACK (400)
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
 * @brief The method that sends a ping to an address addr for duration seconds.
 *
 * This method sends a ping to the given address addr for duration seconds.
 *
 * @param addr A radio address to send pings to.
 * @param duration A duration in seconds.
 */
void pingpong(uint8_t addr, uint16_t duration);

/**
 * @brief Waits for an acknowledgment of a previously sent ping.
 *
 * This method is an internal method and should not be called from the outside.
 */
void wait_pong();

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
 * @brief Handles incoming PING messages.
 *
 * @param src the source-address from which the pingmessage originated from.
 */
void ping_incoming(uint8_t src);

/**
 * @brief Handles incoming ACK messages.
 */
void ack_incoming();

/**
 * @brief Handles incoming broadcast messages.
 */
void broadcast_incoming();

/**
 * @brief The internal method for sending a pingmessage to address addr.
 * @param addr The address to send a pingmessage to.
 */
void send_ping(uint8_t addr);

/**
 * @brief The internal method for sending an acknowledgment of a ping to address
 * addr.
 *
 * @param addr The address to send an acknowledgment to.
 */
void send_ack(uint8_t addr);

/**
 * @brief The internal method for sending a broadcast message.
 */
void send_broadcast();

/**
 * @brief The internal method to calculate the Round-Trip-Time for a ping.
 */
void calc_rtt(void);

/**
 * @brief The internal method to stop this node from pinging or broadcasting.
 *
 * This method should be used for example if a ping was given without a
 * duration, so without manually stopping the node it would send endlessly.
 */
void stop_now(void);

/**
 * @brief The internal thread that stops this node from pinging a target after a
 * specified time.
 */
void stop_after_time(void);

/**
 * @brief Prints a success message when a ping was sent and an ACK for it was
 * returned.
 *
 * This method pings the Round-Trip-Time for a pingmessage to which an ACK was
 * received.
 */
void print_success();

/**
 * @brief Prints a failure message if a ping exceeds the timelimit for an ACK
 * to be received.
 */
void print_failure();

#endif /* PING_H */
