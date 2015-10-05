//*****************************************************************************
// network_if.c
//
// Networking interface functions for CC3200 device
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include <hw_types.h>
#include <string.h>
#include <stdlib.h>

// Simplelink includes 
#include "simplelink.h"

// driverlib includes 
#include "timer.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"

// free-rtos/TI-rtos include
#ifdef SL_PLATFORM_MULTI_THREADED
#include "osi.h"
#endif

// common interface includes 
#include "network_if.h"
#ifndef NOTERM
#include <stdio.h>
#endif

//#include "timer_if.h"
#include "common.h"

#include "periph/uart.h"


int uart_read_line(uart_t uart, char *pcBuffer, unsigned int uiBufLen);

#define GetCmd(CHAR_PTR, LEN)  uart_read_line(STDIO, CHAR_PTR, LEN)



// Network App specific status/error codes which are used only in this file
typedef enum{
     // Choosing this number to avoid overlap w/ host-driver's error codes
    DEVICE_NOT_IN_STATION_MODE = -0x7F0,
    DEVICE_NOT_IN_AP_MODE = DEVICE_NOT_IN_STATION_MODE - 1,
    DEVICE_NOT_IN_P2P_MODE = DEVICE_NOT_IN_AP_MODE - 1,

    STATUS_CODE_MAX = -0xBB8
}e_NetAppStatusCodes;

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
volatile unsigned long  g_ulStatus = 0;   /* SimpleLink Status */
unsigned long  g_ulStaIp = 0;    /* Station IP address */
unsigned long  g_ulGatewayIP = 0; /* Network Gateway IP address */
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; /* Connection SSID */
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; /* Connection BSSID */
volatile unsigned short g_usConnectIndex; /* Connection time delay index */
const char     pcDigits[] = "0123456789"; /* variable used by itoa function */
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


#define IS_SPACE(x)       (x == 32 ? 1 : 0)

//*****************************************************************************
//
//!    Trim the spaces from left and right end of given string
//!
//! \param  Input string on which trimming happens
//!
//! \return length of trimmed string
//
//*****************************************************************************
int TrimSpace(char * pcInput)
{
    size_t size;
    char *endStr, *strData = pcInput;
    char index = 0;
    size = strlen(strData);

    if (!size)
        return 0;

    endStr = strData + size - 1;
    while (endStr >= strData && IS_SPACE(*endStr))
        endStr--;
    *(endStr + 1) = '\0';

    while (*strData && IS_SPACE(*strData))
    {
        strData++;
        index++;
    }
    memmove(pcInput,strData,strlen(strData)+1);

    return strlen(pcInput);
}




#ifdef USE_FREERTOS
//*****************************************************************************
// FreeRTOS User Hook Functions enabled in FreeRTOSConfig.h
//*****************************************************************************

