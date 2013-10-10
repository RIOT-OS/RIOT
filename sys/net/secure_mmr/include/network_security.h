/*
 * network_security.h - Handling of the security on a network level
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        network_security.h
 * @brief       Keeps this node in the network.
 *              Handles security on a network level
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 12:35:25
 */


#ifndef __NETWORK_SECURITY_H
#define __NETWORK_SECURITY_H

/* Timer Includes */
#include "vtimer.h"

/*---------------------------------------------------------------------------*/
//                                      Defines
/*---------------------------------------------------------------------------*/
#define _SHARED_TIMER
// change this to the number of network security components available
#define NETWORK_SECURITY_MAX_COMPONENT_COUNT 4
// First timer ID
#define TIMER_MSG_START 10
// Maximum number of timers allowed
#define MAX_REGISTERED_TIMERS 16



/**
 * @brief the possible errors that can be returned by the secure-routing class
 */
enum NetworkSecurity_Result {
    NetworkSecurity_OK              =  0,   // No error
    NetworkSecurity_PacketHandled   =  1,   // Packets was processed by layer
    NetworkSecurity_Continue        =  2,   // No Error and Continue
    // (used to keep timer ticking)
    NetworkSecurity_Stop            =  3,   // No Error and Stop
    // (used to stop timer ticking)
    NetworkSecurity_NoRebroadcast   =  4,   // Do not re-broadcast a global
    // broadcast packet
    NetworkSecurity_Error           = -1,   // Error
    NetworkSecurity_NoComponents    = -2,   // Error, No components found (e.g
    // components not added to netsec)
    NetworkSecurity_TooLong         = -3,   // Error, Packet is too long
    NetworkSecurity_NotFound        = -4,   // Error, something was not found
    NetworkSecurity_WrongVersion    = -5,   // Version Error
    NetworkSecurity_NotBaseStation  = -6,   // Message only to be processed by
    // base-station
    NetworkSecurity_NoAutoSecurity  = -7    // Automatic security is switched
                                            // off, but expected
};

/**
 * @brief the supported messages by this network security component
 */
enum NetworkSecurity_Messages {
    NetworkSecurity_HELLO           = 0,
    NetworkSecurity_ACK             = 1,
    NetworkSecurity_KeyRequest      = 2,
    NetworkSecurity_KeyTransfer     = 3,
    NetworkSecurity_KeepAlive       = 4,
    NetworkSecurity_SecurityStatus  = 5,
    NetworkSecurity_SecurityRefresh = 6,
    NetworkSecurity_SecurityError   = 7
};

/**
 * @brief List of security relevant events that could be triggered
 */
enum NetworkSecurity_Event {
    NetworkSecurity_NoNeighbours      = 0,
    NetworkSecurity_Deneighboured     = 1,
    NetworkSecurity_OverflowWarning   = 2,
    NetworkSecurity_NeighbourDetected = 3,
    NetworkSecurity_TimerTicked       = 4
};


/*---------------------------------------------------------------------------*/
//                                      Structures
/*---------------------------------------------------------------------------*/

typedef struct timer_callback {
    uint32_t timer_id;
    vtimer_t timer;
    int (*timer_ticked_ptr)(int user_data);
    uint32_t interval;
    uint8_t user_param;
}  timer_callback_t;


/**
 * @brief Interface for the submodules that do the actual security on
 *        the packets
 */
typedef struct network_security_component {
    char *name;
    int (*init)(void);
    int (*start)(void);
    int (*stop)(void);
    int (*packet_arrived)(secure_packet_t *packet);
    int (*event_handler)(enum NetworkSecurity_Event e, uint32_t param1);
} network_security_component_t;


/**
 * @brief Initializes the network security layer. Passes the call on to all
 *        sub components
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_init(void);

/**
 * @brief Starts the network security layer. Passes the call on to all
 *        sub components
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_start(void);

/**
 * @brief Stops the network security layer. Passes the call on to all
 *        sub components
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_stop(void);

/**
 * @brief Called when a packet arrives. This packet is passed on to
 *        all sub-components in turn
 *
 * @param  packet  The secure packet that arrived
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_packet_arrived(secure_packet_t *packet);


/**
 * @brief Adds a sub-component. The order of the adding is the same
 *                      order the processing functions will be called
 *
 * @param component A pointer to the component
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_add_component(network_security_component_t *component);

/**
 * @brief The sub-components receive pointers to this function and can use it
 *        to send their individual messages. This function will wrap up the
 *        components in secure_packet and then do packet security on them
 *
 * @param payload     A pointer to the data
 * @param payload_len The length of the payload
 * @param destination The intended destination of the payload
 * @param ttl         The time to live
 * @param sec_mode    the security mode for the packet (if to encrypt or not)
 * @param peek_hint   The value for the peek flag. If yes, then routed packets
 *                    will be decrypted and examined
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_send_packet(uint8_t *payload, int payload_len,
                                 uint8_t destination, uint8_t ttl,
                                 enum SecurityMode sec_mode,
                                 enum PeekHint peek_hint);

/**
 * @brief Called to set the radio_send function
 *
 * @param send_packet_ptr Pointer to the radio send function
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_set_send_function(int (*send_packet_ptr)
                                       (secure_packet_t *packet));

/**
 * @brief Called by the sub-components to register a timer. They specify
 *        the function that they want called, and this component will set a
 *        timer that calls the function at the specified interval
 *
 * @param interval_in_secs  The interval between ticks
 * @param user_param        an Ooptional user parameter
 * @param timer_ticked_ptr  The pointer to the function to be called
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_register_timer(uint32_t interval_in_secs,
                                    uint8_t user_param,
                                    int (*timer_ticked_ptr)(int user_param));

/**
 * @brief Called by the sub-components when one of the pre-defined events
 *        happens
 *
 * @param e The event raised
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_raise_event(enum NetworkSecurity_Event e, uint32_t param1);

/**
 * @brief Called by the sub-components to continue the ticking of a timer
 *
 * @param timer_id The id of the timer
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_continue_timer(int timer_id);

/**
 * @brief Called by the sub-components to restart a timer
 *
 * @param timer_id      The id of the timer
 * @param new_interval  The interval till the timer should next tick
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_restart_timer(int timer_id, uint32_t new_interval);

/**
 * @brief Called when timer should be stopped.
 *
 * @param timer_id The id of the timer
 *
 * @return a NetworkSecurity_Result indicating success or failure
 */
int network_security_stop_timer(int timer_id);

/**
 * @brief Switches on and off the network security. If off, then the network
 *        security will not start components at all. If they were running,
 *        they will be turned off
 *
 * @param  val True or False
 *
 */
void enable_network_security(int val);

/**
 * @brief Gets the process ID of the timer thread
 *
 * @return  Timer thread process id
 *
 */
unsigned int get_timer_thread_pid(void);

#endif
