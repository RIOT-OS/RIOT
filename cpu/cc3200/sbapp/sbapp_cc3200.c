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
#include "output_messages.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

/**
 * @brief msec timeout before hibernating network processor
 */
#define SL_STOP_TIME_WAIT (1000)

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
		[WLAN_CONNECTION_FAILED] = "wifi connection failed",
		[WLAN_SETMODE_ERR ] = "failed to set wlan mode",
		[WLAN_FILTERSET_ERR ] = "unable to set wlan filter",
		[WLAN_POLICYSET_ERR ] = "wlan set policy failed",
		[WLAN_SET_ERR ] = "unable to set wlan cfg",
		[NET_CFGSET_ERR ] = "failed to set net cfg",
		[NET_SERVICECFG_ERR ] = "unable to exec net service api",
		[SMARTCONFIG_ERR ] = "unable to setup smart config",
		[SOCKET_OPEN_FAIL ] = "socket open failed",
		[SOCKET_CONNECT_FAIL ] = "socket connect failed" };

static uint8_t recv_buffer[RECV_BUFF_SIZE];

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
		.password = NULL
    };

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

			// If the user has initiated 'Disconnect' request,
			//'reason_code' is SL_USER_INITIATED_DISCONNECTION
			//cppcheck-suppress duplicateBranch
			if (SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code) {
				LOG_INFO(
						"[WLAN EVENT]Device disconnected from the AP: %s,"
								"BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
						nwp.ssid, nwp.bssid[0], nwp.bssid[1], nwp.bssid[2],
						nwp.bssid[3], nwp.bssid[4], nwp.bssid[5]);
			} else {
				LOG_INFO("[WLAN ERROR]Device disconnected from the AP: %s,"
						"BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
						nwp.ssid, nwp.bssid[0], nwp.bssid[1], nwp.bssid[2],
						nwp.bssid[3], nwp.bssid[4], nwp.bssid[5]);
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
			LOG_WARNING("[NETAPP EVENT] unexpected event [0x%lx] \n",
					ev->Event);
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

	// Remove all profiles
	//lRetVal = sl_WlanProfileDel(0xFF);
	//ASSERT_ON_ERROR(lRetVal);

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
	msg_t m = {0, SMARTCONFIG_ACTIVE, {0}};

	msg_send(&m, sbapp.main_pid);
}

/**
 * @brief notify that time has come for another connection retry
 */
