/*
 * Copyright (C) 2019 Ha Thach (tinyusb.org)
 *               2022 Gunar Schorcht
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include "fmt.h"
#include "luid.h"
#include "tusb.h"
#include "usb.h"

#include "net/ethernet.h"

#ifdef MODULE_TINYUSB_DFU
#include "riotboot/usb_dfu.h"
#endif

#include "tinyusb_descriptors.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/* don't compile this part if CONFIG_TUSBD_USE_CUSTOM_DESC is set */
#if !defined(CONFIG_TUSBD_USE_CUSTOM_DESC)

#if (MODULE_TINYUSB_CLASS_AUDIO || \
     MODULE_TINYUSB_CLASS_BTH || \
     MODULE_TINYUSB_CLASS_MIDI || \
     MODULE_TINYUSB_CLASS_USBTMC || \
     MODULE_TINYUSB_CLASS_VIDEO || \
     (CONFIG_TUSBD_CDC_NUMOF > 2) || \
     (CONFIG_TUSBD_DFU_NUMOF > 1) || \
     (CONFIG_TUSBD_DFU_RT_NUMOF > 1) || \
     (CONFIG_TUSBD_NET_NUMOF > 1) || \
     (CONFIG_TUSBD_HID_NUMOF > 2) || \
     (CONFIG_TUSBD_MSC_NUMOF > 1) || \
     (CONFIG_TUSBD_VENDOR_NUMOF > 1))
#error Using generic descriptors is not possible for the selected combination \
       of device class interfaces. Custom descriptors have to be implemented.
#endif

/* If CDC ECM and RNDIS are used simultaneously, an alternative configuration
 * descriptor is required. */
#if CONFIG_TUSBD_NET_CDC_ECM && CONFIG_TUSBD_NET_RNDIS
#define _TUD_CONFIG_DESC_NUMOF  2
#else
#define _TUD_CONFIG_DESC_NUMOF  1
#endif

enum {
    _TUD_CONFIG_DESC_ID = 0,
#if _TUD_CONFIG_DESC_NUMOF == 2
    _TUD_CONFIG_DESC_ALT_ID = 1,
#endif
};

/*
 * --------------------------------------------------------------------+
 * Device Descriptors
 * --------------------------------------------------------------------+
 */
__attribute__((weak))
tusb_desc_device_t const tusb_desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = CONFIG_USB_PRODUCT_BCDVERSION,
#if CONFIG_TUSBD_CDC_NUMOF
    /* Use Interface Association Descriptor (IAD) for CDC
     * As required by USB Specs IAD's subclass must be common class (2)
     * and protocol must be IAD (1) */
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
#else
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
#endif
    .bMaxPacketSize0    = CONFIG_TUSBD_EP0_SIZE,

    .idVendor           = CONFIG_USB_VID,
    .idProduct          = CONFIG_USB_PID,
    .bcdDevice          = 0x100,

    .iManufacturer      = TUSBD_STR_IDX_MANUFACTURER,
    .iProduct           = TUSBD_STR_IDX_PRODUCT,
    .iSerialNumber      = TUSBD_STR_IDX_SERIAL,

    .bNumConfigurations = _TUD_CONFIG_DESC_NUMOF
};

/*
 * The function is invoked when GET DEVICE DESCRIPTOR is received.
 * It returns a pointer to the device descriptor.
 */
__attribute__((weak))
uint8_t const *tud_descriptor_device_cb(void)
{
    return (uint8_t const *)&tusb_desc_device;
}

/*
 * --------------------------------------------------------------------+
 * HID Report Descriptor
 * --------------------------------------------------------------------+
 */

#if CONFIG_TUSBD_HID_NUMOF > 0
__attribute__((weak))
uint8_t const tusb_desc_hid_0_report[] =
{
    TUD_HID_REPORT_DESC_GENERIC_INOUT(CONFIG_TUSBD_HID_EP_SIZE),
};
#endif

#if CONFIG_TUSBD_HID_NUMOF > 1
__attribute__((weak))
uint8_t const tusb_desc_hid_1_report[] =
{
    TUD_HID_REPORT_DESC_GENERIC_INOUT(CONFIG_TUSBD_HID_EP_SIZE),
};
#endif

#if CONFIG_TUSBD_HID_NUMOF
/* The function is invoked when GET HID REPORT DESCRIPTOR is received.
 * It returns a pointer to the HID report descriptor whose contents
 * must exist long enough for transfer to complete. */
