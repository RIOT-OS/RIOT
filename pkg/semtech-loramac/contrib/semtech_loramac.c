/*
 * Copyright (C) 2017 Inria
 *               2017 Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     pkg_semtech-loramac
 *
 * @file
 * @brief       Implementation of public API for Semtech LoRaMAC
 *
 * This implementation is an adaption of the applications provided on the
 * Semtech Lora-net repository.
 *
 * The LoRaMAC stack and the SX127x driver run in their own thread and simple
 * IPC messages are exchanged to control the MAC.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Jose Alamos <jose.alamos@inria.cl>
 * @}
 */

#include <string.h>

#include "msg.h"

#include "net/netdev.h"
#include "net/loramac.h"

#include "sx127x.h"
#include "sx127x_params.h"
#include "sx127x_netdev.h"

#include "semtech_loramac.h"
#include "semtech-loramac/board.h"
#include "LoRaMac.h"
#include "region/Region.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define LORAWAN_MAX_JOIN_RETRIES                    (3U)

#if defined(REGION_EU868)
#define LORAWAN_DUTYCYCLE_ON                        (true)
#define USE_SEMTECH_DEFAULT_CHANNEL_LINEUP          (1)

#if (USE_SEMTECH_DEFAULT_CHANNEL_LINEUP)
#define LC4                { 867100000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC5                { 867300000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC6                { 867500000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC7                { 867700000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC8                { 867900000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC9                { 868800000, 0, { ( ( DR_7 << 4 ) | DR_7 ) }, 2 }
#define LC10               { 868300000, 0, { ( ( DR_6 << 4 ) | DR_6 ) }, 1 }
#endif /* USE_SEMTECH_DEFAULT_CHANNEL_LINEUP */
#endif /* REGION_EU868 */

#define SEMTECH_LORAMAC_MSG_QUEUE                   (16U)
#define SEMTECH_LORAMAC_LORAMAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
static char _semtech_loramac_stack[SEMTECH_LORAMAC_LORAMAC_STACKSIZE];
kernel_pid_t semtech_loramac_pid;
kernel_pid_t semtech_loramac_handler_pid;

RadioEvents_t semtech_loramac_radio_events;
uint8_t semtech_loramac_dev_eui[LORAMAC_DEVEUI_LEN];
uint8_t semtech_loramac_app_eui[LORAMAC_APPEUI_LEN];
uint8_t semtech_loramac_app_key[LORAMAC_APPKEY_LEN];
uint8_t semtech_loramac_nwk_skey[LORAMAC_NWKSKEY_LEN];
uint8_t semtech_loramac_app_skey[LORAMAC_APPSKEY_LEN];
uint8_t semtech_loramac_dev_addr[LORAMAC_DEVADDR_LEN];

static uint8_t _semtech_loramac_radio_payload[SX127X_RX_BUFFER_SIZE];
static semtech_loramac_rx_data_t _semtech_loramac_rx_data;

typedef struct {
    uint8_t port;
    uint8_t cnf;
    uint8_t dr;
    uint8_t *payload;
    uint8_t len;
} loramac_send_params_t;

typedef void (*semtech_loramac_func_t)(void *);

/**
 * @brief   Struct containing a semtech loramac function call
 *
 * This function is called inside the semtech loramac thread context.
 */
typedef struct {
    semtech_loramac_func_t func;            /**< the function to call. */
    void *arg;                              /**< argument of the function **/
} semtech_loramac_call_t;

/* Prepares the payload of the frame */
static bool _semtech_loramac_send(uint8_t cnf, uint8_t port, uint8_t dr,
                                  uint8_t *payload, uint8_t len)
{
    DEBUG("[semtech-loramac] send frame %s\n", (char *)payload);
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if (LoRaMacQueryTxPossible(len, &txInfo) != LORAMAC_STATUS_OK) {
        DEBUG("[semtech-loramac] empty frame in order to flush MAC commands\n");
        /* Send empty frame in order to flush MAC commands */
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = (int8_t)dr;
    }
    else {
        if (cnf == LORAMAC_TX_UNCNF) {
            DEBUG("[semtech-loramac] MCPS_UNCONFIRMED\n");
            mcpsReq.Type = MCPS_UNCONFIRMED;
            mcpsReq.Req.Unconfirmed.fPort = port;
            mcpsReq.Req.Unconfirmed.fBuffer = payload;
            mcpsReq.Req.Unconfirmed.fBufferSize = len;
            mcpsReq.Req.Unconfirmed.Datarate = (int8_t)dr;
        }
        else {
            DEBUG("[semtech-loramac] MCPS_CONFIRMED\n");
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.fPort = port;
            mcpsReq.Req.Confirmed.fBuffer = payload;
            mcpsReq.Req.Confirmed.fBufferSize = len;
            mcpsReq.Req.Confirmed.NbTrials = 3;
            mcpsReq.Req.Confirmed.Datarate = (int8_t)dr;
        }
    }

    if (LoRaMacMcpsRequest(&mcpsReq) == LORAMAC_STATUS_OK) {
        DEBUG("[semtech-loramac] MCPS request OK\n");
        return false;
    }

    return true;
}

