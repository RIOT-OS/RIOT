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
#include "mutex.h"

#include "net/netdev.h"
#include "net/netdev/lora.h"
#include "net/loramac.h"

#include "sx127x.h"
#include "sx127x_params.h"
#include "sx127x_netdev.h"

#include "semtech_loramac.h"
#include "LoRaMac.h"
#include "LoRaMacTest.h"
#include "region/Region.h"

#ifdef MODULE_PERIPH_EEPROM
#include "periph/eeprom.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SEMTECH_LORAMAC_MSG_QUEUE                   (4U)
#define SEMTECH_LORAMAC_LORAMAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
static msg_t _semtech_loramac_msg_queue[SEMTECH_LORAMAC_MSG_QUEUE];
static char _semtech_loramac_stack[SEMTECH_LORAMAC_LORAMAC_STACKSIZE];
kernel_pid_t semtech_loramac_pid;

sx127x_t sx127x;
RadioEvents_t semtech_loramac_radio_events;
LoRaMacPrimitives_t semtech_loramac_primitives;
LoRaMacCallback_t semtech_loramac_callbacks;
extern LoRaMacParams_t LoRaMacParams;

typedef struct {
    uint8_t *payload;
    uint8_t len;
} loramac_send_params_t;

typedef void (*semtech_loramac_func_t)(semtech_loramac_t *, void *);

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
static uint8_t _semtech_loramac_send(semtech_loramac_t *mac,
                                     uint8_t *payload, uint8_t len)
{
    DEBUG("[semtech-loramac] send frame %s\n", payload ? (char *)payload : "(empty)");
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;
    uint8_t dr = semtech_loramac_get_dr(mac);

    if (LoRaMacQueryTxPossible(len, &txInfo) != LORAMAC_STATUS_OK) {
        DEBUG("[semtech-loramac] empty frame in order to flush MAC commands\n");
        /* Send empty frame in order to flush MAC commands */
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = (int8_t)dr;
    }
    else {
        if (mac->cnf == LORAMAC_TX_UNCNF) {
            DEBUG("[semtech-loramac] MCPS request: unconfirmed TX\n");
            mcpsReq.Type = MCPS_UNCONFIRMED;
            mcpsReq.Req.Unconfirmed.fPort = mac->port;
            mcpsReq.Req.Unconfirmed.fBuffer = payload;
            mcpsReq.Req.Unconfirmed.fBufferSize = len;
            mcpsReq.Req.Unconfirmed.Datarate = (int8_t)dr;
        }
        else {
            DEBUG("[semtech-loramac] MCPS request: confirmed TX\n");
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.fPort = mac->port;
            mcpsReq.Req.Confirmed.fBuffer = payload;
            mcpsReq.Req.Confirmed.fBufferSize = len;
            mcpsReq.Req.Confirmed.NbTrials = 3;
            mcpsReq.Req.Confirmed.Datarate = (int8_t)dr;
        }
    }

    uint8_t req_status = LoRaMacMcpsRequest(&mcpsReq);
    uint8_t ret = SEMTECH_LORAMAC_TX_ERROR;
    switch (req_status) {
        case LORAMAC_STATUS_OK:
            DEBUG("[semtech-loramac] MCPS request: OK\n");
            ret = SEMTECH_LORAMAC_TX_OK;
            break;
        case LORAMAC_STATUS_PARAMETER_INVALID:
            DEBUG("[semtech-loramac] MCPS request: invalid parameter\n");
            break;
        case LORAMAC_STATUS_BUSY:
            DEBUG("[semtech-loramac] MCPS request: busy\n");
            ret = SEMTECH_LORAMAC_BUSY;
            break;
        case LORAMAC_STATUS_DUTYCYCLE_RESTRICTED:
            DEBUG("[semtech-loramac] MCPS request: dutycycle restricted\n");
            ret = SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED;
            break;
        default:
            DEBUG("[semtech-loramac] MCPS request: unknown status %d\n",
                  req_status);
            break;
    }

    return ret;
}

