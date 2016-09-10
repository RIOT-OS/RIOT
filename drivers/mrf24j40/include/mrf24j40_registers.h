/**
 * @ingroup drivers_mrf24j40
 * @{
 *
 * @file
 * @brief Register and command definitions for MRF24J40 devices
 *
 * @author Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 */

#ifndef MRF24J40_REGISTERS_H_
#define MRF24J40_REGISTERS_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief SPI access specifiers
 ****@{
 */
#define MRF24J40_SHORT_ADDR_TRANS       (0x00)
#define MRF24J40_LONG_ADDR_TRANS        (0x80)
#define MRF24J40_ACCESS_READ            (0x00)
#define MRF24J40_ACCESS_WRITE           (0x01)
#define MRF24J40_ACCESS_WRITE_LNG       (0x10)
#define MRF24J40_ADDR_OFFSET            (0x01)
/** @} */

/**
 * @brief FIFO-Address-Map
 ***@{
 */
#define MRF24J40_TX_NORMAL_FIFO                     (0x000)
#define MRF24J40_TX_BEACON_FIFO                     (0x080)
#define MRF24J40_TX_GTS1_FIFO                       (0x100)
#define MRF24J40_TX_GTS2_FIFO                       (0x180)
#define MRF24J40_RX_FIFO                            (0x300)
/** @} */

/**
 * @brief Short-Register addresses
 ****@{
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
 * @brief Long-Register addresses
 ****@{
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
 * @brief   Bitfield definitions for the RFSTATE register
 * @{
 */
#define MRF24J40_RFSTATE_RTSEL2         (0xE0)
#define MRF24J40_RFSTATE_RTSEL1         (0xC0)
#define MRF24J40_RFSTATE_RX             (0xA0)
#define MRF24J40_RFSTATE_TX             (0x80)
#define MRF24J40_RFSTATE_CALVCO         (0x60)
#define MRF24J40_RFSTATE_SLEEP          (0x40)
#define MRF24J40_RFSTATE_CALFIL         (0x20)
#define MRF24J40_RFSTATE_RESET          (0x00)

/**
 *
 */

#define MRF24J40_RESET_DELAY                    (2000U)     /* Datasheet MRF24J40 ~2ms */
#define MRF24J40_RESET_PULSE_WIDTH              (20000U)    /* 20ms (estimated */

#define MRF24J40_REG_TXMCR_CSMA_BACKOFF_MASK    (0x1F)
#define MRF24J40_WAKEUP_DELAY                   (2000U)


/**
 * @brief   Bitfield definitions for the INTCON register
 * @{
 */
#define MRF24J40_INTCON_MASK__SLPIE             (0x80)
#define MRF24J40_INTCON_MASK__WAKEIE            (0x40)
#define MRF24J40_INTCON_MASK__HSYMTMRIE         (0x20)
#define MRF24J40_INTCON_MASK__SECIE             (0x10)
#define MRF24J40_INTCON_MASK__RXIE              (0x08)
#define MRF24J40_INTCON_MASK__TXG2IE            (0x04)
#define MRF24J40_INTCON_MASK__TXG1IE            (0x02)
#define MRF24J40_INTCON_MASK__TXNIE             (0x01)
/** @} */

/**
 * @brief   Bitfield definitions for the INTSTAT register
 * @{
 */
#define MRF24J40_INTSTAT_MASK__SLPIF            (0x80)
#define MRF24J40_INTSTAT_MASK__WAKEIF           (0x40)
#define MRF24J40_INTSTAT_MASK__HSYMTMRIF        (0x20)
#define MRF24J40_INTSTAT_MASK__SECIF            (0x10)
#define MRF24J40_INTSTAT_MASK__RXIF             (0x08)
#define MRF24J40_INTSTAT_MASK__TXG2IF           (0x04)
#define MRF24J40_INTSTAT_MASK__TXG1IF           (0x02)
#define MRF24J40_INTSTAT_MASK__TXNIF            (0x01)
/** @} */

/**
 * @brief   Bitfield definitions for the TXNCON register
 * @{
 */
#define MRF24J40_TXNCON_MASK__FPSTAT            (0x10)
#define MRF24J40_TXNCON_MASK__INDIRECT          (0x08)
#define MRF24J40_TXNCON_MASK__TXNACKREQ         (0x04)
#define MRF24J40_TXNCON_MASK__TXNSECEN          (0x02)
#define MRF24J40_TXNCON_MASK__TXNTRIG           (0x01)
/** @} */

/**
 * @brief   Bitfield definitions for the TXSTAT register
 * @{
 */
#define MRF24J40_TXSTAT__MAX_FRAME_RETRIES      (0b11000000)
#define MRF24J40_TXSTAT_MASK__TXNRETRY1         (0b10000000)
#define MRF24J40_TXSTAT_MASK__TXNRETRY0         (0b01000000)
#define MRF24J40_TXSTAT_MASK__CCAFAIL           (0b00100000)
#define MRF24J40_TXSTAT_MASK__TXG2FNT           (0b00010000)
#define MRF24J40_TXSTAT_MASK__TXG1FNT           (0b00001000)
#define MRF24J40_TXSTAT_MASK__TXG2STAT          (0b00000100)
#define MRF24J40_TXSTAT_MASK__TXG1STAT          (0b00000010)
#define MRF24J40_TXSTAT_MASK__TXNSTAT           (0b00000001)

/**
 * @brief   Bitfield definitions for the TXMCR register
 * @{
 */
#define MRF24J40_TXMCR__CSMA_CSMABF             (0b100)
#define MRF24J40_TXMCR__MAX_BACKOFF             (0b111)
#define MRF24J40_TXMCR_MASK__MACMINBE           (0b00011000)

#define MRF24J40_TXMCR_MASK__NOCSMA             (0b10000000)
#define MRF24J40_TXMCR_MASK__BATLIFEXT          (0b01000000)
#define MRF24J40_TXMCR_MASK__SLOTTED            (0b00100000)
/** @} */

/**
 * @brief   Bitfield definitions for the RXMCR register
 * @{
 */
#define MRF24J40_RXMCR_MASK__NOACKRSP                (0b00100000)
#define MRF24J40_RXMCR_MASK__PANCOORD                (0b00001000)
#define MRF24J40_RXMCR_MASK__COORD                   (0b00000100)
#define MRF24J40_RXMCR_MASK__ERRPKT                  (0b00000010)
#define MRF24J40_RXMCR_MASK__PROMI                   (0b00000001)
/** @} */

/**
 * @brief   Bitfield definitions for the BBREG1 register
 * @{
 */
#define MRF24J40_BBREG1_MASK__RXDECINV               (0b00000100)
/** @} */

/**
 * @brief   Bitfield definitions for the BBREG2 register
 * @{
 */
#define MRF24J40_BBREG2__CCAMODE3               (0b11000000)
#define MRF24J40_BBREG2__CCAMODE1               (0b10000000)
#define MRF24J40_BBREG2__CCAMODE2               (0b01000000)

#define MRF24J40_BBREG2__CCACSTH_MASK           (0b00111100)
/** @} */

/**
 * @brief   Bitfield definitions for the BBREG6 register
 * @{
 */
#define MRF24J40_BBREG6_MASK__RSSIMODE1         (0b10000000)
#define MRF24J40_BBREG6_MASK__RSSIMODE2         (0b01000000)
#define MRF24J40_BBREG2_MASK__RSSIRDY           (0b00000001)

#define MRF24J40_BBREG2__CCACSTH_MASK           (0b00111100)
/** @} */

/**
 * @brief   Bitfield definitions for the WAKECON register
 * @{
 */
#define MRF24J40_WAKECON_MASK__IMMWAKE          (0b10000000)
#define MRF24J40_WAKECON_MASK__REGWAKE          (0b01000000)
/** @} */

/**
 * @brief   Bitfield definitions for the SOFTRST register
 * @{
 */
#define MRF24J40_SOFTRST_MASK__RSTPWR           (0b00000100)
#define MRF24J40_SOFTRST_MASK__RSTBB            (0b00000010)
#define MRF24J40_SOFTRST_MASK__RSTMAC           (0b00000001)
/** @} */

/**
 * @brief   Bitfield definitions for the SLPACK register
 * @{
 */
#define MRF24J40_SLPACK_MASK__SLPACK            (0b10000000)
/** @} */

/**
 * @brief   Bitfield definitions for the RXFLUSH register
 * @{
 */
#define MRF24J40_RXFLUSH_MASK__WAKEPOL          (0b01000000)
#define MRF24J40_RXFLUSH_MASK__WAKEPAD          (0b00100000)
#define MRF24J40_RXFLUSH_MASK__CMDONLY          (0b00001000)
#define MRF24J40_RXFLUSH_MASK__DATAONLY         (0b00000100)
#define MRF24J40_RXFLUSH_MASK__BCNONLY          (0b00000010)
#define MRF24J40_RXFLUSH_MASK__RXFLUSH          (0b00000001)
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* MRF24J40_REGISTERS_H */
/** @} */

