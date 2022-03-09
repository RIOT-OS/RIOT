/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hd44780
 *
 * @{
 * @file
 * @brief       Driver for the HD44780 LCD
 *
 * @note        The display is also known as LCM1602C from Arduino kits
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <string.h>

#include "log.h"
#include "periph/gpio.h"
#include "xtimer.h"

#include "hd44780.h"
#include "hd44780_internal.h"

static inline void _command(const hd44780_t *dev, uint8_t value);
static void _pulse(const hd44780_t *dev);
static void _send(const hd44780_t *dev, uint8_t value, hd44780_state_t state);
static void _write_bits(const hd44780_t *dev, uint8_t bits, uint8_t value);

/**
 * @brief   Send a command to the display
 *
 * @param[in]  dev          device descriptor of display to initialize
 * @param[in]  value        the command
 */
static inline void _command(const hd44780_t *dev, uint8_t value)
{
    _send(dev, value, HD44780_OFF);
}

/**
 * @brief   Send a pulse to the display to enable new state
 *
 * @param[in]  dev          device descriptor of display to initialize
 */
static void _pulse(const hd44780_t *dev)
{
    gpio_clear(dev->p.enable);
    xtimer_usleep(HD44780_PULSE_WAIT_SHORT);
    gpio_set(dev->p.enable);
    xtimer_usleep(HD44780_PULSE_WAIT_SHORT);
    gpio_clear(dev->p.enable);
    xtimer_usleep(HD44780_PULSE_WAIT_LONG);
}

/**
 * @brief   Send a data value to the display
 *
 * @param[in]  dev          device descriptor of display to initialize
 * @param[in]  value        the data value, either char or command
 * @param[in]  state        send state, to distinguish chars and commands
 */
static void _send(const hd44780_t *dev, uint8_t value, hd44780_state_t state)
{
    (state == HD44780_ON) ? gpio_set(dev->p.rs) : gpio_clear(dev->p.rs);
    /* if RW pin is available, set it to LOW */
    if (gpio_is_valid(dev->p.rw)) {
        gpio_clear(dev->p.rw);
    }
    /* write data in 8Bit or 4Bit mode */
    if (dev->flag & HD44780_8BITMODE) {
        _write_bits(dev, 8, value);
    }
    else {
        _write_bits(dev, 4, value>>4);
        _write_bits(dev, 4, value);
    }
}

static void _write_bits(const hd44780_t *dev, uint8_t bits, uint8_t value)
{
    for (unsigned i = 0; i < bits; ++i) {
        if ((value >> i) & 0x01) {
            gpio_set(dev->p.data[i]);
        }
        else {
            gpio_clear(dev->p.data[i]);
        }
    }
    _pulse(dev);
}

int hd44780_init(hd44780_t *dev, const hd44780_params_t *params)
{
    /* write config params to device descriptor */
    dev->p = *params;
    /* verify cols and rows */
    if ((dev->p.cols > HD44780_MAX_COLS) || (dev->p.rows > HD44780_MAX_ROWS)
                                         || (dev->p.rows * dev->p.cols > 80)) {
        LOG_ERROR("hd44780_init: invalid LCD size!\n");
        return -1;
    }
    dev->flag = 0;
    /* set mode depending on configured pins */
    if (gpio_is_valid(dev->p.data[4])) {
        dev->flag |= HD44780_8BITMODE;
    }
    else {
        dev->flag |= HD44780_4BITMODE;
    }
    /* set flag for 1 or 2 row mode, 4 rows are 2 rows split half */
    if (dev->p.rows > 1) {
        dev->flag |= HD44780_2LINE;
    }
    else {
        dev->flag |= HD44780_1LINE;
    }
    /* set char size to 5x8 as default, 5x10 is hardly used by LCDs anyway */
    dev->flag |= HD44780_5x8DOTS;
    /* calc and set row offsets, depending on number of columns */
    dev->roff[0] = 0x00;
    dev->roff[1] = 0x40;
    dev->roff[2] = 0x00 + dev->p.cols;
    dev->roff[3] = 0x40 + dev->p.cols;

    gpio_init(dev->p.rs, GPIO_OUT);
    /* RW (read/write) of LCD not required, set it to GPIO_UNDEF */
    if (gpio_is_valid(dev->p.rw)) {
        gpio_init(dev->p.rw, GPIO_OUT);
    }
    gpio_init(dev->p.enable, GPIO_OUT);
    /* configure all data pins as output */
    for (int i = 0; i < ((dev->flag & HD44780_8BITMODE) ? 8 : 4); ++i) {
        gpio_init(dev->p.data[i], GPIO_OUT);
    }
    /* see hitachi HD44780 datasheet pages 45/46 for init specs */
    xtimer_usleep(HD44780_INIT_WAIT_XXL);
    gpio_clear(dev->p.rs);
    gpio_clear(dev->p.enable);
    if (gpio_is_valid(dev->p.rw)) {
        gpio_clear(dev->p.rw);
    }
    /* put the LCD into 4 bit or 8 bit mode */
    if (!(dev->flag & HD44780_8BITMODE)) {
        /* see hitachi HD44780 datasheet figure 24, pg 46 */
        _write_bits(dev, 4, 0x03);
        xtimer_usleep(HD44780_INIT_WAIT_LONG);

        _write_bits(dev, 4, 0x03);
        xtimer_usleep(HD44780_INIT_WAIT_LONG);

        _write_bits(dev, 4, 0x03);
        xtimer_usleep(HD44780_INIT_WAIT_SHORT);

        _write_bits(dev, 4, 0x02);
    } else {
        /* see hitachi HD44780 datasheet page 45 figure 23 */
        _command(dev, HD44780_FUNCTIONSET | dev->flag);
        xtimer_usleep(HD44780_INIT_WAIT_LONG);  /* wait more than 4.1ms */

        _command(dev, HD44780_FUNCTIONSET | dev->flag);
        xtimer_usleep(HD44780_INIT_WAIT_SHORT);

        _command(dev, HD44780_FUNCTIONSET | dev->flag);
    }
    _command(dev, HD44780_FUNCTIONSET | dev->flag);

    /* turn the display on with no cursor or blinking default, and clear */
    dev->ctrl = HD44780_DISPLAYON | HD44780_CURSOROFF | HD44780_BLINKOFF;
    hd44780_display(dev, HD44780_ON);
    hd44780_clear(dev);
    /* Initialize to default text direction for western languages */
    dev->mode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
    _command(dev, HD44780_ENTRYMODESET | dev->mode);

    return 0;
}

void hd44780_clear(const hd44780_t *dev)
{
    _command(dev, HD44780_CLEARDISPLAY);
    xtimer_usleep(HD44780_CMD_WAIT);
}

void hd44780_home(const hd44780_t *dev)
{
    _command(dev, HD44780_RETURNHOME);
    xtimer_usleep(HD44780_CMD_WAIT);
}

void hd44780_set_cursor(const hd44780_t *dev, uint8_t col, uint8_t row)
{
    if (row >= dev->p.rows) {
        row = dev->p.rows - 1;
    }
    _command(dev, HD44780_SETDDRAMADDR | (col + dev->roff[row]));
}

void hd44780_display(hd44780_t *dev, hd44780_state_t state)
{
    if (state == HD44780_ON) {
        dev->ctrl |= HD44780_DISPLAYON;
    }
    else {
        dev->ctrl &= ~HD44780_DISPLAYON;
    }
    _command(dev, HD44780_DISPLAYCONTROL | dev->ctrl);
}

void hd44780_cursor(hd44780_t *dev, hd44780_state_t state)
{
    if (state == HD44780_ON) {
        dev->ctrl |= HD44780_CURSORON;
    }
    else {
        dev->ctrl &= ~HD44780_CURSORON;
    }
    _command(dev, HD44780_DISPLAYCONTROL | dev->ctrl);
}

void hd44780_blink(hd44780_t *dev, hd44780_state_t state)
{
    if (state == HD44780_ON) {
        dev->ctrl |= HD44780_BLINKON;
    }
    else {
        dev->ctrl &= ~HD44780_BLINKON;
    }
    _command(dev, HD44780_DISPLAYCONTROL | dev->ctrl);
}

void hd44780_scroll_left(const hd44780_t *dev)
{
    _command(dev, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void hd44780_scroll_right(const hd44780_t *dev) {
    _command(dev, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void hd44780_left2right(hd44780_t *dev)
{
    dev->mode |= HD44780_ENTRYLEFT;
    _command(dev, HD44780_ENTRYMODESET | dev->mode);
}

void hd44780_right2left(hd44780_t *dev)
{
    dev->mode &= ~HD44780_ENTRYLEFT;
    _command(dev, HD44780_ENTRYMODESET | dev->mode);
}

void hd44780_autoscroll(hd44780_t *dev, hd44780_state_t state)
{
    if (state == HD44780_ON) {
        dev->mode |= HD44780_ENTRYSHIFTINCREMENT;
    }
    else {
        dev->mode &= ~HD44780_ENTRYSHIFTINCREMENT;
    }
    _command(dev, HD44780_ENTRYMODESET | dev->mode);
}

void hd44780_create_char(const hd44780_t *dev, uint8_t location, uint8_t charmap[])
{
    location &= 0x7; /* 8 locations (0-7) possible */
    _command(dev, HD44780_SETCGRAMADDR | (location << 3));
    for (unsigned i = 0; i < HD44780_CGRAM_SIZE; ++i) {
        hd44780_write(dev, charmap[i]);
    }
}

void hd44780_write(const hd44780_t *dev, uint8_t value)
{
    _send(dev, value, HD44780_ON);
}

void hd44780_print(const hd44780_t *dev, const char *data )
{
    while (*data != '\0') {
        hd44780_write(dev, *data++);
    }
}
