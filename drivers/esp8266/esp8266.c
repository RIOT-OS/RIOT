/*
 * Copyright (C) 2014 INRIA
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_esp8266
 * @{
 *
 * @file
 * @brief       High-level driver implementation for the ESP8266
 *
 * @author      Markus Blechschmidt <markus.blechschmidt@haw-hamburg.de>
 *
 * @}
 */

#include "esp8266.h"
#include <stdlib.h>
#include <string.h>
#include "errno.h"
#include "msg.h"
#include "thread.h"
#include "include/esp_AT_commands.h"
#include "include/esp_AT_response.h"

#define ENABLE_DEBUG  (0)
#include "debug.h"


/* FUNCTION PROTOTYPES */
static response_t _send_at_command(esp8266_t *dev, const command_t command, const cmd_mode_t cmd_mode, const char *args);
static void _disbale_echo_esp(esp8266_t *dev);
static int _parse_mac(char *mac_str, mac_addr_t *mac_addr);

/* IMPLEMENTATION */

/*
 * UART receive callback function
 */
static void _rx_cb(void *arg, uint8_t c)
{
    esp8266_t *dev = (esp8266_t *)arg;

    /* prevent buffer overflow; leave byte for null termination*/
    if (dev->rx_count >= ESP8266_MAX_PKT_LENGTH) {
        return;
    }
    /* copy char into buffer */
    dev->rx_buf[dev->rx_count] = c;
    dev->rx_count++;
    /* check if end of message is AT response terminator */
    for (int i = 0; i < sizeof(terminators) / sizeof(terminator_t *) / 2; i++) {
        int term_len = terminators[i].len;
        if (dev->rx_count >= term_len) {
            if (memcmp(&(dev->rx_buf[dev->rx_count - term_len]), terminators[i].str,
                       term_len) == 0) {
                /* null terminate string in rx_buf and cut off terminator*/
                dev->rx_buf[dev->rx_count - term_len] = 0;
                DEBUG("(II) %s, uart chars, %d: \"%s%s\"\r\n\r\n",
                      __FUNCTION__, dev->rx_count, dev->rx_buf, terminators[i].str);
                /* prepare new message */
                msg_t rx_msg;
                rx_msg.type = 0;
                /* memorize wether query succeded */
                rx_msg.content.value = terminators[i].term_mode;
                /* cut off the postfix (information is contained in message content value) */
                dev->rx_count -= strlen(terminators[i].str);
                msg_send(&rx_msg, dev->resp_handler);
            }
        }
    }
}

/*
 * AT command relevant functions
 */

static void _build_at_command_in_buf(esp8266_t *dev, const command_t cmd, const cmd_mode_t cmd_mode,
                                     const char *args)
{
    int command_len = 0;

    /* determine string length */
    command_len += strlen(PREFIX);
    command_len += strlen(cmd.instr);
    char *mode = "";
    switch (cmd_mode) {
        case test:
            mode = TEST;
            break;
        case query:
            mode = QUERY;
            break;
        case set:
            mode = SET;
            break;
        case exec:
            mode = EXEC;
            break;
    }
    command_len += strlen(mode);
    command_len += strlen(args);
    command_len += strlen(POSTFIX);
    /* concat strings into transmit buffer */
    sprintf(dev->tx_buf, "%s%s%s%s%s", PREFIX, cmd.instr, mode, args, POSTFIX);
}

static response_t _send_at_command(esp8266_t *dev, const command_t cmd,
                                   const cmd_mode_t cmd_mode, const char *args)
{
    response_t ret = { "", no_resp };

    if (!((cmd.cmd_mode & cmd_mode) != 0)) {
        DEBUG("(EE) %s, Wrong mode for command\r\n", __FUNCTION__);
        return ret;
    }
    /* build AT command and determine length */
    _build_at_command_in_buf(dev, cmd, cmd_mode, args);
    int cmd_len = strlen(dev->tx_buf);
    DEBUG("(II) %s, command, %d: \"%s\"\r\n",
          __FUNCTION__, strlen(dev->tx_buf), dev->tx_buf);
    /* register this thread as response handler for messaging*/
    msg_t msg;
    dev->resp_handler = thread_getpid();
    uart_write(dev->uart, (uint8_t *)dev->tx_buf, cmd_len);
    /* wait for respose */
    msg_receive(&msg);
    /* evaluate response */
    ret.term_mode = msg.content.value;
    DEBUG("(II) %s, response, %d: \"%s\" \r\n",
          __FUNCTION__, strlen(dev->rx_buf), dev->rx_buf);
    /* reset input buffer index and unlock uart mutex */
    dev->rx_count = 0;
    return ret;
}

static void _disbale_echo_esp(esp8266_t *dev)
{
    _send_at_command(dev, ECHO, exec, "0");
}

/*
 * helping functions
 */
static int _parse_mac(char *mac_str, mac_addr_t *mac_addr)
{
    if (strlen(mac_str) ==  17) {
        if (mac_str[2] != ':' || mac_str[5] != ':' ||
            mac_str[8] != ':' || mac_str[11] != ':' || mac_str[14] != ':') {
            return -1;
        }
        for (int i = 0; i < 6; i++) {
            long b = strtol(mac_str + (3 * i), (char **) NULL, 16);
            mac_addr->u8[i] = (char)b;
        }
        return 0;
    }
    else {
        return -1;
    }
}