__attribute__((weak))
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf)
{
#if CONFIG_TUSBD_HID_NUMOF > 0
  if (itf == 0)
  {
    return tusb_desc_hid_0_report;
  }
#endif
#if CONFIG_TUSBD_HID_NUMOF > 1
  else if (itf == 1)
  {
    return tusb_desc_hid_1_report;
  }
#endif
  return NULL;
}

/* The function is invoked when GET_REPORT control request is received.
 * It fills the report buffer content and returns its length. Returning
 * zero will causes the stack to send a STALL request.
 *
 * @note: The function is only a dummy function and therefore defined as a weak
 *        symbol that can be overwritten by real functions if needed by the
 *        application. */
__attribute__((weak))
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t* buffer, uint16_t reqlen)
{
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

/* The function is invoked when SET_REPORT control request is received or
 * data are received on OUT endpoint (Report ID = 0, Type = 0).
 *
 * @note: The function is only a dummy function and therefore defined as a weak
 *        symbol that can be overwritten by real functions if needed by the
 *        application. */
__attribute__((weak))
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer, uint16_t bufsize)
{
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) bufsize;
}
#endif /* CONFIG_TUSBD_HID_NUMOF */

/*
 *--------------------------------------------------------------------+
 * Configuration Descriptors
 *--------------------------------------------------------------------+
 */

#if CONFIG_USB_SELF_POWERED && CONFIG_USB_REM_WAKEUP
#define DESC_DEV_ATTR   (USB_CONF_ATTR_SELF_POWERED || USB_CONF_ATTR_REM_WAKEUP)
#elif CONFIG_USB_SELF_POWERED
#define DESC_DEV_ATTR   (USB_CONF_ATTR_SELF_POWERED)
#elif CONFIG_USB_REM_WAKEUP
#define DESC_DEV_ATTR   (USB_CONF_ATTR_REM_WAKEUP)
#else
#define DESC_DEV_ATTR   (0)
#endif

#define _tusb_speed_fs   0
#define _tusb_speed_hs   1

#define _TUD_CONFIG_DESC(id, len) \
    /* Config number, interface count, string index, total length, attribute,
     * power in mA */ \
    TUD_CONFIG_DESCRIPTOR(id + 1, TUSBD_ITF_NUMOF, 0, len, \
                          DESC_DEV_ATTR, CONFIG_USB_MAX_POWER)

#define _TUD_CDC_DESC(speed, n) \
    /* Interface number, string index, EP notification address and size,
     * EP Data Out & In, EP size. */ \
    TUD_CDC_DESCRIPTOR(TUSBD_ITF_CDC_##n, TUSBD_STR_IDX_CDC_##n, \
                       TUSBD_EP_CDC_##n##_NOTIF, CONFIG_TUSBD_CDC_NOTIF_EP_SIZE, \
                       TUSBD_EP_CDC_##n##_OUT, TUSBD_EP_CDC_##n##_IN, \
                       speed ? CONFIG_TUSBD_CDC_HS_EP_SIZE \
                             : CONFIG_TUSBD_CDC_FS_EP_SIZE)

#define _TUD_DFU_DESC(speed) \
    /* Interface number, alternate count, starting string index, attributes,
     * detach timeout, transfer size */ \
    TUD_DFU_DESCRIPTOR(TUSBD_ITF_DFU, CONFIG_TUSBD_DFU_ALT_NUMOF, \
                       TUSBD_STR_IDX_DFU_SLOT_0, CONFIG_TUSBD_DFU_ATTR, \
                       CONFIG_TUSBD_DFU_DETACH_TIMEOUT, \
                       speed ? CONFIG_TUSBD_DFU_HS_XFER_SIZE \
                             : CONFIG_TUSBD_DFU_FS_XFER_SIZE)

#define _TUD_DFU_RT_DESC(speed) \
    /* Interface number, alternate count, starting string index, attributes,
     * detach timeout, transfer size */ \
    TUD_DFU_RT_DESCRIPTOR(TUSBD_ITF_DFU_RT, \
                          TUSBD_STR_IDX_DFU_RT, DFU_ATTR_WILL_DETACH, \
                          CONFIG_TUSBD_DFU_RT_DETACH_TIMEOUT, \
                          speed ? CONFIG_TUSBD_DFU_RT_HS_XFER_SIZE \
                                : CONFIG_TUSBD_DFU_RT_FS_XFER_SIZE)

