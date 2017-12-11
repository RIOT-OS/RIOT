/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_shtc1
 * @name        Register definition for the SHTC1 Temperature and humidity sensor
 * @{
 *
 * @file
 * @brief       Register definition for the SHTC1 Temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef SHTC1_REGS_H
#define SHTC1_REGS_H

#ifdef __cplusplus 
extern "C" {
#endif

#define SHTC1_CRC                                   0x31    /* crc polynomial */
#define SHTC1_MEASURE_CLOCK_STRETCHING_TEMP_HIGH    0x7C
#define SHTC1_MEASURE_CLOCK_STRETCHING_TEMP_LOW     0xA2
#define SHTC1_COMMAND_RESET_HIGH                    0x80
#define SHTC1_COMMAND_RESET_LOW                     0x5D
#define SHTC1_COMMAND_ID_HIGH                       0xEF
#define SHTC1_COMMAND_ID_LOW                        0xC8
#define SHTC1_ID                                    0x07    /* ID Mask */

#ifdef __cplusplus
}
#endif
#endif /* SHTC1_REGS_H */
