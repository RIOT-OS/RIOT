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
#include "nwp_conf.h"

#include "log.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

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

#define PANIC(ERR_ID)  puts(sl_err_descr[ERR_ID]); while(1) {}
#define PANIC2(ERR_ID, ERR_VAL) printf("%s (%ld)", sl_err_descr[ERR_ID], ERR_VAL); while(1) {}

#define STOP_IF_ERR(ERR_VAL, ERR_ID) if(ERR_VAL < 0) PANIC2(ERR_ID, ERR_VAL)

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
	NET_CFGSET_ERR,
	NET_SERVICECFG_ERR,
	SMARTCONFIG_ERR,
	SOCKET_OPEN_FAIL,
	SOCKET_CONNECT_FAIL
};

const char* sl_err_descr[] = { [NO_ERROR] = "no error", [GEN_ERROR
		] = "simplelink internal error", [SIMPLELINK_START_ERR
		] = "failed to start sl task", [WLAN_START_ERR
		] = "failed to initialize sl device", [WLAN_STOP_ERR
		] = "failed to stop sl device", [WLAN_SETMODE_ERR
		] = "failed to set wlan mode", [WLAN_FILTERSET_ERR
		] = "unable to set wlan filter", [WLAN_POLICYSET_ERR
		] = "wlan set policy failed",
		[WLAN_SET_ERR ] = "unable to set wlan cfg", [NET_CFGSET_ERR
				] = "failed to set net cfg", [NET_SERVICECFG_ERR
				] = "unable to exec net service api", [SMARTCONFIG_ERR
				] = "unable to setup smart config", [SOCKET_OPEN_FAIL
				] = "socket open failed", [SOCKET_CONNECT_FAIL
				] = "socket connect failed" };

/**
 * @brief describe a communication endpoint
 */
typedef struct cd_t {
	kernel_pid_t send_pid;
	kernel_pid_t recv_pid;
	int16_t fd;
	kernel_pid_t pid;
	char *send_stack;
	char *recv_stack;
} cd_t;


static uint8_t recv_buffer[RECV_BUFF_SIZE];


/**
 * @brief send a packet
 *
 * @internal
 */
static int16_t _send(cd_t *cd, gnrc_pktsnip_t *pkt);

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
} sbapp_t;

sbapp_t sbapp = { .main_pid = KERNEL_PID_UNDEF };

/**
 * @brief   memory segment for the NBAPP thread's stack
 */
#if ENABLE_DEBUG
static char _stack[SBAPP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[SBAPP_STACK_SIZE];
#endif

#if 0
/**
 * wifi module return codes
 */
typedef enum {
	// Choosing this number to avoid overlap w/ host-driver's error codes
	LAN_CONNECTION_FAILED = -0x7D0,
	CLIENT_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
	DEVICE_NOT_IN_STATION_MODE = CLIENT_CONNECTION_FAILED - 1,

	STATUS_CODE_MAX = -0xBB8
}app_rc;
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

	long sts = -1;

	sts = sl_Start(0, 0, 0);
	STOP_IF_ERR(sts, WLAN_START_ERR);

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
		STOP_IF_ERR(sts, WLAN_SETMODE_ERR)

		sts = sl_Stop(0xFF);
		STOP_IF_ERR(sts, WLAN_STOP_ERR)

		sts = sl_Start(0, 0, 0);
		STOP_IF_ERR(sts, WLAN_START_ERR);

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
	STOP_IF_ERR(sts, GEN_ERROR);

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
	STOP_IF_ERR(sts, WLAN_POLICYSET_ERR);

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
	STOP_IF_ERR(sts, NET_CFGSET_ERR);

	// Disable scan
	ucConfigOpt = SL_SCAN_POLICY(0);
	sts = sl_WlanPolicySet(SL_POLICY_SCAN, ucConfigOpt, NULL, 0);
	STOP_IF_ERR(sts, WLAN_POLICYSET_ERR);

	// Set Tx power level for station mode
	// Number between 0-15, as dB offset from max power - 0 will set max power
	ucPower = 0;
	sts = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
	WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *) &ucPower);
	STOP_IF_ERR(sts, WLAN_SET_ERR);

	// Set PM policy to normal
	sts = sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);
	STOP_IF_ERR(sts, WLAN_POLICYSET_ERR);

	// Unregister mDNS services
	sts = sl_NetAppMDNSUnRegisterService(0, 0);
	STOP_IF_ERR(sts, NET_SERVICECFG_ERR);

	// Remove  all 64 filters (8*8)
	memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
	sts = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *) &RxFilterIdMask,
			sizeof(_WlanRxFilterOperationCommandBuff_t));
	STOP_IF_ERR(sts, WLAN_FILTERSET_ERR);

	sts = sl_Stop(SL_STOP_TIMEOUT);
	STOP_IF_ERR(sts, WLAN_STOP_ERR);

	return sts;
}

