/**
  * cc2420_settings.h - Definitions and settings for the CC2420.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  *
  * This source code is licensed under the GNU Lesser General Public License,
  * Version 2.  See the file LICENSE for more details.
  */


/**
  * @ingroup CC2420
  * @{
  * @file
  * @brief Definitions and settings for the CC2420
  * @author Milan Babel <babel@inf.fu-berlin.de>
  *
  */
#ifndef CC2420_SETTINGS_H
#define CC2420_SETTINGS_H

/* Access addresses */
#define CC2420_READ_ACCESS      0x40
#define CC2420_WRITE_ACCESS     0x00

#define CC2420_RAM_ACCESS       0x80
#define CC2420_RAM_READ_ACCESS  0x20
#define CC2420_RAM_WRITE_ACCESS 0x00

#define CC2420_REG_TXFIFO       0x3E
#define CC2420_REG_RXFIFO       0x3F

/* RAM addresses */
#define CC2420_RAM_SHORTADR     0x16A
#define CC2420_RAM_PANID        0x168
#define CC2420_RAM_IEEEADR      0x160

/* Strobe command addresses */
#define CC2420_STROBE_NOP       0x00
#define CC2420_STROBE_XOSCON    0x01
#define CC2420_STROBE_TXCAL     0x02
#define CC2420_STROBE_RXON      0x03
#define CC2420_STROBE_TXON      0x04
#define CC2420_STROBE_TXONCCA   0x05
#define CC2420_STROBE_RFOFF     0x06
#define CC2420_STROBE_XOSCOFF   0x07
#define CC2420_STROBE_FLUSHRX   0x08
#define CC2420_STROBE_FLUSHTX   0x09
#define CC2420_STROBE_ACK       0x0A
#define CC2420_STROBE_ACKPEND   0x0B
#define CC2420_STROBE_RXDEC     0x0C
#define CC2420_STROBE_TXENC     0x0D
#define CC2420_STROBE_AES       0x0E

/* Register command addresses */
#define CC2420_REG_MAIN         0x10
#define CC2420_REG_MDMCTRL0     0x11
    #define CC2420_ADR_DECODE          0x800
    #define CC2420_RES_FRM_MODE        0x2000
    #define CC2420_PAN_COORD           0x1000
    #define CC2420_AUTOCRC             0x20
    #define CC2420_AUTOACK             0x10
#define CC2420_REG_MDMCTRL1     0x12
#define CC2420_REG_RSSI         0x13
    #define CC2420_CCATHR_MASK         0xFF00
    #define CC2420_RSSI_MASK           0xFF
#define CC2420_REG_SYNCWORD     0x14
#define CC2420_REG_TXCTRL       0x15
    #define CC2420_PALEVEL_MASK        0x1F
#define CC2420_REG_RXCTRL0      0x16
#define CC2420_REG_RXCTRL1      0x17
    #define CC2420_RXBPF_LOCUR         0x2000
#define CC2420_REG_FSCTRL       0x18
    #define CC2420_FREQ_MASK           0x3FF
#define CC2420_REG_SECCTRL0     0x19
    #define CC2420_RXFIFO_PROTECTION   0x200
#define CC2420_REG_SECCTRL1     0x1A
#define CC2420_REG_BATTMON      0x1B
#define CC2420_REG_IOCFG0       0x1C
    #define CC2420_FIFOPTHR_MASK       0x7F
#define CC2420_REG_IOCFG1       0x1D
#define CC2420_REG_MANFIDL      0x1E
#define CC2420_REG_MANFIDH      0x1F
#define CC2420_REG_FSMTC        0x20
#define CC2420_REG_MANAND       0x21
#define CC2420_REG_MANOR        0x22
#define CC2420_REG_AGCCTRL      0x23
#define CC2420_REG_AGCTST0      0x24
#define CC2420_REG_AGCTST1      0x25
#define CC2420_REG_AGCTST2      0x26
#define CC2420_REG_FSTST0       0x27
#define CC2420_REG_FSTST1       0x28
#define CC2420_REG_FSTST2       0x29
#define CC2420_REG_FSTST3       0x2A
#define CC2420_REG_RXBPFTST     0x2B
#define CC2420_REG_FSMSTATE     0x2C
#define CC2420_REG_ADCTST       0x2D
#define CC2420_REG_DACTST       0x2E
#define CC2420_REG_TOPTST       0x2F

#define NOBYTE 0x0

/* Settings */
#define CC2420_DEFAULT_CHANNR   18
#define CC2420_SYNC_WORD_TX_TIME 900000
#define CC2420_RX_BUF_SIZE      3
#define CC2420_WAIT_TIME        500
#endif
