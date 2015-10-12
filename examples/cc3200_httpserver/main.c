//*****************************************************************************
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

//*****************************************************************************
//
// Application Name     -   HTTP Server
// Application Overview -   This is a sample application demonstrating
//                          interaction between HTTP Client(Browser) and
//                          SimpleLink Device.The SimpleLink device runs an
//                          HTTP Server and user can interact using web browser.
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_HTTP_Server
// or
// doc\examples\CC32xx_HTTP_Server.pdf
//
//*****************************************************************************

//****************************************************************************
//
//! \addtogroup httpserver
//! @{
//
//****************************************************************************

// Standard includes
#include <string.h>
#include <stdlib.h>

// Simplelink includes
#include "simplelink.h"
#include "netcfg.h"

//driverlib includes
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "interrupt.h"
#include "utils.h"
#include "pin.h"
#include "uart.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"

//Free_rtos/ti-rtos includes
#include "osi.h"

// common interface includes
#ifdef ORIGINAL_EXAMPLE
#include "gpio_if.h"
#include "uart_if.h"
#endif

#include "common.h"

#include "smartconfig.h"
#ifdef ORIGINAL_EXAMPLE
#include "pinmux.h"
#else
#include "periph/gpio.h"
#define WIFI_MODE_PIN 2 // PIN_58
#endif

#define APPLICATION_NAME        "HTTP Server"
#define APPLICATION_VERSION     "1.1.1"
#define AP_SSID_LEN_MAX         (33)
#define ROLE_INVALID            (-5)

#define LED_STRING              "LED"
#define LED1_STRING             "LED1_"
#define LED2_STRING             ",LED2_"
#define LED_ON_STRING           "ON"
#define LED_OFF_STRING          "OFF"

#define OOB_TASK_PRIORITY               (1)
#define OSI_STACK_SIZE                  (2048)
#define SH_GPIO_3                       (3)  /* P58 - Device Mode */
#define ROLE_INVALID                    (-5)
#define AUTO_CONNECTION_TIMEOUT_COUNT   (50)   /* 5 Sec */

// Application specific status/error codes
typedef enum {
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    LAN_CONNECTION_FAILED = -0x7D0,
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = INTERNET_CONNECTION_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
} e_AppStatusCodes;

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
volatile unsigned long g_ulStatus = 0; //SimpleLink Status
unsigned long g_ulPingPacketsRecv = 0; //Number of Ping Packets received
unsigned long g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char g_ucConnectionSSID[SSID_LEN_MAX + 1]; //Connection SSID
unsigned char g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
unsigned char POST_token[] = "__SL_P_ULD";
unsigned char GET_token[] = "__SL_G_ULD";
int g_iSimplelinkRole = ROLE_INVALID;
signed int g_uiIpAddress = 0;
unsigned char g_ucSSID[AP_SSID_LEN_MAX];

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
// Variable related to Connection status
//*****************************************************************************
volatile unsigned short g_usMCNetworkUstate = 0;

int g_uiSimplelinkRole = ROLE_INVALID;
unsigned int g_uiDeviceModeConfig = ROLE_STA; //default is STA mode
volatile unsigned char g_ucConnectTimeout = 0;


//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************

