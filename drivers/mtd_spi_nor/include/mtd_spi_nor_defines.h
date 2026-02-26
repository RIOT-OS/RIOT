/*
 * Copyright (C) 2024 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_mtd_spi_nor
 * @{
 *
 * @file
 * @brief       Definitions for the MTD SPI NOR Flash driver
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 *
 */

#ifndef MTD_SPI_NOR_DEFINES_H
#define MTD_SPI_NOR_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Common Status Bits from the Status Register of SPI NOR Flashs
 * @{
 */
/**
 * @brief Write In Progress Flag (R)
 *
 * 0 - Device is ready
 * 1 - Write cycle in progress and device is busy
 */
#define SPI_NOR_STATUS_WIP  0x01u

/**
 * @brief Write Enable Latch Flag (R/W)
 *
 * 0 - Device is not write enabled
 * 1 - Device is write enabled
 */
#define SPI_NOR_STATUS_WEL  0x02u

/**
 * @brief Block Protection Bit 0 Flag (R/W)
 *
 * 0 - Specific blocks are not write-protected
 * 1 - Specific blocks are write-protected
 */
#define SPI_NOR_STATUS_BP0  0x04u

/**
 * @brief Block Protection Bit 1 Flag (R/W)
 *
 * 0 - Specific blocks are not write-protected
 * 1 - Specific blocks are write-protected
 */
#define SPI_NOR_STATUS_BP1  0x08u

/**
 * @brief Block Protection Bit 2 Flag (R/W)
 *
 * 0 - Specific blocks are not write-protected
 * 1 - Specific blocks are write-protected
 */
#define SPI_NOR_STATUS_BP2  0x10u

/**
 * @brief Block Protection Bit 3 Flag (R/W)
 *
 * 0 - Specific blocks are not write-protected
 * 1 - Specific blocks are write-protected
 */
#define SPI_NOR_STATUS_BP3  0x20u

/**
 * @brief Quad Enable Flag (R/W)
 *
 * 0 - Quad output function disabled
 * 1 - Quad output function enabled
 */
#define SPI_NOR_STATUS_QE   0x40u

/**
 * @brief Status Register Write Disable Flag (R/W)
 *
 * 0 - Status Register is not write protected
 * 1 - Status Register is write protected
 */
#define SPI_NOR_STATUS_SRWD 0x80u

/** @} */

/**
 * @name Macronix Style Security Register Bits
 * @note These flags were taken from the MX25L51245G datasheet, but probably apply
 * to other devices from Macronix as well.
 * @{
 */
/**
 * @brief Secured OTP Flag
 *
 * 0 - OTP area not factory locked
 * 1 - OTP area factory locked
 */
#define MX_SECURITY_SOTP  0x01u

/**
 * @brief Lock-down Secured OTP Flag
 *
 * 0 - OTP area not (user) locked
 * 1 - OTP area locked (can not be programmed/erased)
 */
#define MX_SECURITY_LDSO  0x02u

/**
 * @brief Program Suspend Flag
 *
 * 0 - Program is not suspended
 * 1 - Program suspended
 */
#define MX_SECURITY_PSB   0x04u

/**
 * @brief Erase Suspend Flag
 *
 * 0 - Erase is not suspended
 * 1 - Erase is suspended
 */
#define MX_SECURITY_ESB   0x08u

/**
 * @brief Reserved
 */
#define MX_SECURITY_XXXXX 0x10u

/**
 * @brief Program Fail Flag
 *
 * 0 - Program Operation succeeded
 * 1 - Program Operation failed or region is protected
 */
#define MX_SECURITY_PFAIL 0x20u

/**
 * @brief Erase Fail Flag
 *
 * 0 - Erase Operation succeeded
 * 1 - Erase Operation failed or region is protected
 */
#define MX_SECURITY_EFAIL 0x40u

/**
 * @brief Write Protection Selection Flag
 *
 * 0 - Normal Write Protect mode
 * 1 - Advanced Sector Protection mode
 */
#define MX_SECURITY_WPSEL 0x80u
/** @} */

/**
 * @name ISSI Style Security Register Bits from Extended Read Register (ERP)
 * @note These flags were taken from the IS25LE01G datasheet, but probably
 * apply to other devices from ISSI as well.
 * @{
 */

/**
 * @brief Reserved
 */
#define IS_ERP_XXXXX   0x01u

/**
 * @brief Protection Error Flag (R)
 *
 * 0 - No protection error
 * 1 - Protection Error occurred in program or erase
 */
#define IS_ERP_PROT_E  0x02u

/**
 * @brief Program Error Flag (R)
 *
 * 0 - Program Operation succeeded
 * 1 - Program Operation failed or region is protected
 */
#define IS_ERP_P_ERR   0x04u

/**
 * @brief Erase Error Flag (R)
 *
 * 0 - Erase Operation succeeded
 * 1 - Erase Operation failed or region is protected
 */
#define IS_ERP_E_ERR   0x08u

/**
 * @brief Data Learning Pattern Flag (R/W)
 *
 * 0 - DLP is disabled
 * 1 - DLP is enabled
 */
#define IS_ERP_DLPEN   0x10u

/**
 * @brief Output Driver Strength Bit 0 (R/W)
 */
#define IS_ERP_ODS0    0x20u

/**
 * @brief Output Driver Strength Bit 1 (R/W)
 */
#define IS_ERP_ODS1    0x40u

/**
 * @brief Output Driver Strength Bit 2 (R/W)
 */
#define IS_ERP_ODS2    0x80u
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MTD_SPI_NOR_DEFINES_H */
/** @} */