//*****************************************************************************
//
//! \brief Application defined hook (or callback) function - assert
//!
//! \param[in]  pcFile - Pointer to the File Name
//! \param[in]  ulLine - Line Number
//! 
//! \return none
//!
//*****************************************************************************
void
vAssertCalled( const char *pcFile, unsigned long ulLine )
{
    //Handle Assert here
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined idle task hook
//! 
//! \param  none
//! 
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook( void)
{
    //Handle Idle Hook for Profiling, Power Management etc
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//! 
//! \param  none
//! 
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    //Handle Memory Allocation Errors
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//! 
//! \param  none
//! 
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook( OsiTaskHandle *pxTask,
                                   signed char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
#endif //USE_FREERTOS


//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************


//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pSlWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pSlWlanEvent)
{
    if(pSlWlanEvent == NULL)
    {
        return;
    }

     switch(((SlWlanEvent_t*)pSlWlanEvent)->Event)
     {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION_FAILED);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'-Applications
            // can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.STAandP2PModeWlanConnected;
            //
            
            // Copy new connection SSID and BSSID to global parameters
            memcpy(g_ucConnectionSSID,pSlWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pSlWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                    pSlWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);

            UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s , BSSID: "
                        "%x:%x:%x:%x:%x:%x\n\r", g_ucConnectionSSID,
                        g_ucConnectionBSSID[0], g_ucConnectionBSSID[1], 
                        g_ucConnectionBSSID[2], g_ucConnectionBSSID[3],
                        g_ucConnectionBSSID[4], g_ucConnectionBSSID[5]);
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pSlWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request, 
            //'reason_code' is SL_USER_INITIATED_DISCONNECTION 
            if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s, "
                           "BSSID: %x:%x:%x:%x:%x:%x on application's request "
                           "\n\r", g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s,"
                           " BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
        }
        break;

        case SL_WLAN_STA_CONNECTED_EVENT:
        {
            // when device is in AP mode and any client connects to device cc3xxx
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION_FAILED);

            //
            // Information about the connected client (like SSID, MAC etc) will 
            // be available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModeStaConnected;
            //

        }
        break;

        case SL_WLAN_STA_DISCONNECTED_EVENT:
        {
            // when client disconnects from device (AP)
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

            //
            // Information about the connected client (like SSID, MAC etc) will
            // be available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModestaDisconnected;
            //
        }
        break;

        case SL_WLAN_SMART_CONFIG_COMPLETE_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_SMARTCONFIG_START);

            //
            // Information about the SmartConfig details (like Status, SSID, 
            // Token etc) will be available in 'slSmartConfigStartAsyncResponse_t' 
            // - Applications can use it if required
            //
            //  slSmartConfigStartAsyncResponse_t *pEventData = NULL;
            //  pEventData = &pSlWlanEvent->EventData.smartConfigStartResponse;
            //

        }
        break;

        case SL_WLAN_SMART_CONFIG_STOP_EVENT:
        {
            // SmartConfig operation finished
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_SMARTCONFIG_START);

            //
            // Information about the SmartConfig details (like Status, padding 
            // etc) will be available in 'slSmartConfigStopAsyncResponse_t' - 
            // Applications can use it if required
            //
            // slSmartConfigStopAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.smartConfigStopResponse;
            //
        }
        break;

        case SL_WLAN_P2P_DEV_FOUND_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_P2P_DEV_FOUND);
            
            //
            // Information about P2P config details (like Peer device name, own 
            // SSID etc) will be available in 'slPeerInfoAsyncResponse_t' - 
            // Applications can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.P2PModeDevFound;
            //
        }
        break;

        case SL_WLAN_P2P_NEG_REQ_RECEIVED_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_P2P_REQ_RECEIVED);
            
            //
            // Information about P2P Negotiation req details (like Peer device 
            // name, own SSID etc) will be available in 'slPeerInfoAsyncResponse_t'
            //  - Applications can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.P2PModeNegReqReceived;
            //
        }
        break;

        case SL_WLAN_CONNECTION_FAILED_EVENT:
        {
            // If device gets any connection failed event
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION_FAILED);
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event \n\r");
        }
        break;
     }

}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param pNetAppEvent - Pointer indicating device acquired IP
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(pNetAppEvent == NULL)
    {
        return;
    }

    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        case SL_NETAPP_IPV6_IPACQUIRED_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);
            UART_PRINT("[NETAPP EVENT] IP acquired by the device\n\r");
            
            //
            // Information about the IPv4 & IPv6 details (like IP, gateway,dns
            // etc) will be available in 'SlIpV4AcquiredAsync_t / 
            // SlIpV6AcquiredAsync_t' - Applications can use it if required
            //
            // For IPv4:
            //
            // SlIpV4AcquiredAsync_t *pEventData = NULL;
            // pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
            //
            // For IPv6:
            //
            // SlIpV6AcquiredAsync_t *pEventData = NULL;
            // pEventData = &pNetAppEvent->EventData.ipAcquiredV6;
            //

        }
        break;

        case SL_NETAPP_IP_LEASED_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);
            UART_PRINT("[NETAPP EVENT] IP leased to a client\n\r");
            
            //
            // Information about the IP-Leased details(like IP-Leased,lease-time,
            // mac etc) will be available in 'SlIpLeasedAsync_t' - Applications
            // can use it if required
            //
            // SlIpLeasedAsync_t *pEventData = NULL;
            // pEventData = &pNetAppEvent->EventData.ipLeased;
            //

            SlIpLeasedAsync_t *pEventData = NULL;
            pEventData = &pNetAppEvent->EventData.ipLeased;
            g_ulStaIp = pEventData->ip_address;
        }
        break;

        case SL_NETAPP_IP_RELEASED_EVENT:
        {
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);
            UART_PRINT("[NETAPP EVENT] IP released from a client\n\r");

            //
            // Information about the IP-Released details (like IP-address, mac 
            // etc) will be available in 'SlIpReleasedAsync_t' - Applications
            // can use it if required
            //
            // SlIpReleasedAsync_t *pEventData = NULL;
            // pEventData = &pNetAppEvent->EventData.ipReleased;
            //
        }
        break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event \n\r");
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info 
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    if(pDevEvent == NULL)
    {
        return;
    }

    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status, 
               pDevEvent->EventData.deviceEvent.sender);
}

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(pSock == NULL)
    {
        return;
    }

    //
    // This application doesn't work w/ socket - Events are not expected
    //
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status)
            {
                case SL_ECLOSE: 
                    UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\n", 
                                    pSock->socketAsyncEvent.SockTxFailData.sd);
                    break;
                default: 
                    UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                                "(%d) \n\n",
                                pSock->socketAsyncEvent.SockTxFailData.sd, pSock->socketAsyncEvent.SockTxFailData.status);
                  break;
            }
            break;

        case SL_SOCKET_ASYNC_EVENT:

        	 switch(pSock->socketAsyncEvent.SockAsyncData.type)
        	 {
        	 case SSL_ACCEPT:/*accept failed due to ssl issue ( tcp pass)*/
        		 UART_PRINT("[SOCK ERROR] - close socket (%d) operation"
        				 	 "accept failed due to ssl issue\n\r",
        				 	 pSock->socketAsyncEvent.SockAsyncData.sd);
        	 case RX_FRAGMENTATION_TOO_BIG:
        		 UART_PRINT("[SOCK ERROR] -close scoket (%d) operation"
							 "connection less mode, rx packet fragmentation\n\r"
        				 	 "> 16K, packet is being released",
							 pSock->socketAsyncEvent.SockAsyncData.sd);
        	 case OTHER_SIDE_CLOSE_SSL_DATA_NOT_ENCRYPTED:
        		 UART_PRINT("[SOCK ERROR] -close socket (%d) operation"
        				 	 "remote side down from secure to unsecure\n\r",
        		 			pSock->socketAsyncEvent.SockAsyncData.sd);
        	 default:
        		 UART_PRINT("unknown sock async event: %d\n\r",
        				 	 pSock->socketAsyncEvent.SockAsyncData.type);
        	 }
        	break;
        default:
        	UART_PRINT("[SOCK EVENT] - Unexpected Event [%lx0x]\n\n",pSock->Event);
          break;
    }
}

