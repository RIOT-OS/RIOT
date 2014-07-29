/*
 * i2c.c - implementation of the I2C interface for the LPC2387 chip.
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @file
 * @internal
 * @brief       The I2C interface drivers for the LPC2387 chip.
 *              The driver is full abstracted supporting all the i2c-interfaces
 *              of the LPC2387 chip. The user need only to give the requested
 *              i2c-interface and the transmission baudrate.
 *              The user can optionally give a master interrupt handler. If the
 *              user does not declare a handler, an appropriate interrupt is
 *              automatically registered for the specific i2c interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Marco Ziegert <ziegert@inf.fu-berlin.de>
 * @author      Benjamin Aschenbrenner
 * @version     $Revision: 3858 $
 *
 * @note        $Id: i2c.c 3858 2013-09-02 18:11:17 kasmi $
 */

#include "lpc23xx.h"
#include "lpc2387.h"
#include "i2c.h"
#include "VIC.h"
#include "irq.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

volatile uint32_t i2c_master_state = I2C_IDLE;
volatile uint32_t i2c_slave_state = I2C_IDLE;
volatile uint32_t i2c_cmd;
volatile uint32_t i2c_mode;
volatile uint8_t  i2c_master_buffer[I2C_BUFSIZE];
volatile uint32_t i2c_read_length;
volatile uint32_t i2c_write_length;
volatile uint32_t rd_index = 0;
volatile uint32_t wr_index = 0;

static void i2c_interface0_master_handler(void) __attribute__((interrupt(
            "IRQ")));
static void i2c_interface1_master_handler(void) __attribute__((interrupt(
            "IRQ")));
static void i2c_interface2_master_handler(void) __attribute__((interrupt(
            "IRQ")));

bool i2c_initialize(uint8_t i2c_interface, uint32_t i2c_mode,
                    uint8_t slave_addr, uint32_t baud_rate, void *handler)
{
    //puts("i2c_initialize begin...\n");
    printf("i2cInterface = %d\n", i2c_interface);
    i2c_clear_buffer((uint8_t *) i2c_master_buffer,
                     I2C_BUFSIZE * sizeof(uint8_t));

    //activate power for I2C2
    i2c_active_power(i2c_interface);

    //select I2C2 functionality for pins x.xx (SDAx) and x.xx (SCLx)
    i2c_pin_select(i2c_interface);

    // clear I2CCON register flags
    i2c_clear_control_register(i2c_interface);

    //set baud rate
    i2c_set_baud_rate(i2c_interface, baud_rate);

    //set slave mode
    if (i2c_mode == I2CSLAVE) {
        i2c_set_slave_mode(i2c_interface, slave_addr);
    }

    /* Install interrupt handler */
    if (!i2c_irq_handler_register(i2c_interface, handler)) {
        return false;
    }

    i2c_initial_master_transmitter_mode(i2c_interface);

    //puts("...i2c_initialize ended\n");
    return (true);
}

bool i2c_transaction(uint8_t i2c_interface)
{
    //puts("i2cTransaction begin...\n");
    i2c_master_state = I2C_IDLE;
    rd_index = 0;
    wr_index = 0;

    if (i2c_start(i2c_interface) != true) {
        i2c_stop(i2c_interface);
        puts("i2cTransaction return false...\n");
        return (false);
    }

    // puts("entering engine main loop\n");
    while (1) {
        if (i2c_master_state == DATA_NACK) {
            i2c_stop(i2c_interface);
            break;
        }
    }

    // puts("...i2cTransaction ended\n");
    return (true);
}

bool i2c_start(uint8_t i2c_interface)
{
    //  puts("i2c_start begin...\n");
    uint32_t timeout = 0;
    bool retVal = false;

    /*--- Issue a start condition ---*/
    switch (i2c_interface) {
        case I2C0:
            I20CONSET = I2CONSET_STA; /* Set Start flag */
            break;

        case I2C1_0:
        case I2C1_1:
            I21CONSET = I2CONSET_STA; /* Set Start flag */
            break;

        case I2C2:
            I22CONSET = I2CONSET_STA; /* Set Start flag */
    }

    /*--- Wait until START transmitted ---*/
    while (1) {
        if (i2c_master_state == I2C_STARTED) {
            retVal = true;
            break;
        }

        if (timeout >= MAX_TIMEOUT) {
            puts("timeout");
            retVal = false;
            break;
        }

        timeout++;
    }

    //puts("...i2c_start ended\n");
    return (retVal);
}

