/*
 * Copyright (C) 2015 Ell-i open source co-operative
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_encx24j600
 * @{
 *
 * @file
 * @brief       Register definitions for the ENCX24J600 Ethernet device
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ENCX24J600_DEFINES_H
#define ENCX24J600_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI instruction set
 * @{
 */
#define ENC_RCR         0x00  /* read control register */
#define ENC_WCR         0x04  /* write control register */

#define ENC_RCRU        0x20  /* read control register unbanked */
#define ENC_WCRU        0x22  /* write control register unbanked */

#define ENC_BFSU        0x24  /* set bits unbanked */
#define ENC_BFCU        0x26  /* clear bits unbanked */

#define ENC_RGPDATA     0x28  /* Read EGPDATA */
#define ENC_WGPDATA     0x2a  /* Write EGPDATA */

#define ENC_RRXDATA     0x2c  /* Read ERXDATA */
#define ENC_WRXDATA     0x2e  /* Write ERXDATA */

#define ENC_RUDADATA    0x30  /* Read EUDADATA */
#define ENC_WUDADATA    0x32  /* Write EUDADATA */

#define ENC_BFS         0x80  /* Bit Field Set */
#define ENC_BFC         0xa0  /* Bit Field Clear */

#define ENC_SETETHRST   0xca  /* System Reset */
#define ENC_SETPKTDEC   0xcc  /* Decrements PKTCNT by setting PKTDEC (ECON1<5>) */
#define ENC_ENABLERX    0xe8  /* Enables packet reception by setting RXEN (ECON1<0>) */
#define ENC_DISABLERX   0xea  /* Disable packet reception by clearing RXEN (ECON1<0>) */
#define ENC_SETEIE      0xec  /* Enable Ethernet Interrupts by setting INT (ESTAT<16>) */
#define ENC_CLREIE      0xee  /* Disable Ethernet Interrupts by clearing INT (ESTAT<16>) */

#define ENC_B0SEL       0xc0  /* select bank 0 */
#define ENC_B1SEL       0xc2  /* select bank 0 */
#define ENC_B2SEL       0xc4  /* select bank 0 */
#define ENC_B3SEL       0xc6  /* select bank 0 */
#define ENC_RBSEL       0xc8  /* Read Bank Select */

#define ENC_SETTXRTS    0xd4  /* Sets TXRTS (ECON1<1>), sends an Ethernet packet */
/** @} */

/**
 * @name    16bit Registers
 * @{
 */
#define ENC_ETXST       0x00
#define ENC_ETXLEN      0x02
#define ENC_ERXST       0x04
#define ENC_ERXTAIL     0x06
#define ENC_ERXHEAD     0x08
#define ENC_ETXSTAT     0x12
#define ENC_ETXWIRE     0x14
#define ENC_EUDAST      0x16
#define ENC_ESTAT       0x1a
#define ENC_EIR         0x1c    /* Interrupt Flag Register */
#define ENC_ECON1       0x1e

#define ENC_ERXFCON     0x34    /* Receive filter control register */

#define ENC_MACON2      0x42
#define ENC_MAMXFL      0x4a    /* MAC maximum frame length */

#define ENC_MAADR3      0x60    /* MAC address byte 5&6 */
#define ENC_MAADR2      0x62    /* MAC address byte 3&4 */
#define ENC_MAADR1      0x64    /* MAC address byte 1&2 */

#define ENC_MIWR        0x66
#define ENC_MIREGADR    0x54

#define ENC_ECON2       0x6e

#define ENC_EIE         0x72    /* Interrupt Enable Register */

#define ENC_EGPRDPT     0x86    /* General Purpose SRAM read pointer */
#define ENC_EGPWRPT     0x88    /* General Purpose SRAM write pointer */

#define ENC_ERXRDPT     0x8a    /* RX buffer read pointer */
#define ENC_ERXWRPT     0x8c    /* RX buffer write pointer */
/** @} */

/**
 * @name    PHY Registers
 *
 * (access with phy_reg_* functions)
 *
 * @{
 */
#define ENC_PHCON1  0x00
#define ENC_PHSTAT1 0x01
#define ENC_PHANA   0x04
#define ENC_PHANLPA 0x05
#define ENC_PHANE   0x06
#define ENC_PHCON2  0x11
#define ENC_PHSTAT2 0x1b
#define ENC_PHSTAT3 0x1f
/** @} */

/**
 * @name    ESTAT bits
 * @{
 */
#define ENC_PHYLNK  (1<<8)
#define ENC_CLKRDY  (1<<12)
/** @} */

/**
 * @name    ECON1 bits
 * @{
 */
#define ENC_RXEN    (1<<0)
#define ENC_TXRTS   (1<<1)
#define ENC_DMANOCS (1<<2)
#define ENC_DMACSSD (1<<3)
#define ENC_DMACPY  (1<<4)
#define ENC_DMAST   (1<<5)
#define ENC_FCOP0   (1<<6)
#define ENC_FCOP1   (1<<7)
#define ENC_PKTDEC  (1<<8)
#define ENC_AESOP0  (1<<9)
#define ENC_AESOP1  (1<<10)
#define ENC_AESST   (1<<11)
#define ENC_HASHLST (1<<12)
#define ENC_HASHOP  (1<<13)
#define ENC_HASHEN  (1<<14)
#define ENC_MODEXST (1<<15)
/** @} */

/**
 * @name    ECON2 bits
 * @{
 */
#define ENC_ETHRST    (1<<4)
#define ENC_AUTOFC    (1<<7)    /* automatic flow control enable bit */
/** @} */

/**
 * @name    EIR bits
 * @{
 */
#define ENC_PCFULIE     (1<<0)
#define ENC_RXABTIE     (1<<1)
#define ENC_TXABTIE     (1<<2)
#define ENC_TXIE        (1<<3)
#define ENC_DMAIE       (1<<5)
#define ENC_PKTIE       (1<<6)
#define ENC_LINKIE      (1<<11)
#define ENC_AESIE       (1<<12)
#define ENC_HASHIE      (1<<13)
#define ENC_MODEXIE     (1<<14)
#define ENC_INTIE       (1<<15)
/** @} */

/**
 * @name    EIR bits
 * @{
 */
#define ENC_PCFULIF     (1<<0)
#define ENC_RXABTIF     (1<<1)
#define ENC_TXABTIF     (1<<2)
#define ENC_TXIF        (1<<3)
#define ENC_DMAIF       (1<<5)
#define ENC_PKTIF       (1<<6)
#define ENC_LINKIF      (1<<11)
#define ENC_AESIF       (1<<12)
#define ENC_HASHIF      (1<<13)
#define ENC_MODEXIF     (1<<14)
#define ENC_CRYPTEN     (1<<15)
/** @} */

/**
 * @brief    ERXFCON bits
 * @{
 */
#define ENC_MCEN        (1<<1)
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* ENCX24J600_DEFINES_H */
/** @} */
