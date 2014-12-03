/*
 * Copyright (C) 2014  Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef RPL_UDP_H
#define RPL_UDP_H

#ifdef __cplusplus
extern "C" {
#endif

#define APP_VERSION "1.2"

#define RADIO_CHANNEL   (11)

//#define MONITOR_STACK_SIZE  (1024)
#define RCV_BUFFER_SIZE     (32)

/* RPL shell command handlers */
/**
 * @brief   Shell command to initializes RPL and UDP
 *
 * @details Usage: init <r|n>
 *                 `init r` will initialize the node as a RPL root node,
 *                 `init n` as a RPL node.
 *
 * @param[in] argc  Argument count
 * @param[in] argv  Arguments
 */
void rpl_udp_init(int argc, char **argv);



void rpl_udp_init_node(int argc, char **argv);//void);
void rpl_udp_init_node_standalone(void);
/**
 * @brief   Shell command to set node's ID
 *
 * @details Usage: set <ID>
 *          Set the node address
 *
 * @param[in] argc  Argument count
 * @param[in] argv  Arguments
 */
void rpl_udp_set_id(int argc, char **argv);

/**
 * @brief   Shows the dodag
 *
 * @details No parameters required
 *
 * @param[in] argc  Argument count
 * @param[in] argv  Arguments
 */
void rpl_udp_dodag(int argc, char **argv);

/**
 * @brief Command handler to start a UDP server
 *
 * @details No parameters required
 *
 * @param[in] argc  Argument count
 * @param[in] argv  Arguments
 */
void udp_server(int argc, char **argv);

/**
 * @brief Sends a UDP datagram
 *
 * @details Usage: send <ID> <TEXT>
 *          Sends TEXT to the node with IP address:
 *          fe80::ff:fe00:<ID>
 *
 * @param[in] argc  Argument count
 * @param[in] argv  Arguments
 */
void udp_send(int argc, char **argv);

//void udp_adc_send(int argc, char **argv);
void udp_send_payload(int);
void getWeightPayload(char*);
void getTestSmsPayload(char*);
void getBatteryPayload(char*);
void getTemperaturePayload(char*);
void getHumidityPayload(char*);
void node_tara_callback(void);
void node_testsms_callback(void);
/**
 * @brief Ignore a certain node
 *
 * @details Usage: ignore <ID>
 *          Ignore the node with IP address:
 *          fe80::ff:fe00:<ID>
 *
 * @param[in] argc  Argument count
 * @param[in] argv  Arguments
 */
void rpl_udp_ignore(int argc, char **argv);

/**
 * @brief monitoring thread start function
 *
 * @param arg Unused
 */
void *rpl_udp_monitor(void *arg);
void *node_udp_thread(void *);
void *payload_node_thread(void *arg);

/** @brief The nodes radio address */
extern radio_address_t id;

/** @brief Char array for IP address printing */
extern char addr_str[IPV6_MAX_ADDR_STR_LEN];

#ifdef __cplusplus
}
#endif

#endif /* RPL_UDP_H */
