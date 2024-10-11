/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_tinyusb
 * @{
 *
 * @file
 * @brief       TinyUSB commonn descriptor implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef TINYUSB_DESCRIPTORS_H
#define TINYUSB_DESCRIPTORS_H

#include "tusb_config.h"        /* defined by the application */
#include "tinyusb.h"

#if !DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/* don't compile this part if CONFIG_TUSBD_USE_CUSTOM_DESC is set */
#if !defined(CONFIG_TUSBD_USE_CUSTOM_DESC)

#if !defined(HAVE_TUSBD_ITF_TYPE)
enum {
#if CONFIG_TUSBD_CDC_NUMOF > 0
    TUSBD_ITF_CDC_0,        /* CDC0 Notification interface */
    TUSBD_ITF_CDC_0_DATA,   /**< CDC0 Data interface */
#endif
#if CONFIG_TUSBD_CDC_NUMOF > 1
    TUSBD_ITF_CDC_1,        /**< CDC1 Notification interface */
    TUSBD_ITF_CDC_1_DATA,   /**< CDC1 Data interface */
#endif
#if CONFIG_TUSBD_DFU_NUMOF
    TUSBD_ITF_DFU,          /**< DFU interface */
#endif
#if CONFIG_TUSBD_DFU_RT_NUMOF
    TUSBD_ITF_DFU_RT,       /**< DFU Runtime interface */
#endif
#if CONFIG_TUSBD_HID_NUMOF > 0
    TUSBD_ITF_HID_0,        /**< HID0 interface */
#endif
#if CONFIG_TUSBD_HID_NUMOF > 1
    TUSBD_ITF_HID_1,        /**< HID1 interface */
#endif
#if CONFIG_TUSBD_MSC_NUMOF
    TUSBD_ITF_MSC,          /**< MSC interface */
#endif
#if CONFIG_TUSBD_NET_NUMOF
    TUSBD_ITF_NET,          /**< NET Notification interface */
    TUSBD_ITF_NET_DATA,     /**< NET Data interface */
#endif
#if CONFIG_TUSBD_VENDOR_NUMOF
    TUSBD_ITF_VENDOR,       /**< Vendor interface */
#endif
    TUSBD_ITF_NUMOF,        /**< Number of used interfaces */
};
#endif /* !defined(HAVE_TUSBD_ITF_TYPE) */

#if !defined(HAVE_TUSBD_EP_TYPE)
enum {
#if CONFIG_TUSBD_CDC_NUMOF > 0
    TUSBD_EP_CDC_0_NOTIF = TUSBD_ITF_CDC_0 + 0x81,      /**< CDC0 Nototification EP IN */
    TUSBD_EP_CDC_0_OUT   = TUSBD_ITF_CDC_0_DATA + 0x01, /**< CDC0 Data EP OUT */
    TUSBD_EP_CDC_0_IN    = TUSBD_ITF_CDC_0_DATA + 0x81, /**< CDC0 Data EP IN */
#endif
#if CONFIG_TUSBD_CDC_NUMOF > 1
    TUSBD_EP_CDC_1_NOTIF = TUSBD_ITF_CDC_1 + 0x81,      /**< CDC1 Nototification EP IN */
    TUSBD_EP_CDC_1_OUT   = TUSBD_ITF_CDC_1_DATA + 0x01, /**< CDC1 Data EP OUT */
    TUSBD_EP_CDC_1_IN    = TUSBD_ITF_CDC_1_DATA + 0x81, /**< CDC1 Data EP OUT */
#endif
#if CONFIG_TUSBD_HID_NUMOF > 0
    TUSBD_EP_HID_0_OUT   = TUSBD_ITF_HID_0 + 0x01,      /**< HID0 EP OUT */
    TUSBD_EP_HID_0_IN    = TUSBD_ITF_HID_0 + 0x81,      /**< HID0 EP IN */
#endif
#if CONFIG_TUSBD_HID_NUMOF > 1
    TUSBD_EP_HID_1_OUT   = TUSBD_ITF_HID_1 + 0x01,      /**< HID1 EP OUT */
    TUSBD_EP_HID_1_IN    = TUSBD_ITF_HID_1 + 0x81,      /**< HID1 EP IN */
#endif
#if CONFIG_TUSBD_MSC_NUMOF
    TUSBD_EP_MSC_OUT     = TUSBD_ITF_MSC + 0x01,        /**< MSC EP OUT */
    TUSBD_EP_MSC_IN      = TUSBD_ITF_MSC + 0x81,        /**< MSC EP IN */
#endif
#if CONFIG_TUSBD_NET_NUMOF
    TUSBD_EP_NET_NOTIF   = TUSBD_ITF_NET + 0x81,        /**< NET Notification EP IN */
    TUSBD_EP_NET_OUT     = TUSBD_ITF_NET_DATA + 0x01,   /**< NET Data EP OUT */
    TUSBD_EP_NET_IN      = TUSBD_ITF_NET_DATA + 0x81,   /**< NET Data EP IN */
#endif
#if CONFIG_TUSBD_VENDOR_NUMOF
    TUSBD_EP_VENDOR_OUT  = TUSBD_ITF_VENDOR + 0x01,     /**< Vendor EP OUT */
    TUSBD_EP_VENDOR_IN   = TUSBD_ITF_VENDOR + 0x81,     /**< Vendor EP IN */
#endif
    TUSBD_EP_NUMOF,
};
#endif /* !defined(HAVE_TUSBD_EP_TYPE) */

#if !defined(HAVE_TUSBD_STR_IDX_TYPE)
enum {
    TUSBD_STR_IDX_LANGUAGE = 0,
    TUSBD_STR_IDX_MANUFACTURER,
    TUSBD_STR_IDX_PRODUCT,
    TUSBD_STR_IDX_SERIAL,
#if CONFIG_TUSBD_CDC_NUMOF > 0
    TUSBD_STR_IDX_CDC_0,
#endif
#if CONFIG_TUSBD_CDC_NUMOF > 1
    TUSBD_STR_IDX_CDC_1,
#endif
#if CONFIG_TUSBD_DFU_NUMOF
    TUSBD_STR_IDX_DFU_SLOT_0,
    TUSBD_STR_IDX_DFU_SLOT_1,
#endif
#if CONFIG_TUSBD_DFU_RT_NUMOF
    TUSBD_STR_IDX_DFU_RT,
#endif
#if CONFIG_TUSBD_HID_NUMOF > 0
    TUSBD_STR_IDX_HID_0,
#endif
#if CONFIG_TUSBD_HID_NUMOF > 1
    TUSBD_STR_IDX_HID_1,
#endif
#if CONFIG_TUSBD_MSC_NUMOF
    TUSBD_STR_IDX_MSC,
#endif

#if CONFIG_TUSBD_NET_NUMOF
#if CONFIG_TUSBD_NET_CDC_ECM
    TUSBD_STR_IDX_NET_CDC_ECM,
#endif /* CONFIG_TUSBD_NET_CDC_ECM */
#if CONFIG_TUSBD_NET_CDC_NCM
    TUSBD_STR_IDX_NET_CDC_NCM,
#endif /* CONFIG_TUSBD_NET_CDC_NCM */
#if CONFIG_TUSBD_NET_RNDIS
    TUSBD_STR_IDX_NET_RNDIS,
#endif /* CONFIG_TUSBD_NET_RNDIS */
    TUSBD_STR_IDX_NET_MAC,
#endif /* CONFIG_TUSBD_NET_NUMOF */

#if CONFIG_TUSBD_VENDOR_NUMOF
    TUSBD_STR_IDX_VENDOR,
#endif
    TUSBD_STR_IDX_NUMOF,
};
#endif /* !defined(HAVE_TUSBD_STR_IDX_TYPE) */

/* only two slots are supported */
#define CONFIG_TUSBD_DFU_ALT_NUMOF  2

#if !defined(TUSBD_DESC_TOTAL_LEN)

#if CONFIG_TUSBD_NET_CDC_ECM
#define TUSBD_DESC_NET_LEN    TUD_CDC_ECM_DESC_LEN
#elif CONFIG_TUSBD_NET_CDC_NCM
#define TUSBD_DESC_NET_LEN    TUD_CDC_NCM_DESC_LEN
#elif CONFIG_TUSBD_NET_RNDIS
#define TUSBD_DESC_NET_LEN    TUD_RNDIS_DESC_LEN
#else
#define TUSBD_DESC_NET_LEN    0
#endif

#define TUSBD_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + \
                              (CONFIG_TUSBD_CDC_NUMOF * TUD_CDC_DESC_LEN) + \
                              (CONFIG_TUSBD_DFU_NUMOF * TUD_DFU_DESC_LEN(CONFIG_TUSBD_DFU_ALT_NUMOF)) + \
                              (CONFIG_TUSBD_DFU_RT_NUMOF * TUD_DFU_RT_DESC_LEN) + \
                              (CONFIG_TUSBD_HID_NUMOF * TUD_HID_INOUT_DESC_LEN) + \
                              (CONFIG_TUSBD_MSC_NUMOF * TUD_MSC_DESC_LEN) + \
                              (CONFIG_TUSBD_VENDOR_NUMOF * TUD_VENDOR_DESC_LEN) + \
                              TUSBD_DESC_NET_LEN)

