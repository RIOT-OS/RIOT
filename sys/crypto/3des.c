/*
 * Copyright (C) 2013 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       implementation of the 3DES cipher-algorithm
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Tom St Denis <tomstdenis@gmail.com>, http://libtomcrypt.com
 * @author      Dobes Vandermeer
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        18.09.2013 14:32:33
 *
 * @note        This implementation is based on a DES implementation included
 *              in the LibTomCrypt modular cryptographic library.
 *              The LibTomCrypt library provides various cryptographic
 *              algorithms in a highly modular and flexible manner.
 *              The library is free for all purposes without any express
 *              guarantee it works.
 *              Tom St Denis, tomstdenis@gmail.com, http://libtomcrypt.com
 *              DES code submitted by Dobes Vandermeer
 * @}
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "crypto/3des.h"
#include "crypto/ciphers.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/*************** GLOBALS ******************/
/**
 * @brief Interface to the 3DES cipher
 */
static const cipher_interface_t tripledes_interface = {
    THREEDES_BLOCK_SIZE,
    THREEDES_MAX_KEY_SIZE,
    tripledes_init,
    tripledes_encrypt,
    tripledes_decrypt
};
const cipher_id_t CIPHER_3DES = &tripledes_interface;


/**
 * @brief struct for the 3DES key expansion
 */
struct des3_key_s {
    uint32_t ek[3][32];         ///< encryption key
    uint32_t dk[3][32];         ///< decryption key
} des3_key_s;

/************** PROTOTYPES ***************/
static void cookey(const uint32_t *raw1, uint32_t *keyout);
static void deskey(const uint8_t *key, int decrypt, uint32_t *keyout);
static void desfunc(uint32_t *block, const uint32_t *keys);
static uint8_t des3_key_setup(const uint8_t *key, struct des3_key_s *dkey);

/*****************************************/


/* Use the key schedule specific in the standard (ANSI X3.92-1981) */

static const uint8_t pc1[56] = {
    56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,
    9,  1, 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35,
    62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37, 29, 21,
    13,  5, 60, 52, 44, 36, 28, 20, 12,  4, 27, 19, 11,  3
};

static const uint8_t totrot[16] = {
    1,   2,  4,  6,
    8,  10, 12, 14,
    15, 17, 19, 21,
    23, 25, 27, 28
};

static const uint8_t pc2[48] = {
    13, 16, 10, 23, 0, 4, 2, 27, 14, 5, 20, 9,
    22, 18, 11, 3, 25, 7, 15, 6, 26, 19, 12, 1,
    40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31
};


static const uint32_t SP1[64] = {
    0x01010400UL, 0x00000000UL, 0x00010000UL, 0x01010404UL,
    0x01010004UL, 0x00010404UL, 0x00000004UL, 0x00010000UL,
    0x00000400UL, 0x01010400UL, 0x01010404UL, 0x00000400UL,
    0x01000404UL, 0x01010004UL, 0x01000000UL, 0x00000004UL,
    0x00000404UL, 0x01000400UL, 0x01000400UL, 0x00010400UL,
    0x00010400UL, 0x01010000UL, 0x01010000UL, 0x01000404UL,
    0x00010004UL, 0x01000004UL, 0x01000004UL, 0x00010004UL,
    0x00000000UL, 0x00000404UL, 0x00010404UL, 0x01000000UL,
    0x00010000UL, 0x01010404UL, 0x00000004UL, 0x01010000UL,
    0x01010400UL, 0x01000000UL, 0x01000000UL, 0x00000400UL,
    0x01010004UL, 0x00010000UL, 0x00010400UL, 0x01000004UL,
    0x00000400UL, 0x00000004UL, 0x01000404UL, 0x00010404UL,
    0x01010404UL, 0x00010004UL, 0x01010000UL, 0x01000404UL,
    0x01000004UL, 0x00000404UL, 0x00010404UL, 0x01010400UL,
    0x00000404UL, 0x01000400UL, 0x01000400UL, 0x00000000UL,
    0x00010004UL, 0x00010400UL, 0x00000000UL, 0x01010004UL
};

