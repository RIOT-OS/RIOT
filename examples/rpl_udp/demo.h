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
void rpl_udp_init(char *str);

/**
 * @brief   Shell command to set node's ID
 *
 * @param[in] str  Shell input
 */
void rpl_udp_set_id(char *id);

/**
 * @brief   Loops through the routing table
 *
 * @param[in] unused  Guess what
 */
void rpl_udp_loop(char *unused);

/**
 * @brief   Shows the routing table
 *
 * @param[in] unused  Guess what
 */
void rpl_udp_table(char *unused);

/**
 * @brief   Shows the dodag
 *
 * @param[in] unused  Guess what
 */
void rpl_udp_dodag(char *unused);

/* UDP shell command handlers */
void udp_server(char *unused);
void udp_send(char *str);

/* helper command handlers */
void rpl_udp_ip(char *unused);

void rpl_udp_ignore(char *addr);

/* monitoring thread */
void rpl_udp_monitor(void);

extern radio_address_t id;
extern ipv6_addr_t std_addr;
extern char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif /* DEMO_H */