//*****************************************************************************
//
//! This function gets triggered when HTTP Server receives Application
//! defined GET and POST HTTP Tokens.
//!
//! \param pHttpServerEvent Pointer indicating http server event
//! \param pHttpServerResponse Pointer indicating http server response
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent, 
                                SlHttpServerResponse_t *pSlHttpServerResponse)
{

}


//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************


//****************************************************************************
//
//!    \brief This function initializes the application variables
//!
//!    \param[in]  None
//!
//!    \return     0 on success, negative error-code on error
//
//****************************************************************************
void InitializeAppVariables()
{
    g_ulStatus = 0;
    g_ulStaIp = 0;
    g_ulGatewayIP = 0;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
}

//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
long ConfigureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {{0}};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {{0}};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode 
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event 
            // before doing anything 
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
                _SlNonOsMainLoopTask();
#else
                osi_Sleep(1);
#endif
            }
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            //ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        	UART_PRINT("Error [%d] at line [%d] in function [%s]\n\r", DEVICE_NOT_IN_STATION_MODE,__LINE__,__FUNCTION__);
        	return DEVICE_NOT_IN_STATION_MODE;
        }
    }
    
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);
    
    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %lu.%lu.%lu.%lu.31.%lu.%lu.%lu.%lu.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig 
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(lRetVal);

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore 
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#else
              osi_Sleep(1);
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    InitializeAppVariables();
    
    return lRetVal; // Success
}


