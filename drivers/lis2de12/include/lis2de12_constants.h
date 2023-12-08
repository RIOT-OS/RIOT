/*
 * Copyright (C) 2023 LIG
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lis2de12
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Léo <leo.cordier@etu.esisar.grenoble-inp.fr>
 */

#ifndef LIS2DE12_CONSTANTS_H
#define LIS2DE12_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* define here the addresses, register and constants of the driver */
#define LIS2DE12_I2C_ADDRESS    (0x19)
#define LIS2DE12_I2C_DEV        I2C_DEV(0)




#ifdef __cplusplus
}
#endif

#endif /* LIS2DE12_CONSTANTS_H */
/** @} */
