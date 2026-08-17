/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_greth Gaisler GRETH Ethernet MAC driver
 * @ingroup     drivers_netdev
 * @brief       Driver for the Gaisler GRETH Ethernet MAC with EDCL support
 *
 * ## Overview
 *
 * GRETH is a DMA-based Ethernet MAC from Frontgrade Gaisler, commonly found
 * in GRLIB-based FPGA designs (LEON, NOEL-V). This driver implements the
 * RIOT netdev interface for Ethernet.
 *
 * ## Hardware operation
 *
 * The controller uses two rings of buffer descriptors in RAM — one for TX,
 * one for RX. Each descriptor is 8 bytes: a 32-bit control word and a 32-bit
 * buffer address. The CPU prepares descriptors with EN=1 (owned by DMA).
 * The DMA engine processes them and clears EN=0 when done, then raises IRQ 5.
 *
 * @{
 *
 * @file
 * @brief       Interface definitions for the GRETH Ethernet driver
 *
 * @author      Matvii Ivashchenko
 */

#include <stdint.h>
#include <stdbool.h>

#include "net/netdev.h"
#include "net/netdev/eth.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Driver configuration constants
 * @{
 */

/**
 * @brief   Number of RX descriptors (and pre-allocated RX buffers)
 *
 * Must be a power of 2. More descriptors = better throughput at cost of RAM.
 */
#ifndef CONFIG_GRETH_RX_DESC_NUM
#define CONFIG_GRETH_RX_DESC_NUM    (8U)
#endif

/**
 * @brief   Number of TX descriptors
 *
 * Must be a power of 2.
 */
#ifndef CONFIG_GRETH_TX_DESC_NUM
#define CONFIG_GRETH_TX_DESC_NUM    (4U)
#endif

/**
 * @brief   Maximum Ethernet frame size (excluding FCS added by MAC)
 */
#define GRETH_MAX_FRAME_LEN         (1518U)

/**
 * @brief   RX/TX buffer size — rounded up to 32-byte boundary
 */
#define GRETH_BUF_SIZE              (1536U)

/**
 * @brief   PHY auto-negotiate timeout in poll iterations
 */
#define GRETH_ANEG_TIMEOUT          (100000U)
/** @} */

/**
 * @brief   GRETH buffer descriptor (8 bytes, must be kept at natural alignment)
 *
 * The DMA engine accesses this structure directly over the AHB bus.
 * Both fields must remain 32-bit to be compatible with the hardware on rv64.
 */
typedef struct {
    volatile uint32_t ctrl;     /**< Control/status word (see GRETH_BD_*) */
    volatile uint32_t addr;     /**< Physical address of the data buffer */
} greth_desc_t;

/**
 * @brief   GRETH driver parameters (board-level configuration)
 */
typedef struct {
    uint32_t base_addr; /**< APB base address (e.g. 0xff984000 on ZedBoard) */
    unsigned irq;       /**< PLIC interrupt source number (e.g. 5 on ZedBoard) */
    uint8_t  mac[6];    /**< Ethernet MAC address */
} greth_params_t;

/**
 * @brief   GRETH driver device descriptor
 *
 * The @p netdev member must remain first so the driver can be cast between
 * `greth_t *` and `netdev_t *`.
 */
typedef struct {
    netdev_t netdev;            /**< netdev base — MUST be first */

    const greth_params_t *params; /**< Pointer to board configuration */
    greth_desc_t *tx_desc;        /**< TX descriptor ring */
    greth_desc_t *rx_desc;        /**< RX descriptor ring */
    uint8_t      *tx_buf;         /**< flat TX frame buffer */

    unsigned rx_idx;        /**< Index of next RX descriptor to check */
    unsigned tx_idx;        /**< Index of next TX descriptor to use */

    unsigned phy_addr;      /**< PHY MDIO address, read from hardware */
    bool     gbit;          /**< True if gigabit mode is available */
} greth_t;

/**
 * @brief   Set up the GRETH device descriptor
 *
 * Must be called before the netdev `init` callback. Wires the netdev driver
 * pointer and stores the board parameters. Does not touch hardware.
 *
 * @param[out]  dev     Driver state to initialise
 * @param[in]   params  Board-level configuration (address, IRQ, MAC)
 * @param[in]   index   Instance index for netdev_register
 */
void greth_setup(greth_t *dev, const greth_params_t *params, uint8_t index);

#ifdef __cplusplus
}
#endif

/** @} */
