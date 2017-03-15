/*
 * Copyright (C) 2017 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of GNRC Lorawan
 *
 * @author      Francisco Molina <femolina@uc.cl>
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @}
 */

/* Based on the original Semtech LoRaMAC implementation */

#include "net/gnrc/lorawan/crypto.h"
#include "net/gnrc/lorawan/lorawan.h"
#include "thread.h"
#include "random.h"
#include "net/lorawan/hdr.h"
#include <string.h>
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/netdev2.h"

/*!
 * \brief Returns the minimum value betwen a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval minValue Minimum value
 */
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )

/*!
 * \brief Returns the maximum value betwen a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval maxValue Maximum value
 */
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )

/*!
 * \brief Returns 2 raised to the power of n
 *
 * \param [IN] n power value
 * \retval result of raising 2 to the power n
 */
#define POW2( n ) ( 1 << n )

/*!
 * Radio wakeup time from SLEEP mode
 */
#define RADIO_OSC_STARTUP                           1 // [ms]

/*!
 * Radio PLL lock and Mode Ready delay which can vary with the temperature
 */
#define RADIO_SLEEP_TO_RX                           2 // [ms]

/*!
 * Radio complete Wake-up Time with margin for temperature compensation
 */
#define RADIO_WAKEUP_TIME ( RADIO_OSC_STARTUP + RADIO_SLEEP_TO_RX )


/*!
 * FRMPayload overhead to be used when setting the Radio.SetMaxPayloadLength
 * in RxWindowSetup function.
 * Maximum PHYPayload = MaxPayloadOfDatarate/MaxPayloadOfDatarateRepeater + LORA_MAC_FRMPAYLOAD_OVERHEAD
 */
#define LORA_MAC_FRMPAYLOAD_OVERHEAD                13 // MHDR(1) + FHDR(7) + Port(1) + MIC(4)

#define LORAWAN_TX_TIMEOUT_EVENT 10
#define LORAWAN_RX_TIMEOUT_EVENT 11
#define LORAWAN_CRC_ERROR 12
#define LORAWAN_FHSS_CHANGE_CHANNEL 13
#define LORAWAN_CAD_DONE 14

#define GNRC_LORA_MSG_QUEUE 16
/*!
 * Device IEEE EUI
 */
/*!
 * Radio driver supported modems
 */
typedef enum
{
    MODEM_FSK = 0,
    MODEM_LORA,
}RadioModems_t;

void _finish_rx(netdev2_lorawan_t *dev);
int gnrc_lorawan_send_join_request(netdev2_lorawan_t *loradev);
void on_rx2_timeout(netdev2_lorawan_t *loradev);
void on_ack_timeout(netdev2_lorawan_t *loradev);

#if defined( USE_BAND_433 )
/*!
 * Data rates table definition
 */
static const uint8_t lorawan_datarates[]  = { 12, 11, 10,  9,  8,  7,  7, 50 };

/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 */
static const uint8_t lorawan_max_payload_of_datarate[] = { 59, 59, 59, 123, 250, 250, 250, 250 };

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */
static const uint8_t lorawan_max_payload_of_datarate_repeater[] = { 59, 59, 59, 123, 230, 230, 230, 230 };

/*!
 * Tx output powers table definition
 */
static const int8_t lorawan_tx_power[]    = { 20, 14, 11,  8,  5,  2 };

/*!
 * LoRaMac bands
 */
static Band_t bands[LORA_MAX_NB_BANDS] =
{
    BAND0,
};

/*!
 * LoRaMAC channels
 */
static ChannelParams_t channels[LORA_MAX_NB_CHANNELS] =
{
    LC1,
    LC2,
    LC3,
};
#elif defined( USE_BAND_780 )
/*!
 * Data rates table definition
 */
static const uint8_t lorawan_datarates[]  = { 12, 11, 10,  9,  8,  7,  7, 50 };

/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 */
static const uint8_t lorawan_max_payload_of_datarate[] = { 59, 59, 59, 123, 250, 250, 250, 250 };

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */
static const uint8_t lorawan_max_payload_of_datarate_repeater[] = { 59, 59, 59, 123, 230, 230, 230, 230 };

/*!
 * Tx output powers table definition
 */
static const int8_t lorawan_tx_power[]    = { 20, 14, 11,  8,  5,  2 };

/*!
 * LoRaMac bands
 */
static Band_t bands[LORA_MAX_NB_BANDS] =
{
    BAND0,
};

/*!
 * LoRaMAC channels
 */
static ChannelParams_t channels[LORA_MAX_NB_CHANNELS] =
{
    LC1,
    LC2,
    LC3,
};
#elif defined( USE_BAND_868 )
/*!
 * Data rates table definition
 */
static const uint8_t lorawan_datarates[]  = { 12, 11, 10,  9,  8,  7,  7, 50 };

/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 */
static const uint8_t lorawan_max_payload_of_datarate[] = { 51, 51, 51, 115, 242, 242, 242, 242 };

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */
static const uint8_t lorawan_max_payload_of_datarate_repeater[] = { 51, 51, 51, 115, 222, 222, 222, 222 };

/*!
 * Tx output powers table definition
 */
static const int8_t lorawan_tx_power[]    = { 20, 14, 11,  8,  5,  2 };

/*!
 * LoRaMac bands
 */
static Band_t bands[LORA_MAX_NB_BANDS] =
{
    BAND0,
    BAND1,
    BAND2,
    BAND3,
    BAND4,
};

/*!
 * LoRaMAC channels
 */
static ChannelParams_t channels[LORA_MAX_NB_CHANNELS] =
{
    LC1,
    LC2,
    LC3,
};
#elif defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
/*!
 * Data rates table definition
 */
static const uint8_t lorawan_datarates[]  = { 10, 9, 8,  7,  8,  0,  0, 0, 12, 11, 10, 9, 8, 7, 0, 0 };

/*!
 * Up/Down link data rates offset definition
 */
static const int8_t datarate_offsets[16][4] =
{
    { DR_10, DR_9 , DR_8 , DR_8  }, // DR_0
    { DR_11, DR_10, DR_9 , DR_8  }, // DR_1
    { DR_12, DR_11, DR_10, DR_9  }, // DR_2
    { DR_13, DR_12, DR_11, DR_10 }, // DR_3
    { DR_13, DR_13, DR_12, DR_11 }, // DR_4
};

/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 */
static const uint8_t lorawan_max_payload_of_datarate[] = { 11, 53, 125, 242, 242, 0, 0, 0, 53, 129, 242, 242, 242, 242, 0, 0 };

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */
static const uint8_t lorawan_max_payload_of_datarate_repeater[] = { 11, 53, 125, 242, 242, 0, 0, 0, 33, 109, 222, 222, 222, 222, 0, 0 };

/*!
 * Tx output powers table definition
 */
static const int8_t lorawan_tx_power[]    = { 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10 };

/*!
 * LoRaMac bands
 */
static Band_t bands[LORA_MAX_NB_BANDS] =
{
    BAND0,
};

/*!
 *  * LoRaMAC channels
 *   */
static ChannelParams_t channels[LORA_MAX_NB_CHANNELS];

/*!
 * Defines the first channel for RX window 2 for US band
 */
#define LORAMAC_FIRST_RX2_CHANNEL           ( (uint32_t) 923.3e6 )

/*!
 * Defines the last channel for RX window 2 for US band
 */
#define LORAMAC_LAST_RX2_CHANNEL            ( (uint32_t) 927.5e6 )

/*!
 * Defines the step width of the channels for RX window 2
 */
#define LORAMAC_STEPWIDTH_RX2_CHANNEL       ( (uint32_t) 600e3 )

#else
    #error "Please define a frequency band in the compiler options."
#endif

/*!
 * \brief This function prepares the MAC to abort the execution of function
 *        OnRadioRxDone in case of a reception error.
 */
static void _prepare_rx_abort(netdev2_lorawan_t *loradev);

/*!
 * \brief Searches and set the next random available channel
 *
 * \param [OUT] Time to wait for the next transmission according to the duty
 *              cycle.
 *
 * \retval status  Function status [1: OK, 0: Unable to find a channel on the
 *                                  current datarate]
 */
static bool _set_next_channel( netdev2_lorawan_t *loradev, TimerTime_t* time );

/*!
 * \brief Sets the network to public or private. Updates the sync byte.
 *
 * \param [IN] enable if true, it enables a public network
 */
static void _set_public_network(netdev2_lorawan_t *loradev, bool enable );

/*!
 * \brief Initializes and opens the reception window
 *
 * \param [IN] freq window channel frequency
 * \param [IN] datarate window channel datarate
 * \param [IN] bandwidth window channel bandwidth
 * \param [IN] timeout window channel timeout
 */
static void _rx_window_setup(netdev2_lorawan_t *loradev, uint32_t freq, int8_t datarate, uint32_t bandwidth, uint16_t timeout, bool rxContinuous );

/*!
 * \brief Verifies if the RX window 2 frequency is in range
 *
 * \param [IN] freq window channel frequency
 *
 * \retval status  Function status [1: OK, 0: Frequency not applicable]
 */
static bool _rx2_freq_in_range( uint32_t freq );

/*!
 * \brief Adds a new MAC command to be sent.
 *
 * \Remark MAC layer internal function
 *
 * \param [in] cmd MAC command to be added
 *                 [MOTE_MAC_LINK_CHECK_REQ,
 *                  MOTE_MAC_LINK_ADR_ANS,
 *                  MOTE_MAC_DUTY_CYCLE_ANS,
 *                  MOTE_MAC_RX2_PARAM_SET_ANS,
 *                  MOTE_MAC_DEV_STATUS_ANS
 *                  MOTE_MAC_NEW_CHANNEL_ANS]
 * \param [in] p1  1st parameter ( optional depends on the command )
 * \param [in] p2  2nd parameter ( optional depends on the command )
 *
 * \retval status  Function status [0: OK, 1: Unknown command, 2: Buffer full]
 */
static LoRaMacStatus_t _add_mac_command(netdev2_lorawan_t *loradev, uint8_t cmd, uint8_t p1, uint8_t p2 );

/*!
 * \brief Parses the MAC commands which must be repeated.
 *
 * \Remark MAC layer internal function
 *
 * \param [IN] cmdBufIn  Buffer which stores the MAC commands to send
 * \param [IN] length  Length of the input buffer to parse
 * \param [OUT] cmdBufOut  Buffer which stores the MAC commands which must be
 *                         repeated.
 *
 * \retval Size of the MAC commands to repeat.
 */
static uint8_t _parse_to_repeat_mac_commands( uint8_t* cmdBufIn, uint8_t length, uint8_t* cmdBufOut );

/*!
 * \brief Validates if the payload fits into the frame, taking the datarate
 *        into account.
 *
 * \details Refer to chapter 4.3.2 of the LoRaWAN specification, v1.0
 *
 * \param lenN Length of the application payload. The length depends on the
 *             datarate and is region specific
 *
 * \param datarate Current datarate
 *
 * \param fOptsLen Length of the fOpts field
 *
 * \retval [false: payload does not fit into the frame, true: payload fits into
 *          the frame]
 */
static bool _validate_payload_length(netdev2_lorawan_t *loradev, uint8_t lenN, int8_t datarate, uint8_t fOptsLen );

/*!
 * \brief Counts the number of bits in a mask.
 *
 * \param [IN] mask A mask from which the function counts the active bits.
 * \param [IN] nbBits The number of bits to check.
 *
 * \retval Number of enabled bits in the mask.
 */
static uint8_t _count_bits( uint16_t mask, uint8_t nbBits );

#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
/*!
 * \brief Counts the number of enabled 125 kHz channels in the channel mask.
 *        This function can only be applied to US915 band.
 *
 * \param [IN] channelsMask Pointer to the first element of the channel mask
 *
 * \retval Number of enabled channels in the channel mask
 */
static uint8_t _count_bw125_channels( uint16_t *channelsMask );

#if defined( USE_BAND_915_HYBRID )
/*!
 * \brief Validates the correctness of the channel mask for US915, hybrid mode.
 *
 * \param [IN] mask Block definition to set.
 * \param [OUT] channelsMask Pointer to the first element of the channel mask
 */
static void _reenable_channels( uint16_t mask, uint16_t* channelMask );

/*!
 * \brief Validates the correctness of the channel mask for US915, hybrid mode.
 *
 * \param [IN] channelsMask Pointer to the first element of the channel mask
 *
 * \retval [true: channel mask correct, false: channel mask not correct]
 */
static bool _validate_channel_mask( uint16_t* channelMask );
#endif

#endif

/*!
 * \brief Limits the Tx power according to the number of enabled channels
 *
 * \retval Returns the maximum valid tx power
 */
static int8_t _limit_tx_power(netdev2_lorawan_t *loradev, int8_t txPower );

/*!
 * \brief Verifies, if a value is in a given range.
 *
 * \param value Value to verify, if it is in range
 *
 * \param min Minimum possible value
 *
 * \param max Maximum possible value
 *
 * \retval Returns the maximum valid tx power
 */
bool _value_in_range( int8_t value, int8_t min, int8_t max );

/*!
 * \brief Calculates the next datarate to set, when ADR is on or off
 *
 * \param [IN] adrEnabled Specify whether ADR is on or off
 *
 * \param [IN] updateChannelMask Set to true, if the channel masks shall be updated
 *
 * \param [OUT] datarateOut Reports the datarate which will be used next
 *
 * \retval Returns the state of ADR ack request
 */
static bool AdrNextDr(netdev2_lorawan_t *loradev, bool adrEnabled, bool updateChannelMask, int8_t* datarateOut );

/*!
 * \brief Disables channel in a specified channel mask
 *
 * \param [IN] id - Id of the channel
 *
 * \param [IN] mask - Pointer to the channel mask to edit
 *
 * \retval [true, if disable was successful, false if not]
 */
static bool _disable_channel_in_mask( uint8_t id, uint16_t* mask );

/*!
 * \brief Decodes MAC commands in the fOpts field and in the payload
 */
static void _process_mac_commands(netdev2_lorawan_t *loradev, uint8_t *payload, uint8_t macIndex, uint8_t commandsSize, uint8_t snr );

/*
 * \brief Schedules the frame according to the duty cycle
 *
 * \retval Status of the operation
 */
static LoRaMacStatus_t _schedule_next_tx(netdev2_lorawan_t *loradev);

/*
 * \brief Sets the duty cycle for retransmissions
 *
 * \retval Duty cycle
 */
static uint16_t _set_retransmission_duty_cycle( void );

/*
 * \brief Calculates the back-off time for the band of a channel.
 *
 * \param [IN] channel     The last Tx channel index
 */
static void _calculate_backoff(netdev2_lorawan_t *loradev, uint8_t channel );

/*
 * \brief Alternates the datarate of the channel for the join request.
 *
 * \param [IN] nbTrials    Number of performed join requests.
 * \retval Datarate to apply
 */
static int8_t join_request_alternate_datarate(netdev2_lorawan_t *loradev, uint16_t nbTrials );

/*!
 * \brief LoRaMAC layer prepared frame buffer transmission with channel specification
 *
 * \remark PrepareFrame must be called at least once before calling this
 *         function.
 *
 * \param [IN] channel     Channel parameters
 * \retval status          Status of the operation.
 */
LoRaMacStatus_t _send_on_channel(netdev2_lorawan_t *loradev, ChannelParams_t channel );
bool check_rf_freq( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

//TODO!!!
#if 0
void LoRaMacMulticastChannelAdd( MulticastParams_t *channelParam )
{
    _multicast_channel_link( channelParam );
}

void LoRaMacMulticastChannelRemove( MulticastParams_t *channelParam )
{
    _multicast_channel_unlink( channelParam );
}
#endif

/*!
 * \brief Resets MAC specific parameters to default
 */
void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
   dst = dst + ( size - 1 );
   while( size-- )
   {
       *dst-- = *src++;
   }
}
static void _reset_mac(netdev2_lorawan_t *loradev);

