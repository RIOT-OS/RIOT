/*
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_uc8151d
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef UC8151D_CONSTANTS_H
#define UC8151D_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UC8151D SPI commands
 * @brief   Commands used for controlling UC8151D E-Ink displays
 * @{
 */
#define UC8151D_PANEL_SETTING       (0x00)          /**< Panel setting configuration command */
#define UC8151D_POWER_SETTING       (0x01)          /**< Power setting configuration command */
#define UC8151D_POWER_OFF           (0x02)          /**< Power off command */
#define UC8151D_POWER_OFF_SEQUENCE_SETTING (0x03)   /**< Power off sequence configuration command */
#define UC8151D_POWER_ON            (0x04)          /**< Power on command */
#define UC8151D_POWER_ON_MEASURE    (0x05)          /**< Power on measure command */
#define UC8151D_BOOSTER_SOFT_START  (0x06)          /**< Booster soft start configuration command */
#define UC8151D_DEEP_SLEEP          (0x07)          /**< Deep sleep command */
#define UC8151D_DISP_START_TRANS1   (0x10)          /**< Display start transfer 1 command */
#define UC8151D_DATA_STOP           (0x11)          /**< Data stop command */
#define UC8151D_DISP_REFRESH        (0x12)          /**< Display refresh command */
#define UC8151D_DISP_START_TRANS2   (0x13)          /**< Display start transfer 2 command */
#define UC8151D_AUTO_SEQUENCE       (0x17)          /**< Auto sequence command */
#define UC8151D_VCOM_LUT            (0x20)          /**< VCOM LUT configuration command */
#define UC8151D_W2W_LUT             (0x21)          /**< W2W LUT configuration command */
#define UC8151D_B2W_LUT             (0x22)          /**< B2W LUT configuration command */
#define UC8151D_W2B_LUT             (0x23)          /**< W2B LUT configuration command */
#define UC8151D_B2B_LUT             (0x24)          /**< B2B LUT configuration command */
#define UC8151D_LUTOPT              (0x2A)          /**< LUT option configuration command */
#define UC8151D_PLL_CTRL            (0x30)          /**< PLL control command */
#define UC8151D_TEMP_SENSOR_CALL    (0x40)          /**< Temperature sensor calibrate command */
#define UC8151D_TEMP_SENSOR_SEL     (0x41)          /**< Temperature sensor selection command */
#define UC8151D_TEMP_SENSOR_WRITE   (0x42)          /**< Temperature sensor write command */
#define UC8151D_TEMP_SENSOR_READ    (0x43)          /**< Temperature sensor read command */
#define UC8151D_VCOM_DATA_INTERVAL  (0x50)          /**< VCOM and Data interval command */
#define UC8151D_LOW_POWER_DETECTION (0x51)          /**< Low power detection command */
#define UC8151D_TCON_SETTING        (0x60)          /**< TCON settings command */
#define UC8151D_RESOLUTION_SETTING  (0x61)          /**< Resolution configuration command */
#define UC8151D_GS_SETTING          (0x65)          /**< Gate/Source start setting command */
#define UC8151D_REVISION            (0x70)          /**< Revision command */
#define UC8151D_GET_STATUS          (0x71)          /**< Get status command */
#define UC8151D_AUTO_MEASUREMENT_VCOM (0x80)        /**< Auto measure VCOM command */
#define UC8151D_READ_VCOM_VALUE     (0x81)          /**< Read vcom value command */
#define UC8151D_VCM_DC_SETTING      (0x82)          /**< VCOM and DC settings command */
#define UC8151D_PARTIAL_WINDOW      (0x90)          /**< Partial window command */
#define UC8151D_PARTIAL_IN          (0x91)          /**< Partial in command */
#define UC8151D_PARTIAL_OUT         (0x92)          /**< Partial out command */
#define UC8151D_PROGRAM_MODE        (0xA0)          /**< Program mode command */
#define UC8151D_ACTIVE_PROGRAMMING  (0xA1)          /**< Active programming command */
#define UC8151D_READ_OTP            (0xA2)          /**< Read otp command */
#define UC8151D_POWER_SAVING        (0xE3)          /**< Power saving command */
/** @} */

/**
 * @name Panel setting configuration
 * @{ */
#define UC8151D_PANEL_SETTING_DISP_RES_96x230  0x00     /**< Panel configured as 96 x 230 */
#define UC8151D_PANEL_SETTING_DISP_RES_96x252  0x40     /**< Panel configured as 96 x 252 */
#define UC8151D_PANEL_SETTING_DISP_RES_128x296 0x80     /**< Panel configured as 128 x 296 */
#define UC8151D_PANEL_SETTING_DISP_RES_160x296 0xC0     /**< Panel configured as 160 x 296 */
#define UC8151D_PANEL_SETTING_LUT_FROM_REG  (1 << 5)    /**< Read LUT from registers */
#define UC8151D_PANEL_SETTING_BW            (1 << 4)    /**< Enable monochrome mode */
#define UC8151D_PANEL_SETTING_SCAN_DIR_UP   (1 << 3)    /**< Scan direction up */
#define UC8151D_PANEL_SETTING_SCAN_DIR_RIGHT (1 << 2)   /**< Scan direction right */
#define UC8151D_PANEL_SETTING_BOOSTER_SWITCH (1 << 1)   /**< Booster switch enabled */
#define UC8151D_PANEL_SETTING_NO_RESET       (1 << 0)   /**< Do NOT reset the configuration */
/** @} */

