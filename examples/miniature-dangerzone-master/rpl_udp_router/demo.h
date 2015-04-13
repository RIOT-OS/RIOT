#ifndef DEMO_H
#define DEMO_H

#include "kernel.h"

#define APP_VERSION "1.1"

#define RADIO_CHANNEL   (10)

#define MONITOR_STACK_SIZE  (KERNEL_CONF_STACKSIZE_MAIN)
#define RCV_BUFFER_SIZE     (32)

/* RPL shell command handlers */
/**
 * @brief   Shell command to initializes RPL and Destiny
 *
 * @param[in] str  Shell input
 */
void init(char *str);

/**
 * @brief   Shell command to set node's ID
 *
 * @param[in] str  Shell input
 */
void set_id(char *id);

/**
 * @brief   Loops through the routing table
 *
 * @param[in] unused  Guess what
 */
void loop(char *unused);

/**
 * @brief   Shows the routing table
 *
 * @param[in] unused  Guess what
 */
void table(char *unused);

/**
 * @brief   Shows the dodag 
 *
 * @param[in] unused  Guess what
 */
void dodag(char *unused);

/* UDP shell command handlers */
void udp_server(char *unused);
void udp_send(char *str);

/* helper command handlers */
void ip(char *unused);

void ignore(char *addr);

/* monitoring thread */
void monitor(void);

extern radio_address_t id;
extern ipv6_addr_t std_addr;
extern char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif /* DEMO_H */
