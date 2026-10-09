/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Register definitions for the USB device driver
 *
 * The USB controller of the RP2350 is the same as the one of the RP2040, so
 * the USB device driver of the RP2040 in `cpu/rpx0xx/periph_common/usbdev.c` is
 * used. This file maps the register names of the RP2040 vendor header used
 * by the driver to the register names of the Pico SDK.
 *
 * @author          Christopher Buechse
 */

#include "compat_layer.h"
#include "periph_cpu.h"
#include "regs/resets.h"
#include "regs/usb.h"
#include "regs/usb_device_dpram.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    USB controller registers and DPRAM
 * @{
 */
#define USBCTRL_REGS                                    USB
#define USBCTRL_DPRAM                                   USB_DPRAM
/** @} */

/**
 * @name    Buffer and endpoint control registers in the DPRAM
 * @{
 */
#define USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_AVAILABLE_0_Msk \
    USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_AVAILABLE_0_BITS
#define USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_FULL_0_Msk \
    USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_FULL_0_BITS
#define USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_LENGTH_0_Msk \
    USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_LENGTH_0_BITS
#define USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_PID_0_Msk \
    USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_PID_0_BITS
#define USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_Msk \
    USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_BITS
#define USBCTRL_DPRAM_EP1_IN_CONTROL_ENABLE_Msk \
    USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENABLE_BITS
#define USBCTRL_DPRAM_EP1_IN_CONTROL_INTERRUPT_PER_BUFF_Msk \
    USB_DEVICE_DPRAM_EP1_IN_CONTROL_INTERRUPT_PER_BUFF_BITS
#define USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Pos \
    USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_LSB
#define USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Control \
    USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_VALUE_CONTROL
#define USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Isochronous \
    USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_VALUE_ISOCHRONOUS
#define USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Bulk \
    USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_VALUE_BULK
#define USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Interrupt \
    USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_VALUE_INTERRUPT
/** @} */

/**
 * @name    USB controller register bits
 * @{
 */
#define USBCTRL_REGS_ADDR_ENDP_ADDRESS_Msk                  USB_ADDR_ENDP_ADDRESS_BITS
#define USBCTRL_REGS_EP_STALL_ARM_EP0_IN_Msk                USB_EP_STALL_ARM_EP0_IN_BITS
#define USBCTRL_REGS_EP_STALL_ARM_EP0_OUT_Msk               USB_EP_STALL_ARM_EP0_OUT_BITS
#define USBCTRL_REGS_INTE_BUFF_STATUS_Msk                   USB_INTE_BUFF_STATUS_BITS
#define USBCTRL_REGS_INTE_SETUP_REQ_Msk                     USB_INTE_SETUP_REQ_BITS
#define USBCTRL_REGS_INTS_BUFF_STATUS_Msk                   USB_INTS_BUFF_STATUS_BITS
#define USBCTRL_REGS_INTS_BUS_RESET_Msk                     USB_INTS_BUS_RESET_BITS
#define USBCTRL_REGS_INTS_DEV_RESUME_FROM_HOST_Msk          USB_INTS_DEV_RESUME_FROM_HOST_BITS
#define USBCTRL_REGS_INTS_DEV_SUSPEND_Msk                   USB_INTS_DEV_SUSPEND_BITS
#define USBCTRL_REGS_INTS_SETUP_REQ_Msk                     USB_INTS_SETUP_REQ_BITS
#define USBCTRL_REGS_MAIN_CTRL_CONTROLLER_EN_Msk            USB_MAIN_CTRL_CONTROLLER_EN_BITS
#define USBCTRL_REGS_SIE_CTRL_EP0_INT_1BUF_Msk              USB_SIE_CTRL_EP0_INT_1BUF_BITS
#define USBCTRL_REGS_SIE_CTRL_PULLUP_EN_Msk                 USB_SIE_CTRL_PULLUP_EN_BITS
#define USBCTRL_REGS_SIE_STATUS_BUS_RESET_Msk               USB_SIE_STATUS_BUS_RESET_BITS
#define USBCTRL_REGS_SIE_STATUS_RESUME_Msk                  USB_SIE_STATUS_RESUME_BITS
#define USBCTRL_REGS_SIE_STATUS_SETUP_REC_Msk               USB_SIE_STATUS_SETUP_REC_BITS
#define USBCTRL_REGS_SIE_STATUS_SUSPENDED_Msk               USB_SIE_STATUS_SUSPENDED_BITS
#define USBCTRL_REGS_USB_MUXING_SOFTCON_Msk                 USB_USB_MUXING_SOFTCON_BITS
#define USBCTRL_REGS_USB_MUXING_TO_PHY_Msk                  USB_USB_MUXING_TO_PHY_BITS
#define USBCTRL_REGS_USB_PWR_VBUS_DETECT_Msk                USB_USB_PWR_VBUS_DETECT_BITS
#define USBCTRL_REGS_USB_PWR_VBUS_DETECT_OVERRIDE_EN_Msk    USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_BITS
#define RESETS_RESET_usbctrl_Msk                            RESETS_RESET_USBCTRL_BITS
/** @} */

/**
 * @name    Helper functions of the RP2040 port used by the driver
 * @{
 */
#define io_reg_atomic_set(reg, mask)    atomic_set(reg, mask)
#define io_reg_atomic_clear(reg, mask)  atomic_clear(reg, mask)
#define periph_reset(mask)              atomic_set(&RESETS->RESET, mask)
#define periph_reset_done(mask)         reset_component(mask, mask)
#ifdef RP2350_USE_RISCV
#  define NVIC_EnableIRQ(irq)           rp_irq_enable(irq)
#  define cortexm_isr_end()             rp_end_isr()
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
