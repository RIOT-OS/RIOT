/*
 * network_security.c - Exchanges the keys with other nodes in the network
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        network_security.c
 * @brief       Handles security on a network level
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        12.09.2013 19:31:49
 */

/*---------------------------------------------------------------------------*/
//                            Included Files
/*---------------------------------------------------------------------------*/

/*  Radio includes */
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"
#include "cc1100_phy.h"


/* Messaging & Threading */
#include "thread.h"
#include "msg.h"
#include <mutex.h>

/* Project Includes */
#include "secure_routing_globals.h"
#include "network_security.h"
#include "microeval/microeval.h"

/*---------------------------------------------------------------------------*/
//                                      Variables
/*---------------------------------------------------------------------------*/
// List of components
static network_security_component_t component_list[
                                         NETWORK_SECURITY_MAX_COMPONENT_COUNT];
// Number of components
static int component_count = 0;
// pointer to send packet function from secure_routing class
static int (*send_packet)(secure_packet_t *packet);
// Indicates if network security is enabled at all
int enable_net_sec = 1;
static int netsec_started = 0;

// Timer
static int timer_thread_pid = -1;   // ID of the timer thread
static msg_t timer_msg_queue[16];   // Queue of messages intended for timer
static int last_timer_id = TIMER_MSG_START; // Current last added timer ID
// Array of the pointers to the callbacks for the timer
static timer_callback_t timer_callbacks[MAX_REGISTERED_TIMERS];
static char timer_thread_stack_buffer[4500]; // Stack buffer for timer thread

/*---------------------------------------------------------------------------*/
//                                      Functions
/*---------------------------------------------------------------------------*/
#ifdef _SHARED_TIMER
static void timer_handling_thread(void);
#endif


/**
 * @brief Initializes the network security layer. See header file for
 *        extended description
 *
 */
int network_security_init()
{
    int res = NetworkSecurity_NoComponents;

    //start Controller-Thread
    if (timer_thread_pid < 0) {
        ulog("> Creating timer thread");
        timer_thread_pid = thread_create(timer_thread_stack_buffer,
                                                 4500, PRIORITY_MAIN + 1,
                                                 CREATE_STACKTEST | CREATE_WOUT_YIELD,
                                                 timer_handling_thread,
                                                 "timer-thread");

        if (timer_thread_pid <= 0) {
            ulog_error("Could not start timer thread");
            return NetworkSecurity_Error;
        }
    }
    else {
        ulog("> Timer thread already running");
    }

    // Walk through the components
    for (int i = 0; i < component_count; i++) {
        // Get a single component
        network_security_component_t *n = &component_list[i];

        if (n->init != 0) {
            // Call the secure packet function on that component
            res = n->init();

            if (res < 0) {
                ulog_error("Component init failed on security component %d. \
                                    Error=%d", i, res);
            }
        }
    }

    return res;
}

/**
 * @brief Adds a component the network security layer. See header file for
 *        extended description
 *
 */
int network_security_add_component(network_security_component_t *component)
{
    if (component_count == NETWORK_SECURITY_MAX_COMPONENT_COUNT) {
        ulog("> Max component count reached");
        return NetworkSecurity_OK;
    }

    component_list[component_count++] = *component;
    return NetworkSecurity_OK;
}

/**
 * @brief Starts the network security layer. See header file for
 *        extended description
 *
 */
int network_security_start()
{
    if (netsec_started == 1) {
        return NetworkSecurity_OK;
    }

    if (enable_net_sec == 0) {
        ulog("> Not starting network security because it's disabled");
        return NetworkSecurity_OK;
    }

    ulog("> Starting network security. Component count=%d", component_count);

    netsec_started = 1;

    int res = NetworkSecurity_NoComponents;

    // Walk through the components
    for (int i = 0; i < component_count; i++) {
        // Get a single component
        network_security_component_t *n = &component_list[i];

        if (n->start != 0) {
            // Call the secure packet function on that component
            res = n->start();

            if (res < 0) {
                ulog_error("Component start failed on security component %d. \
                                    Error=%d", i, res);
            }
        }
    }

    if (res == NetworkSecurity_NoComponents) {
        ulog_info("No security components added");
    }

    return res;
}

/**
 * @brief Stops the network security layer. See header file for
 *        extended description
 *
 */
int network_security_stop()
{
    netsec_started = 0;

    int res = NetworkSecurity_NoComponents;

    // Walk through the components
    for (int i = 0; i < component_count; i++) {
        // Get a single component
        network_security_component_t *n = &component_list[i];

        if (n->stop != 0) {
            // Call the secure packet function on that component
            res = n->stop();

            if (res < 0) {
                ulog_error("Component stop failed on security component %d. \
                                   Error=%d", i, res);
            }
        }
    }

    return NetworkSecurity_OK;
}

