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
 * @brief   Definitions of user visible types for the NRF24L01+ (NG)
 *          device driver
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_NG_TYPES_H
#define NRF24L01P_NG_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Flag that indicates that a state transition to
 *          POWER_DOWN can be done
 */
#define NRF24L01P_NG_TRANSITION_TO_POWER_DOWN   (1 << 3)
/**
 * @brief   Flag that indicates that a state transition to
 *          STANDBY_1 can be done
 */
#define NRF24L01P_NG_TRANSITION_TO_STANDBY_1    (1 << 4)
/**
 * @brief   Flag that indicates that a state transition to
 *          STANDBY_2 can be done
 */
#define NRF24L01P_NG_TRANSITION_TO_STANDBY_2    (1 << 5)
/**
 * @brief   Flag that indicates that a state transition to
 *          RX_MODE can be done
 */
#define NRF24L01P_NG_TRANSITION_TO_RX_MODE      (1 << 6)
/**
 * @brief   Flag that indicates that a state transition to
 *          TX_MODE can be done
 */
#define NRF24L01P_NG_TRANSITION_TO_TX_MODE      (1 << 7)

/**
 * @brief NRF24L01+ operation states
 */
typedef enum nrf24l01p_ng_state {
    /** @brief State right after voltage supply */
    NRF24L01P_NG_STATE_UNDEFINED  = (0 |
                                    (NRF24L01P_NG_TRANSITION_TO_POWER_DOWN)),
    /** @brief Register values are available and maintained, SPI active */
    NRF24L01P_NG_STATE_POWER_DOWN = (1 |
                                    (NRF24L01P_NG_TRANSITION_TO_STANDBY_1)),
    /** @brief Idle */
    NRF24L01P_NG_STATE_STANDBY_1  = (2 |
                                    (NRF24L01P_NG_TRANSITION_TO_POWER_DOWN |
                                    NRF24L01P_NG_TRANSITION_TO_STANDBY_2   |
                                    NRF24L01P_NG_TRANSITION_TO_RX_MODE     |
                                    NRF24L01P_NG_TRANSITION_TO_TX_MODE)),
    /** @brief TX FIFO empty, fill up TX FIFO again */
    NRF24L01P_NG_STATE_STANDBY_2  = (3 |
                                    (NRF24L01P_NG_TRANSITION_TO_POWER_DOWN |
                                    NRF24L01P_NG_TRANSITION_TO_TX_MODE)),
    /** @brief Constantly search for a valid packet */
    NRF24L01P_NG_STATE_RX_MODE    = (4 |
                                    (NRF24L01P_NG_TRANSITION_TO_POWER_DOWN |
                                    NRF24L01P_NG_TRANSITION_TO_STANDBY_1)),
    /** @brief Transmit next packet */
    NRF24L01P_NG_STATE_TX_MODE    = (5 |
                                    (NRF24L01P_NG_TRANSITION_TO_POWER_DOWN |
                                    NRF24L01P_NG_TRANSITION_TO_STANDBY_1   |
                                    NRF24L01P_NG_TRANSITION_TO_STANDBY_2)),
} nrf24l01p_ng_state_t;

/**
 * @brief   Enumeration of NRF24L01+ data pipes
 */
typedef enum nrf24l01p_ng_pipe {
    NRF24L01P_NG_P0    = 0, /**< Pipe 0 */
    NRF24L01P_NG_P1    = 1, /**< Pipe 1 */
    NRF24L01P_NG_P2    = 2, /**< Pipe 2 */
    NRF24L01P_NG_P3    = 3, /**< Pipe 3 */
    NRF24L01P_NG_P4    = 4, /**< Pipe 4 */
    NRF24L01P_NG_P5    = 5, /**< Pipe 5 */
    NRF24L01P_NG_PX_NUM_OF  /**< Number of supported pipes */
} nrf24l01p_ng_pipe_t;

/**
 * @brief   Possible values to configure the layer-2 address width
 */
typedef enum nrf24l01p_ng_aw {
    NRF24L01P_NG_AW_3BYTE = 1, /**< Use a 3 bytes long layer-2 address */
    NRF24L01P_NG_AW_4BYTE = 2, /**< Use a 4 bytes long layer-2 address */
    NRF24L01P_NG_AW_5BYTE = 3, /**< Use a 5 bytes long layer-2 address */
    NRF24L01P_NG_AW_NUM_OF     /**< Number of possible values to configure
                                    the layer-2 address width */
} nrf24l01p_ng_aw_t;

/**
 * @brief   Possible values to configure the retransmission delay in ESB
 */
