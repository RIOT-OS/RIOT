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
#include "sbapp.h"
#include "sbapp_priv.h"

#include "xtimer.h"
#include "net/gnrc.h"
#include "nwp_conf.h"

#include "log.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#ifdef DEVELHELP

#define PANIC(NS, ERR_ID)  puts(sl_err_descr[ERR_ID]); while(1) {}
#define PANIC2(NS, ERR_ID, ERR_VAL) printf("%s (%d)\n", sl_err_descr[ERR_ID], ERR_VAL); while(1) {}
#define SIGNAL(NS, ERR_ID)  puts(sl_err_descr[ERR_ID])

#else

#define PANIC(NS, ERR_ID)           while(1) {}
#define PANIC2(NS, ERR_ID, ERR_VAL) while(1) {}
#define SIGNAL(NS, ERR_ID)

#endif

#define STOP_IF_ERR(NS, ERR_VAL, ERR_ID) if(ERR_VAL < 0) {PANIC2(NS, ERR_ID, ERR_VAL);}

/**
 * @brief the recv receive buffer
 */
#define RECV_BUFF_SIZE                  (64)

// check ip acquisition every CONNECTION_CHECK_TIME_SLOT msec
#define CONN_CHECK_TIME_SLOT (100)

/**
 * @brief max time in msec waiting for an ip
 */
#define MAX_CONN_WAIT_TIME (10000)

/**
 * after MAX_CONN_COUNTER_VAL periods the device start the SmartConfig process
 * to get ssid/password from smartphone app.
 */
#define MAX_CONN_COUNTER_VAL (MAX_CONN_WAIT_TIME/CONN_CHECK_TIME_SLOT)

// prefix used for morse code led messages
#define NAMESPACE "SL"

/**
 * simplelink driver errors
 */
enum error_types {
    NO_ERROR = 0,
    GEN_ERROR,
    SIMPLELINK_START_ERR,
    WLAN_START_ERR,
    WLAN_STOP_ERR,
    WLAN_SETMODE_ERR,
    WLAN_FILTERSET_ERR,
    WLAN_POLICYSET_ERR,
    WLAN_SET_ERR,
    WLAN_CONNECTION_FAILED,
    NET_CFGSET_ERR,
    NET_SERVICECFG_ERR,
    SMARTCONFIG_ERR,
    SOCKET_OPEN_FAIL,
    SOCKET_CONNECT_FAIL
};

const char* sl_err_descr[] = {
        [NO_ERROR] = "no error",
        [GEN_ERROR ] = "simplelink internal error",
        [SIMPLELINK_START_ERR ] = "failed to start sl task",
        [WLAN_START_ERR ] = "failed to initialize sl device",
        [WLAN_STOP_ERR ] = "failed to stop sl device",
        [WLAN_CONNECTION_FAILED ] = "wifi connection failed",
        [WLAN_SETMODE_ERR ] = "failed to set wlan mode",
        [WLAN_FILTERSET_ERR ] = "unable to set wlan filter",
        [WLAN_POLICYSET_ERR ] = "wlan set policy failed",
        [WLAN_SET_ERR ] = "unable to set wlan cfg",
        [NET_CFGSET_ERR ] = "failed to set net cfg",
        [NET_SERVICECFG_ERR ] = "unable to exec net service api",
        [SMARTCONFIG_ERR ] = "unable to setup smart config",
        [SOCKET_OPEN_FAIL ] = "socket open failed",
        [SOCKET_CONNECT_FAIL ] = "socket connect failed" };

/**
 * @brief the sbapp thread function
 */
static void *_event_loop(void *arg);

/**
 * @brief the sbapp task handle
 */
typedef struct sbapp_t {
    kernel_pid_t main_pid; /**< the main task (send pkts and control commands */
    xtimer_t sig_tim; /**< a signaling timer for wifi events (smartconfig active, ...) */
    uint32_t options; /**< various wifi options */
    const char *ssid;
    const char *password;
} sbapp_t;

sbapp_t sbapp = {
        .main_pid = KERNEL_PID_UNDEF,
        .ssid = NULL,
        .password = NULL };

/**
 * @brief   memory segment for the NBAPP thread's stack
 */
#if ENABLE_DEBUG
static char _stack[SBAPP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[SBAPP_STACK_SIZE];
#endif

/**
 * @brief http sl callback, do nothing
 */
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent,
        SlHttpServerResponse_t *pSlHttpServerResponse) {
}

