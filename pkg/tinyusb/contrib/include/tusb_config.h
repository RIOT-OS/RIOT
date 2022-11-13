/*
 * Copyright (C) 2019 Ha Thach (tinyusb.org)
 *               2022 Gunar Schorcht
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

/**
 * @ingroup     pkg_tinyusb
 * @{
 *
 * @file
 * @brief       TinyUSB default configurations
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

/* defined by the application */
#if __has_include("tinyusb_app_config.h")
#include "tinyusb_app_config.h"
#endif
#include "tinyusb.h"

#if MODULE_TINYUSB_CLASS_NET
#include "net/ethernet.h"
#endif

#if !DOXYGEN

#if MODULE_TINYUSB_CLASS_NET_CDC_NCM && (MODULE_TINYUSB_CLASS_NET_CDC_ECM || MODULE_TINYUSB_CLASS_NET_RNDIS)
#error "CDC NCM device class cannot be used together with CDC ECM or RNDIS device class"
#endif

#ifndef CONFIG_TUSBD_AUDIO_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_AUDIO
#define CONFIG_TUSBD_AUDIO_NUMOF    1
#else
#define CONFIG_TUSBD_AUDIO_NUMOF    0
#endif
#endif

#ifndef CONFIG_TUSBD_BTH_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_BTH
#define CONFIG_TUSBD_BTH_NUMOF      1
#else
#define CONFIG_TUSBD_BTH_NUMOF      0
#endif
#endif

#ifndef CONFIG_TUSBD_CDC_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_CDC
#define CONFIG_TUSBD_CDC_NUMOF      1
#else
#define CONFIG_TUSBD_CDC_NUMOF      0
#endif
#endif

#ifndef CONFIG_TUSBD_DFU_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_DFU
#define CONFIG_TUSBD_DFU_NUMOF      1
#else
#define CONFIG_TUSBD_DFU_NUMOF      0
#endif
#endif

#ifndef CONFIG_TUSBD_DFU_RT_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_DFU_RUNTIME
#define CONFIG_TUSBD_DFU_RT_NUMOF   1
#else
#define CONFIG_TUSBD_DFU_RT_NUMOF   0
#endif
#endif

#ifndef CONFIG_TUSBD_HID_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_HID
#define CONFIG_TUSBD_HID_NUMOF      1
#else
#define CONFIG_TUSBD_HID_NUMOF      0
#endif
#endif

#ifndef CONFIG_TUSBD_MIDI_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_MIDI
#define CONFIG_TUSBD_MIDI_NUMOF     1
#else
#define CONFIG_TUSBD_MIDI_NUMOF     0
#endif
#endif

#ifndef CONFIG_TUSBD_MSC_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_MSC
#define CONFIG_TUSBD_MSC_NUMOF      1
#else
#define CONFIG_TUSBD_MSC_NUMOF      0
#endif
#endif

#ifndef CONFIG_TUSBD_USBTMC_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_USBTMC
#define CONFIG_TUSBD_USBTMC_NUMOF   1
#else
#define CONFIG_TUSBD_USBTMC_NUMOF   0
#endif
#endif

#ifndef CONFIG_TUSBD_VENDOR_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_VENDOR
#define CONFIG_TUSBD_VENDOR_NUMOF   1
#else
#define CONFIG_TUSBD_VENDOR_NUMOF   0
#endif
#endif

#ifndef CONFIG_TUSBD_VIDEO_NUMOF
#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_VIDEO
#define CONFIG_TUSBD_VIDEO_NUMOF    1
#else
#define CONFIG_TUSBD_VIDEO_NUMOF    0
#endif
#endif

#ifndef CONFIG_TUSBH_CDC_NUMOF
#if MODULE_TINYUSB_HOST && MODULE_TINYUSB_CLASS_CDC
#define CONFIG_TUSBH_CDC_NUMOF      1
#else
#define CONFIG_TUSBH_CDC_NUMOF      0
#endif
#endif

#ifndef CONFIG_TUSBH_HID_NUMOF
#if MODULE_TINYUSB_HOST && MODULE_TINYUSB_CLASS_HID
#define CONFIG_TUSBH_HID_NUMOF      1
#else
#define CONFIG_TUSBH_HID_NUMOF      0
#endif
#endif