//*****************************************************************************
//
//! Network_IF_InitDriver
//! The function initializes a CC3200 device and triggers it to start operation
//!  
//! \param  uiMode (device mode in which device will be configured)
//!  
//! \return 0 : sucess, -ve : failure
//
//*****************************************************************************
long
Network_IF_InitDriver(unsigned int uiMode)
{
    long lRetVal = -1;
    // Reset CC3200 Network State Machine
    InitializeAppVariables();

    //
    // Following function configure the device to default state by cleaning
    // the persistent settings stored in NVMEM (viz. connection profiles &
    // policies, power policy etc)
    //
    // Applications may choose to skip this step if the developer is sure
    // that the device is in its default state at start of application
    //
    // Note that all profiles and persistent settings that were done on the
    // device will be lost
    //
    lRetVal = ConfigureSimpleLinkToDefaultState();
    if(lRetVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
           UART_PRINT("Failed to configure the device in its default state \n\r");

        LOOP_FOREVER();
    }

    UART_PRINT("Device is configured in default state \n\r");

    //
    // Assumption is that the device is configured in station mode already
    // and it is in its default state
    //

    lRetVal = sl_Start(NULL,NULL,NULL);

    if (lRetVal < 0 || lRetVal != ROLE_STA)
    {
        UART_PRINT("Failed to start the device \n\r");
        LOOP_FOREVER();
    }

    UART_PRINT("Started SimpleLink Device: STA Mode\n\r");

    if(uiMode == ROLE_AP)
    {
        UART_PRINT("Switching to AP mode on application request\n\r");
        // Switch to AP role and restart
        lRetVal = sl_WlanSetMode(uiMode);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is up in AP Mode
        if (ROLE_AP == lRetVal)
        {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask();
#else
              osi_Sleep(1);
#endif
            }
        }
        else
        {
            // We don't want to proceed if the device is not coming up in AP-mode
            //ASSERT_ON_ERROR(DEVICE_NOT_IN_AP_MODE);
        	UART_PRINT("Error [%d] at line [%d] in function [%s]\n\r", DEVICE_NOT_IN_AP_MODE,__LINE__,__FUNCTION__);
        	return DEVICE_NOT_IN_AP_MODE;

        }

        UART_PRINT("Re-started SimpleLink Device: AP Mode\n\r");
    }
    else if(uiMode == ROLE_P2P)
    {
        UART_PRINT("Switching to P2P mode on application request\n\r");
        // Switch to AP role and restart
        lRetVal = sl_WlanSetMode(uiMode);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again
        if (ROLE_P2P != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in P2P-mode
            //ASSERT_ON_ERROR(DEVICE_NOT_IN_P2P_MODE);
        	UART_PRINT("Error [%d] at line [%d] in function [%s]\n\r", DEVICE_NOT_IN_P2P_MODE,__LINE__,__FUNCTION__);
        	return DEVICE_NOT_IN_P2P_MODE;

        }

        UART_PRINT("Re-started SimpleLink Device: P2P Mode\n\r");
    }
    else
    {
        // Device already started in STA-Mode
    }
    return 0;
}

//*****************************************************************************
//
//! Network_IF_DeInitDriver
//! The function de-initializes a CC3200 device
//!  
//! \param  None
//!  
//! \return On success, zero is returned. On error, other
//
//*****************************************************************************
long
Network_IF_DeInitDriver(void)
{
    long lRetVal = -1;
    UART_PRINT("SL Disconnect...\n\r");

    //
    // Disconnect from the AP
    //
    lRetVal = Network_IF_DisconnectFromAP();

    //
    // Stop the simplelink host
    //
    sl_Stop(SL_STOP_TIMEOUT);

    //
    // Reset the state to uninitialized
    //
    Network_IF_ResetMCUStateMachine();
    return lRetVal;
}


