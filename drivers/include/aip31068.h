/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_aip31068 AIP31068 I2C LCD controller
 * @ingroup     drivers_display
 * @brief       Device driver for AIP31068
 *
 * @{
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdbool.h"
#include "periph/i2c.h"

#include "aip31068_regs.h"

/**
* @brief Keys for custom symbols.
*/
typedef enum {
    CUSTOM_SYMBOL_1 = 0,    /**< 1st custom symbol */
    CUSTOM_SYMBOL_2 = 1,    /**< 2nd custom symbol */
    CUSTOM_SYMBOL_3 = 2,    /**< 3rd custom symbol */
    CUSTOM_SYMBOL_4 = 3,    /**< 4th custom symbol */
    CUSTOM_SYMBOL_5 = 4,    /**< 5th custom symbol */
    CUSTOM_SYMBOL_6 = 5,    /**< 6th custom symbol */
    CUSTOM_SYMBOL_7 = 6,    /**< 7th custom symbol */
    CUSTOM_SYMBOL_8 = 7,    /**< 8th custom symbol */
} aip31068_custom_symbol_t;

/**
 * @brief Defines the direction of the text insertion. Starting from the cursor,
 *        either increment the column of the cursor position after insertion
 *        (LEFT_TO_RIGHT), or decrement the current column of the cursor
 *        position after insertion (RIGHT_TO_LEFT).
 */
typedef enum {
    LEFT_TO_RIGHT,  /**< Insert text from left to right */
    RIGHT_TO_LEFT,  /**< Insert text from right to left */
} aip31068_text_insertion_mode_t;

/**
* @brief Size of a character of the display in dots/pixels.
*/
typedef enum {
    FONT_SIZE_5x8,  /**< Single character has 5x8 pixels */
    FONT_SIZE_5x10, /**< Single character has 5x10 pixels */
} aip31068_font_size_t;

/**
 * @brief Bit mode for the display.
 */
typedef enum {
    BITMODE_4_BIT,  /**< Use 4 bit mode */
    BITMODE_8_BIT,  /**< Use 8 bit mode */
} aip31068_bit_mode_t;

/**
 * @brief   AIP31068 device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                     /**< I2C device */
    uint16_t i2c_addr;                 /**< I2C address of device */
    uint8_t row_count;                 /**< Number of rows */
    uint8_t col_count;                 /**< Number of columns */
    aip31068_font_size_t font_size;    /**< Font size */
    aip31068_bit_mode_t bit_mode;      /**< Bit mode */
} aip31068_params_t;

/**
 * @brief   AIP31068 PWM device data structure type
 */
typedef struct {
    aip31068_params_t params;        /**< Device initialization parameters */
    uint8_t _curr_display_control;   /**< Current value of DISPLAY_CONTROL */
    uint8_t _curr_entry_mode_set;    /**< Current value of ENTRY_MODE_SET */
} aip31068_t;

/**
 * @brief Initialization.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] params    Parameters for device initialization
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_init(aip31068_t *dev, const aip31068_params_t *params);

/**
 * @brief Turn on the display.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_turn_on(aip31068_t *dev);

/**
 * @brief Turn off the display.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_turn_off(aip31068_t *dev);

/**
 * @brief Clear the display and set the cursor to position (0, 0).
 *
 * @note: Also changes to setTextInsertionMode(LEFT_TO_RIGHT)
 *
 * @param[in] dev   Device descriptor of the AIP31068
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_clear(aip31068_t *dev);

/**
 * @brief Reset cursor position to (0, 0) and scroll display to original position.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_return_home(aip31068_t *dev);

/**
 * @brief Enable or disable automated scrolling.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] enabled   Enable or disable
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_set_auto_scroll_enabled(aip31068_t *dev, bool enabled);

/**
 * @brief Enable or disable cursor blinking.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] enabled   Enable or disable
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_set_cursor_blinking_enabled(aip31068_t *dev, bool enabled);

/**
 * @brief Show or hide the cursor.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] visible   Show or hide
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_set_cursor_visible(aip31068_t *dev, bool visible);

/**
 * @brief Move the cursor to a given position.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 * @param[in] row   Row of the new cursor position (starting at 0)
 * @param[in] col   Column of the new cursor position (starting at 0)
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_set_cursor_position(aip31068_t *dev, uint8_t row, uint8_t col);

/**
 * @brief Set the direction from which the text is inserted, starting from the cursor.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 * @param[in] mode  Insertion mode
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_set_text_insertion_mode(aip31068_t *dev,
                                     aip31068_text_insertion_mode_t mode);

/**
 * @brief Move the cursor one unit to the left.
 *
 * When the cursor passes the 40th character of the first line and a second line
 * is available, the cursor will move to the second line.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_move_cursor_left(aip31068_t *dev);

/**
 * @brief Move the cursor one unit to the right.
 *
 * When the cursor passes the 40th character of the first line and a second line
 * is available, the cursor will move to the second line.
 *
 * @note: The cursor respects the setting for the insertion mode and is set
 *        to (1, 0) for LEFT_TO_RIGHT and to (1, COL_MAX) for RIGHT_TO_LEFT.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_move_cursor_right(aip31068_t *dev);

/**
 * @brief Scroll the entire display content (all lines) one unit to the left.
 *
 * @note: The cursor respects the setting for the insertion mode and is set
 *        to (1, 0) for LEFT_TO_RIGHT and to (1, COL_MAX) for RIGHT_TO_LEFT.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_scroll_display_left(aip31068_t *dev);

/**
 * @brief Scroll the entire display content (all lines) one unit to the right.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_scroll_display_right(aip31068_t *dev);

/**
 * @brief Create a custom symbol.
 *
 * Useful link: https://maxpromer.github.io/LCD-Character-Creator/
 *
 * @param[in] dev           Device descriptor of the AIP31068
 * @param[in] customSymbol  Key to which a custom symbol should be assigned
 * @param[in] charmap       Bitmap definition of the custom symbol
 *
 * @note: The size of charmap depends on how the AIP31068 was initialized.
 *        8 bytes for FONT_SIZE_5x8 and 10 bytes for FONT_SIZE_5x10.
 *
 *        This function resets the cursor position. Therefore this function
 *        should be called before printing any characters to the display.
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_set_custom_symbol(aip31068_t *dev,
                               aip31068_custom_symbol_t customSymbol,
                               const uint8_t charmap[]);

/**
 * @brief Print a custom symbol by key reference.
 *
 * @param[in] dev           Device descriptor of the AIP31068
 * @param[in] customSymbol  Key of the custom symbol to be printed
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_print_custom_symbol(aip31068_t *dev,
                                 aip31068_custom_symbol_t customSymbol);

/**
 * @brief Print a string.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 * @param[in] data  String to be printed (null-terminated)
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_print(aip31068_t *dev, const char *data);

/**
 * @brief Print a single character.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 * @param[in] c     Character to be printed
 *
 * @retval  0 on success
 * @retval  -1 if acquiring of I2C bus fails
 * @retval  -EIO When slave device doesn't ACK the byte
 * @retval  -ENXIO When no devices respond on the address sent on the bus
 * @retval  -ETIMEDOUT When timeout occurs before device's response
 * @retval  -EINVAL When an invalid argument is given
 * @retval  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @retval  -EAGAIN When a lost bus arbitration occurs
 */
int aip31068_print_char(aip31068_t *dev, char c);

#ifdef __cplusplus
}
#endif

/** @} */
