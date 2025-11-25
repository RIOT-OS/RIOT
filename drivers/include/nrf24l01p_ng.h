/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_nrf24l01p_ng NRF24L01+ (NG) 2.4 GHz trasceiver driver
 * @ingroup     drivers_netdev
 *
 * This module contains the driver for the NRF24L01+ (NG) 2.4 GHz
 * transceiver.
 *
 * @{
 * @file
 * @brief   Public interface for NRF24L01+ (NG) devices
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#include <stdint.h>

#include "board.h"
#include "kernel_defines.h"
#include "net/gnrc/nettype.h"
#include "net/netdev.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#include "nrf24l01p_ng_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Minimum width of a NRF24L01+ layer-2 address
 */
#define NRF24L01P_NG_MIN_ADDR_WIDTH         (3)

/**
 * @brief   Maximum width of a NRF24L01+ layer-2 address
 */
#define NRF24L01P_NG_MAX_ADDR_WIDTH         (5)

/**
 * @brief Pass the compiler a definition of NRF24L01P_NG_ADDR_WIDTH to configure
 *        the layer-2 address width of this transceiver.
 */
#ifndef NRF24L01P_NG_ADDR_WIDTH
#define NRF24L01P_NG_ADDR_WIDTH             NRF24L01P_NG_MAX_ADDR_WIDTH
#elif (NRF24L01P_NG_ADDR_WIDTH < NRF24L01P_NG_MIN_ADDR_WIDTH) || \
      (NRF24L01P_NG_ADDR_WIDTH > NRF24L01P_NG_MAX_ADDR_WIDTH)
#error "NRF24L01P_NG_ADDR_WIDTH must be within [3, 5]"
#endif

/**
 * @brief   Agreed layer-2 address to send broadcast frames to
 *
 * The broadcast address will be assigned to pipe 0.
 */
#define NRF24L01P_NG_BROADCAST_ADDR         { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

/**
 * @brief   Shortcut to access the address of pipe 0
 */
#define NRF24L01P_NG_ADDR_P0(dev)           ((dev)->arxaddr.rx_addr_long[NRF24L01P_NG_P0])

/**
 * @brief   Shortcut to access the address of pipe 1
 */
#define NRF24L01P_NG_ADDR_P1(dev)           ((dev)->arxaddr.rx_addr_long[NRF24L01P_NG_P1])

/**
 * @brief   Shortcut to access the least significant byte of the
 *          addresses pipe 2, pipe 3, pipe 4 or pipe 5
 *
 * The other bytes are shared with the address of pipe 1
 */
#define NRF24L01P_NG_ADDR_PX_LSB(dev, pipe) ((dev)->arxaddr.rx_addr_short[(pipe) - 2])

/**
 * @brief   Struct that holds all active configuration values
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief padding
     */
    uint8_t :                    1;
    /**
     * @brief   Current channel
     */
    uint8_t cfg_channel :        7;
    /**
     * @brief padding
     */
    uint8_t :                    2;
    /**
     * @brief   Current CRC length configuration value
     *          @see nrf24l01p_ng_crc_t
     */
    uint8_t cfg_crc :            2;
    /**
     * @brief   Current tx power configuration value
     *          @see nrf24l01p_ng_tx_power_t
     */
    uint8_t cfg_tx_power :       2;
    /**
     * @brief   Current data rate configuration value
     *          @see nrf24l01p_ng_rfdr_t
     */
    uint8_t cfg_data_rate :      2;
    /**
     * @brief   Current maximum number of retransmissions
     */
    uint8_t cfg_max_retr :       4;
    /**
     * @brief   Current retransmission delay configuration value
     *          @see nrf24l01p_ng_ard_t
     */
    uint8_t cfg_retr_delay :     4;
} nrf24l01p_ng_cfg_t;

/**
 * @brief Struct of NRF24L01+ initialization parameters
 */
typedef struct {
    spi_t spi;                      /**< SPI bus */
    spi_clk_t spi_clk;              /**< SPI clock speed */
    gpio_t pin_cs;                  /**< SPI chip select gpio pin */
    gpio_t pin_ce;                  /**< NRF24L01+ chip enable gpio pin */
    gpio_t pin_irq;                 /**< NRF24L01+ IRQ gpio pin */
    nrf24l01p_ng_cfg_t config;      /**< Current configuration values */
} nrf24l01p_ng_params_t;

/**
 * @brief   NRF24L01+ device struct
 */
