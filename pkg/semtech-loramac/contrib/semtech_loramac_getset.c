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

#include "net/loramac.h"

#include "semtech-loramac/board.h"
#include "LoRaMac.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

extern uint8_t semtech_loramac_dev_eui[LORAMAC_DEVEUI_LEN];
extern uint8_t semtech_loramac_app_eui[LORAMAC_APPEUI_LEN];
extern uint8_t semtech_loramac_app_key[LORAMAC_APPKEY_LEN];
extern uint8_t semtech_loramac_nwk_skey[LORAMAC_NWKSKEY_LEN];
extern uint8_t semtech_loramac_app_skey[LORAMAC_APPSKEY_LEN];
extern uint8_t semtech_loramac_dev_addr[LORAMAC_DEVADDR_LEN];

extern void _loramac_set_rx2_params(uint32_t freq, uint8_t dr);

void semtech_loramac_set_deveui(const uint8_t *eui)
{
    memcpy(semtech_loramac_dev_eui, eui, LORAMAC_DEVEUI_LEN);
}

void semtech_loramac_get_deveui(uint8_t *eui)
{
    memcpy(eui, semtech_loramac_dev_eui, LORAMAC_DEVEUI_LEN);
}

void semtech_loramac_set_appeui(const uint8_t *eui)
{
    memcpy(semtech_loramac_app_eui, eui, LORAMAC_APPEUI_LEN);
}

void semtech_loramac_get_appeui(uint8_t *eui)
{
    memcpy(eui, semtech_loramac_app_eui, LORAMAC_APPEUI_LEN);
}

void semtech_loramac_set_appkey(const uint8_t *key)
{
    memcpy(semtech_loramac_app_key, key, LORAMAC_APPKEY_LEN);
}

void semtech_loramac_get_appkey(uint8_t *key)
{
    memcpy(key, semtech_loramac_app_key, LORAMAC_APPKEY_LEN);
}

void semtech_loramac_set_appskey(const uint8_t *skey)
{
    memcpy(semtech_loramac_app_skey, skey, LORAMAC_APPSKEY_LEN);
}

void semtech_loramac_get_appskey(uint8_t *skey)
{
    memcpy(skey, semtech_loramac_app_skey, LORAMAC_APPSKEY_LEN);
}

void semtech_loramac_set_nwkskey(const uint8_t *skey)
{
    memcpy(semtech_loramac_nwk_skey, skey, LORAMAC_NWKSKEY_LEN);
}

void semtech_loramac_get_nwkskey(uint8_t *skey)
{
    memcpy(skey, semtech_loramac_nwk_skey, LORAMAC_NWKSKEY_LEN);
}

void semtech_loramac_set_devaddr(const uint8_t *addr)
{
    memcpy(semtech_loramac_dev_addr, addr, LORAMAC_DEVADDR_LEN);
}

void semtech_loramac_get_devaddr(uint8_t *addr)
{
    memcpy(addr, semtech_loramac_dev_addr, LORAMAC_DEVADDR_LEN);
}

void semtech_loramac_set_class(loramac_class_t cls)
{
    DEBUG("[semtech-loramac] set class %d\n", cls);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_DEVICE_CLASS;
    mibReq.Param.Class = (DeviceClass_t)cls;
    LoRaMacMibSetRequestConfirm(&mibReq);
}

loramac_class_t semtech_loramac_get_class(void)
{
    DEBUG("[semtech-loramac] get device class\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_DEVICE_CLASS;
    LoRaMacMibGetRequestConfirm(&mibReq);
    return (loramac_class_t)mibReq.Param.Class;
}

void semtech_loramac_set_dr(uint8_t dr)
{
    DEBUG("[semtech-loramac] set dr %d\n", dr);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_DEFAULT_DATARATE;
    mibReq.Param.ChannelsDatarate = dr;
    LoRaMacMibSetRequestConfirm(&mibReq);
}

uint8_t semtech_loramac_get_dr(void)
{
    DEBUG("[semtech-loramac] get dr\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_DEFAULT_DATARATE;
    LoRaMacMibGetRequestConfirm(&mibReq);
    return (uint8_t)mibReq.Param.ChannelsDatarate;
}

void semtech_loramac_set_adr(bool adr)
{
    DEBUG("[semtech-loramac] set adr %d\n", adr);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = adr;
    LoRaMacMibSetRequestConfirm(&mibReq);
}

bool semtech_loramac_get_adr(void)
{
    DEBUG("[semtech-loramac] get adr\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_ADR;
    LoRaMacMibGetRequestConfirm(&mibReq);
    return mibReq.Param.AdrEnable;
}

void semtech_loramac_set_public_network(bool public)
{
    DEBUG("[semtech-loramac] set public network %d\n", public);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_PUBLIC_NETWORK;
    mibReq.Param.EnablePublicNetwork = public;
    LoRaMacMibSetRequestConfirm(&mibReq);
}

bool semtech_loramac_get_public_network(void)
{
    DEBUG("[semtech-loramac] get public network\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_PUBLIC_NETWORK;
    LoRaMacMibGetRequestConfirm(&mibReq);
    return mibReq.Param.EnablePublicNetwork;
}

void semtech_loramac_set_netid(uint32_t netid)
{
    DEBUG("[semtech-loramac] set NetID %lu\n", netid);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NET_ID;
    mibReq.Param.NetID = netid;
    LoRaMacMibSetRequestConfirm(&mibReq);
}

uint32_t semtech_loramac_get_netid(void)
{
    DEBUG("[semtech-loramac] get NetID\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NET_ID;
    LoRaMacMibGetRequestConfirm(&mibReq);
    return mibReq.Param.NetID;
}

void semtech_loramac_set_tx_power(uint8_t power)
{
    DEBUG("[semtech-loramac] set TX power %d\n", power);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_DEFAULT_TX_POWER;
    mibReq.Param.ChannelsDefaultTxPower = power;
    LoRaMacMibSetRequestConfirm(&mibReq);
}

uint8_t semtech_loramac_get_tx_power(void)
{
    DEBUG("[semtech-loramac] get TX power\n");
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_DEFAULT_TX_POWER;
    LoRaMacMibGetRequestConfirm(&mibReq);
    return (uint8_t)mibReq.Param.ChannelsDefaultTxPower;
}

void semtech_loramac_set_rx2_freq(uint8_t freq)
{
    Rx2ChannelParams_t params;
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    LoRaMacMibGetRequestConfirm(&mibReq);
    params.Frequency = freq;
    params.Datarate = mibReq.Param.Rx2DefaultChannel.Datarate;
    _loramac_set_rx2_params(params.Frequency, params.Datarate);
}

uint32_t semtech_loramac_get_rx2_freq(void)
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    LoRaMacMibGetRequestConfirm(&mibReq);
    return mibReq.Param.Rx2DefaultChannel.Frequency;
}

void semtech_loramac_set_rx2_dr(uint8_t dr)
{
    Rx2ChannelParams_t params;
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    LoRaMacMibGetRequestConfirm(&mibReq);
    params.Datarate = dr;
    params.Frequency = mibReq.Param.Rx2DefaultChannel.Frequency;
    _loramac_set_rx2_params(params.Frequency, params.Datarate);
}

uint8_t semtech_loramac_get_rx2_dr(void)
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    LoRaMacMibGetRequestConfirm(&mibReq);
    return mibReq.Param.Rx2DefaultChannel.Datarate;
}
