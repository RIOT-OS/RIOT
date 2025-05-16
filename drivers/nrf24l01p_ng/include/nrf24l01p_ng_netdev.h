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
 * @brief   Netdev driver interface for NRF24L01+ (NG) device driver
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Struct that holds functions of NRF24L01+ device driver
 */
extern const netdev_driver_t nrf24l01p_ng_driver;

#ifdef __cplusplus
}
#endif

/** @} */