/* MCPS-Confirm event function */
static void mcps_confirm(McpsConfirm_t *confirm)
{
    DEBUG("[semtech-loramac] MCPS confirm event\n");
    msg_t msg;
    msg.type = MSG_TYPE_LORAMAC_MCPS_CONFIRM;
    msg.content.ptr = confirm;
    msg_send(&msg, semtech_loramac_pid);
}

/* MCPS-Indication event function */
static void mcps_indication(McpsIndication_t *indication)
{
    DEBUG("[semtech-loramac] MCPS indication event\n");
    msg_t msg;
    msg.type = MSG_TYPE_LORAMAC_MCPS_INDICATION;
    msg.content.ptr = indication;
    msg_send(&msg, semtech_loramac_pid);
}

/* MLME-Confirm event function */
static void mlme_confirm(MlmeConfirm_t *confirm)
{
    DEBUG("[semtech-loramac] MLME confirm event\n");
    msg_t msg;
    msg.type = MSG_TYPE_LORAMAC_MLME_CONFIRM;
    msg.content.ptr = confirm;
    msg_send(&msg, semtech_loramac_pid);
}

/* MLME-Indication event function */
static void mlme_indication(MlmeIndication_t *indication)
{
    DEBUG("[semtech-loramac] MLME indication event\n");
    msg_t msg;
    msg.type = MSG_TYPE_LORAMAC_MLME_INDICATION;
    msg.content.ptr = indication;
    msg_send(&msg, semtech_loramac_pid);
}

#ifdef MODULE_PERIPH_EEPROM
static size_t _read_uint32(size_t pos, uint32_t *value)
{
    uint8_t array[4] = { 0 };
    size_t ret = eeprom_read(pos, array, sizeof(uint32_t));
    *value = ((uint32_t)array[0] << 24 | (uint32_t)array[1] << 16 |
              (uint32_t)array[2] << 8 | array[3]);
    return ret;
}

static size_t _write_uint32(size_t pos, uint32_t value)
{
    uint8_t array[4] = { 0 };
    array[0] = (uint8_t)(value >> 24);
    array[1] = (uint8_t)(value >> 16);
    array[2] = (uint8_t)(value >> 8);
    array[3] = (uint8_t)(value);
    return eeprom_write(pos, array, sizeof(uint32_t));
}



