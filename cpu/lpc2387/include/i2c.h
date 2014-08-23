/*
 * i2c.h - Definitions for the I2C interface and the functions
 * for the LPC2387 chip.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @internal
 * @brief       The I2C interface driver definitions for the LPC2387 chip.
 *              The driver is full abstracted supporting all the i2c-interfaces
 *              of the LPC2387 chip. The user need only to give the requested
 *              i2c-interface and the transmission baudrate. The user can
 *              optionally give its own master interrupt handler. If the user
 *              does not declare a handler, an appropriate interrupt is
 *              automatically registered for the specific i2c-interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Marco Ziegert <ziegert@inf.fu-berlin.de>
 * @author      Benjamin Aschenbrenner
 * @version     $Revision: 3857 $
 *
 * @note        $Id: i2c.h 3857 2013-09-02 18:11:27 kasmi $
 */

#ifndef __I2C_H
#define __I2C_H

#include <stdbool.h>
#include <stdint.h>

#define I2C_BUFSIZE             0x23
#define MAX_TIMEOUT             0x00FFFFFF

#define I2CMASTER               0x01
#define I2CSLAVE                0x02

//define slave addresses and data registers

#define RD_BIT                  0x01
#define AUTO_INC_BIT            0x80
#define WRITE_ENABLE_BIT_MASK   0XFE
#define READ_ENABLE_BIT_MASK    0x1

#define I2C_IDLE                0
#define I2C_STARTED             1
#define I2C_RESTARTED           2
#define I2C_REPEATED_START      3
#define DATA_ACK                4
#define DATA_NACK               5

#define I2CONSET_I2EN           0x00000040  /* I2C Control Set Register */
#define I2CONSET_AA             0x00000004
#define I2CONSET_SI             0x00000008
#define I2CONSET_STO            0x00000010
#define I2CONSET_STA            0x00000020

#define I2CONCLR_AAC            0x00000004  /* I2C Control clear Register */
#define I2CONCLR_SIC            0x00000008
#define I2CONCLR_STAC           0x00000020
#define I2CONCLR_I2ENC          0x00000040

#define I2DAT_I2C               0x00000000  /* I2C Data Reg */
#define I2ADR_I2C               0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH             45          /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL             45          /* I2C SCL Duty Cycle Low Reg */

//I2C interfaces
#define I2C0                    0 // P0.27 SDA0, P0.28 SCL0
#define I2C1_0                  1 // P0.0  SDA1, P0.1  SCL1
#define I2C1_1                  2 // P0.19 SDA1, P0.20 SCL1
#define I2C2                    3 // P0.10 SDA2, P0.11 SCL2


/* Functions definitions */

/**
 * @brief       Initialize the I2C-Interface with the appropriate operation
 *              mode, baud rate and other parameter.
 *              The user can optionally give a master interrupt handler. If the
 *              user does not declare a handler, an appropriate interrupt is
 *              automatically registered for the specific i2c interface.
 *
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 * @param[in] i2c_mode          the operating mode.
 * @param[in] slave_addr        the slave address.
 * @param[in] baud_rate         the baud rate.
 * @param[in] handler           the I2C2 Interrupt handler (optional); by giving
 *                              a NULL as parameter, an appropriate interrupt is
 *                              automatically selected.
 *
 * @return true if the I2C interrupt handler was installed correctly, otherwise
 *         false.
 */
bool i2c_initialize(uint8_t i2c_interface, uint32_t i2c_mode,
                    uint8_t slave_addr, uint32_t baud_rate, void *handler);


/**
 * @brief       Read from an appropriate slave device over the I2C-Interface.
 *              The read values are stored in a buffer, which it is handed to
 *              the function.
 *
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 * @param[in] slave_addr        the slave address
 * @param[in] reg_addr          a register address, which is residing in the
 *                              slave device.
 * @param[out] rx_buff          a pointer to a receive buffer.
 * @param[in] rx_buff_length    the number of receiving bytes resp.
 *                              the receive-buffer length.
 *
 * @return true if the read transaction is successfully completed, otherwise
 *         false.
 */
bool i2c_read(uint8_t i2c_interface, uint8_t slave_addr, uint8_t reg_addr,
              uint8_t *rx_buff, uint8_t rx_buff_length);


/**
 * @brief       Write to an appropriate slave device over the I2C-Interface.
 *              The values are stored in a buffer, which it is handed to the
 *              function.
 *
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 * @param[in] slave_addr        the slave address
 * @param[in] reg_addr          a register address, which is residing in the
 *                              slave device.
 * @param[in] tx_buff           pointer to a transmitting buffer.
 * @param[in] tx_buff_length    the number of transmitting bytes resp.
 *                              the transmit-buffer length.
 *
 * @return true if the write transaction is successfully completed, otherwise
 *         false.
 */