typedef enum nrf24l01p_ng_ard {
    NRF24L01P_NG_ARD_250US  = 0,  /**< 250 us */
    NRF24L01P_NG_ARD_500US  = 1,  /**< 500 us */
    NRF24L01P_NG_ARD_750US  = 2,  /**< 750 us */
    NRF24L01P_NG_ARD_1000US = 3,  /**< 1000 us */
    NRF24L01P_NG_ARD_1250US = 4,  /**< 1250 us */
    NRF24L01P_NG_ARD_1500US = 5,  /**< 1500 us */
    NRF24L01P_NG_ARD_1750US = 6,  /**< 1750 us */
    NRF24L01P_NG_ARD_2000US = 7,  /**< 2000 us */
    NRF24L01P_NG_ARD_2250US = 8,  /**< 2250 us */
    NRF24L01P_NG_ARD_2500US = 9,  /**< 2500 us */
    NRF24L01P_NG_ARD_2750US = 10, /**< 2750 us */
    NRF24L01P_NG_ARD_3000US = 11, /**< 3000 us */
    NRF24L01P_NG_ARD_3250US = 12, /**< 3250 us */
    NRF24L01P_NG_ARD_3500US = 13, /**< 3500 us */
    NRF24L01P_NG_ARD_3750US = 14, /**< 3750 us */
    NRF24L01P_NG_ARD_4000US = 15, /**< 4000 us */
    NRF24L01P_NG_ARD_NUM_OF       /**< Number of possible values to configure
                                       the retransmission delay */
} nrf24l01p_ng_ard_t;

/**
 * @brief   Possible values to configure the CRC length
 */
typedef enum nrf24l01p_ng_crc {
    NRF24L01P_NG_CRC_0BYTE = 1, /**< 0 bytes CRC length */
    NRF24L01P_NG_CRC_1BYTE = 2, /**< 1 byte CRC length */
    NRF24L01P_NG_CRC_2BYTE = 3, /**< 2 bytes CRC length */
} nrf24l01p_ng_crc_t;

/**
 * @brief   Possible values to configure the radio power
 */
typedef enum nrf24l01p_ng_tx_power {
    NRF24L01P_NG_TX_POWER_MINUS_18DBM = 0, /**< -18 dBm */
    NRF24L01P_NG_TX_POWER_MINUS_12DBM = 1, /**< -12 dBm */
    NRF24L01P_NG_TX_POWER_MINUS_6DBM  = 2, /**<  -6 dBm */
    NRF24L01P_NG_TX_POWER_0DBM        = 3, /**<   0 dBm */
    NRF24L01P_NG_TX_POWER_NUM_OF           /**< Number of possible values
                                                to configure the radio power */
} nrf24l01p_ng_tx_power_t;

/**
 * @brief   Possible values to configure the data rate
 */
typedef enum nrf24l01p_ng_rfdr {
    NRF24L01P_NG_RF_DR_1MBPS   = 0, /**< 1 Mbit/s */
    NRF24L01P_NG_RF_DR_250KBPS = 1, /**< 250 kbit/s */
    NRF24L01P_NG_RF_DR_2MBPS   = 2, /**< 2 Mbit/s */
    NRF24L01P_NG_RF_DR_NUM_OF       /**< Number of possible values to configure
                                         the data rate */
} nrf24l01p_ng_rfdr_t;

struct nrf24l01p_ng;
typedef struct nrf24l01p_ng nrf24l01p_ng_t; /**< typedef of
                                                 forward declaration */

/**
 * @brief   Convert @ref nrf24l01p_ng_aw_t to actual address width
 *
 * @param[in] address_width Address width enum
 *
 * @return                  Address width in [bytes]
 */
static inline
uint8_t nrf24l01p_ng_etoval_aw(nrf24l01p_ng_aw_t address_width)
{
    if (address_width <= NRF24L01P_NG_AW_3BYTE) {
        return 3;
    }
    if (address_width == NRF24L01P_NG_AW_4BYTE) {
        return 4;
    }
    return 5;
}

/**
 * @brief   Convert address width in [bytes] to @ref nrf24l01p_ng_aw_t
 *
 * @param[in] address_width Address width in [bytes]
 *
 * @return                  Corresponding enum
 */
static inline
nrf24l01p_ng_aw_t nrf24l01p_ng_valtoe_aw(uint8_t address_width)
{
    if (address_width <= 3) {
        return NRF24L01P_NG_AW_3BYTE;
    }
    if (address_width == 4) {
        return NRF24L01P_NG_AW_4BYTE;
    }
    return NRF24L01P_NG_AW_5BYTE;
}

/**
 * @brief   Convert @ref nrf24l01p_ng_ard_t to actual retransmission delay
 *
 * @param[in] retr_delay    Retransmission delay enum
 *
 * @return                  Retransmission delay in [us]
 */
