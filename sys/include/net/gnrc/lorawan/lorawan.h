/*
 * Copyright (C) 2017 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_lorawan   Implementation of GNRC Lorawan
 * @ingroup     net_gnrc
 * @brief       Provides an implementation of lorawan. Based on the original Semtech LoRaMAC implementation.
 * @see         https://github.com/Lora-net/LoRaMac-node
 *              @ref net_gnrc
 *
 * @{
 *
 * @file
 * @brief   GNRC Lorawan specific types and function definitions
 *
 * @author  Francisco Molina <femolina@uc.cl>
 * @author  José Ignacio Alamos <jose.alamos@inria.cl>
 */

#ifndef __LORAMAC_H__
#define __LORAMAC_H__

// Includes board dependent definitions such as channels frequencies
#include "net/gnrc/lorawan/definitions.h"
#include "thread.h"
#include "net/netdev2/lorawan.h"

#define MAC_EVENT_HANDLER_STACK_SIZE 2048
#define BEACON_INTERVAL                             128000 /**< Beacon interval in ms */
#define RECEIVE_DELAY1                              1000 /**< Class A&B receive delay 1 in ms */
#define RECEIVE_DELAY2                              2000 /**< Class A&B receive delay 2 in ms */
#define JOIN_ACCEPT_DELAY1                          5000 /**< Join accept receive delay 1 in ms */
#define JOIN_ACCEPT_DELAY2                          6000 /**< Join accept receive delay 2 in ms */
#define MAX_RX_WINDOW                               3000 /**< Class A&B maximum receive window delay in ms */
#define MAX_FCNT_GAP                                16384 /**< Maximum allowed gap for the FCNT field */
#define ADR_ACK_LIMIT                               64 /**< ADR acknowledgement counter limit */
#define ADR_ACK_DELAY                               32 /**< Number of ADR acknowledgement requests before returning to default datarate */
#define ACK_TIMEOUT                                 2000 /**< Number of seconds after the start of the second reception window without receiving an acknowledge.  AckTimeout = \ref ACK_TIMEOUT + Random( -\ref ACK_TIMEOUT_RND, \ref ACK_TIMEOUT_RND ) */
#define ACK_TIMEOUT_RND                             1000 /**< Random number of seconds after the start of the second reception window without receiving an acknowledge AckTimeout = \ref ACK_TIMEOUT + Random( -\ref ACK_TIMEOUT_RND, \ref ACK_TIMEOUT_RND ) */
#define MAC_STATE_CHECK_TIMEOUT                     1000 /**< Check the Mac layer state every MAC_STATE_CHECK_TIMEOUT in ms */
#define MAX_ACK_RETRIES                             8 /**< Maximum number of times the MAC layer tries to get an acknowledge.  */
#define RSSI_FREE_TH                                ( int8_t )( -90 ) /**< RSSI free threshold [dBm] */
#define UP_LINK                                     0 /**< Frame direction definition for up-link communications */
#define DOWN_LINK                                   1 /**< Frame direction definition for down-link communications */
#define LORAMAC_MFR_LEN                             4 /**< Sets the length of the LoRaMAC footer field.  Mainly indicates the MIC field length */
#define LORA_MAC_PRIVATE_SYNCWORD                   0x12 /**< Syncword for Private LoRa networks */
#define LORA_MAC_PUBLIC_SYNCWORD                    0x34 /**< Syncword for Public LoRa networks */

/*!
 * Timer message types
 */
#define LORAWAN_TIMER_MAC_STATE 100
#define LORAWAN_TIMER_RX_WINDOW1 101
#define LORAWAN_TIMER_RX_WINDOW2 102
#define LORAWAN_TIMER_ACK_TIMEOUT 103
#define LORAWAN_TIMER_TX_DELAYED 104



/*!
 * LoRaMAC band parameters definition
 */
typedef struct sBand
{
    uint16_t DCycle; /**< Duty cycle */
    int8_t TxMaxPower; /*<< Maximum Tx power */
    TimerTime_t LastTxDoneTime; /*<< Time stamp of the last Tx frame */
    TimerTime_t TimeOff; /*<< Holds the time where the device is off */
}Band_t;




/*!
 * LoRaMAC frame types
 *
 * LoRaWAN Specification V1.0.1, chapter 4.2.1, table 1
 */
typedef enum eLoRaMacFrameType
{
    FRAME_TYPE_JOIN_REQ              = 0x00, /*<< LoRaMAC join request frame */
    FRAME_TYPE_JOIN_ACCEPT           = 0x01, /*<< LoRaMAC join accept frame */
    FRAME_TYPE_DATA_UNCONFIRMED_UP   = 0x02, /*<< LoRaMAC unconfirmed up-link frame */
    FRAME_TYPE_DATA_UNCONFIRMED_DOWN = 0x03, /*<< LoRaMAC unconfirmed down-link frame */
    FRAME_TYPE_DATA_CONFIRMED_UP     = 0x04, /*<< LoRaMAC confirmed up-link frame */
    FRAME_TYPE_DATA_CONFIRMED_DOWN   = 0x05, /*<< LoRaMAC confirmed down-link frame */
    FRAME_TYPE_RFU                   = 0x06, /*<< LoRaMAC RFU frame */
    FRAME_TYPE_PROPRIETARY           = 0x07, /*<< LoRaMAC proprietary frame */
}LoRaMacFrameType_t;

