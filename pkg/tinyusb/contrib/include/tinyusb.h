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
 * @brief       TinyUSB API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef TINYUSB_H
#define TINYUSB_H

#include "periph_conf.h"
#include "tinyusb_hw_defaults.h"

#ifndef TINYUSB_THREAD_STACKSIZE_MAIN
/** Stack size used for the tinyUSB thread */
#define TINYUSB_THREAD_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)
#endif

#ifndef TINYUSB_PRIORITY
/** Priority used for the tinyUSB thread */
#define TINYUSB_PRIORITY                (2)
#endif

#ifndef TINYUSB_TUD_RHPORT
/** tinyUSB RHPort number used for the device stack, default value is 0 */
#define TINYUSB_TUD_RHPORT              0
#endif

#ifndef TINYUSB_TUH_RHPORT
/** tinyUSB RHPort number used for the host stack, defaults value is 0 */
#define TINYUSB_TUH_RHPORT              0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if MODULE_TINYUSB_CLASS_NET_RNDIS
#define SIZEOF_ETH_HDR 14
#endif

/**
 * @brief Initialize the tinyUSB stack including used peripherals and start the tinyUSB thread
 *
 * @return  0 on success
 * @return  -ENODEV if peripherals couldn't be initialized
 * @return  -EINVAL or -EOVERFLOW if the thread couldn't be created
 */
int tinyusb_setup(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TINYUSB_H */
