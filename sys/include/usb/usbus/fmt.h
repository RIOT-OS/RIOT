/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    usb_usbus_fmt USBUS descriptor formatter functions
 * @ingroup     usb_usbus
 *
 * @{
 *
 * @file
 * @brief       USBUS descriptor formatter functions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <stdint.h>
#include <stdlib.h>
#include "usb/usbus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief generator for the USB configuration descriptor
 *
 * @param[in]   usbus   USBUS context
 *
 * @return      the generated descriptor size in bytes
 */
size_t usbus_fmt_descriptor_conf(usbus_t *usbus);

/**
 * @brief generator for the USB device descriptor
 *
 * @param[in]   usbus   USBUS context
 *
 * @return      the generated descriptor size in bytes
 */
size_t usbus_fmt_descriptor_dev(usbus_t *usbus);

#ifdef __cplusplus
}
#endif
/** @} */