static const uint32_t SP2[64] = {
    0x80108020UL, 0x80008000UL, 0x00008000UL, 0x00108020UL,
    0x00100000UL, 0x00000020UL, 0x80100020UL, 0x80008020UL,
    0x80000020UL, 0x80108020UL, 0x80108000UL, 0x80000000UL,
    0x80008000UL, 0x00100000UL, 0x00000020UL, 0x80100020UL,
    0x00108000UL, 0x00100020UL, 0x80008020UL, 0x00000000UL,
    0x80000000UL, 0x00008000UL, 0x00108020UL, 0x80100000UL,
    0x00100020UL, 0x80000020UL, 0x00000000UL, 0x00108000UL,
    0x00008020UL, 0x80108000UL, 0x80100000UL, 0x00008020UL,
    0x00000000UL, 0x00108020UL, 0x80100020UL, 0x00100000UL,
    0x80008020UL, 0x80100000UL, 0x80108000UL, 0x00008000UL,
    0x80100000UL, 0x80008000UL, 0x00000020UL, 0x80108020UL,
    0x00108020UL, 0x00000020UL, 0x00008000UL, 0x80000000UL,
    0x00008020UL, 0x80108000UL, 0x00100000UL, 0x80000020UL,
    0x00100020UL, 0x80008020UL, 0x80000020UL, 0x00100020UL,
    0x00108000UL, 0x00000000UL, 0x80008000UL, 0x00008020UL,
    0x80000000UL, 0x80100020UL, 0x80108020UL, 0x00108000UL
};

static const uint32_t SP3[64] = {
    0x00000208UL, 0x08020200UL, 0x00000000UL, 0x08020008UL,
    0x08000200UL, 0x00000000UL, 0x00020208UL, 0x08000200UL,
    0x00020008UL, 0x08000008UL, 0x08000008UL, 0x00020000UL,
    0x08020208UL, 0x00020008UL, 0x08020000UL, 0x00000208UL,
    0x08000000UL, 0x00000008UL, 0x08020200UL, 0x00000200UL,
    0x00020200UL, 0x08020000UL, 0x08020008UL, 0x00020208UL,
    0x08000208UL, 0x00020200UL, 0x00020000UL, 0x08000208UL,
    0x00000008UL, 0x08020208UL, 0x00000200UL, 0x08000000UL,
    0x08020200UL, 0x08000000UL, 0x00020008UL, 0x00000208UL,
    0x00020000UL, 0x08020200UL, 0x08000200UL, 0x00000000UL,
    0x00000200UL, 0x00020008UL, 0x08020208UL, 0x08000200UL,
    0x08000008UL, 0x00000200UL, 0x00000000UL, 0x08020008UL,
    0x08000208UL, 0x00020000UL, 0x08000000UL, 0x08020208UL,
    0x00000008UL, 0x00020208UL, 0x00020200UL, 0x08000008UL,
    0x08020000UL, 0x08000208UL, 0x00000208UL, 0x08020000UL,
    0x00020208UL, 0x00000008UL, 0x08020008UL, 0x00020200UL
};

static const uint32_t SP4[64] = {
    0x00802001UL, 0x00002081UL, 0x00002081UL, 0x00000080UL,
    0x00802080UL, 0x00800081UL, 0x00800001UL, 0x00002001UL,
    0x00000000UL, 0x00802000UL, 0x00802000UL, 0x00802081UL,
    0x00000081UL, 0x00000000UL, 0x00800080UL, 0x00800001UL,
    0x00000001UL, 0x00002000UL, 0x00800000UL, 0x00802001UL,
    0x00000080UL, 0x00800000UL, 0x00002001UL, 0x00002080UL,
    0x00800081UL, 0x00000001UL, 0x00002080UL, 0x00800080UL,
    0x00002000UL, 0x00802080UL, 0x00802081UL, 0x00000081UL,
    0x00800080UL, 0x00800001UL, 0x00802000UL, 0x00802081UL,
    0x00000081UL, 0x00000000UL, 0x00000000UL, 0x00802000UL,
    0x00002080UL, 0x00800080UL, 0x00800081UL, 0x00000001UL,
    0x00802001UL, 0x00002081UL, 0x00002081UL, 0x00000080UL,
    0x00802081UL, 0x00000081UL, 0x00000001UL, 0x00002000UL,
    0x00800001UL, 0x00002001UL, 0x00802080UL, 0x00800081UL,
    0x00002001UL, 0x00002080UL, 0x00800000UL, 0x00802001UL,
    0x00000080UL, 0x00800000UL, 0x00002000UL, 0x00802080UL
};

