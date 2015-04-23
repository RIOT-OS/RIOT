/*
 * ssp0-borad.h - header file of the SPI interface for the LPC2387.
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @file
 * @internal
 * @brief       SPI interface definitions for the LPC2387
 *
 * @author      Marco Ziegert <ziegert@inf.fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note        $Id:  avsextrem-ssp0.c  3854 2010-01-18 15:27:01Z zkasmi $
 */
#ifndef SSP_H__
#define SSP_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DMA_ENABLED     0

/*
 * if USE_CS is zero, set SSEL as GPIO that you have total control of the
 * sequence
 **/
#define USE_CS          0

/*
 * if 1, use driver for onboard BMA180, otherwise for external BMA180 utilizing
 * Nanopan Connector
 **/
#define BMA180_ONBOARD      1

#define SMB380_ACC          0
#define NANOPAN             1
#define NORDIC              2
#define BMA180_EXTERN       3
#define BMA180_INTERN       4
#define L3G_EXTERN          5
#define L3G_INTERN          6
#define ACAMDMS             7

/* SPI read and write buffer size */
#define BUFSIZE         256
#define FIFOSIZE        8

/* SSP select pin */
#define SSP0_SEL        1 << 21 //P1.21 SMB380

#define SSP0_SELN       1 << 16 //P0.16 Nanotron

/* SSP1 external interrupt Pin (SMB380 specific) */
#define SMB380_INT1     1 << 1  //P0.1
#define BMA180_INT1     1 << 8  //P2.8



/* SSP1 CR0 register */
#define SSPCR0_DSS      1 << 0
#define SSPCR0_FRF      1 << 4
#define SSPCR0_SPO      1 << 6
#define SSPCR0_SPH      1 << 7
#define SSPCR0_SCR      1 << 8

/* SSP1 CR1 register */
#define SSPCR1_LBM      1 << 0
#define SSPCR1_SSE      1 << 1
#define SSPCR1_MS       1 << 2
#define SSPCR1_SOD      1 << 3

/* SSP1 Interrupt Mask Set/Clear register */
#define SSPIMSC_RORIM       1 << 0
#define SSPIMSC_RTIM        1 << 1
#define SSPIMSC_RXIM        1 << 2
#define SSPIMSC_TXIM        1 << 3

/* SSP1 Interrupt Status register */
#define SSPRIS_RORRIS       1 << 0
#define SSPRIS_RTRIS        1 << 1
#define SSPRIS_RXRIS        1 << 2
#define SSPRIS_TXRIS        1 << 3

/* SSP1 Masked Interrupt register */
#define SSPMIS_RORMIS       1 << 0
#define SSPMIS_RTMIS        1 << 1
#define SSPMIS_RXMIS        1 << 2
#define SSPMIS_TXMIS        1 << 3

/* SSP1 Interrupt clear register */
#define SSPICR_RORIC        1 << 0
#define SSPICR_RTIC         1 << 1

#define SSP1_INTERRUPT_MODE     0
#define SMB380_EXTINT_MODE      1
#define BMA180_EXTINT_MODE      1

uint32_t SSP0Init(void);
uint8_t SSP0Prepare(uint8_t chip, uint8_t datasize, uint8_t cpol, uint8_t cpha,
                    uint16_t freq);
uint8_t SSP0Unprepare(uint8_t chip);
unsigned char SSP0_write(const uint16_t data, uint8_t device);
unsigned short SSP0_read(uint8_t device);
unsigned char SMB380_ssp_write(const unsigned char regAddr,
                               const unsigned char data, unsigned char flag);
unsigned short SMB380_ssp_read(void);
unsigned short nrf24l01_ssp_read_write(const uint8_t data);
unsigned short acam_trx(const uint8_t data);
void SSP0Handler(void);

#ifdef __cplusplus
}
#endif

#endif  /* SSP_H__ */