bool i2c_stop(uint8_t i2c_interface)
{
    //puts("i2c_stop begin...\n");
    switch (i2c_interface) {
        case I2C0:
            I20CONSET = I2CONSET_STO; /* Set Stop flag */
            I20CONCLR = I2CONCLR_SIC; /* Clear SI flag */

            /*--- Wait for STOP detected ---*/
            while (I20CONSET & I2CONSET_STO)
                ;

            break;

        case I2C1_0:
        case I2C1_1:
            I21CONSET = I2CONSET_STO; /* Set Stop flag */
            I21CONCLR = I2CONCLR_SIC; /* Clear SI flag */

            /*--- Wait for STOP detected ---*/
            while (I21CONSET & I2CONSET_STO)
                ;

            break;

        case I2C2:
            I22CONSET = I2CONSET_STO; /* Set Stop flag */
            I22CONCLR = I2CONCLR_SIC; /* Clear SI flag */

            /*--- Wait for STOP detected ---*/
            while (I22CONSET & I2CONSET_STO)
                ;
    }

    //  puts("...i2c_stop ended\n");
    return true;
}

void i2c_active_power(uint8_t i2c_interface)
{
    switch (i2c_interface) {
        case I2C0:
            PCONP |= BIT7;
            break;

        case I2C1_0:
        case I2C1_1:
            PCONP |= BIT19;
            break;

        case I2C2:
            PCONP |= BIT26;
    }
}

//select I2C2 functionality for pins x.xx (SDAx) and x.xx (SCLx)
void i2c_pin_select(uint8_t i2cInterface)
{
    switch (i2cInterface) {
        case I2C0: // P0.27 SDA0, P0.28 SCL0
            PINSEL1 |= BIT22 | BIT24;
            PINSEL1 &= ~(BIT23 | BIT25);
            break;

        case I2C1_0: // P0.0  SDA1, P0.1  SCL1
            PINSEL0 |= BIT0 | BIT1 | BIT2 | BIT3;
            break;

        case I2C1_1: // P0.19 SDA1, P0.20 SCL1
            PINSEL1 |= BIT6 | BIT7 | BIT8 | BIT9;
            break;

        case I2C2: // P0.10 SDA2, P0.11 SCL2
            PINSEL0 |= BIT21 | BIT23;
            PINSEL0 &= ~(BIT20 | BIT22);
    }
}

/*--- clearing of bits in the I2CON register ---*/
void i2c_clear_control_register(uint8_t i2c_interface)
{
    switch (i2c_interface) {
        case I2C0:
            I20CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC |
                        I2CONCLR_I2ENC;
            break;

        case I2C1_0:
        case I2C1_1:
            I21CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC |
                        I2CONCLR_I2ENC;
            break;

        case I2C2:
            I22CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC |
                        I2CONCLR_I2ENC;
    }
}

void i2c_set_baud_rate(uint8_t i2c_interface, uint32_t baud_rate)
{
    uint32_t pclksel = 0;
    uint32_t prescale = 0;
    lpc2387_pclk_scale(F_CPU, baud_rate, &pclksel, &prescale);

    switch (i2c_interface) {
        case I2C0:
            PCLKSEL0 &= ~(BIT14 | BIT15); //clear Bits
            PCLKSEL0 |= pclksel << 14; //set bits

            I20SCLL = prescale / 2;
            I20SCLH = prescale / 2;
            break;

        case I2C1_0:
        case I2C1_1:
            PCLKSEL1 &= ~(BIT6 | BIT7);
            PCLKSEL1 |= pclksel << 6;

            I21SCLL = prescale / 2;
            I21SCLH = prescale / 2;
            break;

        case I2C2:
            PCLKSEL1 &= ~(BIT20 | BIT21);
            PCLKSEL1 |= pclksel << 20;

            I22SCLL = prescale / 2;
            I22SCLH = prescale / 2;
    }
}