void on_tx_done(netdev2_lorawan_t *loradev)
{
    TimerTime_t curTime = xtimer_now_usec64();
    netopt_state_t state = NETOPT_STATE_SLEEP;
    netdev2_t *netdev = (netdev2_t*) loradev;
    if( loradev->LoRaMacDeviceClass != CLASS_C )
    {
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
    }
    else
    {
        on_rx2_timeout(loradev);
    }

    // Store last Tx channel
    loradev->LastTxChannel = loradev->Channel;
    // Update last tx done time for the current channel
    bands[channels[loradev->LastTxChannel].Band].LastTxDoneTime = curTime;
    // Update Aggregated last tx done time
    loradev->AggregatedLastTxDoneTime = curTime;

    if( loradev->IsRxWindowsEnabled == true )
    {
        loradev->RxWindowTimer1.msg.type = LORAWAN_TIMER_RX_WINDOW1;
        xtimer_set_msg(&(loradev->RxWindowTimer1.dev), xtimer_ticks_from_usec(loradev->RxWindow1Delay*1000-5000).ticks32, &(loradev->RxWindowTimer1.msg), loradev->RxWindowTimer1.pid);
        //TimerSetValue( &RxWindowTimer1, dev->RxWindow1Delay, LORAWAN_TIMER_RX_WINDOW1);
        //TimerStart( &RxWindowTimer1, 0 );
        if( loradev->LoRaMacDeviceClass != CLASS_C )
        {
            loradev->RxWindowTimer2.msg.type = LORAWAN_TIMER_RX_WINDOW2;
        xtimer_set_msg(&(loradev->RxWindowTimer2.dev), xtimer_ticks_from_usec(loradev->RxWindow2Delay*1000).ticks32, &(loradev->RxWindowTimer2.msg), loradev->RxWindowTimer2.pid);
            //TimerSetValue( &RxWindowTimer2, dev->RxWindow2Delay, LORAWAN_TIMER_RX_WINDOW2);
            //TimerStart( &RxWindowTimer2, 0);
        }
        if( ( loradev->LoRaMacDeviceClass == CLASS_C ) || ( loradev->NodeAckRequested == true ) )
        {
            loradev->AckTimeoutTimer.msg.type = LORAWAN_TIMER_ACK_TIMEOUT;
            xtimer_set_msg(&(loradev->AckTimeoutTimer.dev), (loradev->RxWindow2Delay + ACK_TIMEOUT + random_uint32_range( -ACK_TIMEOUT_RND, ACK_TIMEOUT_RND))*1000, &(loradev->AckTimeoutTimer.msg), loradev->AckTimeoutTimer.pid);
            //TimerSetValue( &AckTimeoutTimer, dev->RxWindow2Delay + ACK_TIMEOUT +
             //                                randr( -ACK_TIMEOUT_RND, ACK_TIMEOUT_RND ), LORAWAN_TIMER_ACK_TIMEOUT);
            //TimerStart( &AckTimeoutTimer, 0);
        }
    }

    if( loradev->NodeAckRequested == false )
    {
        loradev->ChannelsNbRepCounter++;
    }
}

static void _prepare_rx_abort(netdev2_lorawan_t *loradev)
{
    //Abort TX Running...

    if( loradev->NodeAckRequested )
    {
        on_ack_timeout(loradev);
    }

    if( ( loradev->rx_slot == 0 ) && ( loradev->LoRaMacDeviceClass == CLASS_C ) )
    {
        on_rx2_timeout(loradev);
    }

}

void retransmit_join_req(netdev2_lorawan_t *loradev)
{
    loradev->ChannelsNbRepCounter = 0;
    if(loradev->lorawan.tx_rx.nwk_status == false)
    {
        gnrc_lorawan_send_join_request(loradev);
    }
    else
    {
        _schedule_next_tx(loradev);
    }
}

void retransmit_ack(netdev2_lorawan_t *loradev)
{
    if( ( loradev->ack_timeout_retries_counter < loradev->ack_timeout_retries ) && ( loradev->ack_timeout_retries_counter <= MAX_ACK_RETRIES ) )
    {
        loradev->ack_timeout_retries_counter++;

        if( ( loradev->ack_timeout_retries_counter % 2 ) == 1 )
        {
            loradev->LoRaMacParams.ChannelsDatarate = MAX( loradev->LoRaMacParams.ChannelsDatarate - 1, LORAMAC_TX_MIN_DATARATE );
        }
        // Sends the same frame again
        _schedule_next_tx(loradev);
    }

    if( loradev->ack_timeout_retries_counter >= loradev->ack_timeout_retries )
    {
        loradev->NodeAckRequested = false;
        if( loradev->IsUpLinkCounterFixed == false )
        {
            loradev->uplink_counter++;
        }

    }
}

void on_rx_done(netdev2_lorawan_t *loradev, gnrc_pktsnip_t *recv_pkt,  int16_t rssi, int8_t snr )
{
    lw_hdr_t hdr;
    bool skipIndication = false;

    uint8_t pktHeaderLen = 0;
    uint32_t address = 0;
    uint8_t appPayloadStartIndex = 0;
    uint8_t port = 0xFF;
    uint8_t frameLen = 0;
    uint32_t mic = 0;
    uint32_t micRx = 0;

    uint16_t sequenceCounter = 0;
    uint16_t sequenceCounterPrev = 0;
    uint16_t sequenceCounterDiff = 0;
    uint32_t downLinkCounter = 0;
    uint8_t *payload = recv_pkt->data;
    uint16_t size = recv_pkt->size;

    gnrc_pktsnip_t *pkt=NULL;

    MulticastParams_t *curMulticastParams = NULL;

    uint8_t multicast = 0;

    bool isMicOk = false;
    bool rx_data = false;

    netopt_state_t state = NETOPT_STATE_SLEEP;
    loradev->port = 0;
    loradev->Buffer = NULL;
    loradev->BufferSize = 0;
    //loradev->McpsIndication.McpsIndication = MCPS_UNCONFIRMED;

    if( loradev->LoRaMacDeviceClass != CLASS_C )
    {
        netdev2_t *netdev = (netdev2_t*) loradev;
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
    }
    //TimerStop( &RxWindowTimer2 );
    xtimer_remove(&loradev->RxWindowTimer2.dev);

    hdr.mt_maj = payload[pktHeaderLen++];

    switch( lw_hdr_get_mtype(&hdr) )
    {
        case FRAME_TYPE_JOIN_ACCEPT:
            pkt = gnrc_pktbuf_add(NULL, NULL, size, GNRC_NETTYPE_UNDEF);
            if( loradev->lorawan.tx_rx.nwk_status == true )
            {
                break;
            }
            LoRaMacJoinDecrypt( payload + 1, size - 1, loradev->app_key, ((uint8_t*) pkt->data) + 1 );

            ((uint8_t*) pkt->data)[0] = hdr.mt_maj;

            LoRaMacJoinComputeMic( (uint8_t*) pkt->data, size - LORAMAC_MFR_LEN, loradev->app_key, &mic );

            micRx |= ( uint32_t )((uint8_t*) pkt->data)[size - LORAMAC_MFR_LEN];
            micRx |= ( ( uint32_t )((uint8_t*) pkt->data)[size - LORAMAC_MFR_LEN + 1] << 8 );
            micRx |= ( ( uint32_t )((uint8_t*) pkt->data)[size - LORAMAC_MFR_LEN + 2] << 16 );
            micRx |= ( ( uint32_t )((uint8_t*) pkt->data)[size - LORAMAC_MFR_LEN + 3] << 24 );

            if( micRx == mic )
            {
                LoRaMacJoinComputeSKeys( loradev->app_key, ((uint8_t*) pkt->data) + 1, loradev->dev_nonce, loradev->lorawan.nwk_skey, loradev->lorawan.app_skey );

                loradev->lorawan.net_id = ( uint32_t )((uint8_t*) pkt->data)[4];
                loradev->lorawan.net_id |= ( ( uint32_t )((uint8_t*) pkt->data)[5] << 8 );
                loradev->lorawan.net_id |= ( ( uint32_t )((uint8_t*) pkt->data)[6] << 16 );

                loradev->lorawan.dev_addr = ( uint32_t )((uint8_t*) pkt->data)[7];
                loradev->lorawan.dev_addr |= ( ( uint32_t )((uint8_t*) pkt->data)[8] << 8 );
                loradev->lorawan.dev_addr |= ( ( uint32_t )((uint8_t*) pkt->data)[9] << 16 );
                loradev->lorawan.dev_addr |= ( ( uint32_t )((uint8_t*) pkt->data)[10] << 24 );

                // DLSettings
                loradev->LoRaMacParams.Rx1DrOffset = ( ((uint8_t*) pkt->data)[11] >> 4 ) & 0x07;
                loradev->LoRaMacParams.Rx2Channel.Datarate = ((uint8_t*) pkt->data)[11] & 0x0F;
#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
                /*
                 * WARNING: To be removed once Semtech server implementation
                 *          is corrected.
                 */
                if( loradev->LoRaMacParams.Rx2Channel.Datarate == DR_3 )
                {
                    loradev->LoRaMacParams.Rx2Channel.Datarate = DR_8;
                }
#endif
                // RxDelay
                loradev->LoRaMacParams.ReceiveDelay1 = ( ((uint8_t*) pkt->data)[12] & 0x0F );
                if( loradev->LoRaMacParams.ReceiveDelay1 == 0 )
                {
                    loradev->LoRaMacParams.ReceiveDelay1 = 1;
                }
                loradev->LoRaMacParams.ReceiveDelay1 *= 1e3;
                loradev->LoRaMacParams.ReceiveDelay2 = loradev->LoRaMacParams.ReceiveDelay1 + 1e3;

#if !( defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID ) )
                //CFList
                if( ( size - 1 ) > 16 )
                {
                    ChannelParams_t param;
                    param.DrRange.Value = ( DR_5 << 4 ) | DR_0;

                    for( uint8_t i = 3, j = 0; i < ( 5 + 3 ); i++, j += 3 )
                    {
                        param.Frequency = ( ( uint32_t ) ((uint8_t*) pkt->data)[13 + j] | ( ( uint32_t ) ((uint8_t*) pkt->data)[14 + j] << 8 ) | ( ( uint32_t ) ((uint8_t*) pkt->data)[15 + j] << 16 ) ) * 100;
                        channel_add(loradev, i, param );
                    }
                }
#endif
                loradev->lorawan.tx_rx.nwk_status = true;
                loradev->LoRaMacParams.ChannelsDatarate = loradev->LoRaMacParamsDefaults.ChannelsDatarate;
                loradev->ChannelsNbRepCounter = loradev->LoRaMacParams.ChannelsNbRep;
                loradev->uplink_counter = 0;
                _finish_rx(loradev);
                gnrc_netapi_dispatch(GNRC_NETTYPE_LORAWAN, GNRC_NETREG_DEMUX_CTX_ALL, GNRC_LORAWAN_JOIN_REQUEST_MSG, NULL);
            }
            else
            {
                if( loradev->ChannelsNbRepCounter >= loradev->LoRaMacParams.ChannelsNbRep )
                {
                    retransmit_join_req(loradev);
                }

            }
            break;
        case FRAME_TYPE_DATA_CONFIRMED_DOWN:
        case FRAME_TYPE_DATA_UNCONFIRMED_DOWN:
            {
                address = payload[pktHeaderLen++];
                address |= ( (uint32_t)payload[pktHeaderLen++] << 8 );
                address |= ( (uint32_t)payload[pktHeaderLen++] << 16 );
                address |= ( (uint32_t)payload[pktHeaderLen++] << 24 );

                if( address != loradev->lorawan.dev_addr )
                {
                    curMulticastParams = loradev->MulticastChannels;
                    while( curMulticastParams != NULL )
                    {
                        if( address == curMulticastParams->Address )
                        {
                            multicast = 1;
                            downLinkCounter = curMulticastParams->DownLinkCounter;
                            break;
                        }
                        curMulticastParams = curMulticastParams->Next;
                    }
                    if( multicast == 0 )
                    {
                        // We are not the destination of this frame.
                        _prepare_rx_abort(loradev);
                        return;
                    }
                }
                else
                {
                    multicast = 0;
                    downLinkCounter = loradev->DownLinkCounter;
                }

                hdr.fctrl = payload[pktHeaderLen++];

                sequenceCounter = ( uint16_t )payload[pktHeaderLen++];
                sequenceCounter |= ( uint16_t )payload[pktHeaderLen++] << 8;

                appPayloadStartIndex = 8 + lw_hdr_get_frame_opts_len(&hdr);

                micRx |= ( uint32_t )payload[size - LORAMAC_MFR_LEN];
                micRx |= ( ( uint32_t )payload[size - LORAMAC_MFR_LEN + 1] << 8 );
                micRx |= ( ( uint32_t )payload[size - LORAMAC_MFR_LEN + 2] << 16 );
                micRx |= ( ( uint32_t )payload[size - LORAMAC_MFR_LEN + 3] << 24 );

                sequenceCounterPrev = ( uint16_t )downLinkCounter;
                sequenceCounterDiff = ( sequenceCounter - sequenceCounterPrev );

                if( sequenceCounterDiff < ( 1 << 15 ) )
                {
                    downLinkCounter += sequenceCounterDiff;
                    mic = LoRaMacComputeMic( payload, size - LORAMAC_MFR_LEN, loradev->lorawan.nwk_skey, address, DOWN_LINK, downLinkCounter);
                    if( micRx == mic )
                    {
                        isMicOk = true;
                    }
                }
                else
                {
                    // check for sequence roll-over
                    uint32_t  downLinkCounterTmp = downLinkCounter + 0x10000 + ( int16_t )sequenceCounterDiff;
                    mic = LoRaMacComputeMic( payload, size - LORAMAC_MFR_LEN, loradev->lorawan.nwk_skey, address, DOWN_LINK, downLinkCounterTmp);
                    if( micRx == mic )
                    {
                        isMicOk = true;
                        downLinkCounter = downLinkCounterTmp;
                    }
                }

                // Check for a the maximum allowed counter difference
                if( sequenceCounterDiff >= MAX_FCNT_GAP )
                {
                    _prepare_rx_abort(loradev);
                    return;
                }

                if( isMicOk == true )
                {
                    loradev->Buffer = NULL;
                    loradev->BufferSize = 0;


                    loradev->AdrAckCounter = 0;
                    loradev->MacCommandsBufferToRepeatIndex = 0;

                    // Update 32 bits downlink counter
                    if( multicast == 1 )
                    {
                        //loradev->McpsIndication.McpsIndication = MCPS_MULTICAST;

                        if( ( curMulticastParams->DownLinkCounter == downLinkCounter ) &&
                            ( curMulticastParams->DownLinkCounter != 0 ) )
                        {
                            _prepare_rx_abort(loradev);
                            return;
                        }
                        //curMulticastParams->DownLinkCounter = downLinkCounter;
                    }
                    else
                    {
                        if( lw_hdr_get_mtype(&hdr) == FRAME_TYPE_DATA_CONFIRMED_DOWN)
                        {
                            loradev->SrvAckRequested = true;
                            //loradev->McpsIndication.McpsIndication = MCPS_CONFIRMED;

                            if( ( loradev->DownLinkCounter == downLinkCounter ) &&
                                ( loradev->DownLinkCounter != 0 ) )
                            {
                                // Duplicated confirmed downlink. Skip indication.
                                skipIndication = true;
                            }
                        }
                        else
                        {
                            loradev->SrvAckRequested = false;
                            //loradev->McpsIndication.McpsIndication = MCPS_UNCONFIRMED;

                            if( ( loradev->DownLinkCounter == downLinkCounter ) &&
                                ( loradev->DownLinkCounter != 0 ) )
                            {
                                _prepare_rx_abort(loradev);
                                return;
                            }
                        }
                        loradev->DownLinkCounter = downLinkCounter;
                    }

                    // Check if the frame is an acknowledgement
                    if( lw_hdr_get_ack(&hdr) )
                    {

                        // Stop the AckTimeout timer as no more retransmissions
                        // are needed.
                        //TimerStop( &AckTimeoutTimer );
                        xtimer_remove(&loradev->AckTimeoutTimer.dev);
                        gnrc_pktbuf_release(loradev->pkt);

#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
                        // Re-enable default channels LC1, LC2, LC3
                        loradev->LoRaMacParams.ChannelsMask[0] = loradev->LoRaMacParams.ChannelsMask[0] | ( LC( 1 ) + LC( 2 ) + LC( 3 ) );
#elif defined( USE_BAND_915 )
                        // Re-enable default channels
                        loradev->LoRaMacParams.ChannelsMask[0] = 0xFFFF;
                        loradev->LoRaMacParams.ChannelsMask[1] = 0xFFFF;
                        loradev->LoRaMacParams.ChannelsMask[2] = 0xFFFF;
                        loradev->LoRaMacParams.ChannelsMask[3] = 0xFFFF;
                        loradev->LoRaMacParams.ChannelsMask[4] = 0x00FF;
                        loradev->LoRaMacParams.ChannelsMask[5] = 0x0000;
#elif defined( USE_BAND_915_HYBRID )
                        // Re-enable default channels
                        _reenable_channels( loradev->LoRaMacParams.ChannelsMask[4], loradev->LoRaMacParams.ChannelsMask );
                        loradev->LoRaMacParams.ChannelsMask[0] = 0x3C3C;
                        loradev->LoRaMacParams.ChannelsMask[1] = 0x0000;
                        loradev->LoRaMacParams.ChannelsMask[2] = 0x0000;
                        loradev->LoRaMacParams.ChannelsMask[3] = 0x0000;
                        loradev->LoRaMacParams.ChannelsMask[4] = 0x0001;
                        loradev->LoRaMacParams.ChannelsMask[5] = 0x0000;
#else
#error "Please define a frequency band in the compiler options."
#endif

                        loradev->NodeAckRequested = false;
                        if( loradev->IsUpLinkCounterFixed == false )
                        {
                            loradev->uplink_counter++;
                        }
                    }
                    else
                    {

                        if( loradev->ack_timeout_retries_counter > loradev->ack_timeout_retries )
                        {
                            // Stop the AckTimeout timer as no more retransmissions
                            // are needed.
                            //TimerStop( &AckTimeoutTimer );
                            xtimer_remove(&loradev->AckTimeoutTimer.dev);
                            gnrc_pktbuf_release(loradev->pkt);
                        }
                    }

                    if( ( ( size - 4 ) - appPayloadStartIndex ) > 0 )
                    {
                        port = payload[appPayloadStartIndex++];
                        frameLen = ( size - 4 ) - appPayloadStartIndex;

                        loradev->port = port;

                        if( port == 0 )
                        {
                            if(lw_hdr_get_frame_opts_len(&hdr) == 0)
                            {
                                pkt = gnrc_pktbuf_add(NULL, NULL, frameLen, GNRC_NETTYPE_UNDEF);
                                 LoRaMacPayloadDecrypt( payload + appPayloadStartIndex,
                                                       frameLen,
                                                       loradev->lorawan.nwk_skey,
                                                       address,
                                                       DOWN_LINK,
                                                       downLinkCounter,
                                                       (uint8_t*) pkt->data );

                                // Decode frame payload MAC commands
                                _process_mac_commands(loradev, (uint8_t*) pkt->data, 0, frameLen, snr );
                            }
                            else
                            {
                                skipIndication = true;
                            }
                        }
                        else
                        {
                            if( lw_hdr_get_frame_opts_len(&hdr) > 0 )
                            {
                                // Decode Options field MAC commands. Omit the fPort.
                                _process_mac_commands(loradev, payload, 8, appPayloadStartIndex - 1, snr );
                            }

                            pkt = gnrc_pktbuf_add(NULL, NULL, frameLen, GNRC_NETTYPE_UNDEF);
                             LoRaMacPayloadDecrypt( payload + appPayloadStartIndex,
                                                   frameLen,
                                                   loradev->lorawan.app_skey,
                                                   address,
                                                   DOWN_LINK,
                                                   downLinkCounter,
                                                   (uint8_t*) pkt->data );

                            if( skipIndication == false )
                            {
                                loradev->Buffer = pkt->data;
                                loradev->BufferSize = frameLen;
                                rx_data = true;
                                if(!gnrc_netapi_dispatch_receive(GNRC_NETTYPE_LORAWAN, port, pkt))
                                {
                                    gnrc_pktbuf_release(pkt);
                                }
                            }
                        }
                    }
                    else
                    {
                        if( lw_hdr_get_frame_opts_len(&hdr) > 0 )
                        {
                            // Decode Options field MAC commands
                            _process_mac_commands(loradev, payload, 8, appPayloadStartIndex, snr );
                        }
                    }

                    _finish_rx(loradev);
                }
                else
                {
                    _prepare_rx_abort(loradev);
                    return;
                }
            }

            break;
            /* TODO:
        case FRAME_TYPE_PROPRIETARY:
            {
                memcpy( loradev->LoRaMacRxPayload, &payload[pktHeaderLen], size );

                loradev->Buffer = loradev->LoRaMacRxPayload;
                loradev->BufferSize = size - pktHeaderLen;

                break;
            }
            */
        default:
            _prepare_rx_abort(loradev);
            break;
    }

    if( ( loradev->rx_slot == 0 ) && ( loradev->LoRaMacDeviceClass == CLASS_C ) )
    {
        on_rx2_timeout(loradev);
    }

    if(!rx_data)
    {
        gnrc_pktbuf_release(pkt);
    }

    gnrc_pktbuf_release(recv_pkt);

}