/* MCPS-Confirm event function */
static void mcps_confirm(McpsConfirm_t *confirm)
{
    DEBUG("[semtech-loramac] MCPS confirm event\n");
    if (confirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
        DEBUG("[semtech-loramac] MCPS confirm event OK\n");

        switch (confirm->McpsRequest) {
            case MCPS_UNCONFIRMED:
            {
                /* Check Datarate
                   Check TxPower */
                DEBUG("[semtech-loramac] MCPS confirm event UNCONFIRMED\n");
                msg_t msg;
                msg.type = MSG_TYPE_LORAMAC_NOTIFY;
                msg.content.value = SEMTECH_LORAMAC_TX_DONE;
                msg_send(&msg, semtech_loramac_handler_pid);
                break;
            }

            case MCPS_CONFIRMED:
                /* Check Datarate
                   Check TxPower
                   Check AckReceived
                   Check NbTrials */
                DEBUG("[semtech-loramac] MCPS confirm event CONFIRMED\n");
                break;

            case MCPS_PROPRIETARY:
                DEBUG("[semtech-loramac] MCPS confirm event PROPRIETARY\n");
                break;

            default:
                DEBUG("[semtech-loramac] MCPS confirm event UNKNOWN\n");
                break;
        }
    }
}

/* MCPS-Indication event function */
static void mcps_indication(McpsIndication_t *indication)
{
    DEBUG("[semtech-loramac] MCPS indication event\n");
    if (indication->Status != LORAMAC_EVENT_INFO_STATUS_OK) {
        DEBUG("[semtech-loramac] MCPS indication no OK\n");
        return;
    }

    if (ENABLE_DEBUG) {
        switch (indication->McpsIndication) {
            case MCPS_UNCONFIRMED:
                DEBUG("[semtech-loramac] MCPS indication Unconfirmed\n");
                break;

            case MCPS_CONFIRMED:
                DEBUG("[semtech-loramac] MCPS indication Confirmed\n");
                break;

            case MCPS_PROPRIETARY:
                DEBUG("[semtech-loramac] MCPS indication Proprietary\n");
                break;

            case MCPS_MULTICAST:
                DEBUG("[semtech-loramac] MCPS indication Multicast\n");
                break;

            default:
                break;
        }
    }

    msg_t msg;
    msg.type = MSG_TYPE_LORAMAC_NOTIFY;
    if (indication->RxData) {
        indication->Buffer[indication->BufferSize] = '\0';
        memcpy(_semtech_loramac_rx_data.payload, indication->Buffer,
               indication->BufferSize);
        _semtech_loramac_rx_data.payload[indication->BufferSize] = 0;
        _semtech_loramac_rx_data.payload_len = indication->BufferSize;
        _semtech_loramac_rx_data.port = indication->Port;
        DEBUG("[semtech-loramac] MCPS indication:\n"
              "  - Payload: %s\n"
              "  - Size: %d\n"
              "  - Port: %d\n",
              (char *)_semtech_loramac_rx_data.payload,
              _semtech_loramac_rx_data.payload_len,
              _semtech_loramac_rx_data.port
              );
        msg.content.value = SEMTECH_LORAMAC_RX_DATA;
    }
    else {
        msg.content.value = SEMTECH_LORAMAC_TX_DONE;
    }
    msg_send(&msg, semtech_loramac_handler_pid);
}

/*MLME-Confirm event function */
static void mlme_confirm(MlmeConfirm_t *confirm)
{
    DEBUG("[semtech-loramac] MLME confirm event\n");
    switch (confirm->MlmeRequest) {
        case MLME_JOIN:
            if (confirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                /* Status is OK, node has joined the network */
                DEBUG("[semtech-loramac] join succeeded\n");
                msg_t msg;
                msg.type = MSG_TYPE_LORAMAC_NOTIFY;
                msg.content.value = SEMTECH_LORAMAC_JOIN_SUCCEEDED;
                msg_send(&msg, semtech_loramac_handler_pid);
            }
            else {
                DEBUG("[semtech-loramac] join not successful\n");
                /* Join was not successful. */
                msg_t msg;
                msg.type = MSG_TYPE_LORAMAC_NOTIFY;
                msg.content.value = SEMTECH_LORAMAC_JOIN_FAILED;
                msg_send(&msg, semtech_loramac_handler_pid);
            }
            break;

        default:
            break;
    }
}

