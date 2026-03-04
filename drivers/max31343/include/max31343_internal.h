/*
 * SPDX-FileCopyrightText: 2026 Jakob Müller <ja.mueller@tuhh.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       MAX31343 internal register map and bit field definitions
 *
 * @author      Jakob Müller <ja.mueller@tuhh.de>
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Fixed I2C address */
#define MAX31343_I2C_ADDR           (0x68U)

/* Status/Config registers */
#define MAX31343_REG_STATUS         (0x00U)
#define MAX31343_REG_INT_EN         (0x01U)
#define MAX31343_REG_RTC_RESET      (0x02U)
#define MAX31343_REG_RTC_CFG1       (0x03U)
#define MAX31343_REG_RTC_CFG2       (0x04U)
#define MAX31343_REG_TIMER_CFG      (0x05U)
#define MAX31343_REG_TEMP_MSB       (0x1AU)
#define MAX31343_REG_TEMP_LSB       (0x1BU)
#define MAX31343_REG_TS_CONFIG      (0x1CU)

/* Time register block 0x06..0x0C */
#define MAX31343_REG_TIME0          (0x06U)
#define MAX31343_TIME_LEN           (7U)

/* RTC_config1 bits */
#define MAX31343_RTC_CFG1_ENOSC     (1U << 1)

/* RTC_config2 fields */
#define MAX31343_RTC_CFG2_SQW_HZ_MASK   (0x07U)

/* Month register bit */
#define MAX31343_MONTH_CENTURY      (1U << 7)

/* TS_Config bits */
#define MAX31343_TS_AUTOMODE        (1U << 7)
#define MAX31343_TS_TTSINT_MASK     (0x38U)
#define MAX31343_TS_TTSINT_SHIFT    (3U)

/* Alarm 1 registers (0x0D..0x12) */
#define MAX31343_REG_ALM1_SEC       (0x0DU)
#define MAX31343_REG_ALM1_MIN       (0x0EU)
#define MAX31343_REG_ALM1_HRS       (0x0FU)
#define MAX31343_REG_ALM1_DAY_DATE  (0x10U)
#define MAX31343_REG_ALM1_MON       (0x11U)
#define MAX31343_REG_ALM1_YEAR      (0x12U)
#define MAX31343_ALM1_LEN           (6U)

/* Alarm bits */
#define MAX31343_ALM_MASK_BIT       (1U << 7)
#define MAX31343_ALM_DY_DT          (1U << 6)

/* Status flags (Status reg 0x00) */
#define MAX31343_STATUS_A1F         (1U << 0)
#define MAX31343_STATUS_A2F         (1U << 1)
#define MAX31343_STATUS_OSF         (1U << 6)

/* Interrupt enable bits (Int_en reg 0x01) */
#define MAX31343_INTEN_A1IE         (1U << 0)
#define MAX31343_INTEN_A2IE         (1U << 1)

/* Trickle charger register (0x19) */
#define MAX31343_REG_TRICKLE        (0x19U)

/* TCHE field: bits [7:4] — only 0x5 enables the trickle charger */
#define MAX31343_TRICKLE_TCHE_SHIFT (4U)
#define MAX31343_TRICKLE_TCHE_MASK  (0xF0U)
#define MAX31343_TRICKLE_TCHE_ENABLE (0x5U)

/* D_TRICKLE field: bits [3:0] */
#define MAX31343_TRICKLE_D_MASK     (0x0FU)

/* D_TRICKLE bit 3: charger path select (0 = ON, 1 = OFF) */
#define MAX31343_TRICKLE_D_CHARGER_OFF (1U << 3)

/* D_TRICKLE bit 2: diode select (0 = without diode, 1 = with diode) */
#define MAX31343_TRICKLE_D_DIODE    (1U << 2)

/* D_TRICKLE bits [1:0]: resistor select */
#define MAX31343_TRICKLE_D_RES_MASK (0x03U)

#ifdef __cplusplus
}
#endif