void on_tx_timeout( netdev2_lorawan_t *loradev )
{
    netopt_state_t state = NETOPT_STATE_SLEEP;
    netdev2_t *netdev = (netdev2_t*) loradev;
    if( loradev->LoRaMacDeviceClass != CLASS_C )
    {
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
    }
    else
    {
        on_rx2_timeout(loradev);
    }
}

void on_rx_error(netdev2_lorawan_t *loradev)
{
    netopt_state_t state = NETOPT_STATE_SLEEP;
    netdev2_t *netdev = (netdev2_t*) loradev;
    if( loradev->LoRaMacDeviceClass != CLASS_C )
    {
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
    }
    else
    {
        on_rx2_timeout(loradev);
    }
}

void on_rx_timeout(netdev2_lorawan_t *loradev)
{
    netopt_state_t state = NETOPT_STATE_SLEEP;
    netdev2_t *netdev = (netdev2_t*) loradev;
    if( loradev->LoRaMacDeviceClass != CLASS_C )
    {
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
    }
    else
    {
        on_rx2_timeout(loradev);
    }

    if( loradev->rx_slot == 1 )
    {
        if(loradev->join_req_sent && loradev->lorawan.tx_rx.nwk_status == false && loradev->ChannelsNbRepCounter >= loradev->LoRaMacParams.ChannelsNbRep)
        {
            retransmit_join_req(loradev);
        }
        _finish_rx(loradev);
    }
}

void _finish_rx(netdev2_lorawan_t *loradev)
{
    loradev->ChannelsNbRepCounter = 0;

    loradev->AdrAckCounter++;
    if( loradev->IsUpLinkCounterFixed == false )
    {
        loradev->uplink_counter++;
    }

}

void on_tx_delay_timeout(netdev2_lorawan_t *loradev)
{
    //TimerStop( &TxDelayedTimer );
    xtimer_remove(&loradev->TxDelayedTimer.dev);

    if(loradev->join_req_sent)
    {
        gnrc_lorawan_send_join_request(loradev);
    }
    else
    {
        _schedule_next_tx(loradev);
    }
}

void on_rx1_timeout(netdev2_lorawan_t *loradev)
{
    uint16_t symbTimeout = 5; // DR_2, DR_1, DR_0
    int8_t datarate = 0;
    uint32_t bandwidth = 0; // LoRa 125 kHz
    netdev2_t *netdev = (netdev2_t*) loradev;

    //TimerStop( &RxWindowTimer1 );
    xtimer_remove(&loradev->RxWindowTimer1.dev);
    loradev->rx_slot = 0;

    netopt_state_t state = NETOPT_STATE_STANDBY;
    if( loradev->LoRaMacDeviceClass == CLASS_C )
    {
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
    }

#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
    datarate = loradev->LoRaMacParams.ChannelsDatarate - loradev->LoRaMacParams.Rx1DrOffset;
    if( datarate < 0 )
    {
        datarate = DR_0;
    }

    // For higher datarates, we increase the number of symbols generating a Rx Timeout
    if( ( datarate == DR_3 ) || ( datarate == DR_4 ) )
    { // DR_4, DR_3
        symbTimeout = 8;
    }
    else if( datarate == DR_5 )
    {
        symbTimeout = 10;
    }
    else if( datarate == DR_6 )
    {// LoRa 250 kHz
        bandwidth  = 1;
        symbTimeout = 14;
    }
    _rx_window_setup(loradev, channels[loradev->Channel].Frequency, datarate, bandwidth, symbTimeout, false );
#elif ( defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID ) )
    datarate = datarate_offsets[loradev->LoRaMacParams.ChannelsDatarate][loradev->LoRaMacParams.Rx1DrOffset];
    if( datarate < 0 )
    {
        datarate = DR_0;
    }
    // For higher datarates, we increase the number of symbols generating a Rx Timeout
    switch( datarate )
    {
        case DR_0:       // SF10 - BW125
            symbTimeout = 5;
            break;

        case DR_1:       // SF9  - BW125
        case DR_2:       // SF8  - BW125
        case DR_8:       // SF12 - BW500
        case DR_9:       // SF11 - BW500
        case DR_10:      // SF10 - BW500
            symbTimeout = 8;
            break;

        case DR_3:       // SF7  - BW125
        case DR_11:      // SF9  - BW500
            symbTimeout = 10;
            break;

        case DR_4:       // SF8  - BW500
        case DR_12:      // SF8  - BW500
            symbTimeout = 14;
            break;

        case DR_13:      // SF7  - BW500
            symbTimeout = 16;
            break;
        default:
            break;
    }
    if( datarate >= DR_4 )
    {// LoRa 500 kHz
        bandwidth  = 2;
    }
    _rx_window_setup(loradev, LORAMAC_FIRST_RX2_CHANNEL + ( loradev->Channel % 8 ) * LORAMAC_STEPWIDTH_RX2_CHANNEL, datarate, bandwidth, symbTimeout, false );
#else
    #error "Please define a frequency band in the compiler options."
#endif
}

void on_rx2_timeout(netdev2_lorawan_t *loradev)
{
    uint16_t symbTimeout = 5; // DR_2, DR_1, DR_0
    uint32_t bandwidth = 0; // LoRa 125 kHz

    //TimerStop( &RxWindowTimer2 );
    xtimer_remove(&loradev->RxWindowTimer2.dev);
    loradev->rx_slot = 1;

#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
    // For higher datarates, we increase the number of symbols generating a Rx Timeout
    if( ( loradev->LoRaMacParams.Rx2Channel.Datarate == DR_3 ) || ( loradev->LoRaMacParams.Rx2Channel.Datarate == DR_4 ) )
    { // DR_4, DR_3
        symbTimeout = 8;
    }
    else if( loradev->LoRaMacParams.Rx2Channel.Datarate == DR_5 )
    {
        symbTimeout = 10;
    }
    else if( loradev->LoRaMacParams.Rx2Channel.Datarate == DR_6 )
    {// LoRa 250 kHz
        bandwidth  = 1;
        symbTimeout = 14;
    }
#elif ( defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID ) )
    // For higher datarates, we increase the number of symbols generating a Rx Timeout
    switch( loradev->LoRaMacParams.Rx2Channel.Datarate )
    {
        case DR_0:       // SF10 - BW125
            symbTimeout = 5;
            break;

        case DR_1:       // SF9  - BW125
        case DR_2:       // SF8  - BW125
        case DR_8:       // SF12 - BW500
        case DR_9:       // SF11 - BW500
        case DR_10:      // SF10 - BW500
            symbTimeout = 8;
            break;

        case DR_3:       // SF7  - BW125
        case DR_11:      // SF9  - BW500
            symbTimeout = 10;
            break;

        case DR_4:       // SF8  - BW500
        case DR_12:      // SF8  - BW500
            symbTimeout = 14;
            break;

        case DR_13:      // SF7  - BW500
            symbTimeout = 16;
            break;
        default:
            break;
    }
    if( loradev->LoRaMacParams.Rx2Channel.Datarate >= DR_4 )
    {// LoRa 500 kHz
        bandwidth  = 2;
    }
#else
    #error "Please define a frequency band in the compiler options."
#endif
    if( loradev->LoRaMacDeviceClass != CLASS_C )
    {
        _rx_window_setup(loradev, loradev->LoRaMacParams.Rx2Channel.Frequency, loradev->LoRaMacParams.Rx2Channel.Datarate, bandwidth, symbTimeout, false );
    }
    else
    {
        _rx_window_setup(loradev, loradev->LoRaMacParams.Rx2Channel.Frequency, loradev->LoRaMacParams.Rx2Channel.Datarate, bandwidth, symbTimeout, true );
    }
}

void on_ack_timeout(netdev2_lorawan_t *loradev)
{
    //TimerStop( &AckTimeoutTimer );
    xtimer_remove(&loradev->AckTimeoutTimer.dev);

    if( loradev->NodeAckRequested == true )
    {
        _finish_rx(loradev);
        retransmit_ack(loradev);
    }
}