#endif /* !defined(TUSBD_DESC_TOTAL_LEN) */

/* If CDC ECM and RNDIS are used simultaneously, an alternative configuration
 * descriptor is required. In this case, the main configuration descriptor
 * contains the CDC ECM interface descriptor and the alternative configuration
 * descriptor contains the RNDIS interface descriptor. */
#if CONFIG_TUSBD_NET_CDC_ECM && CONFIG_TUSBD_NET_RNDIS

#define TUSBD_DESC_ALT_NET_LEN    TUD_RNDIS_DESC_LEN

#define TUSBD_DESC_ALT_TOTAL_LEN (TUD_CONFIG_DESC_LEN + \
                                 (CONFIG_TUSBD_CDC_NUMOF * TUD_CDC_DESC_LEN) + \
                                 (CONFIG_TUSBD_DFU_NUMOF * TUD_DFU_DESC_LEN(CONFIG_TUSBD_DFU_ALT_NUMOF)) + \
                                 (CONFIG_TUSBD_DFU_RT_NUMOF * TUD_DFU_RT_DESC_LEN) + \
                                 (CONFIG_TUSBD_HID_NUMOF * TUD_HID_INOUT_DESC_LEN) + \
                                 (CONFIG_TUSBD_MSC_NUMOF * TUD_MSC_DESC_LEN) + \
                                 (CONFIG_TUSBD_VENDOR_NUMOF * TUD_VENDOR_DESC_LEN) + \
                                 TUSBD_DESC_ALT_NET_LEN)

#endif /* CONFIG_TUSBD_NET_CDC_ECM && CONFIG_TUSBD_NET_RNDIS */

#endif /* !defined(CONFIG_TUSBD_USE_CUSTOM_DESC) */

#ifdef __cplusplus
}
#endif

#endif /* !DOXYGEN */
/** @} */
#endif /* TINYUSB_DESCRIPTORS_H */