#ifndef CONFIG_TUSBH_HUB_NUMOF
#if MODULE_TINYUSB_HOST && MODULE_TINYUSB_CLASS_HUB
#define CONFIG_TUSBH_HUB_NUMOF      1
#else
#define CONFIG_TUSBH_HUB_NUMOF      0
#endif
#endif

#ifndef CONFIG_TUSBH_MSC_NUMOF
#if MODULE_TINYUSB_HOST && MODULE_TINYUSB_CLASS_MSC
#define CONFIG_TUSBH_MSC_NUMOF      1
#else
#define CONFIG_TUSBH_MSC_NUMOF      0
#endif
#endif

#ifndef CONFIG_TUSBH_VENDOR_NUMOF
#if MODULE_TINYUSB_HOST && MODULE_TINYUSB_CLASS_VENDOR
#define CONFIG_TUSBH_VENDOR_NUMOF   1
#else
#define CONFIG_TUSBH_VENDOR_NUMOF   0
#endif
#endif

#ifndef CONFIG_TUSBD_EP0_SIZE
#define CONFIG_TUSBD_EP0_SIZE               64
#endif

#ifndef CONFIG_TUSBD_FS_EP_SIZE
#define CONFIG_TUSBD_FS_EP_SIZE             64
#endif

#ifndef CONFIG_TUSBD_HS_EP_SIZE
#define CONFIG_TUSBD_HS_EP_SIZE             512
#endif

#ifndef CONFIG_TUSBD_CDC_NOTIF_EP_SIZE
#define CONFIG_TUSBD_CDC_NOTIF_EP_SIZE      8
#endif

#ifndef CONFIG_TUSBD_CDC_FS_EP_SIZE
#define CONFIG_TUSBD_CDC_FS_EP_SIZE         CONFIG_TUSBD_FS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_CDC_HS_EP_SIZE
#define CONFIG_TUSBD_CDC_HS_EP_SIZE         CONFIG_TUSBD_HS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_DFU_ATTR
#define CONFIG_TUSBD_DFU_ATTR               (DFU_ATTR_CAN_DOWNLOAD | \
                                             DFU_ATTR_WILL_DETACH | \
                                             DFU_ATTR_MANIFESTATION_TOLERANT)
#endif

#ifndef CONFIG_TUSBD_DFU_DETACH_TIMEOUT
#define CONFIG_TUSBD_DFU_DETACH_TIMEOUT     1000
#endif

#ifndef CONFIG_TUSBD_DFU_POLL_TIMEOUT
#define CONFIG_TUSBD_DFU_POLL_TIMEOUT       1
#endif

#ifndef CONFIG_TUSBD_DFU_RESET_DELAY
#define CONFIG_TUSBD_DFU_RESET_DELAY        2
#endif

#ifndef CONFIG_TUSBD_DFU_FS_XFER_SIZE
#define CONFIG_TUSBD_DFU_FS_XFER_SIZE       CONFIG_TUSBD_FS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_DFU_HS_XFER_SIZE
#define CONFIG_TUSBD_DFU_HS_XFER_SIZE       CONFIG_TUSBD_HS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_HID_EP_SIZE
#define CONFIG_TUSBD_HID_EP_SIZE            CONFIG_TUSBD_FS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_DFU_RT_DETACH_TIMEOUT
#define CONFIG_TUSBD_DFU_RT_DETACH_TIMEOUT  1000
#endif

#ifndef CONFIG_TUSBD_DFU_RT_FS_XFER_SIZE
#define CONFIG_TUSBD_DFU_RT_FS_XFER_SIZE    CONFIG_TUSBD_FS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_DFU_RT_HS_XFER_SIZE
#define CONFIG_TUSBD_DFU_RT_HS_XFER_SIZE    CONFIG_TUSBD_HS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_HID_EP_SIZE
#define CONFIG_TUSBD_HID_EP_SIZE            CONFIG_TUSBD_FS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_HID_0_POLL_INTERVALL
#define CONFIG_TUSBD_HID_0_POLL_INTERVALL   10
#endif

#ifndef CONFIG_TUSBD_HID_1_POLL_INTERVALL
#define CONFIG_TUSBD_HID_1_POLL_INTERVALL   10
#endif

