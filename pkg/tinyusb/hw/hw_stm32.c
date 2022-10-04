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
 * @brief       tinyUSB hardware driver for STM32 MCUs
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <errno.h>

#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/pm.h"

#include "tusb.h"
#include "device/usbd.h"
#include "host/usbh.h"

static int tinyusb_hw_init_dev(const dwc2_usb_otg_fshs_config_t *conf)
{
    /* Block both STOP and STANDBY, TODO STOP is unblocked during USB suspend
     * status */
    pm_block(STM32_PM_STOP);
    pm_block(STM32_PM_STANDBY);

    /* Enable the clock to the peripheral */
    periph_clk_en(conf->ahb, conf->rcc_mask);

    /* Enables clock on the GPIO bus */
    gpio_init(conf->dp, GPIO_IN);
    gpio_init(conf->dm, GPIO_IN);
    /* Configure AF for the pins */
    gpio_init_af(conf->dp, conf->af);
    gpio_init_af(conf->dm, conf->af);

#if 0 /* TODO we don't use USB ID pin for now */
    gpio_init(conf->id, GPIO_IN);
#endif

#ifdef USB_OTG_GCCFG_NOVBUSSENS
#if 0 /* TODO V_USB sensing pin */
    gpio_init(conf->vbus, GPIO_OD_PU);
    gpio_init_af(conf->vbus, conf->af);

    USB_OTG_GlobalTypeDef *global_regs =
         (USB_OTG_GlobalTypeDef *)(conf->periph + USB_OTG_GLOBAL_BASE);

    global_regs->GCCFG &= ~USB_OTG_GCCFG_NOVBUSSENS;
    global_regs->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
#else
    /* Enable no Vbus sensing and enable `Power Down Disable` */
    USB_OTG_GlobalTypeDef *global_regs =
                (USB_OTG_GlobalTypeDef *)(conf->periph + USB_OTG_GLOBAL_BASE);

    /* Enable no Vbus Detect enable and enable `Power Down Disable` */
    global_regs->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS | USB_OTG_GCCFG_PWRDWN;
#endif
#endif /* USB_OTG_GCCFG_NOVBUSSENS */

#ifdef DWC2_USB_OTG_HS_ENABLED
    if (conf->type == DWC2_USB_OTG_HS) {
        /* Disable the ULPI clock in low power mode, this is essential for the
         * peripheral when using the built-in phy */
        periph_lpclk_dis(conf->ahb, RCC_AHB1LPENR_OTGHSULPILPEN);
        /* Only the built-in phy supported for now */
        assert(conf->phy == DWC2_USB_OTG_PHY_BUILTIN);
        global_regs->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
    }
#endif

    return 0;
}

int tinyusb_hw_init(void)
{
    for (unsigned i = 0; i < USBDEV_NUMOF; i++) {
        if (tinyusb_hw_init_dev(&dwc2_usb_otg_fshs_config[i]) != 0) {
            return -ENODEV;
        }
    }
    return 0;
}

/*
 * If the STM32 MCU has 2 controllers, tinyUSB supposes that device 0 is
 * a FS device and device 1 is a HS device
 */
#ifdef DWC2_USB_OTG_FS_ENABLED
void isr_otg_fs(void)
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
#endif /* DWC2_USB_OTG_FS_ENABLED */

#ifdef DWC2_USB_OTG_HS_ENABLED
void isr_otg_hs(void)
{
    /* call device interrupt handler with the last device */
    if (IS_USED(MODULE_TINYUSB_DEVICE)) {
        tud_int_handler(TINYUSB_TUD_RHPORT);
    }

    /* call host interrupt handler with the last device */
    if (IS_USED(MODULE_TINYUSB_HOST)) {
        tuh_int_handler(TINYUSB_TUH_RHPORT);
    }

    cortexm_isr_end();
}
#endif /* DWC2_USB_OTG_HS_ENABLED */
