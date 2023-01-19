/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_mrf24j40
 * @{
 *
 * @file
 * @brief Register and command definitions for MRF24J40 devices
 *
 * @author      Neo Nenaco <neo@nenaco.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef MRF24J40_REGISTERS_H
#define MRF24J40_REGISTERS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI access specifiers
 * @{
 */
#define MRF24J40_SHORT_ADDR_TRANS       (0x00)
#define MRF24J40_LONG_ADDR_TRANS        (0x80)
#define MRF24J40_ACCESS_READ            (0x00)
#define MRF24J40_ACCESS_WRITE           (0x01)
#define MRF24J40_ACCESS_WRITE_LNG       (0x10)
#define MRF24J40_ADDR_OFFSET            (0x01)
/** @} */

/**
 * @name    FIFO-Address-Map
 * @{
 */
#define MRF24J40_TX_NORMAL_FIFO         (0x000)
#define MRF24J40_TX_BEACON_FIFO         (0x080)
#define MRF24J40_TX_GTS1_FIFO           (0x100)
#define MRF24J40_TX_GTS2_FIFO           (0x180)
#define MRF24J40_RX_FIFO                (0x300)
/** @} */

/**
 * @name    Short-Register addresses
 * @{
 */
#define MRF24J40_REG_RXMCR              (0x00)
#define MRF24J40_REG_PANIDL             (0x01)
#define MRF24J40_REG_PANIDH             (0x02)
#define MRF24J40_REG_SADRL              (0x03)
#define MRF24J40_REG_SADRH              (0x04)
#define MRF24J40_REG_EADR0              (0x05)
#define MRF24J40_REG_EADR1              (0x06)
#define MRF24J40_REG_EADR2              (0x07)
#define MRF24J40_REG_EADR3              (0x08)
#define MRF24J40_REG_EADR4              (0x09)
#define MRF24J40_REG_EADR5              (0x0A)
#define MRF24J40_REG_EADR6              (0x0B)
#define MRF24J40_REG_EADR7              (0x0C)
#define MRF24J40_REG_RXFLUSH            (0x0D)
#define MRF24J40_REG_ORDER              (0x10)
#define MRF24J40_REG_TXMCR              (0x11)
#define MRF24J40_REG_ACKTMOUT           (0x12)
#define MRF24J40_REG_ESLOTG1            (0x13)
#define MRF24J40_REG_SYMTICKL           (0x14)
#define MRF24J40_REG_SYMTICKH           (0x15)
#define MRF24J40_REG_PACON0             (0x16)
#define MRF24J40_REG_PACON1             (0x17)
#define MRF24J40_REG_PACON2             (0x18)
#define MRF24J40_REG_TXBCON0            (0x1A)
#define MRF24J40_REG_TXNCON             (0x1B)
#define MRF24J40_REG_TXG1CON            (0x1C)
#define MRF24J40_REG_TXG2CON            (0x1D)
#define MRF24J40_REG_ESLOTG23           (0x1E)
#define MRF24J40_REG_ESLOTG45           (0x1F)
#define MRF24J40_REG_ESLOTG67           (0x20)
#define MRF24J40_REG_TXPEND             (0x21)
#define MRF24J40_REG_WAKECON            (0x22)
#define MRF24J40_REG_FRMOFFSET          (0x23)
#define MRF24J40_REG_TXSTAT             (0x24)
#define MRF24J40_REG_TXBCON1            (0x25)
#define MRF24J40_REG_GATECLK            (0x26)
#define MRF24J40_REG_TXTIME             (0x27)
#define MRF24J40_REG_HSYMTMRL           (0x28)
#define MRF24J40_REG_HSYMTMRH           (0x29)
#define MRF24J40_REG_SOFTRST            (0x2A)
#define MRF24J40_REG_SECCON0            (0x2C)
#define MRF24J40_REG_SECCON1            (0x2D)
#define MRF24J40_REG_TXSTBL             (0x2E)
#define MRF24J40_REG_RXSR               (0x30)
#define MRF24J40_REG_INTSTAT            (0x31)
#define MRF24J40_REG_INTCON             (0x32)
#define MRF24J40_REG_GPIO               (0x33)
#define MRF24J40_REG_TRISGPIO           (0x34)
#define MRF24J40_REG_SLPACK             (0x35)
#define MRF24J40_REG_RFCTL              (0x36)
#define MRF24J40_REG_SECCR2             (0x37)
#define MRF24J40_REG_BBREG0             (0x38)
#define MRF24J40_REG_BBREG1             (0x39)
#define MRF24J40_REG_BBREG2             (0x3A)
#define MRF24J40_REG_BBREG3             (0x3B)
#define MRF24J40_REG_BBREG4             (0x3C)
#define MRF24J40_REG_BBREG6             (0x3E)
#define MRF24J40_REG_CCAEDTH            (0x3F)
/** @} */

/**
 * @name    Long-Register addresses
 * @{
 */
#define MRF24J40_REG_RFCON0             (0x200)
#define MRF24J40_REG_RFCON1             (0x201)
#define MRF24J40_REG_RFCON2             (0x202)
#define MRF24J40_REG_RFCON3             (0x203)
#define MRF24J40_REG_RFCON5             (0x205)
#define MRF24J40_REG_RFCON6             (0x206)
#define MRF24J40_REG_RFCON7             (0x207)
#define MRF24J40_REG_RFCON8             (0x208)
#define MRF24J40_REG_SLPCAL0            (0x209)
#define MRF24J40_REG_SLPCAL1            (0x20A)
#define MRF24J40_REG_SLPCAL2            (0x20B)
#define MRF24J40_REG_RFSTATE            (0x20F)
#define MRF24J40_REG_RSSI               (0x210)
#define MRF24J40_REG_SLPCON0            (0x211)
#define MRF24J40_REG_SLPCON1            (0x220)
#define MRF24J40_REG_WAKETIMEL          (0x222)
#define MRF24J40_REG_WAKETIMEH          (0x223)
#define MRF24J40_REG_REMCNTL            (0x224)
#define MRF24J40_REG_REMCNTH            (0x225)
#define MRF24J40_REG_MAINCNT0           (0x226)
#define MRF24J40_REG_MAINCNT1           (0x227)
#define MRF24J40_REG_MAINCNT2           (0x228)
#define MRF24J40_REG_MAINCNT3           (0x229)
#define MRF24J40_REG_TESTMODE           (0x22F)
#define MRF24J40_REG_ASSOEADR0          (0x230)
#define MRF24J40_REG_ASSOEADR1          (0x231)
#define MRF24J40_REG_ASSOEADR2          (0x232)
#define MRF24J40_REG_ASSOEADR3          (0x233)
#define MRF24J40_REG_ASSOEADR4          (0x234)
#define MRF24J40_REG_ASSOEADR5          (0x235)
#define MRF24J40_REG_ASSOEADR6          (0x236)
#define MRF24J40_REG_ASSOEADR7          (0x237)
#define MRF24J40_REG_ASSOSADR0          (0x238)
#define MRF24J40_REG_ASSOSADR1          (0x239)
#define MRF24J40_REG_UPNONCE0           (0x240)
#define MRF24J40_REG_UPNONCE1           (0x241)
#define MRF24J40_REG_UPNONCE2           (0x242)
#define MRF24J40_REG_UPNONCE3           (0x243)
#define MRF24J40_REG_UPNONCE4           (0x244)
#define MRF24J40_REG_UPNONCE5           (0x245)
#define MRF24J40_REG_UPNONCE6           (0x246)
#define MRF24J40_REG_UPNONCE7           (0x247)
#define MRF24J40_REG_UPNONCE8           (0x248)
#define MRF24J40_REG_UPNONCE9           (0x249)
#define MRF24J40_REG_UPNONCE10          (0x24A)
#define MRF24J40_REG_UPNONCE11          (0x24B)
#define MRF24J40_REG_UPNONCE12          (0x24C)
/** @} */

/**
 * @name    Timing definition for the mrf24j40.
 * @{
 */
#define MRF24J40_RESET_DELAY                    (2000U)     /* Datasheet MRF24J40 ~2ms */
#define MRF24J40_RESET_PULSE_WIDTH              (20000U)    /* 20ms (estimated */

#define MRF24J40_WAKEUP_DELAY                   (2000U)
/** Undocumented delay. Probably needed because the bit has to be sampled by the low speed sleep clock */
#define MRF24J40_DELAY_SLEEP_TOGGLE             (50U)
#define MRF24J40_STATE_RESET_DELAY              (200U)
/** @} */

/**
 * @name    Bitfield definitions for the RXMCR register (0x00)
 * @{
 */
#define MRF24J40_RXMCR_NOACKRSP                 (0x20)
#define MRF24J40_RXMCR_PANCOORD                 (0x08)
#define MRF24J40_RXMCR_COORD                    (0x04)
#define MRF24J40_RXMCR_ERRPKT                   (0x02)
#define MRF24J40_RXMCR_PROMI                    (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the RXFLUSH register (0x0D)
 * @{
 */
#define MRF24J40_RXFLUSH_WAKEPOL                (0x40)
#define MRF24J40_RXFLUSH_WAKEPAD                (0x20)
#define MRF24J40_RXFLUSH_CMDONLY                (0x08)
#define MRF24J40_RXFLUSH_DATAONLY               (0x04)
#define MRF24J40_RXFLUSH_BCNONLY                (0x02)
#define MRF24J40_RXFLUSH_RXFLUSH                (0x01)
/** @} */

/**
 * @brief   Bitfield definitions for the TXMCR register (0x11)
 * @{
 */
#define MRF24J40_TXMCR_CSMA_BACKOFF_MASK        (0x07)

#define MRF24J40_TXMCR_MACMINBE                 (0x18)
#define MRF24J40_TXMCR_NOCSMA                   (0x80)
#define MRF24J40_TXMCR_BATLIFEXT                (0x40)
#define MRF24J40_TXMCR_SLOTTED                  (0x20)
#define MRF24J40_TXMCR_MACMINBE1                (0x10)
#define MRF24J40_TXMCR_MACMINBE0                (0x08)
#define MRF24J40_TXMCR_CSMABF2                  (0x04)
#define MRF24J40_TXMCR_CSMABF1                  (0x02)
#define MRF24J40_TXMCR_CSMABF0                  (0x01)

/** @} */

/**
 * @brief    Shift offsets for TXMCR register (0x11)
 * @{
 */
#define MRF24J40_TXMCR_MACMINBE_SHIFT           (3U)
/** @} */

/**
 * @brief   Bitfield definitions for the ACKTMOUT register (0x12)
 * @{
 */
#define MRF24J40_ACKTMOUT_DRPACK                (0x80)
#define MRF24J40_ACKTMOUT_MAWD6                 (0x40)
#define MRF24J40_ACKTMOUT_MAWD5                 (0x20)
#define MRF24J40_ACKTMOUT_MAWD4                 (0x10)
#define MRF24J40_ACKTMOUT_MAWD3                 (0x08)
#define MRF24J40_ACKTMOUT_MAWD2                 (0x04)
#define MRF24J40_ACKTMOUT_MAWD1                 (0x02)
#define MRF24J40_ACKTMOUT_MAWD0                 (0x01)

/** @} */

/**
 * @name    Bitfield definitions for the PACON2 register (0x18)
 * @{
 */
#define MRF24J40_PACON2_FIFOEN                  (0x80)
#define MRF24J40_PACON2_TXONTS3                 (0x20)
#define MRF24J40_PACON2_TXONTS2                 (0x10)
#define MRF24J40_PACON2_TXONTS1                 (0x08)
#define MRF24J40_PACON2_TXONTS0                 (0x04)
#define MRF24J40_PACON2_TXONT8                  (0x02)
#define MRF24J40_PACON2_TXONT7                  (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the TXNCON register (0x1B)
 * @{
 */
#define MRF24J40_TXNCON_FPSTAT            (0x10)
#define MRF24J40_TXNCON_INDIRECT          (0x08)
#define MRF24J40_TXNCON_TXNACKREQ         (0x04)
#define MRF24J40_TXNCON_TXNSECEN          (0x02)
#define MRF24J40_TXNCON_TXNTRIG           (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the WAKECON register (0x22)
 * @{
 */
#define MRF24J40_WAKECON_IMMWAKE                (0x80)
#define MRF24J40_WAKECON_REGWAKE                (0x40)
/** @} */

/**
 * @name    Bitfield definitions for the TXSTAT register (0x24)
 * @{
 */
#define MRF24J40_TXSTAT_MAX_FRAME_RETRIES       (0xC0)
#define MRF24J40_TXSTAT_TXNRETRY1               (0x80)
#define MRF24J40_TXSTAT_TXNRETRY0               (0x40)
#define MRF24J40_TXSTAT_CCAFAIL                 (0x20)
#define MRF24J40_TXSTAT_TXG2FNT                 (0x10)
#define MRF24J40_TXSTAT_TXG1FNT                 (0x08)
#define MRF24J40_TXSTAT_TXG2STAT                (0x04)
#define MRF24J40_TXSTAT_TXG1STAT                (0x02)
#define MRF24J40_TXSTAT_TXNSTAT                 (0x01)
/** @} */

/**
 * @name    Shift offsets for TXSTAT register (0x24)
 * @{
 */
#define MRF24J40_TXSTAT_MAX_FRAME_RETRIES_SHIFT (6U)
#define MRF24J40_TXSTAT_CCAFAIL_SHIFT           (5U)
/** @} */

/**
 * @name    Bitfield definitions for the SOFTRST register (0x2A)
 * @{
 */
#define MRF24J40_SOFTRST_RSTPWR                 (0x04)
#define MRF24J40_SOFTRST_RSTBB                  (0x02)
#define MRF24J40_SOFTRST_RSTMAC                 (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the TXSTBL register (0x2E)
 * @{
 */
#define MRF24J40_TXSTBL_RFSTBL3        (0x80)
#define MRF24J40_TXSTBL_RFSTBL2        (0x40)
#define MRF24J40_TXSTBL_RFSTBL1        (0x20)
#define MRF24J40_TXSTBL_RFSTBL0        (0x10)
#define MRF24J40_TXSTBL_MSIFS3         (0x08)
#define MRF24J40_TXSTBL_MSIFS2         (0x04)
#define MRF24J40_TXSTBL_MSIFS1         (0x02)
#define MRF24J40_TXSTBL_MSIFS0         (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the INTSTAT register (0x31)
 * @{
 */
#define MRF24J40_INTSTAT_SLPIF          (0x80)
#define MRF24J40_INTSTAT_WAKEIF         (0x40)
#define MRF24J40_INTSTAT_HSYMTMRIF      (0x20)
#define MRF24J40_INTSTAT_SECIF          (0x10)
#define MRF24J40_INTSTAT_RXIF           (0x08)
#define MRF24J40_INTSTAT_TXG2IF         (0x04)
#define MRF24J40_INTSTAT_TXG1IF         (0x02)
#define MRF24J40_INTSTAT_TXNIF          (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the INTCON register (0x32)
 * @{
 */
#define MRF24J40_INTCON_SLPIE           (0x80)
#define MRF24J40_INTCON_WAKEIE          (0x40)
#define MRF24J40_INTCON_HSYMTMRIE       (0x20)
#define MRF24J40_INTCON_SECIE           (0x10)
#define MRF24J40_INTCON_RXIE            (0x08)
#define MRF24J40_INTCON_TXG2IE          (0x04)
#define MRF24J40_INTCON_TXG1IE          (0x02)
#define MRF24J40_INTCON_TXNIE           (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the GPIO register (0x33)
 * @{
 */
#define MRF24J40_GPIO_0                 (0x01)
#define MRF24J40_GPIO_1                 (0x02)
#define MRF24J40_GPIO_2                 (0x04)
#define MRF24J40_GPIO_3                 (0x08)
#define MRF24J40_GPIO_4                 (0x10)
#define MRF24J40_GPIO_5                 (0x20)
/** @} */

/**
 * @name    Bitfield definitions for the TRISGPIO register (0x34)
 * @{
 */
#define MRF24J40_TRISGPIO_TRISGP5       (0x20)
#define MRF24J40_TRISGPIO_TRISGP4       (0x10)
#define MRF24J40_TRISGPIO_TRISGP3       (0x08)
#define MRF24J40_TRISGPIO_TRISGP2       (0x04)
#define MRF24J40_TRISGPIO_TRISGP1       (0x02)
#define MRF24J40_TRISGPIO_TRISGP0       (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the SLPACK register (0x35)
 * @{
 */
#define MRF24J40_SLPACK_SLPACK                  (0x80)
/** @} */

/**
 * @name    Bitfield definitions for the RFCTL register (0x36)
 * @{
 */
#define MRF24J40_RFCTL_WAKECNT8         (0x10)
#define MRF24J40_RFCTL_WAKECNT7         (0x08)
#define MRF24J40_RFCTL_RFRST            (0x04)
#define MRF24J40_RFCTL_RFTXMODE         (0x02)
#define MRF24J40_RFCTL_RFRXMODE         (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the BBREG1 register (0x39)
 * @{
 */
#define MRF24J40_BBREG1_RXDECINV                (0x04)
/** @} */

/**
 * @name    Bitfield definitions for the BBREG2 register (0x3A)
 * @{
 */
#define MRF24J40_BBREG2_CCAMODE3                (0xC0)
#define MRF25J40_BBREG2_CCAMODE1                (0x80)
#define MRF24J40_BBREG2_CCAMODE2                (0x40)

#define MRF24J40_BBREG2_CCACSTH                 (0x3C)
/** @} */

/**
 * @name    Bitfield definitions for the BBREG6 register (0x3E)
 * @{
 */
#define MRF24J40_BBREG6_RSSIMODE1               (0x80)
#define MRF24J40_BBREG6_RSSIMODE2               (0x40)
#define MRF24J40_BBREG2_RSSIRDY                 (0x01)

#define MRF24J40_BBREG2_CCACSTH                 (0x3C)
/** @} */

/**
 * @name    Bitfield definitions for the RFCON1 register (0x201)
 * @{
 */
#define MRF24J40_RFCON1_VCOOPT7         (0x80)
#define MRF24J40_RFCON1_VCOOPT6         (0x40)
#define MRF24J40_RFCON1_VCOOPT5         (0x20)
#define MRF24J40_RFCON1_VCOOPT4         (0x10)
#define MRF24J40_RFCON1_VCOOPT3         (0x08)
#define MRF24J40_RFCON1_VCOOPT2         (0x04)
#define MRF24J40_RFCON1_VCOOPT1         (0x02)
#define MRF24J40_RFCON1_VCOOPT0         (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the RFCON2 register (0x202)
 * @{
 */
#define MRF24J40_RFCON2_PLLEN           (0x80)
/** @} */

/**
 * @name    Bitfield definitions for the RFCON6 register (0x206)
 * @{
 */
#define MRF24J40_RFCON6_TXFIL           (0x80)
#define MRF24J40_RFCON6_20MRECVR        (0x10)
#define MRF24J40_RFCON6_BATEN           (0x08)
/** @} */

/**
 * @name    Bitfield definitions for the RFCON7 register (0x207)
 * @{
 */
#define MRF24J40_RFCON7_SLPCLKSEL1      (0x80)
#define MRF24J40_RFCON7_SLPCLKSEL2      (0x40)
/** @} */

/**
 * @name    Bitfield definitions for the RFCON8 register (0x208)
 * @{
 */
#define MRF24J40_RFCON8_RFVCO           (0x10)
/** @} */

/**
 * @name    Bitfield definitions for the RFSTATE register (0x20F)
 * @{
 */
#define MRF24J40_RFSTATE_MASK           (0xA0)
#define MRF24J40_RFSTATE_RTSEL2         (0xE0)
#define MRF24J40_RFSTATE_RTSEL1         (0xC0)
#define MRF24J40_RFSTATE_RX             (0xA0)
#define MRF24J40_RFSTATE_TX             (0x80)
#define MRF24J40_RFSTATE_CALVCO         (0x60)
#define MRF24J40_RFSTATE_SLEEP          (0x40)
#define MRF24J40_RFSTATE_CALFIL         (0x20)
#define MRF24J40_RFSTATE_RESET          (0x00)
/** @} */

/**
 * @name    Bitfield definitions for the SLPCON1 register (0x211)
 * @{
 */
#define MRF24J40_SLPCON0_INTEDGE        (0x02)
#define MRF24J40_SLPCON0_SLPCLKEN       (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the SLPCON1 register (0x212)
 * @{
 */
#define MRF24J40_SLPCON1_CLKOUTEN       (0x20)
#define MRF24J40_SLPCON1_SLPCLKDIV4     (0x10)
#define MRF24J40_SLPCON1_SLPCLKDIV3     (0x08)
#define MRF24J40_SLPCON1_SLPCLKDIV2     (0x04)
#define MRF24J40_SLPCON1_SLPCLKDIV1     (0x02)
#define MRF24J40_SLPCON1_SLPCLKDIV0     (0x01)
/** @} */

/**
 * @name    Bitfield definitions for the TESTMODE register (0x22F)
 * @{
 */
#define MRF24J40_TESTMODE_RSSIWAIT1     (0x10)
#define MRF24J40_TESTMODE_RSSIWAIT0     (0x08)
#define MRF24J40_TESTMODE_TESTMODE2     (0x04)
#define MRF24J40_TESTMODE_TESTMODE1     (0x02)
#define MRF24J40_TESTMODE_TESTMODE0     (0x01)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MRF24J40_REGISTERS_H */
/** @} */