static bool _set_next_channel(netdev2_lorawan_t *loradev, TimerTime_t* time )
{
    uint8_t nbEnabledChannels = 0;
    uint8_t delayTx = 0;
    uint8_t enabledChannels[LORA_MAX_NB_CHANNELS];
    TimerTime_t nextTxDelay = ( TimerTime_t )( -1 );

    memset( enabledChannels, 0, LORA_MAX_NB_CHANNELS );

#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
    if( _count_bw125_channels( loradev->ChannelsMaskRemaining ) == 0 )
    { // Restore default channels
        memcpy( ( uint8_t* ) loradev->ChannelsMaskRemaining, ( uint8_t* ) loradev->LoRaMacParams.ChannelsMask, 8 );
    }
    if( ( loradev->LoRaMacParams.ChannelsDatarate >= DR_4 ) && ( ( loradev->ChannelsMaskRemaining[4] & 0x00FF ) == 0 ) )
    { // Make sure, that the channels are activated
        loradev->ChannelsMaskRemaining[4] = loradev->LoRaMacParams.ChannelsMask[4];
    }
#else
    if( _count_bits( loradev->LoRaMacParams.ChannelsMask[0], 16 ) == 0 )
    {
        // Re-enable default channels, if no channel is enabled
        loradev->LoRaMacParams.ChannelsMask[0] = loradev->LoRaMacParams.ChannelsMask[0] | ( LC( 1 ) + LC( 2 ) + LC( 3 ) );
    }
#endif

    // Update Aggregated duty cycle
    if( loradev->AggregatedTimeOff <= (xtimer_now_usec64()-loradev->AggregatedLastTxDoneTime ) )
    {
        loradev->AggregatedTimeOff = 0;

        // Update bands Time OFF
        for( uint8_t i = 0; i < LORA_MAX_NB_BANDS; i++ )
        {
            if( loradev->DutyCycleOn == true )
            {
                if( bands[i].TimeOff <= (xtimer_now_usec64() - bands[i].LastTxDoneTime ) )
                {
                    bands[i].TimeOff = 0;
                }
                if( bands[i].TimeOff != 0 )
                {
                    nextTxDelay = MIN( bands[i].TimeOff -
                                       (xtimer_now_usec64()- bands[i].LastTxDoneTime ),
                                       nextTxDelay );
                }
            }
            else
            {
                nextTxDelay = 0;
                bands[i].TimeOff = 0;
            }
        }

        // Search how many channels are enabled
        for( uint8_t i = 0, k = 0; i < LORA_MAX_NB_CHANNELS; i += 16, k++ )
        {
            for( uint8_t j = 0; j < 16; j++ )
            {
#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
                if( ( loradev->ChannelsMaskRemaining[k] & ( 1 << j ) ) != 0 )
#else
                if( ( loradev->LoRaMacParams.ChannelsMask[k] & ( 1 << j ) ) != 0 )
#endif
                {
                    if( channels[i + j].Frequency == 0 )
                    { // Check if the channel is enabled
                        continue;
                    }
#if defined( USE_BAND_868 ) || defined( USE_BAND_433 ) || defined( USE_BAND_780 )
                    if( loradev->lorawan.tx_rx.nwk_status == false )
                    {
                        if( ( JOIN_CHANNELS & ( 1 << j ) ) == 0 )
                        {
                            continue;
                        }
                    }
#endif
                    if( ( ( channels[i + j].DrRange.Fields.Min <= loradev->LoRaMacParams.ChannelsDatarate ) &&
                          ( loradev->LoRaMacParams.ChannelsDatarate <= channels[i + j].DrRange.Fields.Max ) ) == false )
                    { // Check if the current channel selection supports the given datarate
                        continue;
                    }
                    if( bands[channels[i + j].Band].TimeOff > 0 )
                    { // Check if the band is available for transmission
                        delayTx++;
                        continue;
                    }
                    enabledChannels[nbEnabledChannels++] = i + j;
                }
            }
        }
    }
    else
    {
        delayTx++;
        nextTxDelay = loradev->AggregatedTimeOff - (xtimer_now_usec64() - loradev->AggregatedLastTxDoneTime );
    }

    if( nbEnabledChannels > 0 )
    {
        loradev->Channel = enabledChannels[random_uint32_range( 0, nbEnabledChannels)];
#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
        if( loradev->Channel < ( LORA_MAX_NB_CHANNELS - 8 ) )
        {
            _disable_channel_in_mask( loradev->Channel, loradev->ChannelsMaskRemaining );
        }
#endif
        *time = 0;
        return true;
    }
    else
    {
        if( delayTx > 0 )
        {
            // Delay transmission due to loradev->AggregatedTimeOff or to a band time off
            *time = nextTxDelay;
            return true;
        }
        // Datarate not supported by any channel
        *time = 0;
        return false;
    }
}

static void _set_public_network(netdev2_lorawan_t *loradev, bool enable )
{
    netdev2_t *netdev = (netdev2_t*) loradev;
    loradev->lorawan.public = enable;
    netopt_enable_t lm = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_LORA_MODE, &lm, sizeof(netopt_enable_t));
    uint8_t sw;
    if( loradev->lorawan.public == true )
    {
        // Change LoRa modem SyncWord
        sw = LORA_MAC_PUBLIC_SYNCWORD;
    }
    else
    {
        // Change LoRa modem SyncWord
        sw = LORA_MAC_PRIVATE_SYNCWORD;
    }
    netdev->driver->set(netdev, NETOPT_LORA_SYNCWORD, &sw, sizeof(uint8_t));
}

void _set_rx_config(netdev2_lorawan_t *loradev, RadioModems_t modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, bool fixLen,
                         uint8_t payloadLen,
                         bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                         bool iqInverted, bool rxContinuous )
{
    netdev2_t *netdev = (netdev2_t*) loradev;

    netopt_enable_t _modem = modem;
    netdev->driver->set(netdev, NETOPT_LORA_MODE, &_modem, sizeof(netopt_enable_t));
    (void) bandwidthAfc;

    bool freq_hop_on = freqHopOn;
    bool iq_invert = iqInverted;
    uint8_t rx_single = rxContinuous ? false : true;
    uint32_t tx_timeout = 3 * 1000 * 1000;
    uint8_t bw = bandwidth + 7;
    uint8_t cr = coderate;
    uint8_t sf = datarate;
    bool implicit = fixLen;
    bool crc = crcOn;
    uint16_t rx_timeout = symbTimeout;
    uint16_t preamble = preambleLen;
    uint8_t payload_len = payloadLen;
    uint8_t hop_period = hopPeriod;
    uint8_t power = 14;

    netdev->driver->set(netdev, NETOPT_LORA_HOP, &freq_hop_on, sizeof(bool));
    netdev->driver->set(netdev, NETOPT_LORA_IQ_INVERT, &iq_invert, sizeof(bool));
    netdev->driver->set(netdev, NETOPT_LORA_SINGLE_RECEIVE, &rx_single, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_TX_TIMEOUT, &tx_timeout, sizeof(uint32_t));

    netdev->driver->set(netdev, NETOPT_LORA_BANDWIDTH, &bw, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_CODING_RATE, &cr, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_SPREADING_FACTOR, &sf, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_IMPLICIT, &implicit, sizeof(bool));
    netdev->driver->set(netdev, NETOPT_CRC, &crc, sizeof(bool));
    netdev->driver->set(netdev, NETOPT_LORA_SYMBOL_TIMEOUT, &rx_timeout, sizeof(uint16_t));
    netdev->driver->set(netdev, NETOPT_LORA_PREAMBLE_LENGTH, &preamble, sizeof(uint16_t));
    netdev->driver->set(netdev, NETOPT_LORA_PAYLOAD_LENGTH, &payload_len, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_HOP_PERIOD, &hop_period, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_TX_POWER, &power, sizeof(uint8_t));
}
static void _rx_window_setup(netdev2_lorawan_t *loradev, uint32_t freq, int8_t datarate, uint32_t bandwidth, uint16_t timeout, bool rxContinuous )
{
    netdev2_t *netdev = (netdev2_t*) loradev;
    uint8_t downlinkDatarate = lorawan_datarates[datarate];
    RadioModems_t modem;

    netopt_state_t state;
    netdev->driver->get(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t)); 

    netopt_enable_t lm;
    uint8_t max_payload;

    if( state == NETOPT_STATE_SLEEP || state == NETOPT_STATE_STANDBY )
    {
        netdev->driver->set(netdev, NETOPT_CHANNEL, &freq, sizeof(uint32_t));

#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
        if( datarate == DR_7 )
        {
            modem = MODEM_FSK;
            _set_rx_config(loradev, modem, 50e3, downlinkDatarate * 1e3, 0, 83.333e3, 5, 0, false, 0, true, 0, 0, false, rxContinuous );
        }
        else
        {
            modem = MODEM_LORA;
            _set_rx_config(loradev, modem, bandwidth, downlinkDatarate, 1, 0, 8, timeout, false, 0, false, 0, 0, true, rxContinuous );
        }
#elif defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
        modem = MODEM_LORA;

        /* Hack Begin*/
        if(loradev->lorawan.tx_rx.nwk_status == false)
        {
            (void) timeout;
            _set_rx_config(loradev, modem, bandwidth, downlinkDatarate, 1, 0, 8, 128, false, 0, false, 0, 0, true, rxContinuous );
        }
        else
            _set_rx_config(loradev, modem, bandwidth, downlinkDatarate, 1, 0, 8, timeout, false, 0, false, 0, 0, true, rxContinuous );
        /* Hack End*/
        //_set_rx_config( modem, bandwidth, downlinkDatarate, 1, 0, 8, timeout, false, 0, false, 0, 0, true, rxContinuous );

#endif

        lm = modem == MODEM_LORA ? NETOPT_ENABLE : NETOPT_DISABLE;
        netdev->driver->set(netdev, NETOPT_LORA_MODE, &lm, sizeof(netopt_enable_t));
        if( loradev->RepeaterSupport == true )
        {
            max_payload = lorawan_max_payload_of_datarate_repeater[datarate] + LORA_MAC_FRMPAYLOAD_OVERHEAD;
        }
        else
        {
            max_payload = lorawan_max_payload_of_datarate[datarate] + LORA_MAC_FRMPAYLOAD_OVERHEAD ;
        }

        netdev->driver->set(netdev, NETOPT_LORA_MAX_PAYLOAD, &max_payload, sizeof(uint8_t));
        uint32_t val;
        netopt_state_t state = NETOPT_STATE_RX;
        if( rxContinuous == false )
        {
            val = loradev->LoRaMacParams.MaxRxWindow*1000;
        }
        else
        {
            val = 0;
        }
        netdev->driver->set(netdev, NETOPT_LORA_RX_TIMEOUT, &val, sizeof(uint32_t));
        netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
    }
}

static bool _rx2_freq_in_range( uint32_t freq )
{
#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
    if( check_rf_freq( freq ) == true )
#elif ( defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID ) )
    if( ( check_rf_freq( freq ) == true ) &&
        ( freq >= LORAMAC_FIRST_RX2_CHANNEL ) &&
        ( freq <= LORAMAC_LAST_RX2_CHANNEL ) &&
        ( ( ( freq - ( uint32_t ) LORAMAC_FIRST_RX2_CHANNEL ) % ( uint32_t ) LORAMAC_STEPWIDTH_RX2_CHANNEL ) == 0 ) )
#endif
    {
        return true;
    }
    return false;
}

static bool _validate_payload_length(netdev2_lorawan_t *loradev, uint8_t lenN, int8_t datarate, uint8_t fOptsLen )
{
    uint16_t maxN = 0;
    uint16_t payloadSize = 0;

    // Get the maximum payload length
    if( loradev->RepeaterSupport == true )
    {
        maxN = lorawan_max_payload_of_datarate_repeater[datarate];
    }
    else
    {
        maxN = lorawan_max_payload_of_datarate[datarate];
    }

    // Calculate the resulting payload size
    payloadSize = ( lenN + fOptsLen );

    // Validation of the application payload size
    if( ( payloadSize <= maxN ) && ( payloadSize <= LORAMAC_PHY_MAXPAYLOAD ) )
    {
        return true;
    }
    return false;
}

static uint8_t _count_bits( uint16_t mask, uint8_t nbBits )
{
    uint8_t nbActiveBits = 0;

    for( uint8_t j = 0; j < nbBits; j++ )
    {
        if( ( mask & ( 1 << j ) ) == ( 1 << j ) )
        {
            nbActiveBits++;
        }
    }
    return nbActiveBits;
}

#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
static uint8_t _count_bw125_channels( uint16_t *channelsMask )
{
    uint8_t nb125kHzChannels = 0;

    for( uint8_t i = 0, k = 0; i < LORA_MAX_NB_CHANNELS - 8; i += 16, k++ )
    {
        nb125kHzChannels += _count_bits( channelsMask[k], 16 );
    }

    return nb125kHzChannels;
}

#if defined( USE_BAND_915_HYBRID )
static void _reenable_channels( uint16_t mask, uint16_t* channelMask )
{
    uint16_t blockMask = mask;

    for( uint8_t i = 0, j = 0; i < 4; i++, j += 2 )
    {
        channelMask[i] = 0;
        if( ( blockMask & ( 1 << j ) ) != 0 )
        {
            channelMask[i] |= 0x00FF;
        }
        if( ( blockMask & ( 1 << ( j + 1 ) ) ) != 0 )
        {
            channelMask[i] |= 0xFF00;
        }
    }
    channelMask[4] = blockMask;
    channelMask[5] = 0x0000;
}

static bool _validate_channel_mask( uint16_t* channelMask )
{
    bool chanMaskState = false;
    uint16_t block1 = 0;
    uint16_t block2 = 0;
    uint8_t index = 0;

    for( uint8_t i = 0; i < 4; i++ )
    {
        block1 = channelMask[i] & 0x00FF;
        block2 = channelMask[i] & 0xFF00;

        if( ( _count_bits( block1, 16 ) > 5 ) && ( chanMaskState == false ) )
        {
            channelMask[i] &= block1;
            channelMask[4] = 1 << ( i * 2 );
            chanMaskState = true;
            index = i;
        }
        else if( ( _count_bits( block2, 16 ) > 5 ) && ( chanMaskState == false ) )
        {
            channelMask[i] &= block2;
            channelMask[4] = 1 << ( i * 2 + 1 );
            chanMaskState = true;
            index = i;
        }
    }

    // Do only change the channel mask, if we have found a valid block.
    if( chanMaskState == true )
    {
        for( uint8_t i = 0; i < 4; i++ )
        {
            if( i != index )
            {
                channelMask[i] = 0;
            }
        }
    }
    return chanMaskState;
}
#endif
#endif

static int8_t _limit_tx_power(netdev2_lorawan_t *loradev, int8_t txPower )
{
    int8_t resultTxPower = txPower;
#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
    if( ( loradev->LoRaMacParams.ChannelsDatarate == DR_4 ) ||
        ( ( loradev->LoRaMacParams.ChannelsDatarate >= DR_8 ) && ( loradev->LoRaMacParams.ChannelsDatarate <= DR_13 ) ) )
    {// Limit tx power to max 26dBm
        resultTxPower =  MAX( txPower, TX_POWER_26_DBM );
    }
    else
    {
        if( _count_bw125_channels( loradev->LoRaMacParams.ChannelsMask ) < 50 )
        {// Limit tx power to max 21dBm
            resultTxPower = MAX( txPower, TX_POWER_20_DBM );
        }
    }
#endif
    return resultTxPower;
}

bool _value_in_range( int8_t value, int8_t min, int8_t max )
{
    if( ( value >= min ) && ( value <= max ) )
    {
        return true;
    }
    return false;
}

static bool _disable_channel_in_mask( uint8_t id, uint16_t* mask )
{
    uint8_t index = 0;
    index = id / 16;

    if( ( index > 4 ) || ( id >= LORA_MAX_NB_CHANNELS ) )
    {
        return false;
    }

    // Deactivate channel
    mask[index] &= ~( 1 << ( id % 16 ) );

    return true;
}

