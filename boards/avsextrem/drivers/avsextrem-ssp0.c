/*
 * avsextrem-ssp0.c - implementation of the SPI0 interface for the LPC2387,
 * and the AVSESTREM board.
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @file
 * @internal
 * @brief       Implements the SPI0 interface for the LPC2387
 *
 * @author      Marco Ziegert <ziegert@inf.fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note        $Id:  avsextrem-ssp0.c  3854 2013-08-14 15:27:01Z zkasmi $
 */

#include "lpc23xx.h"   /* LPC23XX/24xx Peripheral Registers */
#include "cpu.h"
#include "VIC.h"
#include "ssp0-board.h"
#include "smb380-board.h"
//#include "mma7455l-board.h"
#include "gpioint.h"
#include "hwtimer.h"
#include <stdio.h>
#include "lpm.h"

//uint16_t sampleRateMMA7455L;
//uint16_t interruptTicksMMA7455L;
//
//static void MMA7455L_extIntHandler(void);

uint32_t SSP0Init(void)
{

    /*
     * enable clock to SSP0 for security reason.
     * By default, it's enabled already
     */
    PCONP |= PCSSP0;
    //TODO: configure CLK, MISO, MOSI by default as  GPIOs.
#if USE_CS
    //  P1.20 1.21 1.23 1.24
    //	PINSEL3 |= BIT8 | BIT9 | BIT10 | BIT11 | BIT14 | BIT15 | BIT16 | BIT17;
#else
    //  No SSEL0
    //	PINSEL3 |= BIT8 | BIT9 | BIT14 | BIT15 | BIT16 | BIT17; //1.20 1.23 1.24
#endif

#if SSP1_INTERRUPT_MODE

    if (install_irq(SSP1_INT, (void *)SSP0Handler, HIGHEST_PRIORITY) == FALSE) {
        return (FALSE);
    }

    /*
     * Set SSPINMS registers to enable interrupts,
     * enable all error related interrupts
     */

    SSP1IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;
#endif

    return (1);
}

/*
 * datasize (wordsize) in decimal (4-16), cpol&cpha =(0/1) and frequency divided
 * by 1000 (e.g. 8 MHz = 8000)
 */
uint8_t SSP0Prepare(uint8_t chip, uint8_t datasize, uint8_t cpol, uint8_t cpha,
                    uint16_t freq)
{
    switch (chip) {
        case BMA180_INTERN:
        case SMB380_ACC: {
#if USE_CS
            PINSEL3 |= BIT8 | BIT9 | BIT10 | BIT11 | BIT14 | BIT15 | BIT16 |
                       BIT17;	//P1.20 1.21 1.23 1.24
#else
            // 1.20 1.23 1.24 are not configured as SSEL0
            PINSEL3 |= BIT8 | BIT9 | BIT14 | BIT15 | BIT16 | BIT17;
#endif
            break;
        }

        case BMA180_EXTERN:
        case L3G_EXTERN:
        case NANOPAN:
        case ACAMDMS: {
#if USE_CS
            PINSEL0 |= BIT31;
            PINSEL1 |= BIT1 | BIT3 | BIT5;  // P0.15 0.16 0.17 0.18
#else
            // Turn on NanoPAN
            PINSEL0 |= BIT31;
            // 0.15 0.17 0.18 are not configured as SSEL0
            PINSEL1 |= BIT3 | BIT5;
#endif
            break;
        }

        case NORDIC: {
            PINSEL0 |= BIT31;
            PINSEL1 |= BIT3 | BIT5; // 0.15 0.17 0.18 SSEL0 (No)
            break;
        }

        default: {
            printf("wrong CHIP selected\n");
            return 0;
        }
    }

    uint32_t SSP0CR0tmp = 0;

    switch (datasize) {
        case 4:
            SSP0CR0tmp = BIT0 | BIT1;
            break;

        case 5:
            SSP0CR0tmp = BIT2;
            break;

        case 6:
            SSP0CR0tmp = BIT0 | BIT2;
            break;

        case 7:
            SSP0CR0tmp = BIT1 | BIT2;
            break;

        case 8:
            SSP0CR0tmp = BIT0 | BIT1 | BIT2;
            break;

        case 9:
            SSP0CR0tmp = BIT3;
            break;

        case 10:
            SSP0CR0tmp = BIT0 | BIT3;
            break;

        case 11:
            SSP0CR0tmp = BIT1 | BIT3;
            break;

        case 12:
            SSP0CR0tmp = BIT0 | BIT1 | BIT3;
            break;

        case 13:
            SSP0CR0tmp = BIT2 | BIT3;
            break;

        case 14:
            SSP0CR0tmp = BIT0 | BIT2 | BIT3;
            break;

        case 15:
            SSP0CR0tmp = BIT1 | BIT2 | BIT3;
            break;

        case 16:
            SSP0CR0tmp = BIT0 | BIT1 | BIT2 | BIT3;
            break;

        default:
            return 0;
    }

    if (cpol) {
        SSP0CR0tmp |= BIT6;
    }

    if (cpha) {
        SSP0CR0tmp |= BIT7;
    }

    SSP0CR1 = 0x00; // SSP0 disabled

    // Setting	xx-Bit Datasize, CPOL and CPHA
    SSP0CR0 = SSP0CR0tmp;

    // Clock Setup
    uint32_t pclksel;
    uint32_t cpsr;
    lpc2387_pclk_scale(F_CPU / 1000, freq, &pclksel, &cpsr);
    PCLKSEL1 &= ~(BIT10 | BIT11); // CCLK to PCLK divider ???
    PCLKSEL1 |= pclksel << 10;
    SSP0CPSR = cpsr;

    // Enable
    SSP0CR1 |= BIT1; // SSP0 enabled

    uint32_t dummy;

    for (uint32_t i = 0; i < FIFOSIZE; i++) {
        dummy = SSP0DR; // clear the RxFIFO
    }

    /* to suppress unused-but-set-variable */
    (void) dummy;

    return 1;
}

