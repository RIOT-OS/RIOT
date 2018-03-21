/*
 * Copyright (C) 2018 Baptiste Cartier
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */




/**
 * @ingroup     cc430
 * @{
 *
 * @file        RF1A.c
 * @brief       Definitions for radio function
 *
 * API for the cc1101 radio module embedded on the cc430
 *
 * @author      Baptiste Cartier
 *
 */

#include <msp430.h> // generic file for register name, I/O, ...

#include "cc430_radio.h"

#include "thread.h"




/**
 * Global variable for wakeup in interrupt
 */
kernel_pid_t cc430_radio_pid;

/**
 * @brief   Managing the CC1101 interrupt vector, here only used if radio module in receive mode
 */
#pragma vector=CC1101_VECTOR
__attribute__((interrupt(CC1101_VECTOR)))
void CC1101_ISR(void)
{
    if (RF1AIV == RF1AIV_RFIFG4) {
        thread_wakeup(cc430_radio_pid);
    }
}

/**
 * @brief   Simple delay.
 *
 * @param[in]  uint32_t p      Number of loop of the delay
 *
 */
void cc430_radio_delay_rf(volatile uint32_t p)
{
    while (p) {
        p--;
    }
}


/**
 * @brief   Send a command to the radio module
 *
 * @param[in]   uint8_t strobe      Type of command to be sent
 *
 * @return      uint8_t statusByte  value of the RF1ASTATB register, or 0 if invalid strobe command
 */
uint8_t cc430_radio_strobe(uint8_t strobe)
{
    uint8_t statusByte = 0;
    uint16_t gdo_state;

    // Check for valid strobe command
    if ((strobe == 0xBD) || ((strobe >= RF_SRES) && (strobe <= RF_SNOP))) {
        // Clear the Status read flag
        RF1AIFCTL1 &= ~(RFSTATIFG);

        // Wait for radio to be ready for next instruction
        while (!(RF1AIFCTL1 & RFINSTRIFG)) {
        }

        // Write the strobe instruction
        if ((strobe > RF_SRES) && (strobe < RF_SNOP)) {
            gdo_state = cc430_radio_read_single_reg(IOCFG2);    // buffer IOCFG2 state
            cc430_radio_write_single_reg(IOCFG2, 0x29);         // chip-ready to GDO2

            RF1AINSTRB = strobe;
            if ((RF1AIN & 0x04) == 0x04) {  // chip at sleep mode
                if ((strobe == RF_SXOFF) || (strobe == RF_SPWD) || (strobe == RF_SWOR)) {
                }
                else {
                    while ((RF1AIN & 0x04) == 0x04) {
                    }                         // chip-ready ?
                    // Delay for ~810usec at 1.05MHz CPU clock, see erratum RF1A7
                    __delay_cycles(850);
                }
            }
            cc430_radio_write_single_reg(IOCFG2, gdo_state); // restore IOCFG2 setting

            while (!(RF1AIFCTL1 & RFSTATIFG));
        }
        else {                      // chip active mode (SRES)
            RF1AINSTRB = strobe;
        }
        statusByte = RF1ASTATB;
    }
    return statusByte;
}


/**
 * @brief   Read the value of single register
 *
 * @param[in]   uint8_t addr        Address of the register to be read
 *
 * @return      uint8_t data_out    value of the register
 */
uint8_t cc430_radio_read_single_reg(uint8_t addr)
{
    uint8_t data_out;

    // Check for valid configuration register address, 0x3E refers to PATABLE
    if ((addr <= TEST0) || (addr == PATABLE)) {
        // Send address + Instruction + 1 dummy byte (auto-read)
        RF1AINSTR1B = (addr | RF_SNGLREGRD);
    }
    else {
        // Send address + Instruction + 1 dummy byte (auto-read)
        RF1AINSTR1B = (addr | RF_STATREGRD);
    }

    while (!(RF1AIFCTL1 & RFDOUTIFG)) {
    }
    data_out = RF1ADOUT1B;                    // Read data and clears the RFDOUTIFG

    return data_out;
}


/**
 * @brief   Write to a single cc1101 register
 *
 * @param[in]  uint8_t addr       Address of the register
 * @param[in]  uint8_t value      Value to be written
 *
 */
