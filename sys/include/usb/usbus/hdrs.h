/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    usb_usbus_hdrs USBUS header functions
 * @ingroup     usb_usbus
 *
 * @{
 *
 * @file
 * @brief       USBUS header formatter functions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef USB_USBUS_HDRS_H
#define USB_USBUS_HDRS_H

#include <stdint.h>
#include <stdlib.h>
#include "usb/usbus.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t usbus_hdrs_config_size(usbus_t *usbus);
size_t usbus_hdrs_fmt_hdrs(usbus_t *usbus);
size_t usbus_hdrs_fmt_additional(usbus_t *usbus, usbus_hdr_gen_t *hdr);
size_t usbus_hdrs_fmt_ifaces(usbus_t *usbus);
size_t usbus_hdrs_fmt_endpoints(usbus_t *usbus, usbus_endpoint_t *ep);
size_t usbus_hdrs_fmt_conf(usbus_t *usbus);

#ifdef __cplusplus
}
#endif
#endif /* USB_USBUS_HDRS_H */
/** @} */
