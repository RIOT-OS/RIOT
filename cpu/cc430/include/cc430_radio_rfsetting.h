/*
 * Copyright (C) 2018 Baptiste Cartier
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/*****************************************************************
 * Configuration file for the cc1101 registers, change the desired value here.
 * Texas Instrument's SmartRF Studio software can be used to generate this software,
 * but it requires a Windows OS to work.
*****************************************************************/


#ifndef CC430_RADIO_RFSETTING_H
#define CC430_RADIO_RFSETTING_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RECEIVE_OR_TRANSMIT
#define RECEIVE_OR_TRANSMIT 1   //1 for transmission, else for reception
#endif

/*****************************************************************
 * Example configuration for radio module
*****************************************************************/

#if RECEIVE_OR_TRANSMIT == 1

#define CC430_RADIO_RFSETTING_IOCFG2     0x29   //RF_RDYn , active high
#define CC430_RADIO_RFSETTING_IOCFG1     0x2E   //Tri-State, active high
#define CC430_RADIO_RFSETTING_IOCFG0     0x06
#define CC430_RADIO_RFSETTING_FIFOTHR    0x40   //61 bytes in TX FIFO
#define CC430_RADIO_RFSETTING_SYNC1      0xD3   //default
#define CC430_RADIO_RFSETTING_SYNC0      0x91   //default
#define CC430_RADIO_RFSETTING_PKTLEN     0x40   //64 maximum packet length
#define CC430_RADIO_RFSETTING_PKTCTRL1   0x00   //No address check, no CRC flush, no bytes appended
#define CC430_RADIO_RFSETTING_PKTCTRL0   0x01   //Varaible packet length, no data Whitening, Normal mode, no CRC calculation and check
#define CC430_RADIO_RFSETTING_ADDR       0x00   //Device address 0x00
#define CC430_RADIO_RFSETTING_CHANNR     0xC0   //Channel 192
#define CC430_RADIO_RFSETTING_FSCTRL1    0x06   //Desired frequency
#define CC430_RADIO_RFSETTING_FSCTRL0    0x00   //Frequency offset
#define CC430_RADIO_RFSETTING_FREQ2      0x1D
#define CC430_RADIO_RFSETTING_FREQ1      0xEE
#define CC430_RADIO_RFSETTING_FREQ0      0x3F
#define CC430_RADIO_RFSETTING_MDMCFG4    0xF5
#define CC430_RADIO_RFSETTING_MDMCFG3    0x83
#define CC430_RADIO_RFSETTING_MDMCFG2    0x13   //2-GFSK, 30/32 sync word bits detected, Manchester disabled
#define CC430_RADIO_RFSETTING_MDMCFG1    0x22   //4 preamble bytes
#define CC430_RADIO_RFSETTING_MDMCFG0    0xF8
#define CC430_RADIO_RFSETTING_DEVIATN    0x15
#define CC430_RADIO_RFSETTING_MCSM2      0x07   //Timeout disabled
#define CC430_RADIO_RFSETTING_MCSM1      0x3C
#define CC430_RADIO_RFSETTING_MCSM0      0x10
#define CC430_RADIO_RFSETTING_FOCCFG     0x16
#define CC430_RADIO_RFSETTING_BSCFG      0x6C
#define CC430_RADIO_RFSETTING_AGCCTRL2   0x03
#define CC430_RADIO_RFSETTING_AGCCTRL1   0x40
#define CC430_RADIO_RFSETTING_AGCCTRL0   0x91
#define CC430_RADIO_RFSETTING_WOREVT1    0x80
#define CC430_RADIO_RFSETTING_WOREVT0    0x00
#define CC430_RADIO_RFSETTING_WORCTRL    0xFB
#define CC430_RADIO_RFSETTING_FREND1     0x56
#define CC430_RADIO_RFSETTING_FREND0     0x10
#define CC430_RADIO_RFSETTING_FSCAL3     0xE9
#define CC430_RADIO_RFSETTING_FSCAL2     0x2A
#define CC430_RADIO_RFSETTING_FSCAL1     0x00
#define CC430_RADIO_RFSETTING_FSCAL0     0x1F
#define CC430_RADIO_RFSETTING_FSTEST     0x59
#define CC430_RADIO_RFSETTING_PTEST      0x7F
#define CC430_RADIO_RFSETTING_AGCTEST    0x3F
#define CC430_RADIO_RFSETTING_TEST2      0x81
#define CC430_RADIO_RFSETTING_TEST1      0x35
#define CC430_RADIO_RFSETTING_TEST0      0x0B