void cc430_radio_write_single_reg(uint8_t addr, uint8_t value)
{
    while (!(RF1AIFCTL1 & RFINSTRIFG));     // Wait for the Radio to be ready for next instruction
    RF1AINSTRB = (addr | RF_REGWR);         // Send address + Instruction
    RF1ADINB = value;                       // Write data in

    __no_operation();
}



/**
 * @brief   Read sequentially in multiple registers of the cc1101 from a base address
 *
 * @param[in]  uint8_t addr     Address of the first register
 * @param[out] void * buffer    Values of the registers are written here, buffer must be of size uint8_t
 * @param[in]  uint8_t count    Number of register to be read
 *
 */
void cc430_radio_read_burst_reg(uint8_t addr, void *buffer, uint8_t count)
{
    uint8_t i;

    while (!(RF1AIFCTL1 & RFINSTRIFG)) {
    }                                           // Wait for INSTRIFG
    RF1AINSTR1B = (addr | RF_REGRD);            // Send addr of first conf. reg. to be read
                                                // ... and the burst-register read instruction
    for (i = 0; i < (count - 1); i++) {
        while (!(RFDOUTIFG & RF1AIFCTL1)) {
        }                                           // Wait for the Radio Core to update the RF1ADOUTB reg
        ((uint8_t *)buffer)[i] = RF1ADOUT1B;        // Read DOUT from Radio Core + clears RFDOUTIFG
        // Also initiates auo-read for next DOUT byte
    }
    ((uint8_t *)buffer)[count - 1] = RF1ADOUT0B;             // Store the last DOUT from Radio Core
}


/**
 * @brief   write sequentially in multiple registers of the cc1101 from a base address
 *
 * @param[in]  uint8_t addr       Address of the first register
 * @param[in]  void * buffer      The new states of the registers, must be of size uint8_t
 * @param[in]  uint8_t count      Number of register to be written
 *
 */
void cc430_radio_write_burst_reg(uint8_t addr, void *buffer, uint8_t count)
{
    uint8_t i;

    while (!(RF1AIFCTL1 & RFINSTRIFG));                             // Wait for the Radio to be ready for next instruction
    RF1AINSTRW = ((addr | RF_REGWR) << 8) + ((uint8_t *)buffer)[0]; // Send address + Instruction

    for (i = 1; i < count; i++) {
        RF1ADINB = ((uint8_t *)buffer)[i];      // Send data
        while (!(RFDINIFG & RF1AIFCTL1)) {
        }                                       // Wait for TX to finish
    }
    i = RF1ADOUTB;                              // Reset RFDOUTIFG flag which contains status byte
}


/**
 * @brief   Write in multiple registers of the cc1101 in any order
 *
 * @param[in]  uint8_t rf_setting rf_setting[i][0] address of the register, rf_setting[i][1] value to be written
 * @param[in]  uint8_t size       number of register to be modified
 *
 */
void cc430_radio_write_rf_reg(const uint8_t rf_setting[][2], uint8_t size)
{
    uint8_t i;

    for (i = 0; i < (size); i++) {
        cc430_radio_write_single_reg(rf_setting[i][0], rf_setting[i][1]);
    }
}


/**
 * @brief   Reset the radio core with a SRES command
 */
void cc430_radio_reset_radio_core(void)
{
    cc430_radio_strobe(RF_SRES);                            // Reset the Radio Core
    cc430_radio_strobe(RF_SNOP);                            // Reset Radio Pointer
}



/**
 * @brief   Set the power output of the radio module
 *
 * @param[in]  uint8_t value      output needed, defined in rf1a.h
 *
 * @return 1 if setting the state was successful, 0 otherwise.
 */
