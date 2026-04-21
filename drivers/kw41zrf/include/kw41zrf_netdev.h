/*
 * SPDX-FileCopyrightText: 2017 SKF AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_kw41zrf
 * @{
 *
 * @file
 * @brief       Netdev interface for kw41zrf driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t kw41zrf_driver;

#ifdef __cplusplus
}
#endif

/** @} */
