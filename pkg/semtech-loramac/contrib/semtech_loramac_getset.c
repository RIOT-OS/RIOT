/*
 * Copyright (C) 2017 Inria
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
 * @brief       Get/Set functions of the public API for Semtech LoRaMAC
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <string.h>
#include <inttypes.h>

#include "mutex.h"

#include "net/loramac.h"

#include "LoRaMac.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void semtech_loramac_set_deveui(semtech_loramac_t *mac, const uint8_t *eui)
{
    memcpy(mac->deveui, eui, LORAMAC_DEVEUI_LEN);
}

void semtech_loramac_get_deveui(const semtech_loramac_t *mac, uint8_t *eui)
{
    memcpy(eui, mac->deveui, LORAMAC_DEVEUI_LEN);
}

void semtech_loramac_set_appeui(semtech_loramac_t *mac, const uint8_t *eui)
{
    memcpy(mac->appeui, eui, LORAMAC_APPEUI_LEN);
}

void semtech_loramac_get_appeui(const semtech_loramac_t *mac, uint8_t *eui)
{
    memcpy(eui, mac->appeui, LORAMAC_APPEUI_LEN);
}

void semtech_loramac_set_appkey(semtech_loramac_t *mac, const uint8_t *key)
{
    memcpy(mac->appkey, key, LORAMAC_APPKEY_LEN);
}

void semtech_loramac_get_appkey(const semtech_loramac_t *mac, uint8_t *key)
{
    memcpy(key, mac->appkey, LORAMAC_APPKEY_LEN);
}

void semtech_loramac_set_appskey(semtech_loramac_t *mac, const uint8_t *skey)
{
    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_APP_SKEY;
    mibReq.Param.AppSKey = (uint8_t *) skey;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

void semtech_loramac_get_appskey(semtech_loramac_t *mac, uint8_t *skey)
{
    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_APP_SKEY;
    LoRaMacMibGetRequestConfirm(&mibReq);
    memcpy(skey, mibReq.Param.AppSKey, LORAMAC_APPSKEY_LEN);
    mutex_unlock(&mac->lock);
}

void semtech_loramac_set_nwkskey(semtech_loramac_t *mac, const uint8_t *skey)
{
    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NWK_SKEY;
    mibReq.Param.NwkSKey = (uint8_t *) skey;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

void semtech_loramac_get_nwkskey(semtech_loramac_t *mac, uint8_t *skey)
{
    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NWK_SKEY;
    LoRaMacMibGetRequestConfirm(&mibReq);
    memcpy(skey, mibReq.Param.NwkSKey, LORAMAC_NWKSKEY_LEN);
    mutex_unlock(&mac->lock);
}

void semtech_loramac_set_devaddr(semtech_loramac_t *mac, const uint8_t *addr)
{
    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_DEV_ADDR;
    mibReq.Param.DevAddr = ((uint32_t)addr[0] << 24 |
                            (uint32_t)addr[1] << 16 |
                            (uint32_t)addr[2] << 8 |
                            (uint32_t)addr[3]);
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

void semtech_loramac_get_devaddr(semtech_loramac_t *mac, uint8_t *addr)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] get device address\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_DEV_ADDR;
    LoRaMacMibGetRequestConfirm(&mibReq);
    addr[0] = (uint8_t)(mibReq.Param.DevAddr >> 24);
    addr[1] = (uint8_t)(mibReq.Param.DevAddr >> 16);
    addr[2] = (uint8_t)(mibReq.Param.DevAddr >> 8);
    addr[3] = (uint8_t)(mibReq.Param.DevAddr);
    mutex_unlock(&mac->lock);
}

void semtech_loramac_set_class(semtech_loramac_t *mac, loramac_class_t cls)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] set class %d\n", cls);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_DEVICE_CLASS;
    mibReq.Param.Class = (DeviceClass_t)cls;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

loramac_class_t semtech_loramac_get_class(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    loramac_class_t cls;
    DEBUG("[semtech-loramac] get device class\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_DEVICE_CLASS;
    LoRaMacMibGetRequestConfirm(&mibReq);
    cls = (loramac_class_t)mibReq.Param.Class;
    mutex_unlock(&mac->lock);
    return cls;
}

void semtech_loramac_set_dr(semtech_loramac_t *mac, uint8_t dr)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] set dr %d\n", dr);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_DATARATE;
    mibReq.Param.ChannelsDatarate = dr;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

uint8_t semtech_loramac_get_dr(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] get dr\n");
    uint8_t datarate;
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_DATARATE;
    LoRaMacMibGetRequestConfirm(&mibReq);
    datarate = (uint8_t)mibReq.Param.ChannelsDatarate;
    mutex_unlock(&mac->lock);
    return datarate;
}

void semtech_loramac_set_adr(semtech_loramac_t *mac, bool adr)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] set adr %d\n", adr);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = adr;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

bool semtech_loramac_get_adr(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    bool enable;
    DEBUG("[semtech-loramac] get adr\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_ADR;
    LoRaMacMibGetRequestConfirm(&mibReq);
    enable = mibReq.Param.AdrEnable;
    mutex_unlock(&mac->lock);
    return enable;
}

void semtech_loramac_set_public_network(semtech_loramac_t *mac, bool public)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] set public network %d\n", public);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_PUBLIC_NETWORK;
    mibReq.Param.EnablePublicNetwork = public;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

bool semtech_loramac_get_public_network(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    bool enable;
    DEBUG("[semtech-loramac] get public network\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_PUBLIC_NETWORK;
    LoRaMacMibGetRequestConfirm(&mibReq);
    enable = mibReq.Param.EnablePublicNetwork;
    mutex_unlock(&mac->lock);
    return enable;
}

void semtech_loramac_set_netid(semtech_loramac_t *mac, uint32_t netid)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] set NetID %" PRIu32 "\n", netid);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NET_ID;
    mibReq.Param.NetID = netid;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

uint32_t semtech_loramac_get_netid(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    uint32_t netid;
    DEBUG("[semtech-loramac] get NetID\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NET_ID;
    LoRaMacMibGetRequestConfirm(&mibReq);
    netid = mibReq.Param.NetID;
    mutex_unlock(&mac->lock);
    return netid;
}

void semtech_loramac_set_tx_power(semtech_loramac_t *mac, uint8_t power)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] set TX power %d\n", power);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_TX_POWER;
    mibReq.Param.ChannelsTxPower = power;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

uint8_t semtech_loramac_get_tx_power(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    uint8_t tx_power;
    DEBUG("[semtech-loramac] get TX power\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_TX_POWER;
    LoRaMacMibGetRequestConfirm(&mibReq);
    tx_power = (uint8_t)mibReq.Param.ChannelsTxPower;
    mutex_unlock(&mac->lock);
    return tx_power;
}

void semtech_loramac_set_tx_port(semtech_loramac_t *mac, uint8_t port)
{
    mac->port = port;
}

uint8_t semtech_loramac_get_tx_port(semtech_loramac_t *mac)
{
    return mac->port;
}

void semtech_loramac_set_tx_mode(semtech_loramac_t *mac, uint8_t mode)
{
    mac->cnf = mode;
}

uint8_t semtech_loramac_get_tx_mode(semtech_loramac_t *mac)
{
    return mac->cnf;
}

void semtech_loramac_set_system_max_rx_error(semtech_loramac_t *mac, uint32_t error)
{
    MibRequestConfirm_t mibReq;
    mutex_lock(&mac->lock);
    mibReq.Type = MIB_SYSTEM_MAX_RX_ERROR;
    mibReq.Param.SystemMaxRxError = error;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

void semtech_loramac_set_min_rx_symbols(semtech_loramac_t *mac, uint8_t min_rx)
{
    MibRequestConfirm_t mibReq;
    mutex_lock(&mac->lock);
    mibReq.Type = MIB_MIN_RX_SYMBOLS;
    mibReq.Param.MinRxSymbols = min_rx;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

static void _semtech_loramac_set_rx2_params(semtech_loramac_channel_params_t params)
{
    Rx2ChannelParams_t p;
    p.Frequency = params.frequency;
    p.Datarate = params.datarate;

    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    mibReq.Param.Rx2DefaultChannel = p;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_RX2_CHANNEL;
    mibReq.Param.Rx2Channel = p;
    LoRaMacMibSetRequestConfirm(&mibReq);
}

void semtech_loramac_set_rx2_freq(semtech_loramac_t *mac, uint32_t freq)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] setting RX2 freq to %" PRIu32" \n", freq);
    Rx2ChannelParams_t p;
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    LoRaMacMibGetRequestConfirm(&mibReq);
    p.Datarate = mibReq.Param.Rx2DefaultChannel.Datarate;
    semtech_loramac_channel_params_t params;
    params.frequency = freq;
    params.datarate = p.Datarate;
    _semtech_loramac_set_rx2_params(params);
    mutex_unlock(&mac->lock);
}

uint32_t semtech_loramac_get_rx2_freq(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    uint32_t freq;
    DEBUG("[semtech-loramac] getting RX2 freq\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    LoRaMacMibGetRequestConfirm(&mibReq);
    freq = mibReq.Param.Rx2DefaultChannel.Frequency;
    mutex_unlock(&mac->lock);
    return freq;
}

void semtech_loramac_set_rx2_dr(semtech_loramac_t *mac, uint8_t dr)
{
    mutex_lock(&mac->lock);
    DEBUG("[semtech-loramac] setting RX2 datarate to %d\n", dr);
    Rx2ChannelParams_t p;
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    LoRaMacMibGetRequestConfirm(&mibReq);
    p.Frequency = mibReq.Param.Rx2DefaultChannel.Frequency;
    semtech_loramac_channel_params_t params;
    params.datarate = dr;
    params.frequency = p.Frequency;
    _semtech_loramac_set_rx2_params(params);
    mutex_unlock(&mac->lock);
}

uint8_t semtech_loramac_get_rx2_dr(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    uint8_t datarate;
    DEBUG("[semtech-loramac] getting RX2 datarate\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    LoRaMacMibGetRequestConfirm(&mibReq);
    datarate = mibReq.Param.Rx2DefaultChannel.Datarate;
    mutex_unlock(&mac->lock);
    return datarate;
}

void semtech_loramac_set_uplink_counter(semtech_loramac_t *mac, uint32_t counter)
{
    DEBUG("[semtech-loramac] reading uplink counter: %" PRIu32 " \n", counter);
    mutex_lock(&mac->lock);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_UPLINK_COUNTER;
    mibReq.Param.UpLinkCounter = counter;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mutex_unlock(&mac->lock);
}

uint32_t semtech_loramac_get_uplink_counter(semtech_loramac_t *mac)
{
    mutex_lock(&mac->lock);
    uint32_t counter;
    DEBUG("[semtech-loramac] getting uplink counter\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_UPLINK_COUNTER;
    LoRaMacMibGetRequestConfirm(&mibReq);
    counter = mibReq.Param.UpLinkCounter;
    mutex_unlock(&mac->lock);
    return counter;
}