static inline
uint16_t nrf24l01p_ng_etoval_ard(nrf24l01p_ng_ard_t retr_delay)
{
    if (retr_delay >= NRF24L01P_NG_ARD_4000US) {
        return 4000;
    }
    return (retr_delay + 1) * 250;
}

/**
 * @brief   Convert retransmission delay in [us] to @ref nrf24l01p_ng_ard_t
 *
 * @param[in] retr_delay    Retransmission delay in [us]
 *
 * @return                  Corresponding enum
 */
static inline
nrf24l01p_ng_ard_t nrf24l01p_ng_valtoe_ard(uint16_t retr_delay)
{
    if (retr_delay >= 4000) {
        return NRF24L01P_NG_ARD_4000US;
    }
    if (retr_delay < 250) {
        return NRF24L01P_NG_ARD_250US;
    }
    return (nrf24l01p_ng_ard_t)((retr_delay / 250) - 1);
}

/**
 * @brief   Convert @ref nrf24l01p_ng_crc_t to actual CRC length
 *
 * @param[in] crc_len       CRC length enum
 *
 * @return                  CRC length in [bytes]
 */
static inline
uint8_t nrf24l01p_ng_etoval_crc(nrf24l01p_ng_crc_t crc_len)
{
    if (crc_len <= NRF24L01P_NG_CRC_0BYTE) {
        return 0;
    }
    if (crc_len == NRF24L01P_NG_CRC_1BYTE) {
        return 1;
    }
    return 2;
}

/**
 * @brief   Convert CRC length in [bytes] to @ref nrf24l01p_ng_crc_t
 *
 * @param[in] crc_len       CRC length in [bytes]
 *
 * @return                  Corresponding enum
 */
static inline
nrf24l01p_ng_crc_t nrf24l01p_ng_valtoe_crc(uint8_t crc_len)
{
    if (!crc_len) {
        return NRF24L01P_NG_CRC_0BYTE;
    }
    if (crc_len == 1) {
        return NRF24L01P_NG_CRC_1BYTE;
    }
    return NRF24L01P_NG_CRC_2BYTE;
}

/**
 * @brief   Convert @ref nrf24l01p_ng_tx_power_t to actual Tx power
 *
 * @param[in] power         RF power enum
 *
 * @return                  RF power in [dbm]
 */
static inline
int8_t nrf24l01p_ng_etoval_tx_power(nrf24l01p_ng_tx_power_t power)
{
    if (power == NRF24L01P_NG_TX_POWER_MINUS_18DBM) {
        return -18;
    }
    if (power == NRF24L01P_NG_TX_POWER_MINUS_12DBM) {
        return -12;
    }
    if (power == NRF24L01P_NG_TX_POWER_MINUS_6DBM) {
        return -6;
    }
    return 0;
}

/**
 * @brief   Convert RF power in [dbm] to @ref nrf24l01p_ng_tx_power_t
 *
 * @param[in] power         RF power in [dbm]
 *
 * @return                  Corresponding enum
 */
static inline
nrf24l01p_ng_tx_power_t nrf24l01p_ng_valtoe_tx_power(int16_t power)
{
    if (power <= -18) {
        return NRF24L01P_NG_TX_POWER_MINUS_18DBM;
    }
    if (power <= -12) {
        return NRF24L01P_NG_TX_POWER_MINUS_12DBM;
    }
    if (power <= -6) {
        return NRF24L01P_NG_TX_POWER_MINUS_6DBM;
    }
    return NRF24L01P_NG_TX_POWER_0DBM;
}

/**
 * @brief   Convert @ref nrf24l01p_ng_rfdr_t to actual air data rate
 *
 * @param[in] data_rate     Air data rate enum
 *
 * @return                  Air data rate in [kbit/s]
 */
static inline
uint16_t nrf24l01p_ng_etoval_rfdr(nrf24l01p_ng_rfdr_t data_rate)
{
    if (data_rate == NRF24L01P_NG_RF_DR_1MBPS) {
        return 1000;
    }
    if (data_rate == NRF24L01P_NG_RF_DR_250KBPS) {
        return 250;
    }
    return 2000;
}

/**
 * @brief   Convert Air data rate in [kbit/s] to @ref nrf24l01p_ng_rfdr_t
 *
 * @param[in] data_rate     Air data rate in [kbit/s]
 *
 * @return                  Corresponding enum
 */
static inline
nrf24l01p_ng_rfdr_t nrf24l01p_ng_valtoe_rfdr(uint16_t data_rate)
{
    if (data_rate <= 250) {
        return NRF24L01P_NG_RF_DR_250KBPS;
    }
    if (data_rate <= 1000) {
        return NRF24L01P_NG_RF_DR_1MBPS;
    }
    return NRF24L01P_NG_RF_DR_2MBPS;
}

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_NG_TYPES_H */
/** @} */