#ifndef CONFIG_TUSBD_MSC_FS_EP_SIZE
#define CONFIG_TUSBD_MSC_FS_EP_SIZE         CONFIG_TUSBD_FS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_MSC_HS_EP_SIZE
#define CONFIG_TUSBD_MSC_HS_EP_SIZE         CONFIG_TUSBD_HS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_NET_FS_EP_SIZE
#define CONFIG_TUSBD_NET_FS_EP_SIZE         CONFIG_TUSBD_FS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_NET_HS_EP_SIZE
#define CONFIG_TUSBD_NET_HS_EP_SIZE         CONFIG_TUSBD_HS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_VENDOR_FS_EP_SIZE
#define CONFIG_TUSBD_VENDOR_FS_EP_SIZE      CONFIG_TUSBD_FS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_VENDOR_HS_EP_SIZE
#define CONFIG_TUSBD_VENDOR_HS_EP_SIZE      CONFIG_TUSBD_HS_EP_SIZE
#endif

#ifndef CONFIG_TUSBD_NET_NOTIF_EP_SIZE
#define CONFIG_TUSBD_NET_NOTIF_EP_SIZE      64
#endif

#ifndef CONFIG_TUSBD_NET_MTU_SIZE
#define CONFIG_TUSBD_NET_MTU_SIZE           ETHERNET_FRAME_LEN
#endif

#if MODULE_TINYUSB_CLASS_NET_CDC_ECM
#define CONFIG_TUSBD_NET_CDC_ECM            1
#else
#define CONFIG_TUSBD_NET_CDC_ECM            0
#endif

#if MODULE_TINYUSB_CLASS_NET_CDC_NCM
#define CONFIG_TUSBD_NET_CDC_NCM            1
#else
#define CONFIG_TUSBD_NET_CDC_NCM            0
#endif

#if MODULE_TINYUSB_CLASS_NET_RNDIS
#define CONFIG_TUSBD_NET_RNDIS              1
#else
#define CONFIG_TUSBD_NET_RNDIS              0
#endif

#if CONFIG_TUSBD_NET_CDC_ECM || CONFIG_TUSBD_NET_CDC_NCM || CONFIG_TUSBD_NET_RNDIS
#define CONFIG_TUSBD_NET_NUMOF              1
#else
#define CONFIG_TUSBD_NET_NUMOF              0
#endif

#ifndef CONFIG_TUSBH_ENUM_SIZE
#define CONFIG_TUSBH_ENUM_SIZE              256
#endif

#ifndef CONFIG_TUSBH_HID_EPIN_SIZE
#define CONFIG_TUSBH_HID_EPIN_SIZE          CONFIG_TUSBD_HID_EP_SIZE
#endif

#ifndef CONFIG_TUSBH_HID_EPOUT_SIZE
#define CONFIG_TUSBH_HID_EPOUT_SIZE         CONFIG_TUSBD_HID_EP_SIZE
#endif

/**
 * @name Common tinyUSB configurations
 * @{
 */
/** Sanity check, the MCU must be defined */
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

/** RIOT provides the custom OS API */
#define CFG_TUSB_OS                 OPT_OS_CUSTOM

/** Debug log level */
#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG              0
#endif

/**
 * @brief DMA memory section and alignment
 *
 * USB DMA on some MCUs can only access a specific SRAM region with restriction
 * on alignment.
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN          __attribute__ ((aligned(4)))

/** @} */

/**
 * @name Common device configurations
 * @{
 */
#define CFG_TUD_ENABLED             MODULE_TINYUSB_DEVICE

#define CFG_TUD_ENDPOINT0_SIZE      CONFIG_TUSBD_EP0_SIZE