//*****************************************************************************
//
//! Network_IF_ConnectAP  Connect to an Access Point using the specified SSID
//!
//! \param[in]  pcSsid is a string of the AP's SSID
//! \param[in]  SecurityParams is Security parameter for AP
//!
//! \return On success, zero is returned. On error, -ve value is returned
//
//*****************************************************************************
long
Network_IF_ConnectAP(char *pcSsid, SlSecParams_t SecurityParams)
{
#ifndef NOTERM  
    char acCmdStore[128];
    unsigned short usConnTimeout;
    unsigned char ucRecvdAPDetails;
#endif
    long lRetVal;
    unsigned long ulIP = 0;
    unsigned long ulSubMask = 0;
    unsigned long ulDefGateway = 0;
    unsigned long ulDns = 0;

    //
    // Disconnect from the AP
    //
    Network_IF_DisconnectFromAP();
    
    //
    // This triggers the CC3200 to connect to specific AP
    //
    lRetVal = sl_WlanConnect((signed char *)pcSsid, strlen((const char *)pcSsid),
                        NULL, &SecurityParams, NULL);
    ASSERT_ON_ERROR(lRetVal);

    //
    // Wait for ~10 sec to check if connection to desire AP succeeds
    //
    while(g_usConnectIndex < 15)
    {
#ifndef SL_PLATFORM_MULTI_THREADED
        _SlNonOsMainLoopTask();
#else
              osi_Sleep(1);
#endif
        MAP_UtilsDelay(8000000);
        if(IS_CONNECTED(g_ulStatus) && IS_IP_ACQUIRED(g_ulStatus))
        {
            break;
        }
        g_usConnectIndex++;
    }

#ifndef NOTERM
    //
    // Check and loop until AP connection successful, else ask new AP SSID name
    //
    while(!(IS_CONNECTED(g_ulStatus)) || !(IS_IP_ACQUIRED(g_ulStatus)))
    {
        //
        // Disconnect the previous attempt
        //
        Network_IF_DisconnectFromAP();

        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);
        UART_PRINT("Device could not connect to %s\n\r",pcSsid);

        do
        {
            ucRecvdAPDetails = 0;

            UART_PRINT("\n\r\n\rPlease enter the AP(open) SSID name # ");

            //
            // Get the AP name to connect over the UART
            //
            lRetVal = GetCmd(acCmdStore, sizeof(acCmdStore));
            if(lRetVal > 0)
            {
                // remove start/end spaces if any
                lRetVal = TrimSpace(acCmdStore);

                //
                // Parse the AP name
                //
                strncpy(pcSsid, acCmdStore, lRetVal);
                if(pcSsid != NULL)
                {
                    ucRecvdAPDetails = 1;
                    pcSsid[lRetVal] = '\0';

                }
            }
        }while(ucRecvdAPDetails == 0);

        //
        // Reset Security Parameters to OPEN security type
        //
        SecurityParams.Key = (signed char *)"";
        SecurityParams.KeyLen = 0;
        SecurityParams.Type = SL_SEC_TYPE_OPEN;

        UART_PRINT("\n\rTrying to connect to AP: %s ...\n\r",pcSsid);

        //
        // Get the current timer tick and setup the timeout accordingly
        //
        usConnTimeout = g_usConnectIndex + 15;

        //
        // This triggers the CC3200 to connect to specific AP
        //
        lRetVal = sl_WlanConnect((signed char *)pcSsid,
                                  strlen((const char *)pcSsid), NULL,
                                  &SecurityParams, NULL);
        ASSERT_ON_ERROR(lRetVal);

        //
        // Wait ~10 sec to check if connection to specifed AP succeeds
        //
        while(!(IS_CONNECTED(g_ulStatus)) || !(IS_IP_ACQUIRED(g_ulStatus)))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
            _SlNonOsMainLoopTask();
#else
              osi_Sleep(1);
#endif
            MAP_UtilsDelay(8000000);
            if(g_usConnectIndex >= usConnTimeout)
            {
                break;
            }
            g_usConnectIndex++;
        }

    }
#endif
    //
    // Put message on UART
    //
    UART_PRINT("\n\rDevice has connected to %s\n\r",pcSsid);

    //
    // Get IP address
    //
    lRetVal = Network_IF_IpConfigGet(&ulIP,&ulSubMask,&ulDefGateway,&ulDns);
    ASSERT_ON_ERROR(lRetVal);

    //
    // Send the information
    //
    UART_PRINT("Device IP Address is %lu.%lu.%lu.%lu \n\r\n\r",
            SL_IPV4_BYTE(ulIP, 3),SL_IPV4_BYTE(ulIP, 2),
            SL_IPV4_BYTE(ulIP, 1),SL_IPV4_BYTE(ulIP, 0));
    return 0;
}

//*****************************************************************************
//
//! Disconnect  Disconnects from an Access Point
//!
//! \param  none
//!
//! \return 0 disconnected done, other already disconnected
//
//*****************************************************************************
long
Network_IF_DisconnectFromAP()
{
    long lRetVal = 0;

    if (IS_CONNECTED(g_ulStatus))
    {
        lRetVal = sl_WlanDisconnect();
        if(0 == lRetVal)
        {
            // Wait
            while(IS_CONNECTED(g_ulStatus))
            {
    #ifndef SL_PLATFORM_MULTI_THREADED
                  _SlNonOsMainLoopTask();
    #else
                  osi_Sleep(1);
    #endif
            }
            return lRetVal;
        }
        else
        {
            return lRetVal;
        }
    }
    else
    {
        return lRetVal;
    }

}

