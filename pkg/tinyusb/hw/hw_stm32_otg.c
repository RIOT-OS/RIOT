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
 * @brief       tinyUSB hardware driver for STM32 MCUs with USB OTG FS/HS ports
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <errno.h>

#include "periph_conf.h"
#include "periph/gpio.h"
#include "pm_layered.h"

#include "tusb.h"
#include "device/usbd.h"
#include "host/usbh.h"

static int tinyusb_hw_init_dev(const dwc2_usb_otg_fshs_config_t *conf)
{
    /* Block both STOP and STANDBY, TODO STOP is unblocked during USB suspend
     * status */
    pm_block(STM32_PM_STOP);
    pm_block(STM32_PM_STANDBY);

#if defined(PWR_CR2_USV)
    /* on L4: Validate USB Supply */
    PWR->CR2 |= PWR_CR2_USV;
#endif /* PWR_CR2_USV */

#if defined(PWR_SVMCR_USV)
    /* on U5: Validate USB Supply */
    PWR->SVMCR |= PWR_SVMCR_USV;
#endif /* PWR_SVMCR_USV */

    /* Enable the clock to the peripheral */
    periph_clk_en(conf->ahb, conf->rcc_mask);

#ifndef MODULE_PERIPH_USBDEV_HS_ULPI
    /* Enables clock on the GPIO bus */
    gpio_init(conf->dp, GPIO_IN);
    gpio_init(conf->dm, GPIO_IN);
    /* Configure AF for the pins */
    gpio_init_af(conf->dp, conf->af);
    gpio_init_af(conf->dm, conf->af);
#endif /* MODULE_PERIPH_USBDEV_HS_ULPI */

    USB_OTG_GlobalTypeDef *global_regs =
                (USB_OTG_GlobalTypeDef *)(conf->periph + USB_OTG_GLOBAL_BASE);

#ifdef USB_OTG_GCCFG_NOVBUSSENS
    /* set No Vbus Sensing */
    global_regs->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
#elif USB_OTG_GCCFG_VBDEN
    /* clear Vbus Detect Enable */
    global_regs->GCCFG &= ~USB_OTG_GCCFG_VBDEN;
#endif

#ifdef DWC2_USB_OTG_HS_ENABLED
    if (conf->type == DWC2_USB_OTG_HS) {
        if (conf->phy == DWC2_USB_OTG_PHY_BUILTIN) {
            /* set `Power Down Disable` to activate the on-chip FS transceiver */
            global_regs->GCCFG |= USB_OTG_GCCFG_PWRDWN;
            /* Disable the ULPI clock in low power mode, this is essential for the
             * peripheral when using the built-in PHY */
            periph_lpclk_dis(conf->ahb, RCC_AHB1LPENR_OTGHSULPILPEN);
            /* select on-chip builtin PHY */
            global_regs->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
        }
#ifdef MODULE_PERIPH_USBDEV_HS_ULPI
        else if (conf->phy == DWC2_USB_OTG_PHY_ULPI) {
            /* initialize ULPI interface */
            gpio_init(conf->ulpi_clk, GPIO_IN);
            gpio_init(conf->ulpi_d0, GPIO_IN);
            gpio_init(conf->ulpi_d1, GPIO_IN);
            gpio_init(conf->ulpi_d2, GPIO_IN);
            gpio_init(conf->ulpi_d3, GPIO_IN);
            gpio_init(conf->ulpi_d4, GPIO_IN);
            gpio_init(conf->ulpi_d5, GPIO_IN);
            gpio_init(conf->ulpi_d6, GPIO_IN);
            gpio_init(conf->ulpi_d7, GPIO_IN);
            gpio_init(conf->ulpi_stp, GPIO_IN);
            gpio_init(conf->ulpi_dir, GPIO_IN);
            gpio_init(conf->ulpi_nxt, GPIO_IN);
            gpio_init_af(conf->ulpi_clk, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d0, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d1, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d2, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d3, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d4, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d5, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d6, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d7, conf->ulpi_af);
            gpio_init_af(conf->ulpi_stp, conf->ulpi_af);
            gpio_init_af(conf->ulpi_dir, conf->ulpi_af);
            gpio_init_af(conf->ulpi_nxt, conf->ulpi_af);

            /* enable ULPI clock */
            periph_clk_en(conf->ahb, RCC_AHB1ENR_OTGHSULPIEN);

#ifdef USB_OTG_GUSBCFG_ULPI_UTMI_SEL
            /* select ULPI PHY */
            global_regs->GUSBCFG |= USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
#endif
#ifdef USB_OTG_GUSBCFG_PHYIF
            /* use the 8-bit interface */
            global_regs->GUSBCFG &= ~USB_OTG_GUSBCFG_PHYIF;
#endif /* USB_OTG_GUSBCFG_PHYIF */
#ifdef USB_OTG_GUSBCFG_DDRSEL
            /* use single data rate */
            global_regs->GUSBCFG &= ~USB_OTG_GUSBCFG_DDRSEL;
#endif /* USB_OTG_GUSBCFG_DDRSEL */

            /* disable the on-chip FS transceiver */
            global_regs->GUSBCFG &= ~USB_OTG_GUSBCFG_PHYSEL;

            /* use internal V_BUS valid indicator and internal charge pump */
            global_regs->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD |
                                                       USB_OTG_GUSBCFG_ULPIEVBUSI);
            /* disable ULPI FS/LS serial interface */
            global_regs->GUSBCFG &= ~USB_OTG_GUSBCFG_ULPIFSLS;
        }

#elif defined(MODULE_PERIPH_USBDEV_HS_UTMI)
        else if (conf->phy == DWC2_USB_OTG_PHY_UTMI) {
            /* enable ULPI clock */
            periph_clk_en(conf->ahb, RCC_AHB1ENR_OTGHSULPIEN);
            /* enable UTMI HS PHY Controller clock */
            periph_clk_en(APB2, RCC_APB2ENR_OTGPHYCEN);

#ifdef USB_OTG_GUSBCFG_ULPI_UTMI_SEL
            /* select UTMI+ PHY */
            global_regs->GUSBCFG &= ~USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
#endif /* USB_OTG_GUSBCFG_ULPI_UTMI_SEL */
#ifdef USB_OTG_GUSBCFG_PHYIF
            /* use the 8-bit interface and single data rate */
            global_regs->GUSBCFG &= ~USB_OTG_GUSBCFG_PHYIF;
#endif /* USB_OTG_GUSBCFG_PHYIF */

            /* disable the on-chip FS transceiver */
            global_regs->GUSBCFG &= ~USB_OTG_GUSBCFG_PHYSEL;

            /* configure the USB HS PHY Controller (USB_HS_PHYC),
             * USB_HS_PHYC and GCCFG are STM32 specific */
#ifdef USB_HS_PHYC
            /* enable USB HS PHY Controller */
            global_regs->GCCFG |= USB_OTG_GCCFG_PHYHSEN;

            /* determine the PLL input clock of the USB HS PHY from HSE clock */
            switch (CONFIG_CLOCK_HSE) {
                case 12000000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_12MHZ;
                    break;
                case 12500000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_12_5MHZ;
                    break;
                case 16000000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_16MHZ;
                    break;
                case 24000000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_24MHZ;
                    break;
                case 25000000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_25MHZ;
                    break;
                default:
                    assert(0);
            }

            /* configure the tuning interface of the USB HS PHY */
            USB_HS_PHYC->USB_HS_PHYC_TUNE |= conf->phy_tune;

            /* check whether the LDO regulator is used by on the chip */
            if (USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_USED) {
                /* enable the LDO */
                USB_HS_PHYC->USB_HS_PHYC_LDO |= USB_HS_PHYC_LDO_ENABLE;
                /* wait until the LDO is ready */
                while (!(USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_STATUS)) {}
            }

            /* enable the PLL of the USB HS PHY */
            USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL_PLLEN;
#endif /* USB_HS_PHYC */
        }

#else /* MODULE_PERIPH_USBDEV_HS_ULPI */
        else {
            /* only on-chip PHY support enabled */
            assert(conf->phy == DWC2_USB_OTG_PHY_BUILTIN);
        }
#endif /* MODULE_PERIPH_USBDEV_HS_ULPI */
    }
#endif /* DWC2_USB_OTG_HS_ENABLED */

    if (conf->phy == DWC2_USB_OTG_PHY_BUILTIN) {
        /* set `Power Down Disable` to activate the on-chip FS transceiver */
        global_regs->GCCFG |= USB_OTG_GCCFG_PWRDWN;
    }
    else {
        /* clear `Power Down Disable` to deactivate the on-chip FS transceiver */
        global_regs->GCCFG &= ~USB_OTG_GCCFG_PWRDWN;
    }

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
