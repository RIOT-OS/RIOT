/*
 * Copyright (C) 2023 Freie Universität Berlin
 *               2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph_i2s
 * @{
 *
 * @file
 * @brief       common I2S function implementations
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stddef.h>
#include <string.h>
#include "macros/utils.h"
#include "periph/i2s.h"

#if PERIPH_I2S_NEEDS_WRITE_BUFFER
size_t i2s_write_buffer(i2s_t dev, const void *source, size_t bytes, i2s_transaction_t *transaction, size_t offset)
{
    size_t bytes_to_copy = MIN(bytes, transaction_bytes - offset);
    uint8_t *target = transaction->buf + offset;
    memcpy(target, (uint8_t*)source, bytes_to_copy);
    return bytes_to_copy;
}
#endif