static inline void _set_join_state(semtech_loramac_t *mac, bool joined)
{
    DEBUG("[semtech-loramac] set join state ? %d\n", joined);

    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NETWORK_JOINED;
    mibReq.Param.IsNetworkJoined = joined;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

static inline void _read_loramac_config(semtech_loramac_t *mac)
{
    DEBUG("[semtech-loramac] reading configuration from EEPROM\n");

    uint8_t magic[SEMTECH_LORAMAC_EEPROM_MAGIC_LEN] = {0};
    size_t pos = SEMTECH_LORAMAC_EEPROM_START;

    pos += eeprom_read(pos, magic, SEMTECH_LORAMAC_EEPROM_MAGIC_LEN);
    if (strncmp((char*)magic, "RIOT", SEMTECH_LORAMAC_EEPROM_MAGIC_LEN) != 0) {
        DEBUG("[semtech-loramac] no configuration present on EEPROM "
              "(invalid magic '%s')\n", magic);
        return;
    }

    uint8_t tmp_buf[LORAMAC_APPSKEY_LEN] = { 0 };

    /* LoRaWAN EUIs, Keys and device address */
    pos += eeprom_read(pos, mac->deveui, LORAMAC_DEVEUI_LEN);
    pos += eeprom_read(pos, mac->appeui, LORAMAC_APPEUI_LEN);
    pos += eeprom_read(pos, mac->appkey, LORAMAC_APPKEY_LEN);
    pos += eeprom_read(pos, tmp_buf, LORAMAC_APPSKEY_LEN);
    semtech_loramac_set_appskey(mac, tmp_buf);
    pos += eeprom_read(pos, tmp_buf, LORAMAC_NWKSKEY_LEN);
    semtech_loramac_set_nwkskey(mac, tmp_buf);

    /* Read and set device address */
    uint8_t devaddr[LORAMAC_DEVADDR_LEN];
    pos += eeprom_read(pos, devaddr, LORAMAC_DEVADDR_LEN);
    semtech_loramac_set_devaddr(mac, devaddr);

    /* Read uplink counter */
    uint32_t ul_counter;
    pos += _read_uint32(pos, &ul_counter);
    semtech_loramac_set_uplink_counter(mac, ul_counter);

    /* Read RX2 freq */
    uint32_t rx2_freq;
    pos += _read_uint32(pos, &rx2_freq);
    DEBUG("[semtech-loramac] reading rx2 freq: %" PRIu32 "\n", rx2_freq);
    semtech_loramac_set_rx2_freq(mac, rx2_freq);

    /* Read RX2 datarate */
    uint8_t dr;
    pos += eeprom_read(pos, &dr, 1);
    DEBUG("[semtech-loramac] reading rx2 dr: %d\n", dr);
    semtech_loramac_set_rx2_dr(mac, dr);

    /* Read join state */
    uint8_t joined = eeprom_read_byte(pos);
    _set_join_state(mac, (bool)joined);
}

static inline size_t _save_uplink_counter(semtech_loramac_t *mac)
{
    size_t pos = SEMTECH_LORAMAC_EEPROM_START +
                 SEMTECH_LORAMAC_EEPROM_MAGIC_LEN +
                 LORAMAC_DEVEUI_LEN + LORAMAC_APPEUI_LEN +
                 LORAMAC_APPKEY_LEN + LORAMAC_APPSKEY_LEN +
                 LORAMAC_NWKSKEY_LEN + LORAMAC_DEVADDR_LEN;

    uint32_t ul_counter;
    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_UPLINK_COUNTER;
    LoRaMacMibGetRequestConfirm(&mibReq);
    ul_counter = mibReq.Param.UpLinkCounter;
    mutex_unlock(&mac->lock);
    DEBUG("[semtech-loramac] saving uplink counter: %" PRIu32 " \n", ul_counter);
    return _write_uint32(pos, ul_counter);
}

void semtech_loramac_save_config(semtech_loramac_t *mac)
{
    DEBUG("[semtech-loramac] saving configuration on EEPROM\n");

    uint8_t magic[] = SEMTECH_LORAMAC_EEPROM_MAGIC;
    size_t pos = SEMTECH_LORAMAC_EEPROM_START;
    pos += eeprom_write(pos, magic, SEMTECH_LORAMAC_EEPROM_MAGIC_LEN);

    /* Store EUIs, Keys and device address */
    uint8_t tmp_buf[LORAMAC_APPSKEY_LEN] = { 0 };

    pos += eeprom_write(pos, mac->deveui, LORAMAC_DEVEUI_LEN);
    pos += eeprom_write(pos, mac->appeui, LORAMAC_APPEUI_LEN);
    pos += eeprom_write(pos, mac->appkey, LORAMAC_APPKEY_LEN);

    semtech_loramac_get_appskey(mac, tmp_buf);
    pos += eeprom_write(pos, tmp_buf, LORAMAC_APPSKEY_LEN);
    semtech_loramac_get_nwkskey(mac, tmp_buf);
    pos += eeprom_write(pos, tmp_buf, LORAMAC_NWKSKEY_LEN);

    uint8_t devaddr[LORAMAC_DEVADDR_LEN];
    semtech_loramac_get_devaddr(mac, devaddr);
    pos += eeprom_write(pos, devaddr, LORAMAC_DEVADDR_LEN);

    /* save uplink counter, mainly used for ABP activation */
    pos += _save_uplink_counter(mac);

    /* save RX2 freq */
    uint32_t rx2_freq = semtech_loramac_get_rx2_freq(mac);
    DEBUG("[semtech-loramac] saving rx2 freq: %" PRIu32 "\n", rx2_freq);
    pos += _write_uint32(pos, rx2_freq);

    /* save RX2 dr */
    uint8_t dr = semtech_loramac_get_rx2_dr(mac);
    DEBUG("[semtech-loramac] saving rx2 dr: %d\n", dr);
    pos += eeprom_write(pos, &dr, 1);

    /* save join state */
    uint8_t joined = (uint8_t)semtech_loramac_is_mac_joined(mac);
    eeprom_write_byte(pos, joined);
}

void semtech_loramac_erase_config(void)
{
    DEBUG("[semtech-loramac] erasing configuration on EEPROM\n");

    uint8_t magic[SEMTECH_LORAMAC_EEPROM_MAGIC_LEN];
    size_t pos = SEMTECH_LORAMAC_EEPROM_START;
    eeprom_read(pos, magic, SEMTECH_LORAMAC_EEPROM_MAGIC_LEN);
    if (strcmp((char*)magic, "RIOT") != 0) {
        DEBUG("[semtech-loramac] no configuration present on EEPROM\n");
        return;
    }

    size_t uplink_counter_len = sizeof(uint32_t);
    size_t rx2_freq_len = sizeof(uint32_t);
    size_t rx2_dr_len = sizeof(uint8_t);
    size_t joined_state_len = sizeof(uint8_t);

    size_t end = (pos + SEMTECH_LORAMAC_EEPROM_MAGIC_LEN +
                  LORAMAC_DEVEUI_LEN + LORAMAC_APPEUI_LEN +
                  LORAMAC_APPKEY_LEN + LORAMAC_APPSKEY_LEN +
                  LORAMAC_NWKSKEY_LEN + LORAMAC_DEVADDR_LEN +
                  uplink_counter_len + rx2_freq_len + rx2_dr_len +
                  joined_state_len);
    for (size_t p = pos; p < end; p++) {
        eeprom_write_byte(p, 0);
    }
}
#endif /* MODULE_PERIPH_EEPROM */

void _init_loramac(semtech_loramac_t *mac,
                   LoRaMacPrimitives_t * primitives, LoRaMacCallback_t *callbacks)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] initializing loramac\n");
    primitives->MacMcpsConfirm = mcps_confirm;
    primitives->MacMcpsIndication = mcps_indication;
    primitives->MacMlmeConfirm = mlme_confirm;
    primitives->MacMlmeIndication = mlme_indication;
    LoRaMacInitialization(&semtech_loramac_radio_events, primitives, callbacks,
                          LORAMAC_ACTIVE_REGION);
