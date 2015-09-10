/*
 * Copyright (C) 2015 Ell-i open source co-operative
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_encx24j600
 * @{
 *
 * @file
 * @brief       Register definitions for the ENCX24J600 Ethernet device
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ENCX24J600_REGS_H
#define ENCX24J600_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SPI instruction set
 * @{
 */
#define RCR         0x00  /* read control register */
#define WCR         0x04  /* write control register */

#define RCRU        0x20  /* read control register unbanked */
#define WCRU        0x22  /* write control register unbanked */

#define BFSU        0x24  /* set bits unbanked */
#define BFCU        0x26  /* clear bits unbanked */

#define RGPDATA     0x28  /* Read EGPDATA */
#define WGPDATA     0x2a  /* Write EGPDATA */

#define RRXDATA     0x2c  /* Read ERXDATA */
#define WRXDATA     0x2e  /* Write ERXDATA */

#define RUDADATA    0x30  /* Read EUDADATA */
#define WUDADATA    0x32  /* Write EUDADATA */

#define BFS         0x80  /* Bit Field Set */
#define BFC         0xa0  /* Bit Field Clear */

#define SETETHRST   0xca  /* System Reset */
#define SETPKTDEC   0xcc  /* Decrements PKTCNT by setting PKTDEC (ECON1<5>) */
#define ENABLERX    0xe8  /* Enables packet reception by setting RXEN (ECON1<0>) */
#define DISABLERX   0xea  /* Disable packet reception by clearing RXEN (ECON1<0>) */
#define SETEIE      0xec  /* Enable Ethernet Interrupts by setting INT (ESTAT<16>) */
#define CLREIE      0xee  /* Disable Ethernet Interrupts by clearing INT (ESTAT<16>) */

#define B0SEL       0xc0  /* select bank 0 */
#define B1SEL       0xc2  /* select bank 0 */
#define B2SEL       0xc4  /* select bank 0 */
#define B3SEL       0xc6  /* select bank 0 */
#define RBSEL       0xc8  /* Read Bank Select */

#define SETTXRTS    0xd4  /* Sets TXRTS (ECON1<1>), sends an Ethernet packet */
/** @} */

/**
 * @name 16bit Registers
 * @{
 */
#define ETXST       0x00
#define ETXLEN      0x02
#define ERXST       0x04
#define ERXTAIL     0x06
#define ERXHEAD     0x08
#define ETXSTAT     0x12
#define ETXWIRE     0x14
#define EUDAST      0x16
#define ESTAT       0x1a
#define EIR         0x1c    /* Interrupt Flag Register */
#define ECON1       0x1e

#define ERXFCON     0x34    /* Receive filter control register */

#define MACON2      0x42
#define MAMXFL      0x4a    /* MAC maximum frame length */

#define MAADR3      0x60    /* MAC address byte 5&6 */
#define MAADR2      0x62    /* MAC address byte 3&4 */
#define MAADR1      0x64    /* MAC address byte 1&2 */

#define MIWR        0x66
#define MIREGADR    0x54

#define ECON2       0x6e

#define EIE         0x72    /* Interrupt Enable Register */

#define EGPRDPT     0x86    /* General Purpose SRAM read pointer */
#define EGPWRPT     0x88    /* General Purpose SRAM write pointer */

#define ERXRDPT     0x8a    /* RX buffer read pointer */
#define ERXWRPT     0x8c    /* RX buffer write pointer */
/** @} */

/**
 * @name PHY Registers
 *
 * (access with phy_reg_* functions)
 *
 * @{
 */
#define PHCON1  0x00
#define PHSTAT1 0x01
#define PHANA   0x04
#define PHANLPA 0x05
#define PHANE   0x06
#define PHCON2  0x11
#define PHSTAT2 0x1b
#define PHSTAT3 0x1f
/** @} */

/**
 * @name ESTAT bits
 * @{
 */
#define PHYLNK  (1<<8)
#define CLKRDY  (1<<12)
/** @} */

/**
 * @name ECON1 bits
 * @{
 */
#define RXEN    (1<<0)
#define TXRTS   (1<<1)
#define DMANOCS (1<<2)
#define DMACSSD (1<<3)
#define DMACPY  (1<<4)
#define DMAST   (1<<5)
#define FCOP0   (1<<6)
#define FCOP1   (1<<7)
#define PKTDEC  (1<<8)
#define AESOP0  (1<<9)
#define AESOP1  (1<<10)
#define AESST   (1<<11)
#define HASHLST (1<<12)
#define HASHOP  (1<<13)
#define HASHEN  (1<<14)
#define MODEXST (1<<15)
/** @} */

/**
 * @name ECON2 bits
 * @{
 */
#define ETHRST    (1<<4)
#define AUTOFC    (1<<7)    /* automatic flow control enable bit */
/** @} */

/**
 * @name EIR bits
 * @{
 */
#define PCFULIE     (1<<0)
#define RXABTIE     (1<<1)
#define TXABTIE     (1<<2)
#define TXIE        (1<<3)
#define DMAIE       (1<<5)
#define PKTIE       (1<<6)
#define LINKIE      (1<<11)
#define AESIE       (1<<12)
#define HASHIE      (1<<13)
#define MODEXIE     (1<<14)
#define INTIE       (1<<15)
/** @} */

/**
 * @name EIR bits
 * @{
 */
#define PCFULIF     (1<<0)
#define RXABTIF     (1<<1)
#define TXABTIF     (1<<2)
#define TXIF        (1<<3)
#define DMAIF       (1<<5)
#define PKTIF       (1<<6)
#define LINKIF      (1<<11)
#define AESIF       (1<<12)
#define HASHIF      (1<<13)
#define MODEXIF     (1<<14)
#define CRYPTEN     (1<<15)
/** @} */

/**
 * @name ERXFCON bits
 */
#define MCEN        (1<<1)
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* ENCX24J600_REGS_H */
/** @} */
