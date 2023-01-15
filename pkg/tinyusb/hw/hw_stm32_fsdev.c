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
 * @brief       tinyUSB hardware driver for STM32 MCUs with USB FS device port
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <errno.h>

#include "periph_conf.h"
#include "periph/gpio.h"
#include "pm_layered.h"

#include "tusb.h"
#include "device/usbd.h"
#include "host/usbh.h"

static int tinyusb_hw_init_dev(const stm32_usbdev_fs_config_t *conf)
{
    /* Block both STOP and STANDBY, TODO STOP is unblocked during USB suspend
     * status */
    pm_block(STM32_PM_STOP);
    pm_block(STM32_PM_STANDBY);

    /* Enable the clock to the peripheral */
    periph_clk_en(conf->apb, conf->rcc_mask);

#if defined(PWR_CR2_USV)
    /* Validate USB Supply */
    PWR->CR2 |= PWR_CR2_USV;
#endif

#if defined(RCC_APB1SMENR_USBSMEN)
    RCC->APB1SMENR |= RCC_APB1SMENR_USBSMEN;
#elif defined(RCC_APB1SMENR1_USBSMEN)
    RCC->APB1SMENR1 |= RCC_APB1SMENR1_USBSMEN;
#endif

#if defined(CRS_CR_AUTOTRIMEN) && defined(CRS_CR_CEN)
    /* Enable CRS with auto trim enabled */
    CRS->CR |=  (CRS_CR_AUTOTRIMEN | CRS_CR_CEN);
#endif

    if (conf->af == GPIO_AF_UNDEF && conf->disconn == GPIO_UNDEF) {
        /* If the MCU does not have an internal D+ pullup and there is no
         * dedicated GPIO on the board to simulate a USB disconnect, the D+ GPIO
         * is temporarily configured as an output and pushed down to simulate
         * a disconnect/connect cycle to allow the host to recognize the device.
         * This requires an external pullup on D+ signal to work. */
        gpio_init(conf->dp, GPIO_OUT);
        gpio_clear(conf->dp);
        /* wait about a ms */
        ztimer_sleep(ZTIMER_MSEC, 1);
        gpio_init(conf->dp, GPIO_IN);
    }

    if (conf->af != GPIO_AF_UNDEF) {
        /* Configure AF for the pins */
        gpio_init_af(conf->dp, conf->af);
        gpio_init_af(conf->dm, conf->af);
    }

    if (conf->disconn != GPIO_UNDEF) {
        /* In case the MCU has no internal D+ pullup, a GPIO is used to
         * connect/disconnect from USB bus */
        gpio_init(conf->disconn, GPIO_OUT);
        gpio_set(conf->disconn);
    }

    return 0;
}

int tinyusb_hw_init(void)
{
    for (unsigned i = 0; i < USBDEV_NUMOF; i++) {
        if (tinyusb_hw_init_dev(&stm32_usbdev_fs_config[i]) != 0) {
            return -ENODEV;
        }
    }
    return 0;
}

void USBDEV_ISR(void)
{
    /* call device interrupt handler with the last device */
    if (IS_USED(MODULE_TINYUSB_DEVICE)) {
        tud_int_handler(TINYUSB_TUD_RHPORT);
    }

    cortexm_isr_end();
}