static bool AdrNextDr(netdev2_lorawan_t *loradev, bool adrEnabled, bool updateChannelMask, int8_t* datarateOut )
{
    bool adrAckReq = false;
    int8_t datarate = loradev->LoRaMacParams.ChannelsDatarate;

    if( adrEnabled == true )
    {
        if( datarate == LORAMAC_TX_MIN_DATARATE )
        {
            loradev->AdrAckCounter = 0;
            adrAckReq = false;
        }
        else
        {
            if( loradev->AdrAckCounter >= ADR_ACK_LIMIT )
            {
                adrAckReq = true;
            }
            else
            {
                adrAckReq = false;
            }
            if( loradev->AdrAckCounter >= ( ADR_ACK_LIMIT + ADR_ACK_DELAY ) )
            {
                if( ( loradev->AdrAckCounter % ADR_ACK_DELAY ) == 0 )
                {
#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
                    if( datarate > LORAMAC_TX_MIN_DATARATE )
                    {
                        datarate--;
                    }
                    if( datarate == LORAMAC_TX_MIN_DATARATE )
                    {
                        if( updateChannelMask == true )
                        {
                            // Re-enable default channels LC1, LC2, LC3
                            loradev->LoRaMacParams.ChannelsMask[0] = loradev->LoRaMacParams.ChannelsMask[0] | ( LC( 1 ) + LC( 2 ) + LC( 3 ) );
                        }
                    }
#elif defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
                    if( ( datarate > LORAMAC_TX_MIN_DATARATE ) && ( datarate == DR_8 ) )
                    {
                        datarate = DR_4;
                    }
                    else if( datarate > LORAMAC_TX_MIN_DATARATE )
                    {
                        datarate--;
                    }
                    if( datarate == LORAMAC_TX_MIN_DATARATE )
                    {
                        if( updateChannelMask == true )
                        {
#if defined( USE_BAND_915 )
                            // Re-enable default channels
                            loradev->LoRaMacParams.ChannelsMask[0] = 0xFFFF;
                            loradev->LoRaMacParams.ChannelsMask[1] = 0xFFFF;
                            loradev->LoRaMacParams.ChannelsMask[2] = 0xFFFF;
                            loradev->LoRaMacParams.ChannelsMask[3] = 0xFFFF;
                            loradev->LoRaMacParams.ChannelsMask[4] = 0x00FF;
                            loradev->LoRaMacParams.ChannelsMask[5] = 0x0000;
#else // defined( USE_BAND_915_HYBRID )
                            // Re-enable default channels
                            _reenable_channels( loradev->LoRaMacParams.ChannelsMask[4], loradev->LoRaMacParams.ChannelsMask );
                            loradev->LoRaMacParams.ChannelsMask[0] = 0x3C3C;
                            loradev->LoRaMacParams.ChannelsMask[1] = 0x0000;
                            loradev->LoRaMacParams.ChannelsMask[2] = 0x0000;
                            loradev->LoRaMacParams.ChannelsMask[3] = 0x0000;
                            loradev->LoRaMacParams.ChannelsMask[4] = 0x0001;
                            loradev->LoRaMacParams.ChannelsMask[5] = 0x0000;
#endif
                        }
                    }
#else
#error "Please define a frequency band in the compiler options."
#endif
                }
            }
        }
    }

    *datarateOut = datarate;

    return adrAckReq;
}

static LoRaMacStatus_t _add_mac_command(netdev2_lorawan_t *loradev, uint8_t cmd, uint8_t p1, uint8_t p2 )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_BUSY;
    // The maximum buffer length must take MAC commands to re-send into account.
    uint8_t bufLen = LORA_MAC_COMMAND_MAX_LENGTH - loradev->MacCommandsBufferToRepeatIndex;

    switch( cmd )
    {
        case MOTE_MAC_LINK_CHECK_REQ:
            if( loradev->MacCommandsBufferIndex < bufLen )
            {
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = cmd;
                // No payload for this command
                status = LORAMAC_STATUS_OK;
            }
            break;
        case MOTE_MAC_LINK_ADR_ANS:
            if( loradev->MacCommandsBufferIndex < ( bufLen - 1 ) )
            {
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = cmd;
                // Margin
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = p1;
                status = LORAMAC_STATUS_OK;
            }
            break;
        case MOTE_MAC_DUTY_CYCLE_ANS:
            if( loradev->MacCommandsBufferIndex < bufLen )
            {
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = cmd;
                // No payload for this answer
                status = LORAMAC_STATUS_OK;
            }
            break;
        case MOTE_MAC_RX_PARAM_SETUP_ANS:
            if( loradev->MacCommandsBufferIndex < ( bufLen - 1 ) )
            {
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = cmd;
                // Status: Datarate ACK, Channel ACK
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = p1;
                status = LORAMAC_STATUS_OK;
            }
            break;
        case MOTE_MAC_DEV_STATUS_ANS:
            if( loradev->MacCommandsBufferIndex < ( bufLen - 2 ) )
            {
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = cmd;
                // 1st byte Battery
                // 2nd byte Margin
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = p1;
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = p2;
                status = LORAMAC_STATUS_OK;
            }
            break;
        case MOTE_MAC_NEW_CHANNEL_ANS:
            if( loradev->MacCommandsBufferIndex < ( bufLen - 1 ) )
            {
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = cmd;
                // Status: Datarate range OK, Channel frequency OK
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = p1;
                status = LORAMAC_STATUS_OK;
            }
            break;
        case MOTE_MAC_RX_TIMING_SETUP_ANS:
            if( loradev->MacCommandsBufferIndex < bufLen )
            {
                loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex++] = cmd;
                // No payload for this answer
                status = LORAMAC_STATUS_OK;
            }
            break;
        default:
            return LORAMAC_STATUS_SERVICE_UNKNOWN;
    }
    if( status == LORAMAC_STATUS_OK )
    {
        loradev->MacCommandsInNextTx = true;
    }
    return status;
}

static uint8_t _parse_to_repeat_mac_commands( uint8_t* cmdBufIn, uint8_t length, uint8_t* cmdBufOut )
{
    uint8_t i = 0;
    uint8_t cmdCount = 0;

    if( ( cmdBufIn == NULL ) || ( cmdBufOut == NULL ) )
    {
        return 0;
    }

    for( i = 0; i < length; i++ )
    {
        switch( cmdBufIn[i] )
        {
            case MOTE_MAC_RX_PARAM_SETUP_ANS:
            {
                cmdBufOut[cmdCount++] = cmdBufIn[i++];
                cmdBufOut[cmdCount++] = cmdBufIn[i++];
                cmdBufOut[cmdCount++] = cmdBufIn[i];
                break;
            }
            case MOTE_MAC_RX_TIMING_SETUP_ANS:
            {
                cmdBufOut[cmdCount++] = cmdBufIn[i];
                break;
            }
            default:
                break;
        }
    }

    return cmdCount;
}

static void _process_mac_commands(netdev2_lorawan_t *loradev, uint8_t *payload, uint8_t macIndex, uint8_t commandsSize, uint8_t snr )
{
    while( macIndex < commandsSize )
    {
        // Decode Frame MAC commands
        switch( payload[macIndex++] )
        {
            case SRV_MAC_LINK_CHECK_ANS:
                loradev->demod_margin = payload[macIndex++];
                loradev->number_of_gateways = payload[macIndex++];
                gnrc_netapi_dispatch(GNRC_NETTYPE_LORAWAN, GNRC_NETREG_DEMUX_CTX_ALL, GNRC_LORAWAN_LINK_CHECK_MSG, NULL);
                break;
            case SRV_MAC_LINK_ADR_REQ:
                {
                    uint8_t i;
                    uint8_t status = 0x07;
                    uint16_t chMask;
                    int8_t txPower = 0;
                    int8_t datarate = 0;
                    uint8_t nbRep = 0;
                    uint8_t chMaskCntl = 0;
                    uint16_t channelsMask[6] = { 0, 0, 0, 0, 0, 0 };

                    // Initialize local copy of the channels mask array
                    for( i = 0; i < 6; i++ )
                    {
                        channelsMask[i] = loradev->LoRaMacParams.ChannelsMask[i];
                    }
                    datarate = payload[macIndex++];
                    txPower = datarate & 0x0F;
                    datarate = ( datarate >> 4 ) & 0x0F;

                    if( ( loradev->lorawan.tx_rx.adr_ctrl == false ) &&
                        ( ( loradev->LoRaMacParams.ChannelsDatarate != datarate ) || ( loradev->LoRaMacParams.ChannelsTxPower != txPower ) ) )
                    { // ADR disabled don't handle ADR requests if server tries to change datarate or txpower
                        // Answer the server with fail status
                        // Power ACK     = 0
                        // Data rate ACK = 0
                        // Channel mask  = 0
                        _add_mac_command(loradev, MOTE_MAC_LINK_ADR_ANS, 0, 0 );
                        macIndex += 3;  // Skip over the remaining bytes of the request
                        break;
                    }
                    chMask = ( uint16_t )payload[macIndex++];
                    chMask |= ( uint16_t )payload[macIndex++] << 8;

                    nbRep = payload[macIndex++];
                    chMaskCntl = ( nbRep >> 4 ) & 0x07;
                    nbRep &= 0x0F;
                    if( nbRep == 0 )
                    {
                        nbRep = 1;
                    }
#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
                    if( ( chMaskCntl == 0 ) && ( chMask == 0 ) )
                    {
                        status &= 0xFE; // Channel mask KO
                    }
                    else if( ( ( chMaskCntl >= 1 ) && ( chMaskCntl <= 5 )) ||
                             ( chMaskCntl >= 7 ) )
                    {
                        // RFU
                        status &= 0xFE; // Channel mask KO
                    }
                    else
                    {
                        for( i = 0; i < LORA_MAX_NB_CHANNELS; i++ )
                        {
                            if( chMaskCntl == 6 )
                            {
                                if( channels[i].Frequency != 0 )
                                {
                                    chMask |= 1 << i;
                                }
                            }
                            else
                            {
                                if( ( ( chMask & ( 1 << i ) ) != 0 ) &&
                                    ( channels[i].Frequency == 0 ) )
                                {// Trying to enable an undefined channel
                                    status &= 0xFE; // Channel mask KO
                                }
                            }
                        }
                        channelsMask[0] = chMask;
                    }
#elif defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
                    if( chMaskCntl == 6 )
                    {
                        // Enable all 125 kHz channels
                        for( uint8_t i = 0, k = 0; i < LORA_MAX_NB_CHANNELS - 8; i += 16, k++ )
                        {
                            for( uint8_t j = 0; j < 16; j++ )
                            {
                                if( channels[i + j].Frequency != 0 )
                                {
                                    channelsMask[k] |= 1 << j;
                                }
                            }
                        }
                    }
                    else if( chMaskCntl == 7 )
                    {
                        // Disable all 125 kHz channels
                        channelsMask[0] = 0x0000;
                        channelsMask[1] = 0x0000;
                        channelsMask[2] = 0x0000;
                        channelsMask[3] = 0x0000;
                    }
                    else if( chMaskCntl == 5 )
                    {
                        // RFU
                        status &= 0xFE; // Channel mask KO
                    }
                    else
                    {
                        for( uint8_t i = 0; i < 16; i++ )
                        {
                            if( ( ( chMask & ( 1 << i ) ) != 0 ) &&
                                ( channels[chMaskCntl * 16 + i].Frequency == 0 ) )
                            {// Trying to enable an undefined channel
                                status &= 0xFE; // Channel mask KO
                            }
                        }
                        channelsMask[chMaskCntl] = chMask;

                        if( _count_bw125_channels( channelsMask ) < 6 )
                        {
                            status &= 0xFE; // Channel mask KO
                        }

#if defined( USE_BAND_915_HYBRID )
                        if( _validate_channel_mask( channelsMask ) == false )
                        {
                            status &= 0xFE; // Channel mask KO
                        }
#endif
                    }
#else
    #error "Please define a frequency band in the compiler options."
#endif
                    if( _value_in_range( datarate, LORAMAC_TX_MIN_DATARATE, LORAMAC_TX_MAX_DATARATE ) == false )
                    {
                        status &= 0xFD; // Datarate KO
                    }

                    //
                    // Remark MaxTxPower = 0 and MinTxPower = 5
                    //
                    if( _value_in_range( txPower, LORAMAC_MAX_TX_POWER, LORAMAC_MIN_TX_POWER ) == false )
                    {
                        status &= 0xFB; // TxPower KO
                    }
                    if( ( status & 0x07 ) == 0x07 )
                    {
                        loradev->LoRaMacParams.ChannelsDatarate = datarate;
                        loradev->LoRaMacParams.ChannelsTxPower = txPower;

                        loradev->LoRaMacParams.ChannelsMask[0] = channelsMask[0];
                        loradev->LoRaMacParams.ChannelsMask[1] = channelsMask[1];
                        loradev->LoRaMacParams.ChannelsMask[2] = channelsMask[2];
                        loradev->LoRaMacParams.ChannelsMask[3] = channelsMask[3];
                        loradev->LoRaMacParams.ChannelsMask[4] = channelsMask[4];
                        loradev->LoRaMacParams.ChannelsMask[5] = channelsMask[5];

                        loradev->LoRaMacParams.ChannelsNbRep = nbRep;
                    }
                    _add_mac_command(loradev, MOTE_MAC_LINK_ADR_ANS, status, 0 );
                }
                break;
            case SRV_MAC_DUTY_CYCLE_REQ:
                loradev->MaxDCycle = payload[macIndex++];
                loradev->AggregatedDCycle = 1 << loradev->MaxDCycle;
                _add_mac_command(loradev, MOTE_MAC_DUTY_CYCLE_ANS, 0, 0 );
                break;
            case SRV_MAC_RX_PARAM_SETUP_REQ:
                {
                    uint8_t status = 0x07;
                    int8_t datarate = 0;
                    int8_t drOffset = 0;
                    uint32_t freq = 0;

                    drOffset = ( payload[macIndex] >> 4 ) & 0x07;
                    datarate = payload[macIndex] & 0x0F;
                    macIndex++;

                    freq =  ( uint32_t )payload[macIndex++];
                    freq |= ( uint32_t )payload[macIndex++] << 8;
                    freq |= ( uint32_t )payload[macIndex++] << 16;
                    freq *= 100;

                    if( _rx2_freq_in_range( freq ) == false )
                    {
                        status &= 0xFE; // Channel frequency KO
                    }

                    if( _value_in_range( datarate, LORAMAC_RX_MIN_DATARATE, LORAMAC_RX_MAX_DATARATE ) == false )
                    {
                        status &= 0xFD; // Datarate KO
                    }
#if ( defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID ) )
                    if( ( _value_in_range( datarate, DR_5, DR_7 ) == true ) ||
                        ( datarate > DR_13 ) )
                    {
                        status &= 0xFD; // Datarate KO
                    }
#endif
                    if( _value_in_range( drOffset, LORAMAC_MIN_RX1_DR_OFFSET, LORAMAC_MAX_RX1_DR_OFFSET ) == false )
                    {
                        status &= 0xFB; // Rx1DrOffset range KO
                    }

                    if( ( status & 0x07 ) == 0x07 )
                    {
                        loradev->LoRaMacParams.Rx2Channel.Datarate = datarate;
                        loradev->LoRaMacParams.Rx2Channel.Frequency = freq;
                        loradev->LoRaMacParams.Rx1DrOffset = drOffset;
                    }
                    _add_mac_command(loradev, MOTE_MAC_RX_PARAM_SETUP_ANS, status, 0 );
                }
                break;
            case SRV_MAC_NEW_CHANNEL_REQ:
                {
                    uint8_t status = 0x03;

#if ( defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID ) )
                    status &= 0xFC; // Channel frequency and datarate KO
                    macIndex += 5;
#else
                    int8_t channelIndex = 0;
                    ChannelParams_t chParam;

                    channelIndex = payload[macIndex++];
                    chParam.Frequency = ( uint32_t )payload[macIndex++];
                    chParam.Frequency |= ( uint32_t )payload[macIndex++] << 8;
                    chParam.Frequency |= ( uint32_t )payload[macIndex++] << 16;
                    chParam.Frequency *= 100;
                    chParam.DrRange.Value = payload[macIndex++];

                    if( chParam.Frequency == 0 )
                    {
                        if( channelIndex < 3 )
                        {
                            status &= 0xFC;
                        }
                        else
                        {
                            if( channel_remove(loradev, channelIndex ) != LORAMAC_STATUS_OK )
                            {
                                status &= 0xFC;
                            }
                        }
                    }
                    else
                    {
                        switch( channel_add(loradev, channelIndex, chParam ) )
                        {
                            case LORAMAC_STATUS_OK:
                            {
                                break;
                            }
                            case LORAMAC_STATUS_FREQUENCY_INVALID:
                            {
                                status &= 0xFE;
                                break;
                            }
                            case LORAMAC_STATUS_DATARATE_INVALID:
                            {
                                status &= 0xFD;
                                break;
                            }
                            case LORAMAC_STATUS_FREQ_AND_DR_INVALID:
                            {
                                status &= 0xFC;
                                break;
                            }
                            default:
                            {
                                status &= 0xFC;
                                break;
                            }
                        }
                    }
#endif
                    _add_mac_command(loradev, MOTE_MAC_NEW_CHANNEL_ANS, status, 0 );
                }
                break;
            case SRV_MAC_RX_TIMING_SETUP_REQ:
                {
                    uint8_t delay = payload[macIndex++] & 0x0F;

                    if( delay == 0 )
                    {
                        delay++;
                    }
                    loradev->LoRaMacParams.ReceiveDelay1 = delay * 1e3;
                    loradev->LoRaMacParams.ReceiveDelay2 = loradev->LoRaMacParams.ReceiveDelay1 + 1e3;
                    _add_mac_command(loradev, MOTE_MAC_RX_TIMING_SETUP_ANS, 0, 0 );
                }
                break;
            default:
                // Unknown command. ABORT MAC commands processing
                return;
        }
    }
}