bool i2c_irq_handler_register(uint8_t i2c_interface, void *handler)
{
    bool successful = false;

    switch (i2c_interface) {
        case I2C0:
            if (handler == NULL) {
                successful = install_irq(I2C0_INT,
                                         (void *) i2c_interface0_master_handler,
                                         HIGHEST_PRIORITY);
            }
            else {
                successful = install_irq(I2C0_INT, (void *) handler,
                                         HIGHEST_PRIORITY);
            }

            break;

        case I2C1_0:
        case I2C1_1:
            if (handler == NULL) {
                successful = install_irq(I2C1_INT,
                                         (void *) i2c_interface1_master_handler,
                                         HIGHEST_PRIORITY);
            }
            else {
                successful = install_irq(I2C1_INT, (void *) handler,
                                         HIGHEST_PRIORITY);
            }

            break;

        case I2C2:
            if (handler == NULL) {
                successful = install_irq(I2C2_INT,
                                         (void *) i2c_interface2_master_handler,
                                         HIGHEST_PRIORITY);
            }
            else {
                successful = install_irq(I2C2_INT, (void *) handler,
                                         HIGHEST_PRIORITY);
            }
    }

    return successful;
}

void i2c_initial_master_transmitter_mode(uint8_t i2c_interface)
{
    switch (i2c_interface) {
        case I2C0:
            I20CONSET = I2CONSET_I2EN;
            break;

        case I2C1_0:
        case I2C1_1:
            I21CONSET = I2CONSET_I2EN;
            break;

        case I2C2:
            I22CONSET = I2CONSET_I2EN;
    }
}

bool i2c_read(uint8_t i2c_interface, uint8_t slave_addr, uint8_t reg_addr,
              uint8_t *rx_buff, uint8_t rx_buff_length)
{
    i2c_clear_buffer((uint8_t *) i2c_master_buffer,
                     I2C_BUFSIZE * sizeof(uint8_t));
    i2c_write_length = 1;
    i2c_read_length = rx_buff_length;
    bool successful = false;
    uint8_t readIndex = 3;
    i2c_master_buffer[0] = (slave_addr << 1) & WRITE_ENABLE_BIT_MASK;
    i2c_master_buffer[1] = reg_addr;
    i2c_master_buffer[2] = ((slave_addr << 1) & WRITE_ENABLE_BIT_MASK)
                           | READ_ENABLE_BIT_MASK;
    successful = i2c_transaction(i2c_interface);

    if (successful && (rx_buff != NULL) &&
        (rx_buff_length < (I2C_BUFSIZE - readIndex))) {
        memcpy(rx_buff, (const uint8_t *)(i2c_master_buffer + readIndex),
               sizeof(uint8_t) * rx_buff_length);
        return true;
    }
    else {
        return false;
    }
}

void i2c_clear_buffer(void *ptr, uint32_t size)
{
    memset(ptr, 0, size);
}

bool i2c_write(uint8_t i2c_interface, uint8_t slave_addr, uint8_t reg_addr,
               uint8_t *tx_buff, uint8_t tx_buff_length)
{
    //puts("[i2c.c/i2cWrite]: entered\n");
    i2c_clear_buffer((uint8_t *) i2c_master_buffer,
                     I2C_BUFSIZE * sizeof(uint8_t));
    i2c_write_length = tx_buff_length + 1;
    i2c_master_buffer[0] = (slave_addr << 1) & WRITE_ENABLE_BIT_MASK;
    i2c_master_buffer[1] = reg_addr;
    int32_t i;
    int32_t j = 0;

    if ((tx_buff != NULL) && tx_buff_length < (I2C_BUFSIZE - 2)) {
        for (i = 2; i < tx_buff_length + 2; i++) {
            i2c_master_buffer[i] = tx_buff[j];
            j++;
            //printf("I2CMasterBuffer[%d] = %d\n", i, I2CMasterBuffer[i]);
        }

        return i2c_transaction(i2c_interface);
    }
    else {
        puts("[i2c.c/i2cWrite]: Invalid buffer or invalid write buffer size\n");
        return false;
    }
}