/**
 * @brief callback for WLAN events
 *
 * @param[in]  pWlanEvent - pointer to WLAN event info
 *
 * @return none
 */
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent) {
    msg_t msg;

    if (!pWlanEvent) {
        return;
    }

    switch (pWlanEvent->Event) {
    case SL_WLAN_CONNECT_EVENT: {
        SET_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION);

        memcpy(nwp.ssid,
                pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_name,
                pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
        memcpy(nwp.bssid,
                pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                SL_BSSID_LENGTH);

        LOG_INFO("[WLAN EVENT] STA Connected to the AP: %s ,"
                "BSSID: %x:%x:%x:%x:%x:%x\n\r", nwp.ssid, nwp.bssid[0],
                nwp.bssid[1], nwp.bssid[2], nwp.bssid[3], nwp.bssid[4],
                nwp.bssid[5]);
    }
        msg.type = WLAN_CONNECTED;

        msg_send(&msg, sbapp.main_pid);

        break;

    case SL_WLAN_DISCONNECT_EVENT: {
        slWlanConnectAsyncResponse_t* pEventData = NULL;

        CLR_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(nwp.status, STATUS_BIT_IP_ACQUIRED);

        pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

        if (SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code) {
            LOG_INFO("[WLAN EVENT]Device disconnected from the AP: %s,"
                    "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                    nwp.ssid, nwp.bssid[0], nwp.bssid[1], nwp.bssid[2],
                    nwp.bssid[3], nwp.bssid[4], nwp.bssid[5]);
        } else {
            LOG_INFO("[WLAN ERROR]Device disconnected from the AP: %s,"
                    "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r", nwp.ssid,
                    nwp.bssid[0], nwp.bssid[1], nwp.bssid[2], nwp.bssid[3],
                    nwp.bssid[4], nwp.bssid[5]);
        }
        memset(nwp.ssid, 0, sizeof(nwp.ssid));
        memset(nwp.bssid, 0, sizeof(nwp.bssid));
    }
        break;

    case SL_WLAN_STA_CONNECTED_EVENT: {
        // when device is in AP mode and any client connects to device cc3xxx
        SET_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION_FAILED);
    }
        break;

    case SL_WLAN_STA_DISCONNECTED_EVENT: {
        // when client disconnects from device (AP)
        CLR_STATUS_BIT(nwp.status, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(nwp.status, STATUS_BIT_IP_LEASED);
    }
        break;

    case SL_WLAN_SMART_CONFIG_COMPLETE_EVENT: {
        SET_STATUS_BIT(nwp.status, STATUS_BIT_SMARTCONFIG_START);
    }
        break;

    case SL_WLAN_SMART_CONFIG_STOP_EVENT: {
        // SmartConfig operation finished
        CLR_STATUS_BIT(nwp.status, STATUS_BIT_SMARTCONFIG_START);
    }
        break;

    case SL_WLAN_P2P_DEV_FOUND_EVENT: {
        SET_STATUS_BIT(nwp.status, STATUS_BIT_P2P_DEV_FOUND);
    }
        break;

    case SL_WLAN_P2P_NEG_REQ_RECEIVED_EVENT: {
        SET_STATUS_BIT(nwp.status, STATUS_BIT_P2P_REQ_RECEIVED);
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

/**
 * @brief callback for network events such as IP acquisition, IP
 *        leased, IP released etc.
 *
 * @param[in] pNetAppEvent - pointer to NetApp event info
 *
 * @return none
 */
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
    }
        break;

    case SL_NETAPP_IP_RELEASED_EVENT: {
        CLR_STATUS_BIT(nwp.status, STATUS_BIT_IP_LEASED);
    }
        break;

    default: {
        LOG_WARNING("[NETAPP EVENT] unexpected event [0x%lx] \n", ev->Event);
    }
        break;
    }
}

/**
 * @brief socket events callback
 *
 * @param[in] pSock - pointer to socket info
 *
 * @return none
 */
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock) {
    //
    // This application doesn't work w/ socket - Events are not expected
    //
}

/**
 * @brief connect to a WLAN access point using SmartConfig provisioning
 *
 *  This function enables SmartConfig provisioning for adding a new
 *    connection profile to CC3200. Since we have set the connection policy
 *    to Auto, once SmartConfig is complete,CC3200 will connect
 *    automatically to the new connection profile added by smartConfig.
 *
 *   @param[in]               None
 *
 *   @return  0 - Success
 *           -1 - Failure
 *
 *   TODO: verificare
 *   @warning           if the WLAN connection fails or we don't acquire an
 *                      IP address, We will be stuck in this function forever.
 */
