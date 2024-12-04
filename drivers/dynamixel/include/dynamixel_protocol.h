/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_dynamixel
 *
 * @{
 *
 * @file
 * @brief       Dynamixel protocol definitions
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#ifndef DYNAMIXEL_PROTOCOL_H
#define DYNAMIXEL_PROTOCOL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DXL_HEADER ((uint8_t[]){0xFF,0xFF,0xFD})

typedef enum {
    XL320_B_9600    = 0, /**< XL320 available baudrate : 9600 */
    XL320_B_57600   = 1, /**< XL320 available baudrate : 57600 */
    XL320_B_115200  = 2, /**< XL320 available baudrate : 115200 */
    XL320_B_1000000 = 3, /**< XL320 available baudrate : 1000000 */
} xl320_baudrate_t;

typedef enum {
    XL320_VERSION             =  2, /**< Information on the version of firmware [R] */
    XL320_ID                  =  3, /**< ID of Dynamixel [RW] (default=1 ; min=0 ; max=252) */
    XL320_BAUD_RATE           =  4, /**< Baud Rate of Dynamixel [RW] (default=3 ; min=0 ; max=3) */
    XL320_RETURN_DELAY_TIME   =  5, /**< Return Delay Time [RW] (default=250 ; min=0 ; max=254) */
    XL320_CONTROL_MODE        = 11, /**< Control Mode [RW] (default=2 ; min=1 ; max=2) */
    XL320_LIMIT_TEMPERATURE   = 12, /**< Internal Limit Temperature [RW] (default=65 ; min=0 ; max=150) */
    XL320_LOWER_LIMIT_VOLTAGE = 13, /**< Lowest Limit Voltage [RW] (default=60 ; min=50 ; max=250) */
    XL320_UPPER_LIMIT_VOLTAGE = 14, /**< Upper Limit Voltage [RW] (default=90 ; min=50 ; max=250) */
    XL320_RETURN_LEVEL        = 17, /**< Return Level [RW] (default=2 ; min=0 ; max=2) */
    XL320_ALARM_SHUTDOWN      = 18, /**< Shutdown for Alarm [RW] (default=3 ; min=0 ; max=7) */
    XL320_TORQUE_ENABLE       = 24, /**< Torque On/Off [RW] (default=0 ; min=0 ; max=1) */
    XL320_LED                 = 25, /**< LED On/Off [RW] (default=0 ; min=0 ; max=7) */
    XL320_D_GAIN              = 27, /**< D Gain [RW] (default=0 ; min=0 ; max=254) */
    XL320_I_GAIN              = 28, /**< I Gain [RW] (default=0 ; min=0 ; max=254) */
    XL320_P_GAIN              = 29, /**< P Gain [RW] (default=32 ; min=0 ; max=254) */
    XL320_PRESENT_VOLTAGE     = 45, /**< Current Voltage [R] */
    XL320_PRESENT_TEMPERATURE = 46, /**< Present temperature [R] */
    XL320_REGISTERED_INST     = 47, /**< Registered Instruction [R] (default=0) */
    XL320_MOVING              = 49, /**< Moving [R] (default=0) */
    XL320_ERROR               = 50, /**< Hardware error status [R] (default=0) */
} xl320_register8_t;

typedef enum {
    XL320_MODEL_NUMBER     =  0, /**< Model number [R] (default=350) */
    XL320_CW_ANGLE_LIMIT   =  6, /**< clockwise Angle Limit [RW] (default=0 ; min=0 ; max=1023) */
    XL320_CCW_ANGLE_LIMIT  =  8, /**< counterclockwise Angle Limit [RW] (default=1023 ; min=0 ; max=1023) */
    XL320_MAX_TORQUE       = 15, /**< Lowest byte of Max. Torque [RW] (default=1023 ; min=0 ; max=1023) */
    XL320_GOAL_POSITION    = 30, /**< Goal Position [RW] (min=0 ; max=1023) */
    XL320_GOAL_VELOCITY    = 32, /**< Goal Speed [RW] (min=0 ; max=2047) */
    XL320_GOAL_TORQUE      = 35, /**< Goal Torque [RW] (min=0 ; max=1023) */
    XL320_PRESENT_POSITION = 37, /**< Current Position [R] */
    XL320_PRESENT_SPEED    = 39, /**< Current Speed [R] */
    XL320_PRESENT_LOAD     = 41, /**< Current Load [R] */
    XL320_PUNCH            = 51, /**< Punch [RW] (default=32 ; min=0 ; max=1023) */
} xl320_register16_t;

typedef enum {
    DXL_INST_PING          = 0x01, /**< checks if ID is associated to a Device */
    DXL_INST_READ          = 0x02, /**< read data from the Device */
    DXL_INST_WRITE         = 0x03, /**< write data on the Device */
    DXL_INST_REG_WRITE     = 0x04, /**< registers the write instruction to a standby status */
    DXL_INST_ACTION        = 0x05, /**< executes the write instruction previously registered */
    DXL_INST_FACTORY_RESET = 0x06, /**< resets the Control Table to its initial factory default settings */
    DXL_INST_REBOOT        = 0x08, /**< reboot the Device */
    DXL_INST_STATUS        = 0x55, /**< Return Instruction for the Instruction Packet */
    DXL_INST_SYNC_READ     = 0x82, /**< (Multiple devices) read data with same Address and length at once */
    DXL_INST_SYNC_WRITE    = 0x83, /**< (Multiple devices) write data on the same Address and length at once */
    DXL_INST_BULK_READ     = 0x92, /**< (Multiple devices) read data from different Addresses and lengths at once */
    DXL_INST_BULK_WRITE    = 0x93, /**< (Multiple devices) write data on different Addresses and lengths at once */
} dynamixel_intruction_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DYNAMIXEL_PROTOCOL_H */
