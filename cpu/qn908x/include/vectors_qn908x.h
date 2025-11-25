/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @{
 *
 * @file
 * @brief       Interrupt service routine declarations NXP QN908x MCUs
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#ifndef VECTORS_QN908X_H
#define VECTORS_QN908X_H

#include <stdint.h>
#include "vectors_cortexm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dummy handler
 */
void dummy_handler(void);

/* Device specific interrupt vectors */
void isr_ext_gpio_wakeup(void); /**< Ext GPIO wakeup */
void isr_osc(void);             /**< BLE wakeup */
void isr_acmp0(void);           /**< Analog comparator0 */
void isr_acmp1(void);           /**< Analog comparator1 */
void isr_rtc_sec(void);         /**< RTC second */
void isr_rtc_fr(void);          /**< RTC free running */
void isr_cs_wakeup(void);       /**< Capacitive sense wakeup */
void isr_cs(void);              /**< Capacitive sense */
void isr_gpioa(void);           /**< GPIO group A */
void isr_gpiob(void);           /**< GPIO group B */
void isr_dma0(void);            /**< DMA controller */
void isr_pin_int0(void);        /**< pin or pattern match engine slice 0 */
void isr_pin_int1(void);        /**< pin or pattern match engine slice 1 */
void isr_pin_int2(void);        /**< pin or pattern match engine slice 2 */
void isr_pin_int3(void);        /**< pin or pattern match engine slice 3 */
void isr_osc_int_low(void);     /**< Inverse of OSC */
void isr_usb0(void);            /**< USB device */
void isr_flexcomm0(void);       /**< Flexcomm Interface 0 (USART) */
void isr_flexcomm1(void);       /**< Flexcomm Interface 1 (USART, I2C) */
void isr_flexcomm2(void);       /**< Flexcomm Interface 2 (SPI, I2C) */
void isr_flexcomm3(void);       /**< Flexcomm Interface 3 (SPI) */
void isr_ble(void);             /**< BLE interrupts */
void isr_fsp(void);             /**< FSP */
void isr_qdec0(void);           /**< QDEC0 */
void isr_qdec1(void);           /**< QDEC1 */
void isr_ctimer0(void);         /**< Standard counter/timer CTIMER0 */
void isr_ctimer1(void);         /**< Standard counter/timer CTIMER1 */
void isr_ctimer2(void);         /**< Standard counter/timer CTIMER2 */
void isr_ctimer3(void);         /**< Standard counter/timer CTIMER3 */
void isr_wdt(void);             /**< Watch dog timer */
void isr_adc(void);             /**< ADC */
void isr_dac(void);             /**< DAC */
void isr_xtal_ready(void);      /**< High frequency crystal ready */
void isr_flash(void);           /**< Flash */
void isr_spifi0(void);          /**< SPI flash interface */
void isr_sct0(void);            /**< SCTimer/PWM */
void isr_rng(void);             /**< Random number generator */
void isr_calib(void);           /**< Calibration */
void isr_ble_tx(void);          /**< ble tx flag */
void isr_ble_rx(void);          /**< ble rx flag */
void isr_ble_freq_hop(void);    /**< ble frequency hop */
void isr_bod(void);             /**< Brown out detect */

/**
 * @{
 * @brief In-System Programming configuration field
 *
 * After the interrupt vectors, at address 0x110 there's a ISP
 * (In-System Programming) configuration field. Define isp_configuration to
 * any | combination of the following `QN908X_ISP_*` values to change the ISP
 * allowed options.
 */
__attribute__((section(".vectors.100")))
extern const uint32_t isp_configuration;

/**
 * @brief SMART write enabled to ISP
 */
#define QN908X_ISP_SMART_EN 0x80

/**
 * @brief SMART USB disabled to ISP
 */
#define QN908X_ISP_USB_DIS 0x20

/**
 * @brief SMART UART disabled to ISP
 */
#define QN908X_ISP_UART_DIS 0x08

/**
 * @brief SMART SPI disabled to ISP
 */
#define QN908X_ISP_SPI_DIS 0x02

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VECTORS_QN908X_H */
