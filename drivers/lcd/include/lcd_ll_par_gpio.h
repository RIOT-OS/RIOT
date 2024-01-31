/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#if !DOXYGEN    /* hide from documentation */

/**
 * @ingroup     drivers_lcd
 *
 * @brief       GPIO-driven low-level parallel interface implementation
 *
 * @{
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef LCD_LL_PAR_GPIO_H
#define LCD_LL_PAR_GPIO_H

#include <assert.h>

#include "lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the GPIOs of the GPIO-driven low-level parallel interface
 *
 * @param[in]   dev     device descriptor
 */

void lcd_ll_par_gpio_init(lcd_t *dev);

/**
 * @brief   Set the direction of the data GPIOs of the low-level parallel interface
 *
 * @param[in]   dev     device descriptor
 * @param[in]   output  set to output mode if true and to input mode otherwise
 */
void lcd_ll_par_gpio_set_data_dir(lcd_t *dev, bool output);

/**
 * @brief   Write command using the MCU-driven low-level parallel interface
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command
 * @param[in]   cont    operation is continued
 */
void lcd_ll_par_gpio_cmd_start(lcd_t *dev, uint8_t cmd, bool cont);

/**
 * @brief   Write a byte to the GPIO-driven low-level parallel interface
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cont    operation is continued
 * @param[in]   out     byte to be written
 */
void lcd_ll_par_gpio_write_byte(lcd_t *dev, bool cont, uint8_t out);

/**
 * @brief   Write a word to the GPIO-driven low-level parallel interface
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cont    operation is continued
 * @param[in]   out     word to be written
 */
void lcd_ll_par_gpio_write_word(lcd_t *dev, bool cont, uint16_t out);

/**
 * @brief   Read a byte from the GPIO-driven low-level parallel interface
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cont    operation is continued
 *
 * @return  read byte
 */
uint8_t lcd_ll_par_gpio_read_byte(lcd_t *dev, bool cont);

/**
 * @brief   Read a word from the GPIO-driven low-level parallel interface
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cont    operation is continued
 *
 * @return  read word
 */
uint16_t lcd_ll_par_gpio_read_word(lcd_t *dev, bool cont);

#ifdef __cplusplus
}
#endif
#endif /* LCD_LL_PAR_GPIO_H */
/** @} */
#endif /* !DOXYGEN */
