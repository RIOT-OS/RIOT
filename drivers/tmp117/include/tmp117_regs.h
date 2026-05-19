/*
 * tmp117_regs.h
 *
 *  Created on: 13 mai 2026
 *      Author: leo
 */

#ifndef CUSTOM_MODULES_TMP117_INCLUDE_TMP117_REGS_H_
#define CUSTOM_MODULES_TMP117_INCLUDE_TMP117_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    TMP117 registers
 * @{
 */
#define TMP117_REG_TEMP_RESULT       (0x00) /**<  */
#define TMP117_REG_CONFIG		     (0x01) /**<  */
#define TMP117_REG_TEMP_HIGH_LIMIT   (0x02) /**<  */
#define TMP117_REG_TEMP_LOW_LIMIT    (0x03) /**<  */
#define TMP117_REG_EEPROM_UNLOCK     (0x04) /**<  */
#define TMP117_REG_EEPROM_1          (0x05) /**<  */
#define TMP117_REG_EEPROM_2          (0x06) /**<  */
#define TMP117_REG_TEMP_OFFSET       (0x07) /**<  */
#define TMP117_REG_EEPROM_3          (0x08) /**<  */
#define TMP117_REG_DEVICE_ID         (0x0F) /**<  */
/** @} */


/**
 * @name    TMP117 shift mask
 * @{
 */
#define TMP117_DATA_READY_SHIFT    	13 /**<  */
#define TMP117_CONV_MODE_SHIFT    	10 /**<  */
#define TMP117_CONV_CYCLE_SHIFT   	7 /**<  */
#define TMP117_AVG_SHIFT    		5 /**<  */
/** @} */

/**
 * @name    TMP117 field mask
 * @{
 */
#define TMP117_CONV_MODE_MASK      	(0x3 << TMP117_CONV_MODE_SHIFT) /**<  */
#define TMP117_CONV_CYCLE_MASK      (0x7 << TMP117_CONV_CYCLE_SHIFT) /**<  */
#define TMP117_AVG_MASK      		(0x3 << TMP117_AVG_SHIFT) /**<  */
#define TMP117_DATA_READY_MASK      (0x1 << TMP117_DATA_READY_SHIFT) /**<  */

/** @} */




/**
 * @name    TMP117 registers - default values
 * @{
 */

#define TMP117_REG_DEVICE_ID_DEFAULT_VAL         (0x0117) /**<  */
/** @} */


#ifdef __cplusplus
}
#endif



#endif /* CUSTOM_MODULES_TMP117_INCLUDE_TMP117_REGS_H_ */
