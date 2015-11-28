/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */
#include <inttypes.h>
#include <string.h>

#include "byteorder.h"
#include "cpu.h"
#include "hashes.h"
#include "periph/random.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if RANDOM_NUMOF

#ifndef RIOT_XMC_SEED
/* This gets the 80 Bit seed from readings of the temperature
 * sensor. It's a comfortable way to get some entropy of questionable
 * quality. */
static size_t _seed_from_temperature(uint16_t *buf, size_t size)
{
    uint8_t last = 0, current = 0;

    /* Turn on the temperature sensor */
    SCU_ANALOG->ANATSECTRL = 1;

    for (unsigned i = 0; i < size; i++) {
        buf[i] = 0;

        for (unsigned count = 0; count < 16; ) {
            /* Get a new low nibble distinct from the one before */
            while ((current = SCU_ANALOG->ANATSEMON & 0xf) == last) ;

            uint8_t val = last = current;

            /* John von Neumann whitening algorithm to fix
             * simple bias and reduce correlation. */
            while (val) {
                switch (val & 3) {
                    case 1: /* a sequence of 01 becomes a one  */
                    case 2: /* a sequence of 10 becomes a zero */
                        buf[i] = (buf[i] << 1) | (val & 1);
                        count++;
                    case 0: /* when two successive bits are equal, they are discarded */
                    case 3:
                        val >>= 2;
                }
            }
        }
    }

    /* Turn off the temperature sensor (or assume it does, no
     * documentation available) */
    SCU_ANALOG->ANATSECTRL = 0;

    return size;
}
#endif

/* Number of 16Bit values used as seed. Manual recommends 80Bit */
#define SEED_NUM (5)

#define RANDOM_WAIT()                           \
    do { /* wait for random bits */ }           \
    while (!(PRNG->CHK & PRNG_CHK_RDV_Msk))

void random_init(void)
{
    uint64_t discard;

    /* Use
     * $>RIOT_XMC_EXTERNAL_SEED=/dev/urandom make
     * to generate a static random seed at build time
     */
#ifdef RIOT_XMC_SEED
    uint16_t seed[SEED_NUM] = { RIOT_XMC_SEED };
#else
    uint16_t seed[SEED_NUM];
    _seed_from_temperature(&seed[0], SEED_NUM);
#endif

    /* Start key loading mode (KLD) and feed with 16bit words */
    PRNG->CTRL = (1 << PRNG_CTRL_KLD_Pos) | (2 << PRNG_CTRL_RDBS_Pos);

    /* Load seed as 16bit words */
    for (int i = 0; i < SEED_NUM; i++) {
        RANDOM_WAIT();
        PRNG->WORD = seed[i];
        DEBUG("random: loaded seed value 0x%04x\n", (unsigned)PRNG->WORD);
    }

    /* Start streaming mode with 8bit word size */
    PRNG->CTRL = (0 << PRNG_CTRL_KLD_Pos) | (1 << PRNG_CTRL_RDBS_Pos);

    /* "The warm-up phase provides a thorough diffusion of the key
       bits. For this purpose the user must read and discard 64 random
       bits from the register PRNG_WORD." */
    random_read((char *)&discard, sizeof(discard));
}

int random_read(char *buf, unsigned int num)
{
    for (int i = 0; i < num; i++) {
        RANDOM_WAIT();
        buf[i] = (char)PRNG->WORD;
    }
    return num;
}

void random_poweron(void)
{
    /* There is no need to power on the PRNG on XMC. */
}

void random_poweroff(void)
{
    /* The PRNG on XMC is always on and cannot be suspended. */
}

#endif /* RANDOM_NUMOF */