/**
 * @brief signals that smartconfig process is active
 */
void smartconfig_still_active(void* arg) {
	msg_t m = { 0, SMARTCONFIG_ACTIVE, { 0 } };

	msg_send(&m, sbapp.main_pid);
}

/**
 * @brief notify that time has come for another connection retry
 */
void retry_connection(void* arg) {
	msg_t m = { 0, AUTO_CONNECTION_TIME_SLOT, { 0 } };

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

int sbapp_init(void) {

	/* check if thread is already running */
	if (sbapp.main_pid == KERNEL_PID_UNDEF) {
		/* start SBAPP thread */
		sbapp.main_pid = thread_create(_stack, sizeof(_stack), SBAPP_PRIO,
		THREAD_CREATE_STACKTEST, _event_loop, NULL, "sbapp");
	}
	return sbapp.main_pid;
}

int16_t connect_to(const char* server, uint16_t port) {
	SlSockAddrIn_t addr;
	int iAddrSize;
	int16_t fd; // socket file descriptor
	int sts;
	unsigned long server_ip;

	// resolve HOST NAME/IP
	sts = sl_NetAppDnsGetHostByName((signed char *) server, strlen(server),
			&server_ip, SL_AF_INET);

	// filling the TCP server socket address
	addr.sin_family = SL_AF_INET;
	addr.sin_port = sl_Htons(port);
	addr.sin_addr.s_addr = sl_Htonl(server_ip);

	// creating a TCP socket
	fd = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);
	if (fd < 0) {
		DEBUG("%s:%d %s (reason %d)", server, port, sl_err_descr[SOCKET_OPEN_FAIL], fd);
		return fd;
	}

	iAddrSize = sizeof(SlSockAddrIn_t);

	// connect to the TCP socket to the TCP server address
	sts = sl_Connect(fd, (SlSockAddr_t *) &addr, iAddrSize);
	if (sts < 0) {
		sl_Close(fd);
		fd = sts;
		DEBUG("%s:%d %s (reason %d)\n", server, port, sl_err_descr[SOCKET_CONNECT_FAIL], fd);
	}

	return fd;
}

/**
 * @brief send a packet
 */
static int16_t _send(cd_t* cd, gnrc_pktsnip_t *pkt) {
	int16_t sts;

	sts = sl_Send(cd->fd, pkt->data, pkt->size, 0);
	return sts;
}

/**
 * @brief sender task
 */
static void *_send_handler_task(void *arg) {
	msg_t msg, reply;
	msg_t msg_queue[SBAPP_MSG_QUEUE_SIZE];
	gnrc_netreg_entry_t netreg;
	cd_t *conn = (cd_t *) arg;
	char loop = 1;

	/* initialize message queue */
	msg_init_queue(msg_queue, SBAPP_MSG_QUEUE_SIZE);

	/* preset reply message */
	reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
	reply.content.value = (uint32_t) -ENOTSUP;

	/* register SBAPP at netreg */
	netreg.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
	netreg.pid = thread_getpid();
	gnrc_netreg_register(GNRC_NETTYPE_SBAPP, &netreg);

	while (loop) {
		msg_receive(&msg);
		switch (msg.type) {
		case SBAPI_MSG_TYPE_SND:
			DEBUG("sbapp send: GNRC_SBAPI_MSG_TYPE_SND\n");
			_send(conn, (gnrc_pktsnip_t *) msg.content.ptr);
			break;
		case SBAPI_MSG_TYPE_ERR:
			DEBUG("sbapp send: SBAPI_MSG_TYPE_ERR\n");
			loop = 0;
			break;
		case SBAPI_MSG_TYPE_SET:
		case SBAPI_MSG_TYPE_GET:
			msg_reply(&msg, &reply);
			break;
		default:
			DEBUG("sbapp send: received unidentified message\n");
			break;

		}
	}

	reply.type = SBAPI_MSG_TYPE_HANDLER_EXIT;
	reply.content.ptr = conn->send_stack;
	conn->send_stack = NULL;
	msg_send(&reply, sbapp.main_pid);

	if (conn->recv_stack == NULL) {
		close(conn->fd);
		free(conn);
	}

	return NULL;
}

/**
 * receive data task
 */
