/*
 * Copyright (C) Fundación Inria Chile 2017
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_netdev2_eth
 * @{
 *
 * @file
 * @brief       Common code for LoRa(WAN) drivers
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 *
 * @}
 */

#include <assert.h>
#include <string.h>
#include <errno.h>
#include "net/gnrc/lorawan/lorawan.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

extern LoRaMacStatus_t join_request(void);
extern LoRaMacStatus_t link_check(void);
extern void lorawan_set_class(netdev2_lorawan_t *dev, int class);
extern void _set_channel_mask(uint16_t *mask);
extern bool _value_in_range( int8_t value, int8_t min, int8_t max );

int netdev2_lorawan_get(netdev2_lorawan_t *dev, netopt_t opt, void *value, size_t max_len)
{
    switch(opt)
    {
        case NETOPT_LORAWAN_JOIN:
            *((uint8_t*) value) = join_request();
            break;
        case NETOPT_LORAWAN_LINK_CHECK:
            *((uint8_t*) value) = link_check();
            break;
        default:
            break;
    }
    return 0;
}

int netdev2_lorawan_set(netdev2_lorawan_t *dev, netopt_t opt, void *value, size_t value_len)
{
    netdev2_t *netdev = (netdev2_t*) dev;
    uint8_t sw;
    uint8_t tmp;
    netopt_channel_params_t *cp;
    switch (opt) {
        case NETOPT_ADDRESS:
        {
            assert(value_len == sizeof(uint32_t));
            uint32_t dev_addr = *((uint32_t *)value);
            assert(dev_addr <= UINT32_MAX);
            dev->lorawan.dev_addr = dev_addr;
            return sizeof(uint16_t);
        }
        case NETOPT_LORAWAN_NWK_SKEY:
        {
            assert(value_len == KEY_SIZE);
            memcpy(dev->lorawan.nwk_skey, value, value_len);
            return sizeof(uint16_t);
        }
        case NETOPT_LORAWAN_APP_SKEY:
        {
            assert(value_len == KEY_SIZE);
            memcpy(dev->lorawan.app_skey, value, value_len);
            return sizeof(uint16_t);
        }
        case NETOPT_LORAWAN_NET_ID:
        {
            assert(value_len == sizeof(uint32_t));
            uint32_t net_id = *((uint32_t *)value);
            assert(net_id <= UINT32_MAX);
            dev->lorawan.net_id  = net_id ;
            return sizeof(uint16_t);
        }
        case NETOPT_LORAWAN_ADR:
            dev->lorawan.tx_rx.adr_ctrl = *(uint8_t *) value;
        case NETOPT_LORAWAN_DEV_EUI:
            dev->dev_eui = (uint8_t*) value;
            return sizeof(uint8_t*);
        case NETOPT_LORAWAN_APP_EUI:
            dev->app_eui = (uint8_t*) value;
            return sizeof(uint8_t*);
        case NETOPT_LORAWAN_APP_KEY:
            dev->app_key = (uint8_t*) value;
            return sizeof(uint8_t*);
        case NETOPT_LORAWAN_CLASS:
            lorawan_set_class(dev, *((int*) value));
            return sizeof(int);
        case NETOPT_LORAWAN_PUBLIC:
            sw = *((bool*) value) ? 0x34 : 0x12;
            netdev->driver->set(netdev, NETOPT_LORA_SYNCWORD, &sw, sizeof(uint8_t));
            return sizeof(bool);
        case NETOPT_LORAWAN_DUTY_CYCLE:
            dev->DutyCycleOn = *((bool*) value);
            return sizeof(bool);
        case NETOPT_LORAWAN_RX2_PARAMS:
            dev->LoRaMacParams.Rx2Channel = *((Rx2ChannelParams_t*) value);
            return sizeof(Rx2ChannelParams_t);
        case NETOPT_LORAWAN_CHANNEL_MASK:
            _set_channel_mask((uint16_t*) value); 
            return sizeof(uint16_t*);
        case NETOPT_LORAWAN_NB_REP:
            tmp = *((uint8_t*) value);
            //TODO: Add error
            if( ( tmp >= 1 ) &&
                ( tmp <= 15 ) )
            {
                dev->LoRaMacParams.ChannelsNbRep = tmp;
            }
            return sizeof(uint8_t);
        case NETOPT_LORAWAN_MAX_RX_WINDOW:

            dev->LoRaMacParams.MaxRxWindow = *((uint32_t*) value);
            return sizeof(uint32_t);
        case NETOPT_LORAWAN_RX_DELAY_1:
            dev->LoRaMacParams.ReceiveDelay1 = *((uint32_t*) value);
            return sizeof(uint32_t);
        case NETOPT_LORAWAN_RX_DELAY_2:
            dev->LoRaMacParams.ReceiveDelay2 = *((uint32_t*) value);
            return sizeof(uint32_t);
        case NETOPT_LORAWAN_JOIN_ACCEPT_DELAY1:
            dev->LoRaMacParams.JoinAcceptDelay1 = *((uint32_t*) value);
            return sizeof(uint32_t);
        case NETOPT_LORAWAN_JOIN_ACCEPT_DELAY2:
            dev->LoRaMacParams.JoinAcceptDelay2 = *((uint32_t*) value);
            return sizeof(uint32_t);
        case NETOPT_LORAWAN_CHANNELS_DATARATE:
            tmp = *((int8_t*) value);
            if( _value_in_range( tmp, LORAMAC_TX_MIN_DATARATE, LORAMAC_TX_MAX_DATARATE ) )
            {
                dev->LoRaMacParams.ChannelsDatarate = tmp;
            }
            return sizeof(int8_t);
        case NETOPT_LORAWAN_CHANNELS_TX_POWER:
            tmp = *((int8_t*) value);
            if( _value_in_range(tmp, LORAMAC_MAX_TX_POWER, LORAMAC_MIN_TX_POWER ) )
            {
                dev->LoRaMacParams.ChannelsTxPower = tmp;
            }
            return sizeof(int8_t);
        case NETOPT_LORAWAN_CHANNEL:
            cp = (netopt_channel_params_t*) value; 
            channel_add( cp->id, cp->params );
            return sizeof(netopt_channel_params_t);
        case NETOPT_LORAWAN_TX_RETRIES:
            dev->ack_timeout_retries = *((uint8_t*) value);
            return sizeof(uint8_t);
        default:
            break;
    }
    return 0;
}
