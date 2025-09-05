/*
 * SPDX-FileCopyrightText: 2024 Nico Behrens <nifrabe@outlook.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_tm1637
 *
 * @{
 * @file
 * @brief       Driver for the TM1637 4-digit 7-segment display
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 *
 * @}
 */

#include "tm1637.h"

#include "periph/gpio.h"
#include "ztimer.h"
#include "log.h"

/**
 * @brief Amount of digits
 */
#define DIGIT_COUNT                 4

/**
 * @brief Signals data transmission
 */
#define COMMAND_DATA                0x40

/**
 * @brief Sets the brightness and display state to on/off
 */
#define COMMAND_DISPLAY_AND_CONTROL 0x80

/**
 * @brief Sets the address where data is written
 */
#define COMMAND_ADDRESS             0xC0

/**
 * @brief Bit mask for turning the display on/off
 *
 * @note This bit is part of the COMMAND_DISPLAY_AND_CONTROL
 */
#define BIT_MASK_ON                 0x08

/**
 * @brief Bit mask for the dot/colon to the right of a digit
 *
 * @note This bit is part of a digit's segment data
 */
#define BIT_MASK_DOT                0x80

/**
 * @brief Delay between bits in microseconds
 */
#define BIT_TIME_US                 1

/**
 * @brief Array encoding the segments for the digits from 0 to 9
 */
static const uint8_t segments_array[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
};

/**
 * @brief Minus sign for negative numbers
 */
static const uint8_t minus_sign = 0b01000000;

/**
 * @brief Delays the transmission to the display
 */
inline static void _tm1637_delay(void)
{
    ztimer_sleep(ZTIMER_USEC, BIT_TIME_US);
}

/**
 * @brief Starts the transmission to the display
 *
 * @param[in] dev device descriptor of the display
 */
static void _tm1637_start(const tm1637_t *dev)
{
    gpio_write(dev->params->dio, false);
    _tm1637_delay();
}

/**
 * @brief Stops the transmission to the display
 *
 * @note A stop is needed after transmission of certain bytes according to
 * the specification.
 *
 * @param[in] dev device descriptor of the display
 */
static void _tm1637_stop(const tm1637_t *dev)
{
    gpio_write(dev->params->dio, false);
    _tm1637_delay();
    gpio_write(dev->params->clk, true);
    _tm1637_delay();
    gpio_write(dev->params->dio, true);
    _tm1637_delay();
}

/**
 * @brief Transmits a single byte to the display
 *
 * @param[in] dev   device descriptor of the display
 * @param[in] byte  byte to transmit
 *
 * @retval 0        if the transmission was successful
 * @retval -1       if the transmission failed
 */
static int _tm1637_transmit_byte(const tm1637_t *dev, uint8_t byte)
{
    /* transmit each bit */
    for (int i = 0; i < 8; ++i) {
        bool value = (byte >> i) & 0x01;
        gpio_write(dev->params->clk, false);
        _tm1637_delay();
        gpio_write(dev->params->dio, value);
        _tm1637_delay();
        gpio_write(dev->params->clk, true);
        _tm1637_delay();
    }

    gpio_write(dev->params->clk, false);
    gpio_write(dev->params->dio, true);

    /* set the DIO pin to input to later receive the ACK */
    gpio_init(dev->params->dio, GPIO_IN_PU);
    _tm1637_delay();

    gpio_write(dev->params->clk, true);

    /* the transmission is successful if the GPIO reads LOW */
    bool nack = gpio_read(dev->params->dio);
    _tm1637_delay();

    /* set the DIO pin back to output */
    gpio_init(dev->params->dio, GPIO_OUT);
    gpio_write(dev->params->dio, false);
    gpio_write(dev->params->clk, false);
    _tm1637_delay();

    return nack ? -1 : 0;
}

/**
 * @brief Transmits the segments array of length 4 to the display
 *
 * @param[in] dev       device descriptor of the display
 * @param[in] segments  array of length 4 encoding the display's segments
 *
 * @retval 0            if the transmission was successful
 * @retval -1           if the transmission failed
 */
