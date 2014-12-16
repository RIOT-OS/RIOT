/*
 * Copyright (C) 2013, 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @file
 * @brief Node application
 *
 *
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h> 
#include "rpl/rpl_dodag.h"
#include "thread.h"
#include "socket_base/socket.h"
#include "net_help.h"
#include "node.h"
#include "sensor_driver.h"
#include "configuration.h" 
#define ENABLE_DEBUG    (1)
#include "debug.h"
#define SERVER_PORT     (0xFF01)

char addr_str[IPV6_MAX_ADDR_STR_LEN];
msg_t msg_q[MSG_RCV_BUFFER_SIZE];

/* Message types node thread communication */
enum node_msg_type_t {
    WEIGHT,
    BATTERY,
    TEMPERATURE,
    HUMIDITY,
    TESTSMS
};

/* Node UDP Thread */
void *node_udp_thread(void *arg)
{       
    
    msg_init_queue(msg_q, RCV_BUFFER_SIZE);
    while (1) 
    {  
        msg_t m;
        msg_receive(&m);
        udp_send_payload(m.type);  
    }
    return NULL;
}

/* Node payload Thread */
void *node_payload_thread(void *arg)
{       
    msg_t m;
    while (1) 
    {  
        m.type = WEIGHT;
        msg_send(&m, udp_node_thread_pid);
        m.type = BATTERY;
        msg_send(&m, udp_node_thread_pid);
        m.type = TEMPERATURE;
        msg_send(&m, udp_node_thread_pid);
        m.type = HUMIDITY;
        if(msg_send(&m, udp_node_thread_pid))
        {
            sleep(SLEEP_SECONDS);
        }
        else
        {
            DEBUG("%s\n", "node_payload_thread: msg_send == 0. ");
            sleep(SLEEP_SECONDS);
        }
        //  for(int i = 0; i < 10; i++){
        //  vtimer_usleep(500000);
        // }
    }
    return NULL;
}

/* UDP send payload */
void udp_send_payload(int cmd)//int argc, char **argv)
{
    int sock;
    sockaddr6_t sa;
    ipv6_addr_t ipaddr;
    rpl_dodag_t *mydodag;
    int bytes_sent;
    char buffer[64];    
    char* payload = buffer;
    
    mydodag = rpl_get_my_dodag();

    if (mydodag == NULL) {
        DEBUG("Not part of a dodag\n");
        return;
    }
    if ((&mydodag->dodag_id) == NULL) {
        DEBUG("\ndodag_id == null\n");
        return;
    }
    ipaddr = *(&mydodag->dodag_id);

        // ipv6_addr_init(&ipaddr, 
        //     HTONS((&mydodag->dodag_id)->uint16[0]), 
        //     HTONS((&mydodag->dodag_id)->uint16[1]), 
        //     HTONS((&mydodag->dodag_id)->uint16[2]),
        //     HTONS((&mydodag->dodag_id)->uint16[3]), 
        //     HTONS((&mydodag->dodag_id)->uint16[4]), 
        //     HTONS((&mydodag->dodag_id)->uint16[5]),
        //     HTONS((&mydodag->dodag_id)->uint16[6]),
        //     HTONS((&mydodag->dodag_id)->uint16[7]));   


    switch(cmd)
    {
        case WEIGHT:
            getWeightPayload(payload);
            break;
        case BATTERY:
            getBatteryPayload(payload);
            break;
        case TEMPERATURE:
            getTemperaturePayload(payload);
            break;
        case HUMIDITY:
            getHumidityPayload(payload);
            break;
        case TESTSMS:
            getTestSmsPayload(payload);
            break;
        default:
            DEBUG("\n\n Invalid cmd in send \n\n");
            return;
    }

    sock = socket_base_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    
    if (-1 == sock) {
        DEBUG("Error Creating Socket!");
        return;
    }
    
    memset(&sa, 0, sizeof(sa));    
    sa.sin6_family = AF_INET;
    DEBUG("\n BEFORE memcpy &mydodag->dodag_id \n");
    if(&mydodag->dodag_id)
    {
        //memcpy(&sa.sin6_addr, &mydodag->dodag_id, 16);
        memcpy(&sa.sin6_addr, &ipaddr, 16);
    }
    else
    {
        DEBUG("\n memcpy &mydodag->dodag_id DEAD!\n");
        return;
    }

    //memcpy(&sa.sin6_addr, &ipaddr, 16);
    sa.sin6_port = HTONS(SERVER_PORT);

    if(strlen(payload) > 0)
    {   
        bytes_sent = socket_base_sendto(sock, (char *)payload, strlen(payload) + 1, 0, &sa, sizeof(sa));
        if (bytes_sent < 0) 
        {
            DEBUG("Error sending packet!\n");
        }
        else 
        {
            DEBUG("Successful deliverd %i bytes over UDP to %s to 6LoWPAN\n",
                   bytes_sent, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                                                &ipaddr));
        }
        socket_base_close(sock);
    }
    else
    {
        DEBUG("\n\n Payload length <= 0 - ERROR!\n\n");
        return;
    }
}