long smartconfig_connect(void) {
    unsigned char policyVal;

    long sts;

    //set AUTO policy
    sts = sl_WlanPolicySet(SL_POLICY_CONNECTION,
            SL_CONNECTION_POLICY(1, 0, 0, 0, 0), &policyVal,
            1 /* PolicyValLen */);

    if (sts == 0) {
        //
        // Start SmartConfig
        // This example uses the unsecured SmartConfig method
        //
        sts = sl_WlanSmartConfigStart(0, /*groupIdBitmask*/
        SMART_CONFIG_CIPHER_NONE, /*cipher*/
        0, /*publicKeyLen*/
        0, /*group1KeyLen*/
        0, /*group2KeyLen */
        NULL, /*publicKey */
        NULL, /*group1Key */
        NULL); /*group2Key*/
    }

    return sts;
}

/**
 * @brief this function puts the device in its default state. It:
 *           - Set the mode to STATION
 *           - Configures connection policy to Auto and AutoSmartConfig
 *           - Deletes all the stored profiles
 *           - Enables DHCP
 *           - Disables Scan policy
 *           - Sets Tx power to maximum
 *           - Sets power policy to normal
 *           - Unregister mDNS services
 *           - Remove all filters
 *
 * @param   none
 * @return  on success, zero is returned. on error, negative is returned
 */
long simplelink_to_default_state(void) {
    SlVersionFull ver;
    _WlanRxFilterOperationCommandBuff_t RxFilterIdMask;

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    int16_t sts = -1;

    sts = sl_Start(0, 0, 0);
    STOP_IF_ERR(NAMESPACE, sts, WLAN_START_ERR);

    // If the device is not in station-mode, try configuring it in station-mode
    if (ROLE_STA != sts) {
        if (ROLE_AP == sts) {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while (!IS_IP_ACQUIRED(nwp.status)) {
            }
        }

        // Switch to STA role and restart
        sts = sl_WlanSetMode(ROLE_STA);
        STOP_IF_ERR(NAMESPACE, sts, WLAN_SETMODE_ERR);

        sts = sl_Stop(0xFF);
        STOP_IF_ERR(NAMESPACE, sts, WLAN_STOP_ERR);

        sts = sl_Start(0, 0, 0);
        STOP_IF_ERR(NAMESPACE, sts, WLAN_START_ERR);

        // Check if the device is in station again
        if (ROLE_STA != sts) {
            // We don't want to proceed if the device is not in STA-mode
            return -GEN_ERROR;
        }
    }

    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    sts = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, &ucConfigLen,
            (unsigned char *) (&ver));
    STOP_IF_ERR(NAMESPACE, sts, GEN_ERROR);

    LOG_INFO("Host Driver Version: %s\n\r", SL_DRIVER_VERSION);
    LOG_INFO("Build Version %lu.%lu.%lu.%lu.31.%lu.%lu.%lu.%lu.%d.%d.%d.%d\n\r",
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
    sts = sl_WlanPolicySet(SL_POLICY_CONNECTION,
            SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    STOP_IF_ERR(NAMESPACE, sts, WLAN_POLICYSET_ERR);

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore
    // other return-codes
    //
    sts = sl_WlanDisconnect();
    if (0 == sts) {
        // Wait
        while (IS_CONNECTED(nwp.status)) {
        }
    }

    // Enable DHCP client
    sts = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &ucVal);
    STOP_IF_ERR(NAMESPACE, sts, NET_CFGSET_ERR);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    sts = sl_WlanPolicySet(SL_POLICY_SCAN, ucConfigOpt, NULL, 0);
    STOP_IF_ERR(NAMESPACE, sts, WLAN_POLICYSET_ERR);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    sts = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
    WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *) &ucPower);
    STOP_IF_ERR(NAMESPACE, sts, WLAN_SET_ERR);

    // Set PM policy to normal
    sts = sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);
    STOP_IF_ERR(NAMESPACE, sts, WLAN_POLICYSET_ERR);

    // Unregister mDNS services
    sts = sl_NetAppMDNSUnRegisterService(0, 0);
    STOP_IF_ERR(NAMESPACE, sts, NET_SERVICECFG_ERR);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    sts = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *) &RxFilterIdMask,
            sizeof(_WlanRxFilterOperationCommandBuff_t));
    STOP_IF_ERR(NAMESPACE, sts, WLAN_FILTERSET_ERR);

    sts = sl_Stop(SL_STOP_TIMEOUT);
    STOP_IF_ERR(NAMESPACE, sts, WLAN_STOP_ERR);

    return sts;
}