uint8_t SSP0Unprepare(uint8_t chip)
{
    switch (chip) {
        case BMA180_INTERN:
        case SMB380_ACC: {
            // Turn off Acceleration Sensor
            PINSEL3 &= ~(BIT8 | BIT9 | BIT10 | BIT11 | BIT14 | BIT15 | BIT16 |
                         BIT17);
            FIO1DIR |= BIT20 | BIT21 | BIT24;
            FIO1DIR &= ~BIT23;		 // MISO as Input
            FIO1SET = BIT20 | BIT24;	 /*
                                          * CLK + SSEL + MOSI GPIO as Output
                                          * TODO: depends on CPOL+CPHA Settings
                                          */
            FIO1CLR = BIT21;
            break;
        }

        case BMA180_EXTERN:
        case L3G_EXTERN:
        case NORDIC:
        case NANOPAN:
        case ACAMDMS: {
            // Turn off Nanopan (Pins to GPIO)
            PINSEL0 &= ~(BIT30 | BIT31); //CLK to GPIO
            PINSEL1 &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
            FIO0DIR |= BIT15 | BIT16 | BIT18; //CLK + SSEL + MOSI GPIO as Output
            FIO0DIR &= ~BIT17; // MISO as Input
            FIO0SET = BIT15 | BIT16; /*
                           * CLK + SSEL + MOSI GPIO as Output
      * TODO: depends on CPOL+CPHA Settings
      */
            FIO0CLR = BIT18;
            break;
        }

        default: {
            printf("wrong CHIP selected\n");
            return 0;
        }
    }

    return 1;
}

unsigned char SMB380_ssp_write(const unsigned char regAddr,
                               const unsigned char data, unsigned char flag)
{

    uint16_t temp = 0;

    if (flag) {
        temp = (unsigned int)(((regAddr & 0xFF7F) << 8) | data);

    }
    else {
        temp = (((unsigned int)(regAddr | 0x80) << 8) | 0xFF);
    }

    return SSP0_write(temp, SMB380_ACC);
}

unsigned char SSP0_write(const uint16_t data, uint8_t device)
{
    unsigned char ret = 1;

    while ((SSP0SR & (SSPSR_TNF | SSPSR_BSY)) != SSPSR_TNF)
        ;

    if (!USE_CS) {
        switch (device) {
            case BMA180_INTERN:
            case SMB380_ACC: {
                FIO1CLR = SSP0_SEL;
                break;
            }

            case BMA180_EXTERN:
            case L3G_EXTERN:
            case NANOPAN: {
                printf("[%s] line: %d\n", __FUNCTION__, __LINE__);
                FIO0CLR = SSP0_SELN;
                break;
            }

            case ACAMDMS:
            case NORDIC:
                // Chip Select is done in Nordic driver
                break;

            default:
                ret = 0;
                goto ret;
        }
    }

    SSP0DR = data;

    if (!USE_CS) {
        while (SSP0SR & SSPSR_BSY)
            ;

        switch (device) {
            case BMA180_INTERN:
            case SMB380_ACC: {
                FIO1SET = SSP0_SEL;
                break;
            }

            case BMA180_EXTERN:
            case L3G_EXTERN:
            case NANOPAN: {
                FIO0SET = SSP0_SELN;
                break;
            }

            case ACAMDMS:
            case NORDIC:
                // Chip Select is done in Nordic driver
                break;

            default:
                ret = 0;
                goto ret;
        }
    }

ret:
    return ret;
}

unsigned short SSP0_read(uint8_t device)
{
    uint16_t data;

    while (!(SSP0SR & SSPSR_RNE))
        ;

    if (!USE_CS) {
        switch (device) {
            case BMA180_INTERN:
            case SMB380_ACC: {
                FIO1CLR = SSP0_SEL;
                break;
            }

            case BMA180_EXTERN:
            case L3G_EXTERN:
            case NANOPAN: {
                FIO0CLR = SSP0_SELN;
                break;
            }

            case NORDIC:
            case ACAMDMS:
                // Chip Select is done in Nordic driver
                break;

            default:
                data = 0xFFFF;
                goto ret;
        }
    }

    data = SSP0DR;

    if (!USE_CS) {
        switch (device) {
            case BMA180_INTERN:
            case SMB380_ACC: {
                FIO1SET = SSP0_SEL;
                break;
            }

            case BMA180_EXTERN:
            case L3G_EXTERN:
            case NANOPAN: {
                FIO0SET = SSP0_SELN;
                break;
            }

            case ACAMDMS:
            case NORDIC:
                // Chip Select is done in Nordic driver
                break;

            default:
                data = 0xFFFF;
                goto ret;
        }
    }

ret:
    return data;
}

unsigned short SMB380_ssp_read(void)
{
    return SSP0_read(SMB380_ACC);
}

unsigned short nrf24l01_ssp_read_write(const uint8_t data)
{
    SSP0_write(data, NORDIC);
    return SSP0_read(NORDIC);
}

unsigned short acam_trx(const uint8_t data)
{
    SSP0_write(data, ACAMDMS);
    return SSP0_read(ACAMDMS);
}
