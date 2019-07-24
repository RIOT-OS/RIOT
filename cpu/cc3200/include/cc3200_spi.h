/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc3200_spi CC3200 SPI
 * @ingroup         cpu_cc3200_regs
 * @{
 *
 * @file
 * @brief           Driver for the cc3200 SPI controller
 *
 * Header file with register and macro declarations for the cc3200 SPI module
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 *
 * @{
 */
#ifndef CC3200_SPI_H
#define CC3200_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SPI pin configurations
 * @{
 */
#define SPI_SW_CTRL_CS 0x01000000
#define SPI_HW_CTRL_CS 0x00000000
#define SPI_3PIN_MODE 0x02000000
#define SPI_4PIN_MODE 0x00000000
#define SPI_TURBO_ON 0x00080000
#define SPI_TURBO_OFF 0x00000000
#define SPI_CS_ACTIVEHIGH 0x00000000
#define SPI_CS_ACTIVELOW 0x00000040
/** @} */

/**
 * @brief SPI Word length config
 * @{
 */

/**
 * @brief convert word length in bit to configuration value
 *
 */
#define WORD_LENGTH(x) ((x - 1) << 7)

/**
 * @brief Supported SPI word lengths
 *
 */
#define SPI_WL_8 WORD_LENGTH(8)
#define SPI_WL_16 WORD_LENGTH(16)
#define SPI_WL_32 WORD_LENGTH(32)
/** @} */

/**
 * @brief  SPI Sub modes
 *  Polarity | Phase | Sub-Mode |
 *  -------: | :---: | :------- |
 *     0     |   0   |     0    |
 *     0     |   1   |     1    |
 *     1     |   0   |     2    |
 *     1     |   1   |     3    |
 * @{
 */
#define SPI_SUB_MODE_0 0x00000000
#define SPI_SUB_MODE_1 0x00000001
#define SPI_SUB_MODE_2 0x00000002
#define SPI_SUB_MODE_3 0x00000003
/** @} */

/**
 * @brief  SPI Pin strength in mAs
 * @{
 */
#define PIN_STRENGTH_2MA 0x00000020
#define PIN_STRENGTH_4MA 0x00000040
#define PIN_STRENGTH_6MA 0x00000060
/** @} */

/**
 * @brief  SPI available pin types
 * @{
 */
#define PIN_TYPE_STD 0x00000000
#define PIN_TYPE_STD_PU 0x00000100
#define PIN_TYPE_STD_PD 0x00000200

#define PIN_TYPE_OD 0x00000010
#define PIN_TYPE_OD_PU 0x00000110
#define PIN_TYPE_OD_PD 0x00000210
#define PIN_TYPE_ANALOG 0x10000000
/** @} */

/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T 1
typedef enum {
    SPI_CLK_100KHZ = 100000,   /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = 400000,   /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = 1000000,  /**< drive the SPI bus with 1MHz */
    SPI_CLK_4MHZ   = 4000000,  /**< drive the SPI bus with 4MHz */
    SPI_CLK_5MHZ   = 5000000,  /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000000, /**< drive the SPI bus with 10MHz */
    SPI_CLK_13MHZ  = 13000000, /**< drive the SPI bus with 13MHz */
    SPI_CLK_20MHZ  = 20000000, /**< drive the SPI bus with 20MHz */
    SPI_CLK_30MHZ  = 30000000, /**< drive the SPI bus with 20MHz */
} spi_clk_t;
/** @} */

/**
 * @brief   Override SPI mode settings
 * @{
 */
#define HAVE_SPI_MODE_T 1
typedef enum {
    SPI_MODE_0 = 0x00000000, /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = 0x00000001, /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = 0x00000002, /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = 0x00000003, /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @brief  SPI pin configuration
 * @{
 */
typedef struct {
    uint32_t mosi; /**< pin used for MOSI */
    uint32_t miso; /**< pin used for MISO */
    uint32_t sck;  /**< pin used for SCK */
    uint32_t cs;   /**< pin used for CS */
} spi_pins_t;
/** @} */

/**
 * @brief SPI component register
 *
 */
typedef struct cc3200_spi_t {
    cc3200_reg_t rev;           /**< hardware revision */
    uint8_t hwinfo[16];         /**< hardware info (HDL generics) */
    cc3200_reg_t RESERVED1[60]; /**< Sysconfig */
    uint8_t sys_rev[12];        /**< System revision number */
    cc3200_reg_t sys_conf;      /**< System config */
    cc3200_reg_t sys_status;    /**< Sysstatus */
    cc3200_reg_t irq_status;    /**< IRQStatus */
    cc3200_reg_t irq_enable;    /**< IRQEnable */
    cc3200_reg_t wakeup_enable; /**< Wakeupenable */
    cc3200_reg_t sys_test;      /**< system test mode */
    cc3200_reg_t module_ctrl;   /**< MODULE CTL */
    cc3200_reg_t ch0_conf;      /**< CH0CONF CTL */
    cc3200_reg_t ch0_stat;      /**< CH0 Status register */
    cc3200_reg_t ch0_ctrl;      /**< CH0 Control register */
    cc3200_reg_t tx0;           /**< single spi transmit word */
    cc3200_reg_t rx0;           /**< single spi receive word */
} cc3200_spi_t;
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC3200_SPI_H */

/** @} */