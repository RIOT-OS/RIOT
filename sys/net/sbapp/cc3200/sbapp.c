/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author      Attilio Dona'
 *
 */
#include "simplelink.h"

#include "kernel.h"
#include "xtimer.h"
#include "net/sbapp.h"
#include "net/gnrc.h"

#include "log.h"
#include "throw.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"


/**
 * @brief send a packet
 *
 * @internal
 */
static void _send(gnrc_pktsnip_t *pkt);

static void *_event_loop(void *arg);

typedef struct sbapp_t {
    kernel_pid_t main_pid; /**< the main task (send pkts and control commands */
    kernel_pid_t recv_pid; /**< the receive data task */
} sbapp_t;

sbapp_t sbapp = {
        .main_pid = KERNEL_PID_UNDEF,
        .recv_pid = KERNEL_PID_UNDEF,
};

/**
 * @brief   Allocate memory for the NBAPP thread's stack
 */
#if ENABLE_DEBUG
static char _stack[GNRC_NBAPP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_NBAPP_STACK_SIZE];
#endif

/**
 * wifi module return codes
 */
typedef enum {
    // Choosing this number to avoid overlap w/ host-driver's error codes
    LAN_CONNECTION_FAILED = -0x7D0,
    CLIENT_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = CLIENT_CONNECTION_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
} app_rc;


// todo: to be removed
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent,
        SlHttpServerResponse_t *pSlHttpServerResponse) {
}


