/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     drivers_feetech
 *
 * @{
 *
 * @file
 * @brief       Feetech protocol definitions
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */


#ifdef __cplusplus
extern "C" {
#endif

#define FEETECH_START (0xFF)

typedef enum {
    SCS15_B_1M     = 0,
    SCS15_B_0_5M   = 1,
    SCS15_B_250K   = 2,
    SCS15_B_128K   = 3,
    SCS15_B_115200 = 4,
    SCS15_B_76800  = 5,
    SCS15_B_57600  = 6,
    SCS15_B_38400  = 7
} scs15_baudrate_t;

typedef enum {
    SCS15_ID                     = 5,
    SCS15_BAUD_RATE              = 6,
    SCS15_RETURN_DELAY_TIME      = 7,
    SCS15_RETURN_LEVEL           = 8,
    SCS15_LIMIT_TEMPERATURE      = 13,
    SCS15_MAX_LIMIT_VOLTAGE      = 14,
    SCS15_MIN_LIMIT_VOLTAGE      = 15,
    SCS15_ALARM_LED              = 18,
    SCS15_ALARM_SHUTDOWN         = 19,
    SCS15_COMPLIANCE_P           = 21,
    SCS15_COMPLIANCE_D           = 22,
    SCS15_COMPLIANCE_I           = 23,
    SCS15_CW_DEAD                = 26,
    SCS15_CCW_DEAD               = 27,
    SCS15_TORQUE_ENABLE          = 40,
    SCS15_LED                    = 41,
    SCS15_LOCK                   = 48,
    SCS15_PRESENT_VOLTAGE        = 62,
    SCS15_PRESENT_TEMPERATURE    = 63,
    SCS15_REGISTERED_INSTRUCTION = 64,
    SCS15_ERROR                  = 65,
    SCS15_MOVING                 = 66,
} scs15_register8_t;

typedef enum {
    SCS15_MODEL_NUMBER         = 0,
    SCS15_VERSION              = 3,
    SCS15_MIN_ANGLE_LIMIT      = 9,
    SCS15_MAX_ANGLE_LIMIT      = 11,
    SCS15_MAX_TORQUE           = 16,
    SCS15_PUNCH                = 24,
    SCS15_IMAX                 = 28,
    SCS15_OFFSET               = 30,
    SCS15_GOAL_POSITION        = 42,
    SCS15_GOAL_TIME            = 44,
    SCS15_GOAL_SPEED           = 46,
    SCS15_PRESENT_POSITION     = 56,
    SCS15_PRESENT_SPEED        = 58,
    SCS15_PRESENT_LOAD         = 60,
    SCS15_VIR_POSITION         = 67,
    SCS15_CURRENT              = 69,
} scs15_register16_t;

typedef enum {
    INST_PING       = 0x01,
    INST_READ       = 0x02,
    INST_WRITE      = 0x03,
    INST_REG_WRITE  = 0x04,
    INST_ACTION     = 0x05,
    INST_RESET      = 0x06,
    INST_SYNC_WRITE = 0x83,
} feetech_intruction_t;

#ifdef __cplusplus
}
#endif

/** @} */