struct nrf24l01p_ng {
    netdev_t netdev;                /**< Netdev member */
    nrf24l01p_ng_params_t params;   /**< Parameters */
    uint8_t state;                  /**< Current operation state */
    uint8_t idle_state;             /**< State to return to after sending */
    /**
     * @brief   Rx addresses
     *
     * The NRF24L01+ has 6 data pipes.
     * Each data pipe has a unique address.
     * The address of a pipe can be 3 to 5 bytes long
     * but must be equal for all pipes.
     * The addresses of pipe 0 and pipe 1 can be arbitrary while the addresses
     * of pipe 2 to pipe 5 share the most significant bytes, thus they only
     * differ in the least significant byte.
     * The least significant byte must be different for all pipes.
     *
     * Data pipe 0 is also the Tx pipe.
     * That means if you want to send a frame, you must assign to pipe 0
     * the address of the recipient. After a transmission if you want to
     * continue listening on pipe 0, the address of pipe 0 must be restored.
     */
    struct {
        /**
         * @brief   Array to access the addresses of pipe 0 and pipe 1 via
         *          pipe indices @see nrf24l01p_ng_pipe_t
         */
        uint8_t rx_addr_long[2][NRF24L01P_NG_ADDR_WIDTH];
        /**
         * @brief   Array to access the addresses of pipe 2, pipe 3,
         *          pipe 4 and pipe 5 via pipe indices
         *          @see nrf24l01p_ng_pipe_t
         */
        uint8_t rx_addr_short[4];
    } arxaddr;
};

/**
 * @brief   Setup the NRF24L01+ driver, but perform no initialization
 *
 * @ref netdev_driver_t::init can be used after this call to initialize the
 * transceiver.
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] params        Parameters of the device to setup
 * @param[in] index         Index of @p params in a global parameter struct array.
 *                          If initialized manually, pass a unique identifier instead.
 *
 * @retval 0                Device successfully set up
 */
int nrf24l01p_ng_setup(nrf24l01p_ng_t *dev,
                       const nrf24l01p_ng_params_t *params,
                       uint8_t index);

/**
 * @brief   Enable or disable data pipe @p pipe
 *
 *          If @p pipe should be enabled, it is advised to
 *          set an address in advance.
 *          @see nrf24l01p_ng_set_rx_addr
 *          For Enhanced ShockBurst, auto ACK and dynamic
 *          payload length are enabled
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] pipe          Pipe index
 * @param[in] enable        If pipe should be enabled or disabled
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad pipe index
 */
int nrf24l01p_ng_set_enable_pipe(nrf24l01p_ng_t *dev, nrf24l01p_ng_pipe_t pipe,
                                 bool enable);

/**
 * @brief   Query if data pipe @p pipe is enabled
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] pipe          Pipe index
 * @param[out] enable       If pipe is enabled or disabled
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad pipe index
 */
int nrf24l01p_ng_get_enable_pipe(nrf24l01p_ng_t *dev, nrf24l01p_ng_pipe_t pipe,
                                 bool* enable);

/**
 * @brief   Configure air data rate
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] data_rate     Data rate configuration value
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad data rate value
 * @retval -EAGAIN          Current state does not permit changing data rate
 */
int nrf24l01p_ng_set_air_data_rate(nrf24l01p_ng_t *dev,
                                   nrf24l01p_ng_rfdr_t data_rate);

/**
 * @brief   Get currently configured data rate
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[out] data_rate    Configuration data rate value (may be NULL)
 *
 * @return                  Data rate in [kbit/s]
 */
uint16_t nrf24l01p_ng_get_air_data_rate(const nrf24l01p_ng_t *dev,
                                        nrf24l01p_ng_rfdr_t *data_rate);

/**
 * @brief   Configure CRC length
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] crc           Configuration CRC value
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad CRC configuration value
 * @retval -ENOTSUP         CRC length not supported
 * @retval -EAGAIN          Current state does not permit changing CRC length
 */
int nrf24l01p_ng_set_crc(nrf24l01p_ng_t *dev, nrf24l01p_ng_crc_t crc);

/**
 * @brief   Get currently configured CRC length
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[out] crc          Configuration crc value (may be NULL)
 *
 * @return                  Current CRC length
 */
uint8_t nrf24l01p_ng_get_crc(const nrf24l01p_ng_t *dev,
                             nrf24l01p_ng_crc_t *crc);

/**
 * @brief   Configure Tx trasceiver power
 *
 * @param[in] dev           NRf24L01+ device handle
 * @param[in] power         Configuration Tx power value
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad Tx power configuration value
 * @retval -EAGAIN          Current state does not permit changing Tx power
 */
int nrf24l01p_ng_set_tx_power(nrf24l01p_ng_t *dev,
                              nrf24l01p_ng_tx_power_t power);

/**
 * @brief   Get currently configured Tx transceiver power
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[out] power        Configuration Tx power value
 *
 * @return                  Tx power in [dbm]
 */
int8_t nrf24l01p_ng_get_tx_power(const nrf24l01p_ng_t *dev,
                                 nrf24l01p_ng_tx_power_t *power);

/**
 * @brief   Set transceiver channel
 *
 * @param[in] dev           NRF24l01+ device handle
 * @param[in] channel       Channel [0; 15]
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad channel
 * @retval -EAGAIN          Current state does not permit switching channel
 */
int nrf24l01p_ng_set_channel(nrf24l01p_ng_t *dev, uint8_t channel);

/**
 * @brief   Get currently configured transceiver channel
 *
 * @param[in] dev           NRF24L01+ device handle
 *
 * @return                  Transceiver channel
 */
uint8_t nrf24l01p_ng_get_channel(const nrf24l01p_ng_t *dev);