#define _TUD_HID_INOUT_DESC(speed, n) \
    /* Interface number, string index, protocol, report descriptor len,
     * EP Out & EP In address, EP size, polling interval */ \
    TUD_HID_INOUT_DESCRIPTOR(TUSBD_ITF_HID_##n, TUSBD_STR_IDX_HID_##n, \
                             HID_ITF_PROTOCOL_NONE, \
                             sizeof(tusb_desc_hid_##n##_report), \
                             TUSBD_EP_HID_##n##_OUT, TUSBD_EP_HID_##n##_IN, \
                             CONFIG_TUSBD_HID_EP_SIZE, \
                             CONFIG_TUSBD_HID_##n##_POLL_INTERVALL)

#define _TUD_MSC_DESC(speed) \
    /* Interface number, string index, EP Out & In address, EP size */ \
    TUD_MSC_DESCRIPTOR(TUSBD_ITF_MSC, TUSBD_STR_IDX_MSC, \
                       TUSBD_EP_MSC_OUT, TUSBD_EP_MSC_IN, \
                       speed ? CONFIG_TUSBD_MSC_HS_EP_SIZE \
                             : CONFIG_TUSBD_MSC_FS_EP_SIZE)

#define _TUD_CDC_ECM_DESC(speed) \
    /* Interface number, Description string index, MAC address string index,
     * EP notification address and size, EP Data Out & In, EP size, MTU size. */ \
    TUD_CDC_ECM_DESCRIPTOR(TUSBD_ITF_NET, \
                           TUSBD_STR_IDX_NET_CDC_ECM, TUSBD_STR_IDX_NET_MAC, \
                           TUSBD_EP_NET_NOTIF, \
                           CONFIG_TUSBD_NET_NOTIF_EP_SIZE, \
                           TUSBD_EP_NET_OUT, TUSBD_EP_NET_IN, \
                           speed ? CONFIG_TUSBD_NET_HS_EP_SIZE \
                                 : CONFIG_TUSBD_NET_FS_EP_SIZE, \
                           CONFIG_TUSBD_NET_MTU_SIZE)

#define _TUD_CDC_NCM_DESC(speed) \
    /* Interface number, Description string index, MAC address string index,
     * EP notification address and size, EP Data Out & In, EP size, MTU size. */ \
    TUD_CDC_NCM_DESCRIPTOR(TUSBD_ITF_NET, \
                           TUSBD_STR_IDX_NET_CDC_NCM, TUSBD_STR_IDX_NET_MAC, \
                           TUSBD_EP_NET_NOTIF, \
                           CONFIG_TUSBD_NET_NOTIF_EP_SIZE, \
                           TUSBD_EP_NET_OUT, TUSBD_EP_NET_IN, \
                           speed ? CONFIG_TUSBD_NET_HS_EP_SIZE \
                                 : CONFIG_TUSBD_NET_FS_EP_SIZE, \
                           CONFIG_TUSBD_NET_MTU_SIZE)

#define _TUD_RNDIS_DESC(speed) \
    /* Interface number, Description string index, EP notification address and
     * size, EP Data Out & In, EP size */ \
    TUD_RNDIS_DESCRIPTOR(TUSBD_ITF_NET, TUSBD_STR_IDX_NET_RNDIS, \
                         TUSBD_EP_NET_NOTIF, 8, TUSBD_EP_NET_OUT, \
                         TUSBD_EP_NET_IN, \
                         speed ? CONFIG_TUSBD_NET_HS_EP_SIZE \
                               : CONFIG_TUSBD_NET_FS_EP_SIZE)

#define _TUD_VENDOR_DESC(speed) \
    /* Interface number, string index, EP Out & In address, EP size */ \
    TUD_VENDOR_DESCRIPTOR(TUSBD_ITF_VENDOR, TUSBD_STR_IDX_VENDOR, \
                          TUSBD_EP_VENDOR_OUT, TUSBD_EP_VENDOR_IN, \
                          speed ? CONFIG_TUSBD_VENDOR_HS_EP_SIZE \
                                : CONFIG_TUSBD_VENDOR_FS_EP_SIZE)

/* FS configuration */
__attribute__((weak))
uint8_t const tusb_desc_fs_config[] = {
    _TUD_CONFIG_DESC(_TUD_CONFIG_DESC_ID, TUSBD_DESC_TOTAL_LEN),
#if CONFIG_TUSBD_CDC_NUMOF > 0
    _TUD_CDC_DESC(_tusb_speed_fs, 0),
#endif
#if CONFIG_TUSBD_CDC_NUMOF > 1
    _TUD_CDC_DESC(_tusb_speed_fs, 1),
#endif
#if CONFIG_TUSBD_DFU_NUMOF
    _TUD_DFU_DESC(_tusb_speed_fs),
#endif
#if CONFIG_TUSBD_DFU_RT_NUMOF
    _TUD_DFU_RT_DESC(_tusb_speed_fs),
#endif
#if CONFIG_TUSBD_HID_NUMOF > 0
    _TUD_HID_INOUT_DESC(_tusb_speed_fs, 0),
#endif
#if CONFIG_TUSBD_HID_NUMOF > 1
    _TUD_HID_INOUT_DESC(_tusb_speed_fs, 1),
#endif
#if CONFIG_TUSBD_MSC_NUMOF
    _TUD_MSC_DESC(_tusb_speed_fs),
#endif
#if CONFIG_TUSBD_NET_CDC_ECM
    _TUD_CDC_ECM_DESC(_tusb_speed_fs),
#elif CONFIG_TUSBD_NET_CDC_NCM
    _TUD_CDC_NCM_DESC(_tusb_speed_fs),
#elif CONFIG_TUSBD_NET_RNDIS
    _TUD_RNDIS_DESC(_tusb_speed_fs),
#endif
#if CONFIG_TUSBD_VENDOR_NUMOF
    _TUD_VENDOR_DESC(_tusb_speed_fs),
#endif
};

#if _TUD_CONFIG_DESC_NUMOF == 2
__attribute__((weak))
uint8_t const tusb_desc_fs_config_alt[] = {
    _TUD_CONFIG_DESC(_TUD_CONFIG_DESC_ALT_ID, TUSBD_DESC_ALT_TOTAL_LEN),
#if CONFIG_TUSBD_CDC_NUMOF > 0
    _TUD_CDC_DESC(_tusb_speed_fs, 0),
#endif
#if CONFIG_TUSBD_CDC_NUMOF > 1
    _TUD_CDC_DESC(_tusb_speed_fs, 1),
#endif
#if CONFIG_TUSBD_DFU_NUMOF
    _TUD_DFU_DESC(_tusb_speed_fs),
#endif
#if CONFIG_TUSBD_DFU_RT_NUMOF
    _TUD_DFU_RT_DESC(_tusb_speed_fs),
#endif
#if CONFIG_TUSBD_HID_NUMOF > 0
    _TUD_HID_INOUT_DESC(_tusb_speed_fs, 0),
#endif
#if CONFIG_TUSBD_HID_NUMOF > 1
    _TUD_HID_INOUT_DESC(_tusb_speed_fs, 1),
#endif
#if CONFIG_TUSBD_MSC_NUMOF
    _TUD_MSC_DESC(_tusb_speed_fs),
#endif
    /* The alternative configuration descriptor is only required if CDC ECM and
     * RDNIS are used simultaneously. In this case, the main configuration
     * descriptor contains the CDC ECM interface descriptor and the alternative
     * configuration descriptor contains the RNDIS interface descriptor. */
    _TUD_RNDIS_DESC(_tusb_speed_fs),
#if CONFIG_TUSBD_VENDOR_NUMOF
    _TUD_VENDOR_DESC(_tusb_speed_fs),
#endif
};
#endif /* _TUD_CONFIG_DESC_NUMOF == 2 */

#if TUD_OPT_HIGH_SPEED
/* Per USB specs: high speed capable device must report device_qualifier
 * and other_speed_configuration descriptors */

/* HS configuration */
__attribute__((weak))
uint8_t const tusb_desc_hs_config[] = {
    _TUD_CONFIG_DESC(_TUD_CONFIG_DESC_ID, TUSBD_DESC_TOTAL_LEN),
#if CONFIG_TUSBD_CDC_NUMOF > 0
    _TUD_CDC_DESC(_tusb_speed_hs, 0),
#endif
#if CONFIG_TUSBD_CDC_NUMOF > 1
    _TUD_CDC_DESC(_tusb_speed_hs, 1),
#endif
#if CONFIG_TUSBD_DFU
    _TUD_DFU_DESC(_tusb_speed_hs),
#endif
#if CONFIG_TUSBD_DFU_RT_NUMOF
    _TUD_DFU_RT_DESC(_tusb_speed_hs),
#endif
#if CONFIG_TUSBD_HID_NUMOF > 0
    _TUD_HID_INOUT_DESC(_tusb_speed_hs, 0),
#endif
#if CONFIG_TUSBD_HID_NUMOF > 1
    _TUD_HID_INOUT_DESC(_tusb_speed_hs, 1),
#endif
#if CONFIG_TUSBD_MSC_NUMOF
    _TUD_MSC_DESC(_tusb_speed_hs),
#endif
#if CONFIG_TUSBD_NET_CDC_ECM
    _TUD_CDC_ECM_DESC(_tusb_speed_hs),
#elif CONFIG_TUSBD_NET_CDC_NCM
    _TUD_CDC_NCM_DESC(_tusb_speed_hs),
#elif CONFIG_TUSBD_NET_RNDIS
    _TUD_RNDIS_DESC(_tusb_speed_hs),
#endif
#if CONFIG_TUSBD_VENDOR_NUMOF
    _TUD_VENDOR_DESC(_tusb_speed_hs),
#endif
};

#if _TUD_CONFIG_DESC_NUMOF == 2
__attribute__((weak))
uint8_t const tusb_desc_hs_config_alt[] = {
    _TUD_CONFIG_DESC(_TUD_CONFIG_DESC_ALT_ID, TUSBD_DESC_ALT_TOTAL_LEN),
#if CONFIG_TUSBD_CDC_NUMOF > 0
    _TUD_CDC_DESC(_tusb_speed_hs, 0),
#endif
#if CONFIG_TUSBD_CDC_NUMOF > 1
    _TUD_CDC_DESC(_tusb_speed_hs, 1),
#endif
#if CONFIG_TUSBD_DFU
    _TUD_DFU_DESC(_tusb_speed_hs),
#endif
#if CONFIG_TUSBD_DFU_RT_NUMOF
    _TUD_DFU_RT_DESC(_tusb_speed_hs),
#endif
#if CONFIG_TUSBD_HID_NUMOF > 0
    _TUD_HID_INOUT_DESC(_tusb_speed_hs, 0),
#endif
#if CONFIG_TUSBD_HID_NUMOF > 1
    _TUD_HID_INOUT_DESC(_tusb_speed_hs, 1),
#endif
#if CONFIG_TUSBD_MSC_NUMOF
    _TUD_MSC_DESC(_tusb_speed_hs),
#endif
    /* The alternative configuration descriptor is only required if CDC ECM and
     * RDNIS are used simultaneously. In this case, the main configuration
     * descriptor contains the CDC ECM interface descriptor and the alternative
     * configuration descriptor contains the RNDIS interface descriptor. */
    _TUD_RNDIS_DESC(_tusb_speed_hs),
#if CONFIG_TUSBD_VENDOR_NUMOF
    _TUD_VENDOR_DESC(_tusb_speed_hs),
#endif
};
#endif /* _TUD_CONFIG_DESC_NUMOF == 2 */

/* other speed configuration */
uint8_t tusb_desc_other_speed_config[TUSBD_DESC_TOTAL_LEN];

/* device qualifier is mostly similar to device descriptor since we don't
 * change configuration based on speed */
__attribute__((weak))
tusb_desc_device_qualifier_t const tusb_desc_device_qualifier = {
    .bLength            = sizeof(tusb_desc_device_qualifier_t),
    .bDescriptorType    = TUSB_DESC_DEVICE_QUALIFIER,
    .bcdUSB             = CONFIG_USB_PRODUCT_BCDVERSION,

#if CONFIG_TUSBD_CDC_NUMOF
    /* Use Interface Association Descriptor (IAD) for CDC
     * As required by USB Specs IAD's subclass must be common class (2)
     * and protocol must be IAD (1) */
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
#else
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
#endif

    .bMaxPacketSize0    = CONFIG_TUSBD_EP0_SIZE,
    .bNumConfigurations = _TUD_CONFIG_DESC_NUMOF,
    .bReserved          = 0x00
};

/*
 * The function is invoked when GET DEVICE QUALIFIER DESCRIPTOR is received.
 * It returns a pointer to the device_qualifier descriptor whose
 * contents must exist long enough for transfer to complete. The device
 * qualifier descriptor describes information about a high-speed capable
 * device that would change if the device were operating at the other speed.
 * If the device is not highspeed capable, stall this request.
 */
__attribute__((weak))
uint8_t const *tud_descriptor_device_qualifier_cb(void)
{
    return (uint8_t const*)&tusb_desc_device_qualifier;
}

/*
 * The function is invoked when GET OTHER SEED CONFIGURATION DESCRIPTOR is
 * received. It returns a pointer to the other_speed_configuration descriptor
 * whose contents must exist long enough for transfer to complete.
 */
__attribute__((weak))
uint8_t const *tud_descriptor_other_speed_configuration_cb(uint8_t index)
{
    DEBUG("[tinyusb] %s: %u\n", __func__, index);

    assert(index < _TUD_CONFIG_DESC_NUMOF);

    if (index == _TUD_CONFIG_DESC_ID) {
        /* If the link speed is HS, return the FS config, and vice versa.
         * Note: the descriptor type is OHER_SPEED_CONFIG instead of CONFIG */
        memcpy(tusb_desc_other_speed_config,
               (tud_speed_get() == TUSB_SPEED_HIGH) ? tusb_desc_fs_config
                                                    : tusb_desc_hs_config,
               TUSBD_DESC_TOTAL_LEN);

        tusb_desc_other_speed_config[1] = TUSB_DESC_OTHER_SPEED_CONFIG;

        return tusb_desc_other_speed_config;
    }
#if _TUD_CONFIG_DESC_NUMOF == 2
    else if (index == _TUD_CONFIG_DESC_ALT_ID) {
        /* If the link speed is HS, return the FS config, and vice versa.
         * Note: the descriptor type is OHER_SPEED_CONFIG instead of CONFIG */
        memcpy(tusb_desc_other_speed_config,
               (tud_speed_get() == TUSB_SPEED_HIGH) ? tusb_desc_fs_config_alt
                                                    : tusb_desc_hs_config_alt,
               TUSBD_DESC_ALT_TOTAL_LEN);

        tusb_desc_other_speed_config[1] = TUSB_DESC_OTHER_SPEED_CONFIG;

        return tusb_desc_other_speed_config;
    }
#endif
    return NULL;
}

#endif /* TUD_OPT_HIGH_SPEED */

/*
 * The function is invoked when GET CONFIGURATION DESCRIPTOR is received.
 * It returns pointer to the configuration descriptor whose
 * content must exist long enough for the transfer to complete.
 */
__attribute__((weak))
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    DEBUG("[tinyusb] %s: %u\n", __func__, index);

    assert(index < _TUD_CONFIG_DESC_NUMOF);

    if (index == _TUD_CONFIG_DESC_ID) {
#if TUD_OPT_HIGH_SPEED
        /* Although we are HS, host may be FS. */
        return (tud_speed_get() == TUSB_SPEED_HIGH) ? tusb_desc_hs_config
                                                    : tusb_desc_fs_config;
#else /* TUD_OPT_HIGH_SPEED */
        return (uint8_t const *)tusb_desc_fs_config;
#endif /* TUD_OPT_HIGH_SPEED */
    }

#if _TUD_CONFIG_DESC_NUMOF == 2
    else if (index == _TUD_CONFIG_DESC_ALT_ID) {
#if TUD_OPT_HIGH_SPEED
        /* Although we are HS, host may be FS. */
        return (tud_speed_get() == TUSB_SPEED_HIGH) ? tusb_desc_hs_config_alt
                                                    : tusb_desc_fs_config_alt;
#else /* TUD_OPT_HIGH_SPEED */
        return (uint8_t const *)tusb_desc_fs_config_alt;
#endif /* TUD_OPT_HIGH_SPEED */
    }
#endif /* _TUD_CONFIG_DESC_NUMOF == 2 */

    return NULL;
}

/*
 *--------------------------------------------------------------------+
 * String Descriptors
 *--------------------------------------------------------------------+
 */
#ifndef CONFIG_TUSBD_CDC_0_STRING
#define CONFIG_TUSBD_CDC_0_STRING    "TinyUSB CDC0"
#endif

#ifndef CONFIG_TUSBD_CDC_1_STRING
#define CONFIG_TUSBD_CDC_1_STRING    "TinyUSB CDC1"
#endif

#ifndef CONFIG_TUSBD_CDC_ECM_STRING
#define CONFIG_TUSBD_CDC_ECM_STRING  "TinyUSB CDC ECM"
#endif

#ifndef CONFIG_TUSBD_CDC_NCM_STRING
#define CONFIG_TUSBD_CDC_NCM_STRING  "TinyUSB CDC NCM"
#endif

#ifndef CONFIG_TUSBD_DFU_0_STRING
#define CONFIG_TUSBD_DFU_0_STRING    USB_DFU_MODE_SLOT0_NAME
#endif

#ifndef CONFIG_TUSBD_DFU_1_STRING
#define CONFIG_TUSBD_DFU_1_STRING    USB_DFU_MODE_SLOT1_NAME
#endif

#ifndef CONFIG_TUSBD_DFU_RT_STRING
#define CONFIG_TUSBD_DFU_RT_STRING   USB_APP_MODE_SLOT_NAME
#endif

#ifndef CONFIG_TUSBD_HID_0_STRING
#define CONFIG_TUSBD_HID_0_STRING    "TinyUSB HID0 (Generic In/Out)"
#endif

#ifndef CONFIG_TUSBD_HID_1_STRING
#define CONFIG_TUSBD_HID_1_STRING    "TinyUSB HID1 (Generic In/Out)"
#endif

#ifndef CONFIG_TUSBD_MSC_STRING
#define CONFIG_TUSBD_MSC_STRING      "TinyUSB MSC"
#endif

#ifndef CONFIG_TUSBD_NET_DESC_STRING
#define CONFIG_TUSBD_NET_DESC_STRING "TinyUSB Net"
#endif

#ifndef CONFIG_TUSBD_RNDIS_STRING
#define CONFIG_TUSBD_RNDIS_STRING   "TinyUSB RNDIS"
#endif

#ifndef CONFIG_TUSBD_VENDOR_STRING
#define CONFIG_TUSBD_VENDOR_STRING   "TinyUSB Vendor"
#endif

/* array of pointer to string descriptors */
__attribute__((weak))
char const* tusb_string_desc_array[] = {
    (const char[2]){                /* 0: supported language is English (0x0409) */
        CONFIG_USB_DEFAULT_LANGID & 0xff,
        (CONFIG_USB_DEFAULT_LANGID >> 8) & 0xff,
    },
    CONFIG_USB_MANUF_STR,           /* 1: Manufacturer */
    CONFIG_USB_PRODUCT_STR,         /* 2: Product */
#if CONFIG_USB_CUSTOM_SERIAL_STR
    CONFIG_USB_SERIAL_STR,          /* 3: Serial number as configured */
#else
    NULL,                           /* 3: Serial number generated during runtime */
#endif
#if CONFIG_TUSBD_CDC_NUMOF > 0
    CONFIG_TUSBD_CDC_0_STRING,      /* CDC Interface 0 */
#endif
#if CONFIG_TUSBD_CDC_NUMOF > 1
    CONFIG_TUSBD_CDC_1_STRING,      /* CDC Interface 1 */
#endif
#if CONFIG_TUSBD_DFU_NUMOF
    CONFIG_TUSBD_DFU_0_STRING,      /* DFU Firmware Slot 0 */
    CONFIG_TUSBD_DFU_1_STRING,      /* DFU Firmware Slot 1 */
#endif
#if CONFIG_TUSBD_DFU_RT_NUMOF
    CONFIG_TUSBD_DFU_RT_STRING,     /* APP mode */
#endif
#if CONFIG_TUSBD_HID_NUMOF > 0
    CONFIG_TUSBD_HID_0_STRING,      /* HID Interface 0 */
#endif
#if CONFIG_TUSBD_HID_NUMOF > 1
    CONFIG_TUSBD_HID_1_STRING,      /* HID Interface 1 */
#endif
#if CONFIG_TUSBD_MSC_NUMOF
    CONFIG_TUSBD_MSC_STRING,        /* MSC Interface */
#endif
#if CONFIG_TUSBD_VENDOR_NUMOF
    CONFIG_TUSBD_VENDOR_STRING,     /* Vendor Interface */
#endif

#if CONFIG_TUSBD_NET_NUMOF
#if CONFIG_TUSBD_NET_CDC_ECM
CONFIG_TUSBD_CDC_ECM_STRING,        /* CDC ECM Interface */
#endif /* CONFIG_TUSBD_NET_CDC_ECM */
#if CONFIG_TUSBD_NET_CDC_NCM
CONFIG_TUSBD_CDC_NCM_STRING,        /* CDC NCM Interface */
#endif /* CONFIG_TUSBD_NET_CDC_NCM */
#if CONFIG_TUSBD_NET_RNDIS
CONFIG_TUSBD_RNDIS_STRING,          /* RNDIS Interface */
#endif /* CONFIG_TUSBD_NET_RNDIS */
#ifdef CONFIG_TUSBD_NET_CUSTOM_MAC
    CONFIG_TUSBD_NET_MAC_STRING,    /* NET Interface MAC address */
#else
    NULL,
#endif /* CONFIG_TUSBD_NET_CUSTOM_MAC */
#endif /* CONFIG_TUSBD_NET_NUMOF */
};

static uint16_t _desc_str[32];

/* The function is invoked when GET STRING DESCRIPTOR is received.
 * The function returns a pointer to the string descriptor whose contents must
 * exist long enough for transfer to complete.
 */
__attribute__((weak))
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    uint8_t chr_count = 0;

    if (index == TUSBD_STR_IDX_LANGUAGE) {
        memcpy(&_desc_str[1], tusb_string_desc_array[0], 2);
        chr_count = 1;
    }
    else {
        /* Note: The 0xEE index string is a Microsoft OS 1.0 Descriptors.
         * https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors */
        if (index >= ARRAY_SIZE(tusb_string_desc_array)) {
            return NULL;
        }

        const char* str;

        if ((index == TUSBD_STR_IDX_SERIAL) &&
            (tusb_string_desc_array[index] == NULL)) {
            /* otherwise, a generated string has to be used in case of serial string index */
            static char _serial_str[(CONFIG_USB_SERIAL_BYTE_LENGTH << 1) + 1] = { };
            static_assert(CONFIG_USB_SERIAL_BYTE_LENGTH <= UINT8_MAX/4,
                          "USB serial byte length must be at most 63 due to protocol "
                          "limitations");
            if ((index == TUSBD_STR_IDX_SERIAL) && (strlen(_serial_str) == 0)) {
                /* generate the serial string if it is not yet generated */
                uint8_t luid_buf[CONFIG_USB_SERIAL_BYTE_LENGTH];
                luid_get(luid_buf, sizeof(luid_buf));
                fmt_bytes_hex(_serial_str, luid_buf, sizeof(luid_buf));
                _serial_str[(CONFIG_USB_SERIAL_BYTE_LENGTH << 1)] = 0;
            }
            str = _serial_str;
        }
#if CONFIG_TUSBD_NET_NUMOF
        else if ((index == TUSBD_STR_IDX_NET_MAC) &&
                 (tusb_string_desc_array[index] == NULL)) {
            /* generated MAC address string is used */
            static char mac_str[(ETHERNET_ADDR_LEN << 1) + 1] = { };
            if (strlen(mac_str) == 0) {
                /* generate the serial string if it is not yet generated */
                uint8_t luid_buf[ETHERNET_ADDR_LEN];
                luid_get_eui48((eui48_t*)luid_buf);
                fmt_bytes_hex(mac_str, luid_buf, sizeof(luid_buf));
                mac_str[(ETHERNET_ADDR_LEN << 1)] = 0;
            }
            str = mac_str;
        }
#endif
        else {
            str = tusb_string_desc_array[index];
        }

        /* cap at max char */
        chr_count = (uint8_t)strlen(str);
        if (chr_count > 31) {
            chr_count = 31;
        }

        /* Convert ASCII string into UTF-16 */
        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1+i] = str[i];
        }
    }

    /* first byte is length (including header), second byte is string type */
    _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8 ) | (2 * chr_count + 2));

    return _desc_str;
}

#if !CONFIG_TUSBD_RNDIS
__attribute__((weak))
void rndis_class_set_handler(uint8_t *data, int size)
{
    (void)data;
    (void)size;
}

__attribute__((weak))
void tud_network_init_cb(void)
{
    printf("tud_network_init_cb");
}

__attribute__((weak))
bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
    printf("tud_network_init_cb %p %u", src, size);
    return true;
}
#endif

#endif /* !defined(CONFIG_TUSBD_USE_CUSTOM_DESC) */
