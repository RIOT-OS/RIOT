/*
 * Copyright (C) 2016-2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_si114x
 * @brief       Internal addresses, registers, constants for the Si114x sensors family.
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the Si114x sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Si114x I2C address
 */
#define SI114X_ADDR                            (0x60)

/**
 * @name Si114x commands
 * @{
 */
#define SI114X_PARAM_QUERY                     (0x80)
#define SI114X_PARAM_SET                       (0xA0)
#define SI114X_NOP                             (0x00)
#define SI114X_RESET                           (0x01)
#define SI114X_BUSADDR                         (0x02)
#define SI114X_PS_FORCE                        (0x05)
#define SI114X_ALS_FORCE                       (0x06)
#define SI114X_PS_ALS_FORCE                    (0x07)
#define SI114X_PS_PAUSE                        (0x09)
#define SI114X_ALS_PAUSE                       (0x0A)
#define SI114X_PS_ALS_PAUSE                    (0x0B)
#define SI114X_PS_AUTO                         (0x0D)
#define SI114X_ALS_AUTO                        (0x0E)
#define SI114X_PS_ALS_AUTO                     (0x0F)
#define SI114X_GET_CAL                         (0x12)
/** @} */

/**
 * @name Si114x registers
 * @{
 */
#define SI114X_REG_PART_ID                     (0x00)
#define SI114X_REG_REV_ID                      (0x01)
#define SI114X_REG_SEQ_ID                      (0x02)
#define SI114X_REG_INT_CFG                     (0x03)
#define SI114X_REG_IRQ_ENABLE                  (0x04)
#define SI114X_REG_IRQ_MODE1                   (0x05)
#define SI114X_REG_IRQ_MODE2                   (0x06)
#define SI114X_REG_HW_KEY                      (0x07)
#define SI114X_REG_MEAS_RATE0                  (0x08)
#define SI114X_REG_MEAS_RATE1                  (0x09)
#define SI114X_REG_PS_LED21                    (0x0F)
#define SI114X_REG_PS_LED3                     (0x10)
#define SI114X_REG_UCOEF0                      (0x13)
#define SI114X_REG_UCOEF1                      (0x14)
#define SI114X_REG_UCOEF2                      (0x15)
#define SI114X_REG_UCOEF3                      (0x16)
#define SI114X_REG_PARAM_WR                    (0x17)
#define SI114X_REG_COMMAND                     (0x18)
#define SI114X_REG_RESPONSE                    (0x20)
#define SI114X_REG_IRQ_STATUS                  (0x21)
#define SI114X_REG_ALS_VIS_DATA0               (0x22)
#define SI114X_REG_ALS_VIS_DATA1               (0x23)
#define SI114X_REG_ALS_IR_DATA0                (0x24)
#define SI114X_REG_ALS_IR_DATA1                (0x25)
#define SI114X_REG_PS1_DATA0                   (0x26)
#define SI114X_REG_PS1_DATA1                   (0x27)
#define SI114X_REG_PS2_DATA0                   (0x28)
#define SI114X_REG_PS2_DATA1                   (0x29)
#define SI114X_REG_PS3_DATA0                   (0x2A)
#define SI114X_REG_PS3_DATA1                   (0x2B)
#define SI114X_REG_UV_INDEX0                   (0x2C)
#define SI114X_REG_UV_INDEX1                   (0x2D)
#define SI114X_REG_PARAM_RD                    (0x2E)
#define SI114X_REG_CHIP_STAT                   (0x30)
/** @} */

/**
 * @name Si114x response register error codes
 * @{
 */
#define SI114X_NO_ERROR                        (0x00)
#define SI114X_INVALID_SETTING                 (0x80)
#define SI114X_PS1_ADC_OVERFLOW                (0x88)
#define SI114X_PS2_ADC_OVERFLOW                (0x89)
#define SI114X_PS3_ADC_OVERFLOW                (0x8A)
#define SI114X_ALS_VIS_ADC_OVERFLOW            (0x8C)
#define SI114X_ALS_IR_ADC_OVERFLOW             (0x8D)
#define SI114X_UV_ADC_OVERFLOW                 (0x8E)
/** @} */

/**
 * @name Si114x parameters RAM
 *
 * @note These parameter can be accessed indirectly using PARAM_QUERY or
 *       PARAM_SET commands.
 * @{
 */