//burst mode, the first element in the array
bool i2c_trans_receive(uint8_t i2c_interface, uint8_t slave_addr,
                       uint8_t *tx_buff, uint8_t tx_buff_length,
                       uint8_t *rx_buff, uint8_t rx_buff_length)
{
    puts("[i2c.c/i2cTransReceive]: entered\n");
    i2c_clear_buffer((uint8_t *) i2c_master_buffer,
                     I2C_BUFSIZE * sizeof(uint8_t));
    i2c_write_length = tx_buff_length;
    i2c_read_length = rx_buff_length;
    bool successful = false;
    int32_t read_index = 0;
    int32_t i = 0;

    if (tx_buff != NULL && (tx_buff_length > 0)) {
        i2c_master_buffer[0] = (slave_addr << 1) & WRITE_ENABLE_BIT_MASK;

        for (i = 1; i < tx_buff_length + 1; i++) {
            if (i < I2C_BUFSIZE) {
                i2c_master_buffer[i] = tx_buff[i - 1];
            }
        }

        //enable I2C to read
        if ((rx_buff_length > 0) && (i < I2C_BUFSIZE)) {
            i2c_master_buffer[i] = ((slave_addr << 1) & WRITE_ENABLE_BIT_MASK)
                                   | READ_ENABLE_BIT_MASK;
            read_index = i + 1;
        }

        successful = i2c_transaction(i2c_interface);

        if (successful && (rx_buff != NULL) && (rx_buff_length > 0)) {
            memcpy(rx_buff, (const uint8_t *)(i2c_master_buffer + read_index),
                   sizeof(uint8_t) * rx_buff_length);
            return true;
        }
        else {
            return false;
        }
    }
    else {
        puts(
            "[i2c.c/i2cRead]: the txBuff is not valid or has not a valid \
            length value !\n");
        return false;
    }
}

/**
 * @brief       The interrupt handler for the I2C0 interface.
 *              It deals only with the master mode.
 *
 */
void i2c_interface0_master_handler(void) //__irq
{
    //puts("entering I2C handler function\n");
    uint8_t state_value;

    /* this handler deals with master read and master write only */
    state_value = I20STAT;

    //IENABLE;          /* handles nested interrupt */
    //enableIRQ();
    switch (state_value) {
        case 0x08: /* A Start condition is issued. */
            //puts("A Start condition is issued\n");
            I20DAT = i2c_master_buffer[0];
            //printf("I22DAT = %lu\n", I22DAT);
            I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
            i2c_master_state = I2C_STARTED;
            break;

        case 0x10: /* A repeated started is issued */
            //puts("A repeated Start is issued\n");
            //  if ( I2CCmd == L3DG420_WHO_AM_I)
            //  {
            //    I22DAT = I2CMasterBuffer[2];
            //  }
            I20DAT = i2c_master_buffer[2];
            I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
            i2c_master_state = I2C_RESTARTED;
            break;

        case 0x18: /* Regardless, it's a ACK */

            //puts("got an Ack\n");
            if (i2c_master_state == I2C_STARTED) {
                I20DAT = i2c_master_buffer[1 + wr_index];
                wr_index++;
                i2c_master_state = DATA_ACK;

            }

            I20CONCLR = I2CONCLR_SIC;
            break;

        case 0x28: /* Data byte has been transmitted, regardless ACK or NACK */
        case 0x30:

            //puts("Data byte has been transmitted\n");
            if (wr_index != i2c_write_length) {

                // this should be the last one
                I20DAT = i2c_master_buffer[1 + wr_index];

                if (wr_index != i2c_write_length) {
                    i2c_master_state = DATA_ACK;
                }
                else {
                    i2c_master_state = DATA_NACK;
                    I20CONSET = I2CONSET_STO; /* Set Stop flag */

                    if (i2c_read_length != 0) {
                        I20CONSET = I2CONSET_STA; /* Set Repeated-start flag */
                        i2c_master_state = I2C_REPEATED_START;
                    }
                }

                wr_index++;
            }
            else {
                if (i2c_read_length != 0) {
                    I20CONSET = I2CONSET_STA; /* Set Repeated-start flag */
                    i2c_master_state = I2C_REPEATED_START;
                }
                else {
                    i2c_master_state = DATA_NACK;
                    I20CONSET = I2CONSET_STO; /* Set Stop flag */
                }
            }

            I20CONCLR = I2CONCLR_SIC;
            break;

        case 0x40: /* Master Receive, SLA_R has been sent */

            //puts("Master Receive, SLA_R has been sent!\n");
            if (i2c_read_length >= 2) {
                I20CONSET = I2CONSET_AA; /* assert ACK after data is received */
            }

            I20CONCLR = I2CONCLR_SIC;
            break;

            // Data byte has been received, regardless following ACK or NACK
        case 0x50:
        case 0x58:
            //puts("Data received\n");
            i2c_master_buffer[3 + rd_index] = I20DAT;
            rd_index++;

            if (rd_index < (i2c_read_length - 1)) {
                i2c_master_state = DATA_ACK;
                I20CONSET = I2CONSET_AA; /* assert ACK after data is received */
            }
            else {
                I20CONCLR = I2CONCLR_AAC; /* NACK after data is received */
            }

            if (rd_index == i2c_read_length) {
                rd_index = 0;
                i2c_master_state = DATA_NACK;
            }

            I20CONCLR = I2CONCLR_SIC;
            break;

        case 0x20: /* regardless, it's a NACK */
        case 0x48:
            I20CONCLR = I2CONCLR_SIC;
            i2c_master_state = DATA_NACK;
            break;

        case 0x38: /*
                    * Arbitration lost, in this example, we don't
                    *  deal with multiple master situation
                    **/

            //puts("Arbritration lost!\n");
        default:
            I20CONCLR = I2CONCLR_SIC;
            break;
    }

    //IDISABLE;
    //disableIRQ();
    //puts("leave I2C handler function\n");
    VICVectAddr = 0; /* Acknowledge Interrupt */
}

