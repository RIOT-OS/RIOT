/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ksz8851
 * @{
 *
 * @file
 * @brief       Register definitions for KSZ8851SNL devices
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI command phase opcodes (upper two bits of the first command byte)
 * @{
 */
/** @brief Read from a KSZ8851SNL register */
#define CMD_READ                (0x00)
/** @brief Write to a KSZ8851SNL register */
#define CMD_WRITE               (0x40)
/** @brief Read data from the RX FIFO */
#define CMD_FIFO_READ           (0x80)
/** @brief Write data to the TX FIFO */
#define CMD_FIFO_WRITE          (0xC0)
/** @} */

/**
 * @name    Byte enable bits used for 8/16 bit internal register access
 * @{
 */
/** @brief Enable byte lane 0 */
#define BE0                     (0x1)
/** @brief Enable byte lane 1 */
#define BE1                     (0x2)
/** @brief Enable byte lane 2 */
#define BE2                     (0x4)
/** @brief Enable byte lane 3 */
#define BE3                     (0x8)
/** @brief Enable both byte lanes of the low word */
#define BE_WORD_LOW             (BE0 | BE1)
/** @brief Enable both byte lanes of the high word */
#define BE_WORD_HIGH            (BE2 | BE3)
/** @} */

/**
 * @name    Register offsets (see KSZ8851SNL datasheet chapter 4,
 *          "Register Descriptions").
 * @{
 */
#define REG_CCR                 (0x08)              /**< Chip Configuration Register */
#define REG_MARL                (0x10)              /**< MAC Address Low */
#define REG_MARM                (0x12)              /**< MAC Address Middle */
#define REG_MARH                (0x14)              /**< MAC Address High */
#define REG_GRR                 (0x26)              /**< Global Reset Register */
#define REG_TXCR                (0x70)              /**< Transmit Control Register */
#define REG_TXSR                (0x72)              /**< Transmit Status Register */
#define REG_RXCR1               (0x74)              /**< Receive Control Register 1 */
#define REG_RXCR2               (0x76)              /**< Receive Control Register 2 */
#define REG_TXMIR               (0x78)              /**< TX Memory Information Register */
#define REG_RXFHSR              (0x7C)              /**< RX Frame Header Status Register */
#define REG_RXFHBCR             (0x7E)              /**< RX Frame Header Byte Count Register */
#define REG_TXQCR               (0x80)              /**< Transmit Queue Command Register */
#define REG_RXQCR               (0x82)              /**< Receive Queue Command Register */
#define REG_TXFDPR              (0x84)              /**< TX Frame Data Pointer Register */
#define REG_RXFDPR              (0x86)              /**< RX Frame Data Pointer Register */
#define REG_RXDTTR              (0x8C)              /**< RX Duration Timer Threshold Register */
#define REG_RXDBCTR             (0x8E)              /**< RX Data Byte Count Threshold Register */
#define REG_IER                 (0x90)              /**< Interrupt Enable Register */
#define REG_ISR                 (0x92)              /**< Interrupt Status Register */
#define REG_RXFCTR              (0x9C)              /**< RX Frame Count & Threshold Register */
#define REG_MAHTR0              (0xA0)              /**< MAC Address Hash Table Register 0 */
#define REG_MAHTR1              (0xA2)              /**< MAC Address Hash Table Register 1 */
#define REG_MAHTR2              (0xA4)              /**< MAC Address Hash Table Register 2 */
#define REG_MAHTR3              (0xA6)              /**< MAC Address Hash Table Register 3 */
#define REG_FCLWR               (0xB0)              /**< Flow Control Low Watermark Register */
#define REG_FCHWR               (0xB2)              /**< Flow Control High Watermark Register */
#define REG_CIDER               (0xC0)              /**< Chip ID and Enable Register */
#define REG_PHYRR               (0xD8)              /**< PHY Reset Register */
#define REG_P1MBCR              (0xE4)              /**< PHY 1 MII Basic Control Register */
#define REG_P1MBSR              (0xE6)              /**< PHY 1 MII-Register Basic Status Register */
#define REG_P1CR                (0xF6)              /**< PHY 1 Control Register */
#define REG_P1SR                (0xF8)              /**< PHY 1 Status Register */
/** @} */

/**
 * @name    Global Reset Register (0x26)
 * @{
 */
/** @brief Reset the queue management unit */
#define GRR_QMU_MODULE_SOFT_RESET (1 << 1)
/** @brief Reset all KSZ8851SNL modules */
#define GRR_GLOBAL_SOFT_RESET   (1 << 0)
/** @} */

/**
 * @name    Transmit Control Register (0x70)
 * @{
 */
/** @brief Generate the ICMP checksum */
#define TXCR_TCGICMP            (1 << 8)
/** @brief Generate the TCP checksum */
#define TXCR_TCGTCP             (1 << 6)
/** @brief Generate the IP checksum */
#define TXCR_TCGIP              (1 << 5)
/** @brief Flush the transmit queue */
#define TXCR_FTXQ               (1 << 4)
/** @brief Enable TX flow control frames */
#define TXCR_TXFCE              (1 << 3)
/** @brief Enable TX padding */
#define TXCR_TXPE               (1 << 2)
/** @brief Enable TX CRC generation */
#define TXCR_TXCE               (1 << 1)
/** @brief Enable the transmitter */
#define TXCR_TXE                (1 << 0)
/** @} */

/**
 * @name    Receive Control Register 1 (0x74)
 * @{
 */
/** @brief Flush the receive queue */
#define RXCR1_FRXQ              (1 << 15)
/** @brief Enable UDP frame checksum checking */
#define RXCR1_RXUDPFCC          (1 << 14)
/** @brief Enable TCP frame checksum checking */
#define RXCR1_RXTCPFCC          (1 << 13)
/** @brief Enable IP frame checksum checking */
#define RXCR1_RXIPFCC           (1 << 12)
/** @brief Enable physical-address filtering */
#define RXCR1_RXPAFMA           (1 << 11)
/** @brief Enable flow control frame reception */
#define RXCR1_RXFCE             (1 << 10)
/** @brief Enable reception of error frames */
#define RXCR1_RXEFE             (1 << 9)
/** @brief Enable multicast address filtering */
#define RXCR1_RXMAFMA           (1 << 8)
/** @brief Enable broadcast frame reception */
#define RXCR1_RXBE              (1 << 7)
/** @brief Enable multicast frame reception */
#define RXCR1_RXME              (1 << 6)
/** @brief Enable unicast frame reception */
#define RXCR1_RXUE              (1 << 5)
/** @brief Enable reception of all frames */
#define RXCR1_RXAE              (1 << 4)
/** @brief Invert the receive filter */
#define RXCR1_RXINVF            (1 << 1)
/** @brief Enable the receiver */
#define RXCR1_RXE               (1 << 0)
/** @} */

/**
 * @name    Receive Control Register 2 (0x76)
 * @{
 */
/** @brief SRAM data bus latency field bit 2 */
#define RXCR2_SRDBL2            (1 << 7)
/** @brief SRAM data bus latency field bit 1 */
#define RXCR2_SRDBL1            (1 << 6)
/** @brief SRAM data bus latency field bit 0 */
#define RXCR2_SRDBL0            (1 << 5)
/** @brief Accept frames with an invalid IP checksum */
#define RXCR2_IUFFP             (1 << 4)
/** @brief Zero the checksum field of invalid IP frames */
#define RXCR2_RXIUFCEZ          (1 << 3)
/** @brief Enable UDP-Lite frame reception */
#define RXCR2_UDPLFE            (1 << 2)
/** @brief Enable ICMP frame checksum checking */
#define RXCR2_RXICMPFCC         (1 << 1)
/** @brief Enable source-address filtering */
#define RXCR2_RXSAF             (1 << 0)
/** @} */

/** @brief Mask for the amount of available transmit memory */
#define TXMIR_TXMA_MASK         (0x1FFF)

/**
 * @name    Receive Frame Header Status Register (0x7C)
 * @{
 */
/** @brief Received frame is valid */
#define RXFHSR_RXFV             (1 << 15)
/** @brief Received ICMP frame has a checksum error */
#define RXFHSR_RXICMPFCS        (1 << 13)
/** @brief Received IP frame has a checksum error */
#define RXFHSR_RXIPFCS          (1 << 12)
/** @brief Received TCP frame has a checksum error */
#define RXFHSR_RXTCPFCS         (1 << 11)
/** @brief Received UDP frame has a checksum error */
#define RXFHSR_RXUDPFCS         (1 << 10)
/** @brief Received frame is a broadcast */
#define RXFHSR_RXBF             (1 << 7)
/** @brief Received frame is a multicast */
#define RXFHSR_RXMF             (1 << 6)
/** @brief Received frame is a unicast */
#define RXFHSR_RXUF             (1 << 5)
/** @brief Received frame has an MII error */
#define RXFHSR_RXMR             (1 << 4)
/** @brief Received frame is too long */
#define RXFHSR_RXFT             (1 << 3)
/** @brief Received frame is too long for the FIFO */
#define RXFHSR_RXFTL            (1 << 2)
/** @brief Received frame has an RX error */
#define RXFHSR_RXRF             (1 << 1)
/** @brief Received frame has a CRC error */
#define RXFHSR_RXCE             (1 << 0)
/** Bits that indicate the received frame should be discarded */
#define RXFHSR_RX_ERRORS \
    (RXFHSR_RXMR | RXFHSR_RXFTL | RXFHSR_RXRF | RXFHSR_RXCE)
/** @} */

/** @brief Mask for the received frame byte count */
#define RXFHBCR_RXBC_MASK       (0x0FFF)

/**
 * @name    Transmit Queue Command Register (0x80)
 * @{
 */
/** @brief Automatically transmit the frame at the queue head */
#define TXQCR_AETFE             (1 << 2)
/** @brief Enable manual transmit queue management */
#define TXQCR_TXQMAM            (1 << 1)
/** @brief Enable transmit queue management */
#define TXQCR_METFE             (1 << 0)
/** @} */

/**
 * @name    Receive Queue Command Register (0x82)
 * @{
 */
/** @brief Enable RX duration timer threshold status */
#define RXQCR_RXDTTS            (1 << 12)
/** @brief Enable the RX data byte count threshold interrupt */
#define RXQCR_RXDBCTS           (1 << 11)
/** @brief Enable the RX frame count threshold interrupt */
#define RXQCR_RXFCTS            (1 << 10)
/** @brief Enable the IP header timeout */
#define RXQCR_RXIPHTOE          (1 << 9)
/** @brief Enable the RX duration timer threshold */
#define RXQCR_RXDTTE            (1 << 7)
/** @brief Enable the RX data byte count threshold */
#define RXQCR_RXDBCTE           (1 << 6)
/** @brief Enable the RX frame count threshold */
#define RXQCR_RXFCTE            (1 << 5)
/** @brief Enable automatic RX frame drop */
#define RXQCR_ADRFE             (1 << 4)
/** @brief Start reading the RX data FIFO */
#define RXQCR_SDA               (1 << 3)
/** @brief Release the current RX frame */
#define RXQCR_RRXEF             (1 << 0)
/** @} */

/** @brief Automatically increment the TX frame data pointer */
#define TXFDPR_TXFPAI           (1 << 14)
/** @brief Automatically increment the RX frame data pointer */
#define RXFDPR_RXFPAI           (1 << 14)
/** Frame data pointer low bits, wrapped back to 0 at the start of a frame */
#define RXFDPR_RXFP_MASK        (0x07FF)

/**
 * @name    Interrupt Enable / Interrupt Status Register bits (0x90 / 0x92)
 * @{
 */
#define IER_LCIE                (1 << 15)           /**< Link Change */
#define IER_TXIE                (1 << 14)           /**< Transmit Done */
#define IER_RXIE                (1 << 13)           /**< Receive Done */
#define IER_RXOIE               (1 << 11)           /**< Receive Overrun */
#define IER_TXPSIE              (1 << 9)            /**< Transmit Process Stopped */
#define IER_RXPSIE              (1 << 8)            /**< Receive Process Stopped */
#define IER_TXSAIE              (1 << 6)            /**< Transmit Space Available */
#define IER_RXWFDIE             (1 << 5)            /**< Receive Wakeup Frame Detect */
#define IER_RXMPDIE             (1 << 4)            /**< Receive Magic Packet Detect */
#define IER_LDIE                (1 << 3)            /**< Linkup Detect */
#define IER_EDIE                (1 << 2)            /**< Energy Detect */
#define IER_SPIBEIE             (1 << 1)            /**< SPI Bus Error */
/** @brief Clear all interrupt status flags */
#define ISR_CLEAR_ALL           (0xFFFF)
/** @} */

/**
 * @name    Chip ID and Enable Register (0xC0)
 * @{
 */
/** @brief Mask for the chip identifier field */
#define CIDER_CHIP_ID_MASK      (0xFFF0)
/** @brief KSZ8851SNL family identifier */
#define CIDER_FAMILY_ID         (0x8870)
/** @} */

/** @brief PHY Reset Register (0xD8) */
#define PHYRR_PHY_RESET         (1 << 0)

/**
 * @name    PHY 1 Status Register (0xF8)
 * @{
 */
/** @brief Enable PHY HP Auto-MDIX */
#define P1SR_HP_MDIX            (1 << 15)
/** @brief PHY polarity is reversed */
#define P1SR_POLARITY_REVERSE   (1 << 13)
/** @brief PHY operates at 100 Mbit/s when set */
#define P1SR_OPERATION_SPEED    (1 << 10)
/** @brief PHY operates in full duplex when set */
#define P1SR_OPERATION_DUPLEX   (1 << 9)
/** @brief PHY is using MDIX */
#define P1SR_MDIX_STATUS        (1 << 7)
/** @brief PHY auto-negotiation is complete */
#define P1SR_AN_DONE            (1 << 6)
/** @brief PHY link is established */
#define P1SR_LINK_GOOD          (1 << 5)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