static const uint32_t SP5[64] = {
    0x00000100UL, 0x02080100UL, 0x02080000UL, 0x42000100UL,
    0x00080000UL, 0x00000100UL, 0x40000000UL, 0x02080000UL,
    0x40080100UL, 0x00080000UL, 0x02000100UL, 0x40080100UL,
    0x42000100UL, 0x42080000UL, 0x00080100UL, 0x40000000UL,
    0x02000000UL, 0x40080000UL, 0x40080000UL, 0x00000000UL,
    0x40000100UL, 0x42080100UL, 0x42080100UL, 0x02000100UL,
    0x42080000UL, 0x40000100UL, 0x00000000UL, 0x42000000UL,
    0x02080100UL, 0x02000000UL, 0x42000000UL, 0x00080100UL,
    0x00080000UL, 0x42000100UL, 0x00000100UL, 0x02000000UL,
    0x40000000UL, 0x02080000UL, 0x42000100UL, 0x40080100UL,
    0x02000100UL, 0x40000000UL, 0x42080000UL, 0x02080100UL,
    0x40080100UL, 0x00000100UL, 0x02000000UL, 0x42080000UL,
    0x42080100UL, 0x00080100UL, 0x42000000UL, 0x42080100UL,
    0x02080000UL, 0x00000000UL, 0x40080000UL, 0x42000000UL,
    0x00080100UL, 0x02000100UL, 0x40000100UL, 0x00080000UL,
    0x00000000UL, 0x40080000UL, 0x02080100UL, 0x40000100UL
};

static const uint32_t SP6[64] = {
    0x20000010UL, 0x20400000UL, 0x00004000UL, 0x20404010UL,
    0x20400000UL, 0x00000010UL, 0x20404010UL, 0x00400000UL,
    0x20004000UL, 0x00404010UL, 0x00400000UL, 0x20000010UL,
    0x00400010UL, 0x20004000UL, 0x20000000UL, 0x00004010UL,
    0x00000000UL, 0x00400010UL, 0x20004010UL, 0x00004000UL,
    0x00404000UL, 0x20004010UL, 0x00000010UL, 0x20400010UL,
    0x20400010UL, 0x00000000UL, 0x00404010UL, 0x20404000UL,
    0x00004010UL, 0x00404000UL, 0x20404000UL, 0x20000000UL,
    0x20004000UL, 0x00000010UL, 0x20400010UL, 0x00404000UL,
    0x20404010UL, 0x00400000UL, 0x00004010UL, 0x20000010UL,
    0x00400000UL, 0x20004000UL, 0x20000000UL, 0x00004010UL,
    0x20000010UL, 0x20404010UL, 0x00404000UL, 0x20400000UL,
    0x00404010UL, 0x20404000UL, 0x00000000UL, 0x20400010UL,
    0x00000010UL, 0x00004000UL, 0x20400000UL, 0x00404010UL,
    0x00004000UL, 0x00400010UL, 0x20004010UL, 0x00000000UL,
    0x20404000UL, 0x20000000UL, 0x00400010UL, 0x20004010UL
};

