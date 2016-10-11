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
 * @brief       typedef neccesary for esp AT command; constant AT commands
 *
 * @author      Markus Blechschmidt <markus.blechschmidt@haw-hamburg.de>
 */

#ifndef ESP_AT_COMMANDS_H_
#define ESP_AT_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif


/* defines for constant parts of the AT commands */
#define PREFIX      ("AT")
#define POSTFIX     ("\r\n")
#define TEST        ("=?")
#define QUERY       ("?")
#define SET         ("=")
#define EXEC        ("")

#define SSID_MAX_L  (32)
#define PSK_MAX_L   (64)

/* number of possible modes for each command. one per command required. multiple possible */
typedef enum {
    test = 0b0001,
    query = 0b0010,
    set = 0b0100,
    exec = 0b1000
} cmd_mode_t;

/* struct describing command */
typedef struct {
    char *instr;
    cmd_mode_t cmd_mode;
} command_t;

static const command_t
    RESET =         { "RST", exec },            /* Restart module */
    VERSION =       { "+GMR", exec },           /* View version info */
    ECHO =          { "E", exec },              /* Dis-/Enable echo */
// WIFI_MODE           = {"+CWMODE", test|query|exec},  /* WIFI mode
// (station, AP, station + AP) */
    AP_CONN =       { "+CWJAP", query | set },  /* Connect to AP */
    AP_LIST =       { "+CWLAP", set | exec },   /* List/search available APs */
// AP_DISCONN          = {"+CWQAP", exec},              /* Disconnect from AP
// */
// SOFTAP_CONF         = {"+CWSAP", query|exec},        /* Configuration of
// softAP mode */
// SOFTAP_LIST         = {"+CWLIF", exec},              /* List clients
// connected to softAP */
// DHCP                = {"+CWDHCP", set},              /* Enable/Disable
// DHCP */
    STATION_MAC =   { "+CIPSTAMAC", query | set },  /* Get/set MAC address of station */
// SOFTAP_MAC          = {"+CIPAPMAC", query|set},      /* Set MAC address of
// softAP*/
    STATION_IP = { "+CIPSTA", query | set } /* IP address of station */
// SOFTAP_IP           = {"+CIPAP", query|set},         /* Set ip address of
// softAP */
// CONN_INFO           = {"+CIPSTATUS", test|exec},     /* Information about
// connection */
// CONN_START          = {"+CIPSTART", set|test},       /* Establish TCP
// connection or register UDP port and start a connection */
// CONN_SEND           = {"+CIPSEND", test|set|exec},   /* Send data */
// CONN_CLOSE          = {"+CIPCLOSE", test|set|exec},  /* Close TCP or UDP
// connection */
// CONN_ENABLE_MULT    = {"+CIPMUX", set|query},        /* Enable multiple
// connections or not */
// SERVER_CONFIG       = {"+CIPSERVER", set},           /* Configure as server */
// SERVER_TRANS_MODE   = {"+CIPMODE", query|set},       /* Set transfer mode */
// SERVER_TIMEOUT      = {"+CIPSTO", query|set}         /* Set server timeout */

/* unsupported; unnecessary */
// DEEP_SLEEP          = {"+GSLP", set},                /* Enter deep-sleep mode */
// STATION_GET_IP      = {"+CIFSR", test|exec},         /* Get local IP and MAC address */
;



#ifdef __cplusplus
}
#endif

#endif /* ESP_AT_COMMANDS_H_ */
/** @} */