static LoRaMacStatus_t _schedule_next_tx(netdev2_lorawan_t *loradev)
{
    TimerTime_t dutyCycleTimeOff = 0;

    // Check if the device is off
    if( loradev->MaxDCycle == 255 )
    {
        return LORAMAC_STATUS_DEVICE_OFF;
    }
    if( loradev->MaxDCycle == 0 )
    {
        loradev->AggregatedTimeOff = 0;
    }

    _calculate_backoff(loradev, loradev->LastTxChannel );

    // Select channel
    while( _set_next_channel(loradev, &dutyCycleTimeOff ) == false )
    {
        // Set the default datarate
        loradev->LoRaMacParams.ChannelsDatarate = loradev->LoRaMacParamsDefaults.ChannelsDatarate;

#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
        // Re-enable default channels LC1, LC2, LC3
        loradev->LoRaMacParams.ChannelsMask[0] = loradev->LoRaMacParams.ChannelsMask[0] | ( LC( 1 ) + LC( 2 ) + LC( 3 ) );
#endif
    }

    // Schedule transmission of frame
    if( dutyCycleTimeOff == 0 )
    {
        // Try to send now
        return _send_on_channel(loradev, channels[loradev->Channel] );
    }
    else
    {
        // Send later - prepare timer
        loradev->TxDelayedTimer.msg.type = LORAWAN_TIMER_TX_DELAYED;
        xtimer_set_msg(&(loradev->TxDelayedTimer.dev), xtimer_ticks_from_usec(dutyCycleTimeOff*1000).ticks32, &(loradev->TxDelayedTimer.msg), loradev->TxDelayedTimer.pid);
        //TimerSetValue( &dev->TxDelayedTimer, dutyCycleTimeOff, LORAWAN_TIMER_TX_DELAYED);
        //TimerStart( &dev->TxDelayedTimer, 0);

        return LORAMAC_STATUS_OK;
    }
}

static uint16_t _set_retransmission_duty_cycle( void )
{
    uint16_t dutyCycle = 0;

#if defined( USE_BAND_868 ) || defined( USE_BAND_433 ) || defined( USE_BAND_780 )
    TimerTime_t timeElapsed = xtimer_now_usec64();

    if( timeElapsed < 3600000 )
    {
        dutyCycle = BACKOFF_DC_1_HOUR;
    }
    else if( timeElapsed < ( 3600000 + 36000000 ) )
    {
        dutyCycle = BACKOFF_DC_10_HOURS;
    }
    else
    {
        dutyCycle = BACKOFF_DC_24_HOURS;
    }
#endif
    return dutyCycle;
}

static void _calculate_backoff(netdev2_lorawan_t *loradev, uint8_t channel )
{
    uint16_t dutyCycle = bands[channels[channel].Band].DCycle;
    uint16_t joinDutyCycle = 0;
    bool rndTimeOff = false;

    if( loradev->lorawan.tx_rx.nwk_status == false )
    {
        joinDutyCycle = _set_retransmission_duty_cycle( );
        dutyCycle = MAX( dutyCycle, joinDutyCycle );

        // Make sure to not apply the random back-off to the first TX
        if( loradev->TxTimeOnAir > 0 )
        {
            rndTimeOff = true;
        }
    }

    // Update Band Time OFF
    if( loradev->DutyCycleOn == true )
    {
        bands[channels[channel].Band].TimeOff = loradev->TxTimeOnAir * dutyCycle - loradev->TxTimeOnAir;
    }
    else
    {
        bands[channels[channel].Band].TimeOff = 0;
    }

    if( rndTimeOff == true )
    {
        bands[channels[channel].Band].TimeOff = random_uint32_range( bands[channels[channel].Band].TimeOff,
                                                       bands[channels[channel].Band].TimeOff + BACKOFF_RND_OFFSET + 1);
    }

    // Update Aggregated Time OFF
    loradev->AggregatedTimeOff = loradev->AggregatedTimeOff + ( loradev->TxTimeOnAir * loradev->AggregatedDCycle - loradev->TxTimeOnAir );
}

static int8_t join_request_alternate_datarate(netdev2_lorawan_t *loradev, uint16_t nbTrials )
{
    int8_t datarate = LORAMAC_TX_MIN_DATARATE;
#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
#if defined( USE_BAND_915 )
    // Re-enable 500 kHz default channels
    loradev->LoRaMacParams.ChannelsMask[4] = 0x00FF;
#else // defined( USE_BAND_915_HYBRID )
    // Re-enable 500 kHz default channels
    //_reenable_channels( loradev->LoRaMacParams.ChannelsMask[4], loradev->LoRaMacParams.ChannelsMask );
    loradev->LoRaMacParams.ChannelsMask[4] = 0x0001;
#endif

    if( ( nbTrials & 0x01 ) == 0x01 )
    {
        datarate = DR_4;
    }
    else
    {
        datarate = DR_1;
    }
#else
    if( ( nbTrials % 48 ) == 0 )
    {
        datarate = DR_0;
    }
    else if( ( nbTrials % 32 ) == 0 )
    {
        datarate = DR_1;
    }
    else if( ( nbTrials % 24 ) == 0 )
    {
        datarate = DR_2;
    }
    else if( ( nbTrials % 16 ) == 0 )
    {
        datarate = DR_3;
    }
    else if( ( nbTrials % 8 ) == 0 )
    {
        datarate = DR_4;
    }
    else
    {
        datarate = DR_5;
    }
#endif
    return datarate;
}

static void _reset_mac(netdev2_lorawan_t *loradev)
{
    loradev->lorawan.tx_rx.nwk_status = false;

    // Counters
    loradev->uplink_counter = 1;
    loradev->DownLinkCounter = 0;
    loradev->AdrAckCounter = 0;

    loradev->ChannelsNbRepCounter = 0;

    loradev->ack_timeout_retries = 1;
    loradev->ack_timeout_retries_counter = 1;

    loradev->MaxDCycle = 0;
    loradev->AggregatedDCycle = 1;

    loradev->MacCommandsBufferIndex = 0;
    loradev->MacCommandsBufferToRepeatIndex = 0;

    loradev->IsRxWindowsEnabled = true;

    loradev->LoRaMacParams.ChannelsTxPower = loradev->LoRaMacParamsDefaults.ChannelsTxPower;
    loradev->LoRaMacParams.ChannelsDatarate = loradev->LoRaMacParamsDefaults.ChannelsDatarate;

    loradev->LoRaMacParams.MaxRxWindow = loradev->LoRaMacParamsDefaults.MaxRxWindow;
    loradev->LoRaMacParams.ReceiveDelay1 = loradev->LoRaMacParamsDefaults.ReceiveDelay1;
    loradev->LoRaMacParams.ReceiveDelay2 = loradev->LoRaMacParamsDefaults.ReceiveDelay2;
    loradev->LoRaMacParams.JoinAcceptDelay1 = loradev->LoRaMacParamsDefaults.JoinAcceptDelay1;
    loradev->LoRaMacParams.JoinAcceptDelay2 = loradev->LoRaMacParamsDefaults.JoinAcceptDelay2;

    loradev->LoRaMacParams.Rx1DrOffset = loradev->LoRaMacParamsDefaults.Rx1DrOffset;
    loradev->LoRaMacParams.ChannelsNbRep = loradev->LoRaMacParamsDefaults.ChannelsNbRep;

    loradev->LoRaMacParams.Rx2Channel = loradev->LoRaMacParamsDefaults.Rx2Channel;

    memcpy( ( uint8_t* ) loradev->LoRaMacParams.ChannelsMask, ( uint8_t* ) loradev->LoRaMacParamsDefaults.ChannelsMask, sizeof( loradev->LoRaMacParams.ChannelsMask ) );

#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
    memcpy( ( uint8_t* ) loradev->ChannelsMaskRemaining, ( uint8_t* ) loradev->LoRaMacParamsDefaults.ChannelsMask, sizeof( loradev->LoRaMacParams.ChannelsMask ) );
#endif


    loradev->NodeAckRequested = false;
    loradev->SrvAckRequested = false;
    loradev->MacCommandsInNextTx = false;

    // Reset Multicast downlink counters
    MulticastParams_t *cur = loradev->MulticastChannels;
    while( cur != NULL )
    {
        cur->DownLinkCounter = 0;
        cur = cur->Next;
    }

    // Initialize channel index.
    loradev->Channel = LORA_MAX_NB_CHANNELS;
}

int gnrc_lorawan_send_join_request(netdev2_lorawan_t *loradev)
{
    netdev2_t *netdev = (netdev2_t*) loradev;
    lw_hdr_t hdr;

    hdr.mt_maj = 0;
    lw_hdr_set_mtype(&hdr, FRAME_TYPE_JOIN_REQ);

    lw_hdr_set_adr(&hdr, loradev->lorawan.tx_rx.adr_ctrl);
    uint32_t random;
    uint32_t mic=0;

    loradev->NodeAckRequested = false;

    lorawan_join_request_t *jr_hdr;
    loradev->pkt = gnrc_pktbuf_add(NULL, NULL, sizeof(lorawan_join_request_t), GNRC_NETTYPE_LORAWAN);
    *((uint8_t*) loradev->pkt->data) = hdr.mt_maj;
    jr_hdr = loradev->pkt->data;
    loradev->RxWindow1Delay = loradev->LoRaMacParams.JoinAcceptDelay1 - RADIO_WAKEUP_TIME;
    loradev->RxWindow2Delay = loradev->LoRaMacParams.JoinAcceptDelay2 - RADIO_WAKEUP_TIME;

    memcpyr( (uint8_t*) &jr_hdr->app_eui, loradev->app_eui, 8 );
    memcpyr( (uint8_t*) &jr_hdr->dev_eui, loradev->dev_eui, 8 );

    netdev->driver->get(netdev, NETOPT_LORA_RANDOM, &random, sizeof(uint32_t));
    loradev->dev_nonce = random;

    jr_hdr->dev_nonce = byteorder_btols(byteorder_htons(loradev->dev_nonce));

    LoRaMacJoinComputeMic( (uint8_t*) loradev->pkt->data, sizeof(lorawan_join_request_t)-4, loradev->app_key, &mic );

    jr_hdr->mic = byteorder_btoll(byteorder_htonl(mic));
    loradev->join_req_sent = true;

    int status = _schedule_next_tx(loradev);
    return status;
}

void _set_tx_config(netdev2_lorawan_t *loradev, RadioModems_t modem, int8_t power, uint32_t fdev,
                        uint32_t bandwidth, uint32_t datarate,
                        uint8_t coderate, uint16_t preambleLen,
                        bool fixLen, bool crcOn, bool freqHopOn,
                        uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
{
    (void) fdev;

    netdev2_t *netdev = (netdev2_t*) loradev;

    netopt_enable_t _modem = modem;
    netdev->driver->set(netdev, NETOPT_LORA_MODE, &_modem, sizeof(netopt_enable_t));

    bool freq_hop_on = freqHopOn;
    bool iq_invert = iqInverted;
    uint8_t rx_single = false;
    uint32_t tx_timeout = timeout * 1000;
    uint8_t bw = bandwidth + 7;
    uint8_t cr = coderate;
    uint8_t sf = datarate;
    bool implicit = fixLen;
    bool crc = crcOn;
    uint16_t rx_timeout = 10;
    uint16_t preamble = preambleLen;
    uint8_t payload_len = 0;
    uint8_t hop_period = hopPeriod;

    netdev->driver->set(netdev, NETOPT_LORA_HOP, &freq_hop_on, sizeof(bool));
    netdev->driver->set(netdev, NETOPT_LORA_IQ_INVERT, &iq_invert, sizeof(bool));
    netdev->driver->set(netdev, NETOPT_LORA_SINGLE_RECEIVE, &rx_single, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_TX_TIMEOUT, &tx_timeout, sizeof(uint32_t));

    netdev->driver->set(netdev, NETOPT_LORA_BANDWIDTH, &bw, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_CODING_RATE, &cr, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_SPREADING_FACTOR, &sf, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_IMPLICIT, &implicit, sizeof(bool));
    netdev->driver->set(netdev, NETOPT_CRC, &crc, sizeof(bool));
    netdev->driver->set(netdev, NETOPT_LORA_SYMBOL_TIMEOUT, &rx_timeout, sizeof(uint16_t));
    netdev->driver->set(netdev, NETOPT_LORA_PREAMBLE_LENGTH, &preamble, sizeof(uint16_t));
    netdev->driver->set(netdev, NETOPT_LORA_PAYLOAD_LENGTH, &payload_len, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_LORA_HOP_PERIOD, &hop_period, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_TX_POWER, &power, sizeof(uint8_t));

}
LoRaMacStatus_t _send_on_channel(netdev2_lorawan_t *loradev, ChannelParams_t channel )
{
    netdev2_t *netdev = (netdev2_t*) loradev;
    int8_t datarate = lorawan_datarates[loradev->LoRaMacParams.ChannelsDatarate];
    int8_t txPowerIndex = 0;
    int8_t txPower = 0;
    uint8_t max_payload;

    txPowerIndex = _limit_tx_power(loradev, loradev->LoRaMacParams.ChannelsTxPower );
    txPower = lorawan_tx_power[txPowerIndex];

    netdev->driver->set(netdev, NETOPT_CHANNEL, &channel.Frequency, sizeof(uint32_t));
    netopt_enable_t lm;

    max_payload = gnrc_pkt_len(loradev->pkt);
    uint8_t pkt_len = (uint8_t) max_payload;
    uint32_t time_on_air;
    netdev->driver->set(netdev, NETOPT_LORA_TIME_ON_AIR, &pkt_len, sizeof(uint8_t));
#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
    if( loradev->LoRaMacParams.ChannelsDatarate == DR_7 )
    { // High Speed FSK channel
        lm = NETOPT_DISABLE;
        netdev->driver->set(netdev, NETOPT_LORA_MODE, &lm, sizeof(netopt_enable_t));
        netdev->driver->set(netdev, NETOPT_LORA_MAX_PAYLOAD, &max_payload, sizeof(uint8_t));
        _set_tx_config(loradev, MODEM_FSK, txPower, 25e3, 0, datarate * 1e3, 0, 5, false, true, 0, 0, false, 3e3 );
        netdev->driver->get(netdev, NETOPT_LORA_TIME_ON_AIR, &time_on_air, sizeof(uint32_t));
        loradev->TxTimeOnAir = time_on_air;

    }
    else if( loradev->LoRaMacParams.ChannelsDatarate == DR_6 )
    { // High speed LoRa channel
        lm = NETOPT_ENABLE;
        netdev->driver->set(netdev, NETOPT_LORA_MODE, &lm, sizeof(netopt_enable_t));
        netdev->driver->set(netdev, NETOPT_LORA_MAX_PAYLOAD, &max_payload, sizeof(uint8_t));
        _set_tx_config(loradev, MODEM_LORA, txPower, 0, 1, datarate, 1, 8, false, true, 0, 0, false, 3e3 );
        netdev->driver->get(netdev, NETOPT_LORA_TIME_ON_AIR, &time_on_air, sizeof(uint32_t));
        loradev->TxTimeOnAir = time_on_air;
    }
    else
    { // Normal LoRa channel
        lm = NETOPT_ENABLE;
        netdev->driver->set(netdev, NETOPT_LORA_MODE, &lm, sizeof(netopt_enable_t));
        netdev->driver->set(netdev, NETOPT_LORA_MAX_PAYLOAD, &max_payload, sizeof(uint8_t));
        _set_tx_config(loradev, MODEM_LORA, txPower, 0, 0, datarate, 1, 8, false, true, 0, 0, false, 3e3 );
        netdev->driver->get(netdev, NETOPT_LORA_TIME_ON_AIR, &time_on_air, sizeof(uint32_t));
        loradev->TxTimeOnAir = time_on_air;
    }
#elif defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
    lm = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_LORA_MODE, &lm, sizeof(netopt_enable_t));
    netdev->driver->set(netdev, NETOPT_LORA_MAX_PAYLOAD, &max_payload, sizeof(uint8_t));
    if( loradev->LoRaMacParams.ChannelsDatarate >= DR_4 )
    { // High speed LoRa channel BW500 kHz
        _set_tx_config(loradev, MODEM_LORA, txPower, 0, 2, datarate, 1, 8, false, true, 0, 0, false, 3e3 );
        netdev->driver->get(netdev, NETOPT_LORA_TIME_ON_AIR, &time_on_air, sizeof(uint32_t));
        loradev->TxTimeOnAir = time_on_air;
    }
    else
    { // Normal LoRa channel
        _set_tx_config(loradev, MODEM_LORA, txPower, 0, 0, datarate, 1, 8, false, true, 0, 0, false, 3e3 );
        netdev->driver->get(netdev, NETOPT_LORA_TIME_ON_AIR, &time_on_air, sizeof(uint32_t));
        loradev->TxTimeOnAir = time_on_air;
    }
#else
    #error "Please define a frequency band in the compiler options."
#endif

    size_t n;
    struct iovec *vec = (struct iovec*) gnrc_pktbuf_get_iovec(loradev->pkt, &n)->data;
    if(vec)
    {
        netdev->driver->send(netdev, vec, n); 
    }

    return LORAMAC_STATUS_OK;
}