#else

#define CC430_RADIO_RFSETTING_IOCFG2     0x29   //RF_RDYn , active high
#define CC430_RADIO_RFSETTING_IOCFG1     0x2E   //Tri-State, active high
#define CC430_RADIO_RFSETTING_IOCFG0     0x06
#define CC430_RADIO_RFSETTING_FIFOTHR    0x40   //64 bytes in RX FIFO
#define CC430_RADIO_RFSETTING_SYNC1      0xD3   //default
#define CC430_RADIO_RFSETTING_SYNC0      0x91   //default
#define CC430_RADIO_RFSETTING_PKTLEN     0x40   //64 maximum packet length
#define CC430_RADIO_RFSETTING_PKTCTRL1   0x00   //No address check, no CRC flush, no bytes appended
#define CC430_RADIO_RFSETTING_PKTCTRL0   0x01   //Varaible packet length, no data Whitening, Normal mode, no CRC calculation and check
#define CC430_RADIO_RFSETTING_ADDR       0x00   //Device address 0x00
#define CC430_RADIO_RFSETTING_CHANNR     0xC0   //Channel 192
#define CC430_RADIO_RFSETTING_FSCTRL1    0x06   //Desired frequency
#define CC430_RADIO_RFSETTING_FSCTRL0    0x00   //Frequency offset
#define CC430_RADIO_RFSETTING_FREQ2      0x1D
#define CC430_RADIO_RFSETTING_FREQ1      0xEE
#define CC430_RADIO_RFSETTING_FREQ0      0x3F
#define CC430_RADIO_RFSETTING_MDMCFG4    0xF5
#define CC430_RADIO_RFSETTING_MDMCFG3    0x83
#define CC430_RADIO_RFSETTING_MDMCFG2    0x13   //2-GFSK, 30/32 sync word bits detected, Manchester disabled
#define CC430_RADIO_RFSETTING_MDMCFG1    0x22   //4 preamble bytes
#define CC430_RADIO_RFSETTING_MDMCFG0    0xF8
#define CC430_RADIO_RFSETTING_DEVIATN    0x15
#define CC430_RADIO_RFSETTING_MCSM2      0x07   //Timeout disabled
#define CC430_RADIO_RFSETTING_MCSM1      0x3C
#define CC430_RADIO_RFSETTING_MCSM0      0x10
#define CC430_RADIO_RFSETTING_FOCCFG     0x16
#define CC430_RADIO_RFSETTING_BSCFG      0x6C
#define CC430_RADIO_RFSETTING_AGCCTRL2   0x03
#define CC430_RADIO_RFSETTING_AGCCTRL1   0x40
#define CC430_RADIO_RFSETTING_AGCCTRL0   0x91
#define CC430_RADIO_RFSETTING_WOREVT1    0x80
#define CC430_RADIO_RFSETTING_WOREVT0    0x00
#define CC430_RADIO_RFSETTING_WORCTRL    0xFB
#define CC430_RADIO_RFSETTING_FREND1     0x56
#define CC430_RADIO_RFSETTING_FREND0     0x10
#define CC430_RADIO_RFSETTING_FSCAL3     0xE9
#define CC430_RADIO_RFSETTING_FSCAL2     0x2A
#define CC430_RADIO_RFSETTING_FSCAL1     0x00
#define CC430_RADIO_RFSETTING_FSCAL0     0x1F
#define CC430_RADIO_RFSETTING_FSTEST     0x59
#define CC430_RADIO_RFSETTING_PTEST      0x7F
#define CC430_RADIO_RFSETTING_AGCTEST    0x3F
#define CC430_RADIO_RFSETTING_TEST2      0x81
#define CC430_RADIO_RFSETTING_TEST1      0x35
#define CC430_RADIO_RFSETTING_TEST0      0x0B

#endif /*RECEIVE_OR_TRANSMIT == 1*/

#ifdef __cplusplus
}
#endif

#endif /* CC430_RADIO_RFSETTING_H */