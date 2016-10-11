/*
 * Copyright (C) 2016 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_esp8266 ESP8266 driver
 * @ingroup     drivers_netdev
 * @brief       High-level driver for the ESP8266 modem
 * @{
 *
 * @file
 * @brief       High-level driver for the ESP8266 modem
 *
 * @author      Markus Blechschmidt <markus.blechschmidt@haw-hamburg.de>
 */

#ifndef ESP8266_H_
#define ESP8266_H_

#include <stdint.h>
#include <net/ipv4/addr.h>
#include "mutex.h"
#include "thread.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ESP8266_MAX_PKT_LENGTH      (4096U)



/**
 * @brief   esp8266 device descriptor
 */
typedef struct {
    /* device driver specific fields */
    uart_t uart;                                /**< UART interfaced used */
    uint32_t baudrate;
    /* values for the UART TX state machine */
    mutex_t uart_lock;                          /**< mutex to allow only exclusive use of UART */
    char tx_buf[ESP8266_MAX_PKT_LENGTH];        /**< transmit data buffer */
    char rx_buf[ESP8266_MAX_PKT_LENGTH];        /**< receiving data buffer */
    uint16_t rx_count;                          /**< counter for incoming bytes */
    kernel_pid_t resp_handler;                  /**< pid of thread awaiting response */
} esp8266_t;

/**
 * @brief   MAC address struct
 */
typedef struct {
    uint8_t u8[6];
} mac_addr_t;

/**
 * @brief   Initializes an ESP8266
 *
 * @param   dev     Descriptor of the esp8266 to be initialized.
 *
 * @return  1 if setting the init was successful, 0 otherwise.
 */
int esp8266_init(esp8266_t *dev);

/**
 * @brief   Reset ESP8266
 *
 * @param   dev     Descriptor of the ESP8266.
 */
void reset_esp(esp8266_t *dev);

/**
 * @brief   Prints ESP8266 firmware version
 *
 * @param   dev     Descriptor of the ESP8266.
 */
void print_version_esp(esp8266_t *dev);

/**
 * @brief   Get ESP8266 station IP address
 *
 * @param   dev     Descriptor of the ESP8266.
 *
 * @return  The IPv4 address of the ESP8266 in station mode.
 * @return  0.0.0.0 when not connected to station.
 */
ipv4_addr_t get_station_ip_esp(esp8266_t *dev);

/**
 * @brief   Set ESP8266 station IP address
 *
 * @param   dev         Descriptor of the ESP8266.
 * @param   ipv4_addr   adress to be set to
 * @return  1 if succeded
 * @return  -1 if failed
 */
int set_station_ip_esp(esp8266_t *dev, ipv4_addr_t ipv4_addr);

/**
 * @brief   Get ESP8266 station MAC address
 *
 * @param   dev     Descriptor of the ESP8266.
 *
 * @return  The MAC address of the ESP8266 in station mode.
 */
mac_addr_t get_station_mac_esp(esp8266_t *dev);

/**
 * @brief   Print available APs to output
 *
 * @param   dev     Descriptor of the ESP8266.
 */
void list_ap_esp(esp8266_t *dev);

/**
 * @brief   Connect to an 802.11 access point
 *
 * @param   dev     Descriptor of the ESP8266.
 * @param   SSID    Service Set Identifier of the access point.
 * @param   PSK     Pre Shared Kay access point.
 * @return  1 if succeded
 * @return  -1 if failed
 */
int connect_ap_esp(esp8266_t *dev, const char* SSID, const char* PSK);

#ifdef __cplusplus
}
#endif

#endif /* ESP8266_H_ */
/** @} */
