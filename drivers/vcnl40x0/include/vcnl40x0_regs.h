/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vcnl40x0
 * @brief       Register definitions for Vishay VCNL40X0 proximity and ambient light sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef VCNL40X0_REGS_H
#define VCNL40X0_REGS_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Register addresses
 * @{
 */
#define VCNL40X0_REG_COMMAND          (0x80) /**< Command */
#define VCNL40X0_REG_PROD_REV_ID      (0x81) /**< Product and Revision ID */
#define VCNL40X0_REG_PRX_RATE         (0x82) /**< Rate of Proximity Measurement */
#define VCNL40X0_REG_IR_LED_CURR      (0x83) /**< LED Current Setting for Proximity Mode */
#define VCNL40X0_REG_ALS_PARAM        (0x84) /**< Ambient Light Parameter */
#define VCNL40X0_REG_ALS_RES_H        (0x85) /**< Ambient Light Result */
#define VCNL40X0_REG_ALS_RES_L        (0x86) /**< Ambient Light Result low byte */
#define VCNL40X0_REG_PRX_RES_H        (0x87) /**< Proximity Measurement Result */
#define VCNL40X0_REG_PRX_RES_L        (0x88) /**< Proximity Measurement Result */
#define VCNL40X0_REG_INT_CTRL         (0x89) /**< Interrupt Control */
#define VCNL40X0_REG_LOW_THRESH_H     (0x8a) /**< Low Threshold */
#define VCNL40X0_REG_LOW_THRESH_L     (0x8b) /**< Low Threshold */
#define VCNL40X0_REG_HIGH_THRESH_H    (0x8c) /**< High Threshold */
#define VCNL40X0_REG_HIGH_THRESH_L    (0x8d) /**< High Threshold */
#define VCNL40X0_REG_INT_STATUS       (0x8e) /**< Interrupt Status */
#define VCNL40X0_REG_PRX_MOD_ADJ      (0x8f) /**< Proximity Modulator Timing Adjustment */
/** @} */

/**
 * @name    Register structure definitions
 * @{
 */

/* Command Register (VCNL40X0_REG_COMMAND) */
#define VCNL40X0_REG_CFG_LOCK       (0x80)  /**< config lock */
#define VCNL40X0_REG_ALS_DATA_RDY   (0x40)  /**< ambient light measurement data available */
#define VCNL40X0_REG_PRX_DATA_RDY   (0x20)  /**< proximity measurement data available */
#define VCNL40X0_REG_ALS_OD         (0x10)  /**< on-demand ambient light measurement */
#define VCNL40X0_REG_PRX_OD         (0x08)  /**< on-demand proximity measurement */
#define VCNL40X0_REG_ALS_EN         (0x04)  /**< periodic ambient light measurement */
#define VCNL40X0_REG_PRX_EN         (0x02)  /**< periodic proximity measurement */
#define VCNL40X0_REG_SELFTIMED_EN   (0x01)  /**< selftimed measurement */

/* Product and Revision ID (VCNL40X0_REG_PROD_REV_ID) */
#define VCNL40X0_REG_PROD_ID        (0xf0)  /**< Product ID */
#define VCNL40X0_REG_REV            (0x0f)  /**< Revision ID */

/* Proximity Rate Register (VCNL40X0_REG_PRX_RATE) */
#define VCNL40X0_REG_PRX_RATE_M     (0x07)  /**< Rate of Proximity */

/* Ambient Light Parameter Register (VCNL40X0_REG_ALS_PARAM) */
#define VCNL40X0_REG_CONT_CONV      (0x80)  /**< Continuous conversion mode */
#define VCNL40X0_REG_ALS_RATE       (0x70)  /**< Ambient light measurement rate */
#define VCNL40X0_REG_AUTO_OFFSET    (0x08)  /**< Automatic offset compensation */
#define VCNL40X0_REG_AVG_FUNC       (0x07)  /**< Averaging function */

/* LED Current Setting for Proximity Mode Register (VCNL40X0_REG_IR_LED_CURR) */
#define VCNL40X0_REG_IR_LED_CURR_M  (0x3f)  /**< IR LED current value */

/* Proximity Modulator Timing Adjustment Register (VCNL40X0_REG_PRX_MOD_ADJ) */
#define VCNL40X0_REG_MOD_DELAY      (0xe0)  /**< Modulation delay time */
#define VCNL40X0_REG_PRX_FREQ       (0x18)  /**< Proximity frequency */
#define VCNL40X0_REG_MOD_DEAD       (0x07)  /**< Modulation dead time */

/* Interrupt Control Register (VCNL40X0_REG_INT_CTRL) */
#define VCNL40X0_REG_INT_CNT_EXC    (0xe0)  /**< Int count exceed */
#define VCNL40X0_REG_INT_PRX_DRDY_E (0x08)  /**< Proximity data ready enabled */
#define VCNL40X0_REG_INT_ALS_DRDY_E (0x04)  /**< Ambient light data ready enabled */
#define VCNL40X0_REG_INT_THS_E      (0x02)  /**< threshold exceeded enabled*/
#define VCNL40X0_REG_INT_THS_SEL    (0x01)  /**< threshold selection */

/* Interrupt Status Register (VCNL40X0_REG_INT_STATUS) */
#define VCNL40X0_REG_INT_PRX_DRDY   (0x08)  /**< Proximity data ready */
#define VCNL40X0_REG_INT_ALS_DRDY   (0x04)  /**< Ambient light data ready */
#define VCNL40X0_REG_INT_THS_LO     (0x02)  /**< Low threshold exceeded */
#define VCNL40X0_REG_INT_THS_HI     (0x01)  /**< High threshold exceeded */
#define VCNL40X0_REG_INT_ALL        (VCNL40X0_REG_INT_PRX_DRDY | \
                                     VCNL40X0_REG_INT_ALS_DRDY | \
                                     VCNL40X0_REG_INT_THS_LO | \
                                     VCNL40X0_REG_INT_THS_HI) \

/* Proximity Modulator Timing Adjustment Register (VCNL40X0_REG_PRX_MOD_ADJ) */
#define VCNL40X0_REG_PRX_DELAY      (0xe0)  /**< Modulation delay time */
#define VCNL40X0_REG_PRX_FREQ       (0x18)  /**< Proximity frequency */
#define VCNL40X0_REG_PRX_DEAD       (0x07)  /**< Modulation dead time */

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* VCNL40X0_REGS_H */
/** @} */
