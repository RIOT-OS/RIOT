/*
 * Copyright (C) 2022 Samuel Kayode
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_l6470 L6470 driver
 * @ingroup     drivers_actuators
 * @brief       Device driver for the ST L6470 microstepping motor driver with
 * motion engine and SPI
 *
 * ### Introduction
 *
 * The driver allows the use of the L6470 motor driver commanded and configured
 * via SPI to controlling a microstepping motor using PWM. The driver is
 * compatible with both standard SPI and daisy chain SPI. The microstepping
 * resolution ranges from full-step to 1/128-microstep. The default step mode is
 * full-step.
 *
 * ## Supported features of the device are:
 * - motor speed and acceleration control
 * - positioning commands
 * - motion commands
 * - stop commands
 * - device configuration
 * - device reset
 * - reading and writing of select registers
 *
 * ## Driver Design
 *
 * To command the stepper motor, the user must initially apply a management
 * command which fills the private driver buffer with said command to be sent
 * to the L6470 device on calling the SPI commanding function. This design
 * provides flexibility for daisy chain SPI by allowing for different commands
 * to be sent to the motors.
 *
 * The management commands are prefixed with l6470_mgmt_. When calling a
 * management function, the ID of the pertinent L6470 device must be specified.
 * For standard SPI, this ID must be specified as zero. For daisy chain SPI, the
 * required ID is in the range 0 -(number of daisy chain devices specified - 1).
 *
 * A maximum of one management command is permitted before calling the SPI
 * commanding function. Applying more than one management function may result in
 * an incomplete command sent to the device or a complete overwrite of the
 * previous command.
 *
 * @{
 *
 * @file
 *
 * @author      Samuel Kayode <samkay014@gmail.com>
 */

#ifndef L6470_H
#define L6470_H

#include "periph/gpio.h"
#include "periph/spi.h"

#include "l6470_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the API of the driver */

/**
 * @brief   Driver error codes
 */
typedef enum {
    L6470_OK,                   /**< no error */
    L6470_ERROR_SPI,            /**< SPI communication error */
    L6470_ERROR_ID,             /**< ID outside of range */
} l6470_error_codes_t;

/**
 * @brief   Step modes
 *
 */
typedef enum {
    L6470_STEP_FULL,            /**< Full-step */
    L6470_STEP_HALF,            /**< Half-step */
    L6470_STEP_MICRO_4,         /**< 1/4 microstep */
    L6470_STEP_MICRO_8,         /**< 1/8 microstep */
    L6470_STEP_MICRO_16,        /**< 1/16 microstep */
    L6470_STEP_MICRO_32,        /**< 1/32 microstep */
    L6470_STEP_MICRO_64,        /**< 1/64 microstep */
    L6470_STEP_MICRO_128,       /**< 1/128 microstep */
} l6470_step_mode_t;

/**
 * @brief   Motor displacement
 *
 * In agreement with the selected step mode, the motor traverses step microsteps
 * in the desired direction (forward or reverse). The microstep magnitude is in
 * agreement with the selected step mode.
 */
typedef struct {
    uint32_t step : 24;
    uint8_t direction : 1;
} l6470_step_t;

/**
 * @brief   Motor speed control
 *
 * The motor traverses at the given speed in the desired direction.
 * The ABS_POS (Absolute Position) register is reset if act = 0 or the current
 * ABS_POS is copied into the MARK register if ACT = 1, upon the occurence of
 * the external switch turn on event which causes the motor to stop. The minimum
 * speed ranges from 0 to 976.3 step/s. The maximum speed ranges from
 * 15.25 to 15610 step/s. Speed is given in steps/tick.
 */
typedef struct {
    uint32_t speed : 24;
    uint8_t act : 1;
    uint8_t direction : 1;
} l6470_speed_t;

/**
 * @brief   Motor position
 *
 * The motor travels to target position in the desired direction. The target
 * position is in agreement with the selected step mode. The target position
 * ranges from -2097152 to +2097151.
 */
typedef struct {
    uint32_t target : 24;
    uint8_t direction : 1;
} l6470_position_t;

/**
 * @brief   Utility buffers
 *
 * The invoked management commands (l6470_mgmt_) are stored in the xmit buffer
 * before relaying the commands to the device. Some commands require arguments,
 * and this is described by L6470_MAX_BYTES_TX.
 *
 * Buffer recv directly receives the data from the device. L6470_MAX_BYTES_RX
 * defines the maximum number of packets expected.
 */
