#ifndef RPL_UDP_H
#define RPL_UDP_H

#define APP_VERSION "1.1"

#define RADIO_CHANNEL   (10)

#define MONITOR_STACK_SIZE  (KERNEL_CONF_STACKSIZE_MAIN)
#define RCV_BUFFER_SIZE     (32)

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

/* UDP shell command handlers */
void udp_server(int argc, char **argv);
void udp_send(int argc, char **argv);

/* helper command handlers */
void rpl_udp_ip(int argc, char **argv);

void rpl_udp_ignore(int argc, char **argv);

/* monitoring thread */
void *rpl_udp_monitor(void *arg);

extern radio_address_t id;
extern ipv6_addr_t std_addr;
extern char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif /* RPL_UDP_H */