//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent) {
    if (!pWlanEvent) {
        return;
    }

    switch (pWlanEvent->Event) {
    case SL_WLAN_CONNECT_EVENT: {
        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

        //
        // Information about the connected AP (like name, MAC etc) will be
        // available in 'slWlanConnectAsyncResponse_t'-Applications
        // can use it if required
        //
        //  slWlanConnectAsyncResponse_t *pEventData = NULL;
        // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
        //

        // Copy new connection SSID and BSSID to global parameters
        memcpy(g_ucConnectionSSID,
                pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_name,
                pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
        memcpy(g_ucConnectionBSSID,
                pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                SL_BSSID_LENGTH);

        UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s ,"
                "BSSID: %x:%x:%x:%x:%x:%x\n\r", g_ucConnectionSSID,
                g_ucConnectionBSSID[0], g_ucConnectionBSSID[1],
                g_ucConnectionBSSID[2], g_ucConnectionBSSID[3],
                g_ucConnectionBSSID[4], g_ucConnectionBSSID[5]);
    }
        break;

    case SL_WLAN_DISCONNECT_EVENT: {
        slWlanConnectAsyncResponse_t* pEventData = NULL;

        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

        pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

        // If the user has initiated 'Disconnect' request,
        //'reason_code' is SL_USER_INITIATED_DISCONNECTION
        //cppcheck-suppress duplicateBranch
        if (SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code) {
            UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s,"
                    "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                    g_ucConnectionSSID, g_ucConnectionBSSID[0],
                    g_ucConnectionBSSID[1], g_ucConnectionBSSID[2],
                    g_ucConnectionBSSID[3], g_ucConnectionBSSID[4],
                    g_ucConnectionBSSID[5]);
        } else {
            UART_PRINT("[WLAN ERROR]Device disconnected from the AP: %s,"
                    "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                    g_ucConnectionSSID, g_ucConnectionBSSID[0],
                    g_ucConnectionBSSID[1], g_ucConnectionBSSID[2],
                    g_ucConnectionBSSID[3], g_ucConnectionBSSID[4],
                    g_ucConnectionBSSID[5]);
        }
        memset(g_ucConnectionSSID, 0, sizeof(g_ucConnectionSSID));
        memset(g_ucConnectionBSSID, 0, sizeof(g_ucConnectionBSSID));
    }
        break;

    case SL_WLAN_STA_CONNECTED_EVENT: {
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

    case SL_WLAN_STA_DISCONNECTED_EVENT: {
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

    case SL_WLAN_SMART_CONFIG_COMPLETE_EVENT: {
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

    case SL_WLAN_SMART_CONFIG_STOP_EVENT: {
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

    case SL_WLAN_P2P_DEV_FOUND_EVENT: {
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

    case SL_WLAN_P2P_NEG_REQ_RECEIVED_EVENT: {
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

    case SL_WLAN_CONNECTION_FAILED_EVENT: {
        // If device gets any connection failed event
        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION_FAILED);
    }
        break;

    default: {
        UART_PRINT("[WLAN EVENT] Unexpected event [0x%lx]\n\r",
                pWlanEvent->Event);
    }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent) {
    if (!pNetAppEvent) {
        return;
    }

    switch (pNetAppEvent->Event) {
    case SL_NETAPP_IPV4_IPACQUIRED_EVENT: {
        SlIpV4AcquiredAsync_t *pEventData = NULL;

        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

        //Ip Acquired Event Data
        pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
        g_uiIpAddress = pEventData->ip;

        //Gateway IP address
        g_ulGatewayIP = pEventData->gateway;

        /*UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
         "Gateway=%d.%d.%d.%d\n\r",
         SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
         SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
         SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
         SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
         SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
         SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
         SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
         SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));
         */
    }
        break;

    case SL_NETAPP_IP_LEASED_EVENT: {
        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

        //
        // Information about the IP-Leased details(like IP-Leased,lease-time,
        // mac etc) will be available in 'SlIpLeasedAsync_t' - Applications
        // can use it if required
        //
        // SlIpLeasedAsync_t *pEventData = NULL;
        // pEventData = &pNetAppEvent->EventData.ipLeased;
        //

    }
        break;

    case SL_NETAPP_IP_RELEASED_EVENT: {
        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

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

    default: {
        UART_PRINT("[NETAPP EVENT] Unexpected event [0x%lx] \n\r",
                pNetAppEvent->Event);
    }
        break;
    }
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
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock) {
    //
    // This application doesn't work w/ socket - Events are not expected
    //

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
        SlHttpServerResponse_t *pSlHttpServerResponse) {
    unsigned char strLenVal = 0;

    if (!pSlHttpServerEvent || !pSlHttpServerResponse) {
        return;
    }

    switch (pSlHttpServerEvent->Event) {
    case SL_NETAPP_HTTPGETTOKENVALUE_EVENT: {
        unsigned char *ptr;

        ptr = pSlHttpServerResponse->ResponseData.token_value.data;
        pSlHttpServerResponse->ResponseData.token_value.len = 0;
        if (memcmp(pSlHttpServerEvent->EventData.httpTokenName.data, GET_token,
                strlen((const char *) GET_token)) == 0) {
            unsigned char status;
            status = GPIO_IF_LedStatus(MCU_RED_LED_GPIO);
            strLenVal = strlen(LED1_STRING);
            memcpy(ptr, LED1_STRING, strLenVal);
            ptr += strLenVal;
            pSlHttpServerResponse->ResponseData.token_value.len += strLenVal;
            if (status & 0x01) {
                strLenVal = strlen(LED_ON_STRING);
                memcpy(ptr, LED_ON_STRING, strLenVal);
                ptr += strLenVal;
                pSlHttpServerResponse->ResponseData.token_value.len +=
                        strLenVal;
            } else {
                strLenVal = strlen(LED_OFF_STRING);
                memcpy(ptr, LED_OFF_STRING, strLenVal);
                ptr += strLenVal;
                pSlHttpServerResponse->ResponseData.token_value.len +=
                        strLenVal;
            }
            status = GPIO_IF_LedStatus(MCU_GREEN_LED_GPIO);
            strLenVal = strlen(LED2_STRING);
            memcpy(ptr, LED2_STRING, strLenVal);
            ptr += strLenVal;
            pSlHttpServerResponse->ResponseData.token_value.len += strLenVal;
            if (status & 0x01) {
                strLenVal = strlen(LED_ON_STRING);
                memcpy(ptr, LED_ON_STRING, strLenVal);
                ptr += strLenVal;
                pSlHttpServerResponse->ResponseData.token_value.len +=
                        strLenVal;
            } else {
                strLenVal = strlen(LED_OFF_STRING);
                memcpy(ptr, LED_OFF_STRING, strLenVal);
                ptr += strLenVal;
                pSlHttpServerResponse->ResponseData.token_value.len +=
                        strLenVal;
            }
            *ptr = '\0';
        }

    }
        break;

    case SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT: {
        unsigned char *ptr =
                pSlHttpServerEvent->EventData.httpPostData.token_name.data;

        if (memcmp(ptr, POST_token, strlen((const char *) POST_token)) == 0) {
            unsigned char led;

            ptr = pSlHttpServerEvent->EventData.httpPostData.token_value.data;
            strLenVal = strlen(LED_STRING);
            if (memcmp(ptr, LED_STRING, strLenVal) != 0)
                break;
            ptr += strLenVal;
            led = *ptr;
            strLenVal = strlen(LED_ON_STRING);
            ptr += strLenVal;
            if (led == '1') {
                if (memcmp(ptr, LED_ON_STRING, strLenVal) == 0) {
                    GPIO_IF_LedOn(MCU_RED_LED_GPIO);
                } else {
                    GPIO_IF_LedOff(MCU_RED_LED_GPIO);
                }
            } else if (led == '2') {
                if (memcmp(ptr, LED_ON_STRING, strLenVal) == 0) {
                    GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
                } else {
                    GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
                }
            }

        }
    }
        break;
    default:
        break;
    }
}

//*****************************************************************************
//
//! \brief This function initializes the application variables
//!
//! \param    None
//!
//! \return None
//!
//*****************************************************************************
static void InitializeAppVariables(void) {
    g_ulStatus = 0;
    g_uiIpAddress = 0;
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
static long ConfigureSimpleLinkToDefaultState(void) {
    SlVersionFull ver;
    _WlanRxFilterOperationCommandBuff_t RxFilterIdMask;

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode
    if (ROLE_STA != lMode) {
        if (ROLE_AP == lMode) {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while (!IS_IP_ACQUIRED(g_ulStatus)) {
#ifndef SL_PLATFORM_MULTI_THREADED
                _SlNonOsMainLoopTask();
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
        if (ROLE_STA != lRetVal) {
            // We don't want to proceed if the device is not in STA-mode
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }

    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt,
            &ucConfigLen, (unsigned char *) (&ver));
    ASSERT_ON_ERROR(lRetVal);

    UART_PRINT("Host Driver Version: %s\n\r", SL_DRIVER_VERSION);
    UART_PRINT(
            "Build Version %lu.%lu.%lu.%lu.31.%lu.%lu.%lu.%lu.%d.%d.%d.%d\n\r",
            ver.NwpVersion[0], ver.NwpVersion[1], ver.NwpVersion[2],
            ver.NwpVersion[3], ver.ChipFwAndPhyVersion.FwVersion[0],
            ver.ChipFwAndPhyVersion.FwVersion[1],
            ver.ChipFwAndPhyVersion.FwVersion[2],
            ver.ChipFwAndPhyVersion.FwVersion[3],
            ver.ChipFwAndPhyVersion.PhyVersion[0],
            ver.ChipFwAndPhyVersion.PhyVersion[1],
            ver.ChipFwAndPhyVersion.PhyVersion[2],
            ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,
            SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    //lRetVal = sl_WlanProfileDel(0xFF);
    //ASSERT_ON_ERROR(lRetVal);

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if (0 == lRetVal) {
        // Wait
        while (IS_CONNECTED(g_ulStatus)) {
#ifndef SL_PLATFORM_MULTI_THREADED
            _SlNonOsMainLoopTask();
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN, ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
    WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *) &ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *) &RxFilterIdMask,
            sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    InitializeAppVariables();

    return lRetVal; // Success
}

//****************************************************************************
//
//!    \brief Connects to the Network in AP or STA Mode - If ForceAP Jumper is
//!                                             Placed, Force it to AP mode
//!
//! \return                        0 on success else error code
//
//****************************************************************************
long ConnectToNetwork(void) {
    unsigned short len, config_opt;
    long lRetVal = -1;

    // staring simplelink
    g_uiSimplelinkRole = sl_Start(NULL, NULL, NULL);

    // Device is not in STA mode and Force AP Jumper is not Connected
    //- Switch to STA mode
    if (g_uiSimplelinkRole != ROLE_STA && g_uiDeviceModeConfig == ROLE_STA) {
        //Switch to STA Mode
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(SL_STOP_TIMEOUT);

        g_usMCNetworkUstate = 0;
        g_uiSimplelinkRole = sl_Start(NULL, NULL, NULL);
    }

    //Device is not in AP mode and Force AP Jumper is Connected -
    //Switch to AP mode
    if (g_uiSimplelinkRole != ROLE_AP && g_uiDeviceModeConfig == ROLE_AP) {
        //Switch to AP Mode
        lRetVal = sl_WlanSetMode(ROLE_AP);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(SL_STOP_TIMEOUT);

        g_usMCNetworkUstate = 0;
        g_uiSimplelinkRole = sl_Start(NULL, NULL, NULL);
    }

    //No Mode Change Required
    if (g_uiSimplelinkRole == ROLE_AP) {
        char ucAPSSID[AP_SSID_LEN_MAX];
        //waiting for the AP to acquire IP address from Internal DHCP Server
        while (!IS_IP_ACQUIRED(g_ulStatus)) {

        }

        //Stop Internal HTTP Server
        lRetVal = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
        ASSERT_ON_ERROR(lRetVal);

        //Start Internal HTTP Server
        lRetVal = sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);
        ASSERT_ON_ERROR(lRetVal);

        char iCount = 0;
        //Read the AP SSID
        memset(ucAPSSID, '\0', AP_SSID_LEN_MAX);
        len = AP_SSID_LEN_MAX;
        config_opt = WLAN_AP_OPT_SSID;
        lRetVal = sl_WlanGet(SL_WLAN_CFG_AP_ID, &config_opt, &len,
                (unsigned char*) ucAPSSID);
        ASSERT_ON_ERROR(lRetVal);

        Report("\n\rDevice is in AP Mode, Please Connect to AP [%s] and"
                "type [mysimplelink.net] in the browser \n\r", ucAPSSID);

        //Blink LED 3 times to Indicate AP Mode
        for (iCount = 0; iCount < 3; iCount++) {
            //Turn RED LED On
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
            osi_Sleep(400);

            //Turn RED LED Off
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
            osi_Sleep(400);
        }

    } else {
        //Stop Internal HTTP Server
        lRetVal = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
        ASSERT_ON_ERROR(lRetVal);

        //Start Internal HTTP Server
        lRetVal = sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);
        ASSERT_ON_ERROR(lRetVal);

        //waiting for the device to Auto Connect
        while ((!IS_IP_ACQUIRED(g_ulStatus))
                && g_ucConnectTimeout < AUTO_CONNECTION_TIMEOUT_COUNT) {
            //Turn RED LED On
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
            osi_Sleep(50);

            //Turn RED LED Off
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
            osi_Sleep(50);

            g_ucConnectTimeout++;
        }
        //Couldn't connect Using Auto Profile
        if (g_ucConnectTimeout == AUTO_CONNECTION_TIMEOUT_COUNT) {
            //Blink Red LED to Indicate Connection Error
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);

            CLR_STATUS_BIT_ALL(g_ulStatus);

            Report("Use Smart Config Application to configure the device.\n\r");
            //Connect Using Smart Config
            lRetVal = SmartConfigConnect();
            ASSERT_ON_ERROR(lRetVal);

            //Waiting for the device to Auto Connect
            while (!IS_IP_ACQUIRED(g_ulStatus)) {
                MAP_UtilsDelay(500);
            }

        }
        //Turn RED LED Off
        GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        UART_PRINT("\n\rDevice is in STA Mode, Connect to the AP[%s] and type"
                "IP address [%d.%d.%d.%d] in the browser \n\r",
                g_ucConnectionSSID, SL_IPV4_BYTE(g_uiIpAddress, 3),
                SL_IPV4_BYTE(g_uiIpAddress, 2), SL_IPV4_BYTE(g_uiIpAddress, 1),
                SL_IPV4_BYTE(g_uiIpAddress, 0));

    }
    return SUCCESS;
}

//****************************************************************************
//
//!    \brief Read Force AP GPIO and Configure Mode - 1(Access Point Mode)
//!                                                  - 0 (Station Mode)
//!
//! \return                        None
//
//****************************************************************************
static void ReadDeviceConfiguration(void) {
#ifdef ORIGINAL_EXAMPLE
    unsigned int uiGPIOPort;
    unsigned char pucGPIOPin;
#endif
    unsigned char ucPinValue;

    //Read GPIO
#ifdef ORIGINAL_EXAMPLE
    GPIO_IF_GetPortNPin(SH_GPIO_3,&uiGPIOPort,&pucGPIOPin);
    ucPinValue = GPIO_IF_Get(SH_GPIO_3,uiGPIOPort,pucGPIOPin);
#else
    ucPinValue = gpio_read(WIFI_MODE_PIN);
#endif

    //If Connected to VCC, Mode is AP
    if (ucPinValue == 1) {
        //AP Mode
        g_uiDeviceModeConfig = ROLE_AP;
    } else {
        //STA Mode
        g_uiDeviceModeConfig = ROLE_STA;
    }

}

//****************************************************************************
//
//!  \brief                     Handles HTTP Server Task
//!
//! \param[in]                  pvParameters is the data passed to the Task
//!
//! \return                        None
//
//****************************************************************************
static void HTTPServerTask(void *pvParameters) {
    long lRetVal;
    InitializeAppVariables();

    //
    // Following function configure the device to default state by cleaning
    // the persistent settings stored in NVMEM (viz. connection profiles &
    // policies, power policy etc)
    //
    // Applications may choose to skip this step if the developer is sure
    // that the device is in its default state at start of applicaton
    //
    // Note that all profiles and persistent settings that were done on the
    // device will be lost
    //
    lRetVal = ConfigureSimpleLinkToDefaultState();
    if (lRetVal < 0) {
        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
            UART_PRINT(
                    "Failed to configure the device in its default state\n\r");

        LOOP_FOREVER()
        ;
    }

    UART_PRINT("Device is configured in default state \n\r");

    memset(g_ucSSID, '\0', AP_SSID_LEN_MAX);

    //Read Device Mode Configuration
    ReadDeviceConfiguration();

    //Connect to Network
    ConnectToNetwork();

    //Stop Internal HTTP Server
    lRetVal = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
    if (lRetVal < 0) {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER()
        ;
    }

    //Start Internal HTTP Server
    lRetVal = sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);
    if (lRetVal < 0) {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER()
        ;
    }

    //Handle Async Events
    while (1) {

    }
}

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void DisplayBanner(char * AppName) {

    Report("\n\n\n\r");
    Report("\t\t *************************************************\n\r");
    Report("\t\t      CC3200 %s Application       \n\r", AppName);
    Report("\t\t *************************************************\n\r");
    Report("\n\n\n\r");
}

#ifdef ORIGINAL_EXAMPLE

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
    /* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}
#endif

//****************************************************************************
//                            MAIN FUNCTION
//****************************************************************************
int main(void) {
    long lRetVal;

#ifdef ORIGINAL_EXAMPLE
    //Board Initialization
    BoardInit();

    //Pin Configuration
    PinMuxConfig();

    //Change Pin 58 Configuration from Default to Pull Down
    MAP_PinConfigSet(PIN_58,PIN_STRENGTH_2MA|PIN_STRENGTH_4MA,PIN_TYPE_STD_PD);

    //
    // Initialize GREEN and ORANGE LED
    //
    GPIO_IF_LedConfigure(LED1|LED2|LED3);
    //Turn Off the LEDs
    GPIO_IF_LedOff(MCU_ALL_LED_IND);

    //UART Initialization
    MAP_PRCMPeripheralReset(PRCM_UARTA0);

    MAP_UARTConfigSetExpClk(CONSOLE,MAP_PRCMPeripheralClockGet(CONSOLE_PERIPH),
            UART_BAUD_RATE,(UART_CONFIG_WLEN_8 |
                    UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
#else

    //Display Application Banner on UART Terminal
    DisplayBanner(APPLICATION_NAME);

    gpio_init(WIFI_MODE_PIN, GPIO_DIR_IN, GPIO_PULLDOWN);
#endif

    //
    // Simplelinkspawntask
    //
    lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if (lRetVal < 0) {
        UART_PRINT("Unable to start simpelink spawn task\n\r");
        LOOP_FOREVER()
        ;
    }
    //
    // Create HTTP Server Task
    //
    //lRetVal = osi_TaskCreate(HTTPServerTask, (signed char*)"HTTPServerTask",
    //                     OSI_STACK_SIZE, NULL, OOB_TASK_PRIORITY, NULL );
    lRetVal = osi_TaskCreate(HTTPServerTask, (signed char*) "HTTPServerTask",
    OSI_STACK_SIZE, NULL, 12, NULL);
    if (lRetVal < 0) {
        UART_PRINT("Unable to create task\n\r");
        LOOP_FOREVER()
        ;
    }

#ifdef ORIGINAL_EXAMPLE
    //
    // Start OS Scheduler
    //
    osi_start();
    while (1)
    {

    }
#endif

}
