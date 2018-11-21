/*
 * Copyright (C) 2017 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_adxl345
 * @{
 *
 * @file
 * @brief       Register and bit definitions for the ADXL345
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef ADXL345_REGS_H
#define ADXL345_REGS_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    Register addresses
 * @{
 */
#define ADXL345_CHIP_ID_REG    (0x00) /**< Device ID */
#define ADXL345_THRESH_TAP     (0x1D) /**< Tap threshold */
#define ADXL345_OFFSET_X       (0x1E) /**< X-axis offset */
#define ADXL345_OFFSET_Y       (0x1F) /**< Y-axis offset */
#define ADXL345_OFFSET_Z       (0x20) /**< Z-axis offset */
#define ADXL345_TAP_DUR        (0x21) /**< Tap duration */
#define ADXL345_TAP_LAT        (0x22) /**< Tap latency */
#define ADXL345_TAP_WIN        (0x23) /**< Tap window */
#define ADXL345_THRESH_ACT     (0x24) /**< Activity threshold */
#define ADXL345_THRESH_INACT   (0x25) /**< Inactivity threshold */
#define ADXL345_TIME_INACT     (0x26) /**< Inactivity time */
#define ADXL345_ACT_INACT_CTL  (0x27) /**< Axis enable control for activity and inactivity detection */
#define ADXL345_THRESH_FF      (0x28) /**< Free-fall threshold */
#define ADXL345_TIME_FF        (0x29) /**< Free-fall time */
#define ADXL345_TAP_AXES       (0x2A) /**< Axis control for single tap/double tap */
#define ADXL345_ACT_TAP_STATUS (0x2B) /**< Source of single tap/double tap */
#define ADXL345_BW_RATE        (0x2C) /**< Data rate and power mode control */
#define ADXL345_POWER_CTL      (0x2D) /**< Power-saving features control */
#define ADXL345_INT_ENABLE     (0x2E) /**< Interrupt enable control */
#define ADXL345_INT_MAP        (0x2F) /**< Interrupt mapping control */
#define ADXL345_INT_SOURCE     (0x30) /**< Source of interrupts */
#define ADXL345_DATA_FORMAT    (0x31) /**< Data format control */
#define ADXL345_DATA_X0        (0x32) /**< X-Axis Data 0 */
#define ADXL345_DATA_X1        (0x33) /**< X-Axis Data 1 */
#define ADXL345_DATA_Y0        (0x34) /**< Y-Axis Data 0 */
#define ADXL345_DATA_Y1        (0x35) /**< Y-Axis Data 1 */
#define ADXL345_DATA_Z0        (0x36) /**< Z-Axis Data 0 */
#define ADXL345_DATA_Z1        (0x37) /**< Z-Axis Data 1 */
#define ADXL345_FIFO_CTL       (0x38) /**< FIFO control */
#define ADXL345_FIFO_STATUS    (0x39) /**< FIFO status */
/** @} */

/**
 * @name    Device ID for ADXL345
 * @{
 */
#define ADXL345_CHIP_ID        (0xE5)
/** @} */

/**
 * @name    Resolution masks for output data
 * @{
 */
#define ADXL345_RES_10_BITS       (0x03FF)
#define ADXL345_RES_11_BITS       (0x07FF)
#define ADXL345_RES_12_BITS       (0x0FFF)
#define ADXL345_RES_13_BITS       (0x1FFF)
/** @} */

/**
 * @name    bits definitions for ACT_INACT_CTL register
 * @{
 */
#define ADXL345_INACT_Z_ENABLE    (1 << 0)
#define ADXL345_INACT_Y_ENABLE    (1 << 1)
#define ADXL345_INACT_X_ENABLE    (1 << 2)
#define ADXL345_INACT_ACDC        (1 << 3)
#define ADXL345_ACT_Z_ENABLE      (1 << 4)
#define ADXL345_ACT_Y_ENABLE      (1 << 5)
#define ADXL345_ACT_X_ENABLE      (1 << 6)
#define ADXL345_ACT_ACDC          (1 << 7)
/** @} */