#define CFG_TUD_AUDIO               CONFIG_TUSBD_AUDIO_NUMOF
#define CFG_TUD_BTH                 CONFIG_TUSBD_BTH_NUMOF
#define CFG_TUD_CDC                 CONFIG_TUSBD_CDC_NUMOF
#define CFG_TUD_ECM_RNDIS           (CONFIG_TUSBD_NET_CDC_ECM || CONFIG_TUSBD_NET_RNDIS)
#define CFG_TUD_DFU                 CONFIG_TUSBD_DFU_NUMOF
#define CFG_TUD_DFU_RUNTIME         CONFIG_TUSBD_DFU_RT_NUMOF
#define CFG_TUD_HID                 CONFIG_TUSBD_HID_NUMOF
#define CFG_TUD_MIDI                CONFIG_TUSBD_MIDI_NUMOF
#define CFG_TUD_MSC                 CONFIG_TUSBD_MSC_NUMOF
#define CFG_TUD_NCM                 CONFIG_TUSBD_NET_CDC_NCM
#define CFG_TUD_USBTMC              CONFIG_TUSBD_USBTMC_NUMOF
#define CFG_TUD_VENDOR              CONFIG_TUSBD_VENDOR_NUMOF
#define CFG_TUD_VIDEO               CONFIG_TUSBD_VIDEO_NUMOF

/** @} */

/**
 * @name Common host configurations
 * @{
 */
#define CFG_TUH_ENABLED             MODULE_TINYUSB_HOST

#define CFG_TUH_MAX_SPEED           OPT_MODE_DEFAULT_SPEED

#define CFG_TUH_ENUMERATION_BUFSIZE CONFIG_TUSBH_ENUM_SIZE

/** Hub typically has 4 ports */
#define CFG_TUH_DEVICE_MAX          (CFG_TUH_HUB ? 4 : 1)

#define CFG_TUH_CDC                 CONFIG_TUSBH_CDC_NUMOF
#define CFG_TUH_HID                 CONFIG_TUSBH_HID_NUMOF
#define CFG_TUH_HUB                 CONFIG_TUSBH_HUB_NUMOF
#define CFG_TUH_MSC                 CONFIG_TUSBH_MSC_NUMOF
#define CFG_TUH_VENDOR              CONFIG_TUSBH_VENDOR_NUMOF

/** @} */

/**
 * @name Typical required CDC device class configurations
 * @{
 */
#define CFG_TUD_CDC_EP_BUFSIZE      (TUD_OPT_HIGH_SPEED ? CONFIG_TUSBD_CDC_HS_EP_SIZE \
                                                        : CONFIG_TUSBD_CDC_FS_EP_SIZE)
#define CFG_TUD_CDC_RX_BUFSIZE      CFG_TUD_CDC_EP_BUFSIZE
#define CFG_TUD_CDC_TX_BUFSIZE      CFG_TUD_CDC_EP_BUFSIZE

/** @} */

/**
 * @name Typical required DFU device class configurations
 * @{
 */
#define CFG_TUD_DFU_XFER_BUFSIZE    (TUD_OPT_HIGH_SPEED ? CONFIG_TUSBD_DFU_HS_XFER_SIZE \
                                                        : CONFIG_TUSBD_DFU_FS_XFER_SIZE)
/** @} */

/**
 * @name Typical required HID device class configurations
 * @{
 */
#define CFG_TUD_HID_EP_BUFSIZE      CONFIG_TUSBD_HID_EP_SIZE
/** @} */

/**
 * @name Typical required MIDI device class configurations
 * @{
 */
#define CFG_TUD_MIDI_RX_BUFSIZE     CFG_TUD_MIDI_EP_BUFSIZE
#define CFG_TUD_MIDI_TX_BUFSIZE     CFG_TUD_MIDI_EP_BUFSIZE
/** @} */

/**
 * @name Typical required MSC device class configurations
 * @{
 */
#define CFG_TUD_MSC_EP_BUFSIZE      (TUD_OPT_HIGH_SPEED ? CONFIG_TUSBD_MSC_HS_EP_SIZE \
                                                        : CONFIG_TUSBD_MSC_FS_EP_SIZE)
/** @} */

/**
 * @name Typical required NET device class configurations
 * @{
 */
#define CFG_TUD_NET_MTU             CONFIG_TUSBD_NET_MTU_SIZE

/**
 * @name Typical required HID host class configurations
 * @{
 */
#define CFG_TUH_HID_EPIN_BUFSIZE    CONFIG_TUSBH_HID_EPIN_SIZE
#define CFG_TUH_HID_EPOUT_BUFSIZE   CONFIG_TUSBH_HID_EPOUT_SIZE
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* !DOXYGEN */
#endif /* TUSB_CONFIG_H */
/** @} */
