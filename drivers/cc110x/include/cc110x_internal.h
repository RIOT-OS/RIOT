/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Internal functions of the CC110x transceiver driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef CC110X_INTERNAL_H
#define CC110X_INTERNAL_H

#include "cc110x_calibration.h"
#include "cc110x_communication.h"
#include "cc110x_constants.h"
#include "cc110x_netdev.h"
#include "cc110x_rx_tx.h"
#include "cc110x_settings.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Extract the device state from the status register value
 *
 * @param   status  Contents of the CC110x's status register
 * @return  The state encoded in @p status
 *
 * The status register contains the device state at the bits 1 - 3
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Format of the status byte
 *  0 1 2 3 4 5 6 7
 * +-+-+-+-+-+-+-+-+
 * |R|STATE| FIFO  |
 * +-+-+-+-+-+-+-+-+
 *
 * R = Chip Ready bit (0 = ready, 1 = power and crystal are not yet stable)
 * STATE = The device state
 * FIFO = Number of bytes available in RX FIFO or (in TX mode) number of free
 *        bytes
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Note: The FIFO has a size of 64 bytes. If more than 15 bytes are available
 * for reading in the FIFO (or more than 15 bytes can be written in TX), the
 * value will still show 15. This driver never uses this information, but
 * accesses a dedicated register for that.
 */
static inline cc110x_state_t cc110x_state_from_status(uint8_t status)
{
    return (cc110x_state_t)((status >> 4) & 0x7);
}

/**
 * @brief   Figure out of the transceiver is ready or still powering up
 * @param   status  Contents of the CC110x's status register
 * @retval  1       Transceiver is ready
 * @retval  0       *NOT* ready, still powering up
 *
 * @see cc110x_state_from_status
 */
static inline int cc110x_is_ready_from_status(uint8_t status)
{
    return (status & 0x80) ? 0: 1;
}

#ifdef __cplusplus
}
#endif

#endif /* CC110X_INTERNAL_H */
/** @} */