/**
 * @brief Called when new packet arrives. See header file for
 *        extended description
 */
int network_security_packet_arrived(secure_packet_t *packet)
{
    if (packet->protocol != SECURE_ROUTING_PROTOCOL) {
        return NetworkSecurity_OK;
    }

    int res = NetworkSecurity_NoComponents;

    // Walk through the components
    for (int i = 0; i < component_count; i++) {
        // Get a single component
        network_security_component_t *n = &component_list[i];

        if (n->packet_arrived != 0) {
            // Call the packet arrived function on that component
            int local_res = n->packet_arrived(packet);

            if (local_res < 0) {
                ulog_error("Packet processing failed on security component %d. \
                                    Error=%d", i, local_res);
                return local_res;
            }

            if (local_res == NetworkSecurity_NoRebroadcast) {
                // Means packet was handled and the handler does not
                // want this packet re-sent into the network
                return local_res;
            }

            if (res != NetworkSecurity_PacketHandled) {
                res = local_res;
            }
        }
    }

    return res;
}

/**
 * @brief Sets the radio send function. See header file for
 *        extended description
 */
int network_security_set_send_function(int (*send_packet_ptr)
                                               (secure_packet_t *packet))
{
    send_packet = send_packet_ptr;
    return NetworkSecurity_OK;
}

/**
 * @brief Used by components to send data. See header file for
 *        extended description
 */
int network_security_send_packet(uint8_t *payload,
                                         int payload_len,
                                         uint8_t destination,
                                         uint8_t ttl,
                                         enum SecurityMode sec_mode,
                                         enum PeekHint peek_hint)
{
    if (payload_len > MAX_PAYLOAD_LEN) {
        ulog_error("Payload Length greater than max payload len");
        return NetworkSecurity_TooLong;
    }

    // mutex_lock(&send_lock);

    secure_packet_t packet;
    memset(&packet, 0, sizeof(secure_packet_t));

    // Copy data
    memcpy(packet.data, payload, payload_len);

    if (payload_len < 16) {
        // There is a minimum send size, so if our internal messages
        // are too short, we expand it. The extra area is just null packets
        payload_len = 16;
    }

    // Set variables
    packet.protocol = SECURE_ROUTING_PROTOCOL;
    packet.payload_len = payload_len;
    packet.destination = destination;
    packet.source = get_this_node_address();
    packet.ttl = ttl;
    packet.flags = SECURE_FLAG(peek_hint, sec_mode);
    set_physical_source_p(packet);

    if (send_packet(&packet) != NetworkSecurity_OK) {
        return NetworkSecurity_Error;
    }

    // mutex_unlock(&send_lock, MUTEX_YIELD);

    return NetworkSecurity_OK;
}

/**
 * @brief Used by components to register their intervals with timer. See header
 *        file for extended description
 */
int network_security_register_timer(uint32_t interval_in_secs,
                                            uint8_t user_param,
                                            int (*timer_ticked_ptr)(int user_param))
{
    ulog_info("> Registering the timer. Interval=%lu",
                  (uint8_t)interval_in_secs);

    // Init the timer registration in the tick table
    int timer_index = last_timer_id - TIMER_MSG_START;
    ulog_info("> TimerIndex=%d", timer_index);

    timer_callbacks[timer_index].timer_id = last_timer_id;
    timer_callbacks[timer_index].timer_ticked_ptr = timer_ticked_ptr;
    timer_callbacks[timer_index].user_param = user_param;
    timer_callbacks[timer_index].interval = interval_in_secs;

    ulog_info("About to remove vtimer");
    vtimer_remove(&(timer_callbacks[timer_index].timer));

    ulog_info("About to set vtimer. Seconds=%lu", interval_in_secs);
    // Set the timer to tick at the specified interval
    timex_t interval = timex_set(interval_in_secs, 0);
    vtimer_set_msg(&(timer_callbacks[timer_index].timer),
                       interval,
                       (unsigned int)timer_thread_pid,
                       (void *)timer_callbacks[timer_index].timer_id);

    last_timer_id++;

    return last_timer_id - 1;
}

/**
 * @brief Called to prepare timer for next tick. See header file for
 *        extended description
 *
 */
int network_security_continue_timer(int timer_id)
{

    if (timer_id > 100 || timer_id < 0) {
        ulog_error("CONTINUE TIMER WITH INVALID ID!%d", timer_id);
    }

    int timer_index = timer_id - TIMER_MSG_START;

    ulog("> Restarting timer with id %d, index=%d", timer_id, timer_index);
    // timer_callback_t* ptimer = &timer_callbacks[timer_index];

    vtimer_remove(&(timer_callbacks[timer_index].timer));

    // Reset the timer
    timex_t interval = timex_set(timer_callbacks[timer_index].interval, 0);
    vtimer_set_msg(&(timer_callbacks[timer_index].timer),
                       interval, timer_thread_pid,
                       (void *)timer_callbacks[timer_index].timer_id);

    return NetworkSecurity_OK;
}

