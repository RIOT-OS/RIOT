/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    drivers_nrf24l01p NRF24L01P 2.4 GHz trasceiver driver
 * @ingroup     drivers_netdev
 *
 * This module contains the driver for the NRF24L01P 2.4 GHz
 * transceiver.
 *
 * @{
 * @file
 * @brief   Public interface for NRF24L01P devices
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_H
#define NRF24L01P_H

#include <stdint.h>

#include "kernel_defines.h"
#include "net/gnrc/nettype.h"
#include "net/netdev.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#include "nrf24l01p_types.h"
#include "nrf24l01p_conversion.h"
#if IS_USED(MODULE_NRF24L01P_ADVANCED)
#include "nrf24l01p_advanced.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Minimum width of a NRF24L01P layer-2 address
 */
#define NRF24L01P_MIN_ADDR_WIDTH            (3)

/**
 * @brief   Maximum width of a NRF24L01P layer-2 address
 */
#define NRF24L01P_MAX_ADDR_WIDTH            (5)

/**
 * @brief   Struct that holds all active configuration values
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief Padding for byte alignment
     */
    uint8_t :                   5;
    /**
     * @brief   Currently configured protocol
     *          (Do not change after initialization)
     *          @see nrf24l01p_protocol_t
     */
    uint8_t cfg_protocol :       1;
    /**
     * @brief   Current CRC length configuration value
     *          @see nrf24l01p_crc_t
     */
    uint8_t cfg_crc :            2;
    /**
     * @brief   Current tx power configuration value
     *          @see nrf24l01p_tx_power_t
     */
    uint8_t cfg_tx_power :       2;
    /**
     * @brief   Current data rate configuration value
     *          @see nrf24l01p_rfdr_t
     */
    uint8_t cfg_data_rate :      2;
    /**
     * @brief   Current channel
     */
    uint8_t cfg_channel :        4;
    /**
     * @brief   Current address width configuration value
     *          (Do not change after initialization
     *          @see nrf24l01p_aw_t
     */
    uint8_t cfg_addr_width :     2;
    /**
     * @brief   Current maximum number of retransmissions
     *          (Only used if protocol is ESB)
     */
    uint8_t cfg_max_retr :       4;
    /**
     * @brief   Current retransmission delay configuration value
     *          (Only used if protocol is ESB) @see nrf24l01p_ard_t
     */
    uint8_t cfg_retr_delay :     4;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 0 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p0 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 1 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p1 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 2 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p2 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 3 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p3 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 4 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p4 :    5;
    /**
     * @brief   Current number of padding bytes to configure
     *          the payload width of pipe 5 (Only used if protocol is SB)
     */
    uint8_t cfg_plw_padd_p5 :    5;
} nrf24l01p_cfg_t;

/**
 * @brief Struct of NRF24L01P initialization parameters
 */
typedef struct {
    spi_t spi;          /**< SPI bus */
    spi_clk_t spi_clk;  /**< SPI clock speed */
    gpio_t pin_cs;      /**< SPI chip select gpio pin */
    gpio_t pin_ce;      /**< NRF24L01P chip enable gpio pin */
    gpio_t pin_irq;     /**< NRF24L01P IRQ gpio pin */
    union {
        struct {
            /**
             * @brief   Array to access the addresses of pipe 0 and pipe 1 via
             *          pipe indices @see nrf24l01p_pipe_t
             */
            uint8_t rx_addr_long[2][NRF24L01P_MAX_ADDR_WIDTH];
            /**
             * @brief   Array to access the addresses of pipe 2, pipe 3,
             *          pipe 4 and pipe 5 via pipe indices
             *          @see nrf24l01p_pipe_t
             */
            uint8_t rx_addr_short[4];
        } arxaddr; /**< Rx addresses as arrays */
        struct {
            /**
             * @brief   pipe 0 Rx address
             */
            uint8_t rx_pipe_0_addr[NRF24L01P_MAX_ADDR_WIDTH];
            /**
             * @brief   pipe 1 Rx address
             */
            uint8_t rx_pipe_1_addr[NRF24L01P_MAX_ADDR_WIDTH];
            /**
             * @brief   pipe 2 Rx address
             */
            uint8_t rx_pipe_2_addr;
            /**
             * @brief   pipe 3 Rx address
             */
            uint8_t rx_pipe_3_addr;
            /**
             * @brief   pipe 4 Rx address
             */
            uint8_t rx_pipe_4_addr;
            /**
             * @brief   pipe 5 Rx address
             */
            uint8_t rx_pipe_5_addr;
        } rxaddrpx; /**< Rx addresses as named variables */
    } urxaddr; /**< Union of Rx addresses as named variables and as arrays */
    nrf24l01p_cfg_t config; /**< Current configuration values */
} nrf24l01p_params_t;