int gnrc_lorawan_reset(netdev2_lorawan_t *loradev, kernel_pid_t mac_pid)
{
    loradev->MulticastChannels = NULL;
    loradev->uplink_counter = 0;
    loradev->DownLinkCounter = 0;
    loradev->IsUpLinkCounterFixed = false;
    loradev->IsRxWindowsEnabled = true;
    loradev->AdrAckCounter = 0;
    loradev->NodeAckRequested = false;
    loradev->SrvAckRequested = false;
    loradev->MacCommandsInNextTx = false;
    loradev->MacCommandsBufferIndex = 0;
    loradev->MacCommandsBufferToRepeatIndex = 0;
    loradev->ChannelsNbRepCounter = 0;
    loradev->MaxDCycle = 0;
    loradev->ack_timeout_retries = 1;
    loradev->ack_timeout_retries_counter = 1;
    loradev->TxTimeOnAir = 0;
    loradev->rx_slot = 0;
    loradev->join_req_sent = false;

    loradev->LoRaMacDeviceClass = CLASS_A;

    loradev->join_request_trials = 0;
    loradev->RepeaterSupport = false;

    // Reset duty cycle times
    loradev->AggregatedLastTxDoneTime = 0;
    loradev->AggregatedTimeOff = 0;

    // Duty cycle
#if defined( USE_BAND_433 )
    loradev->DutyCycleOn = false;
#elif defined( USE_BAND_780 )
    loradev->DutyCycleOn = false;
#elif defined( USE_BAND_868 )
    loradev->DutyCycleOn = true;
#elif defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
    loradev->DutyCycleOn = false;
#else
    #error "Please define a frequency band in the compiler options."
#endif

    // Reset to defaults
    loradev->LoRaMacParamsDefaults.ChannelsTxPower = LORAMAC_DEFAULT_TX_POWER;
    loradev->LoRaMacParamsDefaults.ChannelsDatarate = LORAMAC_DEFAULT_DATARATE;

    loradev->LoRaMacParamsDefaults.MaxRxWindow = MAX_RX_WINDOW;
    loradev->LoRaMacParamsDefaults.ReceiveDelay1 = RECEIVE_DELAY1;
    loradev->LoRaMacParamsDefaults.ReceiveDelay2 = RECEIVE_DELAY2;
    loradev->LoRaMacParamsDefaults.JoinAcceptDelay1 = JOIN_ACCEPT_DELAY1;
    loradev->LoRaMacParamsDefaults.JoinAcceptDelay2 = JOIN_ACCEPT_DELAY2;

    loradev->LoRaMacParamsDefaults.ChannelsNbRep = 1;
    loradev->LoRaMacParamsDefaults.Rx1DrOffset = 0;

    loradev->LoRaMacParamsDefaults.Rx2Channel = ( Rx2ChannelParams_t )RX_WND_2_CHANNEL;

    // Channel mask
#if defined( USE_BAND_433 )
    loradev->LoRaMacParamsDefaults.ChannelsMask[0] = LC( 1 ) + LC( 2 ) + LC( 3 );
#elif defined( USE_BAND_780 )
    loradev->LoRaMacParamsDefaults.ChannelsMask[0] = LC( 1 ) + LC( 2 ) + LC( 3 );
#elif defined( USE_BAND_868 )
    loradev->LoRaMacParamsDefaults.ChannelsMask[0] = LC( 1 ) + LC( 2 ) + LC( 3 );
#elif defined( USE_BAND_915 )
    loradev->LoRaMacParamsDefaults.ChannelsMask[0] = 0xFFFF;
    loradev->LoRaMacParamsDefaults.ChannelsMask[1] = 0xFFFF;
    loradev->LoRaMacParamsDefaults.ChannelsMask[2] = 0xFFFF;
    loradev->LoRaMacParamsDefaults.ChannelsMask[3] = 0xFFFF;
    loradev->LoRaMacParamsDefaults.ChannelsMask[4] = 0x00FF;
    loradev->LoRaMacParamsDefaults.ChannelsMask[5] = 0x0000;
#elif defined( USE_BAND_915_HYBRID )
    //_reenable_channels( loradev->LoRaMacParams.ChannelsMask[4], loradev->LoRaMacParams.ChannelsMask );
    loradev->LoRaMacParamsDefaults.ChannelsMask[0] = 0x3C3C;
    loradev->LoRaMacParamsDefaults.ChannelsMask[1] = 0x0000;
    loradev->LoRaMacParamsDefaults.ChannelsMask[2] = 0x0000;
    loradev->LoRaMacParamsDefaults.ChannelsMask[3] = 0x0000;
    loradev->LoRaMacParamsDefaults.ChannelsMask[4] = 0x0001;
    loradev->LoRaMacParamsDefaults.ChannelsMask[5] = 0x0000;
#else
    #error "Please define a frequency band in the compiler options."
#endif

#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
    // 125 kHz channels
    for( uint8_t i = 0; i < LORA_MAX_NB_CHANNELS - 8; i++ )
    {
        channels[i].Frequency = 902.3e6 + i * 200e3;
        channels[i].DrRange.Value = ( DR_3 << 4 ) | DR_0;
        channels[i].Band = 0;
    }
    // 500 kHz channels
    for( uint8_t i = LORA_MAX_NB_CHANNELS - 8; i < LORA_MAX_NB_CHANNELS; i++ )
    {
        channels[i].Frequency = 903.0e6 + ( i - ( LORA_MAX_NB_CHANNELS - 8 ) ) * 1.6e6;
        channels[i].DrRange.Value = ( DR_4 << 4 ) | DR_4;
        channels[i].Band = 0;
    }
#endif

    _reset_mac(loradev);

    // Initialize timers

    //TimerInit( &dev->TxDelayedTimer, on_tx_delay_timeout, mac_pid );
    loradev->TxDelayedTimer.dev.target = 0;
    loradev->TxDelayedTimer.dev.callback =  (void*) on_tx_delay_timeout;
    loradev->TxDelayedTimer.pid =  mac_pid;

    //TimerInit( &dev->RxWindowTimer1, on_rx1_timeout, mac_pid );
    loradev->RxWindowTimer1.dev.target = 0;
    loradev->RxWindowTimer1.dev.callback =  (void*) on_rx1_timeout;
    loradev->RxWindowTimer1.pid = mac_pid;

    //TimerInit( &RxWindowTimer2, on_rx2_timeout, mac_pid );
    loradev->RxWindowTimer2.dev.target = 0;
    loradev->RxWindowTimer2.dev.callback =  (void*) on_rx2_timeout;
    loradev->RxWindowTimer2.pid = mac_pid;

    //TimerInit( &AckTimeoutTimer, on_ack_timeout, mac_pid );
    loradev->AckTimeoutTimer.dev.target = 0;
    loradev->AckTimeoutTimer.dev.callback =  (void*) on_ack_timeout;
    loradev->AckTimeoutTimer.pid = mac_pid;


    // Random seed initialization

    netdev2_t *netdev = (netdev2_t*) loradev;
    uint32_t random;
    netdev->driver->get(netdev, NETOPT_LORA_RANDOM, &random, sizeof(uint32_t));
    random_init(random);

    loradev->lorawan.public = true;
    _set_public_network(loradev, loradev->lorawan.public );
    netopt_state_t state = NETOPT_STATE_SLEEP;
    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));

    return LORAMAC_STATUS_OK;
}

void lorawan_set_class(netdev2_lorawan_t *loradev, int class)
{
    netdev2_t *netdev = (netdev2_t*) loradev;
    netopt_state_t state = NETOPT_STATE_SLEEP;
    loradev->LoRaMacDeviceClass = class;
    switch(class)
    {
        case CLASS_A:
        {
            // Set the radio into sleep to setup a defined state
            netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(netopt_state_t));
            break;
        }
        case CLASS_B:
        {
            break;
        }
        case CLASS_C:
        {
            // Set the dev->NodeAckRequested indicator to default
            loradev->NodeAckRequested = false;
            /* Currently there's no netdev pointer here... it should be fixed later. Let's put an assert for now */
            assert(false);
            //on_rx2_timeout(netdev);
            break;
        }
    }
}
void _set_channel_mask(netdev2_lorawan_t *loradev, uint16_t *mask)
{
    uint8_t status;
    (void) status;
#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
    bool chanMaskState = true;

#if defined( USE_BAND_915_HYBRID )
    chanMaskState = _validate_channel_mask(mask);
#endif
    if( chanMaskState == true )
    {
        if( ( _count_bw125_channels(mask) < 6 ) &&
            ( _count_bw125_channels(mask) > 0 ) )
        {
            status = LORAMAC_STATUS_PARAMETER_INVALID;
        }
        else
        {
            memcpy( ( uint8_t* ) loradev->LoRaMacParams.ChannelsMask,
                     ( uint8_t* ) mask, sizeof(mask) );
            for ( uint8_t i = 0; i < sizeof( loradev->LoRaMacParams.ChannelsMask ) / 2; i++ )
            {
                // Disable channels which are no longer available
                loradev->ChannelsMaskRemaining[i] &= loradev->LoRaMacParams.ChannelsMask[i];
            }
        }
    }
    else
    {
        status = LORAMAC_STATUS_PARAMETER_INVALID;
    }
#else
    memcpy( ( uint8_t* ) loradev->LoRaMacParams.ChannelsMask,
             mask, 2 );
#endif
}
LoRaMacStatus_t channel_add(netdev2_lorawan_t *loradev, uint8_t id, ChannelParams_t params )
{
#if ( defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID ) )
    return LORAMAC_STATUS_PARAMETER_INVALID;
#else
    bool datarateInvalid = false;
    bool frequencyInvalid = false;
    uint8_t band = 0;

    // The id must not exceed LORA_MAX_NB_CHANNELS
    if( id >= LORA_MAX_NB_CHANNELS )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    // Validate the datarate
    if( ( params.DrRange.Fields.Min > params.DrRange.Fields.Max ) ||
        ( _value_in_range( params.DrRange.Fields.Min, LORAMAC_TX_MIN_DATARATE,
                        LORAMAC_TX_MAX_DATARATE ) == false ) ||
        ( _value_in_range( params.DrRange.Fields.Max, LORAMAC_TX_MIN_DATARATE,
                        LORAMAC_TX_MAX_DATARATE ) == false ) )
    {
        datarateInvalid = true;
    }

#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
    if( id < 3 )
    {
        if( params.Frequency != channels[id].Frequency )
        {
            frequencyInvalid = true;
        }

        if( params.DrRange.Fields.Min > loradev->LoRaMacParamsDefaults.ChannelsDatarate )
        {
            datarateInvalid = true;
        }
        if( _value_in_range( params.DrRange.Fields.Max, DR_5, LORAMAC_TX_MAX_DATARATE ) == false )
        {
            datarateInvalid = true;
        }
    }
#endif

    // Validate the frequency
    if( ( check_rf_freq( params.Frequency ) == true ) && ( params.Frequency > 0 ) && ( frequencyInvalid == false ) )
    {
#if defined( USE_BAND_868 )
        if( ( params.Frequency >= 865000000 ) && ( params.Frequency <= 868000000 ) )
        {
            band = BAND_G1_0;
        }
        else if( ( params.Frequency > 868000000 ) && ( params.Frequency <= 868600000 ) )
        {
            band = BAND_G1_1;
        }
        else if( ( params.Frequency >= 868700000 ) && ( params.Frequency <= 869200000 ) )
        {
            band = BAND_G1_2;
        }
        else if( ( params.Frequency >= 869400000 ) && ( params.Frequency <= 869650000 ) )
        {
            band = BAND_G1_3;
        }
        else if( ( params.Frequency >= 869700000 ) && ( params.Frequency <= 870000000 ) )
        {
            band = BAND_G1_4;
        }
        else
        {
            frequencyInvalid = true;
        }
#endif
    }
    else
    {
        frequencyInvalid = true;
    }

    if( ( datarateInvalid == true ) && ( frequencyInvalid == true ) )
    {
        return LORAMAC_STATUS_FREQ_AND_DR_INVALID;
    }
    if( datarateInvalid == true )
    {
        return LORAMAC_STATUS_DATARATE_INVALID;
    }
    if( frequencyInvalid == true )
    {
        return LORAMAC_STATUS_FREQUENCY_INVALID;
    }

    // Every parameter is valid, activate the channel
    channels[id] = params;
    channels[id].Band = band;
    loradev->LoRaMacParams.ChannelsMask[0] |= ( 1 << id );

    return LORAMAC_STATUS_OK;
#endif
}

