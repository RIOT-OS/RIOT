/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vcnl40x0
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the VCNL40X0 devices.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef VCNL40X0_INTERNALS_H
#define VCNL40X0_INTERNALS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief VCNL40X0 I2C address
 */
#define VCNL40X0_ADDR                                (0x13)

/**
 * @brief VCNL40X0 product ID
 */
#define VCNL40X0_PRODUCT_ID                          (0x20)

/**
 * @name VCNL40X0 registers
 * @{
 */
#define VCNL40X0_REG_COMMAND                         (0x80)
#define VCNL40X0_REG_PRODUCT_ID                      (0x81)
#define VCNL40X0_REG_PROXIMITY_RATE                  (0x82)
#define VCNL40X0_REG_PROXIMITY_CURRENT               (0x83)
#define VCNL40X0_REG_AMBIENT_PARAMETER               (0x84)
#define VCNL40X0_REG_AMBIENT_VALUE                   (0x85)
#define VCNL40X0_REG_PROXIMITY_VALUE                 (0x87)
#define VCNL40X0_REG_INTERRUPT_CONTROL               (0x89)
#define VCNL40X0_REG_INTERRUPT_LOW_THRES             (0x8a)
#define VCNL40X0_REG_HIGH_THRES                      (0x8c)
#define VCNL40X0_REG_INTERRUPT_STATUS                (0x8e)
#define VCNL40X0_REG_PROX_TIMING                     (0xf9)
#define VCNL40X0_REG_AMBIENT_IR_LIGHT_LEVEL          (0x90)  /* should not be used */
/** @} */

/**
 * @name VCNL40X0 command register constants
 * @{
 */
#define VCNL40X0_COMMAND_ALL_DISABLE                 (0x00)
#define VCNL40X0_COMMAND_SELFTIMED_MODE_ENABLE       (0x01)
#define VCNL40X0_COMMAND_PROX_ENABLE                 (0x02)
#define VCNL40X0_COMMAND_AMBI_ENABLE                 (0x04)
#define VCNL40X0_COMMAND_PROX_ON_DEMAND              (0x08)
#define VCNL40X0_COMMAND_AMBI_ON_DEMAND              (0x10)
#define VCNL40X0_COMMAND_MASK_PROX_DATA_READY        (0x20)
#define VCNL40X0_COMMAND_MASK_AMBI_DATA_READY        (0x40)
#define VCNL40X0_COMMAND_MASK_LOCK                   (0x80)
/** @} */

/**
 * @name VCNL40X0 product ID register constants
 * @{
 */
#define VCNL40X0_PRODUCT_MASK_REVISION_ID            (0x0f)
#define VCNL40X0_PRODUCT_MASK_PRODUCT_ID             (0xf0)
/** @} */

/**
 * @name VCNL40X0 proximity rate register constants
 * @{
 */
#define VCNL40X0_PROXIMITY_MASK_MEASUREMENT_RATE     (0x07)
#define VCNL40X0_PROXIMITY_MASK_LED_CURRENT          (0x3f)
#define VCNL40X0_PROXIMITY_MASK_FUSE_PROG_ID         (0xc0)
/** @} */

/**
 * @name VCNL40X0 ambient light parameter register constants
 * @{
 */
#define VCNL40X0_AMBIENT_MASK_PARA_AVERAGE           (0x07)
#define VCNL40X0_AMBIENT_PARA_AUTO_OFFSET_ENABLE     (0x08)
#define VCNL40X0_AMBIENT_MASK_PARA_AUTO_OFFSET       (0x08)
#define VCNL40X0_AMBIENT_MASK_PARA_MEAS_RATE         (0x70)
#define VCNL40X0_AMBIENT_PARA_CONT_CONV_ENABLE       (0x80)
#define VCNL40X0_AMBIENT_MASK_PARA_CONT_CONV         (0x80)
/** @} */

/**
 * @name VCNL40X0 interrupt control register constants
 * @{
 */
#define VCNL40X0_INTERRUPT_THRES_SEL_PROX            (0x00)
#define VCNL40X0_INTERRUPT_THRES_SEL_ALS             (0x01)
#define VCNL40X0_INTERRUPT_THRES_ENABLE              (0x02)
#define VCNL40X0_INTERRUPT_ALS_READY_ENABLE          (0x04)
#define VCNL40X0_INTERRUPT_PROX_READY_ENABLE         (0x08)
#define VCNL40X0_INTERRUPT_COUNT_EXCEED_1            (0x00)
#define VCNL40X0_INTERRUPT_COUNT_EXCEED_2            (0x20)
#define VCNL40X0_INTERRUPT_COUNT_EXCEED_4            (0x40)
#define VCNL40X0_INTERRUPT_COUNT_EXCEED_8            (0x60)
#define VCNL40X0_INTERRUPT_COUNT_EXCEED_16           (0x80)
#define VCNL40X0_INTERRUPT_COUNT_EXCEED_32           (0xa0)
#define VCNL40X0_INTERRUPT_COUNT_EXCEED_64           (0xc0)
#define VCNL40X0_INTERRUPT_COUNT_EXCEED_128          (0xe0)
#define VCNL40X0_INTERRUPT_MASK_COUNT_EXCEED         (0xe0)
/** @} */

/**
 * @name VCNL40X0 interrupt status register constants
 * @{
 */
#define VCNL40X0_INTERRUPT_STATUS_THRES_HI           (0x01)
#define VCNL40X0_INTERRUPT_STATUS_THRES_LO           (0x02)
#define VCNL40X0_INTERRUPT_STATUS_ALS_READY          (0x04)
#define VCNL40X0_INTERRUPT_STATUS_PROX_READY         (0x08)
#define VCNL40X0_INTERRUPT_MASK_STATUS_THRES_HI      (0x01)
#define VCNL40X0_INTERRUPT_MASK_THRES_LO             (0x02)
#define VCNL40X0_INTERRUPT_MASK_ALS_READY            (0x04)
#define VCNL40X0_INTERRUPT_MASK_PROX_READY           (0x08)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* VCNL40X0_INTERNALS_H */
/** @} */
