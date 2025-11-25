/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
