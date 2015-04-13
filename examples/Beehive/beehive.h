#ifndef SIXLOWAPP_H
#define SIXLOWAPP_H

#include "kernel.h"
#include "ipv6.h"

#define APP_VERSION "1.0"

/**
 * @brief   Which interface should be used for 6LoWPAN
 */
#define IF_ID   (0)

/**
 * @brief   PID for UDP server thread
 */
extern kernel_pid_t sixlowapp_udp_server_pid;

/**
 * @brief   UDP port number that netcat uses to listen at
 */
extern uint16_t sixlowapp_netcat_listen_port;

/**
 * @brief   Helper variable for IP address printing
 */
extern char addr_str[IPV6_MAX_ADDR_STR_LEN];

/**
 * @brief            Shell command to send an ICMPv6 echo request
 *
 * @param[in] argc   Number of arguments supplied to the function invocation.
 * @param[in] argv   The supplied argument list.
 *
 */
void sixlowapp_send_ping(int argc, char **argv);

/**
 * @brief   Shell command for send packet
 *
 * @param[in] argc   Number of arguments supplied to the function invocation.
 * @param[in] argv   The supplied argument list.
 */
void send_packet(int argc, char **argv);

/**
 * @brief   Shell command for send ADC value packet
 *
 * @param[in] argc   Number of arguments supplied to the function invocation.
 * @param[in] argv   The supplied argument list.
 */
void send_adc_value(int argc, char **argv);

/**
 * @brief   Shell command for netcat
 *
 * @param[in] argc   Number of arguments supplied to the function invocation.
 * @param[in] argv   The supplied argument list.
 */
void sixlowapp_netcat(int argc, char **argv);

/**
 * @brief  Wrapper function for sending data of UDP
 *
 * @param[in] dest      Destination IPv6 address
 * @param[in] port      Destination UDP port
 * @param[in] payload   Data to send
 * @param[in] len       Size of data to send
 */
void sixlowapp_udp_send(ipv6_addr_t *dest, uint16_t port, char *payload, size_t len);

/**
 * @brief               Monitoring thread
 *
 * @param[in] unused    Obsolete
 */
void *sixlowapp_monitor(void *unused);

/**
 * @brief           UDP server thread
 *
 * @param[in] arg   Obsolete
 */
void *sixlowapp_udp_server_loop(void *arg);

#endif /* SIXLOWAPP_H */
