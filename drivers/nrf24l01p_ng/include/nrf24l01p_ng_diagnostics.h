/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief   Functions to print NRF24L01+ (NG) debug information
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#include "nrf24l01p_ng.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Print all NRF24L01+ register values
 *
 * @param[in] dev           NRF24L01+ device handle
 */
void nrf24l01p_ng_diagnostics_print_all_regs(const nrf24l01p_ng_t *dev);

/**
 * @brief   Print NRF24L01+ device handle information
 *
 * @param[in] dev           NRF24L01+ device handle
 */
void nrf24l01p_ng_diagnostics_print_dev_info(const nrf24l01p_ng_t *dev);

/**
 * @brief   Print NRF24L01+ frame
 *
 * @param[in] dev           NRF24L01+ device handle
 * @param[in] frame         Frame
 * @param[in] len           Frame length
 */
void nrf24l01p_ng_diagnostics_print_frame(const nrf24l01p_ng_t *dev,
                                          const void *frame, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