static void *_receive_handler_task(void* arg) {

	msg_t msg;   //, msg_queue[8];
	cd_t *cd = (cd_t *) arg;
	int16_t size = 0;
	gnrc_pktsnip_t *pkt;
	int nettype;

	nettype = GNRC_NETTYPE_UNDEF;

	do {
		size = recv(cd->fd, recv_buffer, RECV_BUFF_SIZE, 0);
		if (size <= 0) {
			/**
			 * error condition detected
			 * when a connection is closed by peer recv return 0
			 */
			DEBUG("channel %d: error %d\n", cd->fd, size);
			msg.type = SBAPI_MSG_TYPE_ERR;
			msg.content.value = size;

			// notify the twin thread
			msg_send(&msg, cd->send_pid);

		} else {
			recv_buffer[size] = 0;

			/* copy packet payload into pktbuf */
			pkt = gnrc_pktbuf_add(NULL, recv_buffer, size + 1, nettype);

			DEBUG("channel %d: recv: %s (len %d)\n",
					cd->fd, (char *) pkt->data, size);
			msg.type = SBAPI_MSG_TYPE_RCV;
			//msg.content.ptr = (char*)recv_buffer;
			msg.content.ptr = (void*) pkt;
		}
		msg_send(&msg, cd->pid);

	} while (size > 0);

	msg.type = SBAPI_MSG_TYPE_HANDLER_EXIT;
	msg.content.ptr = cd->recv_stack;
	cd->recv_stack = NULL;
	msg_send(&msg, sbapp.main_pid);

	if (cd->send_stack == NULL) {
		close(cd->fd);
		free(cd);
	}

	return NULL;
}

sbh_t sbapp_connect(const char* server, uint16_t port, kernel_pid_t pid) {

	while (!IS_IP_ACQUIRED(nwp.status)) {
		// wait for ip layer setup
	}

	// create a connection descriptor
	cd_t *conn = malloc(sizeof(cd_t));
	if (conn == NULL) {
		return NULL;
	}

	conn->fd = connect_to(server, port);
	if (conn->fd < 0) {
		return NULL;
	}

	conn->pid = pid;

#if ENABLE_DEBUG
	conn->send_stack = malloc(SBAPP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF);
#else
	conn->send_stack = malloc(SBAPP_STACK_SIZE);
#endif
	if (conn->send_stack == NULL) {
		free(conn);
		return NULL;
	}

#if ENABLE_DEBUG
	conn->recv_stack = malloc(SBAPP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF);
#else
	conn->recv_stack = malloc(SBAPP_STACK_SIZE);
#endif
	if (conn->recv_stack == NULL) {
		free(conn);
		free(conn->send_stack);
		return NULL;
	}

	// start the send task
	conn->send_pid = thread_create(conn->send_stack, SBAPP_STACK_SIZE,
	SBAPP_PRIO, THREAD_CREATE_STACKTEST, _send_handler_task, conn, server);

	// start the receive task
	conn->recv_pid = thread_create(conn->recv_stack, SBAPP_STACK_SIZE,
			SBAPP_PRIO, THREAD_CREATE_STACKTEST, _receive_handler_task, conn,
			server);

	return conn;
}

void sbapp_close(sbh_t fd) {
	cd_t *handle = (cd_t *) fd;

	close(handle->fd);
	free(handle->recv_stack);
	free(handle->send_stack);
	free(handle);
}

int sbapp_send(sbh_t fd, void* data, size_t len) {
	msg_t msg;
	cd_t *handle = (cd_t *) fd;

	gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, data, len, GNRC_NETTYPE_UNDEF);

	msg.type = SBAPI_MSG_TYPE_SND;
	msg.content.ptr = (void*) pkt;
	msg_send(&msg, handle->send_pid);

	return 0;
}

static void *_event_loop(void *arg) {
	(void) arg;
	msg_t msg, reply;
	msg_t msg_queue[SBAPP_MSG_QUEUE_SIZE];
	int counter = 0;

	/* preset reply message */
	reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
	reply.content.value = (uint32_t) -ENOTSUP;

	sbapp.sig_tim.callback = retry_connection;

	/* initialize message queue */
	msg_init_queue(msg_queue, SBAPP_MSG_QUEUE_SIZE);

	if (VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY) < 0) {
		PANIC(SIMPLELINK_START_ERR);
	}

	wifi_connect();

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
				LOG_INFO("Use Smart Config App to configure the device.\n");
				//Connect Using Smart Config
				if (smartconfig_connect() < 0) {
					PANIC(SMARTCONFIG_ERR);
				}

				sbapp.sig_tim.callback = smartconfig_still_active;
				xtimer_set(&sbapp.sig_tim, MSEC_TO_TICKS(200));
				LED_RED_TOGGLE;
				LED_YELLOW_OFF;
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

			break;

		case SBAPI_MSG_TYPE_SET:
		case SBAPI_MSG_TYPE_GET:
			msg_reply(&msg, &reply);
			break;
		case SBAPI_MSG_TYPE_HANDLER_EXIT:
			free(msg.content.ptr);
			break;
		default:
			DEBUG("sbapp: received unidentified message\n");
			break;
		}
	}

	/* never reached */
	return NULL;

}

