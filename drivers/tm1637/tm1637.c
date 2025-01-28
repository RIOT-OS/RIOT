/*
 * Copyright (C) 2024 Nico Behrens <nifrabe@outlook.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

/**
 * @brief Amount of digits
 */
#define DIGIT_COUNT                 4

/**
 * @brief Signals data transmission
 */
#define DATA_COMMAND                0x40

/**
 * @brief Sets the brightness and display state to on/off
 */
#define DISPLAY_AND_CONTROL_COMMAND 0x80

/**
 * @brief Sets the address where data is written
 */
#define ADDRESS_COMMAND             0xC0

/**
 * @brief Bit mask for turning the display on/off
 *
 * @note This bit is part of the DISPLAY_AND_CONTROL_COMMAND
 */
#define ON_BIT_MASK                 0x08

/**
 * @brief Bit mask for the dot/colon to the right of a digit
 *
 * @note This bit is part of a digit's segment data
 */
#define DOT_BIT_MASK                0x80

/**
 * @brief Delay between bits in milliseconds
 */
#define BIT_TIME_MS                 1

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
inline static void delay(void)
{
    ztimer_sleep(ZTIMER_MSEC, BIT_TIME_MS);
}

/**
 * @brief Starts the transmission to the display
 *
 * @param[in] dev device descriptor of the display
 */
static void start(const tm1637_t *dev)
{
    gpio_write(dev->params.dio, false);
    delay();
}

/**
 * @brief Stops the transmission to the display
 *
 * @note A stop is needed after transmission of certain bytes according to
 * the specification.
 *
 * @param[in] dev device descriptor of the display
 */
static void stop(const tm1637_t *dev)
{
    gpio_write(dev->params.dio, false);
    delay();
    gpio_write(dev->params.clk, true);
    delay();
    gpio_write(dev->params.dio, true);
    delay();
}

/**
 * @brief Transmits a single byte to the display
 *
 * @param[in] dev   device descriptor of the display
 * @param[in] byte  byte to transmit
 */
static void transmit_byte(const tm1637_t *dev, uint8_t byte)
{
    for (int i = 0; i < 8; ++i) {
        bool value = (byte >> i) & 0x01;
        gpio_write(dev->params.clk, false);
        delay();
        gpio_write(dev->params.dio, value);
        delay();
        gpio_write(dev->params.clk, true);
        delay();
    }

    /**
     * We do not read the ACK as it is not necessary for the display's functionality.
     * According to the specification the display sends an ACK via the DIO pin but
     * this would require us to set the pin into input mode and read the value, then set it
     * back to write mode. This would take too long and therefore the ACK check is omitted.
     */
    gpio_write(dev->params.clk, false);
    gpio_write(dev->params.dio, true);
    delay();
    gpio_write(dev->params.clk, true);
    delay();
    gpio_write(dev->params.clk, false);
    delay();
}

/**
 * @brief Transmits the segments array of length 4 to the display
 *
 * @param[in] dev       device descriptor of the display
 * @param[in] segments  array of length 4 encoding the display's segments
 */
static void transmit_segments(const tm1637_t *dev,
                              const uint8_t *segments,
                              tm1637_brightness_t brightness)
{
    /* transmit the data command first */
    start(dev);
    transmit_byte(dev, DATA_COMMAND);
    stop(dev);

    /**
     * set the address using the auto increment mode for addresses and
     * start at zero
     */
    start(dev);
    transmit_byte(dev, ADDRESS_COMMAND);

    /* transmit each bit indiviudally */
    for (int i = 0; i < DIGIT_COUNT; ++i) {
        transmit_byte(dev, segments[i]);
    }
    stop(dev);

    /* transmit the display state and the brightness */
    start(dev);
    transmit_byte(dev, DISPLAY_AND_CONTROL_COMMAND |
                           brightness | ON_BIT_MASK);

    stop(dev);
}

/**
 * @brief Modifies the segments array to enable the colon
 *
 * @param[in,out] segments  segments to enable the colon on
 */
inline static void enable_colon(uint8_t *segments)
{
    /* the second digit uses the colon */
    segments[1] |= DOT_BIT_MASK;
}

int tm1637_init(tm1637_t *dev, const tm1637_params_t *params)
{
    assert(params != NULL);
    assert(dev != NULL);
    assert(params->clk != GPIO_UNDEF);
    assert(params->dio != GPIO_UNDEF);

    /* set the parameters */
    dev->params = *params;

    if (gpio_init(dev->params.clk, GPIO_OUT)) {
        return -1;
    }

    if (gpio_init(dev->params.dio, GPIO_OUT)) {
        return -1;
    }

    gpio_write(dev->params.clk, false);
    gpio_write(dev->params.dio, false);

    return 0;
}

void tm1637_clear(const tm1637_t *dev)
{
    uint8_t segments[DIGIT_COUNT] = { 0 };
    transmit_segments(dev, segments, TM1637_PW_1_16);
}

void tm1637_write_number(const tm1637_t *dev, int16_t number,
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
        number = -number;
        for (int i = 0; i < DIGIT_COUNT; ++i) {
            if (number != 0) {
                segments[DIGIT_COUNT - 1 - i] = segments_array[number % 10];
                number /= 10;
            }
            else if (!leading_zeros) {
                segments[DIGIT_COUNT - 1 - i] = minus_sign;
                break;
            }
            else {
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
        enable_colon(segments);
    }

    transmit_segments(dev, segments, brightness);
}
