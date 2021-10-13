/*
 * Copyright (C) 2019 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    drivers_gpio_linux Linux User Mode GPIO Driver
 * @ingroup     cpu_native
 * @brief       Implementation of GPIO access from Linux User Space
 *
 * This module allows to connect a RIOT application that runs on a Linux host to
 * the physical GPIO pins of that host. To do so, the application has to be
 * compiled for the native board in a Linux environment.
 *
 * GPIO support is automatically included if either a module requiring the
 * `periph_gpio` feature is added to the application or if it is explicitly
 * listed as `FEATURES_REQUIRED` in the application's Makefile.
 *
 * At runtime, the process has to be connected to a specific GPIO bank on the host
 * machine. GPIO banks are exposed as `/dev/gpiochipN` character files, where N
 * is the bank ID to which several GPIO pins are connected.
 *
 * Example:
 *
 * ```
 * $ ./riot_native_app --gpio=/dev/gpiochip0 --gpio=/dev/gpiochip1
 * ```
 *
 * This will add `/dev/gpiochip0` and `/dev/gpiochip1` as PORT(0) and PORT(1) in RIOT.
 * The first pin can be used with PIN(0,0) as `gpio_t`, the second one on the first
 * port would be PIN(0,1) and so on.
 *
 * Please refer to your board's documentation for the mapping of the pins.
 *
 * @{
 *
 * @file
 * @brief       Implementation of GPIO access from Linux User Space
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#ifndef GPIODEV_LINUX_H
#define GPIODEV_LINUX_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   register `/dev/gpiochip*` device to be used for GPIO
 *
 * @param[in] device   The gpiochip device to open.
 *
 * @return             0 on success, error otherwise
 */
int gpio_linux_setup(const char* device);

/**
 * @brief   shutdown GPIO subsystem
 *
 * This closes all GPIO fds.
 */
void gpio_linux_teardown(void);

#ifdef __cplusplus
}
#endif

#endif /* GPIODEV_LINUX_H */
/** @} */
