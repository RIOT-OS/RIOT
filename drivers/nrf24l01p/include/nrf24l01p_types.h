/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief   Definitions of user visible types for the NRF24L01P device driver
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_TYPES_H
#define NRF24L01P_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief NRF24L01P operation states
 */
typedef enum nrf24l01p_state {
    NRF24L01P_STATE_UNDEFINED  =   2, /**< State after voltage supply */
    NRF24L01P_STATE_POWER_DOWN =   4, /**< Sleep but SPI remains active */
    NRF24L01P_STATE_STANDBY_1  =   8, /**< Idle */
    NRF24L01P_STATE_STANDBY_2  =  16, /**< TX FIFO empty,
                                           fill up TX FIFO again */
    NRF24L01P_STATE_RX_MODE    =  32, /**< Baseband protocol engine
                                           constantly searches for
                                           a valid packet */
    NRF24L01P_STATE_TX_MODE    =  64, /**< Transmit next packet */
} nrf24l01p_state_t;

/**
 * @brief ShockBurst
 */
#define NRF24L01P_SHOCKBURST                (0)

/**
 * @brief   Enhanded ShockBurst
 */
#define NRF24L01P_ENHANCED_SHOCKBURST       (1)

/**
 * @brief   Possible protocols for NRF24L01P
 */
typedef enum nrf2401p_protocol {
    NRF24L01P_PROTOCOL_SB  = NRF24L01P_SHOCKBURST,         /**< ShockBurst */
    NRF24L01P_PROTOCOL_ESB = NRF24L01P_ENHANCED_SHOCKBURST /**< Enhanced SB */
} nrf24l01p_protocol_t;

/**
 * @brief   Enumeration of NRF24L01P data pipes
 */
typedef enum nrf24l01p_pipe {
    NRF24L01P_P0 = 0,    /**< Pipe 0 */
    NRF24L01P_P1 = 1,    /**< Pipe 1 */
    NRF24L01P_P2 = 2,    /**< Pipe 2 */
    NRF24L01P_P3 = 3,    /**< Pipe 3 */
    NRF24L01P_P4 = 4,    /**< Pipe 4 */
    NRF24L01P_P5 = 5,    /**< Pipe 5 */
    NRF24L01P_PX_NUM_OF  /**< Number of supported pipes */
} nrf24l01p_pipe_t;

/**
 * @brief   Possible values to configure the layer-2 address width
 */
typedef enum nrf24l01p_aw {
    NRF24L01P_AW_3BYTE  = 1, /**< Use a 3 bytes long layer-2 address */
    NRF24L01P_AW_4BYTE  = 2, /**< Use a 4 bytes long layer-2 address */
    NRF24L01P_AW_5BYTE  = 3, /**< Use a 5 bytes long layer-2 address */
    NRF24L01P_AW_NUM_OF      /**< Number of possible values to configure
                                  the layer-2 address width */
} nrf24l01p_aw_t;

/**
 * @brief   Possible values to configure the retransmission delay in ESB
 */
typedef enum nrf24l01p_ard {
    NRF24L01P_ARD_250US     = 0,  /**< 250 us */
    NRF24L01P_ARD_500US     = 1,  /**< 500 us */
    NRF24L01P_ARD_750US     = 2,  /**< 750 us */
    NRF24L01P_ARD_1000US    = 3,  /**< 1000 us */
    NRF24L01P_ARD_1250US    = 4,  /**< 1250 us */
    NRF24L01P_ARD_1500US    = 5,  /**< 1500 us */
    NRF24L01P_ARD_1750US    = 6,  /**< 1750 us */
    NRF24L01P_ARD_2000US    = 7,  /**< 2000 us */
    NRF24L01P_ARD_2250US    = 8,  /**< 2250 us */
    NRF24L01P_ARD_2500US    = 9,  /**< 2500 us */
    NRF24L01P_ARD_2750US    = 10, /**< 2750 us */
    NRF24L01P_ARD_3000US    = 11, /**< 3000 us */
    NRF24L01P_ARD_3250US    = 12, /**< 3250 us */
    NRF24L01P_ARD_3500US    = 13, /**< 3500 us */
    NRF24L01P_ARD_3750US    = 14, /**< 3750 us */
    NRF24L01P_ARD_4000US    = 15, /**< 4000 us */
    NRF24L01P_ARD_NUM_OF          /**< Number of possible values to configure
                                       the retransmission delay */
} nrf24l01p_ard_t;

/**
 * @brief   Possible values to configure the CRC length
 */
typedef enum nrf24l01p_crc {
    NRF24L01P_CRC_0BYTE = 1, /**< 0 bytes CRC length */
    NRF24L01P_CRC_1BYTE = 2, /**< 1 byte CRC length */
    NRF24L01P_CRC_2BYTE = 3, /**< 2 bytes CRC length */
    NRF24L01P_CRC_NUM_OF     /**< Number of possible values to configure
                                  CRC length */
} nrf24l01p_crc_t;

/**
 * @brief   Possible values to configure the radio power
 */
typedef enum nrf24l01p_tx_power {
    NRF24L01P_TX_POWER_MINUS_18DBM = 0, /**< -18 dBm */
    NRF24L01P_TX_POWER_MINUS_12DBM = 1, /**< -12 dBm */
    NRF24L01P_TX_POWER_MINUS_6DBM  = 2, /**<  -6 dBm */
    NRF24L01P_TX_POWER_0DBM        = 3, /**<   0 dBm */
    NRF24L01P_TX_POWER_NUM_OF           /**< Number of possible values to
                                             configure the radio power */
} nrf24l01p_tx_power_t;

/**
 * @brief   Possible values to configure the data rate
 */
typedef enum nrf24l01p_rfdr {
    NRF24L01P_RF_DR_1MBPS   = 0, /**< 1 Mbit/s */
    NRF24L01P_RF_DR_250KBPS = 1, /**< 250 kbit/s */
    NRF24L01P_RF_DR_2MBPS   = 2, /**< 2 Mbit/s */
    NRF24L01P_RF_DR_NUM_OF       /**< Number of possible values to configure
                                      the data rate */
} nrf24l01p_rfdr_t;

struct nrf24l01p;
typedef struct nrf24l01p nrf24l01p_t; /**< typedef of forward declaration */

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_TYPES_H */
/** @} */
