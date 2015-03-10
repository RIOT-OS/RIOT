/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
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
 * @brief       Headers for the implementation of the NLSv2 stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Gregory Rose
 * @author      Philip Hawkes
 * @author      Michael Paddon
 * @author      Miriam Wiggers de Vries
 *
 */

#ifndef __NLSV2_H_
#define __NLSV2_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Algorithm constants
 * @{
 */
#define N 17			/* number of iterations */
#define INITKONST 0x6996c53a	/* value of KONST to use during key loading */
#define KEYP 15         	/* where to insert key words */
#define FOLDP 4         	/* where to insert extra non-linear feedback */
/** @} */

/**
 * @name Bit arithmetic macros
 * @{
 */
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFF)
#define ROTL(v, n) (U32V((v) << (n)) | ((v) >> (32 - (n))))
#define U8TO32_LITTLE(p) (((uint32_t*)(p))[0])
#define U32TO8_LITTLE(p, v) (((uint32_t*)(p))[0] = v)
#define Byte(x,i) ((uint8_t)(((x) >> (8*(i))) & 0xFF))

#define BYTE2WORD(b) ( \
        (((uint32_t)(b)[3] & 0xFF)<<24) | \
        (((uint32_t)(b)[2] & 0xFF)<<16) | \
        (((uint32_t)(b)[1] & 0xFF)<<8) | \
        (((uint32_t)(b)[0] & 0xFF)) \
)

#define WORD2BYTE(w, b) { \
        (b)[3] = Byte(w,3); \
        (b)[2] = Byte(w,2); \
        (b)[1] = Byte(w,1); \
        (b)[0] = Byte(w,0); \
}

#define XORWORD(w, b) { \
        (b)[3] ^= Byte(w,3); \
        (b)[2] ^= Byte(w,2); \
        (b)[1] ^= Byte(w,1); \
        (b)[0] ^= Byte(w,0); \
}

#define A c->M[0]
#define B c->M[1]
#define C c->M[2]
#define D c->M[3]
#define E c->M[4]
#define F c->M[5]
#define G c->M[6]
#define H c->M[7]
#define SIGMA0(x) (ROTR((x), 2) ^ ROTR((x), 13) ^ ROTR((x), 22))
#define SIGMA1(x) (ROTR((x), 6) ^ ROTR((x), 11) ^ ROTR((x), 25))
#define CHOOSE(x,y,z) (((x) & (y)) ^ ((~(x)) & (z)))
#define MAJORITY(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
/** @} */

/**
 * @brief The NLSv2 state context
 */
typedef struct {
    uint32_t        R[N]; /**< Storage for the shift register */
    uint32_t        initR[N]; /**< Saved register contents */
    uint32_t        konst; /**< Key dependent constant */
    uint32_t        sbuf; /**< Encryption buffer */
    uint32_t        ivsize; /**< IV size */
    int             nbuf; /**< Number of stream bits buffered */
} nlsv2_ctx;

/**
 * @brief  NLSv2 Key setup
 *
 * @param[in]   ctx             NLSv2 context
 * @param[in]   key             Key to setup the ctx
 * @param[in]   keysize         Size of the key in bits
 * @param[in]   ivsize          Size of the IV in bits
 */
void nlsv2_keysetup(nlsv2_ctx *ctx, const uint8_t *key, uint32_t keysize, uint32_t ivsize);

/**
 * @brief  NLSv2 IV setup
 *
 * @param[in]   ctx             NLSv2 context
 * @param[in]   iv              IV to setup the ctx
 */
void nlsv2_ivsetup(nlsv2_ctx *ctx, const uint8_t *iv);

/**
 * @brief Encrypt or decrypt "msglen" number of message bytes
 *
 * @param[in]   ctx             NLSv2 context
 * @param[in]   input           (Plain/Cipher)text for (en/de)crypting
 * @param[out]  output          (Cipher/Plain)text for (en/de)crypting
 * @param[in]   msglen          Number of bytes to (en/de)crypt
 */
void nlsv2_process_bytes(nlsv2_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen);

/**
 * @brief The NLSv2 sbox definitions
 */