/**
 * @brief       The interrupt handler for the I2C1 interface.
 *              It deals only with the master mode.
 *
 */
void i2c_interface1_master_handler(void) //__irq
{
    //puts("entering I2C handler function\n");
    uint8_t state_value;

    /* this handler deals with master read and master write only */
    state_value = I21STAT;

    //IENABLE;    /* handles nested interrupt */
    //enableIRQ();
    switch (state_value) {
        case 0x08: /* A Start condition is issued. */
            //puts("A Start condition is issued\n");
            I21DAT = i2c_master_buffer[0];
            //printf("I22DAT = %lu\n", I22DAT);
            I21CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
            i2c_master_state = I2C_STARTED;
            break;

        case 0x10: /* A repeated started is issued */
            //puts("A repeated Start is issued\n");
            //  if ( I2CCmd == L3DG420_WHO_AM_I)
            //  {
            //    I22DAT = I2CMasterBuffer[2];
            //  }
            I21DAT = i2c_master_buffer[2];
            I21CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
            i2c_master_state = I2C_RESTARTED;
            break;

        case 0x18: /* Regardless, it's a ACK */

            //puts("got an Ack\n");
            if (i2c_master_state == I2C_STARTED) {
                I21DAT = i2c_master_buffer[1 + wr_index];
                wr_index++;
                i2c_master_state = DATA_ACK;

            }

            I21CONCLR = I2CONCLR_SIC;
            break;

        case 0x28: /* Data byte has been transmitted, regardless ACK or NACK */
        case 0x30:

            //puts("Data byte has been transmitted\n");
            if (wr_index != i2c_write_length) {
                // this should be the last one
                I21DAT = i2c_master_buffer[1 + wr_index];

                if (wr_index != i2c_write_length) {
                    i2c_master_state = DATA_ACK;
                }
                else {
                    i2c_master_state = DATA_NACK;
                    I21CONSET = I2CONSET_STO; /* Set Stop flag */

                    if (i2c_read_length != 0) {
                        I21CONSET = I2CONSET_STA; /* Set Repeated-start flag */
                        i2c_master_state = I2C_REPEATED_START;
                    }
                }

                wr_index++;
            }
            else {
                if (i2c_read_length != 0) {
                    I21CONSET = I2CONSET_STA; /* Set Repeated-start flag */
                    i2c_master_state = I2C_REPEATED_START;
                }
                else {
                    i2c_master_state = DATA_NACK;
                    I21CONSET = I2CONSET_STO; /* Set Stop flag */
                }
            }

            I21CONCLR = I2CONCLR_SIC;
            break;

        case 0x40: /* Master Receive, SLA_R has been sent */

            //puts("Master Receive, SLA_R has been sent!\n");
            if (i2c_read_length >= 2) {
                I21CONSET = I2CONSET_AA; /* assert ACK after data is received */
            }

            I21CONCLR = I2CONCLR_SIC;
            break;

        case 0x50: /*
                    * Data byte has been received, regardless following ACK or
                    * NACK
                    **/
        case 0x58:
            //puts("Data received\n");
            i2c_master_buffer[3 + rd_index] = I21DAT;
            rd_index++;

            if (rd_index < (i2c_read_length - 1)) {
                i2c_master_state = DATA_ACK;
                I21CONSET = I2CONSET_AA; /* assert ACK after data is received */
            }
            else {
                I21CONCLR = I2CONCLR_AAC; /* NACK after data is received */
            }

            if (rd_index == i2c_read_length) {
                rd_index = 0;
                i2c_master_state = DATA_NACK;
            }

            I21CONCLR = I2CONCLR_SIC;
            break;

        case 0x20: /* regardless, it's a NACK */
        case 0x48:
            I21CONCLR = I2CONCLR_SIC;
            i2c_master_state = DATA_NACK;
            break;

        case 0x38: /*
                    * Arbitration lost, in this example, we don't
                    * deal with multiple master situation
                    **/

            //puts("Arbritration lost!\n");
        default:
            I21CONCLR = I2CONCLR_SIC;
            break;
    }

    //IDISABLE;
    //disableIRQ();
    //puts("leave I2C handler function\n");
    VICVectAddr = 0; /* Acknowledge Interrupt */
}

