/*
 * Copyright (C) 2020 Hackerspace San Salvador
 *
 * This driver is a port for RIOT-OS based on the Wiegand protocol for
 * Arduino implementation from monkeyboards.org.
 *
 * Original implementation available at:
 *   https://github.com/monkeyboard/Wiegand-Protocol-Library-for-Arduino
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_wiegand
 * @{
 *
 * @file
 * @brief       Driver for the Wiegand Protocol.
 *
 * @author      Mario Gomez <mario.gomez@teubi.co>
 *
 * @}
 */

#include "xtimer.h"
#include "wiegand_params.h"

/**
 * @brief Handler for D0 signal in Wiegand protocol
 *
 */
static void wiegand_read_d0(void *arg)
{
    wiegand_t *dev = (wiegand_t *)arg;

    // Increment bit count for Interrupt connected to D0
    dev->wg_bit_count++;
    // If bit count more than 31, process high bits
    if (dev->wg_bit_count > 31) {
        // Gets most significant bit from the lower-bit buffer
        // and adds to the LSBit of the higher-bit buffer.
        // Then it displaces the bits to the left.
        dev->wg_card_temp_high |= ((0x80000000 & dev->wg_card_temp) >> 31);
        dev->wg_card_temp_high <<= 1;
        dev->wg_card_temp <<= 1;
    }
    else {
        // D0 represent binary 0, so just left shift card data
        dev->wg_card_temp <<= 1;
    }
}

/**
 * @brief Handler for D1 signal in Wiegand protocol
 *
 */
static void wiegand_read_d1(void *arg)
{
    wiegand_t *dev = (wiegand_t *)arg;

    // Increment bit count for Interrupt connected to D1
    dev->wg_bit_count++;

    // If bit count more than 31, process high bits
    if (dev->wg_bit_count > 31) {
        // Gets most significant bit from the lower-bit buffer
        // and adds to the LSBit of the higher-bit buffer.
        // Then it displaces the bits to the left.
        dev->wg_card_temp_high |= ((0x80000000 & dev->wg_card_temp) >> 31);
        dev->wg_card_temp_high <<= 1;
        dev->wg_card_temp |= 1;
        dev->wg_card_temp <<= 1;
    }
    else {
        // D1 represent binary 1, so OR card data with 1 then
        dev->wg_card_temp |= 1;
        // left shift card data
        dev->wg_card_temp <<= 1;
    }
}

/**
 * @brief Returns the last code read from the Wiegand device.
 *
 */
unsigned long wiegand_get_code(wiegand_t *dev)
{
    return dev->code;
}

/**
 * @brief Returns the Wiegand type.
 *
 */
int16_t wiegand_get_type(wiegand_t *dev)
{
    return dev->type;
}

/**
 * @brief Translates * and # keys to [ENTER] and [ESCAPE] keys.
 *
 */
static char translate_enter_escape_key_press(char originalKeyPress)
{
    switch (originalKeyPress) {
        case 0x0b:          // 11 or * key
            return 0x0d;    // 13 or ASCII ENTER

        case 0x0a:          // 10 or # key
            return 0x1b;    // 27 or ASCII ESCAPE

        default:
            return originalKeyPress;
    }
}

/**
 * @brief Disables interrupt handling for D0 and D1 lines.
 *
 */
static inline void wiegand_disable_int(wiegand_t *dev)
{
    gpio_irq_disable(dev->conn->d0);
    gpio_irq_disable(dev->conn->d1);
}

/**
 * @brief Enables interrupt handling for D0 and D1 lines.
 *
 */
static inline void wiegand_enable_int(wiegand_t *dev)
{
    gpio_irq_enable(dev->conn->d0);
    gpio_irq_enable(dev->conn->d1);
}

/**
 * @brief Returns the card identification.
 *
 */
static unsigned long wiegand_get_card_id(volatile unsigned long *codehigh,
                                         volatile unsigned long *codelow,
                                         char bitlength)
{

    if (bitlength == 26) { // EM tag
        return (*codelow & 0x1FFFFFE) >> 1;
    }

    if (bitlength == 34) {              // Mifare
        *codehigh = *codehigh & 0x03;   // only need the 2 LSB of the codehigh
        *codehigh <<= 30;               // shift 2 LSB to MSB
        *codelow >>= 1;
        return *codehigh | *codelow;
    }
    return *codelow; // EM tag or Mifare without parity bits
}

/**
 * @brief Process the temporary Wiegand buffer and decodes the data.
 *
 */
