/*
 * Copyright (C) 2017 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_lorawan   Definitions for GNRC Lorawan
 * @ingroup     net_gnrc
 * @brief       Provides definitions for lorawan. Based on the original Semtech LoRaMAC implementation.
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
#ifndef __LORAMAC_BOARD_H__
#define __LORAMAC_BOARD_H__

/*!
 * Returns individual channel mask
 *
 * \param[IN] channelIndex Channel index 1 based
 * \retval channelMask
 */
#define LC( channelIndex )            ( uint16_t )( 1 << ( channelIndex - 1 ) )

#if defined( USE_BAND_433 )

/*!
 * LoRaMac maximum number of channels
 */
#define LORA_MAX_NB_CHANNELS                        16

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_TX_MIN_DATARATE                     DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_TX_MAX_DATARATE                     DR_7

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_RX_MIN_DATARATE                     DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_RX_MAX_DATARATE                     DR_7

/*!
 * Default datarate used by the node
 */
#define LORAMAC_DEFAULT_DATARATE                    DR_0

/*!
 * Minimal Rx1 receive datarate offset
 */
#define LORAMAC_MIN_RX1_DR_OFFSET                   0

/*!
 * Maximal Rx1 receive datarate offset
 */
#define LORAMAC_MAX_RX1_DR_OFFSET                   5

/*!
 * Minimal Tx output power that can be used by the node
 */
#define LORAMAC_MIN_TX_POWER                        TX_POWER_M5_DBM

/*!
 * Maximal Tx output power that can be used by the node
 */
#define LORAMAC_MAX_TX_POWER                        TX_POWER_10_DBM

/*!
 * Default Tx output power used by the node
 */
#ifndef LORAMAC_DEFAULT_TX_POWER
#define LORAMAC_DEFAULT_TX_POWER                    TX_POWER_10_DBM
#endif

/*!
 * LoRaMac TxPower definition
 */
#define TX_POWER_10_DBM                             0
#define TX_POWER_07_DBM                             1
#define TX_POWER_04_DBM                             2
#define TX_POWER_01_DBM                             3
#define TX_POWER_M2_DBM                             4
#define TX_POWER_M5_DBM                             5

/*!
 * LoRaMac datarates definition
 */
#define DR_0                                        0  // SF12 - BW125
#define DR_1                                        1  // SF11 - BW125
#define DR_2                                        2  // SF10 - BW125
#define DR_3                                        3  // SF9  - BW125
#define DR_4                                        4  // SF8  - BW125
#define DR_5                                        5  // SF7  - BW125
#define DR_6                                        6  // SF7  - BW250
#define DR_7                                        7  // FSK

/*!
 * Second reception window channel definition.
 */
// Channel = { Frequency [Hz], Datarate }
#define RX_WND_2_CHANNEL                                  { 434665000, DR_0 }

/*!
 * LoRaMac maximum number of bands
 */
#define LORA_MAX_NB_BANDS                           1

// Band = { DutyCycle, TxMaxPower, LastTxDoneTime, TimeOff }
#define BAND0              { 100, TX_POWER_10_DBM, 0,  0 } //  1.0 %

/*!
 * LoRaMac default channels
 */
// Channel = { Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
#define LC1                { 433175000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC2                { 433375000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC3                { 433575000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

/*!
 * LoRaMac duty cycle for the back-off procedure
 */
#define BACKOFF_DC_1_HOUR       100
#define BACKOFF_DC_10_HOURS     1000
#define BACKOFF_DC_24_HOURS     10000

#define BACKOFF_RND_OFFSET      600000

/*!
 * LoRaMac channels which are allowed for the join procedure
 */
#define JOIN_CHANNELS      ( uint16_t )( LC( 1 ) | LC( 2 ) | LC( 3 ) )

#elif defined( USE_BAND_780 )

/*!
 * LoRaMac maximum number of channels
 */
#define LORA_MAX_NB_CHANNELS                        16

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_TX_MIN_DATARATE                     DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_TX_MAX_DATARATE                     DR_7

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_RX_MIN_DATARATE                     DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_RX_MAX_DATARATE                     DR_7

/*!
 * Default datarate used by the node
 */
#define LORAMAC_DEFAULT_DATARATE                    DR_0

/*!
 * Minimal Rx1 receive datarate offset
 */
#define LORAMAC_MIN_RX1_DR_OFFSET                   0

/*!
 * Maximal Rx1 receive datarate offset
 */
#define LORAMAC_MAX_RX1_DR_OFFSET                   5

/*!
 * Minimal Tx output power that can be used by the node
 */
#define LORAMAC_MIN_TX_POWER                        TX_POWER_M5_DBM

/*!
 * Maximal Tx output power that can be used by the node
 */
#define LORAMAC_MAX_TX_POWER                        TX_POWER_10_DBM

/*!
 * Default Tx output power used by the node
 */
#ifndef LORAMAC_DEFAULT_TX_POWER
#define LORAMAC_DEFAULT_TX_POWER                    TX_POWER_10_DBM
#endif

/*!
 * LoRaMac TxPower definition
 */
