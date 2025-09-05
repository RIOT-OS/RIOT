/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_aip31068
 * @brief       Device driver for AIP31068
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */

#include <assert.h>
#include <stdio.h>

#include "xtimer.h"

#include "aip31068.h"
#include "aip31068_regs.h"
#include "aip31068_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Write a data byte to the device.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] value     Data byte to write
 *
 * @return  0 on success
 * @return  -1 if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static inline int _data(aip31068_t *dev, uint8_t value);

/**
 * @brief Write a command byte with it's arguments (modified bits) to the device.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] value     Command byte to write
 *
 * @return  0 on success
 * @return  -1 if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static inline int _command(aip31068_t *dev, uint8_t value);

/**
 * @brief Write a command or data byte to the device.
 *
 * @param[in] dev       Device descriptor of the AIP31068
 * @param[in] data_byte Byte to write
 * @param[in] is_cmd    Whether byte should be interpreted as data or command
 *
 * @return  0 on success
 * @return  -1 if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static inline int _write(aip31068_t *dev, uint8_t data_byte, bool is_cmd);

/**
 * @brief Write data to the device.
 *
 * @param[in] dev   Device descriptor of the AIP31068
 * @param[in] data  Data to write
 * @param[in] len   Length of the data
 *
 * @return  0 on success
 * @return  -1 if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
static int _device_write(aip31068_t *dev, uint8_t *data, uint8_t len);

int aip31068_init(aip31068_t *dev, const aip31068_params_t *params)
{
    assert(dev);
    assert(params);

    /* displays with more than 4 lines are not supported
     * (see aip31068_set_cursor_position) */
    assert(params->row_count <= 4);

    dev->params = *params;
    dev->_curr_display_control = 0;
    dev->_curr_entry_mode_set = 0;

    uint8_t _function_set = 0;

    /* configure bit mode */
    if (params->bit_mode == BITMODE_8_BIT) {
        SETBIT(_function_set, AIP31068_BIT_FUNCTION_SET_BITMODE);
    }

    /* configure line count */
    if (params->row_count >= 2) {
        SETBIT(_function_set, AIP31068_BIT_FUNCTION_SET_LINECOUNT);
    }

    /* configure character size */
    if (params->font_size == FONT_SIZE_5x10) {
        SETBIT(_function_set, AIP31068_BIT_FUNCTION_SET_FONTSIZE);
    }

    /* Begin of the initialization sequence (page 20 in the datasheet).
     * The initialization sequence here is writing AIP31068_CMD_FUNCTION_SET
     * three times with different sleep periods in between according to the
     * data sheet of the HD44780. The AIP31068 is forwarding commands to the
     * HD44780 hidden behind it so to be sure that it works for all kinds of
     * HD44780 we follow the initialization sequence of the HD44780, even though
     * it might be unnecessary for others. */
    xtimer_msleep(50);

    int rc = 0;

    /* send function set command sequence */
    rc = _command(dev, AIP31068_CMD_FUNCTION_SET | _function_set);
    if (rc < 0) {
        return rc;
    }

    /* wait after the first try */
    xtimer_msleep(5);

    /* second try */
    rc = _command(dev, AIP31068_CMD_FUNCTION_SET | _function_set);
    if (rc < 0) {
        return rc;
    }

    /* wait after the second try */
    xtimer_usleep(500);

    /* third go */
    rc = _command(dev, AIP31068_CMD_FUNCTION_SET | _function_set);
    if (rc < 0) {
        return rc;
    }

    rc = aip31068_turn_off(dev);
    if (rc < 0) {
        return rc;
    }

    rc = aip31068_clear(dev);
    if (rc < 0) {
        return rc;
    }

    rc = aip31068_set_text_insertion_mode(dev, LEFT_TO_RIGHT);
    if (rc < 0) {
        return rc;
    }

    /* End of the initialization sequence. */

    return 0;
}

