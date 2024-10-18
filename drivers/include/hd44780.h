/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hd44780 HD44780 LCD driver
 * @ingroup     drivers_display
 * @brief       Driver for the Hitachi HD44780 LCD driver
 *
 * @note        The driver currently supports direct addressing, no I2C
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the HD44780 LCD driver
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */
#ifndef HD44780_H
#define HD44780_H

#include <stdint.h>

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximal number of columns supported by the driver
 */
#define HD44780_MAX_COLS        (40U)

/**
 * @brief   Maximal number of rows supported by the driver
 */
#define HD44780_MAX_ROWS        (4U)

/**
 * @brief   Number of data pins for communication 4 or 8.
 */
#define HD44780_MAX_PINS        (8U)

/**
 * @brief   Size of RAM for custom chars
 *
 * Generally the driver could support 8 chars of size 5x8 or 4 of size 5x10.
 * However, most displays only use the former, which is (hard wired) default.
 */
#define HD44780_CGRAM_SIZE      (8U)

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    uint8_t cols;                   /**< number of LCD cols */
    uint8_t rows;                   /**< number of LCD rows */
    gpio_t rs;                      /**< rs gpio pin */
    gpio_t rw;                      /**< rw gpio pin */
    gpio_t enable;                  /**< enable gpio pin */
    gpio_t data[HD44780_MAX_PINS];  /**< data gpio pins */
} hd44780_params_t;

/**
 * @brief   Device descriptor for HD44780 LCD
 */
typedef struct {
    hd44780_params_t p;             /**< LCD config parameters */
    uint8_t flag;                   /**< LCD functional flags */
    uint8_t ctrl;                   /**< LCD control flags */
    uint8_t mode;                   /**< LCD mode flags */
    uint8_t roff[HD44780_MAX_ROWS]; /**< offsets for LCD rows */
} hd44780_t;

/**
 * @brief   Simple state values
 */
typedef enum {
    HD44780_OFF,                    /**< disable feature */
    HD44780_ON                      /**< enable feature */
} hd44780_state_t;

/**
 * @brief   Initialize the given driver
 *
 * @param[out] dev          device descriptor of display to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  0 on success, otherwise error
 */
int hd44780_init(hd44780_t *dev, const hd44780_params_t *params);

/**
 * @brief   Clear display, delete all chars
 *
 * @param[in]  dev          device descriptor of LCD
 */
void hd44780_clear(const hd44780_t *dev);

/**
 * @brief   Reset cursor to row 0 and column 0
 *
 * @param[in]  dev          device descriptor of LCD
 */
void hd44780_home(const hd44780_t *dev);

/**
 * @brief   Set cursor to specific position in column and row
 *
 * @param[in]  dev          device descriptor of LCD
 * @param[in]  col          column position
 * @param[in]  row          row position
 */
void hd44780_set_cursor(const hd44780_t *dev, uint8_t col, uint8_t row);

/**
 * @brief   Turn display on or off
 *
 * @param[in]  dev          device descriptor of LCD
 * @param[in]  state        display on or off
 */
void hd44780_display(hd44780_t *dev, hd44780_state_t state);

/**
 * @brief   Show cursor, on or off
 *
 * @param[in]  dev          device descriptor of LCD
 * @param[in]  state        cursor on or off
 */
void hd44780_cursor(hd44780_t *dev, hd44780_state_t state);

/**
 * @brief   Blink cursor, on or off
 *
 * @param[in]  dev          device descriptor of LCD
 * @param[in]  state        blink on or off
 */
void hd44780_blink(hd44780_t *dev, hd44780_state_t state);

/**
 * @brief   Enable left scrolling
 *
 * @param[in]  dev          device descriptor of LCD
 */
void hd44780_scroll_left(const hd44780_t *dev);

/**
 * @brief   Enable right scrolling
 *
 * @param[in]  dev          device descriptor of LCD
 */
void hd44780_scroll_right(const hd44780_t *dev);

/**
 * @brief   Set display direction left to right
 *
 * @param[in]  dev          device descriptor of LCD
 */
void hd44780_left2right(hd44780_t *dev);

/**
 * @brief   Set display direction right to left
 *
 * @param[in]  dev          device descriptor of LCD
 */
void hd44780_right2left(hd44780_t *dev);

/**
 * @brief   Display autoscroll on or off
 *
 * @param[in]  dev          device descriptor of LCD
 * @param[in]  state        scroll on or off
 */
void hd44780_autoscroll(hd44780_t *dev, hd44780_state_t state);

/**
 * @brief   Create and store a custom character on display memory
 *
 * @param[in]  dev          device descriptor of LCD
 * @param[in]  location     memory location
 * @param[in]  charmap      character bitmap
 *
 * @pre charmap has to be of size HD44780_CGRAM_SIZE, which is 8 by default
 */
void hd44780_create_char(const hd44780_t *dev, uint8_t location, uint8_t charmap[]);

/**
 * @brief   Write a single character on the LCD
 *
 * @param[in]  dev          device descriptor of LCD
 * @param[in]  value        the character to print, i.e., memory location
 */
void hd44780_write(const hd44780_t *dev, uint8_t value);

/**
 * @brief   Write a string on the LCD
 *
 * @param[in]  dev          device descriptor of LCD
 * @param[in]  data         the string to print
 */
void hd44780_print(const hd44780_t *dev, const char *data);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HD44780_H */
