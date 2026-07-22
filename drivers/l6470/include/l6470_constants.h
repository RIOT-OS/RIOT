/*
 * Copyright (C) 2022 Samuel Kayode
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l6470
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Samuel Kayode <samkay014@gmail.com>
 */

#ifndef L6470_CONSTANTS_H
#define L6470_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register addresses
 * 
 */
#define L6470_REG_ABS_POS 0x01      /**< current position */
#define L6470_REG_EL_POS 0x02       /**< electrical position */
#define L6470_REG_MARK 0x03         /**< mark position */
#define L6470_REG_SPEED 0x04        /**< current speed */
#define L6470_REG_ACC 0x05          /**< current acceleration */
#define L6470_REG_MAX_SPEED 0x07    /**< maximum speed */
#define L6470_REG_MIN_SPEED 0x08    /**< minimum speed */
#define L6470_REG_FS_SPD 0x15       /**< full-step speed */
#define L6470_REG_KVAL_HOLD 0x09    /**< holding Kval */
#define L6470_REG_KVAL_RUN 0x0A     /**< constant speed Kval */
#define L6470_REG_KVAL_ACC 0x0B     /**< acceleration starting Kval */
#define L6470_REG_KVAL_DEC 0x0C     /**< deceleration starting Kval */
#define L6470_REG_INT_SPEED 0x0D    /**< intersect speed */
#define L6470_REG_ST_SLP 0x0E       /**< start slope */
#define L6470_REG_FN_SLP_ACC 0x0F   /**< acceleration final slope */
#define L6470_REG_FN_SLP_DEC 0x10   /**< deceleration final slope */
#define L6470_REG_K_THERM 0x11      /**< thermal compensation factor */
#define L6470_REG_ADC_OUT 0x12      /**< ADC output */
#define L6470_REG_OCD_TH 0x13       /**< OCD threshold */
#define L6470_REG_STALL_TH 0x14     /**< STALL threshold */
#define L6470_REG_STEP_MODE 0x16    /**< step mode */
#define L6470_REG_ALARM_EN 0x17     /**< alarm enable */
#define L6470_REG_CONFIG 0x18       /**< IC configuration */
#define L6470_REG_STATUS 0x19       /**< Status */

/**
 * @brief Application commands - headers
 * 
 */
#define L6470_CMD_HARD_STOP 0xB8      /**< hard stop motor */
#define L6470_CMD_SOFT_STOP 0xB0      /**< soft stop motor */
#define L6470_CMD_RESET_DEV 0xC0      /**< reset device */
#define L6470_CMD_GET_STATUS 0xD0     /**< get device status */
#define L6470_CMD_RESET_POS 0xD8      /**< set value in ABS_POS reg to zero */
#define L6470_CMD_GET_PARAM 0x20      /**< get value in reg */
#define L6470_CMD_RUN 0x50            /**< run motor at constant speed */
#define L6470_CMD_MOVE 0x40           /**< move motor by a number of steps */
#define L6470_CMD_GO_TO 0x60          /**< go to target position */
#define L6470_CMD_GO_TO_DIR 0x68      /**< go to target position */
#define L6470_CMD_GO_UNTIL 0x82       /**< go until SW pin triggered */
#define L6470_CMD_RELEASE_SW 0x92     /**< go until SW pin triggered */
#define L6470_CMD_SET_PARAM 0x00      /**< set value in reg */
#define L6470_CMD_GO_HOME 0x70        /**< go to zero (HOME) position */
#define L6470_CMD_GO_MARK 0x78        /**< go to MARK position */
#define L6470_CMD_SOFT_HI_Z 0xA0      /**< disable power bridges */
#define L6470_CMD_HARD_HI_Z 0xA8      /**< immediately disable power bridges */

/**
 * @brief Default values
 * 
 */
#define L6470_CONFIG_DEFAULT 0x2E88    /**< default value in config register */

/**
 * @brief Device profile
 * 
 */
#define L6470_MAX_DAISY_CHAIN_DEVS 40  /**< max daisy chain devices possible */
#define L6470_MAX_BYTES_TX 4           /**< max bytes that can be transmitted */
#define L6470_MAX_BYTES_RX 3           /**< max bytes that can be received */

#ifdef __cplusplus
}
#endif

#endif /**< L6470_CONSTANTS_H */
/** @} */