static int _tm1637_transmit_segments(const tm1637_t *dev,
                                     const uint8_t segments[DIGIT_COUNT],
                                     tm1637_brightness_t brightness)
{
    /* transmit the data command first */
    _tm1637_start(dev);
    int res = _tm1637_transmit_byte(dev, COMMAND_DATA);
    if (res < 0) {
        return -1;
    }

    _tm1637_stop(dev);

    /**
     * set the address using the auto increment mode for addresses and
     * start at zero
     */
    _tm1637_start(dev);
    res = _tm1637_transmit_byte(dev, COMMAND_ADDRESS);
    if (res < 0) {
        return -1;
    }

    /* transmit each byte indiviudally */
    for (int i = 0; i < DIGIT_COUNT; ++i) {
        res = _tm1637_transmit_byte(dev, segments[i]);
        if (res < 0) {
            return -1;
        }
    }
    _tm1637_stop(dev);

    /* transmit the display state and the brightness */
    _tm1637_start(dev);
    res = _tm1637_transmit_byte(dev, COMMAND_DISPLAY_AND_CONTROL |
                                         brightness | BIT_MASK_ON);
    if (res < 0) {
        return -1;
    }
    _tm1637_stop(dev);

    return 0;
}

/**
 * @brief Modifies the segments array to enable the colon
 *
 * @param[in,out] segments  segments to enable the colon on
 */
inline static void _enable_colon(uint8_t *segments)
{
    /* the digit to the left of the middle uses the colon */
    segments[DIGIT_COUNT / 2 - 1] |= BIT_MASK_DOT;
}

int tm1637_init(tm1637_t *dev, const tm1637_params_t *params)
{
    assert(params != NULL);
    assert(dev != NULL);
    assert(params->clk != GPIO_UNDEF);
    assert(params->dio != GPIO_UNDEF);

    /* set the parameters */
    dev->params = params;

    if (gpio_init(dev->params->clk, GPIO_OUT)) {
        return -1;
    }

    if (gpio_init(dev->params->dio, GPIO_OUT)) {
        return -1;
    }

    gpio_write(dev->params->clk, false);
    gpio_write(dev->params->dio, false);

    /**
     * This is necessary as the display needs to warm-up to the clock speed.
     * If the following lines are omitted, the display will generate a NACK after the first
     * byte transmission.
     */
    _tm1637_start(dev);
    _tm1637_stop(dev);

    return 0;
}

int tm1637_clear(const tm1637_t *dev)
{
    uint8_t segments[DIGIT_COUNT] = { 0 };
    return _tm1637_transmit_segments(dev, segments, TM1637_PW_1_16);
}

int tm1637_write_number(const tm1637_t *dev, int16_t number,
                        tm1637_brightness_t brightness, bool colon,
                        bool leading_zeros)
{
    /* with only 4 digits available, this range can't be exceeded */
    assert(number <= 9999);
    assert(number >= -999);

    uint8_t segments[DIGIT_COUNT] = { 0 };

    if (number == 0) {
        segments[DIGIT_COUNT - 1] = segments_array[0];
    }
    else if (number < 0) {
        /* for a negative number invert the signedness */
        number = -number;
        for (int i = 0; i < DIGIT_COUNT; ++i) {
            if (number != 0) {
                segments[DIGIT_COUNT - 1 - i] = segments_array[number % 10];
                number /= 10;
            }
            else if (!leading_zeros) {
                /**
                 * without leading zeros, the minus sign is to the left of the most
                 * significant digit
                 */
                segments[DIGIT_COUNT - 1 - i] = minus_sign;
                break;
            }
            else {
                /* with leading zeros, the minus sign is always at the leftmost position */
                segments[0] = minus_sign;
                break;
            }
        }
    }
    else {
        for (int i = 0; i < DIGIT_COUNT; ++i) {
            if (number != 0) {
                segments[DIGIT_COUNT - 1 - i] = segments_array[number % 10];
                number /= 10;
            }
        }
    }

    /* fill out all segments that have not yet been filled with zeros */
    if (leading_zeros) {
        for (int i = 0; i < DIGIT_COUNT; ++i) {
            if (segments[i] == 0) {
                segments[i] = segments_array[0];
            }
        }
    }

    if (colon) {
        _enable_colon(segments);
    }

    return _tm1637_transmit_segments(dev, segments, brightness);
}
