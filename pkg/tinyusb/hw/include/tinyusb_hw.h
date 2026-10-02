/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_tinyusb
 * @{
 *
 * @file
 * @brief       TinyUSB hardware driver API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the peripherals for tinyUSB.
 *
 * This functions is called by #tinyusb_setup to initialize the peripherals.
 *
 * @return  0 on success
 * @return  -ENODEV if peripherals couldn't be initialized
 */
int tinyusb_hw_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