void _loramac_set_rx2_params(uint32_t freq, uint8_t dr)
{
    Rx2ChannelParams_t params;
    params.Frequency = freq;
    params.Datarate = dr;

    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    mibReq.Param.Rx2DefaultChannel = params;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_RX2_CHANNEL;
    mibReq.Param.Rx2Channel = params;
    LoRaMacMibSetRequestConfirm(&mibReq);
}

void _init_loramac(LoRaMacPrimitives_t * primitives, LoRaMacCallback_t *callbacks)
{
    DEBUG("[semtech-loramac] initializing loramac\n");
    primitives->MacMcpsConfirm = mcps_confirm;
    primitives->MacMcpsIndication = mcps_indication;
    primitives->MacMlmeConfirm = mlme_confirm;
#if defined(REGION_AS923)
    DEBUG("[semtech-loramac] initialize loramac for AS923 region\n");
    LoRaMacInitialization(&semtech_loramac_radio_events, primitives, callbacks,
                          LORAMAC_REGION_AS923);
#elif defined(REGION_AU915)
    DEBUG("[semtech-loramac] initialize loramac for AU915 region\n");
    LoRaMacInitialization(&semtech_loramac_radio_events, primitives, callbacks,
                          LORAMAC_REGION_AU915);
#elif defined(REGION_CN779)
    DEBUG("[semtech-loramac] initialize loramac for CN779 region\n");
    LoRaMacInitialization(&semtech_loramac_radio_events, primitives, callbacks,
                          LORAMAC_REGION_CN779);
#elif defined(REGION_EU868)
    DEBUG("[semtech-loramac] initialize loramac for EU868 region\n");
    LoRaMacInitialization(&semtech_loramac_radio_events, primitives, callbacks,
                          LORAMAC_REGION_EU868);
#elif defined(REGION_IN865)
    DEBUG("[semtech-loramac] initialize loramac for IN865 region\n");
    LoRaMacInitialization(&semtech_loramac_radio_events, primitives, callbacks,
                          LORAMAC_REGION_IN865);
#elif defined(REGION_KR920)
    DEBUG("[semtech-loramac] initialize loramac for KR920 region\n");
    LoRaMacInitialization(&semtech_loramac_radio_events, primitives, callbacks,
                          LORAMAC_REGION_KR920);
#elif defined(REGION_US915)
    DEBUG("[semtech-loramac] initialize loramac for US915 region\n");
    LoRaMacInitialization(&semtech_loramac_radio_events, primitives, callbacks,
                          LORAMAC_REGION_US915);
#elif defined(REGION_US915_HYBRID)
    DEBUG("[semtech-loramac] initialize loramac for US915 hybrid region\n");
    LoRaMacInitialization(&semtech_loramac_radio_events, primitives, callbacks,
                          LORAMAC_REGION_US915_HYBRID);
#else
#error "Please define a region in the compiler options."
#endif

#if defined(REGION_EU868) && USE_SEMTECH_DEFAULT_CHANNEL_LINEUP
    DEBUG("[semtech-loramac] EU868 region: use default channels\n");
    LoRaMacChannelAdd(3, (ChannelParams_t)LC4);
    LoRaMacChannelAdd(4, (ChannelParams_t)LC5);
    LoRaMacChannelAdd(5, (ChannelParams_t)LC6);
    LoRaMacChannelAdd(6, (ChannelParams_t)LC7);
    LoRaMacChannelAdd(7, (ChannelParams_t)LC8);
    LoRaMacChannelAdd(8, (ChannelParams_t)LC9);
    LoRaMacChannelAdd(9, (ChannelParams_t)LC10);

    _loramac_set_rx2_params(LORAMAC_DEFAULT_RX2_FREQ, LORAMAC_DEFAULT_RX2_DR);
#endif
}

static void _join_otaa(void)
{
    DEBUG("[semtech-loramac] starting OTAA join\n");

    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NETWORK_JOINED;
    mibReq.Param.IsNetworkJoined = false;
    LoRaMacMibSetRequestConfirm(&mibReq);

    MlmeReq_t mlmeReq;
    mlmeReq.Type = MLME_JOIN;
    mlmeReq.Req.Join.DevEui = semtech_loramac_dev_eui;
    mlmeReq.Req.Join.AppEui = semtech_loramac_app_eui;
    mlmeReq.Req.Join.AppKey = semtech_loramac_app_key;
    mlmeReq.Req.Join.NbTrials = LORAWAN_MAX_JOIN_RETRIES;
    LoRaMacMlmeRequest(&mlmeReq);
}