LoRaMacStatus_t channel_remove(netdev2_lorawan_t *loradev, uint8_t id )
{
#if defined( USE_BAND_433 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )

    if( ( id < 3 ) || ( id >= LORA_MAX_NB_CHANNELS ) )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    else
    {
        // Remove the channel from the list of channels
        channels[id] = ( ChannelParams_t ){ 0, { 0 }, 0 };

        // Disable the channel as it doesn't exist anymore
        if( _disable_channel_in_mask( id, loradev->LoRaMacParams.ChannelsMask ) == false )
        {
            return LORAMAC_STATUS_PARAMETER_INVALID;
        }
    }
    return LORAMAC_STATUS_OK;
#elif ( defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID ) )
    return LORAMAC_STATUS_PARAMETER_INVALID;
#endif
}

LoRaMacStatus_t _multicast_channel_link(netdev2_lorawan_t *loradev, MulticastParams_t *channelParam )
{
    if( channelParam == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    // Reset downlink counter
    channelParam->DownLinkCounter = 0;

    if( loradev->MulticastChannels == NULL )
    {
        // New node is the fist element
        loradev->MulticastChannels = channelParam;
    }
    else
    {
        MulticastParams_t *cur = loradev->MulticastChannels;

        // Search the last node in the list
        while( cur->Next != NULL )
        {
            cur = cur->Next;
        }
        // This function always finds the last node
        cur->Next = channelParam;
    }

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t _multicast_channel_unlink(netdev2_lorawan_t *loradev, MulticastParams_t *channelParam )
{
    if( channelParam == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    if( loradev->MulticastChannels != NULL )
    {
        if( loradev->MulticastChannels == channelParam )
        {
          // First element
          loradev->MulticastChannels = channelParam->Next;
        }
        else
        {
            MulticastParams_t *cur = loradev->MulticastChannels;

            // Search the node in the list
            while( cur->Next && cur->Next != channelParam )
            {
                cur = cur->Next;
            }
            // If we found the node, remove it
            if( cur->Next )
            {
                cur->Next = channelParam->Next;
            }
        }
        channelParam->Next = NULL;
    }

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t join_request(netdev2_lorawan_t *loradev)
{
    LoRaMacStatus_t status = LORAMAC_STATUS_SERVICE_UNKNOWN;
    _reset_mac(loradev);

    loradev->join_request_trials++;
    loradev->LoRaMacParams.ChannelsDatarate = join_request_alternate_datarate(loradev, loradev->join_request_trials );

    gnrc_lorawan_send_join_request(loradev);

    if( status != LORAMAC_STATUS_OK )
    {
        loradev->NodeAckRequested = false;
    }

    return status;
}

LoRaMacStatus_t link_check(netdev2_lorawan_t *loradev)
{
    LoRaMacStatus_t status = LORAMAC_STATUS_SERVICE_UNKNOWN;

    // LoRaMac will send this command piggy-pack
    status = _add_mac_command(loradev, MOTE_MAC_LINK_CHECK_REQ, 0, 0 );

    if( status != LORAMAC_STATUS_OK )
    {
        loradev->NodeAckRequested = false;
    }

    return status;
}

static unsigned char message[64];
static void _event_cb(netdev2_t *dev, netdev2_event_t event)
{
    msg_t msg;
    msg.type = NETDEV2_MSG_TYPE_EVENT;
    kernel_pid_t *pid = (kernel_pid_t*) dev->context;
    size_t len;
    struct netdev2_radio_rx_info rx_info;
    gnrc_pktsnip_t *recv_pkt;
    switch(event)
    {
        case NETDEV2_EVENT_ISR:
            msg_send(&msg, *pid);
            break;
        case NETDEV2_EVENT_TX_COMPLETE:
            printf("TX done\n");
            on_tx_done((netdev2_lorawan_t*) dev);
            break;
        case NETDEV2_EVENT_TX_TIMEOUT:
            msg.type = LORAWAN_TX_TIMEOUT_EVENT;
            msg_send(&msg, *pid);
            break;
        case NETDEV2_EVENT_RX_COMPLETE:
            len = dev->driver->recv(dev, NULL, 5, &rx_info);
            recv_pkt = gnrc_pktbuf_add(NULL, NULL, len, GNRC_NETTYPE_LORAWAN);
            dev->driver->recv(dev, (uint8_t*) recv_pkt->data, len, NULL);
            printf("%s\n. {RSSI: %i, SNR: %i}", message, rx_info.rssi, (signed int) rx_info.lqi);
            puts("sx1276: RX Done");
            on_rx_done((netdev2_lorawan_t*) dev, recv_pkt, (signed int) rx_info.rssi, (signed int) rx_info.lqi);
            break;
        case NETDEV2_EVENT_RX_TIMEOUT:
            msg.type = LORAWAN_RX_TIMEOUT_EVENT;
            msg_send(&msg, *pid);
            break;
        case NETDEV2_EVENT_CRC_ERROR:
            msg.type = LORAWAN_CRC_ERROR;
            msg_send(&msg, *pid);
            break;
        case NETDEV2_EVENT_FHSS_CHANGE_CHANNEL:
            msg.type = LORAWAN_FHSS_CHANGE_CHANNEL;
            msg_send(&msg, *pid);
            break;
        case NETDEV2_EVENT_CAD_DONE:
            msg.type = LORAWAN_CAD_DONE;
            msg_send(&msg, *pid);
            break;
        default:
            break;
    }
}

static int _send(netdev2_lorawan_t *loradev, gnrc_pktsnip_t *pkt)
{
    lw_hdr_t hdr;
    hdr.mt_maj = 0;
    hdr.fctrl = 0;

    uint8_t *buf = pkt->data;
    const void *payload = buf+1;
    uint8_t payloadSize = pkt->size-1;
    uint8_t port = buf[0];
    uint8_t pktHeaderLen = 1;
    lw_hdr_t *lw_hdr;
    bool allow_piggybacked = false;
    uint32_t mic = 0;

    lw_hdr_set_adr(&hdr, loradev->lorawan.tx_rx.adr_ctrl);
    lw_hdr_set_adr_ack_req(&hdr, false);
    lw_hdr_set_ack(&hdr, false);
    lw_hdr_set_frame_pending(&hdr, false);
    lw_hdr_set_frame_opts_len(&hdr, 0);
    size_t pkt_size=0;

    if(loradev->ack_timeout_retries > 1)
    {
        loradev->NodeAckRequested = true;
        loradev->join_req_sent = false;
        lw_hdr_set_mtype(&hdr, FRAME_TYPE_DATA_CONFIRMED_UP);
        loradev->ack_timeout_retries_counter = 1;
    }
    else
    {
        lw_hdr_set_mtype(&hdr, FRAME_TYPE_DATA_UNCONFIRMED_UP);
    }

    loradev->join_req_sent = false;
    if( loradev->lorawan.tx_rx.nwk_status == false )
    {
        gnrc_pktbuf_release(pkt);
        return LORAMAC_STATUS_NO_NETWORK_JOINED; // No network has been joined yet
    }

    lw_hdr_set_adr_ack_req(&hdr, AdrNextDr(loradev, lw_hdr_get_adr(&hdr), true, &loradev->LoRaMacParams.ChannelsDatarate ));

    if( _validate_payload_length(loradev, payloadSize, loradev->LoRaMacParams.ChannelsDatarate, loradev->MacCommandsBufferIndex ) == false )
    {
        gnrc_pktbuf_release(pkt);
        return LORAMAC_STATUS_LENGTH_ERROR;
    }

    loradev->RxWindow1Delay = loradev->LoRaMacParams.ReceiveDelay1 - RADIO_WAKEUP_TIME;
    loradev->RxWindow2Delay = loradev->LoRaMacParams.ReceiveDelay2 - RADIO_WAKEUP_TIME;

    if( loradev->SrvAckRequested == true )
    {
        loradev->SrvAckRequested = false;
        lw_hdr_set_ack(&hdr, true);
    }


    // Copy the MAC commands which must be re-send into the MAC command buffer
    memcpy( &loradev->MacCommandsBuffer[loradev->MacCommandsBufferIndex], loradev->MacCommandsBufferToRepeat, loradev->MacCommandsBufferToRepeatIndex );
    loradev->MacCommandsBufferIndex += loradev->MacCommandsBufferToRepeatIndex;

    pktHeaderLen = sizeof(lw_hdr_t);

    if(payload)
    {
        allow_piggybacked = ( loradev->MacCommandsBufferIndex <= LORA_MAC_COMMAND_MAX_LENGTH ) && ( loradev->MacCommandsInNextTx == true );  
        pkt_size = sizeof(lw_hdr_t) + loradev->MacCommandsBufferIndex*allow_piggybacked + 1 + payloadSize + 4;
    }
    else
    {
        payloadSize = 0;
        if( ( loradev->MacCommandsBufferIndex > 0 ) && ( loradev->MacCommandsInNextTx ) )
        {
            payloadSize = loradev->MacCommandsBufferIndex;
            payload = loradev->MacCommandsBuffer;
            port = 0;
            pkt_size = sizeof(lw_hdr_t) + 1 + payloadSize + 4;
        }
    }

    (void) pkt_size;
    loradev->pkt = gnrc_pktbuf_add(NULL, NULL, pkt_size, GNRC_NETTYPE_LORAWAN);

    lw_hdr = loradev->pkt->data;
    lw_hdr->mt_maj = hdr.mt_maj;
    lw_hdr->addr = byteorder_btoll(byteorder_htonl(loradev->lorawan.dev_addr));
    lw_hdr->fctrl = hdr.fctrl;
    lw_hdr->fcnt = byteorder_btols(byteorder_htons(loradev->uplink_counter));

    loradev->MacCommandsInNextTx = false;
    // Store MAC commands which must be re-send in case the device does not receive a downlink anymore
    loradev->MacCommandsBufferToRepeatIndex = _parse_to_repeat_mac_commands( loradev->MacCommandsBuffer, loradev->MacCommandsBufferIndex, loradev->MacCommandsBufferToRepeat );
    if( loradev->MacCommandsBufferToRepeatIndex > 0 )
    {
        loradev->MacCommandsInNextTx = true;
    }
    loradev->MacCommandsBufferIndex = 0;

    if(allow_piggybacked)
    {
        // Update FCtrl field with new value of OptionsLength
        lw_hdr_set_frame_opts_len(lw_hdr, lw_hdr_get_frame_opts_len(&hdr) + loradev->MacCommandsBufferIndex);
        memcpy(((uint8_t*) loradev->pkt->data) + pktHeaderLen, loradev->MacCommandsBuffer, loradev->MacCommandsBufferIndex);
        pktHeaderLen += loradev->MacCommandsBufferIndex;
    }

    if(payloadSize > 0)
    {
        ((uint8_t*) loradev->pkt->data)[pktHeaderLen++] = port;

        /* TODO: Check...*/
        if( port == 0 )
        {
             LoRaMacPayloadEncrypt( (uint8_t* ) payload, payloadSize, loradev->lorawan.nwk_skey, loradev->lorawan.dev_addr, UP_LINK, loradev->uplink_counter, loradev->LoRaMacPayload );
        }
        else
        {
             LoRaMacPayloadEncrypt( (uint8_t* ) payload, payloadSize, loradev->lorawan.app_skey, loradev->lorawan.dev_addr, UP_LINK, loradev->uplink_counter, loradev->LoRaMacPayload );
        }
        memcpy( ((uint8_t*) loradev->pkt->data) + pktHeaderLen,  loradev->LoRaMacPayload, payloadSize );
    }
    mic = LoRaMacComputeMic( (uint8_t*) loradev->pkt->data, pkt_size-4, loradev->lorawan.nwk_skey, loradev->lorawan.dev_addr, UP_LINK, loradev->uplink_counter);

    ((uint8_t*) loradev->pkt->data)[pkt_size-4] = mic & 0xFF;
    ((uint8_t*) loradev->pkt->data)[pkt_size-3] = ( mic >> 8 ) & 0xFF;
    ((uint8_t*) loradev->pkt->data)[pkt_size-2] = ( mic >> 16 ) & 0xFF;
    ((uint8_t*) loradev->pkt->data)[pkt_size-1] = ( mic >> 24 ) & 0xFF;

    int status = _schedule_next_tx(loradev);

    if( status != LORAMAC_STATUS_OK )
    {
        loradev->NodeAckRequested = false;
    }

    gnrc_pktbuf_release(pkt);
    return status;
}


void *_gnrc_lorawan_event_loop(void *arg)
{
    static msg_t _msg_q[GNRC_LORA_MSG_QUEUE];
    msg_t msg, reply;
    netdev2_t *netdev = (netdev2_t*) arg;
    msg_init_queue(_msg_q, GNRC_LORA_MSG_QUEUE);
    gnrc_pktsnip_t *pkt=NULL;
    gnrc_netapi_opt_t *opt;
    int res;

    netdev->event_callback = _event_cb;
    //TODO: Fix
    kernel_pid_t pid = thread_getpid();
    netdev->context = &pid;
    netdev->driver->init(netdev);
    gnrc_lorawan_reset((netdev2_lorawan_t*) arg, thread_getpid() );
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case NETDEV2_MSG_TYPE_EVENT:
                netdev->driver->isr(netdev);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                pkt = msg.content.ptr;
                _send((netdev2_lorawan_t*) netdev, pkt);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                /* read incoming options */
                opt = msg.content.ptr;
                /* set option for device driver */
                res = netdev->driver->set(netdev, opt->opt, opt->data, opt->data_len);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                /* read incoming options */
                opt = msg.content.ptr;
                /* get option from device driver */
                res = netdev->driver->get(netdev, opt->opt, opt->data, opt->data_len);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case LORAWAN_TX_TIMEOUT_EVENT:
                puts("sx1276: TX timeout");
                on_tx_timeout((netdev2_lorawan_t*) netdev);
                break;
            case LORAWAN_RX_TIMEOUT_EVENT:
                puts("sx1276: RX timeout");
                on_rx_timeout((netdev2_lorawan_t*) netdev);
                break;
            case LORAWAN_CRC_ERROR:
                puts("sx1276: RX CRC_ERROR");
                on_rx_error((netdev2_lorawan_t*) netdev);
                break;
            case LORAWAN_FHSS_CHANGE_CHANNEL:
                //events->FhssChangeChannel(sx1276._internal.last_channel);
                break;
            case LORAWAN_CAD_DONE:
                //events->CadDone(sx1276._internal.is_last_cad_success);
                break;
            case LORAWAN_TIMER_RX_WINDOW1:
                on_rx1_timeout((netdev2_lorawan_t*) netdev);
                break;
            case LORAWAN_TIMER_RX_WINDOW2:
                on_rx2_timeout((netdev2_lorawan_t*) netdev);
                break;
            case LORAWAN_TIMER_ACK_TIMEOUT:
                on_ack_timeout((netdev2_lorawan_t*) netdev);
                break;
            case LORAWAN_TIMER_TX_DELAYED:
                on_tx_delay_timeout((netdev2_lorawan_t*) netdev);
                break;
            default:
                break;
        }
    }
    return NULL;
}

kernel_pid_t gnrc_lorawan_init(char *stack, int stacksize, char priority,
                        const char *name, netdev2_lorawan_t *loradev)
{
    kernel_pid_t res;

    /* check if given netdev device is defined and the driver is set */
    if (loradev == NULL) {
        return -ENODEV;
    }

    /* create new gnrc_netdev2 thread */
    res = thread_create(stack, stacksize, priority, THREAD_CREATE_STACKTEST,
                         _gnrc_lorawan_event_loop, (void *) loradev, name);
    if (res <= 0) {
        return -EINVAL;
    }

    return res;
}

