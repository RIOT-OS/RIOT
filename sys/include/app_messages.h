/*
 * Copyright (C) 2016 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 *
 * @addtogroup     <group>
 * @{
 *
 * @file
 * @brief
 *
 * @author      Attilio Dona' <attilio.dona>
 *
 */

#ifndef SYS_INCLUDE_APP_MESSAGES_H_
#define SYS_INCLUDE_APP_MESSAGES_H_

/**
 * @brief sockloop generated messages
 */
enum {
    MSG_SOCK_CLOSED = 11
};

/**
 * @brief application messages
 */
enum {
    SBAPP_IP_ACQUIRED = 0x1001,
    SBAPP_LINK_DOWN
};

enum {
    TCPSERVER_CONFIG_OK = 0x2000,
    REBOOT_REQUEST
};

enum {
    SET_AP_SECRET = 0x3000
};

// board_io events identifiers
enum {
    BUTTON_SW3_PRESSED = 0x8002,
    BUTTON_SW2_PRESSED,
    BUTTON_SW3_TIMEOUT,
    BUTTON_SW2_TIMEOUT,
    ALARM_EVENT,
    ALARM_TIMEOUT,
    WLAN_STA_ONLINE,
    WLAN_AP_ONLINE,
    WLAN_OFFLINE,
    SERVER_ON,
    SERVER_OFF,
    RED_LED_TIMEOUT // red led toggle timeout
};

#endif /* SYS_INCLUDE_APP_MESSAGES_H_ */
