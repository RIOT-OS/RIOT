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
 * @brief       tinyUSB hardware driver for nRF52 MCUs
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <errno.h>

#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/pm.h"

#include "tusb.h"
#include "device/usbd.h"
#include "host/usbh.h"

/*
 * Definition of events as used by the tinyusb/portable/nordic/nrf5x/dcd_nrf5x.c
 * https://github.com/hathach/tinyusb/blob/0.14.0/src/portable/nordic/nrf5x/dcd_nrf5x.c#L954
 */
#define USB_EVT_DETECTED    0
#define USB_EVT_REMOVED     1
#define USB_EVT_READY       2

int tinyusb_hw_init(void)
{
    if (IS_USED(MODULE_TINYUSB_DEVICE)) {
        NVIC_SetPriority(USBD_IRQn, 2);
    }

    extern void tusb_hal_nrf_power_event(uint32_t event);

    if ( NRF_POWER->USBREGSTATUS & POWER_USBREGSTATUS_VBUSDETECT_Msk ) {
        tusb_hal_nrf_power_event(USB_EVT_DETECTED);
    }

    /* it requires some time to activate the clock */
    ztimer_sleep(ZTIMER_MSEC, 1);

    if ( NRF_POWER->USBREGSTATUS & POWER_USBREGSTATUS_OUTPUTRDY_Msk  ) {
        tusb_hal_nrf_power_event(USB_EVT_READY);
    }

    return 0;
}

void isr_usbd(void)
{
    /* call device interrupt handler with the first device */
    if (IS_USED(MODULE_TINYUSB_DEVICE)) {
        tud_int_handler(0);
    }

    /* call host interrupt handler with the first device */
    if (IS_USED(MODULE_TINYUSB_HOST)) {
        tuh_int_handler(0);
    }

    cortexm_isr_end();
}