static const uint32_t SP7[64] = {
    0x00200000UL, 0x04200002UL, 0x04000802UL, 0x00000000UL,
    0x00000800UL, 0x04000802UL, 0x00200802UL, 0x04200800UL,
    0x04200802UL, 0x00200000UL, 0x00000000UL, 0x04000002UL,
    0x00000002UL, 0x04000000UL, 0x04200002UL, 0x00000802UL,
    0x04000800UL, 0x00200802UL, 0x00200002UL, 0x04000800UL,
    0x04000002UL, 0x04200000UL, 0x04200800UL, 0x00200002UL,
    0x04200000UL, 0x00000800UL, 0x00000802UL, 0x04200802UL,
    0x00200800UL, 0x00000002UL, 0x04000000UL, 0x00200800UL,
    0x04000000UL, 0x00200800UL, 0x00200000UL, 0x04000802UL,
    0x04000802UL, 0x04200002UL, 0x04200002UL, 0x00000002UL,
    0x00200002UL, 0x04000000UL, 0x04000800UL, 0x00200000UL,
    0x04200800UL, 0x00000802UL, 0x00200802UL, 0x04200800UL,
    0x00000802UL, 0x04000002UL, 0x04200802UL, 0x04200000UL,
    0x00200800UL, 0x00000000UL, 0x00000002UL, 0x04200802UL,
    0x00000000UL, 0x00200802UL, 0x04200000UL, 0x00000800UL,
    0x04000002UL, 0x04000800UL, 0x00000800UL, 0x00200002UL
};

static const uint32_t SP8[64] = {
    0x10001040UL, 0x00001000UL, 0x00040000UL, 0x10041040UL,
    0x10000000UL, 0x10001040UL, 0x00000040UL, 0x10000000UL,
    0x00040040UL, 0x10040000UL, 0x10041040UL, 0x00041000UL,
    0x10041000UL, 0x00041040UL, 0x00001000UL, 0x00000040UL,
    0x10040000UL, 0x10000040UL, 0x10001000UL, 0x00001040UL,
    0x00041000UL, 0x00040040UL, 0x10040040UL, 0x10041000UL,
    0x00001040UL, 0x00000000UL, 0x00000000UL, 0x10040040UL,
    0x10000040UL, 0x10001000UL, 0x00041040UL, 0x00040000UL,
    0x00041040UL, 0x00040000UL, 0x10041000UL, 0x00001000UL,
    0x00000040UL, 0x10040040UL, 0x00001000UL, 0x00041040UL,
    0x10001000UL, 0x00000040UL, 0x10000040UL, 0x10040000UL,
    0x10040040UL, 0x10000000UL, 0x00040000UL, 0x10001040UL,
    0x00000000UL, 0x10041040UL, 0x00040040UL, 0x10000040UL,
    0x10040000UL, 0x10001000UL, 0x10001040UL, 0x00000000UL,
    0x10041040UL, 0x00041000UL, 0x00041000UL, 0x00001040UL,
    0x00001040UL, 0x00040040UL, 0x10000000UL, 0x10041000UL
};


int tripledes_init(cipher_context_t *context, const uint8_t *key,
                  uint8_t keySize)
{
    uint8_t i;

    // Make sure that context is large enough. If this is not the case,
    // you should build with -DTHREEDES
    if(CIPHER_MAX_CONTEXT_SIZE < THREEDES_MAX_KEY_SIZE) {
        return 0;
    }

    //key must be at least 24 Bytes long
    if (keySize < 24) {
        //fill up by concatenating key to as long as needed
        for (i = 0; i < 24; i++) {
            context->context[i] = key[(i % keySize)];
        }
    }
    else {
        for (i = 0; i < 24; i++) {
            context->context[i] = key[i];
        }
    }

    return 1;
}