void retry_connection(void* arg) {
	msg_t m = {0, AUTO_CONNECTION_TIME_SLOT, {0}};

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
	kernel_pid_t parent = thread_getpid();

	sbapp.options = options;

	/* check if thread is already running */
	if (sbapp.main_pid == KERNEL_PID_UNDEF) {
		/* start SBAPP thread */
		sbapp.main_pid = thread_create(_stack, sizeof(_stack), SBAPP_PRIO,
		THREAD_CREATE_STACKTEST, _event_loop, &parent, "sbapp");

		// yield until ip connection is ready or an error condition prevents
		// a successful connection
	    msg_receive(&reply);
	    if ((int)reply.content.value < 0) {
	        return (int)reply.content.value;
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

int16_t connect_to(cd_t *cd, uint32_t remote_ip, uint16_t remote_port,
		uint16_t local_port) {
	//SlSockAddrIn_t addr;
	int iAddrSize;
	int16_t fd; // socket file descriptor
	int sts;
	//unsigned long server_ip;

	//SlSockAddrIn_t  local_addr;

	cd->local_addr.sin_family = SL_AF_INET;
	if (local_port) {
		cd->local_addr.sin_port = sl_Htons(local_port);
	} else {
		cd->local_addr.sin_port = 0;
	}

	cd->local_addr.sin_addr.s_addr = 0;

	// filling the TCP server socket address
	cd->addr.sin_family = SL_AF_INET;
	cd->addr.sin_port = sl_Htons(remote_port);
	cd->addr.sin_addr.s_addr = sl_Htonl(remote_ip);

	// creating a TCP or UDP socket
	fd = sl_Socket(SL_AF_INET, cd->conn_type, 0);
	if (fd < 0) {
		DEBUG("%ld:%d %s (reason %d)", remote_ip, remote_port,
				sl_err_descr[SOCKET_OPEN_FAIL], fd);
		return fd;
	}

	sts = sl_Bind(fd, (SlSockAddr_t *) &cd->local_addr, sizeof(SlSockAddrIn_t));
	if (sts < 0) {
		DEBUG("%lu:%d bind failed (reason %d)\n", remote_ip, remote_port, fd);
		sl_Close(fd);
	}

	if (cd->conn_type == TCP) {
		iAddrSize = sizeof(sockaddr_in);

		// connect to the TCP socket to the TCP server address
		sts = sl_Connect(fd, (SlSockAddr_t *) &cd->addr, iAddrSize);
		if (sts < 0) {
			sl_Close(fd);
			fd = sts;
			DEBUG("%lu:%d %s (reason %d)\n", remote_ip, remote_port,
					sl_err_descr[SOCKET_CONNECT_FAIL], fd);
		}
	}
	return fd;
}


/**
 * receive data task
 */
static void *_receive_handler_task(void* arg) {

	msg_t msg;   //, msg_queue[8];
	cd_t *cd = (cd_t *) arg;
	int16_t size = 0;
	gnrc_pktsnip_t *l4, *l3;
	sockaddr_in from_addr;
	socklen_t addr_size = sizeof(sockaddr_in);

	do {
		size = recvfrom(cd->fd, recv_buffer, RECV_BUFF_SIZE, 0,
				(sockaddr*) &from_addr, &addr_size);
		if (size <= 0) {
			/**
			 * error condition detected
			 * when a connection is closed by peer recv return 0
			 */
			DEBUG("channel %d: error %d\n", cd->fd, size);
		} else {
			recv_buffer[size] = 0;

			cd->addr = from_addr;

			/* copy packet payload into pktbuf */
			l3 = gnrc_pktbuf_add(NULL, cd, sizeof(cd_t), GNRC_NETTYPE_SBAPP);

			l4 = gnrc_pktbuf_add(l3, recv_buffer, size + 1, GNRC_NETTYPE_UNDEF);

			DEBUG("channel %d: recv: %s (len %d) from %lu.%lu.%lu.%lu:%d\n",
					cd->fd, (char *) l4->data, size,
					SL_IPV4_BYTE(cd->addr.sin_addr.s_addr, 0),
					SL_IPV4_BYTE(cd->addr.sin_addr.s_addr, 1),
					SL_IPV4_BYTE(cd->addr.sin_addr.s_addr, 2),
					SL_IPV4_BYTE(cd->addr.sin_addr.s_addr, 3),
					ntohs(cd->addr.sin_port));
			msg.type = SBAPI_MSG_TYPE_RCV;
			//msg.content.ptr = (char*)recv_buffer;
			msg.content.ptr = (void*) l4;
		}
		msg_send(&msg, cd->pid);

	} while (size > 0);

	msg.type = SBAPI_MSG_TYPE_HANDLER_EXIT;
	msg.content.ptr = (char *)cd;
	msg_send(&msg, sbapp.main_pid);

#if 0
	if (cd->send_stack == NULL) {
		close(cd->fd);
		free(cd);
	}
#endif

	return NULL;
}


sbh_t sbapp_connect(uint8_t conn_type, uint32_t remote_ip, uint16_t remote_port,
		uint16_t local_port, kernel_pid_t pid) {

	// create a connection descriptor
	cd_t *conn = malloc(sizeof(cd_t));
	if (conn == NULL) {
		return NULL;
	}
	conn->conn_type = conn_type;

	conn->fd = connect_to(conn, remote_ip, remote_port, local_port);
	if (conn->fd < 0) {
		return NULL;
	}

	conn->pid = pid;

#if ENABLE_DEBUG
	conn->recv_stack = malloc(SBAPP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF);
#else
	conn->recv_stack = malloc(SBAPP_STACK_SIZE);
#endif
	if (conn->recv_stack == NULL) {
		free(conn);
		return NULL;
	}

	// start the receive task
	conn->recv_pid = thread_create(conn->recv_stack, SBAPP_STACK_SIZE,
	SBAPP_PRIO, THREAD_CREATE_STACKTEST, _receive_handler_task, conn, "sbapp");

	return conn;
}

void sbapp_close(sbh_t fd) {
	cd_t *handle = (cd_t *) fd;

	close(handle->fd);
	free(handle->recv_stack);
	free(handle);
}


/**
 * @brief send a packet
 */
int sbapp_send(sbh_t fd, void* data, size_t len) {
    int16_t sts;
    cd_t *cd = (cd_t *) fd;

    if (cd->conn_type == TCP) {
        sts = sl_Send(cd->fd, data, len, 0);
    } else {
        sts = sl_SendTo(cd->fd, data, len, 0, (sockaddr *) &cd->addr,
                sizeof(sockaddr_in));
    }
    return sts;
}



int sbapp_sendto(sbh_t fd, void* data, size_t len, sockaddr_in addr) {
	int16_t sts;
	cd_t* cd = (cd_t *)fd;

	sts = sl_SendTo(cd->fd, data, len, 0, (sockaddr *) &addr,
			sizeof(sockaddr_in));

	return sts;
}

int8_t sbapp_is_connected(uint16_t msec) {
    uint32_t t1 = xtimer_now();
    int32_t delta = 0;

    while (delta <= MSEC_TO_TICKS(msec) && !IS_IP_ACQUIRED(nwp.status) ) {
        xtimer_usleep(MSEC_TO_TICKS(100));
        delta = xtimer_now() - t1;

    }
    return IS_IP_ACQUIRED(nwp.status);
}

int16_t sbapp_add_profile(const char* ssid, const char* pwd) {
    SlSecParams_t params;

    params.Key = (signed char *)pwd;
    params.KeyLen = strlen(pwd);
    params.Type = SECURITY_TYPE;

    return sl_WlanProfileAdd((signed char*) ssid, strlen(ssid), 0,
                             &params, 0, 1, 0);

}

uint16_t static _sbapp_add_profile(void) {
    SlSecParams_t params;

    params.Key = (signed char *)sbapp.password;
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

	simplelink_to_default_state();

	xtimer_set(&sbapp.sig_tim, MSEC_TO_TICKS(100));

	// start the simplelink device channel
	nwp.role = sl_Start(NULL, NULL, NULL);

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
						"IP address [%lu.%lu.%lu.%lu] in the browser \n",
						nwp.ssid, SL_IPV4_BYTE(nwp.ip, 3),
						SL_IPV4_BYTE(nwp.ip, 2), SL_IPV4_BYTE(nwp.ip, 1),
						SL_IPV4_BYTE(nwp.ip, 0));

				LED_GREEN_ON;
				LED_YELLOW_OFF;
				msg.type = 0x1000;
				msg_send(&msg, parent_thr);
				break;

			case SBAPI_MSG_TYPE_SET: {
			    sbapi_eval_msg((sbapi_opt_t *)msg.content.ptr);
			    break;
			}
			case SBAPI_MSG_TYPE_GET:
				msg_reply(&msg, &reply);
				break;
			case SBAPI_MSG_TYPE_HANDLER_EXIT:
				free(msg.content.ptr);
				sbapp_close(msg.content.ptr);

				// TODO: send a message to upper layer

				break;
			default:
				DEBUG("sbapp: received unidentified message\n");
				break;
		}
	}

	/* never reached */
	return NULL;

}

