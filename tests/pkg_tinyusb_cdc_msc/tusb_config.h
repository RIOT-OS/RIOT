/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * By default, the number of `CFG_TUD_*` device class and `CFG_TUH_*`
 * host class interfaces is defined to 1 if the corresponding `tinyusb_class_*`
 * and `tinyusb_device`/`tinyusb_host` module are enabled, and 0 otherwise.
 * That is, there is one interface of each class.
 *
 * For example, if the `tinyusb_device` and `tinyusb_class_cdc` modules are
 * enabled, `CFG_TUD_CDC` is defined to 1 by default. The number of all other
 * `CFG_TUD_*` device class interfaces are 0.
 *
 * To define a different number of device class or host class interfaces,
 * just define them here to override these default values, for example:
 * ```c
 * #define CFG_TUD_CDC 2
 * #define CFG_TUD_HID 3
 * ```
 */

/* Default configuration defined by RIOT package tinyUSB has to be included last */
#include "tinyusb_config.h"

#ifdef __cplusplus
}
#endif

#endif /* TUSB_CONFIG_H */
