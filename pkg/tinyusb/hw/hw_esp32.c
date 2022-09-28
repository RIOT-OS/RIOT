/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_tinyusb
 * @brief
 * @{
 *
 * @brief       tinyUSB hardware driver for ESP32x SoCs
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <errno.h>

#include "esp_err.h"
#include "esp_private/usb_phy.h"

#include "log.h"

static usb_phy_handle_t phy_hdl;

int tinyusb_hw_init(void)
{
    usb_phy_config_t phy_conf = {
        .controller = USB_PHY_CTRL_OTG,
        .otg_mode = USB_OTG_MODE_DEVICE,
        .target = USB_PHY_TARGET_INT,       /* only internal PHY supported */
    };

    if (usb_new_phy(&phy_conf, &phy_hdl) != ESP_OK) {
        LOG_TAG_ERROR("usb", "Install USB PHY failed");
        return -ENODEV;
    }

    return 0;
}
