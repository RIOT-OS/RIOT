/*
 * Copyright (C) 2016 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for esp8266 network device driver
 *
 * @author      Markus Blechschmidt <markus.blechschmidt@haw-hamburg.de>
 *
 * @}
 */

#define ENABLE_DEBUG  (0)
#include "debug.h"
#include "esp8266.h"



esp8266_t esp8266;

int main(void)
{
    DEBUG("begin main\r\n");
    esp8266.uart = 1;
    esp8266.baudrate = 115200;
    DEBUG("calling esp8266_init\r\n");
    esp8266_init(&esp8266);

    connect_ap_esp(&esp8266, "TP-LINK_POCKET_3020_69CB8C", "50204465");

    set_station_ip_esp(&esp8266, (ipv4_addr_t){.u8 = { 192, 168, 0, 101 } });

    ipv4_addr_t station_ip = get_station_ip_esp(&esp8266);
    for (int i = 0; i < 4; i++) {
        printf("%d ", station_ip.u8[i]);
    }
    printf("\r\n");

    mac_addr_t station_mac = get_station_mac_esp(&esp8266);
    for (int i = 0; i < 6; i++) {
        printf("%x ", station_mac.u8[i]);
    }
    printf("\r\n");


    return 0;
}