/**
 * @name Power setting configuration
 * @{ */
#define UC8151D_POWER_SETTING_EN_DCDC_VDHVDL    (1 << 1)    /**< Enable the DC/DC for VDH/VDL/VDHR */
#define UC8151D_POWER_SETTING_EN_DCDC_VGHVGL    (1 << 0)    /**< Enable the DC/DC for VGH/VGL */
/** @} */

/**
 * @brief LUT voltage pattern for the 4 groups
 */
typedef enum {
    UC8151D_VOLTAGE_PATTERN_0000    = 0x0,  /**< Pattern: DC, DC, DC, DC */
    UC8151D_VOLTAGE_PATTERN_000H    = 0x1,  /**< Pattern: DC, DC, DC, High */
    UC8151D_VOLTAGE_PATTERN_000L    = 0x2,  /**< Pattern: DC, DC, DC, Low */
    UC8151D_VOLTAGE_PATTERN_000F    = 0x3,  /**< Pattern: DC, DC, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_00H0    = 0x4,  /**< Pattern: DC, DC, High, DC */
    UC8151D_VOLTAGE_PATTERN_00HH    = 0x5,  /**< Pattern: DC, DC, High, High */
    UC8151D_VOLTAGE_PATTERN_00HL    = 0x6,  /**< Pattern: DC, DC, High, Low */
    UC8151D_VOLTAGE_PATTERN_00HF    = 0x7,  /**< Pattern: DC, DC, High, Floating */
    UC8151D_VOLTAGE_PATTERN_00L0    = 0x8,  /**< Pattern: DC, DC, Low, DC */
    UC8151D_VOLTAGE_PATTERN_00LH    = 0x9,  /**< Pattern: DC, DC, Low, High */
    UC8151D_VOLTAGE_PATTERN_00LL    = 0xa,  /**< Pattern: DC, DC, Low, Low */
    UC8151D_VOLTAGE_PATTERN_00LF    = 0xb,  /**< Pattern: DC, DC, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_00F0    = 0xc,  /**< Pattern: DC, DC, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_00FH    = 0xd,  /**< Pattern: DC, DC, Floating, High */
    UC8151D_VOLTAGE_PATTERN_00FL    = 0xe,  /**< Pattern: DC, DC, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_00FF    = 0xf,  /**< Pattern: DC, DC, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_0H00    = 0x10, /**< Pattern: DC, High, DC, DC */
    UC8151D_VOLTAGE_PATTERN_0H0H    = 0x11, /**< Pattern: DC, High, DC, High */
    UC8151D_VOLTAGE_PATTERN_0H0L    = 0x12, /**< Pattern: DC, High, DC, Low */
    UC8151D_VOLTAGE_PATTERN_0H0F    = 0x13, /**< Pattern: DC, High, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_0HH0    = 0x14, /**< Pattern: DC, High, High, DC */
    UC8151D_VOLTAGE_PATTERN_0HHH    = 0x15, /**< Pattern: DC, High, High, High */
    UC8151D_VOLTAGE_PATTERN_0HHL    = 0x16, /**< Pattern: DC, High, High, Low */
    UC8151D_VOLTAGE_PATTERN_0HHF    = 0x17, /**< Pattern: DC, High, High, Floating */
    UC8151D_VOLTAGE_PATTERN_0HL0    = 0x18, /**< Pattern: DC, High, Low, DC */
    UC8151D_VOLTAGE_PATTERN_0HLH    = 0x19, /**< Pattern: DC, High, Low, High */
    UC8151D_VOLTAGE_PATTERN_0HLL    = 0x1a, /**< Pattern: DC, High, Low, Low */
    UC8151D_VOLTAGE_PATTERN_0HLF    = 0x1b, /**< Pattern: DC, High, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_0HF0    = 0x1c, /**< Pattern: DC, High, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_0HFH    = 0x1d, /**< Pattern: DC, High, Floating, High */
    UC8151D_VOLTAGE_PATTERN_0HFL    = 0x1e, /**< Pattern: DC, High, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_0HFF    = 0x1f, /**< Pattern: DC, High, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_0L00    = 0x20, /**< Pattern: DC, Low, DC, DC */
    UC8151D_VOLTAGE_PATTERN_0L0H    = 0x21, /**< Pattern: DC, Low, DC, High */
    UC8151D_VOLTAGE_PATTERN_0L0L    = 0x22, /**< Pattern: DC, Low, DC, Low */
    UC8151D_VOLTAGE_PATTERN_0L0F    = 0x23, /**< Pattern: DC, Low, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_0LH0    = 0x24, /**< Pattern: DC, Low, High, DC */
    UC8151D_VOLTAGE_PATTERN_0LHH    = 0x25, /**< Pattern: DC, Low, High, High */
    UC8151D_VOLTAGE_PATTERN_0LHL    = 0x26, /**< Pattern: DC, Low, High, Low */
    UC8151D_VOLTAGE_PATTERN_0LHF    = 0x27, /**< Pattern: DC, Low, High, Floating */
    UC8151D_VOLTAGE_PATTERN_0LL0    = 0x28, /**< Pattern: DC, Low, Low, DC */
    UC8151D_VOLTAGE_PATTERN_0LLH    = 0x29, /**< Pattern: DC, Low, Low, High */
    UC8151D_VOLTAGE_PATTERN_0LLL    = 0x2a, /**< Pattern: DC, Low, Low, Low */
    UC8151D_VOLTAGE_PATTERN_0LLF    = 0x2b, /**< Pattern: DC, Low, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_0LF0    = 0x2c, /**< Pattern: DC, Low, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_0LFH    = 0x2d, /**< Pattern: DC, Low, Floating, High */
    UC8151D_VOLTAGE_PATTERN_0LFL    = 0x2e, /**< Pattern: DC, Low, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_0LFF    = 0x2f, /**< Pattern: DC, Low, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_0F00    = 0x30, /**< Pattern: DC, Floating, DC, DC */
    UC8151D_VOLTAGE_PATTERN_0F0H    = 0x31, /**< Pattern: DC, Floating, DC, High */
    UC8151D_VOLTAGE_PATTERN_0F0L    = 0x32, /**< Pattern: DC, Floating, DC, Low */
    UC8151D_VOLTAGE_PATTERN_0F0F    = 0x33, /**< Pattern: DC, Floating, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_0FH0    = 0x34, /**< Pattern: DC, Floating, High, DC */
    UC8151D_VOLTAGE_PATTERN_0FHH    = 0x35, /**< Pattern: DC, Floating, High, High */
    UC8151D_VOLTAGE_PATTERN_0FHL    = 0x36, /**< Pattern: DC, Floating, High, Low */
    UC8151D_VOLTAGE_PATTERN_0FHF    = 0x37, /**< Pattern: DC, Floating, High, Floating */
    UC8151D_VOLTAGE_PATTERN_0FL0    = 0x38, /**< Pattern: DC, Floating, Low, DC */
    UC8151D_VOLTAGE_PATTERN_0FLH    = 0x39, /**< Pattern: DC, Floating, Low, High */
    UC8151D_VOLTAGE_PATTERN_0FLL    = 0x3a, /**< Pattern: DC, Floating, Low, Low */
    UC8151D_VOLTAGE_PATTERN_0FLF    = 0x3b, /**< Pattern: DC, Floating, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_0FF0    = 0x3c, /**< Pattern: DC, Floating, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_0FFH    = 0x3d, /**< Pattern: DC, Floating, Floating, High */
    UC8151D_VOLTAGE_PATTERN_0FFL    = 0x3e, /**< Pattern: DC, Floating, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_0FFF    = 0x3f, /**< Pattern: DC, Floating, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_H000    = 0x40, /**< Pattern: High, DC, DC, DC */
    UC8151D_VOLTAGE_PATTERN_H00H    = 0x41, /**< Pattern: High, DC, DC, High */
    UC8151D_VOLTAGE_PATTERN_H00L    = 0x42, /**< Pattern: High, DC, DC, Low */
    UC8151D_VOLTAGE_PATTERN_H00F    = 0x43, /**< Pattern: High, DC, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_H0H0    = 0x44, /**< Pattern: High, DC, High, DC */
    UC8151D_VOLTAGE_PATTERN_H0HH    = 0x45, /**< Pattern: High, DC, High, High */
    UC8151D_VOLTAGE_PATTERN_H0HL    = 0x46, /**< Pattern: High, DC, High, Low */
    UC8151D_VOLTAGE_PATTERN_H0HF    = 0x47, /**< Pattern: High, DC, High, Floating */
    UC8151D_VOLTAGE_PATTERN_H0L0    = 0x48, /**< Pattern: High, DC, Low, DC */
    UC8151D_VOLTAGE_PATTERN_H0LH    = 0x49, /**< Pattern: High, DC, Low, High */
    UC8151D_VOLTAGE_PATTERN_H0LL    = 0x4a, /**< Pattern: High, DC, Low, Low */
    UC8151D_VOLTAGE_PATTERN_H0LF    = 0x4b, /**< Pattern: High, DC, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_H0F0    = 0x4c, /**< Pattern: High, DC, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_H0FH    = 0x4d, /**< Pattern: High, DC, Floating, High */
    UC8151D_VOLTAGE_PATTERN_H0FL    = 0x4e, /**< Pattern: High, DC, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_H0FF    = 0x4f, /**< Pattern: High, DC, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_HH00    = 0x50, /**< Pattern: High, High, DC, DC */
    UC8151D_VOLTAGE_PATTERN_HH0H    = 0x51, /**< Pattern: High, High, DC, High */
    UC8151D_VOLTAGE_PATTERN_HH0L    = 0x52, /**< Pattern: High, High, DC, Low */
    UC8151D_VOLTAGE_PATTERN_HH0F    = 0x53, /**< Pattern: High, High, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_HHH0    = 0x54, /**< Pattern: High, High, High, DC */
    UC8151D_VOLTAGE_PATTERN_HHHH    = 0x55, /**< Pattern: High, High, High, High */
    UC8151D_VOLTAGE_PATTERN_HHHL    = 0x56, /**< Pattern: High, High, High, Low */
    UC8151D_VOLTAGE_PATTERN_HHHF    = 0x57, /**< Pattern: High, High, High, Floating */
    UC8151D_VOLTAGE_PATTERN_HHL0    = 0x58, /**< Pattern: High, High, Low, DC */
    UC8151D_VOLTAGE_PATTERN_HHLH    = 0x59, /**< Pattern: High, High, Low, High */
    UC8151D_VOLTAGE_PATTERN_HHLL    = 0x5a, /**< Pattern: High, High, Low, Low */
    UC8151D_VOLTAGE_PATTERN_HHLF    = 0x5b, /**< Pattern: High, High, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_HHF0    = 0x5c, /**< Pattern: High, High, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_HHFH    = 0x5d, /**< Pattern: High, High, Floating, High */
    UC8151D_VOLTAGE_PATTERN_HHFL    = 0x5e, /**< Pattern: High, High, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_HHFF    = 0x5f, /**< Pattern: High, High, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_HL00    = 0x60, /**< Pattern: High, Low, DC, DC */
    UC8151D_VOLTAGE_PATTERN_HL0H    = 0x61, /**< Pattern: High, Low, DC, High */
    UC8151D_VOLTAGE_PATTERN_HL0L    = 0x62, /**< Pattern: High, Low, DC, Low */
    UC8151D_VOLTAGE_PATTERN_HL0F    = 0x63, /**< Pattern: High, Low, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_HLH0    = 0x64, /**< Pattern: High, Low, High, DC */
    UC8151D_VOLTAGE_PATTERN_HLHH    = 0x65, /**< Pattern: High, Low, High, High */
    UC8151D_VOLTAGE_PATTERN_HLHL    = 0x66, /**< Pattern: High, Low, High, Low */
    UC8151D_VOLTAGE_PATTERN_HLHF    = 0x67, /**< Pattern: High, Low, High, Floating */
    UC8151D_VOLTAGE_PATTERN_HLL0    = 0x68, /**< Pattern: High, Low, Low, DC */
    UC8151D_VOLTAGE_PATTERN_HLLH    = 0x69, /**< Pattern: High, Low, Low, High */
    UC8151D_VOLTAGE_PATTERN_HLLL    = 0x6a, /**< Pattern: High, Low, Low, Low */
    UC8151D_VOLTAGE_PATTERN_HLLF    = 0x6b, /**< Pattern: High, Low, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_HLF0    = 0x6c, /**< Pattern: High, Low, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_HLFH    = 0x6d, /**< Pattern: High, Low, Floating, High */
    UC8151D_VOLTAGE_PATTERN_HLFL    = 0x6e, /**< Pattern: High, Low, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_HLFF    = 0x6f, /**< Pattern: High, Low, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_HF00    = 0x70, /**< Pattern: High, Floating, DC, DC */
    UC8151D_VOLTAGE_PATTERN_HF0H    = 0x71, /**< Pattern: High, Floating, DC, High */
    UC8151D_VOLTAGE_PATTERN_HF0L    = 0x72, /**< Pattern: High, Floating, DC, Low */
    UC8151D_VOLTAGE_PATTERN_HF0F    = 0x73, /**< Pattern: High, Floating, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_HFH0    = 0x74, /**< Pattern: High, Floating, High, DC */
    UC8151D_VOLTAGE_PATTERN_HFHH    = 0x75, /**< Pattern: High, Floating, High, High */
    UC8151D_VOLTAGE_PATTERN_HFHL    = 0x76, /**< Pattern: High, Floating, High, Low */
    UC8151D_VOLTAGE_PATTERN_HFHF    = 0x77, /**< Pattern: High, Floating, High, Floating */
    UC8151D_VOLTAGE_PATTERN_HFL0    = 0x78, /**< Pattern: High, Floating, Low, DC */
    UC8151D_VOLTAGE_PATTERN_HFLH    = 0x79, /**< Pattern: High, Floating, Low, High */
    UC8151D_VOLTAGE_PATTERN_HFLL    = 0x7a, /**< Pattern: High, Floating, Low, Low */
    UC8151D_VOLTAGE_PATTERN_HFLF    = 0x7b, /**< Pattern: High, Floating, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_HFF0    = 0x7c, /**< Pattern: High, Floating, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_HFFH    = 0x7d, /**< Pattern: High, Floating, Floating, High */
    UC8151D_VOLTAGE_PATTERN_HFFL    = 0x7e, /**< Pattern: High, Floating, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_HFFF    = 0x7f, /**< Pattern: High, Floating, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_L000    = 0x80, /**< Pattern: Low, DC, DC, DC */
    UC8151D_VOLTAGE_PATTERN_L00H    = 0x81, /**< Pattern: Low, DC, DC, High */
    UC8151D_VOLTAGE_PATTERN_L00L    = 0x82, /**< Pattern: Low, DC, DC, Low */
    UC8151D_VOLTAGE_PATTERN_L00F    = 0x83, /**< Pattern: Low, DC, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_L0H0    = 0x84, /**< Pattern: Low, DC, High, DC */
    UC8151D_VOLTAGE_PATTERN_L0HH    = 0x85, /**< Pattern: Low, DC, High, High */
    UC8151D_VOLTAGE_PATTERN_L0HL    = 0x86, /**< Pattern: Low, DC, High, Low */
    UC8151D_VOLTAGE_PATTERN_L0HF    = 0x87, /**< Pattern: Low, DC, High, Floating */
    UC8151D_VOLTAGE_PATTERN_L0L0    = 0x88, /**< Pattern: Low, DC, Low, DC */
    UC8151D_VOLTAGE_PATTERN_L0LH    = 0x89, /**< Pattern: Low, DC, Low, High */
    UC8151D_VOLTAGE_PATTERN_L0LL    = 0x8a, /**< Pattern: Low, DC, Low, Low */
    UC8151D_VOLTAGE_PATTERN_L0LF    = 0x8b, /**< Pattern: Low, DC, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_L0F0    = 0x8c, /**< Pattern: Low, DC, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_L0FH    = 0x8d, /**< Pattern: Low, DC, Floating, High */
    UC8151D_VOLTAGE_PATTERN_L0FL    = 0x8e, /**< Pattern: Low, DC, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_L0FF    = 0x8f, /**< Pattern: Low, DC, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_LH00    = 0x90, /**< Pattern: Low, High, DC, DC */
    UC8151D_VOLTAGE_PATTERN_LH0H    = 0x91, /**< Pattern: Low, High, DC, High */
    UC8151D_VOLTAGE_PATTERN_LH0L    = 0x92, /**< Pattern: Low, High, DC, Low */
    UC8151D_VOLTAGE_PATTERN_LH0F    = 0x93, /**< Pattern: Low, High, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_LHH0    = 0x94, /**< Pattern: Low, High, High, DC */
    UC8151D_VOLTAGE_PATTERN_LHHH    = 0x95, /**< Pattern: Low, High, High, High */
    UC8151D_VOLTAGE_PATTERN_LHHL    = 0x96, /**< Pattern: Low, High, High, Low */
    UC8151D_VOLTAGE_PATTERN_LHHF    = 0x97, /**< Pattern: Low, High, High, Floating */
    UC8151D_VOLTAGE_PATTERN_LHL0    = 0x98, /**< Pattern: Low, High, Low, DC */
    UC8151D_VOLTAGE_PATTERN_LHLH    = 0x99, /**< Pattern: Low, High, Low, High */
    UC8151D_VOLTAGE_PATTERN_LHLL    = 0x9a, /**< Pattern: Low, High, Low, Low */
    UC8151D_VOLTAGE_PATTERN_LHLF    = 0x9b, /**< Pattern: Low, High, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_LHF0    = 0x9c, /**< Pattern: Low, High, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_LHFH    = 0x9d, /**< Pattern: Low, High, Floating, High */
    UC8151D_VOLTAGE_PATTERN_LHFL    = 0x9e, /**< Pattern: Low, High, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_LHFF    = 0x9f, /**< Pattern: Low, High, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_LL00    = 0xa0, /**< Pattern: Low, Low, DC, DC */
    UC8151D_VOLTAGE_PATTERN_LL0H    = 0xa1, /**< Pattern: Low, Low, DC, High */
    UC8151D_VOLTAGE_PATTERN_LL0L    = 0xa2, /**< Pattern: Low, Low, DC, Low */
    UC8151D_VOLTAGE_PATTERN_LL0F    = 0xa3, /**< Pattern: Low, Low, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_LLH0    = 0xa4, /**< Pattern: Low, Low, High, DC */
    UC8151D_VOLTAGE_PATTERN_LLHH    = 0xa5, /**< Pattern: Low, Low, High, High */
    UC8151D_VOLTAGE_PATTERN_LLHL    = 0xa6, /**< Pattern: Low, Low, High, Low */
    UC8151D_VOLTAGE_PATTERN_LLHF    = 0xa7, /**< Pattern: Low, Low, High, Floating */
    UC8151D_VOLTAGE_PATTERN_LLL0    = 0xa8, /**< Pattern: Low, Low, Low, DC */
    UC8151D_VOLTAGE_PATTERN_LLLH    = 0xa9, /**< Pattern: Low, Low, Low, High */
    UC8151D_VOLTAGE_PATTERN_LLLL    = 0xaa, /**< Pattern: Low, Low, Low, Low */
    UC8151D_VOLTAGE_PATTERN_LLLF    = 0xab, /**< Pattern: Low, Low, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_LLF0    = 0xac, /**< Pattern: Low, Low, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_LLFH    = 0xad, /**< Pattern: Low, Low, Floating, High */
    UC8151D_VOLTAGE_PATTERN_LLFL    = 0xae, /**< Pattern: Low, Low, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_LLFF    = 0xaf, /**< Pattern: Low, Low, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_LF00    = 0xb0, /**< Pattern: Low, Floating, DC, DC */
    UC8151D_VOLTAGE_PATTERN_LF0H    = 0xb1, /**< Pattern: Low, Floating, DC, High */
    UC8151D_VOLTAGE_PATTERN_LF0L    = 0xb2, /**< Pattern: Low, Floating, DC, Low */
    UC8151D_VOLTAGE_PATTERN_LF0F    = 0xb3, /**< Pattern: Low, Floating, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_LFH0    = 0xb4, /**< Pattern: Low, Floating, High, DC */
    UC8151D_VOLTAGE_PATTERN_LFHH    = 0xb5, /**< Pattern: Low, Floating, High, High */
    UC8151D_VOLTAGE_PATTERN_LFHL    = 0xb6, /**< Pattern: Low, Floating, High, Low */
    UC8151D_VOLTAGE_PATTERN_LFHF    = 0xb7, /**< Pattern: Low, Floating, High, Floating */
    UC8151D_VOLTAGE_PATTERN_LFL0    = 0xb8, /**< Pattern: Low, Floating, Low, DC */
    UC8151D_VOLTAGE_PATTERN_LFLH    = 0xb9, /**< Pattern: Low, Floating, Low, High */
    UC8151D_VOLTAGE_PATTERN_LFLL    = 0xba, /**< Pattern: Low, Floating, Low, Low */
    UC8151D_VOLTAGE_PATTERN_LFLF    = 0xbb, /**< Pattern: Low, Floating, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_LFF0    = 0xbc, /**< Pattern: Low, Floating, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_LFFH    = 0xbd, /**< Pattern: Low, Floating, Floating, High */
    UC8151D_VOLTAGE_PATTERN_LFFL    = 0xbe, /**< Pattern: Low, Floating, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_LFFF    = 0xbf, /**< Pattern: Low, Floating, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_F000    = 0xc0, /**< Pattern: Floating, DC, DC, DC */
    UC8151D_VOLTAGE_PATTERN_F00H    = 0xc1, /**< Pattern: Floating, DC, DC, High */
    UC8151D_VOLTAGE_PATTERN_F00L    = 0xc2, /**< Pattern: Floating, DC, DC, Low */
    UC8151D_VOLTAGE_PATTERN_F00F    = 0xc3, /**< Pattern: Floating, DC, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_F0H0    = 0xc4, /**< Pattern: Floating, DC, High, DC */
    UC8151D_VOLTAGE_PATTERN_F0HH    = 0xc5, /**< Pattern: Floating, DC, High, High */
    UC8151D_VOLTAGE_PATTERN_F0HL    = 0xc6, /**< Pattern: Floating, DC, High, Low */
    UC8151D_VOLTAGE_PATTERN_F0HF    = 0xc7, /**< Pattern: Floating, DC, High, Floating */
    UC8151D_VOLTAGE_PATTERN_F0L0    = 0xc8, /**< Pattern: Floating, DC, Low, DC */
    UC8151D_VOLTAGE_PATTERN_F0LH    = 0xc9, /**< Pattern: Floating, DC, Low, High */
    UC8151D_VOLTAGE_PATTERN_F0LL    = 0xca, /**< Pattern: Floating, DC, Low, Low */
    UC8151D_VOLTAGE_PATTERN_F0LF    = 0xcb, /**< Pattern: Floating, DC, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_F0F0    = 0xcc, /**< Pattern: Floating, DC, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_F0FH    = 0xcd, /**< Pattern: Floating, DC, Floating, High */
    UC8151D_VOLTAGE_PATTERN_F0FL    = 0xce, /**< Pattern: Floating, DC, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_F0FF    = 0xcf, /**< Pattern: Floating, DC, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_FH00    = 0xd0, /**< Pattern: Floating, High, DC, DC */
    UC8151D_VOLTAGE_PATTERN_FH0H    = 0xd1, /**< Pattern: Floating, High, DC, High */
    UC8151D_VOLTAGE_PATTERN_FH0L    = 0xd2, /**< Pattern: Floating, High, DC, Low */
    UC8151D_VOLTAGE_PATTERN_FH0F    = 0xd3, /**< Pattern: Floating, High, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_FHH0    = 0xd4, /**< Pattern: Floating, High, High, DC */
    UC8151D_VOLTAGE_PATTERN_FHHH    = 0xd5, /**< Pattern: Floating, High, High, High */
    UC8151D_VOLTAGE_PATTERN_FHHL    = 0xd6, /**< Pattern: Floating, High, High, Low */
    UC8151D_VOLTAGE_PATTERN_FHHF    = 0xd7, /**< Pattern: Floating, High, High, Floating */
    UC8151D_VOLTAGE_PATTERN_FHL0    = 0xd8, /**< Pattern: Floating, High, Low, DC */
    UC8151D_VOLTAGE_PATTERN_FHLH    = 0xd9, /**< Pattern: Floating, High, Low, High */
    UC8151D_VOLTAGE_PATTERN_FHLL    = 0xda, /**< Pattern: Floating, High, Low, Low */
    UC8151D_VOLTAGE_PATTERN_FHLF    = 0xdb, /**< Pattern: Floating, High, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_FHF0    = 0xdc, /**< Pattern: Floating, High, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_FHFH    = 0xdd, /**< Pattern: Floating, High, Floating, High */
    UC8151D_VOLTAGE_PATTERN_FHFL    = 0xde, /**< Pattern: Floating, High, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_FHFF    = 0xdf, /**< Pattern: Floating, High, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_FL00    = 0xe0, /**< Pattern: Floating, Low, DC, DC */
    UC8151D_VOLTAGE_PATTERN_FL0H    = 0xe1, /**< Pattern: Floating, Low, DC, High */
    UC8151D_VOLTAGE_PATTERN_FL0L    = 0xe2, /**< Pattern: Floating, Low, DC, Low */
    UC8151D_VOLTAGE_PATTERN_FL0F    = 0xe3, /**< Pattern: Floating, Low, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_FLH0    = 0xe4, /**< Pattern: Floating, Low, High, DC */
    UC8151D_VOLTAGE_PATTERN_FLHH    = 0xe5, /**< Pattern: Floating, Low, High, High */
    UC8151D_VOLTAGE_PATTERN_FLHL    = 0xe6, /**< Pattern: Floating, Low, High, Low */
    UC8151D_VOLTAGE_PATTERN_FLHF    = 0xe7, /**< Pattern: Floating, Low, High, Floating */
    UC8151D_VOLTAGE_PATTERN_FLL0    = 0xe8, /**< Pattern: Floating, Low, Low, DC */
    UC8151D_VOLTAGE_PATTERN_FLLH    = 0xe9, /**< Pattern: Floating, Low, Low, High */
    UC8151D_VOLTAGE_PATTERN_FLLL    = 0xea, /**< Pattern: Floating, Low, Low, Low */
    UC8151D_VOLTAGE_PATTERN_FLLF    = 0xeb, /**< Pattern: Floating, Low, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_FLF0    = 0xec, /**< Pattern: Floating, Low, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_FLFH    = 0xed, /**< Pattern: Floating, Low, Floating, High */
    UC8151D_VOLTAGE_PATTERN_FLFL    = 0xee, /**< Pattern: Floating, Low, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_FLFF    = 0xef, /**< Pattern: Floating, Low, Floating, Floating */
    UC8151D_VOLTAGE_PATTERN_FF00    = 0xf0, /**< Pattern: Floating, Floating, DC, DC */
    UC8151D_VOLTAGE_PATTERN_FF0H    = 0xf1, /**< Pattern: Floating, Floating, DC, High */
    UC8151D_VOLTAGE_PATTERN_FF0L    = 0xf2, /**< Pattern: Floating, Floating, DC, Low */
    UC8151D_VOLTAGE_PATTERN_FF0F    = 0xf3, /**< Pattern: Floating, Floating, DC, Floating */
    UC8151D_VOLTAGE_PATTERN_FFH0    = 0xf4, /**< Pattern: Floating, Floating, High, DC */
    UC8151D_VOLTAGE_PATTERN_FFHH    = 0xf5, /**< Pattern: Floating, Floating, High, High */
    UC8151D_VOLTAGE_PATTERN_FFHL    = 0xf6, /**< Pattern: Floating, Floating, High, Low */
    UC8151D_VOLTAGE_PATTERN_FFHF    = 0xf7, /**< Pattern: Floating, Floating, High, Floating */
    UC8151D_VOLTAGE_PATTERN_FFL0    = 0xf8, /**< Pattern: Floating, Floating, Low, DC */
    UC8151D_VOLTAGE_PATTERN_FFLH    = 0xf9, /**< Pattern: Floating, Floating, Low, High */
    UC8151D_VOLTAGE_PATTERN_FFLL    = 0xfa, /**< Pattern: Floating, Floating, Low, Low */
    UC8151D_VOLTAGE_PATTERN_FFLF    = 0xfb, /**< Pattern: Floating, Floating, Low, Floating */
    UC8151D_VOLTAGE_PATTERN_FFF0    = 0xfc, /**< Pattern: Floating, Floating, Floating, DC */
    UC8151D_VOLTAGE_PATTERN_FFFH    = 0xfd, /**< Pattern: Floating, Floating, Floating, High */
    UC8151D_VOLTAGE_PATTERN_FFFL    = 0xfe, /**< Pattern: Floating, Floating, Floating, Low */
    UC8151D_VOLTAGE_PATTERN_FFFF    = 0xff, /**< Pattern: Floating, Floating, Floating, Floating */
} uc8151d_voltage_pattern_t;