/**
 * @brief   NRF24L01P device struct
 */
struct nrf24l01p {
    netdev_t netdev;            /**< Netdev member */
    nrf24l01p_params_t params;  /**< Parameters */
    uint8_t state;              /**< Current operation state */
    uint8_t idle_state;         /**< State to return to after sending */
#if !defined (NDEBUG) || defined(DOXYGEN)
    uint8_t have_spi_access;    /**< != 0: dev already has SPI bus acquired*/
    uint8_t transitions;        /**< Possible transitions from current state */
#endif
};

/**
 * @brief   Setup the NRF24L01P driver, but perform no initialization
 *
 * @ref netdev_driver_t::init can be used after this call to initialize the
 * transceiver.
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] params        Parameters of the device to setup
 *
 * @retval 0                Device successfully set up
 * @retval -ENOTSUP         Parameter request could be satisfied
 */
int nrf24l01p_setup(nrf24l01p_t *dev, const nrf24l01p_params_t *params);

/**
 * @brief   Configure air data rate
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] data_rate     Data rate configuration value
 *
 * @retval 0                Success
 * @retval -EINVAL         Bad data rate value
 * @retval -EAGAIN          Current state does not permit changing data rate
 */
int nrf24l01p_set_air_data_rate(nrf24l01p_t *dev, nrf24l01p_rfdr_t data_rate);

/**
 * @brief   Get currently configured data rate
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[out] data_rate    Configuration data rate value (may be NULL)
 *
 * @return                  Data rate in [kbit/s]
 */
uint16_t nrf24l01p_get_air_data_rate(const nrf24l01p_t *dev,
                                     nrf24l01p_rfdr_t *data_rate);

/**
 * @brief   Configure CRC length
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] crc           Configuration CRC value
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad CRC configuration value
 * @retval -EAGAIN          Current state does not permit changing CRC length
 */
int nrf24l01p_set_crc(nrf24l01p_t *dev, nrf24l01p_crc_t crc);

/**
 * @brief   Get currently configured CRC length
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[out] crc          Configuration crc value (may be NULL)
 *
 * @return                  Current CRC length
 */
uint8_t nrf24l01p_get_crc(const nrf24l01p_t *dev, nrf24l01p_crc_t *crc);

/**
 * @brief   Configure Tx trasceiver power
 *
 * @param[in] dev           NRf24L01P device handle
 * @param[in] power         Configuration Tx power value
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad Tx power configuration value
 * @retval -EAGAIN          Current state does not permit changin Tx power
 */
int nrf24l01p_set_tx_power(nrf24l01p_t *dev, nrf24l01p_tx_power_t power);

/**
 * @brief   Get currently configured Tx transceiver power
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[out] power        Configuration Tx power value
 *
 * @return                  Tx power in [dbm]
 */
int8_t nrf24l01p_get_tx_power(const nrf24l01p_t *dev,
                              nrf24l01p_tx_power_t *power);

/**
 * @brief   Set transceiver channel
 *
 * @param[in] dev           NRF24l01P device handle
 * @param[in] channel       Channel [0; 15]
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad channel
 * @retval -EAGAIN          Current state does not permit switching channel
 */
