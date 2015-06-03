/*
 * msba2acc-smb380.h - Definitions of the Driver for the SMB380 acceleration
 * sensor.
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 **/

/**
 * @file
 * @internal
 * @brief   SMB380 acceleration sensor definitions for the LPC2387
 *
 * @author  Marco Ziegert <ziegert@inf.fu-berlin.de>
 * @author  Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 */

#ifndef SMB380_H_
#define SMB380_H_
#include <stdint.h>
#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SMB380_DEBUG_MESSAGE "SMB380 Driver Error: "

#define MSG_TYPE_SMB380_WAKEUP          814

#define SMB380_X_AXIS               0   /* X Axis-Name */
#define SMB380_Y_AXIS               1   /* Y Axis-Name */
#define SMB380_Z_AXIS               2   /* Z Axis-Name */

#define LPM_PREVENT_SLEEP_ACCSENSOR     BIT2

enum SMB380_MODE {
    SMB380_POLL,
    SMB380_POLL_AFTER_CONTINOUS,
    SMB380_CONTINOUS,
    SMB380_THRESHOLD,
    SMB380_FALSEALERT
};

volatile enum SMB380_MODE smb380_mode;

/* Writeable values to EEPROM: from 0x0A (control1) to 0x1D (offset_T).
   For writing enable ... flag and add eeprom_offset_address.

   Generally it is a good idea to read out shared register addresses before writing,
   registers 0x14 and 34h are especially critical.
*/

#define SMB380_EEPROM_OFFSET                0x20
/* Chip-ID Bit0-2, default: 010b */
#define SMB380_CHIP_ID                      0x00
/* Chip-ID mask */
#define SMB380_CHIP_ID_MASK                 0x07
/* ml_version Bit0-3 ; al_version Bit4-7 */
#define SMB380_AL_ML_VERSION                0x01
#define SMB380_AL_MASK                      0xF0
#define SMB380_ML_MASK                      0x0F
/* LSB_acc_x Bit6-7; new_data_x Bit0 */
#define SMB380_ACC_X_LSB_NEWDATA            0x02
/* MSB_acc_x Bit0-7 */
#define SMB380_ACC_X_MSB                    0x03
/* LSB_acc_y Bit6-7; new_data_y Bit0 */
#define SMB380_ACC_Y_LSB_NEWDATA            0x04
/* MSB_acc_y Bit0-7 */
#define SMB380_ACC_Y_MSB                    0x05
/* LSB_acc_z Bit6-7; new_data_z Bit0 */
#define SMB380_ACC_Z_LSB_NEWDATA            0x06
/* MSB_acc_z Bit0-7 */
#define SMB380_ACC_Z_MSB                    0x07
#define SMB380_ACC_LSB_MASK                 0xC0
#define SMB380_ACC_MSB_MASK                 0xFF
#define SMB380_ACC_NEWDATA_MASK             0x01
/* Temperature Bit0-7 */
#define SMB380_TEMP                         0x08
/* Status register, contains six flags */
#define SMB380_STATUS                       0x09
#define SMB380_STATUS_ST_RESULT_MASK        0x80
#define SMB380_STATUS_ALERT_PHASE_MASK      0x10
#define SMB380_STATUS_LG_LATCHED_MASK       0x08
#define SMB380_STATUS_HG_LATCHED_MASK       0x04
#define SMB380_STATUS_STATUS_LG_MASK        0x02
#define SMB380_STATUS_STATUS_HG_MASK        0x01
/* Control register - contains seven values, default: x000 0000b */
#define SMB380_CONTROL1                     0x0A
#define SMB380_CONTROL1_RESET_INT_MASK      0x40
#define SMB380_CONTROL1_UPDATE_MASK         0x20
#define SMB380_CONTROL1_EE_W_MASK           0x10
#define SMB380_CONTROL1_SELF_TEST_1_MASK    0x08
#define SMB380_CONTROL1_SELF_TEST_0_MASK    0x04
#define SMB380_CONTROL1_SOFT_RESET_MASK     0x02
#define SMB380_CONTROL1_SLEEP_MASK          0x01
/* Control register - contains six values, default: x000 0011b */
#define SMB380_CONTROL2                     0x0B
#define SMB380_CONTROL2_ALERT_MASK          0x80
#define SMB380_CONTROL2_ANY_MOTION_MASK     0x40
#define SMB380_CONTROL2_COUNTER_HG_MASK     0x30
#define SMB380_CONTROL2_COUNTER_LG_MASK     0x0C
#define SMB380_CONTROL2_ENABLE_HG_MASK      0x02
#define SMB380_CONTROL2_ENABLE_LG_MASK      0x01
/* default: 20 */
#define SMB380_LG_THRES                     0x0C
/* default: 150 */
#define SMB380_LG_DUR                       0x0D
/* default: 160 */
#define SMB380_HG_THRES                     0x0E
/* default: 150 */
#define SMB380_HG_DUR                       0x0F
/* default: 0 */
#define SMB380_ANY_MOTION_THRES             0x10
/* default: 0000 0000b */
#define SMB380_ANY_MOTION_DUR_HYST          0x1
#define SMB380_ANY_MOTION_DUR_MASK          0xC0
#define SMB380_ANY_MOTION_DUR_HG_HYST_MASK  0x38
#define SMB380_ANY_MOTION_DUR_LG_HYST_MASK  0x07
/* default: 162 */
#define SMB380_CUST1                        0x12
/* default: 13 */
#define SMB380_CUST2                        0x13
/* default: xxx0 1110b */
#define SMB380_CONTROL3                     0x14
#define SMB380_CONTROL3_RANGE_MASK          0x18
#define SMB380_CONTROL3_BANDWITH_MASK       0x07
/* default: 1000 0000b */
#define SMB380_CONTROL4                     0x15
#define SMB380_CONTROL4_SPI4_MASK           0x80
#define SMB380_CONTROL4_ENABLE_ADV_INT_MASK 0x40
#define SMB380_CONTROL4_NEW_DATA_INT_MASK   0x20
#define SMB380_CONTROL4_LATCH_INT_MASK      0x10
#define SMB380_CONTROL4_SHADOW_DIS_MASK     0x08
#define SMB380_CONTROL4_WAKEUP_PAUSE_MASK   0x06
#define SMB380_CONTROL4_WAKEUP_MASK         0x01