int aip31068_turn_on(aip31068_t *dev)
{
    SETBIT(dev->_curr_display_control, AIP31068_BIT_DISPLAY_CONTROL_DISPLAY);

    return _command(dev, AIP31068_CMD_DISPLAY_CONTROL | dev->_curr_display_control);
}

int aip31068_turn_off(aip31068_t *dev)
{
    CLRBIT(dev->_curr_display_control, AIP31068_BIT_DISPLAY_CONTROL_DISPLAY);

    return _command(dev, AIP31068_CMD_DISPLAY_CONTROL | dev->_curr_display_control);
}

int aip31068_clear(aip31068_t *dev)
{
    int rc = _command(dev, AIP31068_CMD_CLEAR_DISPLAY);

    xtimer_usleep(AIP31068_EXECUTION_TIME_MAX);

    return rc;
}

int aip31068_return_home(aip31068_t *dev)
{
    int rc = _command(dev, AIP31068_CMD_RETURN_HOME);

    xtimer_usleep(AIP31068_EXECUTION_TIME_MAX);

    return rc;
}

int aip31068_set_auto_scroll_enabled(aip31068_t *dev, bool enabled)
{
    if (enabled) {
        SETBIT(dev->_curr_entry_mode_set, AIP31068_BIT_ENTRY_MODE_AUTOINCREMENT);
    }
    else {
        CLRBIT(dev->_curr_entry_mode_set, AIP31068_BIT_ENTRY_MODE_AUTOINCREMENT);
    }

    return _command(dev, AIP31068_CMD_ENTRY_MODE_SET | dev->_curr_entry_mode_set);
}

int aip31068_set_cursor_blinking_enabled(aip31068_t *dev, bool enabled)
{
    if (enabled) {
        SETBIT(dev->_curr_display_control, AIP31068_BIT_DISPLAY_CONTROL_CURSOR_BLINKING);
    }
    else {
        CLRBIT(dev->_curr_display_control, AIP31068_BIT_DISPLAY_CONTROL_CURSOR_BLINKING);
    }

    return _command(dev, AIP31068_CMD_DISPLAY_CONTROL | dev->_curr_display_control);
}

int aip31068_set_cursor_visible(aip31068_t *dev, bool visible)
{
    if (visible) {
        SETBIT(dev->_curr_display_control, AIP31068_BIT_DISPLAY_CONTROL_CURSOR);
    }
    else {
        CLRBIT(dev->_curr_display_control, AIP31068_BIT_DISPLAY_CONTROL_CURSOR);
    }

    return _command(dev, AIP31068_CMD_DISPLAY_CONTROL | dev->_curr_display_control);
}

int aip31068_set_cursor_position(aip31068_t *dev, uint8_t row, uint8_t col)
{
    uint8_t row_offsets[4];
    row_offsets[0] = 0x00;
    row_offsets[1] = 0x40;
    row_offsets[2] = 0x00 + dev->params.col_count;
    row_offsets[3] = 0x40 + dev->params.col_count;

    if (row >= dev->params.row_count) {
        row = dev->params.row_count - 1;
    }

    return _command(dev, AIP31068_CMD_SET_DDRAM_ADDR | (col | row_offsets[row]));
}

int aip31068_set_text_insertion_mode(aip31068_t *dev,
                                     aip31068_text_insertion_mode_t mode)
{
    if (mode == LEFT_TO_RIGHT) {
        SETBIT(dev->_curr_entry_mode_set, AIP31068_BIT_ENTRY_MODE_INCREMENT);
    }
    else {
        CLRBIT(dev->_curr_entry_mode_set, AIP31068_BIT_ENTRY_MODE_INCREMENT);
    }

    return _command(dev, AIP31068_CMD_ENTRY_MODE_SET | dev->_curr_entry_mode_set);
}

int aip31068_move_cursor_left(aip31068_t *dev)
{
    uint8_t cmd = AIP31068_CMD_CURSOR_DISPLAY_SHIFT;
    CLRBIT(cmd, AIP31068_BIT_CURSOR_DISPLAY_SHIFT_DIRECTION);

    return _command(dev, cmd);
}