/**
 *  @note char pointers are used assuming they are not freed by client meanwhile
 */
void sbapp_set_security(const char* ssid, const char* password) {
    sbapp.ssid = ssid;
    sbapp.password = password;
}

/**
 * @brief signals that smartconfig process is active
 */
void smartconfig_still_active(void* arg) {
    msg_t m = {
            0,
            SMARTCONFIG_ACTIVE,
            {
                    0 } };

    msg_send(&m, sbapp.main_pid);
}

/**
 * @brief notify that time has come for another connection retry
 */
void retry_connection(void* arg) {
    msg_t m = {
            0,
            AUTO_CONNECTION_TIME_SLOT,
            {
                    0 } };

    msg_send(&m, sbapp.main_pid);
}

/**
 * @brief Connects to the Network in STA Mode
 *
 * @return ROLE_STA (0) on success or a negative value on error
 */
long wifi_connect(void) {

    nwp.role = sl_Start(NULL, NULL, NULL);

    return nwp.role;
}

int sbapp_init(uint32_t options) {

    msg_t reply;
    msg_t timeout_msg;
    kernel_pid_t parent = thread_getpid();
    xtimer_t guard_timer;

    timeout_msg.type = 0x1;
    timeout_msg.content.value = -2;

    sbapp.options = options;

    /* check if thread is already running */
    if (sbapp.main_pid == KERNEL_PID_UNDEF) {

        // protection timer
        xtimer_set_msg(&guard_timer, MSEC_TO_TICKS(20000), &timeout_msg,
                parent);

        /* start SBAPP thread */
        sbapp.main_pid = thread_create(_stack, sizeof(_stack), SBAPP_PRIO,
        THREAD_CREATE_STACKTEST, _event_loop, &parent, "sbapp");

        // yield until ip connection is ready or an error condition prevents
        // a successful connection
        msg_receive(&reply);

        xtimer_remove(&guard_timer);
        if ((int) reply.content.value < 0) {
            return (int) reply.content.value;
        }
    }

    return sbapp.main_pid;
}

int sbapp_stop(void) {
    int sts;
    sbapp.main_pid = KERNEL_PID_UNDEF;
    sts = sl_Stop(SL_STOP_TIME_WAIT);
    VDeleteSimpleLinkSpawnTask();
    return sts;
}

uint32_t net_atoi(const char* name) {
    uint32_t ip;
    int sts;

    // resolve HOST NAME/IP
    sts = gethostbyname((signed char *) name, strlen(name), &ip, SL_AF_INET);
    if (sts < 0) {
        // TODO
    }
    DEBUG("%s ipv4 int value: %lu\n", name, ip);
    return ip;

}

int8_t sbapp_is_connected(uint16_t msec) {
    uint32_t t1 = xtimer_now();
    int32_t delta = 0;

    while (delta <= MSEC_TO_TICKS(msec) && !IS_IP_ACQUIRED(nwp.status)) {
        xtimer_usleep(MSEC_TO_TICKS(100));
        delta = xtimer_now() - t1;

    }
    return IS_IP_ACQUIRED(nwp.status);
}

int16_t sbapp_add_profile(const char* ssid, const char* pwd) {
    SlSecParams_t params;

    params.Key = (signed char *) pwd;
    params.KeyLen = strlen(pwd);
    params.Type = SECURITY_TYPE;

    return sl_WlanProfileAdd((signed char*) ssid, strlen(ssid), 0, &params, 0,
            1, 0);

}

uint16_t static _sbapp_add_profile(void) {
    SlSecParams_t params;

    params.Key = (signed char *) sbapp.password;
    params.KeyLen = strlen(sbapp.password);

    params.Type = SECURITY_TYPE;

    return sl_WlanProfileAdd((signed char*) sbapp.ssid, strlen(sbapp.ssid), 0,
            &params, 0, 1, 0);
}

static void sbapi_eval_msg(sbapi_opt_t *cmd) {
    switch (cmd->opt) {
    case SBAPI_STATUS:
        //nwp.role = sl_Start(0,0,0);
        break;
    default:
        // TODO: do nothing at the moment
        DEBUG("unexpected sbapi option/command\n");
    }
}

