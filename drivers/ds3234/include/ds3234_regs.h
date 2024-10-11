/*
 * Copyright 2018 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ds3234
 * @{
 *
 * @file
 * @brief       Register map for the DS323x driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef DS3234_REGS_H
#define DS3234_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Register addresses
 * @{
 */
enum {
    DS323X_REG_TIME_SECONDS     = (0x00),
    DS323X_REG_TIME_MINUTES     = (0x01),
    DS323X_REG_TIME_HOURS       = (0x02),
    DS323X_REG_TIME_WEEKDAY     = (0x03),
    DS323X_REG_TIME_DATE        = (0x04),
    DS323X_REG_TIME_MONTH       = (0x05),
    DS323X_REG_TIME_YEAR        = (0x06),
    DS323X_REG_ALARM1_SECONDS   = (0x07),
    DS323X_REG_ALARM1_MINUTES   = (0x08),
    DS323X_REG_ALARM1_HOURS     = (0x09),
    DS323X_REG_ALARM1_DAYDATE   = (0x0A),
    DS323X_REG_ALARM2_MINUTES   = (0x0B),
    DS323X_REG_ALARM2_HOURS     = (0x0C),
    DS323X_REG_ALARM2_DAYDATE   = (0x0D),
    DS323X_REG_CONTROL          = (0x0E),
    DS323X_REG_CONTROL_STATUS   = (0x0F),
    DS323X_REG_XTAL_AGING       = (0x10),
    DS323X_REG_TEMP_MSB         = (0x11),
    DS323X_REG_TEMP_LSB         = (0x12),
    DS323X_REG_TEMP_DISABLE     = (0x13),
    DS323X_REG_SRAM_ADDR        = (0x18),
    DS323X_REG_SRAM_DATA        = (0x19),
};
/** @} */

/**
 * @name    Control register bits
 * @{
 */
#define DS323X_REG_CONTROL_EOSC_MASK    (0x80) /**< Enable oscillator */
#define DS323X_REG_CONTROL_BBSQW_MASK   (0x40) /**< Battery-Backed Square-Wave Enable */
#define DS323X_REG_CONTROL_CONV_MASK    (0x20) /**< Convert Temperature */
#define DS323X_REG_CONTROL_RS2_MASK     (0x10) /**< Rate Select 2 */
#define DS323X_REG_CONTROL_RS1_MASK     (0x08) /**< Rate Select 1 */
#define DS323X_REG_CONTROL_INTCN_MASK   (0x04) /**< Interrupt Control */
#define DS323X_REG_CONTROL_A2IE_MASK    (0x02) /**< Alarm 2 Interrupt Enable */
#define DS323X_REG_CONTROL_A1IE_MASK    (0x01) /**< Alarm 1 Interrupt Enable */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DS3234_REGS_H */