int aip31068_move_cursor_right(aip31068_t *dev)
{
    uint8_t cmd = AIP31068_CMD_CURSOR_DISPLAY_SHIFT;
    SETBIT(cmd, AIP31068_BIT_CURSOR_DISPLAY_SHIFT_DIRECTION);

    return _command(dev, cmd);
}

int aip31068_scroll_display_left(aip31068_t *dev)
{
    uint8_t cmd = AIP31068_CMD_CURSOR_DISPLAY_SHIFT;
    SETBIT(cmd, AIP31068_BIT_CURSOR_DISPLAY_SHIFT_SELECTION);
    CLRBIT(cmd, AIP31068_BIT_CURSOR_DISPLAY_SHIFT_DIRECTION);

    return _command(dev, cmd);
}

int aip31068_scroll_display_right(aip31068_t *dev)
{
    uint8_t cmd = AIP31068_CMD_CURSOR_DISPLAY_SHIFT;
    SETBIT(cmd, AIP31068_BIT_CURSOR_DISPLAY_SHIFT_SELECTION);
    SETBIT(cmd, AIP31068_BIT_CURSOR_DISPLAY_SHIFT_DIRECTION);

    return _command(dev, cmd);
}

int aip31068_set_custom_symbol(aip31068_t *dev,
                               aip31068_custom_symbol_t custom_symbol,
                               const uint8_t charmap[])
{
    /* Bits 0-2 define the row address of a custom character in CGRAM.
     * Bits 3-5 define the base address of a custom character in CGRAM. */
    uint8_t location = custom_symbol << 3;
    int rc = _command(dev, AIP31068_CMD_SET_CGRAM_ADDR | location);

    if (rc < 0) {
        return rc;
    }

    /* How many rows are necessary for a complete character for given font? */
    int row_count = dev->params.font_size == FONT_SIZE_5x8 ? 8 : 10;

    for (int i = 0; i < row_count; i++) {
        rc = _data(dev, charmap[i]);

        if (rc < 0) {
            return rc;
        }
    }

    return _command(dev, AIP31068_CMD_SET_DDRAM_ADDR);
}

int aip31068_print_custom_symbol(aip31068_t *dev,
                                 aip31068_custom_symbol_t custom_symbol)
{
    return _data(dev, (uint8_t) custom_symbol);
}

int aip31068_print(aip31068_t *dev, const char *data)
{
    while (*data != '\0') {
        int rc;
        if ((rc = _data(dev, *data)) != 0) {
            return rc;
        }
        data++;
    }

    return 0;
}

int aip31068_print_char(aip31068_t *dev, char c)
{
    return _data(dev, c);
}

static inline int _data(aip31068_t *dev, uint8_t value)
{
    int rc = _write(dev, value, false);

    xtimer_usleep(AIP31068_EXECUTION_TIME_DEFAULT);

    return rc;
}

static inline int _command(aip31068_t *dev, uint8_t value)
{
    int rc = _write(dev, value, true);

    xtimer_usleep(AIP31068_EXECUTION_TIME_DEFAULT);

    return rc;
}

static inline int _write(aip31068_t *dev, uint8_t data_byte, bool is_cmd)
{
    uint8_t control_byte = 0;
    if (!is_cmd) {
        SETBIT(control_byte, AIP31068_BIT_CONTROL_BYTE_RS);
    }

    uint8_t data[] = { control_byte, data_byte };

    return _device_write(dev, data, sizeof(data));
}

static int _device_write(aip31068_t* dev, uint8_t *data, uint8_t len)
{
    i2c_t i2c_dev = dev->params.i2c_dev;

    i2c_acquire(i2c_dev);

    int rc = i2c_write_bytes(i2c_dev, dev->params.i2c_addr, data, len, 0);

    i2c_release(i2c_dev);

    return rc;
}
