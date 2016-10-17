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

#define ENABLE_DEBUG  (1)
#include "debug.h"
#include "esp8266.h"
#include "xtimer.h"
#include "periph/uart.h"
#include "thread.h"


ESP8266_t esp8266;
char threadstack[THREAD_STACKSIZE_DEFAULT];

void* update_loop(void* args){
    ESP8266_t* esp8266_p = (ESP8266_t*) args;
    uint32_t last_wakeup = xtimer_now();
    while(1){
        xtimer_periodic_wakeup(&last_wakeup, 1000);
        ESP8266_Update(esp8266_p);
        ESP8266_TimeUpdate(esp8266_p, 1);
    }
    return NULL;
}

int main(void)
{
    DEBUG("starting %s\r\n", __FILE__);
    thread_create(threadstack, THREAD_STACKSIZE_DEFAULT,
                  THREAD_PRIORITY_MAIN - 1, 0, update_loop, &esp8266, "esp_update");
    ESP8266_Init(&esp8266, 115200);
    ESP8266_SetMode(&esp8266, ESP8266_Mode_STA);
    ESP8266_WifiConnect(&esp8266, "SSID", "PSK");
    
    return 0;
}

/* WIFI */
void ESP8266_Callback_WifiGotIP(ESP8266_t* ESP8266){
    DEBUG("WiFi connected and got IP\r\n");
    ESP8266_StartClientConnectionUDP(ESP8266, "test", "192.168.1.248", 1337, 1337, "");
}

void ESP8266_Callback_WifiDisconnected(ESP8266_t* ESP8266){
    DEBUG("WiFi disconnected\r\n");

}

void ESP8266_Callback_WifiConnectFailed(ESP8266_t* ESP8266){
    DEBUG("WiFi connect failed\r\n");
}

/* TCP CONNECTION */
void ESP8266_Callback_ClientConnectionConnected(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
    DEBUG("Client connected\r\n");
    while(1){
        ESP8266_WaitReady(ESP8266);
        ESP8266_RequestSendData(ESP8266, Connection);
        xtimer_sleep(1);
    }
}

void ESP8266_Callback_ClientConnectionClosed(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
    DEBUG("Connection closed\r\n");
    ESP8266_WaitReady(ESP8266);
    ESP8266_WifiDisconnect(ESP8266);
}

void ESP8266_Callback_ClientConnectionError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
    DEBUG("Connection Error\r\n");
    ESP8266_WaitReady(ESP8266);
    ESP8266_WifiDisconnect(ESP8266);
}

uint16_t ESP8266_Callback_ClientConnectionSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size) {
    sprintf(Buffer, "hello world\r\n");
    return strlen(Buffer);
}

void ESP8266_Callback_ClientConnectionDataReceived(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer) {
    DEBUG("Data received from server on connection: %s\r\n",
        Connection->Name
    );
}

void ESP8266_Callback_ClientConnectionDataSent(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
    DEBUG("Data successfully sent as client!\r\n");
    // ESP8266_WaitReady(ESP8266);
    // ESP8266_CloseConnection(ESP8266, Connection);
}