/**
 * @brief   Set Rx address of a certain data pipe
 *
 * If you want to set the address of pipe 2, 3, 4 or 5
 * you only set the LSB because the other bytes are
 * equal to the address of pipe 1.
 *
 * For pipe 0 and 1 NRF24L01P_NG_ADDR_WIGTH bytes are expected
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] addr          Rx address
 * @param[in] pipe          Pipe index
 *
 * @retval 0                Success
 * @retval -EINVAL          Bad address length
 * @return -EAGAIN          Current state does not permit changing Rx address
 */
int nrf24l01p_ng_set_rx_address(nrf24l01p_ng_t *dev, const uint8_t *addr,
                                nrf24l01p_ng_pipe_t pipe);

/**
 * @brief   Get current Rx address of a certain data pipe
 *
 * @p addr must be at least NRF24L01P_NG_ADDR_WIDTH wide.
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[out] addr         Rx address
 * @param[in] pipe          Pipe index
 *
 * @return                  Address width
 * @retval -EINVAL          Bad pipe index
 */
int nrf24l01p_ng_get_rx_address(const nrf24l01p_ng_t *dev, uint8_t *addr,
                                nrf24l01p_ng_pipe_t pipe);

/**
 * @brief   Configure maximum number of retransmissions for ESB
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] max_rt        Number of maximum retransmissions [0; 15]
 *
 * @retval 0                Success
 * @retval -EINVAL          Unsupported number of retransmissions
 * @retval -EAGAIN          Current state does not permit changing
 *                          the maximum number of retransmissions
 */
int nrf24l01p_ng_set_max_retransm(nrf24l01p_ng_t *dev, uint8_t max_rt);

/**
 * @brief   Get currently configured number of maximum retransmissions for ESB
 *
 * @param[in] dev           NRF24L01+ device handle
 *
 * @return                  Maximum number of retransmissions
 */
uint8_t nrf24l01p_ng_get_max_retransm(const nrf24l01p_ng_t *dev);

/**
 * @brief   Set retransmission delay for ESB
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] rt_delay      Configuration retransmission delay value
 *
 * @return 0
 * @return -EINVAL          Bad retransmission delay value
 * @return -EAGAIN          Current state does not permit changing
 *                          retransmission delay
 */
int nrf24l01p_ng_set_retransm_delay(nrf24l01p_ng_t *dev,
                                    nrf24l01p_ng_ard_t rt_delay);

/**
 * @brief   Get retransmission delay for ESB
 *
 * @param[in] dev          NRF24L01+ device handle
 * @param[out] rt_delay    Configuration retransmission delay value
 *
 * @return                  Retransmission delay in [us]
 */
uint16_t nrf24l01p_ng_get_retransm_delay(const nrf24l01p_ng_t *dev,
                                         nrf24l01p_ng_ard_t *rt_delay);

/**
 * @brief   Put device into
 *          sleep mode(@ref NRF24L01P_NG_STATE_POWER_DOWN),
 *          standby mode (@ref NRF24L01P_NG_STATE_STANDBY_1),
 *          or Rx mode (@ref NRF24L01P_NG_STATE_RX_MODE)
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] state         State
 *
 * @return                  Old state
 * @retval -EAGAIN          Device is currently not permitted to change state
 * @retval -ENOTSUP         Device is not permitted to change state to @p state
 */
int nrf24l01p_ng_set_state(nrf24l01p_ng_t *dev, nrf24l01p_ng_state_t state);

/**
 * @brief   Get current device state
 *
 * @param[in] dev             NRf24L01+ device handle
 *
 * @return                    Device state
 */
nrf24l01p_ng_state_t nrf24l01p_ng_get_state(const nrf24l01p_ng_t *dev);

/**
 * @brief   Retrieve a unique layer-2 address for an nrf24l01p_ng instance
 *
 * @note    This function has __attribute__((weak)) so you can override this, e.g.
 *          to construct an address. By default @ref luid_get_lb is used.
 *
 * @param[in]   dev     The device descriptor of the transceiver
 * @param[out]  eui     Destination to write the address to
 */
void nrf24l01p_ng_eui_get(const netdev_t *dev, uint8_t *eui);

#if IS_USED(MODULE_NRF24L01P_NG_DIAGNOSTICS)
/**
 * @brief Get state variable as a string
 *
 * @param[in] state     State
 *
 * @return              @p state as a string
 */
const char *
nrf24l01p_ng_diagnostics_state_to_string(nrf24l01p_ng_state_t state);

/**
 * @brief Convert string to state variable
 *
 * @param[in] sstate    State string
 *
 * @return              State variable
 */
nrf24l01p_ng_state_t
nrf24l01p_ng_diagnostics_string_to_state(const char *sstate);

/**
 * @brief Print all registers
 *
 * @param[in] dev       NRf24L01+ device handle
 */
void nrf24l01p_ng_print_all_regs(nrf24l01p_ng_t *dev);

/**
 * @brief Print device parameters
 *
 * @param[in] dev       NRf24L01+ device handle
 */
void nrf24l01p_ng_print_dev_info(const nrf24l01p_ng_t *dev);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