int tripledes_encrypt(const cipher_context_t *context, const uint8_t *plain, uint8_t *crypt)
{
    int res;
    struct des3_key_s *key = malloc(sizeof(des3_key_s));
    uint32_t work[2];

    if (!key) {
        DEBUG("%s:%d in %s: [ERROR] Could NOT malloc space for the des3_key_s struct.\r\n",
              RIOT_FILE_RELATIVE, __LINE__, DEBUG_FUNC);
        return -1;
    }

    memset(key, 0, sizeof(des3_key_s));
    res = des3_key_setup(context->context, key);

    if (res < 0) {
        DEBUG("%s:%d in %s: [ERROR] des3_key_setup failed with Code %i\r\n",
              RIOT_FILE_RELATIVE, __LINE__, DEBUG_FUNC, res);
        free(key);
        return -2;
    }

    work[0] = WPA_GET_BE32(plain);
    work[1] = WPA_GET_BE32(plain + 4);
    desfunc(work, key->ek[0]);
    desfunc(work, key->ek[1]);
    desfunc(work, key->ek[2]);
    WPA_PUT_BE32(crypt, work[0]);
    WPA_PUT_BE32(crypt + 4, work[1]);

    free(key);
    return 1;
}


int tripledes_decrypt(const cipher_context_t *context, const uint8_t *crypt, uint8_t *plain)
{
    int res;
    struct des3_key_s *key = malloc(sizeof(des3_key_s));
    uint32_t work[2];

    if (!key) {
        DEBUG("%s:%d in %s: [ERROR] Could NOT malloc space for the des3_key_s struct.\r\n",
              __FILE__, __LINE__, DEBUG_FUNC);
        return -1;
    }

    memset(key, 0, sizeof(des3_key_s));
    res = des3_key_setup(context->context, key);

    if (res < 0) {
        DEBUG("%s:%d in %s: [ERROR] des3_key_setup failed with Code %i\r\n",
              __FILE__, __LINE__, DEBUG_FUNC, res);
        free(key);
        return -2;
    }

    work[0] = WPA_GET_BE32(crypt);
    work[1] = WPA_GET_BE32(crypt + 4);
    desfunc(work, key->dk[0]);
    desfunc(work, key->dk[1]);
    desfunc(work, key->dk[2]);
    WPA_PUT_BE32(plain, work[0]);
    WPA_PUT_BE32(plain + 4, work[1]);

    free(key);
    return 1;
}

static void cookey(const uint32_t *raw1, uint32_t *keyout)
{
    uint32_t *cook;
    uint32_t dough[32];
    int i;

    cook = dough;

    for (i = 0; i < 16; i++, raw1++) {
        const uint32_t *raw0 = raw1++;
        *cook    = (*raw0 & 0x00fc0000L) << 6;
        *cook   |= (*raw0 & 0x00000fc0L) << 10;
        *cook   |= (*raw1 & 0x00fc0000L) >> 10;
        *cook++ |= (*raw1 & 0x00000fc0L) >> 6;
        *cook    = (*raw0 & 0x0003f000L) << 12;
        *cook   |= (*raw0 & 0x0000003fL) << 16;
        *cook   |= (*raw1 & 0x0003f000L) >> 4;
        *cook++ |= (*raw1 & 0x0000003fL);
    }

    memcpy(keyout, dough, sizeof(dough));
}


static void deskey(const uint8_t *key, int decrypt, uint32_t *keyout)
{
    uint32_t i, j, l, m, kn[32];
    uint8_t pc1m[56], pcr[56];

    for (j = 0; j < 56; j++) {
        l = (uint32_t) pc1[j];
        m = l & 7;
        pc1m[j] = (uint8_t)
                  ((key[l >> 3U] & bytebit[m]) == bytebit[m] ? 1 : 0);
    }

    for (i = 0; i < 16; i++) {
        if (decrypt) {
            m = (15 - i) << 1;
        }
        else {
            m = i << 1;
        }

        uint32_t n = m + 1;
        kn[m] = kn[n] = 0L;

        for (j = 0; j < 28; j++) {
            l = j + (uint32_t) totrot[i];

            if (l < 28) {
                pcr[j] = pc1m[l];
            }
            else {
                pcr[j] = pc1m[l - 28];
            }
        }

        for (/* j = 28 */; j < 56; j++) {
            l = j + (uint32_t) totrot[i];

            if (l < 56) {
                pcr[j] = pc1m[l];
            }
            else {
                pcr[j] = pc1m[l - 28];
            }
        }

        for (j = 0; j < 24; j++) {
            if ((int) pcr[(int) pc2[j]] != 0) {
                kn[m] |= bigbyte[j];
            }

            if ((int) pcr[(int) pc2[j + 24]] != 0) {
                kn[n] |= bigbyte[j];
            }
        }
    }

    cookey(kn, keyout);
}


