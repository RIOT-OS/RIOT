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

#ifndef TINYUSB_CONFIG_H
#define TINYUSB_CONFIG_H

#include "tusb_config.h"        /* defined by the application */
#include "tinyusb.h"

#if !DOXYGEN

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
#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN          __attribute__ ((aligned(4)))
#endif

/** @} */

/**
 * @name Common device configurations
 * @{
 */
#define CFG_TUD_ENABLED             MODULE_TINYUSB_DEVICE

#ifndef CFG_TUD_MAX_SPEED
#define CFG_TUD_MAX_SPEED           OPT_MODE_DEFAULT_SPEED
#endif

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE      64
#endif

#ifndef CFG_TUD_AUDIO
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_AUDIO)
#define CFG_TUD_AUDIO               1
#else
#define CFG_TUD_AUDIO               0
#endif
#endif

#ifndef CFG_TUD_BTH
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_BTH)
#define CFG_TUD_BTH                 1
#else
#define CFG_TUD_BTH                 0
#endif
#endif /* CFG_TUD_BTH */

#ifndef CFG_TUD_CDC
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_CDC)
#define CFG_TUD_CDC                 1
#else
#define CFG_TUD_CDC                 0
#endif
#endif /* CFG_TUD_CDC */

#ifndef CFG_TUD_DFU
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_DFU)
#define CFG_TUD_DFU                 1
#else
#define CFG_TUD_DFU                 0
#endif
#endif /* CFG_TUD_DFU */

#ifndef CFG_TUD_DFU_RUNTIME
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_DFU_RUNTIME)
#define CFG_TUD_DFU_RUNTIME         1
#else
#define CFG_TUD_DFU_RUNTIME         0
#endif
#endif /* CFG_TUD_DFU */

#ifndef CFG_TUD_HID
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_HID)
#define CFG_TUD_HID                 1
#else
#define CFG_TUD_HID                 0
#endif
#endif /* CFG_TUD_HID */

#ifndef CFG_TUD_MIDI
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_MIDI)
#define CFG_TUD_MIDI                1
#else
#define CFG_TUD_MIDI                0
#endif
#endif /* CFG_TUD_MIDI */

#ifndef CFG_TUD_MSC
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_MSC)
#define CFG_TUD_MSC                 1
#else
#define CFG_TUD_MSC                 0
#endif
#endif /* CFG_TUD_MSC */

#ifndef CFG_TUD_ECM_RNDIS
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_NET_ECM_RNDIS)
#define CFG_TUD_ECM_RNDIS           1
#else
#define CFG_TUD_ECM_RNDIS           0
#endif
#endif /* CFG_TUD_ECM_RNDIS */

#ifndef CFG_TUD_NCM
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_NET_NCM)
#define CFG_TUD_NCM                 1
#else
#define CFG_TUD_NCM                 0
#endif
#endif /* CFG_TUD_NCM */

#ifndef CFG_TUD_USBMTC
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_USBMTC)
#define CFG_TUD_USBMTC              1
#else
#define CFG_TUD_USBMTC              0
#endif
#endif /* CFG_TUD_USBMTC */

#ifndef CFG_TUD_VENDOR
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_VENDOR)
#define CFG_TUD_VENDOR              1
#else
#define CFG_TUD_VENDOR              0
#endif
#endif /* CFG_TUD_VENDOR */

#ifndef CFG_TUD_VIDEO
#if defined(MODULE_TINYUSB_DEVICE) && defined(MODULE_TINYUSB_CLASS_VIDEO)
#define CFG_TUD_VIDEO               1
#else
#define CFG_TUD_VIDEO               0
#endif
#endif /* CFG_TUD_VIDEO */

/** @} */

/**
 * @name Common host configurations
 * @{
 */
#define CFG_TUH_ENABLED             MODULE_TINYUSB_HOST

#ifndef CFG_TUH_MAX_SPEED
#define CFG_TUH_MAX_SPEED           OPT_MODE_DEFAULT_SPEED
#endif

#ifndef CFG_TUH_ENUMERATION_BUFSIZE
#define CFG_TUH_ENUMERATION_BUFSIZE 256
#endif

/** Hub typically has 4 ports */
#ifndef CFG_TUH_DEVICE_MAX
#define CFG_TUH_DEVICE_MAX          (CFG_TUH_HUB ? 4 : 1)
#endif

#ifndef CFG_TUH_CDC
#if defined(MODULE_TINYUSB_HOST) && defined(MODULE_TINYUSB_CLASS_CDC)
#define CFG_TUH_CDC                 1
#else
#define CFG_TUH_CDC                 0
#endif
#endif /* CFG_TUH_CDC */

#ifndef CFG_TUH_HID
#if defined(MODULE_TINYUSB_HOST) && defined(MODULE_TINYUSB_CLASS_HID)
#define CFG_TUH_HID                 1
#else
#define CFG_TUH_HID                 0
#endif
#endif /* CFG_TUH_HID */

#ifndef CFG_TUH_MSC
#if defined(MODULE_TINYUSB_HOST) && defined(MODULE_TINYUSB_CLASS_MSC)
#define CFG_TUH_MSC                 1
#else
#define CFG_TUH_MSC                 0
#endif
#endif /* CFG_TUH_MSC */

#ifndef CFG_TUD_VENDOR
#if defined(MODULE_TINYUSB_HOST) && defined(MODULE_TINYUSB_CLASS_VENDOR)
#define CFG_TUH_VENDOR              1
#else
#define CFG_TUH_VENDOR              0
#endif
#endif /* CFG_TUD_VENDOR */

/** @} */

/**
 * @name Typical required CDC device class configurations
 * @{
 */
/** CDC RX FIFO size */
#ifndef CFG_TUD_CDC_RX_BUFSIZE
#define CFG_TUD_CDC_RX_BUFSIZE      CFG_TUD_CDC_EP_BUFSIZE
#endif

/** CDC RX FIFO size */
#ifndef CFG_TUD_CDC_TX_BUFSIZE
#define CFG_TUD_CDC_TX_BUFSIZE      CFG_TUD_CDC_EP_BUFSIZE
#endif

/** @} */

/**
 * @name Typical required DFU device class configurations
 * @{
 */
#ifndef CFG_TUD_DFU_XFER_BUFSIZE
#define CFG_TUD_DFU_XFER_BUFSIZE    (TUD_OPT_HIGH_SPEED ? 512 : 64)
#endif
/** @} */

/**
 * @name Typical required MIDI device class configurations
 * @{
 */
#ifndef CFG_TUD_MIDI_RX_BUFSIZE
#define CFG_TUD_MIDI_RX_BUFSIZE     CFG_TUD_MIDI_EP_BUFSIZE
#endif

#ifndef CFG_TUD_MIDI_TX_BUFSIZE
#define CFG_TUD_MIDI_TX_BUFSIZE     CFG_TUD_MIDI_EP_BUFSIZE
#endif
/** @} */

/**
 * @name Typical required MSC device class configurations
 * @{
 */
#ifndef CFG_TUD_MSC_EP_BUFSIZE
#define CFG_TUD_MSC_EP_BUFSIZE      512
#endif
/** @} */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* !DOXYGEN */
#endif /* TINYUSB_CONFIG_H */
/** @} */
