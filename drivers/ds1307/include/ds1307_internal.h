/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_ds1307
 * @brief
 * @{
 *
 * @file
 * @brief   Register definitions for DS1307 RTC
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef DS1307_INTERNAL_H
#define DS1307_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Registers
 * @{
 */
#define DS1307_REG_SEC              (0x00)  /**< seconds */
#define DS1307_REG_MIN              (0x01)  /**< minutes */
#define DS1307_REG_HOUR             (0x02)  /**< hours */
#define DS1307_REG_DOW              (0x03)  /**< day of week (1-7, 1 == sunday) */
#define DS1307_REG_DOM              (0x04)  /**< day of month */
#define DS1307_REG_MON              (0x05)  /**< month */
#define DS1307_REG_YEAR             (0x06)  /**< year */
#define DS1307_REG_SQW_CTL          (0x07)  /**< SQW control */
#define DS1307_REG_RAM_FIRST        (0x08)  /**< NVRAM start */
#define DS1307_REG_RAM_LAST         (0x3F)  /**< NVRAM end */
/** @} */

/**
 * @name    Register bits
 * @{
 */
#define DS1307_REG_SEC_CH           (0x80)  /**< clock halt bit */
#define DS1307_REG_SEC_MASK         (0x7f)  /**< seconds mask */
#define DS1307_REG_MIN_MASK         (0x7f)  /**< minutes mask */
#define DS1307_REG_HOUR_12H         (0x40)  /**< 12-hour (0) / 24-hour (1) mode */
#define DS1307_REG_HOUR_PM          (0x20)  /**< AM (0) / PM (1) in 12-hour mode */
#define DS1307_REG_HOUR_12H_MASK    (0x2f)  /**< hour (12-hour mode) */
#define DS1307_REG_HOUR_24H_MASK    (0x3f)  /**< hour (24-hour mode) */
#define DS1307_REG_DOW_MASK         (0x07)  /**< day of week mask */
#define DS1307_REG_DOM_MASK         (0x3f)  /**< day of month mask */
#define DS1307_REG_MON_MASK         (0x1f)  /**< month mask */
/** @} */

/**
 * @name    Custom offsets (to DS1307 registers to struct tm)
 * @{
 */
#define DS1307_DOW_OFFSET           (1)     /**< offset in days from sunday */
#define DS1307_MON_OFFSET           (1)     /**< offset in month from January */
#define DS1307_YEAR_OFFSET          (-100)  /**< offset in years from 1900 */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DS1307_INTERNAL_H */