static const uint32_t Sbox[256] = {
    0xa3aa1887, 0xd65e435c, 0x0b65c042, 0x800e6ef4,
    0xfc57ee20, 0x4d84fed3, 0xf066c502, 0xf354e8ae,
    0xbb2ee9d9, 0x281f38d4, 0x1f829b5d, 0x735cdf3c,
    0x95864249, 0xbc2e3963, 0xa1f4429f, 0xf6432c35,
    0xf7f40325, 0x3cc0dd70, 0x5f973ded, 0x9902dc5e,
    0xda175b42, 0x590012bf, 0xdc94d78c, 0x39aab26b,
    0x4ac11b9a, 0x8c168146, 0xc3ea8ec5, 0x058ac28f,
    0x52ed5c0f, 0x25b4101c, 0x5a2db082, 0x370929e1,
    0x2a1843de, 0xfe8299fc, 0x202fbc4b, 0x833915dd,
    0x33a803fa, 0xd446b2de, 0x46233342, 0x4fcee7c3,
    0x3ad607ef, 0x9e97ebab, 0x507f859b, 0xe81f2e2f,
    0xc55b71da, 0xd7e2269a, 0x1339c3d1, 0x7ca56b36,
    0xa6c9def2, 0xb5c9fc5f, 0x5927b3a3, 0x89a56ddf,
    0xc625b510, 0x560f85a7, 0xace82e71, 0x2ecb8816,
    0x44951e2a, 0x97f5f6af, 0xdfcbc2b3, 0xce4ff55d,
    0xcb6b6214, 0x2b0b83e3, 0x549ea6f5, 0x9de041af,
    0x792f1f17, 0xf73b99ee, 0x39a65ec0, 0x4c7016c6,
    0x857709a4, 0xd6326e01, 0xc7b280d9, 0x5cfb1418,
    0xa6aff227, 0xfd548203, 0x506b9d96, 0xa117a8c0,
    0x9cd5bf6e, 0xdcee7888, 0x61fcfe64, 0xf7a193cd,
    0x050d0184, 0xe8ae4930, 0x88014f36, 0xd6a87088,
    0x6bad6c2a, 0x1422c678, 0xe9204de7, 0xb7c2e759,
    0x0200248e, 0x013b446b, 0xda0d9fc2, 0x0414a895,
    0x3a6cc3a1, 0x56fef170, 0x86c19155, 0xcf7b8a66,
    0x551b5e69, 0xb4a8623e, 0xa2bdfa35, 0xc4f068cc,
    0x573a6acd, 0x6355e936, 0x03602db9, 0x0edf13c1,
    0x2d0bb16d, 0x6980b83c, 0xfeb23763, 0x3dd8a911,
    0x01b6bc13, 0xf55579d7, 0xf55c2fa8, 0x19f4196e,
    0xe7db5476, 0x8d64a866, 0xc06e16ad, 0xb17fc515,
    0xc46feb3c, 0x8bc8a306, 0xad6799d9, 0x571a9133,
    0x992466dd, 0x92eb5dcd, 0xac118f50, 0x9fafb226,
    0xa1b9cef3, 0x3ab36189, 0x347a19b1, 0x62c73084,
    0xc27ded5c, 0x6c8bc58f, 0x1cdde421, 0xed1e47fb,
    0xcdcc715e, 0xb9c0ff99, 0x4b122f0f, 0xc4d25184,
    0xaf7a5e6c, 0x5bbf18bc, 0x8dd7c6e0, 0x5fb7e420,
    0x521f523f, 0x4ad9b8a2, 0xe9da1a6b, 0x97888c02,
    0x19d1e354, 0x5aba7d79, 0xa2cc7753, 0x8c2d9655,
    0x19829da1, 0x531590a7, 0x19c1c149, 0x3d537f1c,
    0x50779b69, 0xed71f2b7, 0x463c58fa, 0x52dc4418,
    0xc18c8c76, 0xc120d9f0, 0xafa80d4d, 0x3b74c473,
    0xd09410e9, 0x290e4211, 0xc3c8082b, 0x8f6b334a,
    0x3bf68ed2, 0xa843cc1b, 0x8d3c0ff3, 0x20e564a0,
    0xf8f55a4f, 0x2b40f8e7, 0xfea7f15f, 0xcf00fe21,
    0x8a6d37d6, 0xd0d506f1, 0xade00973, 0xefbbde36,
    0x84670fa8, 0xfa31ab9e, 0xaedab618, 0xc01f52f5,
    0x6558eb4f, 0x71b9e343, 0x4b8d77dd, 0x8cb93da6,
    0x740fd52d, 0x425412f8, 0xc5a63360, 0x10e53ad0,
    0x5a700f1c, 0x8324ed0b, 0xe53dc1ec, 0x1a366795,
    0x6d549d15, 0xc5ce46d7, 0xe17abe76, 0x5f48e0a0,
    0xd0f07c02, 0x941249b7, 0xe49ed6ba, 0x37a47f78,
    0xe1cfffbd, 0xb007ca84, 0xbb65f4da, 0xb59f35da,
    0x33d2aa44, 0x417452ac, 0xc0d674a7, 0x2d61a46a,
    0xdc63152a, 0x3e12b7aa, 0x6e615927, 0xa14fb118,
    0xa151758d, 0xba81687b, 0xe152f0b3, 0x764254ed,
    0x34c77271, 0x0a31acab, 0x54f94aec, 0xb9e994cd,
    0x574d9e81, 0x5b623730, 0xce8a21e8, 0x37917f0b,
    0xe8a9b5d6, 0x9697adf8, 0xf3d30431, 0x5dcac921,
    0x76b35d46, 0xaa430a36, 0xc2194022, 0x22bca65e,
    0xdaec70ba, 0xdfaea8cc, 0x777bae8b, 0x242924d5,
    0x1f098a5a, 0x4b396b81, 0x55de2522, 0x435c1cb8,
    0xaeb8fe1d, 0x9db3c697, 0x5b164f83, 0xe0c16376,
    0xa319224c, 0xd0203b35, 0x433ac0fe, 0x1466a19a,
    0x45f0b24f, 0x51fda998, 0xc0d52d71, 0xfa0896a8,
    0xf9e6053f, 0xa4b0d300, 0xd499cbcc, 0xb95e3d40,
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __NLSV2_H_ */