/**
 * @brief Called when timer should be stopped. See header file for
 *        extended description
 */
int network_security_stop_timer(int timer_id)
{

    if (timer_id == -1 || timer_id == 255 || timer_id == 254) {
        return NetworkSecurity_OK;
    }

    if (timer_id > 100 || timer_id < 0) {
        ulog_error("STOP TIMER WITH INVALID ID!%d", timer_id);
    }

    int timer_index = timer_id - TIMER_MSG_START;

    ulog("> Stopping timer with id %d, index=%d", timer_id, timer_index);
    // timer_callback_t* ptimer = &timer_callbacks[timer_index];

    // Remove the old timer
    vtimer_remove(&(timer_callbacks[timer_index].timer));

    return NetworkSecurity_OK;
}

/**
 * @brief Called when timer should be restarted. See header file for
 *        extended description
 *
 */
int network_security_restart_timer(int timer_id, uint32_t new_interval)
{
    if (timer_id > 100 || timer_id < 0) {
        ulog_error("RESTART TIMER WITH INVALID ID!%d", timer_id);
    }

    int timer_index = timer_id - TIMER_MSG_START;

    ulog("> Restarting timer with id %d, index=%d", timer_id, timer_index);
    // timer_callback_t* ptimer = &timer_callbacks[timer_index];

    // Remove the old timer
    vtimer_remove(&(timer_callbacks[timer_index].timer));

    // Change the interval
    timer_callbacks[timer_index].interval = new_interval;

    // Reset the timer
    timex_t interval = timex_set(timer_callbacks[timer_index].interval, 0);
    vtimer_set_msg(&(timer_callbacks[timer_index].timer),
                       interval, timer_thread_pid,
                       (void *)timer_callbacks[timer_index].timer_id);

    return NetworkSecurity_OK;
}

/**
 * @brief entry point for secondary thread responsible for receiving
 *        timer messages and distributing them to each of the components
 */
static void timer_handling_thread()
{
    /* Waits for a timer message then passes it on to the
     * appropriate handler component
     */

    ulog("Timer thread started");

    int res = msg_init_queue(timer_msg_queue, 16);

    if (res < 0) {
        ulog_error("ERROR: Message queue a could not be inited");
    }

    msg_t timer_msg;

    while (1) {
        ulog_info("Timer thread about to wait for message");
        msg_receive(&timer_msg);

        ulog_info("timer thread got message");

        switch (timer_msg.type) {
            case (MSG_TIMER): {
                /* Pass this message to all components in turn. Only the
                 * component which matches value will react to the event
                 * network_security_raise_event(NetworkSecurity_TimerTicked,
                 * timer_msg.content.value);
                                 */
                // smutex_lock(&netsec_mutex);
                ulog("> Timer %lu ticked!", timer_msg.content.value);

                // Loop through all registered timer events
                for (int i = 0; i < last_timer_id - TIMER_MSG_START; i++) {
                    // get timer structure
                    timer_callback_t *ptimer = &timer_callbacks[i];

                    if (ptimer->timer_id == timer_msg.content.value) {
                        /* It seems you have to remove the timer and reset it
                         * again
                         */
                        vtimer_remove(&(ptimer->timer));

                        // Call the timer handlers
                        if (ptimer->timer_ticked_ptr(ptimer->user_param) ==
                                NetworkSecurity_Continue) {
                            ulog_info("Setting timer to tick again");
                            network_security_continue_timer(
                                                            TIMER_MSG_START + i);
                        }
                    }
                } // for

                // mutex_unlock(&netsec_mutex, MUTEX_YIELD);
            }
        }
    }
}

/**
 * @brief Receives a network security error event. See header file for
 *        extended description
 */
int network_security_raise_event(enum NetworkSecurity_Event e, uint32_t param1)
{
    // Called when a sub-component discovers a certain state that
    // should be communicated with the network security layer

    // NetworkSecurity_NoNeighbours will be handled by keyexchange component


    for (int i = 0; i < component_count; i++) {
        // Get a single component
        network_security_component_t *n = &component_list[i];

        if (n->event_handler != 0) {
            n->event_handler(e, param1);
        }
    }

    return NetworkSecurity_OK;
}

/**
 * @brief Switches on and off the network security. See header file for details
 *
 */
void enable_network_security(int val)
{
    enable_net_sec = val;

    // Stop all running components
    if (enable_net_sec == 0) {
        network_security_stop();
    }
    else {
        network_security_start();
    }
}

/**
 * @brief Return the process id of the timer thread
 *
 */
unsigned int get_timer_thread_pid()
{
    return timer_thread_pid;
}
