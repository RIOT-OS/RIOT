/*
 * Copyright (C) 2015  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * Please notice:
 *  - I am not a cryptographer.
 *  - You should not use this code to run a nuclear power plant without a proper review.
 */

#include "crypto/chacha.h"
#include "mutex.h"

#include <string.h>

static chacha_ctx _chacha_prng_ctx = {
    .state = { RIOT_CHACHA_PRNG_DEFAULT },
    .rounds = 8,
};
static uint32_t _chacha_prng_data[64];
static signed _chacha_prng_pos = 0;
static mutex_t _chacha_prng_mutex = MUTEX_INIT;

void chacha_prng_seed(const void *data, size_t bytes)
{
    mutex_lock(&_chacha_prng_mutex);

    memcpy(_chacha_prng_ctx.state, data, bytes);
    _chacha_prng_pos = 0;

    mutex_unlock(&_chacha_prng_mutex);
}

uint32_t chacha_prng_next(void)
{
    mutex_lock(&_chacha_prng_mutex);

    if (--_chacha_prng_pos < 0) {
        _chacha_prng_pos = 15;
        chacha_keystream_bytes(&_chacha_prng_ctx, _chacha_prng_data);
    }
    uint32_t result = _chacha_prng_data[_chacha_prng_pos];

    mutex_unlock(&_chacha_prng_mutex);
    return result;
}
