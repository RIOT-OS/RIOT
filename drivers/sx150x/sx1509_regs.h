/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx150x
 * @{
 *
 * @file
 * @brief       Register definitions for SX150X devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SX150X_REGS_H
#define SX150X_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

#define OFFSET(pin)             ((pin > 7) ? 0 : 1)

#define REG_INPUT_DISABLE(pin)  (0x00 + OFFSET(pin))
#define REG_DIR(pin)            (0x0e + OFFSET(pin))
#define REG_DATA(pin)           (0x10 + OFFSET(pin))

#define REG_RESET               (0x7d)

#define RESET_SEQ_1             (0x12)
#define RESET_SEQ_2             (0x34)

#ifdef __cplusplus
}
#endif

#endif /* SX150X_REGS_H */
/** @} */

