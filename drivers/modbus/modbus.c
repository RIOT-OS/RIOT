/*
 * Copyright (C) 2020-2023 Denis Litvinov <li.denis.iv@gmail.com>
 * Copyright (C) 2023-2024 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_modbus
 * @brief       Modbus
 *
 * @{
 * @file
 * @brief       Modbus interface implementation
 *
 * @author      Denis Litvinov <li.denis.iv@gmail.com>
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 * @}
 */

#include <stdlib.h>
#include <string.h>

#include "modbus.h"

inline uint16_t modbus_bit_count_to_size(uint16_t count)
{
    div_t d = div(count, 8);

    if (d.rem) {
        return d.quot + 1;
    }
    return d.quot;
}

inline uint16_t modbus_reg_count_to_size(uint16_t count)
{
    return count * MODBUS_REGISTER_SIZE;
}

void modbus_copy_bits(uint8_t *dst, uint16_t start_bit_dst,
                      const uint8_t *src, uint16_t start_bit_src,
                      uint16_t count, bool pad_zeroes)
{
    if (pad_zeroes) {
        div_t a = div(start_bit_dst, 8);
        div_t b = div(start_bit_dst + count, 8);

        memset(&(dst[a.quot]), 0, b.quot - a.quot + 1);
    }

    while (count-- > 0) {
        div_t s = div(start_bit_src++, 8);
        div_t d = div(start_bit_dst++, 8);
        uint8_t bit = src[s.quot] & (1 << s.rem);
        if (bit) {
            dst[d.quot] |= 1 << (d.rem);
        }
        else {
            dst[d.quot] &= ~(1 << (d.rem));
        }
    }
}

inline void modbus_copy_bit(uint8_t *dst, uint16_t start_bit_dst,
                            const uint8_t *src, uint16_t start_bit_src,
                            bool pad_zeroes)
{
    modbus_copy_bits(dst, start_bit_dst, src, start_bit_src, 1, pad_zeroes);
}

inline void modbus_copy_regs(void *dst, const void *src, uint16_t count)
{
    memcpy(dst, src, modbus_reg_count_to_size(count));
}

inline void modbus_copy_reg(void *dst, const void *src)
{
    memcpy(dst, src, modbus_reg_count_to_size(1));
}

int modbus_check_message(const modbus_message_t *message)
{
    uint16_t size;

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
        if (message->count < 1 || message->count > 2000) {
            return MODBUS_ERR_MESSAGE;
        }
        if (message->data == NULL) {
            return MODBUS_ERR_MESSAGE;
        }
        size = modbus_bit_count_to_size(message->count);
        if (message->data_size < size) {
            return MODBUS_ERR_MESSAGE;
        }
        break;

    case MODBUS_FC_READ_DISCRETE_INPUTS:
        if (message->count < 1 || message->count > 2000) {
            return MODBUS_ERR_MESSAGE;
        }
        if (message->data == NULL) {
            return MODBUS_ERR_MESSAGE;
        }
        size = modbus_bit_count_to_size(message->count);
        if (message->data_size < size) {
            return MODBUS_ERR_MESSAGE;
        }
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
        if (message->count < 1 || message->count > 1968) {
            return MODBUS_ERR_MESSAGE;
        }
        size = modbus_bit_count_to_size(message->count);
        if (message->data_size < size) {
            return MODBUS_ERR_MESSAGE;
        }
        break;

    case MODBUS_FC_READ_INPUT_REGISTERS:
        if (message->count < 1 || message->count > 125) {
            return MODBUS_ERR_MESSAGE;
        }
        if (message->data == NULL) {
            return MODBUS_ERR_MESSAGE;
        }
        size = modbus_reg_count_to_size(message->count);
        if (message->data_size < size) {
            return MODBUS_ERR_MESSAGE;
        }
        break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
        if (message->count < 1 || message->count > 125) {
            return MODBUS_ERR_MESSAGE;
        }
        if (message->data == NULL) {
            return MODBUS_ERR_MESSAGE;
        }
        size = modbus_reg_count_to_size(message->count);
        if (message->data_size < size) {
            return MODBUS_ERR_MESSAGE;
        }
        break;

    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        if (message->count < 1 || message->count > 125) {
            return MODBUS_ERR_MESSAGE;
        }
        if (message->data == NULL) {
            return MODBUS_ERR_MESSAGE;
        }
        size = modbus_reg_count_to_size(message->count);
        if (message->data_size < size) {
            return MODBUS_ERR_MESSAGE;
        }
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
        if (message->data == NULL) {
            return MODBUS_ERR_MESSAGE;
        }
        if (message->data_size < 1) {
            return MODBUS_ERR_MESSAGE;
        }
        /* data must be 0xFF00 or 0x0000 */
        if (!(message->data[0] == 0x00 || message->data[0] == 0xFF) && message->data[1] != 0x00) {
            return MODBUS_ERR_MESSAGE;
        }
        break;

    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        if (message->data == NULL) {
            return MODBUS_ERR_MESSAGE;
        }
        size = modbus_reg_count_to_size(1);
        if (message->data_size < size) {
            return MODBUS_ERR_MESSAGE;
        }
        break;

    default:
        return MODBUS_ERR_MESSAGE;
        break;
    }

    return MODBUS_OK;
}
