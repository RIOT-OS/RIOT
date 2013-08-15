/*
 * keepalive.h - Sends keep-alive packets between nodes
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        keepalive.h
 * @brief       Sends keep-alive packets between nodes.
 *              Transfers keep-alives
 *
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics.
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 11:39:55
 */

#ifndef __KEEP_ALIVE_H
#define __KEEP_ALIVE_H


/**
 * @brief struct for the keepalive-packet
 */
typedef struct keepalive_msg {
    // the type of the packet - here LOCALKEEPALIVE
    uint8_t type;
    // The current global version
    uint16_t gkey_version;
} __attribute__((packed)) keepalive_msg_t;


/**
 * Gets the keep-alive component
 *
 * @return Pointer to the keep-alive component
 */
network_security_component_t *keep_alive_get_component(void);

/**
 * Initializes the Keep-Alive component
 *
 * @return Whether initialization was successful.
 */
int keepalive_init(void);

/**
 * Starts the keep-alive component
 *
 * @return Whether start was successful.
 */
int keepalive_start(void);

/**
 * Stops the keep-alive componnent
 *
 * @return Whether stop was successful.
 */
int keepalive_stop(void);


/**
 * @brief Called to send out a keep-alive broadcast
 *
 * @return Whether operation was successful.
 *
 */
int broadcast_keepalive(void);

/**
 * @brief Print out internal info about keep alive component
 */
void keepalive_print_info(void);

#endif