static void *_event_loop(void *arg) {
    kernel_pid_t parent_thr = *(kernel_pid_t *) arg;
    msg_t msg, reply;
    msg_t msg_queue[SBAPP_MSG_QUEUE_SIZE];
    int counter = 0;
    int16_t sts;

    /* preset reply message */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t) -ENOTSUP;

    sbapp.main_pid = thread_getpid();
    sbapp.sig_tim.callback = retry_connection;

    /* initialize message queue */
    msg_init_queue(msg_queue, SBAPP_MSG_QUEUE_SIZE);

    if (VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY) < 0) {
        PANIC(NAMESPACE, SIMPLELINK_START_ERR);
    }

    //simplelink_to_default_state();

    xtimer_set(&sbapp.sig_tim, MSEC_TO_TICKS(100));

    // start the simplelink device channel
    nwp.role = sl_Start(NULL, NULL, NULL);

    if (sbapp.options & SBAPI_AUTOSTART) {
        // Set connection policy to Auto
        sts = sl_WlanPolicySet(SL_POLICY_CONNECTION,
                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    } else {
        sts = sl_WlanPolicySet(SL_POLICY_CONNECTION,
                SL_CONNECTION_POLICY(0, 0, 0, 0, 0), NULL, 0);
    }
    STOP_IF_ERR(NAMESPACE, sts, WLAN_POLICYSET_ERR);

    if (sbapp.options & SBAPI_DELETE_PROFILES) {
        sl_WlanProfileDel(0xFF);
    }

    if (sbapp.ssid != NULL) {
        if (_sbapp_add_profile() < 0) {
            DEBUG("add profile failed, try to go ahead\n");
        }
    }

    /* dispatch SBAPI messages and manage simplelink events */
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {

        case SMARTCONFIG_ACTIVE:
            if (!IS_IP_ACQUIRED(nwp.status)) {
                xtimer_set(&sbapp.sig_tim, MSEC_TO_TICKS(200));
                LED_RED_TOGGLE;
            } else {
                LED_RED_OFF;
                LED_GREEN_ON;
            }

            break;
        case AUTO_CONNECTION_TIME_SLOT:
            if (counter == MAX_CONN_COUNTER_VAL) {
                counter = 0;
                if (sbapp.options & SBAPI_SMARTCONFIG) {
                    LOG_INFO("Use Smart Config App to configure the device.\n");
                    //Connect Using Smart Config
                    if (smartconfig_connect() < 0) {
                        PANIC(NAMESPACE, SMARTCONFIG_ERR);
                    }
                    sbapp.sig_tim.callback = smartconfig_still_active;
                    xtimer_set(&sbapp.sig_tim, MSEC_TO_TICKS(200));
                    LED_RED_TOGGLE;
                    LED_YELLOW_OFF;
                } else {
                    // PANIC(NAMESPACE, WLAN_CONNECTION_FAILED);
                    // TODO: send a FAILURE message
                    SIGNAL(NAMESPACE, WLAN_CONNECTION_FAILED);
                    msg.type = 0x1000;
                    msg.content.value = SBAPI_CONNECTION_ERR;
                    msg_send(&msg, parent_thr);
                }
            } else if (!IS_IP_ACQUIRED(nwp.status)) {
                counter++;
                xtimer_set(&sbapp.sig_tim, MSEC_TO_TICKS(100));
                LED_YELLOW_TOGGLE;
            }

            break;
        case WLAN_CONNECTED: {
            break;
        }
        case IP_ACQUIRED:

            LOG_INFO("\nSTA mode, connect to the AP[%s] and type"
                    "IP address [%lu.%lu.%lu.%lu] in the browser \n", nwp.ssid,
                    SL_IPV4_BYTE(nwp.ip, 3), SL_IPV4_BYTE(nwp.ip, 2),
                    SL_IPV4_BYTE(nwp.ip, 1), SL_IPV4_BYTE(nwp.ip, 0));

            LED_GREEN_ON;
            LED_YELLOW_OFF;
            msg.type = 0x1000;
            msg_send(&msg, parent_thr);
            break;

        case SBAPI_MSG_TYPE_SET: {
            sbapi_eval_msg((sbapi_opt_t *) msg.content.ptr);
            break;
        }
        case SBAPI_MSG_TYPE_GET:
            msg_reply(&msg, &reply);
            break;
        default:
            DEBUG("sbapp: received unidentified message\n");
            break;
        }
    }

    /* never reached */
    return NULL;

}

