/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_amg88xx
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   AMG88xx chip addresses.
 */
#define AMG88XX_I2C_ADDRESS     (0x69)  /**< Default I2C address */

/**
 * @name    AMG88xx device registers.
 * @{
 */
#define AMG88XX_REG_PCTL        (0x00)  /**< Power control register */
#define AMG88XX_REG_RST         (0x01)  /**< Reset register */
#define AMG88XX_REG_FPSC        (0x02)  /**< Frame rate register */
#define AMG88XX_REG_INTC        (0x03)  /**< Interrupt control register */
#define AMG88XX_REG_STAT        (0x04)  /**< Status register */
#define AMG88XX_REG_SCLR        (0x05)  /**< Status clear register */
#define AMG88XX_REG_AVE         (0x07)  /**< Average register */
#define AMG88XX_REG_INTHL       (0x08)  /**< Interrupt upper level low byte */
#define AMG88XX_REG_INTHH       (0x09)  /**< Interrupt upper level high byte */
#define AMG88XX_REG_INTLL       (0x0A)  /**< Interrupt lower level low byte */
#define AMG88XX_REG_INTLH       (0x0B)  /**< Interrupt lower level high byte */
#define AMG88XX_REG_INYSL       (0x0C)  /**< Interrupt hysteresis low byte */
#define AMG88XX_REG_INYSH       (0x0D)  /**< Interrupt hysteresis high byte */
#define AMG88XX_REG_TTHL        (0x0E)  /**< Thermistor temperature low byte */
#define AMG88XX_REG_TTHH        (0x0F)  /**< Thermistor temperature high byte */
#define AMG88XX_REG_INT0        (0x10)  /**< Interrupt table byte 0 (pixels 0-7) */
#define AMG88XX_REG_INT1        (0x11)  /**< Interrupt table byte 1 (pixels 8-15) */
#define AMG88XX_REG_INT2        (0x12)  /**< Interrupt table byte 2 (pixels 16-23) */
#define AMG88XX_REG_INT3        (0x13)  /**< Interrupt table byte 3 (pixels 24-31) */
#define AMG88XX_REG_INT4        (0x14)  /**< Interrupt table byte 4 (pixels 32-39) */
#define AMG88XX_REG_INT5        (0x15)  /**< Interrupt table byte 5 (pixels 40-47) */
#define AMG88XX_REG_INT6        (0x16)  /**< Interrupt table byte 6 (pixels 48-55) */
#define AMG88XX_REG_INT7        (0x17)  /**< Interrupt table byte 7 (pixels 56-63) */
#define AMG88XX_REG_T01L        (0x80)  /**< Pixel 0 temperature low byte (start of pixel data) */
/** @} */

/**
 * @name    Power control register values
 * @{
 */
#define AMG88XX_PCTL_NORMAL_MODE        (0x00)  /**< Normal operating mode */
#define AMG88XX_PCTL_SLEEP_MODE         (0x10)  /**< Sleep mode */
#define AMG88XX_PCTL_STANDBY_MODE_60S   (0x20)  /**< Standby with 60s intermittence */
#define AMG88XX_PCTL_STANDBY_MODE_10S   (0x21)  /**< Standby with 10s intermittence */
/** @} */

/**
 * @name    Reset register values
 * @{
 */
#define AMG88XX_RST_FLAG_RESET      (0x30)  /**< Flag reset (clear status/interrupt flags) */
#define AMG88XX_RST_INITIAL_RESET   (0x3F)  /**< Initial reset (return to defaults) */
/** @} */

/**
 * @name    Frame rate register values
 * @{
 */
#define AMG88XX_FPSC_FPS            (0x01)  /**< Frame rate 1 FPS bit */
/** @} */

/**
 * @name    Interrupt control register bit masks
 * @{
 */
#define AMG88XX_INTC_INTEN          (0x00)  /**< Interrupt control: interrupt disabled */
#define AMG88XX_INTC_INTMOD         (0x01)  /**< Interrupt control: difference mode */
#define AMG88XX_INTC_INTEN_BIT      (0x01)  /**< Interrupt enable bit */
#define AMG88XX_INTC_INTMOD_BIT     (0x02)  /**< Interrupt mode bit (0=difference, 1=absolute) */
/** @} */

/**
 * @name    Status register bit masks
 * @{
 */
#define AMG88XX_STAT_INTF           (0x02)  /**< Interrupt occurrence flag */
#define AMG88XX_STAT_OVF_THS        (0x04)  /**< Thermistor temperature overflow flag */
#define AMG88XX_STAT_OVF_IRS        (0x08)  /**< Pixel temperature overflow flag */
/** @} */

/**
 * @name    Status clear register bit masks
 * @{
 */
#define AMG88XX_SCLR_INT_CLR        (0x02)  /**< Clear interrupt flag */
#define AMG88XX_SCLR_OVS_CLR        (0x04)  /**< Clear thermistor overflow flag */
#define AMG88XX_SCLR_OVT_CLR        (0x08)  /**< Clear pixel overflow flag */
/** @} */

/**
 * @name    Average register bit masks
 * @{
 */
#define AMG88XX_AVE_MAMOD_BIT       (0x20)  /**< Twice moving average mode enable bit */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
