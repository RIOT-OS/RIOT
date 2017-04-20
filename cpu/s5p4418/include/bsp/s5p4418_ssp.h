/*
 * Copyright (C) 2017 PDTECTH Co., LTD
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author      Phuong Dang <kamejoko80@yahoo.com>
 */

#ifndef __S5P4418_SSP_H__
#define __S5P4418_SSP_H__

#include <stdint.h>
#include <stdbool.h>
#include "s5p4418_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief SSP channel definition
 */
typedef enum {
    SSP_CH0 = 0,
    SSP_CH1 = 1,
    SSP_CH2 = 2,
}ssp_channel_t;

/**
 * @brief SSP protocol frame format
 */
typedef enum {
    PROTOCOL_MOTOROLA = 0,  /**< Motorola SSP format             */
    PROTOCOL_TI       = 1,  /**< TI SPI protocol                 */
    PROTOCOL_NATIONAL = 2,  /**< National microwave frame format */
}ssp_protocol_t;

/**
 * @brief SSP data size
 */
typedef enum {
    DATA_SIZE_04_BIT = 3,
    DATA_SIZE_05_BIT = 4,
    DATA_SIZE_06_BIT = 5,
    DATA_SIZE_07_BIT = 6,
    DATA_SIZE_08_BIT = 7,
    DATA_SIZE_09_BIT = 8,
    DATA_SIZE_10_BIT = 9,
    DATA_SIZE_11_BIT = 10,
    DATA_SIZE_12_BIT = 11,
    DATA_SIZE_13_BIT = 12,
    DATA_SIZE_14_BIT = 13,
    DATA_SIZE_15_BIT = 14,
    DATA_SIZE_16_BIT = 15,
}ssp_data_size_t;

/**
 * @brief SSP mode
 */
typedef enum {
    SSP_MODE_0 = 0, /**< CPOL = 0, CPHA = 0 ([SPH:SPO][7:6] = 00) */
    SSP_MODE_1 = 2, /**< CPOL = 0, CPHA = 1 ([SPH:SPO][7:6] = 10) */
    SSP_MODE_2 = 1, /**< CPOL = 1, CPHA = 0 ([SPH:SPO][7:6] = 01) */
    SSP_MODE_3 = 3, /**< CPOL = 1, CPHA = 1 ([SPH:SPO][7:6] = 11) */
}ssp_mode_t;

/**
 * @brief SSP device mode
 */
typedef enum {
    MASTER_MODE = 0,
    DEVICE_MODE = 1,
}ssp_device_mode_t;

/**
 * @brief SSP information structure
 */
typedef struct {
    ssp_channel_t     channel;     /**< ssp channel                   */
    ssp_device_mode_t device_mode; /**< ssp device mode master/slaver */
    ssp_mode_t        mode;        /**< ssp mode                      */
    ssp_protocol_t    protocol;    /**< ssp protocol                  */
    ssp_data_size_t   data_size;   /**< ssp data size                 */
    uint32_t          clock_freq;  /**< ssp clock frequency           */
}ssp_info_t;

/**
 * @brief SSP Register set
 */
typedef struct {
    volatile uint32_t SSPCR0;       /**< 0x000 SPI/SSP control register 0                   */
    volatile uint32_t SSPCR1;       /**< 0x004 SPI/SSP control register 1                   */
    volatile uint32_t SSPDR;        /**< 0x008 SPI/SSP data register                        */
    volatile uint32_t SSPSR;        /**< 0x00C SPI/SSP status register                      */
    volatile uint32_t SSPCPSR;      /**< 0x010 SPI/SSP clock prescaler register             */
    volatile uint32_t SSPIMSC;      /**< 0x014 SPI/SSP interrupt mask set or clear register */
    volatile uint32_t SSPRIS;       /**< 0x018 SPI/SSP raw interrupt status register        */
    volatile uint32_t SSPMIS;       /**< 0x01C SPI/SSP masked interrupt status register     */
    volatile uint32_t SSPICR;       /**< 0x020 SPI/SSP interrupt clear register             */
    volatile uint32_t SSPDMACR;     /**< 0x024 SPI/SSP DMA control register                 */
}ssp_typedef;

/**
 * @brief GPIO bank register base definition
 */
#define SSP0 ((ssp_typedef *) S5P4418_SSP0_BASE)
#define SSP1 ((ssp_typedef *) S5P4418_SSP1_BASE)
#define SSP2 ((ssp_typedef *) S5P4418_SSP2_BASE)

bool s5p4418_ssp_init(ssp_info_t ssp_info);
void s5p4418_ssp_cs_config(uint8_t gpio);
void s5p4418_ssp_gpio_config(ssp_channel_t ch);
void s5p4418_ssp_ncs_out(ssp_channel_t ch, bool val);
void s5p4418_ssp_wr_byte(ssp_channel_t ch, uint8_t *txdata, uint8_t *rxdata);

#ifdef __cplusplus
}
#endif

#endif