int8_t cc430_radio_write_pa_table(uint8_t value)
{
    uint8_t valueRead = 0;
    uint16_t rf1ainstrw_value;

    //int8_t return_code = 1;

    switch (value) {
        case cc430_radio_POWER_OUTPUT_MINUS_30DBM:
            rf1ainstrw_value = RADIOINSTRW_MINUS_30DBM;
            break;

        case cc430_radio_POWER_OUTPUT_MINUS_12DBM:
            rf1ainstrw_value = RADIOINSTRW_MINUS_12DBM;
            break;

        case cc430_radio_POWER_OUTPUT_MINUS_6DBM:
            rf1ainstrw_value = RADIOINSTRW_MINUS_6DBM;
            break;

        case cc430_radio_POWER_OUTPUT_0DBM:
            rf1ainstrw_value = RADIOINSTRW_0DBM;
            break;

        case cc430_radio_POWER_OUTPUT_10DBM:
            rf1ainstrw_value = RADIOINSTRW_10DBM;
            break;

        case cc430_radio_POWER_OUTPUT_MAX:
            rf1ainstrw_value = RADIOINSTRW_MAX;
            break;

        case cc430_radio_POWER_OUTPUT_DEFAULT_8_8DBM:
            rf1ainstrw_value = RADIOINSTRW_DEFAULT_8_8DBM;
            break;

        default:
            return 0;
            break;
    }


    while (valueRead != value) {
        /* Write the power output to the PA_TABLE and verify the write operation.  */
        uint8_t i = 0;

        /* wait for radio to be ready for next instruction */
        while (!(RF1AIFCTL1 & RFINSTRIFG)) {
        }

        RF1AINSTRW = rf1ainstrw_value;

        /* wait for radio to be ready for next instruction */
        while (!(RF1AIFCTL1 & RFINSTRIFG)) {
        }
        RF1AINSTR1B = RF_PATABRD;

        // Traverse PATABLE pointers to read
        for (i = 0; i < 7; i++) {
            while (!(RF1AIFCTL1 & RFDOUTIFG)) {
            }
            valueRead = RF1ADOUT1B;
        }
        while (!(RF1AIFCTL1 & RFDOUTIFG)) {
        }
        valueRead = RF1ADOUTB;
    }

    return 1;
}



/**
 * @brief   Transmit data with the radio module. User must make sure they do not overflow the TXBuffer
 * of the radio module
 *
 * @param[in]  void * buffer      buffer of bytes to be sent
 * @param[in]  uint8_t length     number of bytes to be sent
 *
 */
void cc430_radio_transmit(void *buffer, uint8_t length)
{
    RF1AIES |= BIT9;
    RF1AIFG &= ~BIT9;                         // Clear pending interrupts

    cc430_radio_write_burst_reg(RF_TXFIFOWR, buffer, length);

    cc430_radio_strobe( RF_STX );                         // cc430_radio_strobe STX
}


/**
 * @brief   Receive data with the radio module, will put the callling thread to sleep until data as been received
 * Size of data received cannot exceed the size of RXBuffer of the radio module. If paquets bigger than RXBuffer must be received,
 * cc430_radio_receive must be called the right amount of times
 *
 * @param[out]  void * buffer      buffer of bytes to be written to
 * @param[out]  uint8_t length     number of bytes to be received
 *
 */

void cc430_radio_receive(void *buffer, uint8_t *length)
{
    cc430_radio_pid = thread_getpid();
    thread_sleep();
    *length = cc430_radio_read_single_reg(RXBYTES);
    cc430_radio_read_burst_reg(RF_RXFIFORD, buffer, *length);
}

/**
 * @brief   Enable the reception mode
 *
 */
void cc430_radio_receive_on(void)
{
    RF1AIFG &= ~BIT4;                           // Clear a pending interrupt
    RF1AIE |= BIT4;                             // Enable the interrupt

    // Previous state has been Tx
    cc430_radio_strobe( RF_SIDLE );
    cc430_radio_delay_rf(TX_TO_IDLE_TIME);
    cc430_radio_strobe( RF_SRX );
    cc430_radio_delay_rf(IDLE_TO_RX_TIME);
}



/**
 * @brief   Disable the reception mode
 *
 */
void cc430_radio_receive_off(void)
{
    RF1AIE &= ~BIT4;                            // Disable RX interrupts
    RF1AIFG &= ~BIT4;                           // Clear pending IFG

    // Previous state has been Rx
    cc430_radio_strobe( RF_SIDLE );
    cc430_radio_delay_rf(RX_TO_IDLE_TIME);
    cc430_radio_strobe( RF_SFRX);      /* Flush the receive FIFO of any residual data */

}
