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
 * @brief   Internal helper functions and defines for
 *          the NRF24L01P device driver
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_INTERNAL_H
#define NRF24L01P_INTERNAL_H

#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Agreed layer-2 address to send broadcast frames to
 *
 * A node that wants to receive broadcast frames must set it´s
 * pipe 1 address to that address.
 */
#define NRF24L01P_BROADCAST_ADDR            { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

/**
 * @brief   Address that instructs the driver to auto generate
 *          a layer-2 address for pipe 0 and pipe 1
 *          @see module_luid
 */
#define NRF24L01P_L2ADDR_AUTO               { 0x00, 0x00, 0x00, 0x00, 0x00 }

/**
 * @brief   Address that instructs the driver to auto generate
 *          a leyer-2 address for pipe 2, pipe 3, pipe 4 and pipe 5
 *          @see module_luid
 */
#define NRF24L01P_L2ADDR_UNDEF              (0x00)

/**
 * @brief   Check if device is awake
 *
 * @param[in] dev           NRF24L01P device handle
 *
 * @retval !=0              Awake
 * @retval 0                Asleep
 */
int nrf24l01p_assert_awake(const nrf24l01p_t *dev);

/**
 * @brief   Restore rx address from params after a transmission
 *
 * @param[in] dev           NRF24L01P device handle
 */
void nrf24l01p_restore_address(const nrf24l01p_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_INTERNAL_H */
/** @} */
