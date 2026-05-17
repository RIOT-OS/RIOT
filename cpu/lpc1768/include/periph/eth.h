/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       Low-level Ethernet driver interface for the LPC1768 EMAC
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "iolist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the EMAC peripheral and PHY
 *
 * @param[in] mac    6-byte MAC address to program into SA0/SA1/SA2
 *
 * @retval  0 on success
 * @retval  -ENODEV if the PHY could not be detected
 * @retval  -EIO if the PHY could not be initialized
 */
int lpc1768_eth_init(const uint8_t *mac);

/**
 * @brief   Submit a frame for transmission
 *
 * The frame data is gathered from the @p iolist into the next available TX
 * buffer in AHB SRAM.
 *
 * @param[in] iolist    IO vector list to send
 *
 * @retval  >0 number of bytes queued for transmission
 * @retval  -EBUSY on TX descriptor timeout
 * @retval  -EOVERFLOW if the gathered frame exceeds the MTU
 */
int lpc1768_eth_send(const iolist_t *iolist);

/**
 * @brief   Receive a frame from the RX ring
 *
 * Behaviour mirrors the netdev `recv()` contract:
 *  - `buf == NULL && max_len == 0`: return the length of the frame at head.
 *  - `buf == NULL && max_len > 0` : drop the head frame, return its length.
 *  - `buf != NULL`                : copy `min(len, max_len)` bytes into
 *                                   @p buf, advance the ring, return the
 *                                   number of bytes copied.
 *
 * @param[out] buf       Destination buffer (may be NULL)
 * @param[in]  max_len   Capacity of @p buf
 *
 * @retval  >0 number of bytes successfully received
 * @retval  -ENODATA if the ring is empty
 */
int lpc1768_eth_recv(void *buf, size_t max_len);

/**
 * @brief   Read the currently programmed MAC address
 *
 * @param[out] out  Destination buffer of at least 6 bytes
 */
void lpc1768_eth_get_mac(uint8_t out[6]);

/**
 * @brief   Program a new MAC address
 *
 * @param[in] mac   6-byte MAC address to set
 */
void lpc1768_eth_set_mac(const uint8_t mac[6]);

/**
 * @brief   Read the PHY link status (BMSR.LINK)
 *
 * @retval  true if the link is up
 * @retval  false if the link is down
 */
bool lpc1768_eth_link_up(void);

/**
 * @brief   Return true if the RX ring contains at least one unconsumed frame
 *
 * @retval  true if at least one frame is pending in the RX ring
 * @retval  false if the RX ring is empty
 */
bool lpc1768_eth_rx_pending(void);

/**
 * @brief   Query the completion status of the most recent transmission
 *
 * Checks whether the descriptor queued by the most recent @ref lpc1768_eth_send
 * has been consumed by the EMAC and, if so, inspects its transmit status word.
 *
 * This method must only be called after a transmission was started by
 * @ref lpc1768_eth_send. The return value is only valid for the most recent
 * transmission.
 *
 * @retval  >0 number of bytes successfully transmitted
 * @retval  -EAGAIN if the frame is still being transmitted
 * @retval  -EBUSY if the frame was aborted due to a collision
 * @retval  -EIO if the frame failed for any other reason
 */
int lpc1768_eth_tx_status(void);

/**
 * @brief   Enable or disable promiscuous mode
 *
 * @param[in] enable    true to enable promiscuous mode, false to disable
 */
void lpc1768_eth_set_promiscuous(bool enable);

/**
 * @brief   Return the current promiscuous mode setting
 *
 * @retval  true if promiscuous mode is enabled
 * @retval  false if promiscuous mode is disabled
 */
bool lpc1768_eth_get_promiscuous(void);

/**
 * @brief   Configure the MAC for a given link speed and duplex
 *
 * Programs the SUPP, MAC2, Command and IPGT registers to match the requested
 * speed and duplex. Used both to apply a statically configured link speed and
 * to apply the result of auto-negotiation.
 *
 * @param[in] speed     bitmask of MII BMCR speed/duplex bits
 *                      (@ref MII_BMCR_SPEED_100, @ref MII_BMCR_FULL_DPLX)
 */
void lpc1768_eth_set_link_speed(uint16_t speed);

/**
 * @brief   Start PHY auto-negotiation
 *
 * Advertises all 10/100 Mbps half- and full-duplex capabilities and restarts
 * auto-negotiation on the PHY. The negotiation runs asynchronously: call
 * @ref lpc1768_eth_complete_auto_negotiation once the link is up to apply the
 * negotiated parameters to the MAC.
 *
 * @retval 0 on success
 * @retval -EOPNOTSUPP if the PHY does not support auto-negotiation
 */
int lpc1768_eth_start_auto_negotiation(void);

/**
 * @brief   Complete auto-negotiation and apply speed/duplex to the MAC
 *
 * Waits until the PHY reports auto-negotiation complete, then derives the
 * negotiated speed and duplex from the advertised and link-partner capability
 * registers and programs SUPP, MAC2, Command and IPGT accordingly.
 */
void lpc1768_eth_complete_auto_negotiation(void);

#ifdef __cplusplus
}
#endif

/** @} */