static void _join_abp(void)
{
    DEBUG("[semtech-loramac] starting ABP join\n");

    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NETWORK_JOINED;
    mibReq.Param.IsNetworkJoined = false;
    LoRaMacMibSetRequestConfirm(&mibReq);

    semtech_loramac_set_netid(LORAMAC_DEFAULT_NETID);

    mibReq.Type = MIB_DEV_ADDR;
    mibReq.Param.DevAddr = ((uint32_t)semtech_loramac_dev_addr[0] << 24 |
                            (uint32_t)semtech_loramac_dev_addr[1] << 16 |
                            (uint32_t)semtech_loramac_dev_addr[2] << 8 |
                            (uint32_t)semtech_loramac_dev_addr[3]);
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_NWK_SKEY;
    mibReq.Param.NwkSKey = semtech_loramac_nwk_skey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_APP_SKEY;
    mibReq.Param.AppSKey = semtech_loramac_app_skey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_NETWORK_JOINED;
    mibReq.Param.IsNetworkJoined = true;
    LoRaMacMibSetRequestConfirm(&mibReq);
}

static void _join(void *arg)
{
    (void) arg;
    uint8_t join_type = *(uint8_t *)arg;

    switch (join_type) {
        case LORAMAC_JOIN_OTAA:
            _join_otaa();
            break;

        case LORAMAC_JOIN_ABP:
            _join_abp();
            break;
    }
}

static void _send(void *arg)
{
    loramac_send_params_t params = *(loramac_send_params_t *)arg;
    _semtech_loramac_send(params.cnf, params.port, params.dr,
                          params.payload, params.len);
}

static void _semtech_loramac_call(semtech_loramac_func_t func, void *arg)
{
    semtech_loramac_call_t call;
    call.func = func;
    call.arg = arg;

    msg_t msg;
    msg.type = MSG_TYPE_LORAMAC_CMD;
    msg.content.ptr = &call;
    msg_send(&msg, semtech_loramac_pid);
}

static void _semtech_loramac_event_cb(netdev_t *dev, netdev_event_t event)
{
    netdev_sx127x_lora_packet_info_t packet_info;

    msg_t msg;
    msg.content.ptr = dev;

    switch (event) {
        case NETDEV_EVENT_ISR:
            msg.type = MSG_TYPE_ISR;
            if (msg_send(&msg, semtech_loramac_pid) <= 0) {
                DEBUG("[semtech-loramac] possibly lost interrupt.\n");
            }
            break;

        case NETDEV_EVENT_TX_COMPLETE:
            sx127x_set_sleep((sx127x_t *)dev);
            semtech_loramac_radio_events.TxDone();
            DEBUG("[semtech-loramac] Transmission completed\n");
            break;

        case NETDEV_EVENT_TX_TIMEOUT:
            msg.type = MSG_TYPE_TX_TIMEOUT;
            if (msg_send(&msg, semtech_loramac_pid) <= 0) {
                DEBUG("[semtech-loramac] TX timeout, possibly lost interrupt.\n");
            }
            break;

        case NETDEV_EVENT_RX_COMPLETE:
        {
            size_t len;
            len = dev->driver->recv(dev, NULL, 0, 0);
            dev->driver->recv(dev, _semtech_loramac_radio_payload, len, &packet_info);
            semtech_loramac_radio_events.RxDone(_semtech_loramac_radio_payload,
                                                 len,
                                                 packet_info.rssi,
                                                 packet_info.snr);
            break;
        }
        case NETDEV_EVENT_RX_TIMEOUT:
            msg.type = MSG_TYPE_RX_TIMEOUT;
            if (msg_send(&msg, semtech_loramac_pid) <= 0) {
                DEBUG("[semtech-loramac] RX timeout, possibly lost interrupt.\n");
            }
            break;

        case NETDEV_EVENT_CRC_ERROR:
            DEBUG("[semtech-loramac] RX CRC error\n");
            semtech_loramac_radio_events.RxError();
            break;

        case NETDEV_EVENT_FHSS_CHANGE_CHANNEL:
            DEBUG("[semtech-loramac] FHSS channel change\n");
            semtech_loramac_radio_events.FhssChangeChannel(((sx127x_t *)dev)->_internal.last_channel);
            break;

        case NETDEV_EVENT_CAD_DONE:
            DEBUG("[semtech-loramac] test: CAD done\n");
            semtech_loramac_radio_events.CadDone(((sx127x_t *)dev)->_internal.is_last_cad_success);
            break;

        default:
            DEBUG("[semtech-loramac] unexpected netdev event received: %d\n",
                  event);
    }
}