static void desfunc(uint32_t *block, const uint32_t *keys)
{
    uint32_t work, right, leftt;
    int cur_round;

    leftt = block[0];
    right = block[1];

    work = ((leftt >> 4)  ^ right) & 0x0f0f0f0fL;
    right ^= work;
    leftt ^= (work << 4);

    work = ((leftt >> 16) ^ right) & 0x0000ffffL;
    right ^= work;
    leftt ^= (work << 16);

    work = ((right >> 2)  ^ leftt) & 0x33333333L;
    leftt ^= work;
    right ^= (work << 2);

    work = ((right >> 8)  ^ leftt) & 0x00ff00ffL;
    leftt ^= work;
    right ^= (work << 8);

    right = ROLc(right, 1);
    work = (leftt ^ right) & 0xaaaaaaaaL;

    leftt ^= work;
    right ^= work;
    leftt = ROLc(leftt, 1);

    for (cur_round = 0; cur_round < 8; cur_round++) {
        work  = RORc(right, 4) ^ *keys++;
        leftt ^= SP7[work        & 0x3fL]
                 ^ SP5[(work >>  8) & 0x3fL]
                 ^ SP3[(work >> 16) & 0x3fL]
                 ^ SP1[(work >> 24) & 0x3fL];
        work  = right ^ *keys++;
        leftt ^= SP8[ work        & 0x3fL]
                 ^  SP6[(work >>  8) & 0x3fL]
                 ^  SP4[(work >> 16) & 0x3fL]
                 ^  SP2[(work >> 24) & 0x3fL];

        work = RORc(leftt, 4) ^ *keys++;
        right ^= SP7[ work        & 0x3fL]
                 ^  SP5[(work >>  8) & 0x3fL]
                 ^  SP3[(work >> 16) & 0x3fL]
                 ^  SP1[(work >> 24) & 0x3fL];
        work  = leftt ^ *keys++;
        right ^= SP8[ work        & 0x3fL]
                 ^  SP6[(work >>  8) & 0x3fL]
                 ^  SP4[(work >> 16) & 0x3fL]
                 ^  SP2[(work >> 24) & 0x3fL];
    }

    right = RORc(right, 1);
    work = (leftt ^ right) & 0xaaaaaaaaL;
    leftt ^= work;
    right ^= work;
    leftt = RORc(leftt, 1);
    work = ((leftt >> 8) ^ right) & 0x00ff00ffL;
    right ^= work;
    leftt ^= (work << 8);
    /* -- */
    work = ((leftt >> 2) ^ right) & 0x33333333L;
    right ^= work;
    leftt ^= (work << 2);
    work = ((right >> 16) ^ leftt) & 0x0000ffffL;
    leftt ^= work;
    right ^= (work << 16);
    work = ((right >> 4) ^ leftt) & 0x0f0f0f0fL;
    leftt ^= work;
    right ^= (work << 4);

    block[0] = right;
    block[1] = leftt;
}

static uint8_t des3_key_setup(const uint8_t *key, struct des3_key_s *dkey)
{
    deskey(key, 0, dkey->ek[0]);
    deskey(key + 8, 1, dkey->ek[1]);
    deskey(key + 16, 0, dkey->ek[2]);

    deskey(key, 1, dkey->dk[2]);
    deskey(key + 8, 0, dkey->dk[1]);
    deskey(key + 16, 1, dkey->dk[0]);
    return 1;
}
