/*
 * Copyright (C) 2023 TU Braunschweig Institut für Betriebssysteme und Rechnerverbund
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SEN5X_CONSTANTS_H
#define SEN5X_CONSTANTS_H

/**
 * @ingroup     drivers_sen5x
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Daniel Prigoshij <prigoshi@ibr.cs.tu-bs.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SEN5X_ADRESS_I2C
/**
 * @brief     SEN5x I2C address
 */
#define SEN5X_ADRESS_I2C            (0x69)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SEN5X_CONSTANTS_H */
