/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef CRYPTO_RNG_h
#define CRYPTO_RNG_h

#include <inttypes.h>
#include <stddef.h>

class NoiseSource;

class RNGClass
{
public:
    RNGClass();
    ~RNGClass();

    void begin(const char *tag);
    void addNoiseSource(NoiseSource &source);

    void setAutoSaveTime(uint16_t minutes);

    void rand(uint8_t *data, size_t len);
    bool available(size_t len) const;

    void stir(const uint8_t *data, size_t len, unsigned int credit = 0);

    void save();

    void loop();

    void destroy();

    static const int SEED_SIZE = 48;

private:
    uint32_t block[16];
    uint32_t stream[16];
    uint16_t credits : 13;
    uint16_t firstSave : 1;
    uint16_t initialized : 1;
    uint16_t trngPending : 1;
    unsigned long timer;
    unsigned long timeout;
    NoiseSource *noiseSources[4];
    uint8_t count;
    uint8_t trngPosn;

    void rekey();
    void mixTRNG();
};

extern RNGClass CryptRNG;

#endif
