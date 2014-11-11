/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    lm75a LM75A
 * @ingroup     drivers
 * @brief       Driver for the LM75A digital temperature sensor and thermal watchdog
 *
 * The connection between the MCU and the LM75A is based on the i2c-interface.
 *
 * @{
 *
 * @file
 * @internal
 * @brief       Definitions of the LM75A temperature sensor driver.
 *
 * The connection between the LM75A and the MCU is based on the I2C-interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 */

#ifndef LM75A_H_
#define LM75A_H_

#include <stdint.h>
#include <math.h>
#include "periph/i2c.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LM75A register addresses */
#define LM75A_ADDR                      0x49
#define LM75A_TEMPERATURE_REG           0x0
#define LM75A_CONFIG_REG                0x1
#define LM75A_THYST_REG                 0x2
#define LM75A_OVER_TEMP_REG             0x3

/* Define the used I2C Interface */
#define LM75A_I2C_INTERFACE I2C_0                // P0.27 SDA0, P0.28 SCL0
//#define LM75A_I2C_INTERFACE             I2C1_0    // P0.0  SDA1, P0.1  SCL1
//#define LM75A_I2C_INTERFACE I2C1_1              // P0.19 SDA1, P0.20 SCL1
//#define LM75A_I2C_INTERFACE I2C2                // P0.10 SDA2, P0.11 SCL2

/* LM75A operation modes */
enum OPERATION_MODES {
    LM75A_NORMAL_OPERATION_MODE,
    LM75A_SHUTDOWN_MODE,
    LM75A_COMPARATOR_MODE,
    LM75A_INTERRUPT_MODE
};

/*Common definitions for LMA75A  */
#define LM75A_BIT0                      0x0
#define LM75A_BIT1                      0x1
#define LM75A_BIT2                      0x2
#define LM75A_BIT3                      0x3
#define LM75A_BIT4                      0x4
#define LM75A_BIT5                      0x5
#define LM75A_BIT6                      0x6
#define LM75A_BIT7                      0x7
#define LM75A_BIT8                      0x8
#define LM75A_BIT9                      0x9
#define LM75A_BIT10                     0xA
#define LM75A_BIT15                     0xF
#define LM75A_MOST_SIG_BYTE_MASK        0xFF00
#define LM75A_LEAST_SIG_BYTE_MASK       0x00FF
#define LM75A_DATA_BITS_MASK            0x07FF
#define LM75A_SIGN_BIT_MASK             (1<<LM75A_BIT10)
#define LM75A_LSB_MASK                  0x1
#define LM75A_EXTINT_MODE               0x1

/* LM75A configuration register */
#define LM75A_ACTIVE_LOW                0
#define LM75A_ACTIVE_HIGH               1
#define LM75A_DEFAULT_CONFIG_VALUE      0

enum FAULT_QUEUE_VALUES {
    LM75A_ONE_FAULT = 1,
    LM75A_TWO_FAULT = 2,
    LM75A_FOUR_FAULT = 4,
    LM75A_SIX_FAULT = 6
};

/* LM75A default values */
enum DEFAULT_VALUES {
    LM75A_DEFAULT_TOS = 80,
    LM75A_DEFAULT_THYST = 75,
    LM75A_DEFAULT_OPERATION = LM75A_NORMAL_OPERATION_MODE,
    LM75A_DEFAULT_MODE = LM75A_COMPARATOR_MODE,
    LM75A_DEFAULT_POLARITY = LM75A_ACTIVE_LOW,
    LM75A_DEFAULT_FAULT_NUM = LM75A_ONE_FAULT
};

/*define inter-threads messages */
#define LM75A_EXIT_MSG                  0
#define LM75A_SAMPLING_MSG              1
#define LM75A_SLEEP_MSG                 2
#define LM75A_WEAKUP_MSG                3

/**
 * @brief       Set the over-temperature shutdown threshold (TOS).
 *
 * @param[in] tos       the TOS value.
 *
 */
void lm75A_set_over_temperature(float_t tos);


/**
 * @brief       Set the hysteresis temperature.
 *
 * @param[in]  thsyt    the hysteresis value.
 *
 */
void lm75A_set_hysteresis_temperature(float_t thsyt);


/**
 * @brief       Set various operation modes of the temperature sensor.
 *              The LM75A provide four modes: normal, comparator, interrupt,
 *              and the shutdown mode.
 *              All these modes are defined in the lm75a-temp-sensor.h
 *
 * @param[in]  op_mode  the operation mode value: the normal, shutdown,
 *             comparator, or interrupt mode.
 *
 */
void lm75A_set_operation_mode(uint8_t op_mode);


/**
 * @brief       Get the content of the configuration register.
 *
 * @return the configuration register value.
 *
 */
uint8_t lm75A_get_config_reg(void);


/**
 * @brief       Get the adjusted hysteresis temperature.
 *
 * @return the content of the hysteresis register.
 *
 */
float_t lm75A_get_hysteresis_temperature(void);


/**
 * @brief       Get the adjusted over-temperature shutdown threshold (TOS).
 *
 * @return the content of the TOS-register.
 *
 */
float_t lm75A_get_over_temperature(void);


/**
 * @brief       Get the ambient temperature which is measured from the
 *              LM75A sensor.
 *
 * @return the content of the temperature register.
 *
 */
float_t lm75A_get_ambient_temperature(void);


/**
 * @brief       Set the LM75A sensor in the initial state.
 *              The temperature sensor has the following values in this state:
 *              config_register = 0; hyst_register = 75; the tos_reg = 80.
 *
 */
void lm75A_reset(void);


/**
 * @brief       Start a continuous sampling of the temperature values.
 *              This function prints the values of all registers over
 *              the rs232 interface.
 *
 * @param[in] external_interr_handler   pointer to an external task handler
 *                                      which is performed, if an external
 *                                      interrupt is occurred and the external
 *                                      subroutine is leaved. This parameter
 *                                      is optional, the NULL-value can be
 *                                      entered.
 */
void lm75A_start_sensor_sampling(void (*extern_interrupt_task)(void));


/**
 * @brief       Register an interrupt handler for the external interrupt.
 *              Only the port0 and port2 are supported.
 *
 * @param[in] port              port number.
 * @param[in] pin_bit_mask      pin number in form of a bit mask: Pin0 --> BIT0,
 *                              Pin1 --> BIT1, Pin2 --> BIT2 = 2^2 = 4
 * @param[in] flags             define if the interrupt is generated on rising
 *                              or falling edge (#GPIOINT_RISING_EDGE,
 *                              #GPIOINT_FALLING_EDGE).
 * @param[in] handler           pointer to an interrupt handler.
 *
 * @return true if the the external interrupt handler is successfully
 *         registered, otherwise false.
 */
bool lm75A_ext_irq_handler_register(int32_t port, uint32_t pin_bit_mask,
                                    int32_t flags, void *handler);


/**
 * @brief       Initialize the LM75A temperature sensor.
 *              The baud rate and the handler for the external interrupt can be
 *              initialized. The external interrupt handler is optional, if no
 *              handler is available, the NULL-value can be entered.
 *              The hysteresis and the over-temperature are displayed before and
 *              after a rest action is performed. After this the LM7A sensor is
 *              set in the interrupt or the comparator mode.
 *
 * @param[in] i2c_interface             the i2c interface, several interfaces
 *                                      can be selected: i2c0, i2c1 and i2c2.
 * @param[in] baud_rate                 the baud rate.
 * @param[in] external_interr_handler   pointer to a handler for the external
 *                                      interrupt.
 *
 * @return true if the I2C interface and the external interrupt handler are
 *         successfully initialized, otherwise false.
 */
bool lm75A_init(uint8_t i2c_interface, uint32_t baud_rate,
                void *external_interr_handler);


/**
 * @brief       Register the external interrupt handler for the over-temperature
 *              shutdown output.
 *
 * @param[in] handler   pointer to a handler for the external interrupts.
 *
 * @return true if the the external interrupt handler is successfully
 *         registered, otherwise false.
 */
bool lm75A_external_interrupt_register(void *handler);


/**
 * @brief       Alarm the sensor sampling task about an external interrupt.
 *
 * @param[in] b is true if an external interrupt is occurred, otherwise false.
 *
 */
void lm75A_set_in_alarm(bool b);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LM75A_H_ */