bool i2c_write(uint8_t i2c_interface, uint8_t slave_addr, uint8_t reg_addr,
               uint8_t *tx_buff, uint8_t tx_buff_length);


/**
 * @brief       Simultaneous sending and receiving to/from the I2C-Interface.
 *              The sending/receiving bytes are stored in two buffers, which are
 *              handed to the function.
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 * @param[in] slave_addr        slave address
 * @param[in] tx_buff           pointer to a sending buffer.
 * @param[in] tx_buff_length    the number of sending bytes resp.
 *                              the send-buffer length.
 * @param[out] rx_buff          pointer to a receive buffer.
 * @param[in] rx_buff_length    the number of receiving bytes resp.
 *                              the receive-buffer length.
 *
 * @return true if the write/receive transaction are successfully completed,
 *         otherwise false.
 */
bool i2c_trans_receive(uint8_t i2c_interface, uint8_t slave_addr,
                       uint8_t *tx_buff, uint8_t tx_buff_length,
                       uint8_t *rx_buff, uint8_t rx_buff_length);


/**
 * @brief       Enable the appropriate I2C interface.
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 *
 */
void i2c_active_power(uint8_t i2c_interface);


/**
 * @brief       Select the appropriate pins for the I2C interface.
 *              It selects the respective Serial Data Line (SDAx) and Serial
 *              Clock (SCLx).
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 *
 */
void i2c_pin_select(uint8_t i2c_interface);


/**
 * @brief       Clear the I2C control register.
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 *
 */
void i2c_clear_control_register(uint8_t i2c_interface);


/**
 * @brief       Set the I2C baud rate.
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 * @param[in] baud_rate         the baud rate.
 *
 */
void i2c_set_baud_rate(uint8_t i2c_interface, uint32_t baud_rate);


/**
 * @brief       Register an interrupt handler for the I2C interface.
 *              The user can register his own irq-handler.
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 * @param[in] handler           the i2c irq-handler.
 *
 * @return true if the the interrupt handler is successfully registered,
 *         otherwise false.
 *
 */
bool i2c_irq_handler_register(uint8_t i2c_interface, void *handler);


/**
 * @brief       This routine complete a I2C transaction from start to stop
 *              conditions.
 *              All the intermitten steps are handled in the interrupt handler.
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 *
 * @return false if the start condition can never be generated and timed out,
 *         otherwise true.
 *
 */
bool i2c_transaction(uint8_t i2c_interface);


/**
 * @brief       Enable master transmitter mode for the appropriate
 *              i2c-interface.
 *
 * @param[in]   i2c_interface   the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 *
 */
void i2c_initial_master_transmitter_mode(uint8_t i2c_interface);


/**
 * @brief       Set slave mode for the appropriate i2c-interface.
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 * @param[in] slave_addr        the slave address.
 *
 */
void i2c_set_slave_mode(uint8_t i2c_interface, uint8_t slave_addr);


/**
 * @brief       Clear a number of bytes in a buffer.
 *
 * @param[in] ptr       pointer to a buffer
 * @param[in] size      the number of bytes, that must be cleared.
 *
 */
void i2c_clear_buffer(void *ptr, uint32_t size);

/**
 * @brief       Create a I2C start condition.
 *
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 *
 * @return false if the start condition is timed out, otherwise true.
 *
 */
bool i2c_start(uint8_t i2c_interface);


/**
 * @brief       Set a I2C stop condition.
 *              If the function never exit, it's a fatal bus error
 *
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 *
 * @return true if the stop condition is issued, otherwise it never returns.
 *
 */
bool i2c_stop(uint8_t i2c_interface);


/**
 * @brief       Enable the on-chip pull-up resistor for the appropriate
 *              i2c-interface.
 *
 * @param[in] i2c_interface     the i2c interface, only the i2c1 and the
 *                              i2c2-interfaces are supported. The pin mode
 *                              cannot be selected for pins P0[27] to P0[31].
 *                              Pins P0[27] and P0[28] are dedicated I2C
 *                              open-drain pins without pull-up/down.
 */
void i2c_enable_pull_up_resistor(uint8_t i2c_interface);


/**
 * @brief       Disable the on-chip pull-up resistor for the appropriate
 *              i2c-interface.
 *
 * @param[in] i2c_interface     the i2c interface, only the i2c1 and the
 *                              i2c2-interfaces are supported. The pin mode
 *                              cannot be selected for pins P0[27] to P0[31].
 *                              Pins P0[27] and P0[28] are dedicated I2C
 *                              open-drain pins without pull-up/down.
 */
void i2c_disable_pull_up_resistor(uint8_t i2c_interface);

#endif /* end __I2C_H */
