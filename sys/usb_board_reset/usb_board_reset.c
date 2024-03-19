/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys
 * @{
 * @file
 *
 * @author  Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include "log.h"

#include "usb/usbus/cdc/acm.h"
#include "usb_board_reset.h"

#include "periph/pm.h"

#ifndef RESET_IN_BOOTLOADER_TRIGGER_BAUDRATE
#define RESET_IN_BOOTLOADER_TRIGGER_BAUDRATE    (1200U)
#endif

#ifndef RESET_IN_APPLICATION_TRIGGER_BAUDRATE
#define RESET_IN_APPLICATION_TRIGGER_BAUDRATE   (600U)
#endif

int usb_board_reset_coding_cb(usbus_cdcacm_device_t *cdcacm,
                              uint32_t baud, uint8_t bits,
                              uint8_t parity, uint8_t stop)
{
    /* Note: This function is also used by tinyUSB if module `usb_board_reset`
     * is used. In this case no USBUS CDC ACM device is defined and parameter
     * `cdcacm` is NULL. Please keep this in mind if `cdcacm` is to be used
     * in future versions. */
    (void)cdcacm;

    (void)bits;
    (void)parity;
    (void)stop;
    switch (baud) {
        case RESET_IN_BOOTLOADER_TRIGGER_BAUDRATE:
            LOG_DEBUG("[cdc-acm] reset in bootloader\n");
            usb_board_reset_in_bootloader();
            break;
        case RESET_IN_APPLICATION_TRIGGER_BAUDRATE:
            LOG_DEBUG("[cdc-acm] reset in application\n");
            usb_board_reset_in_application();
            break;
        default:
            (void)baud;
            break;
    }

    return 0;
}

#ifndef MODULE_RIOTBOOT_RESET
/*
 * Definition of a function as weak symbol for reset in bootloader which
 * prints an error message if no real implementation is compiled in and
 * the `riotboot_reset` module is not used. This is required if the module
 * `usb_board_reset` is used to restart the board with an application via
 * an USB CDC ACM interface, but the board's bootloader does not support
 * a reset in the bootloader.
 */
__attribute__((weak))
void usb_board_reset_in_bootloader(void)
{
    LOG_ERROR("[cdc-acm] reset in bootloader is not supported\n");
}
#endif

void usb_board_reset_in_application(void)
{
    pm_reboot();
    while (1) {}
}
