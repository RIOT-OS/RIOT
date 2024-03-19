/*
 * Copyright (C) 2023 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Trigger reset to the bootloader stored in the internal boot ROM
 *              memory.
 *
 *              This will start the USB/UART bootloader.
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "driver/periph_ctrl.h"
#include "rom/usb/chip_usb_dw_wrapper.h"
#include "rom/usb/usb_persist.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/soc_caps.h"
#if SOC_USB_SERIAL_JTAG_SUPPORTED
#include "soc/usb_serial_jtag_reg.h"
#include "soc/usb_serial_jtag_struct.h"
#endif

#include "periph/pm.h"

static void _reconfigure_usb(void)
{
#ifdef CPU_FAM_ESP32S3
    /* disable USB OTG controller */
    periph_module_reset(PERIPH_USB_MODULE);
    periph_module_disable(PERIPH_USB_MODULE);

    /* allow USB Serial/JTAG to use the internal USB transceiver */
    RTCCNTL.usb_conf.sw_usb_phy_sel = 0;
    /* control the internal USB transceiver selection via hardware (efuse) */
    RTCCNTL.usb_conf.sw_hw_usb_phy_sel = 0;
    /* don't enable USB transceiver function */
    RTCCNTL.usb_conf.usb_pad_enable = 0;

    /* select internal PHY for USB Serial/JTAG */
    USB_SERIAL_JTAG.conf0.phy_sel = 0;
    /* enable USB pad function */
    USB_SERIAL_JTAG.conf0.usb_pad_enable = 1;
#endif
}

void __attribute__((weak)) usb_board_reset_in_bootloader(void)
{
    /* If we are here, the USB port is not connected to the Serial/JTAG interface.
       We have to re-configure it back to this mode first. */
    _reconfigure_usb();

    chip_usb_set_persist_flags(USBDC_PERSIST_ENA);
    REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
    pm_reboot();
}
