/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_stm32_lcd_fmc STM32 FMC/FSMC LCD low-level parallel interface driver
 * @ingroup     cpu_stm32
 *
 * @{
 */

#ifndef LCD_FMC_H
#define LCD_FMC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of LCDs using FMC banks
 *
 * It represents the number of elements in LCD FMC bank descriptor array of
 * type @ref lcd_fmc_desc_t. Because it is used by the preprocessor, it has
 * to be defined as a number. It is not possible to use the @ref ARRAY_SIZE
 * macro here.
 *
 * @note `LCD_FMC_NUMOF` has to be equal to the number of elements in the
 *       LCD FMC bank descriptor array of type @ref lcd_fmc_desc_t.
 */
#if DOXYGEN
#define LCD_FMC_NUMOF  1
#endif

/**
 * @brief   Descriptor of the FMC bank used for a LCD
 *
 * The board definition has to specify the array @ref lcd_fmc_desc of type
 * @ref lcd_fmc_desc_t which defines the FMC banks and the address offsets used
 * for the LCD displays that are connected to FMC banks.
 *
 * @note In the case that multiple LCDs are connected to FMC banks, the FMC
 *       bank descriptors for LCDs of type @ref lcd_fmc_desc_t
 *       must be defined in same order as the LCD devices.
 */
typedef struct {
    const fmc_bank_conf_t *bank;    /**< FMC bank config used for the LCD */
    uint32_t cmd_offset;            /**< offset to the bank address used for commands */
    uint32_t data_offset;           /**< offset to the bank address used for data */
} lcd_fmc_desc_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LCD_FMC_H */
