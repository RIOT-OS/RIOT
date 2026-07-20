/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
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
