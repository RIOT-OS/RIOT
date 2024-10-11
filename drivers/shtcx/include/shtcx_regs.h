/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_shtcx
 *
 * @{
 * @file
 * @brief       Register definitions for SHTCX devices
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef SHTCX_REGS_H
#define SHTCX_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* @name SHTCX registers
* @{
*/
#define SHTCX_CRC                                   (0x31)  /**< crc polynomial */
#define SHTCX_MEASURE_CLOCK_STRETCHING_TEMP_HIGH    (0x7C)  /**< Clock stretching enable high*/
#define SHTCX_MEASURE_CLOCK_STRETCHING_TEMP_LOW     (0xA2)  /**< Clock stretching disable high*/
#define SHTCX_COMMAND_RESET_HIGH                    (0x80)  /**< Reset command high*/
#define SHTCX_COMMAND_RESET_LOW                     (0x5D)  /**< Reset command low*/
#define SHTCX_COMMAND_ID_HIGH                       (0xEF)  /**< Get ID command low*/
#define SHTCX_COMMAND_ID_LOW                        (0xC8)  /**< Get ID command low*/
/** @} */

/**
* @brief SHTCX default ID
*/
#define SHTCX_ID                                    (0x07)    /* ID Mask */

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* SHTCX_REGS_H */