#ifdef DISABLE_LORAMAC_DUTYCYCLE
    LoRaMacTestSetDutyCycleOn(false);
#endif
    mutex_unlock(&mac->lock);

    semtech_loramac_set_dr(mac, LORAMAC_DEFAULT_DR);
    semtech_loramac_set_adr(mac, LORAMAC_DEFAULT_ADR);
    semtech_loramac_set_public_network(mac, LORAMAC_DEFAULT_PUBLIC_NETWORK);
    semtech_loramac_set_class(mac, LORAMAC_DEFAULT_DEVICE_CLASS);
    semtech_loramac_set_tx_port(mac, LORAMAC_DEFAULT_TX_PORT);
    semtech_loramac_set_tx_mode(mac, LORAMAC_DEFAULT_TX_MODE);
    semtech_loramac_set_system_max_rx_error(mac,
                                            LORAMAC_DEFAULT_SYSTEM_MAX_RX_ERROR);
    semtech_loramac_set_min_rx_symbols(mac, LORAMAC_DEFAULT_MIN_RX_SYMBOLS);
#ifdef MODULE_PERIPH_EEPROM
    _read_loramac_config(mac);
#endif
}

static void _join_otaa(semtech_loramac_t *mac)
{
    DEBUG("[semtech-loramac] starting OTAA join\n");

    uint8_t dr = semtech_loramac_get_dr(mac);

    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NETWORK_JOINED;
    mibReq.Param.IsNetworkJoined = false;
    LoRaMacMibSetRequestConfirm(&mibReq);

    MlmeReq_t mlmeReq;
    mlmeReq.Type = MLME_JOIN;
    mlmeReq.Req.Join.DevEui = mac->deveui;
    mlmeReq.Req.Join.AppEui = mac->appeui;
    mlmeReq.Req.Join.AppKey = mac->appkey;
    mlmeReq.Req.Join.Datarate = dr;
    mutex_unlock(&mac->lock);
    uint8_t ret = LoRaMacMlmeRequest(&mlmeReq);
    switch(ret) {
        case LORAMAC_STATUS_OK:
            /* Let the MAC do his job */
            return;
        case LORAMAC_STATUS_DUTYCYCLE_RESTRICTED:
        {
            DEBUG("[semtech-loramac] join otaa: duty cycle restricted\n");
            /* Cannot join. */
            msg_t msg;
            msg.type = MSG_TYPE_LORAMAC_JOIN_STATUS;
            msg.content.value = SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED;
            msg_send(&msg, semtech_loramac_pid);
            return;
        }
        case LORAMAC_STATUS_BUSY:
        {
            DEBUG("[semtech-loramac] join otaa: mac is busy\n");
            /* Cannot join. */
            msg_t msg;
            msg.type = MSG_TYPE_LORAMAC_JOIN_STATUS;
            msg.content.value = SEMTECH_LORAMAC_BUSY;
            msg_send(&msg, semtech_loramac_pid);
            return;
        }
        default:
        {
            DEBUG("[semtech-loramac] join otaa: failed with status %d\n", ret);
            /* Cannot join. */
            msg_t msg;
            msg.type = MSG_TYPE_LORAMAC_JOIN_STATUS;
            msg.content.value = SEMTECH_LORAMAC_JOIN_FAILED;
            msg_send(&msg, semtech_loramac_pid);
            return;
        }
    }
}

