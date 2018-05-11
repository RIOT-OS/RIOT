/*
 * Copyright (C) 2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include <inttypes.h>
#include <assert.h>


#include "rail_helper.h"


#define ENABLE_DEBUG (0)
#include "debug.h"

#include "log.h"


eui64_t rail_helper_get_hw_EUI(void)
{

    /* this is a bit messy, because everthing has or what it in different
       endianess
       for convenience we read it once and save it in the netdev structure in
       big endianess
     */
    le_uint64_t tmp;
    tmp.u32[0] = DEVINFO->UNIQUEL;
    tmp.u32[1] = DEVINFO->UNIQUEH;

    eui64_t eui;
    eui.uint64 = byteorder_ltobll(tmp);

    return eui;
}



void rail_event_queue_init(rail_event_queue_t* queue)
{
    assert(queue != NULL);

    /* init the ringbuffer for the rail event queue */
    ringbuffer_init(&(queue->ring_buffer), (char *) queue->_buffer, sizeof(queue->_buffer));

}

int rail_event_queue_peek(rail_event_queue_t* queue, rail_event_msg_t* event_msg)
{
    assert(event_msg != NULL);

    if (ringbuffer_empty(&(queue->ring_buffer)) != 0) {
        return -1;
    }


    unsigned r = ringbuffer_peek(&(queue->ring_buffer), (char *) event_msg, sizeof(rail_event_msg_t));

    if (r != sizeof(rail_event_msg_t)) {
            return -1;
    }

    return 0;

}

int rail_event_queue_poll(rail_event_queue_t* queue, rail_event_msg_t* event_msg)
{

    if (ringbuffer_empty(&(queue->ring_buffer)) != 0) {
        return -1;
    }

    if (event_msg != NULL) {
        unsigned r = ringbuffer_get(&(queue->ring_buffer), (char *) event_msg, sizeof(rail_event_msg_t));

        if (r != sizeof(rail_event_msg_t)) {
            return -1;
        }
    }
    else {
        unsigned r = ringbuffer_remove(&(queue->ring_buffer), sizeof(rail_event_msg_t));

        if (r != sizeof(rail_event_msg_t)) {
            return -1;
        }
    }

    return 0;
}

int rail_event_queue_add(rail_event_queue_t* queue, rail_event_msg_t* event_msg)
{
    if (ringbuffer_full(&(queue->ring_buffer)) != 0) {
        LOG_ERROR("Rail event ring buffer is full\n");
        return -1;
    }

    unsigned r = ringbuffer_add(&(queue->ring_buffer), (char *) event_msg, sizeof(rail_event_msg_t));

    if (r != sizeof(rail_event_msg_t)) {
            return -1;
    }

    return 0;
}

const char *rail_error2str(RAIL_Status_t status)
{

    switch (status) {
        case (RAIL_STATUS_NO_ERROR):
            return "No error";
        case (RAIL_STATUS_INVALID_PARAMETER):
            return "Invalid parameter";
        case (RAIL_STATUS_INVALID_STATE):
            return "Invalid state";
        case (RAIL_STATUS_INVALID_CALL):
            return "Invalid Call";
        case (RAIL_STATUS_SUSPENDED):
            return "Status suspended";
    }
    (void) status;
    return "Error code unknown";
}


const char *rail_event2str(RAIL_Events_t event)
{
#ifdef DEVELHELP
    if (event & RAIL_EVENT_RX_PACKET_RECEIVED) {
        return "RAIL_EVENT_RX_PACKET_RECEIVED";
    }
    if (event & RAIL_EVENT_TX_PACKET_SENT) {
        return "RAIL_EVENT_TX_PACKET_SENT";
    }

    return "RAIL EVENT: TODO";
#else
    (void) event;
    return "enable DEVELHELP";
#endif /* DEVELHELP */
}


const char *rail_packetStatus2str(RAIL_RxPacketStatus_t status)
{
#ifdef DEVELHELP
    switch (status) {
        case (RAIL_RX_PACKET_NONE):
            return "Radio is idle or searching for a packet.";
        case (RAIL_RX_PACKET_ABORT_FORMAT):
            return "Format/Length error.";
        case (RAIL_RX_PACKET_ABORT_FILTERED):
            return "Filtering error (address).";
        case (RAIL_RX_PACKET_ABORT_ABORTED):
            return "Aborted error.";
        case (RAIL_RX_PACKET_ABORT_OVERFLOW):
            return "Receive overflowed buffer.";
        case (RAIL_RX_PACKET_ABORT_CRC_ERROR):
            return "CRC error aborted.";
        case (RAIL_RX_PACKET_READY_CRC_ERROR):
            return "CRC error accepted (details available).";
        case (RAIL_RX_PACKET_READY_SUCCESS):
            return "Success (details available).";
        case (RAIL_RX_PACKET_RECEIVING):
            return "Receiving in progress.";
        default:
            return "Unknown status";
    }
#else
    (void) status;
    return "enable DEVELHELP";
#endif /* DEVELHELP */
}

const char *rail_radioState2str(RAIL_RadioState_t state)
{
#ifdef DEVELHELP
    switch (state) {
        case (RAIL_RF_STATE_INACTIVE):
            return "state inactive";
        case (RAIL_RF_STATE_ACTIVE):
            return "state active / idle";
        case (RAIL_RF_STATE_RX):
            return "state rx";
        case (RAIL_RF_STATE_TX):
            return "state tx";
        default:
            return "unknown state";
    }
#else
    (void) state;
    return "enable DEVELHELP";
#endif /* DEVELHELP */
}