/**
 * @brief Display frame rate PLL settings
 */
typedef enum {
    UC8151D_FREQUENCY_4HZ   = 0x0F, /**< 4 Hz Frame rate */
    UC8151D_FREQUENCY_5HZ   = 0x0E, /**< 5 Hz Frame rate */
    UC8151D_FREQUENCY_6HZ   = 0x0D, /**< 6 Hz Frame rate */
    UC8151D_FREQUENCY_7HZ   = 0x0C, /**< 7 Hz Frame rate */
    UC8151D_FREQUENCY_8HZ   = 0x17, /**< 8 Hz Frame rate */
    UC8151D_FREQUENCY_10HZ  = 0x16, /**< 10 Hz Frame rate */
    UC8151D_FREQUENCY_11HZ  = 0x15, /**< 11 Hz Frame rate */
    UC8151D_FREQUENCY_12HZ  = 0x1F, /**< 12 Hz Frame rate */
    UC8151D_FREQUENCY_14HZ  = 0x14, /**< 14 Hz Frame rate */
    UC8151D_FREQUENCY_16HZ  = 0x27, /**< 16 Hz Frame rate */
    UC8151D_FREQUENCY_17HZ  = 0x1D, /**< 17 Hz Frame rate */
    UC8151D_FREQUENCY_19HZ  = 0x26, /**< 19 Hz Frame rate */
    UC8151D_FREQUENCY_20HZ  = 0x2F, /**< 20 Hz Frame rate */
    UC8151D_FREQUENCY_21HZ  = 0x1C, /**< 21 Hz Frame rate */
    UC8151D_FREQUENCY_23HZ  = 0x25, /**< 23 Hz Frame rate */
    UC8151D_FREQUENCY_24HZ  = 0x2E, /**< 24 Hz Frame rate */
    UC8151D_FREQUENCY_29HZ  = 0x24, /**< 29 Hz Frame rate */
    UC8151D_FREQUENCY_33HZ  = 0x3E, /**< 33 Hz Frame rate */
    UC8151D_FREQUENCY_34HZ  = 0x35, /**< 34 Hz Frame rate */
    UC8151D_FREQUENCY_36HZ  = 0x2C, /**< 36 Hz Frame rate */
    UC8151D_FREQUENCY_38HZ  = 0x23, /**< 38 Hz Frame rate */
    UC8151D_FREQUENCY_40HZ  = 0x3D, /**< 40 Hz Frame rate */
    UC8151D_FREQUENCY_43HZ  = 0x34, /**< 43 Hz Frame rate */
    UC8151D_FREQUENCY_48HZ  = 0x2B, /**< 48 Hz Frame rate */
    UC8151D_FREQUENCY_50HZ  = 0x3C, /**< 50 Hz Frame rate */
    UC8151D_FREQUENCY_57HZ  = 0x33, /**< 57 Hz Frame rate */
    UC8151D_FREQUENCY_67HZ  = 0x3B, /**< 67 Hz Frame rate */
    UC8151D_FREQUENCY_72HZ  = 0x2A, /**< 72 Hz Frame rate */
    UC8151D_FREQUENCY_86HZ  = 0x19, /**< 86 Hz Frame rate */
    UC8151D_FREQUENCY_100HZ = 0x3A, /**< 100 Hz Frame rate */
    UC8151D_FREQUENCY_114HZ = 0x21, /**< 114 Hz Frame rate */
    UC8151D_FREQUENCY_150HZ = 0x29, /**< 150 Hz Frame rate */
    UC8151D_FREQUENCY_171HZ = 0x31, /**< 171 Hz Frame rate */
    UC8151D_FREQUENCY_200HZ = 0x39, /**< 200 Hz Frame rate */
} uc8151d_frequency_t;

#ifdef __cplusplus
}
#endif
#endif /* UC8151D_CONSTANTS_H */
/** @} */