//*****************************************************************************
//
//! Network_IF_IpConfigGet  Get the IP Address of the device.
//!
//! \param  pulIP IP Address of Device
//! \param  pulSubnetMask Subnetmask of Device
//! \param  pulDefaultGateway Default Gateway value
//! \param  pulDNSServer DNS Server
//!
//! \return On success, zero is returned. On error, -1 is returned
//
//*****************************************************************************
long
Network_IF_IpConfigGet(unsigned long *pulIP, unsigned long *pulSubnetMask,
                unsigned long *pulDefaultGateway, unsigned long *pulDNSServer)
{
    unsigned char isDhcp;
    unsigned char len = sizeof(SlNetCfgIpV4Args_t);
    long lRetVal = -1;
    SlNetCfgIpV4Args_t ipV4 = {0};

    lRetVal = sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO,&isDhcp,&len,
                                  (unsigned char *)&ipV4);
    ASSERT_ON_ERROR(lRetVal);

    *pulIP=ipV4.ipV4;
    *pulSubnetMask=ipV4.ipV4Mask;
    *pulDefaultGateway=ipV4.ipV4Gateway;
    *pulDefaultGateway=ipV4.ipV4DnsServer;

    return lRetVal;
}

//*****************************************************************************
//
//! Network_IF_GetHostIP
//!
//! \brief  This function obtains the server IP address using a DNS lookup
//!
//! \param[in]  pcHostName        The server hostname
//! \param[out] pDestinationIP    This parameter is filled with host IP address.
//!
//! \return On success, +ve value is returned. On error, -ve value is returned
//!
//
//*****************************************************************************
long Network_IF_GetHostIP( char* pcHostName,unsigned long * pDestinationIP )
{
    long lStatus = 0;

    lStatus = sl_NetAppDnsGetHostByName((signed char *) pcHostName,
                                            strlen(pcHostName),
                                            pDestinationIP, SL_AF_INET);
    ASSERT_ON_ERROR(lStatus);

    UART_PRINT("Get Host IP succeeded.\n\rHost: %s IP: %lu.%lu.%lu.%lu \n\r\n\r",
                    pcHostName, SL_IPV4_BYTE(*pDestinationIP,3),
                    SL_IPV4_BYTE(*pDestinationIP,2),
                    SL_IPV4_BYTE(*pDestinationIP,1),
                    SL_IPV4_BYTE(*pDestinationIP,0));
    return lStatus;

}

//*****************************************************************************
//
//!  \brief  Reset state from the state machine
//!
//!  \param  None
//!
//!  \return none
//!
//*****************************************************************************
void 
Network_IF_ResetMCUStateMachine()
{
    g_ulStatus = 0;
}

//*****************************************************************************
//
//!  \brief  Return the current state bits
//!
//!  \param  None
//!
//!  \return none
//!
//
//*****************************************************************************
unsigned long
Network_IF_CurrentMCUState()
{
    return g_ulStatus;
}
//*****************************************************************************
//
//!  \brief  sets a state from the state machine
//!
//!  \param  cStat Status of State Machine defined in e_StatusBits
//!
//!  \return none
//!
//*****************************************************************************
void 
Network_IF_SetMCUMachineState(char cStat)
{
    SET_STATUS_BIT(g_ulStatus, cStat);
}

//*****************************************************************************
//
//!  \brief  Unsets a state from the state machine
//!
//!  \param  cStat Status of State Machine defined in e_StatusBits
//!
//!  \return none
//!
//*****************************************************************************
void 
Network_IF_UnsetMCUMachineState(char cStat)
{
    CLR_STATUS_BIT(g_ulStatus, cStat);
}

#if 0
//*****************************************************************************
//
//! itoa
//!
//!    @brief  Convert integer to ASCII in decimal base
//!
//!     @param  cNum is input integer number to convert
//!     @param  cString is output string
//!
//!     @return number of ASCII parameters
//!
//!
//
//*****************************************************************************
unsigned short itoa(short cNum, char *cString)
{
    char* ptr;
    short uTemp = cNum;
    unsigned short length;

    // value 0 is a special case
    if (cNum == 0)
    {
        length = 1;
        *cString = '0';

        return length;
    }

    // Find out the length of the number, in decimal base
    length = 0;
    while (uTemp > 0)
    {
        uTemp /= 10;
        length++;
    }

    // Do the actual formatting, right to left
    uTemp = cNum;
    ptr = cString + length;
    while (uTemp > 0)
    {
        --ptr;
        *ptr = pcDigits[uTemp % 10];
        uTemp /= 10;
    }

    return length;
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************