typedef struct {
    uint8_t xmit[L6470_MAX_BYTES_TX][L6470_MAX_DAISY_CHAIN_DEVS];
    uint8_t recv[L6470_MAX_BYTES_RX][L6470_MAX_DAISY_CHAIN_DEVS];
} l6470_comms_t;

/**
 * @brief L6470 device initialization parsmeters
 *
 */
typedef struct {
    spi_t dev;
    gpio_t cs;
    spi_mode_t spi_mode;
    spi_clk_t clk;
#if IS_USED(MODULE_L6470_STBY) || DOXYGEN
    gpio_t stby;
#endif
    l6470_step_mode_t step_mode;
    uint8_t spi_daisy_chain_devs;
} l6470_params_t;

/**
 * @brief L6470 device data structure
 *
 */
typedef struct {
    l6470_params_t params;
    l6470_comms_t priv;
} l6470_t;

/**
 * @brief   Initialize the L6470 device
 *
 * This function intializes the L6470 device according to the provided
 * configuration parameters. The startup position of the motor is designated as
 * the HOME position. If MODULE_L6470_SW is used, easy initialization of the
 * device is performed by spinning the motor at a specified minimum speed until
 * the external switch is triggered, hence stopping the motor.
 *
 * @param[inout] dev        Device descriptor of the device(s) to initialize
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success or
 * @return                  < 0 on error
 */
int l6470_init(l6470_t *dev, const l6470_params_t *params);

/**
 * @brief   Reads the value at the specified register address
 *
 * This mangement function is for the purpose of reading the current value
 * stored at the register address of the specifed device (dev_id). The response
 * length varies between 1 and 3 bytes. If standard SPI, the device ID must be
 * specified as zero.
 *
 * @param dev           Device descriptor of the device(s)
 * @param reg           Register address to read data from
 * @param dev_id        ID of daisy chain SPI device to read from
 *
 * @return              0 on sucess
 * @return              < 0 on error
 */
int l6470_mgmt_get_param(l6470_t *dev, uint8_t reg, int dev_id);

/**
 * @brief   Sets the value at the specified register address
 *
 * This management function is for the purpose of setting the value stored at
 * the register address of the specified device (dev_id). Not all registers can
 * be written. Some registers can only be written in certain conditions.
 *
 * @param dev           Device descriptor of the device(s)
 * @param reg           Register address to write data to
 * @param val           Value to write to register
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_set_param(l6470_t *dev, uint8_t reg, uint32_t val, int dev_id);

/**
 * @brief   Motor runs at given speed
 *
 * This management function is for the purpose of spinning the motor of the
 * specified device (dev_id) at a given speed in the desired direction. The
 * units of the speed specified is in step/tick. The given speed should be in
 * the devices minimum-maximum speed range.
 *
 *
 * @param dev           Device descriptor of the device
 * @param fast          Target speed and direction
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on sucess
 * @return              < 0 on error
 */
int l6470_mgmt_run(l6470_t *dev, l6470_speed_t fast, int dev_id);