int nrf24l01p_set_channel(nrf24l01p_t *dev, uint8_t channel);

/**
 * @brief   Get currently configured transceiver channel
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  Transceiver channel
 */
uint8_t nrf24l01p_get_channel(const nrf24l01p_t *dev);

/**
 * @brief   Configure expected MTU of a certain data pipe
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] mtu           MTU
 * @param[in] pipe          Pipe index
 *
 * @retval 0                Success
 * @retval -ERANGE          Bad pipe index
 * @retval -EINVAL          Bad payload width
 * @retval -EAGAIN          Current state does not permit changing payload width
 */
int nrf24l01p_set_mtu(nrf24l01p_t *dev, uint8_t mtu, nrf24l01p_pipe_t pipe);

/**
 * @brief   Get currently configured expected MTU of a certain data pipe
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] pipe          Pipe index
 *
 * @return                  MTU
 * @retval NRF24L_MTU       Dynamic length for ESB protocol
 * @retval -ERANGE          Bad pipe index
 */
int nrf24l01p_get_mtu(const nrf24l01p_t *dev, nrf24l01p_pipe_t pipe);

/**
 * @brief   Set Rx address of a certain data pipe
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] addr          Rx address
 * @param[in] addr_len      Address length (must match current address length)
 * @param[in] pipe          Pipe index
 *
 * @retval 0                Success
 * @retval -ERANGE          Bad
 * @retval -EINVAL          Bad address length
 * @return -EAGAIN          Current state does not permit changin Rx address
 */
int nrf24l01p_set_rx_address(nrf24l01p_t *dev, const uint8_t *addr,
                             size_t addr_len, nrf24l01p_pipe_t pipe);

/**
 * @brief   Get current Rx address of a certain data pipe
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[out] addr         Rx address
 * @param[in] pipe          Pipe index
 *
 * @return                  Address width
 * @retval -ERANGE          Bad pipe index
 */
int nrf24l01p_get_rx_address(const nrf24l01p_t *dev, uint8_t *addr,
                             nrf24l01p_pipe_t pipe);

/**
 * @brief   Configure maximum number of retransmissions for ESB
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] max_rt        Number of maximum retransmissions [0; 15]
 *
 * @retval 0                Success
 * @retval -ENOTSUP         Protocol is SB
 * @retval -EINVAL          Unsupported number of retransmissions
 * @retval -EAGAIN          State does not permit changing the maximum number
 *                          of retransmissions
 */
int nrf24l01p_set_max_retransm(nrf24l01p_t *dev, uint8_t max_rt);

/**
 * @brief   Get currently configured number of maximum retransmissions for ESB
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  Maximum number of retransmissions
 */
uint8_t nrf24l01p_get_max_retransm(const nrf24l01p_t *dev);

/**
 * @brief   Set retransmission delay for ESB
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] rt_delay      Configuration retransmission delay value
 *
 * @return 0
 * @retval -ENOTSUP         Protocol is SB
 * @return -EINVAL          Bad retransmission delay value
 * @return -EAGAIN          Current state does not permit changing the
 *                          retransmission delay
 */
int nrf24l01p_set_retransm_delay(nrf24l01p_t *dev, nrf24l01p_ard_t rt_delay);

/**
 * @brief   Get retransmission delay for ESB
 *
 * @param[in] dev          NRF24L01P device handle
 * @param[out] rt_delay    Configuration retransmission delay value
 *
 * @return                  Retransmission delay in [us]
 */
uint16_t nrf24l01p_get_retransm_delay(const nrf24l01p_t *dev,
                                      nrf24l01p_ard_t *rt_delay);

/**
 * @brief   Put device into
 *          sleep mode(@ref NRF24L01P_STATE_POWER_DOWN),
 *          standby mode (@ref NRF24L01P_STATE_STANDBY_1),
 *          or Rx mode (@ref NRF24L01P_STATE_RX_MODE)
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] state         State
 *
 * @return                  Old state
 * @retval -EAGAIN          Device is currently not permitted to change state
 * @retval -ENOTSUP         Device is not permitted to change state to @p state
 */