static bool wiegand_do_conversion(wiegand_t *dev)
{
    uint64_t sysTick;

    sysTick = xtimer_now_usec64();

    // Here we check if we don't have recent activity
    if (
        sysTick - dev->wg_last_wiegand > WIEGAND_WAIT &&
        dev->wg_bit_count != 0 &&
        dev->wg_bit_count == dev->last_bit_count) {
        dev->ready = 1;
    }
    dev->wg_last_wiegand = sysTick;
    dev->last_bit_count = dev->wg_bit_count;

    if (dev->ready) {
        // If there is no recent activity
        // bitCount for keypress=4 or 8, Wiegand 26=24 or 26,
        // Wiegand 34=32 or 34
        switch (dev->wg_bit_count) {
            case 4:
            case 8:
            case 24:
            case 26:
            case 32:
            case 34:
                // Shift right 1 bit to get back the real value - interrupt
                // adds 1 left shift in advance
                dev->wg_card_temp >>= 1;

                // An additional right shift is needed if bit-count
                // higher to 32 bits as we also add 1 left shift in
                // advance to the high-bit buffer
                if (dev->wg_bit_count > 32) {
                    dev->wg_card_temp_high >>= 1;
                }

                // Used by the 8-bit decoder
                char highNibble;
                char lowNibble;

                // We process the code based on the bit-length
                switch (dev->wg_bit_count) {
                    case 4:
                        // 4-bit Wiegand codes have no data integrity check so
                        // we justread the LOW nibble.
                        dev->code = (int)translate_enter_escape_key_press(
                            dev->wg_card_temp & 0x0000000F);

                        dev->type = dev->wg_bit_count;
                        dev->wg_bit_count = 0;
                        dev->wg_card_temp = 0;
                        dev->wg_card_temp_high = 0;
                        dev->ready = 0;
                        dev->last_bit_count = 0;

                        return true;
                        break;
                    case 8:
                        // 8-bit Wiegand keyboard data, high nibble is
                        // the "NOT" of low nibble
                        // eg if key 1 pressed, data=E1 in binary 11100001,
                        // high nibble=1110 , low nibble = 0001
                        highNibble = (dev->wg_card_temp & 0xf0) >> 4;
                        lowNibble = (dev->wg_card_temp & 0x0f);
                        dev->type = dev->wg_bit_count;
                        dev->wg_bit_count = 0;
                        dev->wg_card_temp = 0;
                        dev->wg_card_temp_high = 0;

                        // check if low nibble matches the "NOT" of high nibble.
                        if (lowNibble == (~highNibble & 0x0f)) {
                            dev->code =
                                (int)translate_enter_escape_key_press(
                                    lowNibble
                                    );
                            dev->ready = 0;
                            dev->last_bit_count = 0;
                            return true;
                        }
                        else {
                            // Validation fails. Then we reset the
                            // reader
                            dev->wg_last_wiegand = sysTick;
                            dev->wg_bit_count = 0;
                            dev->wg_card_temp = 0;
                            dev->wg_card_temp_high = 0;
                            dev->ready = 0;
                            dev->last_bit_count = 0;
                            return false;
                        }

                        // We never reach this code... Just OCD
                        return false;
                        break;
                    default:
                        // Handles wiegand 16, 32 or 34
                        dev->code =
                            wiegand_get_card_id(&(dev->wg_card_temp_high),
                                                &(dev->wg_card_temp),
                                                dev->wg_bit_count);
                        dev->type = dev->wg_bit_count;
                        dev->wg_bit_count = 0;
                        dev->wg_card_temp = 0;
                        dev->wg_card_temp_high = 0;
                        dev->ready = 0;
                        dev->last_bit_count = 0;
                        return true;
                        break; // We never reach this code... Just OCD
                }
                break;
            default:
                // Well time over 25 ms and bit-count is count is not even
                // then it must be noise or something else. Reset everything.
                dev->wg_last_wiegand = sysTick;
                dev->wg_bit_count = 0;
                dev->wg_card_temp = 0;
                dev->wg_card_temp_high = 0;
                dev->ready = 0;
                dev->last_bit_count = 0;
                return false;
                break; // Not reachable... Just OCD
        }
    }
    else {
        // Return false if the device is not ready.
        return false;
    }
}

/**
 * @brief Tries to process the wiegand data and returns true if successful.
 *
 */
bool wiegand_available(wiegand_t *dev)
{
    bool ret;

    wiegand_disable_int(dev);
    ret = wiegand_do_conversion(dev);
    wiegand_enable_int(dev);

    return ret;
}

/**
 * @brief Initializes the wiegand device.
 *
 */
int16_t wiegand_init(wiegand_t *dev, const wiegand_params_t *params)
{
    dev->code = 0;
    dev->type = 0;
    dev->wg_card_temp_high = 0;
    dev->wg_card_temp = 0;
    dev->wg_last_wiegand = 0;
    dev->wg_bit_count = 0;
    dev->ready = 0;
    dev->last_bit_count = 0;
    dev->conn = params;

    if (gpio_init_int(
            dev->conn->d0,
            GPIO_IN,
            dev->conn->flank,
            wiegand_read_d0,
            (void *)dev
            )) {
        return -1;
    }

    if (gpio_init_int(
            dev->conn->d1,
            GPIO_IN,
            dev->conn->flank,
            wiegand_read_d1,
            (void *)dev
            )) {
        return -1;
    }

    return 0;
}