/**
 * @brief   Motor traverses the given microsteps
 *
 * This management function is for the purpose of motor displacement in the
 * provided microstep magnitude in the desired direction for the specified
 * device (dev_id).
 *
 * @param dev           Device descriptor of the device
 * @param rotate        Microsteps and direction
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_move(l6470_t *dev, l6470_step_t rotate, int dev_id);

/**
 * @brief   Motor travels to given position
 *
 * This management function is for the purpose of motor travel to a given
 * position via the shortest path for the specified device (dev_id).
 *
 * @param dev           Device descriptor of the device
 * @param position      Target absolute position
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_go_to(l6470_t *dev, l6470_position_t position, int dev_id);

/**
 * @brief   Motor travels to given position
 *
 * This management function is for the purpose of motor travel to a given
 * position in the desired direction for the specified device (dev_id). The
 * given position is in concordance with the selected step mode. The absolute
 * position ranges from -2097152 to +2097151.
 *
 * @param dev           Device descriptor of the device
 * @param position      Target absolute position and direction
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_go_to_dir(l6470_t *dev, l6470_position_t position, int dev_id);

/**
 * @brief   Motor travels at specified speed until external switch triggered
 *
 * This management function is for the purpose of running the motor at a given
 * speed in a desired direction for the specified device (dev_id) until the SW 
 * pin detects that a connection to ground was established. The ground 
 * connection could be setup via an external switch or by setting the SW pin to
 * low.
 *
 * @param dev           Device descriptor of the device
 * @param fast          Speed, direction and action
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_go_until(l6470_t *dev, l6470_speed_t fast, int dev_id);

/**
 * @brief   Motor travels at minimum speed until external switch triggered
 *
 * This management function is for the purpose of running the motor at the
 * minimum speed in a desired direction for the specified device (dev_id) until 
 * the SW pin detects that a connection to ground was established. The SW pin 
 * is internally pulled up to Vdd. The ground connection could be setup via an 
 * external switch or by setting the SW pin to low.
 *
 * @param dev           Device descriptor of the device
 * @param fast          Direction and action
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_release_sw(l6470_t *dev, l6470_speed_t fast, int dev_id);

/**
 * @brief   Motor travels to HOME position
 *
 * This management function is for the purpose of motor travel to the HOME
 * position (0) via the shortest path for the specified device (dev_id).
 *
 * @param dev           Device descriptor of the device
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_go_home(l6470_t *dev, int dev_id);

/**magnitude
 * @brief   Motor travels to MARK position
 *
 * This management function is for the purpose of motor travel to the MARK
 * position (position stored in MARK register) via the shortest path for the
 * specified device (dev_id).
 *
 * @param dev           Device descriptor of the device
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_go_mark(l6470_t *dev, int dev_id);

/**
 * @brief   Set current position as HOME position
 *
 * This management function is for the purpose of designating the current
 * position as HOME for the specified device (dev_id). This is performed by
 * resetting the ABS_POS register to zero.
 *
 * @param dev           Device descriptor of the device
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_reset_pos(l6470_t *dev, int dev_id);

/**
 * @brief  Reset device
 *
 * This management function is for the purpose of reset to power-up conditions
 * for the specified device (dev_id).
 *
 * @param dev           Device descriptor of the device
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_reset_dev(l6470_t *dev, int dev_id);

/**
 * @brief  Soft stop device
 *
 * This management function is for the purpose of gradual deceleration to zero
 * speed for the specified device (dev_id). The deceleration value is stored in
 * the DEC register.
 *
 * @param dev           Device descriptor of the device
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_soft_stop(l6470_t *dev, int dev_id);

/**
 * @brief  Hard stop device
 *
 * This management function is for the purpose of infinite deceleration to zero
 * speed for the specified device (dev_id).
 *
 * @param dev           Device descriptor of the device
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_hard_stop(l6470_t *dev, int dev_id);

/**
 * @brief  Put device in high impedance state
 *
 * This management function is for the purpose of disabling the power bridges
 * after a deceleration to zero speed for the specified device (dev_id). The
 * deceleration value is stored in the DEC register.
 *
 * @param dev           Device descriptor of the device
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_soft_hi_z(l6470_t *dev, int dev_id);

/**
 * @brief  Immediately put device in high impedance state
 *
 * This management function is for the purpose of immediately disabling the
 * power bridges for the specified device (dev_id).
 *
 * @param dev           Device descriptor of the device
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_hard_hi_z(l6470_t *dev, int dev_id);

/**
 * @brief  Get device status
 *
 * This management function is for the purpose of checking the
 * status of the specified device (dev_id).
 *
 * @param dev           Device descriptor of the device
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_get_status(l6470_t *dev, int dev_id);

/**
 * @brief  Set step mode
 *
 * This management function is for the purpose of setting the step mode for the
 * specified device (dev_id).
 *
 * @param dev           Device descriptor of the device
 * @param mode          Step mode
 * @param dev_id        ID of daisy chain SPI device to write to
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_mgmt_set_step_mode(l6470_t *dev, l6470_step_mode_t mode, int dev_id);

/**
 * @brief  SPI communication
 *
 * This function transmits the commands stored in the xmit buffer to all
 * pertinent devices and receives the responses, if any, from these devices. The
 * received response is later stored in the recv buffer.
 *
 * @param dev           Device descriptor of the device
 * @param recv          User buffer to store response in
 *
 * @return              0 on success
 * @return              < 0 on error
 */
int l6470_send_command(l6470_t *dev, uint8_t *recv);

#if IS_USED(MODULE_L6470_STBY) || DOXYGEN
/**
 * @brief Resets device
 * 
 * This function resets the device when the reset GPIO pin is pulled low, 
 * putting it into standby mode.
 * 
 * @param dev           Device descriptor of the device
 * 
 * @return              0 on success
 * @return              < 0 0n error
 */
int l6470_reset_dev(const l6470_t *dev);
#endif /* IS_USED(MODULE_L6470_STBY) || DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /**< L6470_H */
/** @} */
