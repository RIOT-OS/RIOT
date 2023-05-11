/*
 * Copyright (C) 2023 TU Braunschweig Institut für Betriebssysteme und Rechnerverbund
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sen5x
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Daniel Prigoshij <d.prigoshij@tu-braunschweig.de>
 */

#ifndef SEN5X_CONSTANTS_H
#define SEN5X_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* define here the addresses, register and constants of the driver */

#ifndef SEN5X_I2C_ADDRESS
#define SEN5X_I2C_ADDRESS            (0x69)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SEN5X_CONSTANTS_H */
/** @} */
