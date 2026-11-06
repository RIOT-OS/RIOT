/*
 * SPDX-FileCopyrightText: 2018 Robin Lösch
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 *
 * @file
 * @brief       Interface definition for the stm32 ethernet driver
 *
 * @author      Robin Lösch <robin@chilio.net>
 *
 * @{
 */

#pragma once

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Setup netdev
 *
 */
void stm32_eth_netdev_setup(netdev_t *netdev);

#ifdef __cplusplus
}
#endif

/** @} */
