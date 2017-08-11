/*
 * Copyright (C) 2016 Attilio Dona'
 *
 ***********************************************************************
 Derived from:

 Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the
    distribution.

    Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * @file
 * @ingroup         cpu_cc3200
 * @{
 *
 * @brief Contains the common macro/enum definitions used by networking apps
 *
 * @author          Attilio Dona'
 */

#ifndef __NWP_CONF__H__
#define __NWP_CONF__H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define SSID_LEN_MAX        32
#define BSSID_LEN_MAX       6

#define SPAWN_TASK_PRIORITY     2
#define SL_STOP_TIMEOUT         200
#define UNUSED(x)               ((x) = (x))
#define SUCCESS                 0
#define FAILURE                 -1

typedef enum {
    AUTO_CONNECTION_TIME_SLOT,   // timeout for auto connection slot period

    SMARTCONFIG_ACTIVE,          // waiting password from smartconfig

    WLAN_CONNECTED,              // Link established

    WLAN_DISCONNECTED,           // Link broken

    IP_ACQUIRED,                 // ip address assignment

    SOCKET_CONNECTION,           // a socket connection event

    LOST_IN_PANIC,               // blocked in an critical condition

} nwp_event;

// @brief Status bits - These are used to set/reset the corresponding bits in
// given variable
typedef enum {
    STATUS_BIT_NWP_INIT = 0, //!< If this bit is set: Network Processor is
                             //!< powered up

    STATUS_BIT_CONNECTION,   //!< If this bit is set: the device is connected to
                             //!< the AP or client is connected to device (AP)

    STATUS_BIT_IP_LEASED,    //!< If this bit is set: the device has leased IP to
                             //!< any connected client

    STATUS_BIT_IP_ACQUIRED,  //!< If this bit is set: the device has acquired an IP

    STATUS_BIT_SMARTCONFIG_START, //!< If this bit is set: the SmartConfiguration
                                  //!< process is started from SmartConfig app

    STATUS_BIT_P2P_DEV_FOUND,    //!< If this bit is set: the device (P2P mode)
                                 //!< found any p2p-device in scan

    STATUS_BIT_P2P_REQ_RECEIVED, //!< If this bit is set: the device (P2P mode)
                                 //!< found any p2p-negotiation request

    STATUS_BIT_CONNECTION_FAILED, //!< If this bit is set: the device(P2P mode)
                                  //!< connection to client(or reverse way) is failed

    STATUS_BIT_PING_DONE,        //!< If this bit is set: the device has completed
                                 //!< the ping operation
} e_StatusBits;

#define CLR_STATUS_BIT_ALL(status_variable)  (status_variable = 0)
#define SET_STATUS_BIT(status_variable, bit) status_variable |= (1<<(bit))
#define CLR_STATUS_BIT(status_variable, bit) status_variable &= ~(1<<(bit))
#define CLR_STATUS_BIT_ALL(status_variable)   (status_variable = 0)
#define GET_STATUS_BIT(status_variable, bit) (0 != (status_variable & (1<<(bit))))

#define IS_NW_PROCSR_ON(status_variable)     GET_STATUS_BIT(status_variable,\
                                                            STATUS_BIT_NWP_INIT)
#define IS_CONNECTED(status_variable)        GET_STATUS_BIT(status_variable,\
                                                         STATUS_BIT_CONNECTION)
#define IS_IP_LEASED(status_variable)        GET_STATUS_BIT(status_variable,\
                                                           STATUS_BIT_IP_LEASED)
#define IS_IP_ACQUIRED(status_variable)       GET_STATUS_BIT(status_variable,\
                                                          STATUS_BIT_IP_ACQUIRED)
#define IS_SMART_CFG_START(status_variable)  GET_STATUS_BIT(status_variable,\
                                                   STATUS_BIT_SMARTCONFIG_START)
#define IS_P2P_DEV_FOUND(status_variable)    GET_STATUS_BIT(status_variable,\
                                                       STATUS_BIT_P2P_DEV_FOUND)
#define IS_P2P_REQ_RCVD(status_variable)     GET_STATUS_BIT(status_variable,\
                                                    STATUS_BIT_P2P_REQ_RECEIVED)
#define IS_CONNECT_FAILED(status_variable)   GET_STATUS_BIT(status_variable,\
                                                   STATUS_BIT_CONNECTION_FAILED)
#define IS_PING_DONE(status_variable)        GET_STATUS_BIT(status_variable,\
                                                           STATUS_BIT_PING_DONE)

// @brief simplelink active role initialization value
#define ROLE_INVALID            (-5)


/**
 * @brief network processor status and configuration
 */
typedef struct nwp_t {
    volatile uint16_t status;
    unsigned char ssid[SSID_LEN_MAX + 1];
    unsigned char bssid[BSSID_LEN_MAX + 1];
    uint32_t ip;
    uint32_t gw_ip;
    uint16_t role;   //< the current active role (STA/AP/P2P) or ROLE_INVALID
} nwp_t;

// network processor status and config handle
extern nwp_t nwp;


#ifdef __cplusplus
}
#endif
#endif //__NWP_CONF__H__
/** @} */
