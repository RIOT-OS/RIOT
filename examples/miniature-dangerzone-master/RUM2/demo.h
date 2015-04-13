#ifndef DEMO_H
#define DEMO_H

#include "kernel.h"
#include "radio/types.h"
#include "tl_socket/socket.h"

#define APP_VERSION "0.5"

/**
 * Basic network configuration (addresses, channel, etc)
 */
#ifndef NODE_ADDRESS
#define NODE_ADDRESS            31
#endif
// #if NODE_ADDRESS == 31
// #define LEVEL1
// #elif (NODE_ADDRESS == 32) | (NODE_ADDRESS == 33)
// #define LEVEL2
// #elif (NODE_ADDRESS == 41) | (NODE_ADDRESS == 51)
// #define LEVEL3
// #endif
#if NODE_ADDRESS == 31
#define NODE_31
#elif NODE_ADDRESS == 32
#define NODE_32
#elif NODE_ADDRESS == 33
#define NODE_33
#endif

#define NODE_MODE               "n"
#define RADIO_CHANNEL           (10)
#define GW_ADDRESS              "23"
#define APPLICATION_PORT        (2350)

/**
 * Set this value to 1 when you want to send the RIOT-TV information
 * via UDP instead of printing it to the std out.
 */
// #define VIZ_REMOTE              0

#define CEBIT_RX_TOGGLE         1

/**
 * List of addresses (as strings) of nodes interested in sensor events
 */
#define OBSERVER_NUMOF          (2U)
#define OBSERVER_NODES          {23, 51}

/**
 * List a number of nodes that are ignored from this node
 */
#define IGNORE_NUMOF            0
#define IGNORE_NODES            {}

/**
 * Define the number of times each UDP packet is transmitted
 */
#define RETRANSMISSIONS         1

/**
 * General UDP socket configuration
 */
#define MONITOR_STACK_SIZE      (KERNEL_CONF_STACKSIZE_MAIN)
#define RCV_BUFFER_SIZE         (32)

/* RPL shell command handlers */
/**
 * @brief   Shell command to initializes RPL and Destiny
 */
void rpl_udp_init(int argc, char **argv);

/**
 * @brief   Shell command to set node's ID
 */
void rpl_udp_set_id(int argc, char **argv);

/**
 * @brief   Loops through the routing table
 */
void rpl_udp_loop(int argc, char **argv);

/**
 * @brief   Shows the routing table
 */
void rpl_udp_table(int argc, char **argv);

/**
 * @brief   Shows the dodag
 */
void rpl_udp_dodag(int argc, char **argv);

/* helper command handlers */
void rpl_udp_ip(int argc, char **argv);

void rpl_udp_ignore(uint16_t addr);
void rpl_udp_ignore_cmd(int argc, char **argv);

/* monitoring thread */
void rpl_udp_monitor(void);

extern radio_address_t id;
extern ipv6_addr_t std_addr;
extern char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif /* DEMO_H */