void *_semtech_loramac_event_loop(void *arg)
{
    (void) arg;
    static msg_t _msg_q[SEMTECH_LORAMAC_MSG_QUEUE];
    msg_init_queue(_msg_q, SEMTECH_LORAMAC_MSG_QUEUE);
    LoRaMacPrimitives_t primitives;
    LoRaMacCallback_t callbacks;

    _init_loramac(&primitives, &callbacks);
    semtech_loramac_set_dr(LORAMAC_DEFAULT_DR);
    semtech_loramac_set_adr(LORAMAC_DEFAULT_ADR);
    semtech_loramac_set_public_network(LORAMAC_DEFAULT_PUBLIC_NETWORK);
    semtech_loramac_set_class(LORAMAC_DEFAULT_DEVICE_CLASS);

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        switch (msg.type) {
            case MSG_TYPE_ISR:
            {
                netdev_t *dev = msg.content.ptr;
                dev->driver->isr(dev);
                break;
            }
            case MSG_TYPE_RX_TIMEOUT:
                DEBUG("[semtech-loramac] RX timer timeout\n");
                semtech_loramac_radio_events.RxTimeout();
                break;

            case MSG_TYPE_TX_TIMEOUT:
                DEBUG("[semtech-loramac] TX timer timeout\n");
                semtech_loramac_radio_events.TxTimeout();
                break;

            case MSG_TYPE_MAC_TIMEOUT:
            {
                DEBUG("[semtech-loramac] MAC timer timeout\n");
                void (*callback)(void) = msg.content.ptr;
                callback();
                break;
            }
            case MSG_TYPE_LORAMAC_CMD:
            {
                DEBUG("[semtech-loramac] loramac cmd\n");
                semtech_loramac_call_t *call = msg.content.ptr;
                call->func(call->arg);
                break;
            }
            default:
                DEBUG("[semtech-loramac] Unexpected msg type '%04x'\n", msg.type);
        }
    }
}

int semtech_loramac_init(sx127x_t *dev)
{
    dev->netdev.driver = &sx127x_driver;
    dev->netdev.event_callback = _semtech_loramac_event_cb;

    semtech_loramac_handler_pid = thread_getpid();
    semtech_loramac_pid = thread_create(_semtech_loramac_stack,
                                        sizeof(_semtech_loramac_stack),
                                        THREAD_PRIORITY_MAIN - 1,
                                        THREAD_CREATE_STACKTEST,
                                        _semtech_loramac_event_loop, NULL,
                                        "recv_thread");

    if (semtech_loramac_pid <= KERNEL_PID_UNDEF) {
        DEBUG("Creation of receiver thread failed\n");
        return -1;
    }

    return 0;
}

uint8_t semtech_loramac_join(uint8_t type)
{
    _semtech_loramac_call(_join, &type);

    if (type == LORAMAC_JOIN_OTAA) {
        /* Wait until the OTAA join procedure is complete */
        msg_t msg;
        msg_receive(&msg);
        return (uint8_t)msg.content.value;
    }

    /* ABP join procedure always works */
    return SEMTECH_LORAMAC_JOIN_SUCCEEDED;
}

uint8_t semtech_loramac_send(uint8_t cnf, uint8_t port,
                             uint8_t *tx_buf, uint8_t tx_len,
                             semtech_loramac_rx_data_t *rx_data)
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NETWORK_JOINED;
    LoRaMacMibGetRequestConfirm(&mibReq);

    if (!mibReq.Param.IsNetworkJoined) {
        DEBUG("[semtech-loramac] network is not joined\n");
        return SEMTECH_LORAMAC_NOT_JOINED;
    }

    loramac_send_params_t params;
    params.cnf = cnf;
    params.port = port;
    params.dr = semtech_loramac_get_dr();
    params.payload = tx_buf;
    params.len = tx_len;

    _semtech_loramac_call(_send, &params);

    /* Wait until sending is fully done */
    msg_t msg;
    msg_receive(&msg);
    uint8_t status = (uint8_t)msg.content.value;
    if (status == SEMTECH_LORAMAC_RX_DATA) {
        memcpy(rx_data, &_semtech_loramac_rx_data,
               sizeof(semtech_loramac_rx_data_t));
    }

    return status;
}
