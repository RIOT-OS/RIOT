/*
 * Copyright (C) 2019 Noel Le Moult <noel.lemoult@dfxlab.fr>
 * Copyright (C) 2019 William MARTIN <william.martin@power-lan.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tca6507
 * @brief       Internal definitions for TCA6507 I2C Led controller
 * @author      Noel Le Moult <noel.lemoult@dfxlab.fr>
 * @author      William MARTIN <william.martin@power-lan.com>
 * @file
 * @{
 */

#ifndef TCA6507_INTERNALS_H
#define TCA6507_INTERNALS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TCA6507 chip addresses.
 */
#define TCA6507_I2C_ADDRESS         (0x45)

/**
 * @name TCA6507 device commands.
 * @{
 */
#define TCA6507_COMMAND_AUTOINC            (0x10)
#define TCA6507_SELECT0                    (0x00)
#define TCA6507_SELECT1                    (0x01)
#define TCA6507_SELECT2                    (0x02)
#define TCA6507_FADEON_TIME                (0x03)
#define TCA6507_FULLYON_TIME               (0x04)
#define TCA6507_FADEOFF_TIME               (0x05)
#define TCA6507_FIRST_FULLYOFF_TIME        (0x06)
#define TCA6507_SEC_FULLYOFF_TIME          (0x07)
#define TCA6507_MAX_INTENSITY              (0x08)
#define TCA6507_ONESHOT_MASTER_INTENSITY   (0x09)
#define TCA6507_INIT                       (0x0A)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* TCA6507_INTERNALS_H */
/** @} */