/**
 * @brief    The interrupt handler for the I2C2 interface.
 *           It deals only with the master mode.
 *
 */
void i2c_interface2_master_handler(void) //__irq
{
    //puts("entering I2C handler function\n");
    uint8_t state_value;

    /* this handler deals with master read and master write only */
    state_value = I22STAT;

    //IENABLE;  /* handles nested interrupt */
    //enableIRQ();
    switch (state_value) {
        case 0x08: /* A Start condition is issued. */
            //puts("A Start condition is issued\n");
            I22DAT = i2c_master_buffer[0];
            //printf("I22DAT = %lu\n", I22DAT);
            I22CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
            i2c_master_state = I2C_STARTED;
            break;

        case 0x10: /* A repeated started is issued */
            //puts("A repeated Start is issued\n");
            //  if ( I2CCmd == L3DG420_WHO_AM_I)
            //  {
            //    I22DAT = I2CMasterBuffer[2];
            //  }
            I22DAT = i2c_master_buffer[2];
            I22CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
            i2c_master_state = I2C_RESTARTED;
            break;

        case 0x18: /* Regardless, it's a ACK */

            //puts("got an Ack\n");
            if (i2c_master_state == I2C_STARTED) {
                I22DAT = i2c_master_buffer[1 + wr_index];
                wr_index++;
                i2c_master_state = DATA_ACK;

            }

            I22CONCLR = I2CONCLR_SIC;
            break;

        case 0x28: /* Data byte has been transmitted, regardless ACK or NACK */
        case 0x30:

            //puts("Data byte has been transmitted\n");
            if (wr_index != i2c_write_length) {
                // this should be the last one
                I22DAT = i2c_master_buffer[1 + wr_index];

                if (wr_index != i2c_write_length) {
                    i2c_master_state = DATA_ACK;
                }
                else {
                    i2c_master_state = DATA_NACK;
                    I22CONSET = I2CONSET_STO; /* Set Stop flag */

                    if (i2c_read_length != 0) {
                        I22CONSET = I2CONSET_STA; /* Set Repeated-start flag */
                        i2c_master_state = I2C_REPEATED_START;
                    }
                }

                wr_index++;
            }
            else {
                if (i2c_read_length != 0) {
                    I22CONSET = I2CONSET_STA; /* Set Repeated-start flag */
                    i2c_master_state = I2C_REPEATED_START;
                }
                else {
                    i2c_master_state = DATA_NACK;
                    I22CONSET = I2CONSET_STO; /* Set Stop flag */
                }
            }

            I22CONCLR = I2CONCLR_SIC;
            break;

        case 0x40: /* Master Receive, SLA_R has been sent */

            //puts("Master Receive, SLA_R has been sent!\n");
            if (i2c_read_length >= 2) {
                I22CONSET = I2CONSET_AA; /* assert ACK after data is received */
            }

            I22CONCLR = I2CONCLR_SIC;
            break;

        case 0x50: /*
                    * Data byte has been received, regardless following ACK or
                    * NACK
                    **/
        case 0x58:
            //puts("Data received\n");
            i2c_master_buffer[3 + rd_index] = I22DAT;
            rd_index++;

            if (rd_index < (i2c_read_length - 1)) {
                i2c_master_state = DATA_ACK;
                I22CONSET = I2CONSET_AA; /* assert ACK after data is received */
            }
            else {
                I22CONCLR = I2CONCLR_AAC; /* NACK after data is received */
            }

            if (rd_index == i2c_read_length) {
                rd_index = 0;
                i2c_master_state = DATA_NACK;
            }

            I22CONCLR = I2CONCLR_SIC;
            break;

        case 0x20: /* regardless, it's a NACK */
        case 0x48:
            I22CONCLR = I2CONCLR_SIC;
            i2c_master_state = DATA_NACK;
            break;

        case 0x38: /*
                    * Arbitration lost, in this example, we don't
                    * deal with multiple master situation
                    **/

            //puts("Arbritration lost!\n");
        default:
            I22CONCLR = I2CONCLR_SIC;
            break;
    }

    //IDISABLE;
    //disableIRQ();
    //puts("leave I2C handler function\n");
    VICVectAddr = 0; /* Acknowledge Interrupt */
}

