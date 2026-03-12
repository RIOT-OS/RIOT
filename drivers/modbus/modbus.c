/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_modbus
 * @brief       Modbus
 *
 * @{
 * @file
 * @brief       Modbus interface implementation
 *
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 * @}
 */

#include <stdlib.h>
#include <string.h>

#include "modbus.h"

#define ENABLE_DEBUG 0
#include "debug.h"

size_t modbus_bit_count_to_size(uint16_t count)
{
    div_t d = div(count, 8);

    if (d.rem) {
        return d.quot + 1;
    }

    return d.quot;
}

size_t modbus_reg_count_to_size(uint16_t count)
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

void modbus_copy_bit(uint8_t *dst, uint16_t start_bit_dst,
                     const uint8_t *src, uint16_t start_bit_src,
                     bool pad_zeroes)
{
    modbus_copy_bits(dst, start_bit_dst, src, start_bit_src, 1, pad_zeroes);
}

void modbus_copy_regs(void *dst, const void *src, const uint16_t count)
{
    memcpy(dst, src, modbus_reg_count_to_size(count));
}

void modbus_copy_reg(void *dst, const void *src)
{
    memcpy(dst, src, modbus_reg_count_to_size(1));
}

int modbus_check_message(const modbus_message_t *message)
{
    size_t size;

    if (message->id > MODBUS_ID_MAX) {
        DEBUG("[modbus] modbus_check_message: invalid message id %" PRIu8 "\n", message->id);
        return -EINVAL;
    }

    if ((message->func & ~0x80) != message->func) {
        DEBUG("[modbus] modbus_check_message: exception bit set in function code %u\n",
              message->func);
        return -EINVAL;
    }

    if (message->exc != MODBUS_EXC_NONE) {
        if (message->id == MODBUS_ID_BROADCAST) {
            DEBUG_PUTS("[modbus] modbus_check_message: broadcast message has an exception set");
            return -EINVAL;
        }

        return 0;
    }

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
        if (message->count < 1 || message->count > 2000) {
            DEBUG("[modbus] modbus_check_message: invalid read coils count %" PRIu16 "\n", message->count);
            return -EINVAL;
        }
        if (message->data == NULL) {
            DEBUG_PUTS("[modbus] modbus_check_message: read coils data is NULL");
            return -EINVAL;
        }
        size = modbus_bit_count_to_size(message->count);
        if (message->data_size < size) {
            DEBUG("[modbus] modbus_check_message: read coils data size %" PRIuSIZE " is too small, need %" PRIuSIZE "\n",
                  message->data_size, size);
            return -EINVAL;
        }
        break;

    case MODBUS_FC_READ_DISCRETE_INPUTS:
        if (message->count < 1 || message->count > 2000) {
            DEBUG("[modbus] modbus_check_message: invalid read discrete inputs count %" PRIu16 "\n",
                  message->count);
            return -EINVAL;
        }
        if (message->data == NULL) {
            DEBUG_PUTS("[modbus] modbus_check_message: read discrete inputs data is NULL");
            return -EINVAL;
        }
        size = modbus_bit_count_to_size(message->count);
        if (message->data_size < size) {
            DEBUG(
                "[modbus] modbus_check_message: read discrete inputs data size %" PRIuSIZE " is too small, need %" PRIuSIZE "\n",
                message->data_size, size);
            return -EINVAL;
        }
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
        if (message->count < 1 || message->count > 1968) {
            DEBUG("[modbus] modbus_check_message: invalid write multiple coils count %" PRIu16 "\n",
                  message->count);
            return -EINVAL;
        }
        size = modbus_bit_count_to_size(message->count);
        if (message->data_size < size) {
            DEBUG(
                "[modbus] modbus_check_message: write multiple coils data size %" PRIuSIZE " is too small, need %" PRIuSIZE "\n",
                message->data_size, size);
            return -EINVAL;
        }
        break;

    case MODBUS_FC_READ_INPUT_REGISTERS:
        if (message->count < 1 || message->count > 125) {
            DEBUG("[modbus] modbus_check_message: invalid read input registers count %" PRIu16 "\n",
                  message->count);
            return -EINVAL;
        }
        if (message->data == NULL) {
            DEBUG_PUTS("[modbus] modbus_check_message: read input registers data is NULL");
            return -EINVAL;
        }
        size = modbus_reg_count_to_size(message->count);
        if (message->data_size < size) {
            DEBUG(
                "[modbus] modbus_check_message: read input registers data size %" PRIuSIZE " is too small, need %" PRIuSIZE" \n",
                message->data_size, size);
            return -EINVAL;
        }
        break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
        if (message->count < 1 || message->count > 125) {
            DEBUG("[modbus] modbus_check_message: invalid read holding registers count %" PRIu16 "\n",
                  message->count);
            return -EINVAL;
        }
        if (message->data == NULL) {
            DEBUG_PUTS("[modbus] modbus_check_message: read holding registers data is NULL");
            return -EINVAL;
        }
        size = modbus_reg_count_to_size(message->count);
        if (message->data_size < size) {
            DEBUG(
                "[modbus] modbus_check_message: read holding registers data size %" PRIuSIZE " is too small, need %" PRIuSIZE "\n",
                message->data_size, size);
            return -EINVAL;
        }
        break;

    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        if (message->count < 1 || message->count > 125) {
            DEBUG(
                "[modbus] modbus_check_message: invalid write multiple holding registers count %u\n",
                message->count);
            return -EINVAL;
        }
        if (message->data == NULL) {
            DEBUG_PUTS(
                "[modbus] modbus_check_message: write multiple holding registers data is NULL");
            return -EINVAL;
        }
        size = modbus_reg_count_to_size(message->count);
        if (message->data_size < size) {
            DEBUG(
                "[modbus] modbus_check_message: write multiple holding registers data size %" PRIuSIZE " is too small, need %" PRIuSIZE "\n",
                message->data_size, size);
            return -EINVAL;
        }
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
        if (message->data == NULL) {
            DEBUG_PUTS("[modbus] modbus_check_message: write single coil data is NULL");
            return -EINVAL;
        }
        if (message->data_size < 2) {
            DEBUG(
                "[modbus] modbus_check_message: write single coil data size %" PRIuSIZE " is too small, need 2\n",
                message->data_size);
            return -EINVAL;
        }
        /* data must be 0xFF00 or 0x0000 */
        if (!((message->data[0] == 0x00 || message->data[0] == 0xFF) && message->data[1] == 0x00)) {
            DEBUG_PUTS(
                "[modbus] modbus_check_message: write single coil data invalid (need 0xFF00 or 0x0000)");
            return -EINVAL;
        }
        break;

    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        if (message->data == NULL) {
            DEBUG_PUTS("[modbus] modbus_check_message: write single holding register data is NULL");
            return -EINVAL;
        }
        size = modbus_reg_count_to_size(1);
        if (message->data_size < size) {
            DEBUG(
                "[modbus] modbus_check_message: write single holding register data size %" PRIuSIZE " is too small, need %" PRIuSIZE"\n",
                message->data_size, size);
            return -EINVAL;
        }
        break;

    default:
        return -EINVAL;
        break;
    }

    return 0;
}

int modbus_init(modbus_t *modbus, const modbus_desc_t *driver, void *dev, void *params)
{
    assert(modbus != NULL);
    assert(driver != NULL);
    assert(dev != NULL);

    /* initialize mutex */
    mutex_init(&(modbus->lock));

    /* initialize servers */
    modbus->servers = NULL;

    /* initialize driver specifics */
    modbus->driver = driver;
    modbus->dev = dev;
    modbus->params = params;

    int res = modbus->driver->init(modbus);

    if (res != 0) {
        DEBUG("[modbus] modbus_init: driver init failed (%d)\n", res);
        return res;
    }

    return 0;
}