static void _join_abp(semtech_loramac_t *mac)
{
    DEBUG("[semtech-loramac] starting ABP join\n");

    semtech_loramac_set_netid(mac, LORAMAC_DEFAULT_NETID);

    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NETWORK_JOINED;
    mibReq.Param.IsNetworkJoined = true;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);

    /* ABP join procedure always works */
    msg_t msg;
    msg.type = MSG_TYPE_LORAMAC_JOIN_STATUS;
    msg.content.value = SEMTECH_LORAMAC_JOIN_SUCCEEDED;
    msg_send(&msg, semtech_loramac_pid);
}

static void _join(semtech_loramac_t *mac, void *arg)
{
    uint8_t join_type = *(uint8_t *)arg;

    switch (join_type) {
        case LORAMAC_JOIN_OTAA:
            _join_otaa(mac);
            break;

        case LORAMAC_JOIN_ABP:
            _join_abp(mac);
            break;
    }
}

static void _send(semtech_loramac_t *mac, void *arg)
{
    loramac_send_params_t params = *(loramac_send_params_t *)arg;
    uint8_t status = _semtech_loramac_send(mac, params.payload, params.len);
    if (status != SEMTECH_LORAMAC_TX_OK) {
        msg_t msg;
        msg.type = MSG_TYPE_LORAMAC_TX_STATUS;
        msg.content.value = (uint8_t)status;
        msg_send(&msg, semtech_loramac_pid);
    }
}

static void _semtech_loramac_call(semtech_loramac_func_t func, void *arg)
{
    semtech_loramac_call_t call;
    call.func = func;
    call.arg = arg;

    msg_t msg, msg_resp;
    msg.type = MSG_TYPE_LORAMAC_CMD;
    msg.content.ptr = &call;
    msg_send_receive(&msg, &msg_resp, semtech_loramac_pid);
}