#define TX_POWER_10_DBM                             0
#define TX_POWER_07_DBM                             1
#define TX_POWER_04_DBM                             2
#define TX_POWER_01_DBM                             3
#define TX_POWER_M2_DBM                             4
#define TX_POWER_M5_DBM                             5

/*!
 * LoRaMac datarates definition
 */
#define DR_0                                        0  // SF12 - BW125
#define DR_1                                        1  // SF11 - BW125
#define DR_2                                        2  // SF10 - BW125
#define DR_3                                        3  // SF9  - BW125
#define DR_4                                        4  // SF8  - BW125
#define DR_5                                        5  // SF7  - BW125
#define DR_6                                        6  // SF7  - BW250
#define DR_7                                        7  // FSK

/*!
 * Second reception window channel definition.
 */
// Channel = { Frequency [Hz], Datarate }
#define RX_WND_2_CHANNEL                                  { 786000000, DR_0 }

/*!
 * LoRaMac maximum number of bands
 */
#define LORA_MAX_NB_BANDS                           1

// Band = { DutyCycle, TxMaxPower, LastTxDoneTime, TimeOff }
#define BAND0              { 100, TX_POWER_10_DBM, 0,  0 } //  1.0 %

/*!
 * LoRaMac default channels
 */
// Channel = { Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
#define LC1                { 779500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC2                { 779700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC3                { 779900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

/*!
 * LoRaMac duty cycle for the back-off procedure
 */
#define BACKOFF_DC_1_HOUR       100
#define BACKOFF_DC_10_HOURS     1000
#define BACKOFF_DC_24_HOURS     10000

#define BACKOFF_RND_OFFSET      600000

/*!
 * LoRaMac channels which are allowed for the join procedure
 */
#define JOIN_CHANNELS      ( uint16_t )( LC( 1 ) | LC( 2 ) | LC( 3 ) )

#elif defined( USE_BAND_868 )

/*!
 * LoRaMac maximum number of channels
 */
#define LORA_MAX_NB_CHANNELS                        16

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_TX_MIN_DATARATE                     DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_TX_MAX_DATARATE                     DR_7

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_RX_MIN_DATARATE                     DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_RX_MAX_DATARATE                     DR_7

/*!
 * Default datarate used by the node
 */
#define LORAMAC_DEFAULT_DATARATE                    DR_0

/*!
 * Minimal Rx1 receive datarate offset
 */
#define LORAMAC_MIN_RX1_DR_OFFSET                   0

/*!
 * Maximal Rx1 receive datarate offset
 */
#define LORAMAC_MAX_RX1_DR_OFFSET                   5

/*!
 * Minimal Tx output power that can be used by the node
 */
#define LORAMAC_MIN_TX_POWER                        TX_POWER_02_DBM

/*!
 * Maximal Tx output power that can be used by the node
 */
#define LORAMAC_MAX_TX_POWER                        TX_POWER_20_DBM

/*!
 * Default Tx output power used by the node
 */
#ifndef LORAMAC_DEFAULT_TX_POWER
#define LORAMAC_DEFAULT_TX_POWER                    TX_POWER_14_DBM
#endif

/*!
 * LoRaMac TxPower definition
 */
#define TX_POWER_20_DBM                             0
#define TX_POWER_14_DBM                             1
#define TX_POWER_11_DBM                             2
#define TX_POWER_08_DBM                             3
#define TX_POWER_05_DBM                             4
#define TX_POWER_02_DBM                             5

/*!
 * LoRaMac datarates definition
 */
#define DR_0                                        0  // SF12 - BW125
#define DR_1                                        1  // SF11 - BW125
#define DR_2                                        2  // SF10 - BW125
#define DR_3                                        3  // SF9  - BW125
#define DR_4                                        4  // SF8  - BW125
#define DR_5                                        5  // SF7  - BW125
#define DR_6                                        6  // SF7  - BW250
#define DR_7                                        7  // FSK

/*!
 * Second reception window channel definition.
 */
// Channel = { Frequency [Hz], Datarate }
#define RX_WND_2_CHANNEL                                  { 869525000, DR_0 }

/*!
 * LoRaMac maximum number of bands
 */
#define LORA_MAX_NB_BANDS                           5

/*!
 * LoRaMac EU868 default bands
 */
typedef enum
{
    BAND_G1_0,
    BAND_G1_1,
    BAND_G1_2,
    BAND_G1_3,
    BAND_G1_4,
}BandId_t;

// Band = { DutyCycle, TxMaxPower, LastTxDoneTime, TimeOff }
#define BAND0              { 100 , TX_POWER_14_DBM, 0,  0 } //  1.0 %
#define BAND1              { 100 , TX_POWER_14_DBM, 0,  0 } //  1.0 %
#define BAND2              { 1000, TX_POWER_14_DBM, 0,  0 } //  0.1 %
#define BAND3              { 10  , TX_POWER_14_DBM, 0,  0 } // 10.0 %
#define BAND4              { 100 , TX_POWER_14_DBM, 0,  0 } //  1.0 %

