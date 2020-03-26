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
 * @brief   Functions to print NRF24L01P debug information
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_DIAGNOSTICS_H
#define NRF24L01P_DIAGNOSTICS_H

#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Print all NRF24L01P register values
 *
 * @param[in] dev           NRF24L01P device handle
 */
void nrf24l01p_diagnostics_print_all_regs(const nrf24l01p_t *dev);

/**
 * @brief   Print NRF24L01P device handle information
 *
 * @param[in] dev           NRF24L01P device handle
 */
void nrf24l01p_diagnostics_print_dev_info(const nrf24l01p_t *dev);

/**
 * @brief   Print NRF24L01P frame
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] frame         Frame
 * @param[in] len           Frame length
 */
void nrf24l01p_diagnostics_print_frame(const nrf24l01p_t *dev,
                                       const void *frame, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_DIAGNOSTICS_H */
/** @} */