static void _semtech_loramac_event_cb(netdev_t *dev, netdev_event_t event)
{
    netdev_lora_rx_info_t packet_info;

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

        case NETDEV_EVENT_RX_STARTED:
            DEBUG("[semtech-loramac] RX started\n");
            break;

        case NETDEV_EVENT_RX_COMPLETE:
        {
            size_t len;
            uint8_t radio_payload[SX127X_RX_BUFFER_SIZE];
            len = dev->driver->recv(dev, NULL, 0, 0);
            dev->driver->recv(dev, radio_payload, len, &packet_info);
            semtech_loramac_radio_events.RxDone(radio_payload,
                                                len, packet_info.rssi,
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
            if(semtech_loramac_radio_events.FhssChangeChannel) {
                semtech_loramac_radio_events.FhssChangeChannel((
                            (sx127x_t *)dev)->_internal.last_channel);
            }
            break;

        case NETDEV_EVENT_CAD_DONE:
            DEBUG("[semtech-loramac] test: CAD done\n");
            if(semtech_loramac_radio_events.CadDone) {
                semtech_loramac_radio_events.CadDone((
                            (sx127x_t *)dev)->_internal.is_last_cad_success);
            }
            break;

        default:
            DEBUG("[semtech-loramac] unexpected netdev event received: %d\n",
                  event);
    }
}

void *_semtech_loramac_event_loop(void *arg)
{
    msg_init_queue(_semtech_loramac_msg_queue, SEMTECH_LORAMAC_MSG_QUEUE);
    semtech_loramac_t *mac = (semtech_loramac_t *)arg;

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == MSG_TYPE_ISR) {
            netdev_t *dev = msg.content.ptr;
            dev->driver->isr(dev);
        }
        else {
            switch (msg.type) {
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
                    msg_t msg_resp;
                    DEBUG("[semtech-loramac] loramac cmd msg\n");
                    semtech_loramac_call_t *call = msg.content.ptr;
                    call->func(mac, call->arg);
                    msg_reply(&msg, &msg_resp);
                    break;
                }
                case MSG_TYPE_LORAMAC_JOIN_STATUS:
                {
                    DEBUG("[semtech-loramac] join status msg received\n");
                    msg_t msg_ret;
                    msg_ret.content.value = msg.content.value;
                    msg_send(&msg_ret, mac->tx_pid);
                    break;
                }
                case MSG_TYPE_LORAMAC_TX_STATUS:
                {
                    DEBUG("[semtech-loramac] TX status msg received\n");
                    msg_t msg_ret;
                    msg_ret.content.value = msg.content.value;
                    msg_send(&msg_ret, mac->tx_pid);
                    break;
                }
                case MSG_TYPE_LORAMAC_MLME_CONFIRM:
                {
                    DEBUG("[semtech-loramac] MLME confirm msg received\n");
                    MlmeConfirm_t *confirm = (MlmeConfirm_t *)msg.content.ptr;
                    switch (confirm->MlmeRequest) {
                        case MLME_JOIN:
                        {
                            msg_t msg_ret;
                            msg_ret.type = MSG_TYPE_LORAMAC_JOIN_STATUS;
                            if (confirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                                /* Status is OK, node has joined the network */
                                DEBUG("[semtech-loramac] join succeeded\n");
                                msg_ret.content.value = SEMTECH_LORAMAC_JOIN_SUCCEEDED;
                                /* Set RX2 DR parameters in the MIB from MAC params */
                                semtech_loramac_set_rx2_dr(mac, LoRaMacParams.Rx2Channel.Datarate);
                            }
                            else {
                                DEBUG("[semtech-loramac] join not successful\n");
                                /* Join was not successful. */
                                msg_ret.content.value = SEMTECH_LORAMAC_JOIN_FAILED;
                            }
                            msg_send(&msg_ret, mac->tx_pid);
                            break;
                        }
#ifdef MODULE_SEMTECH_LORAMAC_RX
                        case MLME_LINK_CHECK:
                            if (confirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                                mac->link_chk.demod_margin = confirm->DemodMargin;
                                mac->link_chk.nb_gateways = confirm->NbGateways;
                                DEBUG("[semtech-loramac] link check info received:\n"
                                      "  - Demodulation margin: %d\n"
                                      "  - Number of gateways: %d\n",
                                      mac->link_chk.demod_margin,
                                      mac->link_chk.nb_gateways);
                                msg_t msg_ret;
                                msg_ret.content.value = SEMTECH_LORAMAC_RX_LINK_CHECK;
                                msg_send(&msg_ret, mac->rx_pid);
                            }
#endif
                        default:
                            break;
                    }

                    break;
                }
                case MSG_TYPE_LORAMAC_MLME_INDICATION:
                {
                    DEBUG("[semtech-loramac] MLME indication msg received\n");
                    MlmeIndication_t *indication = (MlmeIndication_t *)msg.content.ptr;
                    if (indication->MlmeIndication == MLME_SCHEDULE_UPLINK) {
                        DEBUG("[semtech-loramac] MLME indication: schedule an uplink\n");
                        uint8_t prev_port = mac->port;
                        mac->port = 0;
                        _semtech_loramac_send(mac, NULL, 0);
                        mac->port = prev_port;
                    }
                    break;
                }
                case MSG_TYPE_LORAMAC_MCPS_CONFIRM:
                {
                    DEBUG("[semtech-loramac] MCPS confirm msg received\n");
                    McpsConfirm_t *confirm = (McpsConfirm_t *)msg.content.ptr;
                    msg_t msg_ret;
                    msg_ret.type = MSG_TYPE_LORAMAC_TX_STATUS;
                    uint8_t status = SEMTECH_LORAMAC_TX_CNF_FAILED;
                    if (confirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                        DEBUG("[semtech-loramac] MCPS confirm event OK\n");
                        status = SEMTECH_LORAMAC_TX_DONE;
#ifdef MODULE_PERIPH_EEPROM
                        /* save the uplink counter */
                        _save_uplink_counter(mac);
#endif
                        if (ENABLE_DEBUG) {
                            switch (confirm->McpsRequest) {
                                case MCPS_UNCONFIRMED:
                                {
                                    /* Check Datarate
                                       Check TxPower */
                                    DEBUG("[semtech-loramac] MCPS confirm event: UNCONFIRMED\n");
                                    break;
                                }

                                case MCPS_CONFIRMED:
                                    /* Check Datarate
                                       Check TxPower
                                       Check AckReceived
                                       Check NbTrials */
                                    DEBUG("[semtech-loramac] MCPS confirm event: CONFIRMED\n");
                                    break;

                                case MCPS_PROPRIETARY:
                                    DEBUG("[semtech-loramac] MCPS confirm event: PROPRIETARY\n");
                                    break;

                                default:
                                    DEBUG("[semtech-loramac] MCPS confirm event: UNKNOWN\n");
                                    break;
                            }
                        }
                    }

                    DEBUG("[semtech-loramac] forward TX status to sender thread\n");
                    msg_ret.content.value = status;
                    msg_send(&msg_ret, mac->tx_pid);
                    break;
                }
                case MSG_TYPE_LORAMAC_MCPS_INDICATION:
                {
                    DEBUG("[semtech-loramac] MCPS indication msg received\n");
                    McpsIndication_t *indication = (McpsIndication_t *)msg.content.ptr;
                    if (indication->Status != LORAMAC_EVENT_INFO_STATUS_OK) {
                        DEBUG("[semtech-loramac] MCPS indication no OK\n");
                        break;
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

                    /* Check Multicast
                       Check Port
                       Check Datarate
                       Check FramePending */
                    if (indication->FramePending) {
                        /* The server signals that it has pending data to be sent.
                           We schedule an uplink as soon as possible to flush the server. */
                        DEBUG("[semtech-loramac] MCPS indication: pending data, schedule an "
                              "uplink\n");
                        uint8_t prev_port = mac->port;
                        mac->port = 0;
                        _semtech_loramac_send(mac, NULL, 0);
                        mac->port = prev_port;
                    }
#ifdef MODULE_SEMTECH_LORAMAC_RX
                    if (indication->RxData) {
                        DEBUG("[semtech-loramac] MCPS indication: data received\n");
                        memcpy(mac->rx_data.payload,
                               indication->Buffer, indication->BufferSize);
                        mac->rx_data.payload_len = indication->BufferSize;
                        mac->rx_data.port = indication->Port;
                        DEBUG("[semtech-loramac] loramac RX data:\n"
                              "  - Payload: %s\n"
                              "  - Size: %d\n"
                              "  - Port: %d\n",
                              (char *)mac->rx_data.payload,
                              mac->rx_data.payload_len,
                              mac->rx_data.port);
                        msg_t msg_ret;
                        msg_ret.content.value = SEMTECH_LORAMAC_RX_DATA;
                        msg_send(&msg_ret, mac->rx_pid);
                    }
                    if (indication->AckReceived) {
                        DEBUG("[semtech-loramac] MCPS indication: ACK received\n");
                        msg_t msg_ret;
                        msg_ret.content.value = SEMTECH_LORAMAC_RX_CONFIRMED;
                        msg_send(&msg_ret, mac->rx_pid);
                    }
#endif

                    break;
                }
                default:
                    DEBUG("[semtech-loramac] Unexpected msg type '%04x'\n",
                          msg.type);
            }
        }
    }
}