#define SMB380_OFFSET_LSB_GAIN_X            0x16
#define SMB380_OFFSET_LSB_GAIN_Y            0x17
#define SMB380_OFFSET_LSB_GAIN_Z            0x18
#define SMB380_OFFSET_LSB_GAIN_T            0x19
#define SMB380_OFFSET_LSB_MASK              0xC0
#define SMB380_OFFSET_GAIN_MASK             0x3F

#define SMB380_OFFSET_MSB_X                 0x1A
#define SMB380_OFFSET_MSB_Y                 0x1B
#define SMB380_OFFSET_MSB_Z                 0x1C
#define SMB380_OFFSET_MSB_T                 0x1D

#define SMB380_TEMP_OFFSET                  -30
#define SMB380_DEFAULT_MAXG                 4.0f
#define SMB380_READ_REGISTER                0x00
#define SMB380_WRITE_REGISTER               0x01
#define SMB380_WAKE_UP_PAUSE_20MS           0x00
#define SMB380_WAKE_UP_PAUSE_80MS           0x01
#define SMB380_WAKE_UP_PAUSE_320MS          0x02
#define SMB380_WAKE_UP_PAUSE_2560MS         0x03
#define SMB380_RANGE_2G                     0x00
#define SMB380_RANGE_4G                     0x01
#define SMB380_RANGE_8G                     0x02
#define SMB380_BAND_WIDTH_25HZ              0x00
#define SMB380_BAND_WIDTH_50HZ              0x01
#define SMB380_BAND_WIDTH_100HZ             0x02
#define SMB380_BAND_WIDTH_190HZ             0x03
#define SMB380_BAND_WIDTH_375HZ             0x04
#define SMB380_BAND_WIDTH_750HZ             0x05
#define SMB380_BAND_WIDTH_1500HZ            0x06
/* SMB380_RING_BUFF_SIZE * int16_t (2Byte) * 4 (x,y,z,Temp) = 512 Byte (for 64) */
#define SMB380_RING_BUFF_SIZE               256
/* TODO chsnge size to 2048 */
#define SMB380_RING_BUFF_MAX_THREADS        10
#define SMB380_SAMPLE_RATE_MAX              3000