/**
 * @brief The Function Handles WLAN Events
 *
 * @param[in]  pWlanEvent - Pointer to WLAN Event Info
 *
 * @return None
 */
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent) {
    msg_t msg;

    if (!pWlanEvent) {
        return;
    }

    switch (pWlanEvent->Event) {
        case SL_WLAN_CONNECT_EVENT: {
            SET_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'-Applications
            // can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //

            // Copy new connection SSID and BSSID to global parameters
            memcpy(nwp.ssid,
                    pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_name,
                    pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(nwp.bssid,
                    pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                    SL_BSSID_LENGTH);

            LOG_INFO("[WLAN EVENT] STA Connected to the AP: %s ,"
                    "BSSID: %x:%x:%x:%x:%x:%x\n\r", nwp.ssid,
                    nwp.bssid[0], nwp.bssid[1],
                    nwp.bssid[2], nwp.bssid[3],
                    nwp.bssid[4], nwp.bssid[5]);
            }
            msg.type = WLAN_CONNECTED;

            msg_send(&msg, sbapp.main_pid);

            //cpuid_get(NULL);
            break;

        case SL_WLAN_DISCONNECT_EVENT: {
            slWlanConnectAsyncResponse_t* pEventData = NULL;

            CLR_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(nwp.status, STATUS_BIT_IP_ACQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request,
            //'reason_code' is SL_USER_INITIATED_DISCONNECTION
            //cppcheck-suppress duplicateBranch
            if (SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code) {
                LOG_INFO(
                        "[WLAN EVENT]Device disconnected from the AP: %s,"
                                "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                        nwp.ssid, nwp.bssid[0],
                        nwp.bssid[1], nwp.bssid[2],
                        nwp.bssid[3], nwp.bssid[4],
                        nwp.bssid[5]);
            } else {
                LOG_INFO("[WLAN ERROR]Device disconnected from the AP: %s,"
                        "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                        nwp.ssid, nwp.bssid[0],
                        nwp.bssid[1], nwp.bssid[2],
                        nwp.bssid[3], nwp.bssid[4],
                        nwp.bssid[5]);
            }
            memset(nwp.ssid, 0, sizeof(nwp.ssid));
            memset(nwp.bssid, 0, sizeof(nwp.bssid));
        }
            break;

        case SL_WLAN_STA_CONNECTED_EVENT: {
            // when device is in AP mode and any client connects to device cc3xxx
            SET_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION_FAILED);

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
            CLR_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(nwp.status, STATUS_BIT_IP_LEASED);

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
            SET_STATUS_BIT(nwp.status, STATUS_BIT_SMARTCONFIG_START);

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
            CLR_STATUS_BIT(nwp.status, STATUS_BIT_SMARTCONFIG_START);

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
            SET_STATUS_BIT(nwp.status, STATUS_BIT_P2P_DEV_FOUND);

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
            SET_STATUS_BIT(nwp.status, STATUS_BIT_P2P_REQ_RECEIVED);

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
            SET_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION_FAILED);
        }
            break;

        default: {
            LOG_WARNING("[WLAN EVENT] Unexpected event [0x%lx]\n\r",
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
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *ev) {
    msg_t msg;
    SlIpV4AcquiredAsync_t *pEventData = NULL;

    if (!ev) {
        return;
    }

    switch (ev->Event) {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:

            SET_STATUS_BIT(nwp.status, STATUS_BIT_IP_ACQUIRED);

            //Ip Acquired Event Data
            pEventData = &ev->EventData.ipAcquiredV4;
            nwp.ip = pEventData->ip;

            //Gateway IP address
            nwp.gw_ip = pEventData->gateway;

            LOG_INFO("[NETAPP EVENT] IP Acquired: IP=%lu.%lu.%lu.%lu , "
                    "Gateway=%lu.%lu.%lu.%lu\n\r",
                    SL_IPV4_BYTE(ev->EventData.ipAcquiredV4.ip, 3),
                    SL_IPV4_BYTE(ev->EventData.ipAcquiredV4.ip, 2),
                    SL_IPV4_BYTE(ev->EventData.ipAcquiredV4.ip, 1),
                    SL_IPV4_BYTE(ev->EventData.ipAcquiredV4.ip, 0),
                    SL_IPV4_BYTE(ev->EventData.ipAcquiredV4.gateway, 3),
                    SL_IPV4_BYTE(ev->EventData.ipAcquiredV4.gateway, 2),
                    SL_IPV4_BYTE(ev->EventData.ipAcquiredV4.gateway, 1),
                    SL_IPV4_BYTE(ev->EventData.ipAcquiredV4.gateway, 0));

            msg.type = IP_ACQUIRED;
            msg_send(&msg, sbapp.main_pid);

            break;

        case SL_NETAPP_IP_LEASED_EVENT: {
            SET_STATUS_BIT(nwp.status, STATUS_BIT_IP_LEASED);

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
            CLR_STATUS_BIT(nwp.status, STATUS_BIT_IP_LEASED);

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
            LOG_WARNING("[NETAPP EVENT] Unexpected event [0x%lx] \n\r",
                    ev->Event);
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


#if 1
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
long ConfigureSimpleLinkToDefaultState(void) {
    SlVersionFull ver;
    _WlanRxFilterOperationCommandBuff_t RxFilterIdMask;

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode;

    lMode = sl_Start(0, 0, 0);
    THROW2(GEN_ERROR, lMode);

    // If the device is not in station-mode, try configuring it in station-mode
    if (ROLE_STA != lMode) {
        if (ROLE_AP == lMode) {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while (!IS_IP_ACQUIRED(nwp.status)) {
#ifndef SL_PLATFORM_MULTI_THREADED
                _SlNonOsMainLoopTask();
#endif
            }
        }

        // Switch to STA role and restart
        lRetVal = sl_WlanSetMode(ROLE_STA);
        //ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        //ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        //ASSERT_ON_ERROR(lRetVal);

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
    THROW2(GEN_ERROR, lRetVal);

    LOG_INFO("Host Driver Version: %s\n\r", SL_DRIVER_VERSION);
    LOG_INFO(
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
    lRetVal = sl_WlanPolicySet(
            SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1),
            NULL, 0);
    THROW2(GEN_ERROR, lRetVal);

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
        while (IS_CONNECTED(nwp.status)) {
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &ucVal);
    THROW2(GEN_ERROR, lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(
            SL_POLICY_SCAN, ucConfigOpt, NULL, 0);
    THROW2(GEN_ERROR, lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(
            SL_WLAN_CFG_GENERAL_PARAM_ID,
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *) &ucPower);
    THROW2(GEN_ERROR, lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(
            SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);
    THROW2(GEN_ERROR, lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    THROW2(GEN_ERROR, lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(
            SL_REMOVE_RX_FILTER, (_u8 *) &RxFilterIdMask,
            sizeof(_WlanRxFilterOperationCommandBuff_t));
    THROW2(GEN_ERROR, lRetVal);

    lRetVal = sl_Stop(
            SL_STOP_TIMEOUT);
    THROW2(GEN_ERROR, lRetVal);

    //InitializeAppVariables();

    return lRetVal;// Success
}
#endif




/**
 * @brief Connects to the Network in STA Mode
 *
 * @return ROLE_STA (0) on success or a negative value on error
 */
long wifi_connect(void) {

    nwp.role = sl_Start(NULL, NULL, NULL);

    return nwp.role;
}


int sbapp_init(void) {

    /* check if thread is already running */
    if (sbapp.main_pid == KERNEL_PID_UNDEF) {
        /* start NBAPP thread */
        sbapp.main_pid = thread_create(_stack, sizeof(_stack), GNRC_NBAPP_PRIO,
                           THREAD_CREATE_STACKTEST, _event_loop, NULL, "nbapp");
    }
    return sbapp.main_pid;
}

static void _send(gnrc_pktsnip_t *pkt) {

}

static void _receive(gnrc_pktsnip_t *pkt)
{
}


static void *_event_loop(void *arg)
{
    (void)arg;
    msg_t msg, reply;
    msg_t msg_queue[GNRC_NBAPP_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t netreg;

    /* preset reply message */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t)-ENOTSUP;

    /* initialize message queue */
    msg_init_queue(msg_queue, GNRC_NBAPP_MSG_QUEUE_SIZE);

    /* register NBAPP at netreg */
    netreg.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    netreg.pid = thread_getpid();
    gnrc_netreg_register(GNRC_NETTYPE_NBAPP, &netreg);

    if (VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY) < 0) {
        PANIC(SIMPLELINK_START_ERROR);
    }

    wifi_connect();

    /* dispatch NETAPI messages */
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {

            case SMARTCONFIG_ACTIVE:
                if (!IS_IP_ACQUIRED(nwp.status)) {
                    xtimer_set(&timer, MSEC_TO_TICKS(200));
                    LED_RED_TOGGLE;
                } else {
                    LED_RED_OFF;
                    LED_GREEN_ON;
                }

                break;
            case AUTO_CONNECTION_TIME_SLOT:
                if (counter == MAX_CONN_COUNTER_VAL) {
                    counter = 0;
                    LOG_INFO("Use Smart Config App to configure the device.\n");
#if 1
                    //Connect Using Smart Config
                    sts = SmartConfigConnect();
                    if (sts < 0) {
                        THROW(SMARTCONFIG_ERROR);
                    }

                    timer.callback = smartconfig_active;
                    xtimer_set(&timer, MSEC_TO_TICKS(200));
#endif
                    LED_RED_TOGGLE;
                    LED_YELLOW_OFF;
                } else if (!IS_IP_ACQUIRED(nwp.status)) {
                    counter++;
                    xtimer_set(&timer, MSEC_TO_TICKS(100));
                    LED_YELLOW_TOGGLE;
                }

                break;
            case WLAN_CONNECTED:
            {
                char mac[8];
                cpuid_get(mac);
                printf("%x-%x-%x\n", mac[0], mac[1], mac[2]);
                printf("role: %d - sts: %X\n", nwp.role, nwp.status);
                break;
            }
            case IP_ACQUIRED:

                LOG_INFO("\nSTA mode, connect to the AP[%s] and type"
                        "IP address [%lu.%lu.%lu.%lu] in the browser \n",
                        nwp.ssid, SL_IPV4_BYTE(nwp.ip, 3),
                        SL_IPV4_BYTE(nwp.ip, 2),
                        SL_IPV4_BYTE(nwp.ip, 1),
                        SL_IPV4_BYTE(nwp.ip, 0));

                LED_GREEN_ON;
                LED_YELLOW_OFF;

                handler_cb();
                break;


            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("nbapp: GNRC_NETAPI_MSG_TYPE_RCV\n");
                _receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("nbapp: GNRC_NETAPI_MSG_TYPE_SND\n");
                _send((gnrc_pktsnip_t *)msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
            case GNRC_NETAPI_MSG_TYPE_GET:
                msg_reply(&msg, &reply);
                break;
            default:
                DEBUG("nbapp: received unidentified message\n");
                break;
        }
    }

    /* never reached */
    return NULL;

}