int semtech_loramac_init(semtech_loramac_t *mac)
{
    sx127x_setup(&sx127x, &sx127x_params[0]);
    sx127x.netdev.driver = &sx127x_driver;
    sx127x.netdev.event_callback = _semtech_loramac_event_cb;

    semtech_loramac_pid = thread_create(_semtech_loramac_stack,
                                        sizeof(_semtech_loramac_stack),
                                        THREAD_PRIORITY_MAIN - 1,
                                        THREAD_CREATE_STACKTEST,
                                        _semtech_loramac_event_loop, mac,
                                        "recv_thread");

    if (semtech_loramac_pid <= KERNEL_PID_UNDEF) {
        DEBUG("Creation of receiver thread failed\n");
        return -1;
    }

    _init_loramac(mac, &semtech_loramac_primitives, &semtech_loramac_callbacks);

    return 0;
}

bool semtech_loramac_is_mac_joined(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NETWORK_JOINED;
    LoRaMacMibGetRequestConfirm(&mibReq);
    bool is_joined = mibReq.Param.IsNetworkJoined;
    mutex_unlock(&mac->lock);

    return is_joined;
}

uint8_t semtech_loramac_join(semtech_loramac_t *mac, uint8_t type)
{
    DEBUG("[semtech-loramac] Starting join procedure: %d\n", type);

    if (semtech_loramac_is_mac_joined(mac)) {
        DEBUG("[semtech-loramac] network is already joined\n");
        return SEMTECH_LORAMAC_ALREADY_JOINED;
    }

    mac->tx_pid = thread_getpid();

    _semtech_loramac_call(_join, &type);

    /* Wait until the MAC replies about join procedure */
    msg_t msg;
    msg_receive(&msg);

    return (uint8_t)msg.content.value;
}