void i2c_set_slave_mode(uint8_t i2c_interface, uint8_t slave_addr)
{
    switch (i2c_interface) {
        case I2C0:
            I20ADR = slave_addr;
            break;

        case I2C1_0:
        case I2C1_1:
            I21ADR = slave_addr;
            break;

        case I2C2:
            I22ADR = slave_addr;
    }
}

void i2c_enable_pull_up_resistor(uint8_t i2c_interface)
{

    switch (i2c_interface) {

        case I2C1_0: // P0.0  SDA1, P0.1  SCL1
            puts("The on-chip pull-up resistor is enabled for the I2C1_0");
            PINMODE0 &= ~(BIT0 | BIT1 | BIT2 | BIT3);
            break;

        case I2C1_1: // P0.19 SDA1, P0.20 SCL1
            puts("The on-chip pull-up resistor is enabled for the I2C1_1");
            PINMODE1 &= ~(BIT6 | BIT7 | BIT8 | BIT9);
            break;

        case I2C2: // P0.10 SDA2, P0.11 SCL2
            puts("The on-chip pull-up resistor is enabled for the I2C2");
            PINMODE0 &= ~(BIT20 | BIT21 | BIT22 | BIT23);
            //PINMODE0 &= ~(BIT20 | BIT22);
    }
}

void i2c_disable_pull_up_resistor(uint8_t i2c_interface)
{
    switch (i2c_interface) {
        case I2C1_0: // P0.0  SDA1, P0.1  SCL1
            puts("The on-chip pull-up resistor is disbled for the I2C1_0");
            PINMODE0 &= ~(BIT0 | BIT2);
            PINMODE0 |= (BIT1 | BIT3);
            break;

        case I2C1_1: // P0.19 SDA1, P0.20 SCL1
            puts("The on-chip pull-up resistor is disabled for the I2C1_1");
            PINMODE1 &= ~(BIT6 | BIT8);
            PINMODE1 |= (BIT7 | BIT9);
            break;

        case I2C2: // P0.10 SDA2, P0.11 SCL2
            puts("The on-chip pull-up resistor is disbled for the I2C2");
            PINMODE0 &= ~(BIT20 | BIT22);
            PINMODE1 |= (BIT21 | BIT23);

    }
}
