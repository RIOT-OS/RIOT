/*
 * Copyright (C) 2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_rail
 * @{
 *
 * @file
 * @brief       helper functions for EFR32 radio driver
 *
 * @author      Kai Beckmann <kai.beckmann@hs-rm.de>
 */

#ifndef RAIL_HELPER_H
#define RAIL_HELPER_H

#include <stdint.h>
#include <stdbool.h>

#include "rail.h"

#include "byteorder.h"
#include "net/eui64.h"
#include "ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Reads and converts the EUI of the transceiver
 *
 * @return the EUI
 */
eui64_t rail_helper_get_hw_EUI(void);


/**
 *
 * @brief       Simple event queue based on a ringbuffer to pass rail events to the netdev layer
 * @{
 *
 * @author  Kai Beckmann <kai.beckmann@hs-rm.de>
 */


/**
 * @brief count of possible events to hold in queue
 */
#define RAIL_EVENT_MSG_COUNT 10

/**
 * @brief Structure to store all relevant data for an rail event
 *
 */
typedef struct {

    RAIL_Events_t event;                /**< The rail event that occurred */
    RAIL_RxPacketHandle_t rx_packet;    /**< if a packet was received, the rail handle to the packet */
    RAIL_RxPacketInfo_t rx_packet_info; /**< if a packet was received, the basic packet information
                                             extracted in the rail event hanlder */
    uint16_t rx_packet_size;            /**< size of the received packet */
    uint32_t event_count;               /**< the number / count of this event since driver init */

} rail_event_msg_t;

/**
 * @brief Structure for a simple rail event queue.
 *
 * Structure for a simple event queue, to be used in @see rail_t
 * The memory for the ring buffer is part of the structure, so every
 * instance of the driver can have its seperate queue.
 */
typedef struct {
    ringbuffer_t ring_buffer;               /**< ring buffer for incomming events */
    uint8_t _buffer[sizeof(rail_event_msg_t) * RAIL_EVENT_MSG_COUNT]; /**< memory to hold the ringbuffer */

} rail_event_queue_t;


/**
 * @brief Initialise an event queue for rail events
 *
 * @param[in,out] queue     event queue
 *
 */
void rail_event_queue_init(rail_event_queue_t* queue);

/**
 * @brief Peek an event from queue without removing it
 *
 * @param[in,out] queue     event queue
 * @param[out] event_msg    the event
 *
 * @return 0 on success
 * @return -1 on failure
 */
int rail_event_queue_peek(rail_event_queue_t* queue, rail_event_msg_t* event_msg);

/**
 * @brief Polls an event from queue with removing it
 *
 * @param[in,out] queue     event queue
 * @param[out] event_msg    the event, if NULL the first event is just removed
 *
 * @return 0 on success
 * @return -1 on failure
 */
int rail_event_queue_poll(rail_event_queue_t* queue, rail_event_msg_t* event_msg);

/**
 * @brief Adds an event to the end of the queue
 *
 * @param[in,out] queue     event queue
 * @param[out] event_msg    the event to add
 *
 * @return 0 on success
 * @return -1 on failurePeek an event from queue without removing it
 */
int rail_event_queue_add(rail_event_queue_t* queue, rail_event_msg_t* event_msg);

/** @} */

/**
 * @brief maps librail error / status codes to strings
 *
 * @param[in] status    error or status code
 *
 * @return String describing the status/error
 */
const char *rail_error2str(RAIL_Status_t status);

/**
 * @brief Debug / helper functions for mapping librail stati and
 * events to strings
 * Strings are only included with @see DEVELHELP set
 *
 * @{
 */

/**
 * @brief map rx packet status to string expression
 *
 * @param[in] status    status to map
 *
 * @return String
 */
const char *rail_packetStatus2str(RAIL_RxPacketStatus_t status);

/**
 * @brief map radio state to string expression
 *
 * @param[in] state    status to map
 *
 * @return String
 */
const char *rail_radioState2str(RAIL_RadioState_t state);

/**
 * @brief map rail event to string expression
 *
 * @param[in] event    event to map
 *
 * @return String
 */
const char *rail_event2str(RAIL_Events_t event);


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* RAIL_HELPER_H */
/** @} */
