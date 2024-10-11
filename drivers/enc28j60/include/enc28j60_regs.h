/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_enc28j60
 * @{
 *
 * @file
 * @brief     Register definitions for the ENC28J60 Ethernet device
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ENC28J60_REGS_H
#define ENC28J60_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI instruction set
 * @{
 */
#define CMD_RCR             0x00            /* read control register */
#define CMD_RBM             0x3a            /* read buffer memory */
#define CMD_WCR             0x40            /* write control register */
#define CMD_WBM             0x7a            /* write buffer memory */
#define CMD_BFS             0x80            /* bit field set */
#define CMD_BFC             0xa0            /* bit field clear */
#define CMD_SRC             0xff            /* system reset command (soft reset) */
/** @} */

/**
 * @name    Available address pointers
 * @{
 */
#define ADDR_READ_PTR       0x00    /**< Read pointer */
#define ADDR_WRITE_PTR      0x02    /**< Write data pointer */
#define ADDR_TX_START       0x04    /**< TX buffer start */
#define ADDR_TX_END         0x06    /**< TX buffer end */
#define ADDR_RX_START       0x08    /**< RX buffer start */
#define ADDR_RX_END         0x0a    /**< RX buffer end */
#define ADDR_RX_READ        0x0c    /**< start of oldest packet in RX buffer */
#define ADDR_RX_WRITE       0x0e    /**< start of free space in RX buffer */
/** @} */

/**
 * @name  Shared registers (accessible on each bank)
 * @{
 */
#define REG_EIE             0x1b    /**< interrupt enable */
#define REG_EIR             0x1c    /**< interrupt flags */
#define REG_ESTAT           0x1d    /**< status */
#define REG_ECON2           0x1e    /**< configuration 1 */
#define REG_ECON1           0x1f    /**< configuration 2 */
/** @} */

/**
 * @name    Register in bank 0 - Ethernet registers
 * @{
 */
#define REG_B0_ERDPTL       0x00    /* read data pointer - low byte */
#define REG_B0_ERDPTH       0x01    /* read data pointer - high byte */
#define REG_B0_EWRPTL       0x02    /* write data pointer - low byte */
#define REG_B0_EWRPTH       0x03    /* write data pointer - high byte */
#define REG_B0_ETXSTL       0x04    /* TX start pointer - low byte */
#define REG_B0_ETXSTH       0x05    /* TX start pointer - high byte */
#define REG_B0_ETXNDL       0x06    /* TX end pointer - low byte */
#define REG_B0_ETXNDH       0x07    /* TX end pointer - high byte */
#define REG_B0_ERXSTL       0x08    /* RX start pointer - low byte */
#define REG_B0_ERXSTH       0x09    /* RX start pointer - high byte */
#define REG_B0_ERXNDL       0x0a    /* RX end pointer - low byte */
#define REG_B0_ERXNDH       0x0b    /* RX end pointer - high byte */
#define REG_B0_ERXRDPTL     0x0c    /* RX read pointer - low byte */
#define REG_B0_ERXRDPTH     0x0d    /* RX read pointer - high byte */
#define REG_B0_ERXWRPTL     0x0e    /* RX write pointer - low byte */
#define REG_B0_ERXWRPTH     0x0f    /* RX write pointer - high byte */
#define REG_B0_EDMASTL      0x10    /* DMA start pointer - low byte */
#define REG_B0_EDMASTH      0x11    /* DMA start pointer - high byte */
#define REG_B0_EDMANDL      0x12    /* DMA end pointer - low byte */
#define REG_B0_EDMANDH      0x13    /* DMA end pointer - high byte */
#define REG_B0_EDMADSTL     0x14    /* DMA destination pointer - low byte */
#define REG_B0_EDMADSTH     0x15    /* DMA destination pointer - high byte */
#define REG_B0_EDMACSL      0x16    /* DMA checksum - low byte */
#define REG_B0_EDMACSH      0x17    /* DMA checksum - high byte */
/** @} */

/**
 * @name    Registers in bank 1 - Ethernet registers
 * @{
 */
#define REG_B1_EHT0         0x00    /* hash table - byte 0 */
#define REG_B1_EHT1         0x01    /* hash table - byte 1 */
#define REG_B1_EHT2         0x02    /* hash table - byte 2 */
#define REG_B1_EHT3         0x03    /* hash table - byte 3 */
#define REG_B1_EHT4         0x04    /* hash table - byte 4 */
#define REG_B1_EHT5         0x05    /* hash table - byte 5 */
#define REG_B1_EHT6         0x06    /* hash table - byte 6 */
#define REG_B1_EHT7         0x07    /* hash table - byte 7 */
#define REG_B1_EPMM0        0x08    /* pattern match mask - byte 0 */
#define REG_B1_EPMM1        0x09    /* pattern match mask - byte 1 */
#define REG_B1_EPMM2        0x0a    /* pattern match mask - byte 2 */
#define REG_B1_EPMM3        0x0b    /* pattern match mask - byte 3 */
#define REG_B1_EPMM4        0x0c    /* pattern match mask - byte 4 */
#define REG_B1_EPMM5        0x0d    /* pattern match mask - byte 5 */
#define REG_B1_EPMM6        0x0e    /* pattern match mask - byte 6 */
#define REG_B1_EPMM7        0x0f    /* pattern match mask - byte 7 */
#define REG_B1_EPMCSL       0x10    /* pattern match checksum - low byte */
#define REG_B1_EPMCSH       0x11    /* pattern match checksum - high byte */
#define REG_B1_EPMOL        0x14    /* pattern match offset - low byte */
#define REG_B1_EPMOH        0x15    /* pattern match offset - high byte */
#define REG_B1_ERXFCON      0x18    /* receive filter control register */
#define REG_B1_EPKTCNT      0x19    /* packet count */
/** @} */

/**
 * @name    Registers in bank 2 - MAC registers
 * @{
 */
#define REG_B2_MACON1       0x00    /* MAC control register 1 */
#define REG_B2_MACON3       0x02    /* MAC control register 3 */
#define REG_B2_MACON4       0x03    /* MAC control register 4 */
#define REG_B2_MABBIPG      0x04    /* back-to-back inter-packet gap */
#define REG_B2_MAIPGL       0x06    /* non-back-to-back inter-packet gap - low byte */
#define REG_B2_MAIPGH       0x07    /* non-back-to-back inter-packet gap - high byte */
#define REG_B2_MACLCON1     0x08    /* retransmission maximum */
#define REG_B2_MACLCON2     0x09    /* collision window */
#define REG_B2_MAMXFLL      0x0a    /* maximum frame length - low byte */
#define REG_B2_MAMXFLH      0x0b    /* maximum frame length - high byte */
#define REG_B2_MICMD        0x12    /* MIIM command */
#define REG_B2_MIREGADR     0x14    /* MIIM register address */
#define REG_B2_MIWRL        0x16    /* MIIM write data register - low byte */
#define REG_B2_MIWRH        0x17    /* MIIM write data register - high byte */
#define REG_B2_MIRDL        0x18    /* MIIM read data register - low byte */
#define REG_B2_MIRDH        0x19    /* MIIM read data register - high byte */
/** @} */

/**
 * @name    Registers in bank 3 - MIXED registers
 * @{
 */
#define REG_B3_MAADR5       0x00    /* MAC address - byte 5 */
#define REG_B3_MAADR6       0x01    /* MAC address - byte 6 */
#define REG_B3_MAADR3       0x02    /* MAC address - byte 3 */
#define REG_B3_MAADR4       0x03    /* MAC address - byte 4 */
#define REG_B3_MAADR1       0x04    /* MAC address - byte 1 */
#define REG_B3_MAADR2       0x05    /* MAC address - byte 2 */
#define REG_B3_EBSTSD       0x06    /* built-in self-test fill seed */
#define REG_B3_EBSTCON      0x07    /* built-in self-test control register */
#define REG_B3_EBSTCSL      0x08    /* built-in self-test checksum - low byte */
#define REG_B3_EBSTCSH      0x09    /* built-in self-test checksum - high byte */
#define REG_B3_MISTAT       0x0a    /* MIIM status register */
#define REG_B3_EREVID       0x12    /* Ethernet revision ID */
#define REG_B3_ECOCON       0x15    /* clock output control */
#define REG_B3_EFLOCON      0x17    /* Ethernet flow control */
#define REG_B3_EPAUSL       0x18    /* pause timer value - low byte */
#define REG_B3_EPAUSH       0x19    /* pause timer value - high byte */
/** @} */

/**
 * @name    PHY Registers
 * @{
 */
#define REG_PHY_PHCON1      0x00
#define REG_PHY_PHSTAT1     0x01
#define REG_PHY_PHID1       0x02
#define REG_PHY_PHID2       0x03
#define REG_PHY_PHCON2      0x10
#define REG_PHY_PHSTAT2     0x11
#define REG_PHY_PHIE        0x12
#define REG_PHY_PHIR        0x13
#define REG_PHY_PHLCON      0x14
/** @} */

/**
 * @name    EIE bitfields
 * @{
 */
#define EIE_INTIE           0x80
#define EIE_PKTIE           0x40
#define EIE_DMAIE           0x20
#define EIE_LINKIE          0x10
#define EIE_TXIE            0x08
#define EIE_TXERIE          0x02
#define EIE_RXERIE          0x01
/** @} */

/**
 * @name    EIR bitfields
 * @{
 */
#define EIR_PKTIF           0x40
#define EIR_DMAIF           0x20
#define EIR_LINKIF          0x10
#define EIR_TXIF            0x08
#define EIR_TXERIF          0x02
#define EIR_RXERIF          0x01
/** @} */

/**
 * @name    ESTAT bitfields
 * @{
 */
#define ESTAT_INT           0x80
#define ESTAT_BUFFER        0x40
#define ESTAT_LATECOL       0x10
#define ESTAT_RXBUSY        0x40
#define ESTAT_TXABRT        0x20
#define ESTAT_CLKRDY        0x01
/** @} */

/**
 * @name    ECON1 bitfields
 * @{
 */
#define ECON1_TXRST         0x80
#define ECON1_RXRST         0x40
#define ECON1_DMAST         0x20
#define ECON1_CSUMEN        0x10
#define ECON1_TXRTS         0x08
#define ECON1_RXEN          0x04
#define ECON1_BSEL1         0x02
#define ECON1_BSEL0         0x01
#define ECON1_BSEL_MASK     0x03
/** @} */

/**
 * @name    ECON2 bitfields
 * @{
 */
#define ECON2_AUTOINC       0x80
#define ECON2_PKTDEC        0x40
#define ECON2_PWRSV         0x20
#define ECON2_VRPS          0x40
/** @} */

/**
 * @name    ERXFCON bitfields
 * @{
 */
#define ERXFCON_UCEN        0x80
#define ERXFCON_ANDOR       0x40
#define ERXFCON_CRCEN       0x20
#define ERXFCON_PMEN        0x10
#define ERXFCON_MPEN        0x08
#define ERXFCON_HTEN        0x04
#define ERXFCON_MCEN        0x02
#define ERXFCON_BCEN        0x01
/** @} */

/**
 * @name    MACON1 bitfields
 * @{
 */
#define MACON1_TXPAUS       0x08
#define MACON1_RXPAUS       0x04
#define MACON1_PASSALL      0x02
#define MACON1_MARXEN       0x01
/** @} */

/**
 * @name    MACON3 bitfields
 * @{
 */
#define MACON3_PADCFG2      0x80
#define MACON3_PADCFG1      0x40
#define MACON3_PADCFG0      0x20
#define MACON3_TXCRCEN      0x10
#define MACON3_PHDREN       0x08
#define MACON3_HFRMEN       0x04
#define MACON3_FRMLNEN      0x02
#define MACON3_FULDPX       0x01
/** @} */

/**
 * @name    MACON4 bitfields
 * @{
 */
#define MACON4_DEFER        0x40
#define MACON4_BPEN         0x20
#define MACON4_NOBKOFF      0x10
/** @} */

/**
 * @name    MABBIPG bitfields
 * @{
 */
#define MABBIPG_FD          0x15
#define MABBIPG_HD          0x12
/** @} */

/**
 * @name    MAIPGL bitfields
 * @{
 */
#define MAIPGL_FD           0x12
/** @} */

/**
 * @name    MICMD bitfields
 * @{
 */
#define MICMD_MIISCAN       0x02
#define MICMD_MIIRD         0x01
/** @} */

/**
 * @name    MISTAT bitfields
 * @{
 */
#define MISTAT_NVALID       0x04
#define MISTAT_SCAN         0x02
#define MISTAT_BUSY         0x01
/** @} */

/**
 * @name    EFLOCON bitfields
 * @{
 */
#define EFLOCON_FULDPXS     0x04
#define EFLOCON_FCEN1       0x02
#define EFLOCON_FCEN0       0x01
#define EFLOCON_FCEN_MASK   0x03
/** @} */

/**
 * @name    PHCON1 bitfields
 * @{
 */
#define PHCON1_PRST         0x8000
#define PHCON1_PLOOPBK      0x4000
#define PHCON1_PPWRSV       0x0800
#define PHCON1_PDPXMD       0x0100
/** @} */

/**
 * @name    PHSTAT1 bitfields
 * @{
 */
#define PHSTAT1_PFDPX       0x1000
#define PHSTAT1_PHDPX       0x0800
#define PHSTAT1_LLSTAT      0x0004
#define PHSTAT1_JBSTAT      0x0002
/** @} */

/**
 * @name    PHCON2 bitfields
 * @{
 */
#define PHCON2_FRCLNK       0x4000
#define PHCON2_TXDIS        0x2000
#define PHCON2_JABBER       0x0400
#define PHCON2_HDLDIS       0x0100
/** @} */

/**
 * @name    PHSTAT2 bitfields
 * @{
 */
#define PHSTAT2_TXSTAT      0x2000
#define PHSTAT2_RXSTAT      0x1000
#define PHSTAT2_COLSTAT     0x0800
#define PHSTAT2_LSTAT       0x0400
#define PHSTAT2_DPXSTAT     0x0200
#define PHSTAT2_PLRITY      0x0020
/** @} */

/**
 * @name    PHIE bitfields
 * @{
 */
#define PHIE_PLNKIE         0x0010
#define PHIE_PGEIE          0x0002
/** @} */

/**
 * @name    PHIR bitfields
 * @{
 */
#define PHIR_PLNKIF         0x0010
#define PHIR_PGIF           0x0004
/** @} */

/**
 * @name    PHLCON bitfields
 * @{
 */
#define PHLCON_LACFG(x)     ((x & 0xf) << 8)
#define PHLCON_LBCFG(x)     ((x & 0xf) << 4)
#define PHLCON_LFRQ(x)      ((x & 0x3) << 2)
#define PHLCON_STRCH        0x0002
/** @} */

/**
 * @name    Frame status bitfields
 * @{
 */
#define FRAME_4_RECV_OK     0x80
#define FRAME_4_LENGTH_OOR  0x40
#define FRAME_4_LENGTH_ERR  0x20
#define FRAME_4_CRC_ERR     0x10
#define FRAME_4_CARRIER_EVT 0x04
#define FRAME_4_LONG_EVT    0x01

#define FRAME_5_VLAN        0x40
#define FRAME_5_UKWN_OPCODE 0x20
#define FRAME_5_PAUSE       0x10
#define FRAME_5_RCV_CTRL    0x08
#define FRAME_5_DRIPPLE     0x04
#define FRAME_5_BCAST       0x02
#define FRAME_5_MCAST       0x01
/** @} */

/**
 * @name    TX control byte bitfields
 * @{
 */
#define TX_PHUGEEN          0x08
#define TX_PPADEN           0x04
#define TX_PCRCEN           0x02
#define TX_POVERRIDE        0x01
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ENC28J60_REGS_H */
