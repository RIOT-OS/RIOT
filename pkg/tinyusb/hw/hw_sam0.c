/*
 * Copyright (C) 2022 Mesotic SAS
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
 * @brief       tinyUSB hardware driver for SAM0 MCUs
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include <errno.h>

#include "periph_conf.h"
#include "periph/gpio.h"
#include "pm_layered.h"
#include "log.h"
#include "tusb.h"
#include "device/usbd.h"
#include "host/usbh.h"

int tinyusb_hw_init(void)
{
    /* Initialize GPIO pins */
    gpio_init(sam_usbdev_config[0].dp, GPIO_IN);
    gpio_init(sam_usbdev_config[0].dm, GPIO_IN);
    gpio_init_mux(sam_usbdev_config[0].dm, sam_usbdev_config[0].d_mux);
    gpio_init_mux(sam_usbdev_config[0].dp, sam_usbdev_config[0].d_mux);

    /* Initialize clocks */
    sam0_gclk_enable(sam_usbdev_config[0].gclk_src);

#if defined(MCLK)
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_USB;
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_USB;
#else
    PM->AHBMASK.reg |= PM_AHBMASK_USB;
    PM->APBBMASK.reg |= PM_APBBMASK_USB;
#endif

#if defined(CPU_COMMON_SAMD21)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN
                        | GCLK_CLKCTRL_GEN(sam_usbdev_config[0].gclk_src)
                        | GCLK_CLKCTRL_ID(USB_GCLK_ID);
    pm_block(SAMD21_PM_IDLE_1);
#else
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                                   | GCLK_PCHCTRL_GEN(sam_usbdev_config[0].gclk_src);
#endif

    return 0;
}

void isr_usb(void)
{
    /* call device interrupt handler with the first device */
    if (IS_USED(MODULE_TINYUSB_DEVICE)) {
        tud_int_handler(TINYUSB_TUD_RHPORT);
    }

    /* call host interrupt handler with the first device */
    if (IS_USED(MODULE_TINYUSB_HOST)) {
        tuh_int_handler(TINYUSB_TUH_RHPORT);
    }

    cortexm_isr_end();
}

void isr_usb0(void)
{
    isr_usb();
}

void isr_usb1(void)
{
    isr_usb();
}
void isr_usb2(void)
{
    isr_usb();
}
void isr_usb3(void)
{
    isr_usb();
}
