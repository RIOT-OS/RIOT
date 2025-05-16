/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Calibration related functions of the CC110x transceiver driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Perform a recalibration of the transceiver
 *
 * @param   dev     The device descriptor of the transceiver
 *
 * @retval  0       Success
 * @retval  -EIO    Failed
 *
 * @pre     @p dev has been acquired using @ref cc110x_acquire
 * @pre     Transceiver is in IDLE state
 * @post    On success @p dev is still acquired, the caller has to release
 *          it. On failure the SPI bus is **already** **released**
 * @post    Transceiver is again in IDLE state, calibration has been
 *          performed and calibration data has been backed up on MCU.
 */
int cc110x_recalibrate(cc110x_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