int nrf24l01p_set_state(nrf24l01p_t *dev, nrf24l01p_state_t state);

/**
 * @brief   Get current device state
 *
 * @param[in] dev             NRf24L01P device handle
 *
 * @return                    Device state
 */
nrf24l01p_state_t nrf24l01p_get_state(const nrf24l01p_t *dev);

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 0
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p0(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 1
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p1(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 2
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p2(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 3
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p3(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p4(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_mtu to set the
 *          payload width of pipe 5
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] width     paylaod width
 *
 * @return              @see nrf24l01p_set_mtu
 */
static inline int nrf24l01p_set_mtu_p5(nrf24l01p_t *dev, uint8_t width)
{
    return nrf24l01p_set_mtu(dev, width, NRF24L01P_P5);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 0
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p0(const nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 1
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p1(const nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 2
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p2(const nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 3
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p3(const nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p4(const nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_mtu to get the
 *          payload width of pipe 5
 *
 * @param[in] dev       NRf24L01P device handle
 *
 * @return              @see nrf24l01p_get_mtu
 */
static inline int nrf24l01p_get_mtu_p5(const nrf24l01p_t *dev)
{
    return nrf24l01p_get_mtu(dev, NRF24L01P_P5);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 0
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p0(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 1
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p1(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 2
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p2(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 3
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p3(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p4(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_rx_address to set the
 *          address of pipe 5
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[in] addr      Address
 * @param[in] addr_len  Address width, must match current address width
 *
 * @return              @see nrf24l01p_set_rx_address
 */
static inline int nrf24l01p_set_rx_address_p5(nrf24l01p_t *dev,
                                              const uint8_t *addr,
                                              size_t addr_len)
{
    return nrf24l01p_set_rx_address(dev, addr, addr_len, NRF24L01P_P5);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 0
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p0(const nrf24l01p_t *dev,
                                              uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 1
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p1(const nrf24l01p_t *dev,
                                              uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 2
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p2(const nrf24l01p_t *dev,
                                              uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 3
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p3(const nrf24l01p_t *dev,
                                              uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p4(const nrf24l01p_t *dev,
                                              uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_get_rx_address to set the
 *          address of pipe 4
 *
 * @param[in] dev       NRf24L01P device handle
 * @param[out] addr     Address
 *
 * @return              @see nrf24l01p_get_rx_address
 */
static inline int nrf24l01p_get_rx_address_p5(const nrf24l01p_t *dev,
                                              uint8_t *addr)
{
    return nrf24l01p_get_rx_address(dev, addr, NRF24L01P_P5);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_state
 *          to go to sleep mode
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  @see nrf24l01p_set_state
 */
static inline int nrf24l01p_set_state_sleep(nrf24l01p_t *dev)
{
    return nrf24l01p_set_state(dev, NRF24L01P_STATE_POWER_DOWN);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_state
 *          to go to idle mode
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  @see nrf24l01p_set_state
 */
static inline int nrf24l01p_set_state_idle(nrf24l01p_t *dev)
{
    return nrf24l01p_set_state(dev, NRF24L01P_STATE_STANDBY_1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_state
 *          to go to Rx mode
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @return                  @see nrf24l01p_set_state
 */
static inline int nrf24l01p_set_state_rx(nrf24l01p_t *dev)
{
    return nrf24l01p_set_state(dev, NRF24L01P_STATE_RX_MODE);
}

/**
 * @brief Print all registers
 *
 * @param[in] dev       NRf24L01P device handle
 */
void nrf24l01p_print_all_regs(nrf24l01p_t *dev);

/**
 * @brief Print device parameters
 *
 * @param[in] dev       NRf24L01P device handle
 */
void nrf24l01p_print_dev_info(const nrf24l01p_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_H */
/** @} */
