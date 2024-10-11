/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_tinyusb
 * @{
 *
 * @file
 * @brief       TinyUSB hardware driver API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef TINYUSB_HW_H
#define TINYUSB_HW_H

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
#endif /* TINYUSB_HW_H */
