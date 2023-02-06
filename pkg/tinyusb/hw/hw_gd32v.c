/*
 * Copyright (C) 2023 Gunar Schorcht
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
 * @brief       tinyUSB hardware driver for GD32VF103 MCUs
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <errno.h>

#include "periph_conf.h"
#include "periph/gpio.h"
#include "pm_layered.h"

#include "tusb.h"
#include "device/usbd.h"
#include "host/usbh.h"

#define RCU_CFG0_SCS_PLL        (2UL << RCU_CFG0_SCS_Pos)   /* PLL used */

#define USB_OTG_GCCFG_PWRON     (1UL << 16) /* Power on */
#define USB_OTG_GCCFG_VBUSBCEN  (1UL << 19) /* VBUS B-device comparer enable */
#define USB_OTG_GCCFG_VBUSIG    (1UL << 21) /* VBUS detection ignore */

/* Although the following defines are done in `vendor/gd32vf103_rcu.h`, they
 * have to be defined here since `vendor/gd32vf103_rcu.h` can't be included due
 * to type conflicts with `vendor/gd32vf103_periph.h`. */
#define RCU_CKUSB_CKPLL_DIV1_5  (0UL << 22) /* USB clock is PLL clock/1.5 */
#define RCU_CKUSB_CKPLL_DIV1    (1UL << 22) /* USB clock is PLL clock */
#define RCU_CKUSB_CKPLL_DIV2_5  (2UL << 22) /* USB clock is PLL clock/2.5 */
#define RCU_CKUSB_CKPLL_DIV2    (3UL << 22) /* USB clock is PLL clock/2 */

static int tinyusb_hw_init_dev(const dwc2_usb_otg_fshs_config_t *conf)
{
    /* Block both DEEP_SLEEP and STANDBY, TODO DEEP_SLEEP is unblocked during
     * USB suspend status */
    pm_block(GD32V_PM_DEEPSLEEP);
    pm_block(GD32V_PM_STANDBY);

    /* ensure that PLL clock is used */
    assert((RCU->CFG0 & RCU_CFG0_SCS_Msk) == RCU_CFG0_SCS_PLL);
    /* ensure that the 48 MHz USB clock can be generated */
    assert((CLOCK_CORECLOCK == MHZ(48)) || (CLOCK_CORECLOCK == MHZ(72)) ||
           (CLOCK_CORECLOCK == MHZ(96)) || (CLOCK_CORECLOCK == MHZ(120)));

    /* divide the core clock to get 48 MHz USB clock */
    RCU->CFG0 &= ~RCU_CFG0_USBFSPSC_Msk;
    switch (CLOCK_CORECLOCK) {
    case MHZ(48):
        RCU->CFG0 |= RCU_CKUSB_CKPLL_DIV1;
        break;
    case MHZ(72):
        RCU->CFG0 |= RCU_CKUSB_CKPLL_DIV1_5;
        break;
    case MHZ(96):
        RCU->CFG0 |= RCU_CKUSB_CKPLL_DIV2;
        break;
    case MHZ(120):
        RCU->CFG0 |= RCU_CKUSB_CKPLL_DIV2_5;
        break;
    }

    /* enable USB OTG clock */
    periph_clk_en(conf->bus, conf->rcu_mask);

    /* reset the USB OTG peripheral */
    RCU->AHBRST |= RCU_AHBRST_USBFSRST_Msk;
    RCU->AHBRST &= ~RCU_AHBRST_USBFSRST_Msk;

    void isr_otg_fs(unsigned irq);
    clic_set_handler(conf->irqn, isr_otg_fs);
    clic_enable_interrupt(conf->irqn, CPU_DEFAULT_IRQ_PRIO);

    USB_OTG_GlobalTypeDef *global_regs =
                (USB_OTG_GlobalTypeDef *)(conf->periph + USB_OTG_GLOBAL_BASE);

    /* disable VBUS sensing */
    global_regs->GCCFG |= USB_OTG_GCCFG_PWRON |
                          USB_OTG_GCCFG_VBUSIG |
                          USB_OTG_GCCFG_VBUSBCEN;
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

void isr_otg_fs(unsigned irq)
{
    (void)irq;

    /* call device interrupt handler with the first device */
    if (IS_USED(MODULE_TINYUSB_DEVICE)) {
        tud_int_handler(TINYUSB_TUD_RHPORT);
    }

    /* call host interrupt handler with the first device */
    if (IS_USED(MODULE_TINYUSB_HOST)) {
        tuh_int_handler(TINYUSB_TUH_RHPORT);
    }
}