#ifdef MODULE_SEMTECH_LORAMAC_RX
void semtech_loramac_request_link_check(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    MlmeReq_t mlmeReq;
    mlmeReq.Type = MLME_LINK_CHECK;
    LoRaMacMlmeRequest(&mlmeReq);
    mutex_unlock(&mac->lock);
}
#endif

uint8_t semtech_loramac_send(semtech_loramac_t *mac, uint8_t *data, uint8_t len)
{
    if (!semtech_loramac_is_mac_joined(mac)) {
        DEBUG("[semtech-loramac] network is not joined\n");
        return SEMTECH_LORAMAC_NOT_JOINED;
    }

    /* Correctly set the sender thread pid */
    mac->tx_pid = thread_getpid();

    loramac_send_params_t params;
    params.payload = data;
    params.len = len;

    _semtech_loramac_call(_send, &params);

    /* Wait for TX status information from the MAC */
    msg_t msg;
    msg_receive(&msg);
    return (uint8_t)msg.content.value;
}

#ifdef MODULE_SEMTECH_LORAMAC_RX
uint8_t semtech_loramac_recv(semtech_loramac_t *mac)
{
    /* Correctly set the receiver thread pid */
    mac->rx_pid = thread_getpid();

    /* Wait until the mac receive some information */
    msg_t msg;
    msg_receive(&msg);
    DEBUG("[semtech-loramac] received something\n");

    return (uint8_t)msg.content.value;
}
#endif