/*
 * public functions
 */
int esp8266_init(esp8266_t *dev)
{
    /* check device and bus parameters */
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->uart >= UART_NUMOF) {
        return -ENXIO;
    }
    dev->rx_count = 0;
    /* initialize buffers and locks*/
    mutex_init(&(dev->uart_lock));
    /* initialize UART */
    if (uart_init(dev->uart, dev->baudrate, _rx_cb, dev) < 0) {
        DEBUG("(EE) %s, Error initializing UART\n", __FUNCTION__);
        return -ENXIO;
    }
    DEBUG("(II) %s, disabling echo\r\n", __FUNCTION__);
    _disbale_echo_esp(dev);
    return 1;
}

void reset_esp(esp8266_t *dev)
{
    _send_at_command(dev, RESET, exec, "");
    return;
}

void print_version_esp(esp8266_t *dev)
{
    response_t resp = _send_at_command(dev, VERSION, exec, "");

    if (resp.term_mode != no_resp) {
        printf("%s\r\n", resp.str);
    }
    return;
}

ipv4_addr_t get_station_ip_esp(esp8266_t *dev)
{
    /* initialize ip adress with 0.0.0.0 */
    ipv4_addr_t ipv4_addr = { .u32.u32 = 0 };

    mutex_lock(&(dev->uart_lock));
    response_t resp =  _send_at_command(dev, STATION_IP, query, "");
    if (resp.term_mode == ok) {
        /* calculate stringlength for ip extraction*/
        int resp_prefix_len = strlen(STATION_IP.instr) + strlen(":\"");
        int ip_end = strlen(dev->rx_buf) - strlen("\"\r\n");
        dev->rx_buf[ip_end] = 0;
        char *ip_str = dev->rx_buf + resp_prefix_len;
        DEBUG("(II) %s, station ip, %d: %s\r\n", __FUNCTION__, strlen(ip_str), ip_str);
        /* extract and convert ip string */
        ipv4_addr_from_str(&ipv4_addr, ip_str);
    }
    else {
        DEBUG("(EE) %s ERROR near line %d\r\n", __FUNCTION__, __LINE__);
    }
    mutex_unlock(&(dev->uart_lock));
    return ipv4_addr;
}

int set_station_ip_esp(esp8266_t *dev, ipv4_addr_t ipv4_addr)
{
    int result = 0;
    char ip_str[IPV4_ADDR_MAX_STR_LEN] = { 0 };
    char args[IPV4_ADDR_MAX_STR_LEN + sizeof("\"\"") - 1] = { 0 };

    /* compose arguments */
    ipv4_addr_to_str(ip_str, &ipv4_addr, IPV4_ADDR_MAX_STR_LEN);
    sprintf(args, "\"%s\"", ip_str);

    /* request and evaluate response */
    mutex_lock(&(dev->uart_lock));
    response_t resp =  _send_at_command(dev, STATION_IP, set, args);
    if (resp.term_mode == ok) {
        result = 1;
    }
    else {
        DEBUG("(EE) %s ERROR near line %d\r\n", __FUNCTION__, __LINE__);
        result = -1;
    }
    mutex_unlock(&(dev->uart_lock));

    return result;
}


mac_addr_t get_station_mac_esp(esp8266_t *dev)
{
    /* initialize MAC adress with 00:00:00:00:00:00 */
    mac_addr_t mac_addr = { .u8 = { 0 } };

    mutex_lock(&(dev->uart_lock));
    response_t resp =  _send_at_command(dev, STATION_MAC, query, "");
    if (resp.term_mode == ok) {
        /* calculate stringlength for ip extraction*/
        int resp_prefix_len = strlen(STATION_MAC.instr) + strlen(":\"");
        int mac_end = strlen(dev->rx_buf) - strlen("\"\r\n");
        dev->rx_buf[mac_end] = 0;
        char *mac_str = dev->rx_buf + resp_prefix_len;
        DEBUG("(II) %s, station mac, %d: %s\r\n", __FUNCTION__, strlen(mac_str), mac_str);
        _parse_mac(mac_str, &mac_addr);
    }
    else {
        DEBUG("(EE) %s ERROR near line %d\r\n", __FUNCTION__, __LINE__);
    }
    mutex_unlock(&(dev->uart_lock));
    return mac_addr;
}

void list_ap_esp(esp8266_t *dev)
{
    response_t resp = _send_at_command(dev, AP_LIST, exec, "");

    if (resp.term_mode != no_resp) {
        printf("%s\r\n", resp.str);
    }
}

int connect_ap_esp(esp8266_t *dev, const char *SSID, const char *PSK)
{
    int result = 0;
    char args[SSID_MAX_L + PSK_MAX_L + sizeof("\"\",\"\"")] = { 0 };

    /* compose arguments */
    sprintf(args, "\"%s\",\"%s\"", SSID, PSK);

    /* request and evaluate response */
    mutex_lock(&(dev->uart_lock));
    response_t resp =  _send_at_command(dev, AP_CONN, set, args);
    if (resp.term_mode == ok) {
        result = 1;
    }
    else {
        DEBUG("(EE) %s ERROR near line %d\r\n", __FUNCTION__, __LINE__);
        result = -1;
    }
    mutex_unlock(&(dev->uart_lock));

    return result;
}