/*!
 * LoRaMac default channels
 */
// Channel = { Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
#define LC1                { 868100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 1 }
#define LC2                { 868300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 1 }
#define LC3                { 868500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 1 }

/*!
 * LoRaMac duty cycle for the back-off procedure
 */
#define BACKOFF_DC_1_HOUR       100
#define BACKOFF_DC_10_HOURS     1000
#define BACKOFF_DC_24_HOURS     10000

#define BACKOFF_RND_OFFSET      600000

/*!
 * LoRaMac channels which are allowed for the join procedure
 */
#define JOIN_CHANNELS      ( uint16_t )( LC( 1 ) | LC( 2 ) | LC( 3 ) )

#elif defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )

/*!
 * LoRaMac maximum number of channels
 */
#define LORA_MAX_NB_CHANNELS                        72

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_TX_MIN_DATARATE                     DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_TX_MAX_DATARATE                     DR_4

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_RX_MIN_DATARATE                     DR_8

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_RX_MAX_DATARATE                     DR_13

/*!
 * Default datarate used by the node
 */
#define LORAMAC_DEFAULT_DATARATE                    DR_0

/*!
 * Minimal Rx1 receive datarate offset
 */
#define LORAMAC_MIN_RX1_DR_OFFSET                   0

/*!
 * Maximal Rx1 receive datarate offset
 */
#define LORAMAC_MAX_RX1_DR_OFFSET                   3

/*!
 * Minimal Tx output power that can be used by the node
 */
#define LORAMAC_MIN_TX_POWER                        TX_POWER_10_DBM

/*!
 * Maximal Tx output power that can be used by the node
 */
#define LORAMAC_MAX_TX_POWER                        TX_POWER_20_DBM

/*!
 * Default Tx output power used by the node
 */
#ifndef LORAMAC_DEFAULT_TX_POWER
#define LORAMAC_DEFAULT_TX_POWER                    TX_POWER_14_DBM
#endif

/*!
 * LoRaMac TxPower definition
 */
#define TX_POWER_30_DBM                             0
#define TX_POWER_28_DBM                             1
#define TX_POWER_26_DBM                             2
#define TX_POWER_24_DBM                             3
#define TX_POWER_22_DBM                             4
#define TX_POWER_20_DBM                             5
#define TX_POWER_18_DBM                             6
#define TX_POWER_16_DBM                             7
#define TX_POWER_14_DBM                             8
#define TX_POWER_12_DBM                             9
#define TX_POWER_10_DBM                             10

/*!
 * LoRaMac datarates definition
 */
#define DR_0                                        0  // SF10 - BW125 |
#define DR_1                                        1  // SF9  - BW125 |
#define DR_2                                        2  // SF8  - BW125 +-> Up link
#define DR_3                                        3  // SF7  - BW125 |
#define DR_4                                        4  // SF8  - BW500 |
#define DR_5                                        5  // RFU
#define DR_6                                        6  // RFU
#define DR_7                                        7  // RFU
#define DR_8                                        8  // SF12 - BW500 |
#define DR_9                                        9  // SF11 - BW500 |
#define DR_10                                       10 // SF10 - BW500 |
#define DR_11                                       11 // SF9  - BW500 |
#define DR_12                                       12 // SF8  - BW500 +-> Down link
#define DR_13                                       13 // SF7  - BW500 |
#define DR_14                                       14 // RFU          |
#define DR_15                                       15 // RFU          |

/*!
 * Second reception window channel definition.
 */
// Channel = { Frequency [Hz], Datarate }
#define RX_WND_2_CHANNEL                                  { 923300000, DR_8 }

/*!
 * LoRaMac maximum number of bands
 */
#define LORA_MAX_NB_BANDS                           1

// Band = { DutyCycle, TxMaxPower, LastTxDoneTime, TimeOff }
#define BAND0              { 1, TX_POWER_20_DBM, 0,  0 } //  100.0 %

#define BACKOFF_RND_OFFSET      600000

/*!
 * LoRaMac default channels
 */
// Channel = { Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
/*
 * US band channels are initialized using a loop in LoRaMacInit function
 * \code
 * // 125 kHz channels
 * for( uint8_t i = 0; i < LORA_MAX_NB_CHANNELS - 8; i++ )
 * {
 *     Channels[i].Frequency = 902.3e6 + i * 200e3;
 *     Channels[i].DrRange.Value = ( DR_3 << 4 ) | DR_0;
 *     Channels[i].Band = 0;
 * }
 * // 500 kHz channels
 * for( uint8_t i = LORA_MAX_NB_CHANNELS - 8; i < LORA_MAX_NB_CHANNELS; i++ )
 * {
 *     Channels[i].Frequency = 903.0e6 + ( i - ( LORA_MAX_NB_CHANNELS - 8 ) ) * 1.6e6;
 *     Channels[i].DrRange.Value = ( DR_4 << 4 ) | DR_4;
 *     Channels[i].Band = 0;
 * }
 * \endcode
 */
#else
    #error "Please define a frequency band in the compiler options."
#endif

#endif // __LORAMAC_BOARD_H__