/*!
 * LoRaMAC mote MAC commands
 *
 * LoRaWAN Specification V1.0.1, chapter 5, table 4
 */
typedef enum eLoRaMacMoteCmd
{
    MOTE_MAC_LINK_CHECK_REQ          = 0x02, /*<< LinkCheckReq */
    MOTE_MAC_LINK_ADR_ANS            = 0x03, /*<< LinkADRAns */
    MOTE_MAC_DUTY_CYCLE_ANS          = 0x04, /*<< DutyCycleAns */
    MOTE_MAC_RX_PARAM_SETUP_ANS      = 0x05, /*<< RXParamSetupAns */
    MOTE_MAC_DEV_STATUS_ANS          = 0x06, /*<< DevStatusAns */
    MOTE_MAC_NEW_CHANNEL_ANS         = 0x07, /*<< NewChannelAns */
    MOTE_MAC_RX_TIMING_SETUP_ANS     = 0x08, /*<< RXTimingSetupAns */
}LoRaMacMoteCmd_t;

/*!
 * LoRaMAC server MAC commands
 *
 * LoRaWAN Specification V1.0.1 chapter 5, table 4
 */
typedef enum eLoRaMacSrvCmd
{
    SRV_MAC_LINK_CHECK_ANS           = 0x02, /*<< LinkCheckAns */
    SRV_MAC_LINK_ADR_REQ             = 0x03, /*<< LinkADRReq */
    SRV_MAC_DUTY_CYCLE_REQ           = 0x04, /*<< DutyCycleReq */
    SRV_MAC_RX_PARAM_SETUP_REQ       = 0x05, /*<< RXParamSetupReq */
    SRV_MAC_DEV_STATUS_REQ           = 0x06, /*<< DevStatusReq */
    SRV_MAC_NEW_CHANNEL_REQ          = 0x07, /*<< NewChannelReq */
    SRV_MAC_RX_TIMING_SETUP_REQ      = 0x08, /*<< RXTimingSetupReq */
}LoRaMacSrvCmd_t;


/*!
 * LoRaMAC Status
 */
typedef enum eLoRaMacStatus
{
    LORAMAC_STATUS_OK, /*<< Service started successfully */
    LORAMAC_STATUS_BUSY, /*<< Service not started - LoRaMAC is busy */
    LORAMAC_STATUS_SERVICE_UNKNOWN, /*<< Service unknown */
    LORAMAC_STATUS_PARAMETER_INVALID, /*<< Service not started - invalid parameter */
    LORAMAC_STATUS_FREQUENCY_INVALID, /*<< Service not started - invalid frequency */
    LORAMAC_STATUS_DATARATE_INVALID, /*<< Service not started - invalid datarate */
    LORAMAC_STATUS_FREQ_AND_DR_INVALID, /*<< Service not started - invalid frequency and datarate */
    LORAMAC_STATUS_NO_NETWORK_JOINED, /*<< Service not started - the device is not in a LoRaWAN */
    LORAMAC_STATUS_LENGTH_ERROR, /*<< Service not started - playload lenght error */
    LORAMAC_STATUS_MAC_CMD_LENGTH_ERROR, /*<< Service not started - playload lenght error */
    LORAMAC_STATUS_DEVICE_OFF, /*<< Service not started - the device is switched off */
}LoRaMacStatus_t;




/*!
 * \brief   LoRaMAC channel add service
 *
 * \details Adds a new channel to the channel list and activates the id in
 *          the channel mask. For the US915 band, all channels are enabled
 *          by default. It is not possible to activate less than 6 125 kHz
 *          channels.
 *
 * \param   [IN] id - Id of the channel. Possible values are:
 *
 *          0-15 for EU868
 *          0-72 for US915
 *
 * \param   [IN] params - Channel parameters to set.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t channel_add( uint8_t id, ChannelParams_t params );

/*!
 * \brief   LoRaMAC channel remove service
 *
 * \details Deactivates the id in the channel mask.
 *
 * \param   [IN] id - Id of the channel.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t channel_remove( uint8_t id );

/*!
 * \brief   LoRaMAC multicast channel link service
 *
 * \details Links a multicast channel into the linked list.
 *
 * \param   [IN] channelParam - Multicast channel parameters to link.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacMulticastChannelLink(netdev2_lorawan_t *loradev, MulticastParams_t *channelParam );

/*!
 * \brief   LoRaMAC multicast channel unlink service
 *
 * \details Unlinks a multicast channel from the linked list.
 *
 * \param   [IN] channelParam - Multicast channel parameters to unlink.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacMulticastChannelUnlink(netdev2_lorawan_t *loradev, MulticastParams_t *channelParam );

typedef struct netopt_channel_params
{
    uint8_t id;
    ChannelParams_t params;
} netopt_channel_params_t;

kernel_pid_t gnrc_lorawan_init(char *stack, int stacksize, char priority,
                        const char *name, netdev2_lorawan_t *loradev);

/*! \} defgroup LORAMAC */

#endif // __LORAMAC_H__