/*
 * change from Header (public) to internal use (private)
 * (use it after every write to EEPROM).
 * set update_image Bit in control1 to
 * copie content from EEPROM (0x2B to 0x3D) to Image (0x0B to 0x1D)
 **/
void SMB380_update_image(void);

/*
 * change from Header (public) to internal use (private)
 * set ee_w Bit in control1 to
 * enable read to 0x16 to 0x22  and
 * enable write to 0x16 to 0x3D
 **/
void SMB380_enable_eeprom_default(void);
/* Example Hysterese function */
uint8_t SMB380_HystereseFunctionSample(int16_t *value);

/* Simple api for single-sample, single thread interrupt mode */
uint8_t SMB380_init_simple(uint16_t samplerate, uint8_t bandwidth,
                           uint8_t range);


/* Enables Interrupts (normally only once called) */
uint8_t SMB380_init(uint8_t (*func)(int16_t *));
void SMB380_setSampleRate(uint16_t rate);
uint16_t SMB380_getSampleRate(void);
void freeRingReadPointer(void);
void actualizeRingReadPointer(void);
uint8_t readRingBuff(int16_t *value);
uint8_t writeRingBuff(int16_t *value);
void SMB380_activateDynRangeSet(uint8_t activate);
uint8_t checkRange(int16_t *value);

void SMB380_enableEEPROM(void);
void SMB380_disableEEPROM(void);

/*  getter */
float SMB380_getSampleRatio(void);
void SMB380_getAcceleration(unsigned char axis, int16_t *pAbs, int16_t *pMg);
int16_t SMB380_getTemperature(void);
unsigned char SMB380_getChipID(void);
unsigned char SMB380_getWakeUpPause(void);
unsigned char SMB380_getBandWidth(void);
int16_t SMB380_getBandWidthAbs(void);
unsigned char SMB380_getRange(void);
unsigned char SMB380_getCustomerReg(void);
unsigned char SMB380_readOffset(uint16_t *offset);
unsigned char SMB380_readOffsetTemp(uint16_t *offset);
unsigned char SMB380_readGain(uint16_t *gain);
unsigned char SMB380_readGainTemp(uint16_t *gain);

/*  setter */
void SMB380_setTempOffset(uint16_t offset, uint8_t EEPROM);
void SMB380_setWakeUpPause(unsigned char duration);
void SMB380_setBandWidth(unsigned char bandWidth);
void SMB380_setRange(unsigned char range);
void SMB380_softReset(void);
void SMB380_setCustomerReg(unsigned char data);
void SMB380_setUpperLimit(void);
void SMB380_enableUpperLimit(void);
void SMB380_disableUpperLimit(void);
void SMB380_enableLowerLimit(void);
void SMB380_disableLowerLimit(void);
uint8_t SMB380_setAnyMotionLimit(uint16_t mg, uint16_t gvalueint);
void SMB380_enableAnyMotionLimit(void);
void SMB380_disableAnyMotionLimit(void);
void SMB380_enableNewDataInt(void);
void SMB380_disableNewDataInt(void);
void SMB380_resetInterruptFlags(void);
void SMB380_writeOffset(uint16_t *offset, uint8_t EEPROM);
void SMB380_writeOffsetTemp(uint16_t *offset, uint8_t EEPROM);

/*  stats */
void SMB380_ShowMemory(void);
void SMB380_Selftest_1(void);

#ifdef __cplusplus
}
#endif

#endif   /* SMB380_H_ */