#define SI114X_I2C_ADDR                        (0x00)
#define SI114X_PARAM_CHLIST                    (0x01)
#define SI114X_PARAM_CHLIST_ENUV               (0x80)
#define SI114X_PARAM_CHLIST_ENAUX              (0x40)
#define SI114X_PARAM_CHLIST_ENALSIR            (0x20)
#define SI114X_PARAM_CHLIST_ENALSVIS           (0x10)
#define SI114X_PARAM_CHLIST_ENPS1              (0x01)
#define SI114X_PARAM_CHLIST_ENPS2              (0x02)
#define SI114X_PARAM_CHLIST_ENPS3              (0x04)
#define SI114X_PARAM_PSLED12SEL                (0x02)
#define SI114X_PARAM_PSLED12SEL_PS2NONE        (0x00)
#define SI114X_PARAM_PSLED12SEL_PS2LED1        (0x10)
#define SI114X_PARAM_PSLED12SEL_PS2LED2        (0x20)
#define SI114X_PARAM_PSLED12SEL_PS2LED3        (0x40)
#define SI114X_PARAM_PSLED12SEL_PS1NONE        (0x00)
#define SI114X_PARAM_PSLED12SEL_PS1LED1        (0x01)
#define SI114X_PARAM_PSLED12SEL_PS1LED2        (0x02)
#define SI114X_PARAM_PSLED12SEL_PS1LED3        (0x04)
#define SI114X_PARAM_PSLED3SEL                 (0x03)
#define SI114X_PARAM_PSENCODE                  (0x05)
#define SI114X_PARAM_ALSENCODE                 (0x06)
#define SI114X_PARAM_PS1ADCMUX                 (0x07)
#define SI114X_PARAM_PS2ADCMUX                 (0x08)
#define SI114X_PARAM_PS3ADCMUX                 (0x09)
#define SI114X_PARAM_PSADCOUNTER               (0x0A)
#define SI114X_PARAM_PSADCGAIN                 (0x0B)
#define SI114X_PARAM_PSADCMISC                 (0x0C)
#define SI114X_PARAM_PSADCMISC_RANGE           (0x20)
#define SI114X_PARAM_PSADCMISC_PSMODE          (0x04)
#define SI114X_PARAM_ALSIRADCMUX               (0x0E)
#define SI114X_PARAM_AUXADCMUX                 (0x0F)
#define SI114X_PARAM_ALSVISADCOUNTER           (0x10)
#define SI114X_PARAM_ALSVISADCGAIN             (0x11)
#define SI114X_PARAM_ALSVISADCMISC             (0x12)
#define SI114X_PARAM_ALSVISADCMISC_VISRANGE    (0x20)
#define SI114X_PARAM_ALSIRADCOUNTER            (0x1D)
#define SI114X_PARAM_ALSIRADCGAIN              (0x1E)
#define SI114X_PARAM_ALSIRADCMISC              (0x1F)
#define SI114X_PARAM_ALSIRADCMISC_RANGE        (0x20)
#define SI114X_PARAM_ADCMUX_SMALLIR            (0x00)
#define SI114X_PARAM_ADCMUX_LARGEIR            (0x03)
/** @} */

/**
 * @name Si114x constants
 * @{
 */
#define SI1145_ID                              (0x45)
#define SI1146_ID                              (0x46)
#define SI1147_ID                              (0x47)
#define SI114X_STARTUP_TIME_MS                 (25UL)   /**< startup time (25ms) */
#define SI114X_WAIT_10MS                       (10UL)   /* 10ms */
#define SI114X_INIT_VALUE                      (0x17)
#define SI114X_UCOEF0_DEFAULT                  (0x29)
#define SI114X_UCOEF1_DEFAULT                  (0x89)
#define SI114X_UCOEF2_DEFAULT                  (0x02)
#define SI114X_UCOEF3_DEFAULT                  (0x00)
#define SI114X_ADC_REC_CLK                     (0x70) /* 511 ADC Clock */
/** @} */

/**
 * @name Si114x register bits
 * @{
 */
#define SI114X_EN_UV                           (0x80)
#define SI114X_EN_AUX                          (0x40)
#define SI114X_EN_ALS_IR                       (0x20)
#define SI114X_EN_ALS_VIS                      (0x10)

#define SI114X_EN_PS3_IE                       (0x10)
#define SI114X_EN_PS2_IE                       (0x08)
#define SI114X_EN_PS1_IE                       (0x04)
#define SI114X_EN_ALS_IE                       (0x01)

#define SI114X_INTCFG_INTOE                    (0x01)
#define SI114X_INTCFG_INTMODE                  (0x02)

#define SI114X_PS3_ALIGN                       (0x40)
#define SI114X_PS2_ALIGN                       (0x20)
#define SI114X_PS1_ALIGN                       (0x10)
#define SI114X_ALS_IR_ALIGN                    (0x20)
#define SI114X_ALS_VIS_ALIGN                   (0x10)
#define SI114X_PS_RANGE                        (0x20)
#define SI114X_PS_ADC_MODE                     (0x04)
#define SI114X_VIS_RANGE                       (0x20)
#define SI114X_IR_RANGE                        (0x20)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
