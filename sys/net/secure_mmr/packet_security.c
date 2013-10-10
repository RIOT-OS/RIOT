/*
 * packet_security.c - Encryption of particular packets.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */


/*
 * @file        packet_security.c
 * @brief       The packet security classes are used to encrypt particular
 *              packets
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        15.09.2013 13:42:35
 */

/*---------------------------------------------------------------------------*/
//                                      Included Files
/*---------------------------------------------------------------------------*/
/*  Radio includes */
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"
#include "cc1100_phy.h"
#include <mutex.h>

/* Project Includes */
#include "secure_routing_globals.h"
#include "packet_security.h"
#include "microeval.h"



/*---------------------------------------------------------------------------*/
//                                      Local Variables
/*---------------------------------------------------------------------------*/
// List of components
static packet_security_component_t
component_list[PACKET_SECURITY_MAX_COMPONENT_COUNT];
// Number of components in here
static int component_count = 0;
// Switches on and off this component
static int enable_packet_sec = 1;
// Prevent multiple calls to secure_packet
// static mutex_t pack_sec_mutex;
/**
 * @brief  Initializes the Packet Security
 *
 * @return Whether initialization was successful.
 */
int packet_security_init()
{

    // mutex_init(&pack_sec_mutex);

    int res = PacketSecurity_NoComponents;

    // Walk through the components
    for (int i = 0; i < component_count; i++) {
        // Get a single component
        packet_security_component_t *p = &component_list[i];

        if (p->init_component != 0) {
            // Call the secure packet function on that component
            res = p->init_component();

            if (res < 0) {
                ulog_error("Component init failed on component %d. Error=%d",
                           i, res);
            }
        }
    }

    return res;
}

/**
 * @brief  Adds a sub-component to the security component
 *
 * @param  component The interface of the component to be added
 *
 * @return PacketSecurity_Result indicating if the component was added
 */
int packet_security_add_component(packet_security_component_t *component)
{
    if (component_count == PACKET_SECURITY_MAX_COMPONENT_COUNT) {
        ulog("> Max component count reached");
        return PacketSecurity_OK;
    }

    component_list[component_count] = *component;
    component_count++;

    return PacketSecurity_OK;
}

/**
 * @brief  Secures a packet by passing it to all sub-components to secure
 *         the packet
 *
 */
int secure_packet(secure_packet_t *packet, uint16_t phy_src)
{
    /* Sends the packet to be secured by all the packet security modules: mac,
     * encryption...
     */

    int res = PacketSecurity_NoComponents;

    if (!enable_packet_sec) {
        return PacketSecurity_Disabled;
    }

    // mutex_lock(&pack_sec_mutex);

    int i = 0;

    for (i = packet->payload_len + 1; i < MAX_PAYLOAD_LEN; i++) {
        packet->data[i] = (rand() % 256);
    }

    // Walk through the components
    for (i = 0; i < component_count; i++) {
        // Get a single component
        packet_security_component_t *p = &component_list[i];

        if (p->secure_packet != 0) {
            // Call the secure packet function on that component
            res = p->secure_packet(packet, phy_src);

            if (res < 0) {
                ulog_error("Packet securing failed on component %d. Error=%d",
                           i, res);
                // mutex_unlock(&pack_sec_mutex, MUTEX_YIELD);
                return res;
            }
        }
    }

    ulog_info("Secure packet returning %d", res);
    ulog_info("> Secured Packet %d:%d",
              packet->source,
              packet->seq_nmbr);

    // mutex_unlock(&pack_sec_mutex, MUTEX_YIELD);
    return res;
}

/**
 * @brief  Examines a previously secured packet to see if it still secure.
 *         Primarily via the message-authentication code
 *
 * @param  packet The secure packet to be verified
 *
 * @return PacketSecurity_Result with the success or failure of the operation
 */
int verify_packet(secure_packet_t *packet, uint16_t phy_src)
{
    int res = PacketSecurity_NoComponents;

    if (!enable_packet_sec) {
        // Do not use this to turn off encryption because replay protection is
        // needed to prevent broadcasts being processed twice (routing).
        // Rather, disable the mac and enc components individually
        return PacketSecurity_Disabled;
    }


    // mutex_lock(&pack_sec_mutex);

    // Walk through the components
    for (int i = 0; i < component_count; i++) {
        // Get a single component
        packet_security_component_t *p = &component_list[i];

        if (p->verify_packet != 0) {
            // Call the secure packet function on that component
            res = p->verify_packet(packet, phy_src);

            if (res < 0) {
                if (res != PacketSecurity_ReplayedBroadcast) {
                    ulog_error("Packet verification failed on component %d. \
                                Error=%d", i, res);
                }

                // mutex_unlock(&pack_sec_mutex, MUTEX_YIELD);
                return res;

            }
        }
    }

    // mutex_unlock(&pack_sec_mutex, MUTEX_YIELD);
    return res;
}

/**
 * @brief Takes a previously secured packet and desecures it (in-place).
 *        This would primarily be done by decrypting it.
 *
 * @return PacketSecurity_Result with the success or failure of the operation
 */
int desecure_packet(secure_packet_t *packet, uint16_t phy_src)
{
    int res = PacketSecurity_NoComponents;

    if (!enable_packet_sec) {
        return PacketSecurity_Disabled;
    }

    // mutex_lock(&pack_sec_mutex);
    // Walk through the components
    for (int i = 0; i < component_count; i++) {
        // Get a single component
        packet_security_component_t *p = &component_list[i];

        if (p->desecure_packet != 0) {
            // Call the secure packet function on that component
            res = p->desecure_packet(packet, phy_src);

            if (res < 0) {
                ulog_error("Packet descuring failed on component %d. Error=%d",
                           i, res);
                // mutex_unlock(&pack_sec_mutex, MUTEX_YIELD);
                return res;
            }
        }
    }

    // mutex_unlock(&pack_sec_mutex, MUTEX_YIELD);
    return res;
}

/**
 * @brief Enables or disables packet security
 */
void enable_packet_security(int val)
{
    ulog("> Changed packet security mode to %d", val);
    enable_packet_sec = val;
}