void getWeightPayload(char* _p)
{
    uint32_t result = 0;
    char weightStr[6];
    char hwIdStr[6];

    uint32_t adcValue = getAdcValue();
    result = getWeightValueFromAdcInput(adcValue);
    DEBUG("\n\nadcValue: %d \n\n", (int)adcValue);
    DEBUG("result: %d \n", (int)result);

    sprintf(&weightStr, "%d", (int)result);
    sprintf(&hwIdStr, "%d", (int)id);

    strcpy(_p, PutData0);
    strcat(_p, PutData1);
    strcat(_p, hwIdStr);
    strcat(_p, PutData2);
    strcat(_p, "weight");
    strcat(_p, PutData3);
    strcat(_p, weightStr);
    strcat(_p, PutData4);

    DEBUG("\n\nWeight Payload is: %s\n\n", _p);
}

void getBatteryPayload(char* _p)
{
    uint32_t result = 0;
    char weightStr[6];
    char hwIdStr[6];

    uint32_t adcValue = getAdcValue();
    result = (uint32_t)getBatteryValueFromAdcInput(adcValue); // STUB
    DEBUG("\n\nadcValue: %d \n\n", (int)adcValue);
    DEBUG("result: %d \n", (int)result);

    sprintf(&weightStr, "%d", (int)result);
    sprintf(&hwIdStr, "%d", (int)id);

    strcpy(_p, PutData0);
    strcat(_p, PutData1);
    strcat(_p, hwIdStr);
    strcat(_p, PutData2);
    strcat(_p, "battery");
    strcat(_p, PutData3);
    strcat(_p, weightStr);
    strcat(_p, PutData4);

    DEBUG("\n\nBattery Payload is: %s\n\n", _p);
}

void getTemperaturePayload(char* _p)
{
    uint32_t result = 0;
    char weightStr[6];
    char hwIdStr[6];

    uint32_t i2cValue = 34;//getAdcValue();
    result = (uint32_t)getTemperatureValueFromI2cInput(i2cValue); // STUB
    DEBUG("\n\nadcValue: %d \n\n", (int)i2cValue);
    DEBUG("result: %d \n", (int)result);

    sprintf(&weightStr, "%d", (int)result);
    sprintf(&hwIdStr, "%d", (int)id);

    strcpy(_p, PutData0);
    strcat(_p, PutData1);
    strcat(_p, hwIdStr);
    strcat(_p, PutData2);
    strcat(_p, "temp");
    strcat(_p, PutData3);
    strcat(_p, weightStr);
    strcat(_p, PutData4);

    DEBUG("\n\nTemperature Payload is: %s\n\n", _p);
}

void getHumidityPayload(char* _p)
{
    uint32_t result = 0;
    char weightStr[6];
    char hwIdStr[6];

    uint32_t i2cValue = 65;//getAdcValue();
    result = (uint32_t)getHumidityValueFromI2cInput(i2cValue); // STUB
    DEBUG("\n\nadcValue: %d \n\n", (int)i2cValue);
    DEBUG("result: %d \n", (int)result);

    sprintf(&weightStr, "%d", (int)result);
    sprintf(&hwIdStr, "%d", (int)id);

    strcpy(_p, PutData0);
    strcat(_p, PutData1);
    strcat(_p, hwIdStr);
    strcat(_p, PutData2);
    strcat(_p, "humi");
    strcat(_p, PutData3);
    strcat(_p, weightStr);
    strcat(_p, PutData4);

    DEBUG("\n\nHumidity Payload is: %s\n\n", _p);
}

void getTestSmsPayload(char* _p) // PINA19
{    
    char hwIdStr[6];
    sprintf(&hwIdStr, "%d", (int)id);

    strcpy(_p, cmdTestSms0);
    strcat(_p, cmdTestSms1);
    strcat(_p, hwIdStr);
    strcat(_p, cmdTestSms2);

    DEBUG("\n\nTestSMS Payload is: %s\n\n", _p);
}

void node_tara_callback(void)
{
    DEBUG("\n\n\n %s \n\n\n", "node_tara_callback TRIGGERED!" );
    /* TODO: HANDLE TARA !!! */ 

}

void node_testsms_callback(void)
{
    DEBUG("\n\n\n %s \n\n\n", "node_testsms_callback TRIGGERED!" );
    msg_t m;
    m.type = TESTSMS;
    msg_send(&m, udp_node_thread_pid);
}