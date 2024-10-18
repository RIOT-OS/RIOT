/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bq2429x
 *
 * @{
 * @file
 * @brief       Internal address, registers, constants for the BQ2429x family
 *              power ICs.
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef BQ2429X_INTERNAL_H
#define BQ2429X_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   BQ2429x I2C device slave address
 */
#define BQ2429X_I2C_ADDR            (0x6B)

/**
 * @brief   Registers
 * @{
 */
#define BQ2429X_REG00               (0x00)  /**< Input Source Control */
#define BQ2429X_REG01               (0x01)  /**< Power-On Configuration */
#define BQ2429X_REG02               (0x02)  /**< Charge Current Control */
#define BQ2429X_REG04               (0x04)  /**< Charge Voltage Control */
#define BQ2429X_REG05               (0x05)  /**< Charge Termination/Timer
                                                 Control */
#define BQ2429X_REG08               (0x08)  /**< System Status */
#define BQ2429X_REG09               (0x09)  /**< New Fault Register */
#define BQ2429X_REG0A               (0x0A)  /**< Vendor / Part / Revision
                                                 Status */
/** @} */

/**
 * @brief   Register values
 * @{
 */
#define BQ2429X_REG00_VINDPM_m      (0x78)
#define BQ2429X_REG00_VINDPM_s      (3)
#define BQ2429X_REG00_IINLIM_m      (0x07)
#define BQ2429X_REG00_IINLIM_s      (0)

#define BQ2429X_REG01_OTG_m         (0x20)
#define BQ2429X_REG01_OTG_s         (5)
#define BQ2429X_REG01_OTG_ENABLE    (1)
#define BQ2429X_REG01_OTG_DISABLE   (0)
#define BQ2429X_REG01_CHG_m         (0x10)
#define BQ2429X_REG01_CHG_s         (4)
#define BQ2429X_REG01_CHG_ENABLE    (1)
#define BQ2429X_REG01_CHG_DISABLE   (0)

#define BQ2429X_REG02_ICHG_m        (0xFC)
#define BQ2429X_REG02_ICHG_s        (2)

#define BQ2429X_REG04_VREG_m        (0xFC)
#define BQ2429X_REG04_VREG_s        (2)

#define BQ2429X_REG05_WD_m          (0x30)
#define BQ2429X_REG05_WD_s          (4)
#define BQ2429X_REG05_WD_DISABLE    (0x00)

#define BQ2429X_REG08_VBUS_STATUS_m (0xC0)
#define BQ2429X_REG08_VBUS_STATUS_s (6)
#define BQ2429X_REG08_CHG_STAT_m    (0x30)
#define BQ2429X_REG08_CHG_STAT_s    (4)
#define BQ2429X_REG08_DPM_STAT_m    (0x08)
#define BQ2429X_REG08_DPM_STAT_s    (3)
#define BQ2429X_REG08_PG_STAT_m     (0x04)
#define BQ2429X_REG08_PG_STAT_s     (2)
#define BQ2429X_REG08_THERM_STAT_m  (0x02)
#define BQ2429X_REG08_THERM_STAT_s  (1)
#define BQ2429X_REG08_VSYS_STAT_m   (0x01)
#define BQ2429X_REG08_VSYS_STAT_s   (0)

#define BQ2429X_REG09_WD_FAULT_m    (0x80)
#define BQ2429X_REG09_WD_FAULT_s    (7)
#define BQ2429X_REG09_OTG_FAULT_m   (0x40)
#define BQ2429X_REG09_OTG_FAULT_s   (6)
#define BQ2429X_REG09_CHRG_FAULT_m  (0x30)
#define BQ2429X_REG09_CHRG_FAULT_s  (4)
#define BQ2429X_REG09_BAT_FAULT_m   (0x08)
#define BQ2429X_REG09_BAT_FAULT_s   (3)
#define BQ2429X_REG09_NTC_FAULT_1_m (0x02)
#define BQ2429X_REG09_NTC_FAULT_1_s (1)
#define BQ2429X_REG09_NTC_FAULT_0_m (0x01)
#define BQ2429X_REG09_NTC_FAULT_0_s (0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BQ2429X_INTERNAL_H */
