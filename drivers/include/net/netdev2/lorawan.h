/*
 * Copyright (C) Fundación Inria Chile 2017
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     drivers_netdev_netdev2
 * @{ *
 * @file
 * @brief       Definitions for netdev2 common lorawan code
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 */

#ifndef NETDEV2_ETH_H
#define NETDEV2_ETH_H

#include <stdint.h>
#include <stdbool.h>

#include "net/netdev2.h"
#include "net/netopt.h"
#include "net/gnrc/lorawan/timer.h"
#include "net/gnrc/lorawan/definitions.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_SIZE		16	

#define LORAMAC_PHY_MAXPAYLOAD                      255
#define LORA_MAC_COMMAND_MAX_LENGTH                 15

#define GNRC_LORAWAN_JOIN_REQUEST_MSG 1001
#define GNRC_LORAWAN_LINK_CHECK_MSG 1002

/*!
 * LoRaMAC channels parameters definition
 */
typedef union uDrRange
{
    int8_t Value;
    struct sFields
    {
        int8_t Min : 4;
        int8_t Max : 4;
    }Fields;
}DrRange_t;

typedef struct sChannelParams
{
    uint32_t Frequency;
    DrRange_t DrRange;
    uint8_t Band;
}ChannelParams_t;


typedef struct lorawan_tx_rx_config
{
	bool adr_ctrl;                            /* LoRaMac ADR control status*/
	bool nwk_status;                       /* Boolean if network is joined*/
}lorawan_tx_rx_config_t;

typedef struct lorawan_sesion
{
	uint8_t nwk_skey[KEY_SIZE];        		   /* AES encryption/decryption cipher network session key */
	uint8_t app_skey[KEY_SIZE];                /* AES encryption/decryption cipher application session key */
	uint32_t dev_addr;                 		   /* Mote Address */
	uint32_t net_id;              			   /* Network ID ( 3 bytes ) */	
	bool public;                               /* Indicates if the node is connected to a private or public network */
	bool RepeaterSupport;                      /* Indicates if the node supports repeaters*/
	//LoRaMacPrimitives_t *LoRaMacPrimitives;  /* LoRaMac upper layer event functions*/
	//LoRaMacCallback_t *LoRaMacCallbacks;     /* LoRaMac upper layer callback functions*/
	lorawan_tx_rx_config_t tx_rx;              /* Transreception session info*/
}lorawan_sesion_t;

typedef struct sMulticastParams
{
    uint32_t Address;
    uint8_t NwkSKey[16];
    uint8_t AppSKey[16];
    uint32_t DownLinkCounter;
    struct sMulticastParams *Next;
}MulticastParams_t;

typedef enum eDeviceClass
{
    CLASS_A,
    CLASS_B,
    CLASS_C,
}DeviceClass_t;

typedef struct sRx2ChannelParams
{
    uint32_t Frequency;
    uint8_t  Datarate;
}Rx2ChannelParams_t;

typedef struct sLoRaMacParams
{
    int8_t ChannelsTxPower;
    int8_t ChannelsDatarate;
    uint32_t MaxRxWindow;
    uint32_t ReceiveDelay1;
    uint32_t ReceiveDelay2;
    uint32_t JoinAcceptDelay1;
    uint32_t JoinAcceptDelay2;
    uint8_t ChannelsNbRep;
    uint8_t Rx1DrOffset;
    Rx2ChannelParams_t Rx2Channel;
    uint16_t ChannelsMask[6];
}LoRaMacParams_t;


typedef enum eMcps
{
    MCPS_UNCONFIRMED,
    MCPS_CONFIRMED,
    MCPS_MULTICAST,
    MCPS_PROPRIETARY,
}Mcps_t;

typedef struct {
    netdev2_t netdev;                       /**< @ref netdev2_t base class */
    lorawan_sesion_t lorawan;
    uint8_t *dev_eui;
    uint8_t *app_key;
    uint8_t *app_eui;
    uint16_t dev_nonce;
    MulticastParams_t *MulticastChannels;
    DeviceClass_t LoRaMacDeviceClass;
    bool RepeaterSupport;
    uint8_t LoRaMacPayload[LORAMAC_PHY_MAXPAYLOAD];
    uint32_t uplink_counter;
    uint32_t DownLinkCounter;
    bool IsUpLinkCounterFixed;
    bool IsRxWindowsEnabled;
    uint32_t AdrAckCounter;
    bool NodeAckRequested;
    bool SrvAckRequested;
    bool MacCommandsInNextTx;
    uint8_t MacCommandsBufferIndex;
    uint8_t MacCommandsBufferToRepeatIndex;
    uint8_t MacCommandsBuffer[LORA_MAC_COMMAND_MAX_LENGTH];
    uint8_t MacCommandsBufferToRepeat[LORA_MAC_COMMAND_MAX_LENGTH];
    uint16_t ChannelsMaskRemaining[6];
    LoRaMacParams_t LoRaMacParams;
    LoRaMacParams_t LoRaMacParamsDefaults;
    uint8_t ChannelsNbRepCounter;
    uint8_t MaxDCycle;
    uint16_t AggregatedDCycle;
    TimerTime_t AggregatedLastTxDoneTime;
    TimerTime_t AggregatedTimeOff;
    bool DutyCycleOn;
    uint8_t Channel;
    uint8_t LastTxChannel;
    TimerEvent_t TxDelayedTimer;
    TimerEvent_t RxWindowTimer1;
    TimerEvent_t RxWindowTimer2;
    uint32_t RxWindow1Delay;
    uint32_t RxWindow2Delay;
    TimerEvent_t AckTimeoutTimer;
    uint8_t ack_timeout_retries; /**< Current number of ACK attempts */
    uint8_t ack_timeout_retries_counter; /**< Number of ACK retries */
    TimerTime_t TxTimeOnAir;
    uint16_t join_request_trials; /**< Attempts of join requests */
    uint8_t rx_slot; /**< Current RX slot */

    uint8_t demod_margin;
    uint8_t number_of_gateways;

    bool join_req_sent; /**< A join request packet was sent */
    //TODO: Port to be removed...
    uint8_t port; 
    uint8_t *Buffer;
    uint8_t BufferSize;
    bool RxData;
    //Frame variables
    bool b_tx;
    bool b_rx;
    bool link_check;
    bool join_req;
    gnrc_pktsnip_t *pkt;
} netdev2_lorawan_t;

/**
 * @brief   Fallback function for netdev2 lorawan devices' _get function
 *
 * Supposed to be used by netdev2 drivers as default case.
 *
 * @warning Driver *MUST* implement NETOPT_ADDRESS case!
 *
 * @param[in]   dev     network device descriptor
 * @param[in]   opt     option type
 * @param[out]  value   pointer to store the option's value in
 * @param[in]   max_len maximal amount of byte that fit into @p value
 *
 * @return              number of bytes written to @p value
 * @return              <0 on error
 */
int netdev2_lorawan_get(netdev2_lorawan_t *dev, netopt_t opt, void *value, size_t max_len);

/**
 * @brief   Fallback function for netdev2 ethernet devices' _set function
 *
 * @param[in] dev       network device descriptor
 * @param[in] opt       option type
 * @param[in] value     value to set
 * @param[in] value_len the length of @p value
 *
 * @return              number of bytes used from @p value
 * @return              <0 on error
 */
int netdev2_lorawan_set(netdev2_lorawan_t *dev, netopt_t opt, void *value, size_t value_len);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NETDEV2_ETH_H */
