/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cc430
 * @{
 */

/**
 * @file
 * @brief       eZ430 radio driver (cpu dependent part)
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

#include <stdint.h>
#include "irq.h"
#include "cc110x_ng.h"
#include "board.h"
#include "hwtimer.h"

/**************************************************************************************************
 * @fn          Strobe
 * @brief       Send command to radio.
 * @param       none
 * @return      none
 *************************************************************************************************/
uint8_t cc110x_strobe(uint8_t c)
{
    uint8_t statusByte = 0;

    /* Check for valid strobe command */
    if ((c == 0xBD) || ((c > RF_SRES) && (c < RF_SNOP))) {
        uint16_t int_state = disableIRQ();

        /* Clear the Status read flag */
        RF1AIFCTL1 &= ~(RFSTATIFG);

        /* Wait for radio to be ready for next instruction */
        while (!(RF1AIFCTL1 & RFINSTRIFG));

        /* Write the strobe instruction */
        if ((c > RF_SRES) && (c < RF_SNOP)) {

            uint16_t gdo_state = cc110x_read_reg(IOCFG2); /* buffer IOCFG2 state */
            cc110x_write_reg(IOCFG2, 0x29); /* c-ready to GDO2 */

            RF1AINSTRB = c;

            if ((RF1AIN & 0x04) == 0x04) {      /* chip at sleep mode */
                if ((c == RF_SXOFF) || (c == RF_SPWD) || (c == RF_SWOR)) { }
                else {
                    while ((RF1AIN & 0x04) == 0x04);                /* c-ready ? */

                    hwtimer_wait(RTIMER_TICKS(9800));   /* Delay for ~810usec at 12MHz CPU clock */
                }
            }

            cc110x_write_reg(IOCFG2, gdo_state); /* restore IOCFG2 setting */
        }
        else {  /* chip active mode */
            RF1AINSTRB = c;
        }

        statusByte = RF1ASTATB;

        while (!(RF1AIFCTL1 & RFSTATIFG));

        restoreIRQ(int_state);
    }

    return statusByte;
}


/***************************************************************************************************
 * @fn          cc110x_read_reg
 * @brief       Read byte from register.
 * @param       none
 * @return      none
 ***************************************************************************************************/
uint8_t cc110x_read_reg(uint8_t addr)
{
    unsigned char x;
    uint16_t int_state;

    int_state = disableIRQ();

    RF1AINSTR1B = (addr | RF_REGRD);
    x = RF1ADOUT1B;

    restoreIRQ(int_state);
    return x;
}


/**************************************************************************************************
 * @fn          cc110x_write_reg
 * @brief       Write byte to register.
 * @param       none
 * @return      none
 **************************************************************************************************/
void cc110x_write_reg(uint8_t addr, uint8_t value)
{
    volatile unsigned int i;
    uint16_t int_state;

    int_state = disableIRQ();

    while (!(RF1AIFCTL1 & RFINSTRIFG));      /* Wait for the Radio to be ready for the next instruction */

    RF1AINSTRW = ((addr | RF_REGWR) << 8) + value; /* Send address + Instruction */

    while (!(RFDINIFG & RF1AIFCTL1));

    i = RF1ADOUTB;                            /* Reset RFDOUTIFG flag which contains status byte */

    restoreIRQ(int_state);
}

uint8_t cc110x_read_status(uint8_t addr)
{
    unsigned char x;
    uint16_t int_state;

    int_state = disableIRQ();

    RF1AINSTR1B = (addr | RF_STATREGRD);
    x = RF1ADOUT1B;

    restoreIRQ(int_state);
    return x;
}

/****************************************************************************************************
 * @fn          cc110x_readburst_reg
 * @brief       Read sequence of bytes from register.
 * @param       none
 * @return      none
 ***************************************************************************************************/
void cc110x_readburst_reg(uint8_t addr, char *buffer, uint8_t count)
{
    unsigned int i;
    uint16_t int_state;

    int_state = disableIRQ();

    while (!(RF1AIFCTL1 & RFINSTRIFG));        /* Wait for the Radio to be ready for next instruction */

    RF1AINSTR1B = (addr | RF_REGRD);          /* Send address + Instruction */

    for (i = 0; i < (count - 1); i++) {
        while (!(RFDOUTIFG & RF1AIFCTL1));       /* Wait for the Radio Core to update the RF1ADOUTB reg */

        buffer[i] = RF1ADOUT1B;                 /* Read DOUT from Radio Core + clears RFDOUTIFG */
        /* Also initiates auo-read for next DOUT byte */
    }

    buffer[count - 1] = RF1ADOUT0B;           /* Store the last DOUT from Radio Core */

    restoreIRQ(int_state);
}

void cc110x_read_fifo(char *buffer, uint8_t count)
{
    unsigned int i;
    uint16_t int_state;

    int_state = disableIRQ();

    while (!(RF1AIFCTL1 & RFINSTRIFG));        /* Wait for the Radio to be ready for next instruction */

    RF1AINSTR1B = (RF_RXFIFORD);          /* Send address + Instruction */

    for (i = 0; i < (count - 1); i++) {
        while (!(RFDOUTIFG & RF1AIFCTL1));       /* Wait for the Radio Core to update the RF1ADOUTB reg */

        buffer[i] = RF1ADOUT1B;                 /* Read DOUT from Radio Core + clears RFDOUTIFG */
        /* Also initiates auo-read for next DOUT byte */
    }

    buffer[count - 1] = RF1ADOUT0B;           /* Store the last DOUT from Radio Core */

    restoreIRQ(int_state);
}
/***************************************************************************************************
 * @fn          cc110x_writeburst_reg
 * @brief       Write sequence of bytes to register.
 * @param       none
 * @return      none
 **************************************************************************************************/
uint8_t cc110x_writeburst_reg(uint8_t addr, char *buffer, uint8_t count)
{
    /* Write Burst works wordwise not bytewise - bug known already */
    unsigned char i;
    uint16_t int_state;

    int_state = disableIRQ();

    while (!(RF1AIFCTL1 & RFINSTRIFG));        /* Wait for the Radio to be ready for next instruction */

    RF1AINSTRW = ((addr | RF_REGWR) << 8) + buffer[0]; /* Send address + Instruction */

    for (i = 1; i < count; i++) {
        RF1ADINB = buffer[i];                   /* Send data */

        while (!(RFDINIFG & RF1AIFCTL1));        /* Wait for TX to finish */
    }

    i = RF1ADOUTB;                            /* Reset RFDOUTIFG flag which contains status byte */

    restoreIRQ(int_state);
    return count;
}