/**
 * @name    bits definitions for TAP_AXES register
 * @{
 */
#define ADXL345_TAP_Z_ENABLE      (1 << 0)
#define ADXL345_TAP_Y_ENABLE      (1 << 1)
#define ADXL345_TAP_X_ENABLE      (1 << 2)
#define ADXL345_SUPPRESS          (1 << 3)
#define ADXL345_TAP_ALL_ENABLE    (ADXL345_TAP_Z_ENABLE | \
                                   ADXL345_TAP_Y_ENABLE | \
                                   ADXL345_TAP_X_ENABLE)
/** @} */

/**
 * @name    bits definitions for ACT_TAP_STATUS register
 * @{
 */
#define ADXL345_TAP_Z_SRC         (1 << 0)
#define ADXL345_TAP_Y_SRC         (1 << 1)
#define ADXL345_TAP_X_SRC         (1 << 2)
#define ADXL345_ASLEEP            (1 << 3)
#define ADXL345_ACT_Z_SRC         (1 << 4)
#define ADXL345_ACT_Y_SRC         (1 << 5)
#define ADXL345_ACT_X_SRC         (1 << 6)
/** @} */

/**
 * @name    bits definitions for BW_RATE register
 * @{
 */
#define ADXL345_RATE_MASK         (0x0F)
#define ADXL345_LOWPOWER          (1 << 4)
/** @} */

/**
 * @name    bits definitions for PWR_CTL register
 * @{
 */
#define ADXL345_WAKEUP_8HZ        (0x00)
#define ADXL345_WAKEUP_4HZ        (0x01)
#define ADXL345_WAKEUP_2HZ        (0x02)
#define ADXL345_WAKEUP_1HZ        (0x03)
#define ADXL345_SLEEP_BIT         (1 << 2)
#define ADXL345_MEASURE_BIT       (1 << 3)
#define ADXL345_AUTOSLEEP_BIT     (1 << 4)
#define ADXL345_LINK_BIT          (1 << 5)
/** @} */

/**
 * @name    interrupts pins definitions for INT_ENABLE, INT_MAP and INT_SOURCE
 * registers
 * @{
 */
#define ADXL345_OVERRUN          (1 << 0)
#define ADXL345_WATERMARK        (1 << 1)
#define ADXL345_FREEFALL         (1 << 2)
#define ADXL345_INACTIVITY       (1 << 3)
#define ADXL345_ACTIVITY         (1 << 4)
#define ADXL345_DOUBLE_TAP       (1 << 5)
#define ADXL345_SINGLE_TAP       (1 << 6)
#define ADXL345_DATA_READY       (1 << 7)
/** @} */

/**
 * @name    bits definitions for DATA_FORMAT register
 * @{
 */
#define ADXL345_RANGE_MASK        (0x03)
#define ADXL345_JUSTIFY           (1 << 2)
#define ADXL345_FULL_RES          (1 << 3)
#define ADXL345_INT_INVERT        (1 << 5)
#define ADXL345_SPI_BIT           (1 << 6)
#define ADXL345_SELF_TEST         (1 << 7)
/** @} */

/**
 * @name    bits definitions for FIFO_CTL register
 * @{
 */
#define ADXL345_SAMPLES_MASK      (0x0F)
#define ADXL345_FIFO_TRIGGER_POS  (4)
#define ADXL345_FIFO_TRIGGER      (1 << ADXL345_FIFO_TRIGGER_POS)
#define ADXL345_FIFO_MODE_POS     (6)
#define ADXL345_FIFO_MODE_MASK    (0xC0)
/** @} */

/**
 * @name    bits definitions for FIFO_STATUS register
 * @{
 */
#define ADXL345_FIFO_ENTRIES_MASK (0x3F)
#define ADXL345_FIFO_TRIG         (1 << 7)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADXL345_REGS_H */
/** @} */
