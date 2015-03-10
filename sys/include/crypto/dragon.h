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
 * @file        dragon.h
 * @brief       Headers for the implementation of the Dragon stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Kevin Chen
 * @author      Matt Henricksen
 * @author      William Millan
 * @author      Joanne Fuller
 * @author      Leonie Simpson
 * @author      Ed Dawson
 * @author      HoonJae Lee
 * @author      SangJae Moon
 *
 */

#ifndef __DRAGON_H_
#define __DRAGON_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The macro definitions
 */
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFF)
#define ROTL32(v, n) (U32V((v) << (n)) | ((v) >> (32 - (n))))
#define SWAP32(v) ((ROTL32(v,  8) & 0x00FF00FF) | (ROTL32(v, 24) & 0xFF00FF00))
#define U32TO32_BIG(v) SWAP32(v)
#define U8TO32_BIG(p) U32TO32_BIG(((uint32_t*)(p))[0])

#define DRAGON_NLFSR_SIZE      32 /* size of NLFSR in 32-bit multiples */
#define DRAGON_KEYSTREAM_SIZE   2 /* size of output in 32-bit multiples */
#define DRAGON_BUFFER_SIZE      1
#define DRAGON_BUFFER_BYTES    (DRAGON_BUFFER_SIZE * DRAGON_KEYSTREAM_SIZE * 4)

/**
 * @brief The Dragon state context
 */
typedef struct {
    uint32_t  nlfsr_word[DRAGON_NLFSR_SIZE];
    uint64_t  state_counter;
    uint32_t  nlfsr_offset;
    uint32_t  init_state[DRAGON_NLFSR_SIZE];
    uint32_t  key_size;
    uint8_t   keystream_buffer[DRAGON_BUFFER_BYTES];
    uint32_t  buffer_index;
} dragon_ctx;

/**
 * The DRAGON_OFFSET macro calculates the position of the
 * ith_element within the circular buffer that represents the
 * NLFSR.
 */
#define DRAGON_OFFSET(ctx, ith_element, state_size) \
    ((ctx->nlfsr_offset + ith_element) & state_size)

/**
 * The DRAGON_NLFSR_WORD macro retrieves the ith 32-bit word
 * from the Dragon NLFSR.
 */
#define DRAGON_NLFSR_WORD(ctx, ith_word) \
    *(ctx->nlfsr_word + DRAGON_OFFSET(ctx, ith_word, (DRAGON_NLFSR_SIZE - 1)))

/**
 * The Dragon update function consists of a pre-mixing, post-mixing and s-box
 * layer. The pre- and post-mixing in turn consist of one XOR layer and one
 * ADD layer. The s-box layer contains two sublayers, one which uses three G
 * s-boxes, and the other which uses three H s-boxes.
 */
#define XOR_LAYER(d1, s1, d2, s2, d3, s3)	\
    d1 ^= s1;					\
    d2 ^= s2;					\
    d3 ^= s3;

#define ADD_LAYER(d1, s1, d2, s2, d3, s3)	\
    d1 += s1;					\
    d2 += s2;					\
    d3 += s3;					\

#define SBOX_LAYER(sbox, d1, s1, d2, s2, d3, s3)	\
    d1 ^= sbox##1(s1);					\
    d2 ^= sbox##2(s2);					\
    d3 ^= sbox##3(s3);

#define DRAGON_UPDATE(a, b, c, d, e, f)	\
    XOR_LAYER(b, a, d, c, f, e)		\
    ADD_LAYER(c, b, e, d, a, f)		\
    SBOX_LAYER(G, d, a, f, c, b, e)	\
    SBOX_LAYER(H, a, b, c, d, e, f)	\
    ADD_LAYER(f, c, b, e, d, a)		\
    XOR_LAYER(a, f, c, b, e, d)

#define DRAGON_MIXING_STAGES   16

/**
 * DRAGON_ROUND produces one block of keystream
 */
#define DRAGON_ROUND(ctx, a, b, c, d, e, f)				\
    a = DRAGON_NLFSR_WORD(ctx, 0);					\
    b = DRAGON_NLFSR_WORD(ctx, 9);					\
    c = DRAGON_NLFSR_WORD(ctx, 16);					\
    d = DRAGON_NLFSR_WORD(ctx, 19);					\
    e = DRAGON_NLFSR_WORD(ctx, 30) ^ U32V(ctx->state_counter >> 32);	\
    f = DRAGON_NLFSR_WORD(ctx, 31) ^ U32V(ctx->state_counter);		\
    DRAGON_UPDATE(a, b, c, d, e, f);					\
    ctx->state_counter++;						\
    ctx->nlfsr_offset -= 2;						\
    DRAGON_NLFSR_WORD(ctx, 0) = b;					\
    DRAGON_NLFSR_WORD(ctx, 1) = c;

/* virtual 32x32 s-boxes G and H */
#define G1(x) 			\
    sbox2[x & 0xFF] ^		\
    sbox1[(x >> 8) & 0xFF]  ^	\
    sbox1[(x >> 16) & 0xFF] ^	\
    sbox1[(x >> 24) & 0xFF]

#define G2(x)			\
    sbox1[x & 0xFF] ^		\
    sbox2[(x >> 8) & 0xFF]  ^	\
    sbox1[(x >> 16) & 0xFF] ^	\
    sbox1[(x >> 24) & 0xFF]

#define G3(x)			\
    sbox1[x & 0xFF] ^		\
    sbox1[(x >> 8) & 0xFF]  ^	\
    sbox2[(x >> 16) & 0xFF] ^	\
    sbox1[(x >> 24) & 0xFF]

#define H1(x)			\
    sbox1[x & 0xFF] ^		\
    sbox2[(x >> 8) & 0xFF]  ^	\
    sbox2[(x >> 16) & 0xFF] ^	\
    sbox2[(x >> 24) & 0xFF]

#define H2(x)			\
    sbox2[x & 0xFF] ^		\
    sbox1[(x >> 8) & 0xFF]  ^	\
    sbox2[(x >> 16) & 0xFF] ^	\
    sbox2[(x >> 24) & 0xFF]

#define H3(x)			\
    sbox2[x & 0xFF] ^		\
    sbox2[(x >> 8) & 0xFF]  ^	\
    sbox1[(x >> 16) & 0xFF] ^	\
    sbox2[(x >> 24) & 0xFF]


/**
 * @brief  Dragon Key setup
 *
 * @param[in]   ctx     Dragon context
 * @param[in]   key     Key to setup the ctx
 * @param[in]   keysize Size of the key
 */
void dragon_keysetup(dragon_ctx *ctx, const uint8_t *key, uint32_t keysize);

/**
 * @brief  Dragon IV setup
 *
 * @param[in]   ctx     Dragon context
 * @param[in]   iv      IV to setup the ctx
 */
void dragon_ivsetup(dragon_ctx *ctx, const uint8_t *iv);

/**
 * @brief  Generate "blocks" number of keystream blocks
 *
 * @param[in]   ctx         Dragon context
 * @param[out]  keystream   Pre-allocated array containing 8*(blocks)
 *                          bytes of memory
 * @param[in]   length      Number of keystream blocks to produce
 */
void dragon_keystream_blocks(dragon_ctx *ctx, uint8_t *keystream, uint32_t blocks);

/**
 * @brief  Generate "length" number of keystream bytes
 *
 * @param[in]   ctx         Dragon context
 * @param[out]  keystream   Pre-allocated array containing (msglen)
 *                          bytes of memory
 * @param[in]   length      Number of keystream bytes to produce
 */
void dragon_keystream_bytes(dragon_ctx *ctx, uint8_t *keystream, uint32_t length);

/**
 * @brief  Encrypt or decrypt a "blocks" number of blocks
 *
 * @param[in]   ctx         Dragon context
 * @param[in]   input       (Plain/Cipher)text blocks for (en/de)crypting
 * @param[out]  output      Pre-allocated array for (cipher/plain)text blocks
 *                          consisting of 8*(blocks) bytes
 * @param[in]   blocks      Number of blocks to (en/de)crypt
 */
void dragon_process_blocks(dragon_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t blocks);

/**
 * @brief  Encrypt or decrypt "msglen" number of message bytes
 *
 * @param[in]   ctx         Dragon context
 * @param[in]   input       (Plain/Cipher)text for (en/de)crypting
 * @param[out]  output      (Cipher/Plain)text for (en/de)crypting
 * @param[in]   msglen      Number of bytes to (en/de)crypt
 */
void dragon_process_bytes(dragon_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen);

/**
 * @brief The Dragon sbox definitions
 */
static const uint32_t sbox1[256] = {
    0x393BCE6B, 0x232BA00D, 0x84E18ADA, 0x84557BA7,
    0x56828948, 0x166908F3, 0x414A3437, 0x7BB44897,
    0x2315BE89, 0x7A01F224, 0x7056AA5D, 0x121A3917,
    0xE3F47FA2, 0x1F99D0AD, 0x9BAD518B, 0x99B9E75F,
    0x8829A7ED, 0x2C511CA9, 0x1D89BF75, 0xF2F8CDD0,
    0x2DA2C498, 0x48314C42, 0x922D9AF6, 0xAA6CE00C,
    0xAC66E078, 0x7D4CB0C0, 0x5500C6E8, 0x23E4576B,
    0x6B365D40, 0xEE171139, 0x336BE860, 0x5DBEEEFE,
    0x0E945776, 0xD4D52CC4, 0x0E9BB490, 0x376EB6FD,
    0x6D891655, 0xD4078FEE, 0xE07401E7, 0xA1E4350C,
    0xABC78246, 0x73409C02, 0x24704A1F, 0x478ABB2C,
    0xA0849634, 0x9E9E5FEB, 0x77363D8D, 0xD350BC21,
    0x876E1BB5, 0xC8F55C9D, 0xD112F39F, 0xDF1A0245,
    0x9711B3F0, 0xA3534F64, 0x42FB629E, 0x15EAD26A,
    0xD1CFA296, 0x7B445FEE, 0x88C28D4A, 0xCA6A8992,
    0xB40726AB, 0x508C65BC, 0xBE87B3B9, 0x4A894942,
    0x9AEECC5B, 0x6CA6F10B, 0x303F8934, 0xD7A8693A,
    0x7C8A16E4, 0xB8CF0AC9, 0xAD14B784, 0x819FF9F0,
    0xF20DCDFA, 0xB7CB7159, 0x58F3199F, 0x9855E43B,
    0x1DF6C2D6, 0x46114185, 0xE46F5D0F, 0xAAC70B5B,
    0x48590537, 0x0FD77B28, 0x67D16C70, 0x75AE53F4,
    0xF7BFECA1, 0x6017B2D2, 0xD8A0FA28, 0xB8FC2E0D,
    0x80168E15, 0x0D7DEC9D, 0xC5581F55, 0xBE4A2783,
    0xD27012FE, 0x53EA81CA, 0xEBAA07D2, 0x54F5D41D,
    0xABB26FA6, 0x41B9EAD9, 0xA48174C7, 0x1F3026F0,
    0xEFBADD8E, 0x387E9014, 0x1505AB79, 0xEADF0DF7,
    0x67755401, 0xDA2EF962, 0x41670B0E, 0x0E8642F2,
    0xCE486070, 0xA47D3312, 0x4D7343A7, 0xECDA58D0,
    0x1F79D536, 0xD362576B, 0x9D3A6023, 0xC795A610,
    0xAE4DF639, 0x60C0B14E, 0xC6DD8E02, 0xBDE93F4E,
    0xB7C3B0FF, 0x2BE6BCAD, 0xE4B3FDFD, 0x79897325,
    0x3038798B, 0x08AE6353, 0x7D1D20EB, 0x3B208D21,
    0xD0D6D104, 0xC5244327, 0x9893F59F, 0xE976832A,
    0xB1EB320B, 0xA409D915, 0x7EC6B543, 0x66E54F98,
    0x5FF805DC, 0x599B223F, 0xAD78B682, 0x2CF5C6E8,
    0x4FC71D63, 0x08F8FED1, 0x81C3C49A, 0xE4D0A778,
    0xB5D369CC, 0x2DA336BE, 0x76BC87CB, 0x957A1878,
    0xFA136FBA, 0x8F3C0E7B, 0x7A1FF157, 0x598324AE,
    0xFFBAAC22, 0xD67DE9E6, 0x3EB52897, 0x4E07E855,
    0x87CE73F5, 0x8D046706, 0xD42D18F2, 0xE71B1727,
    0x38473B38, 0xB37B24D5, 0x381C6AE1, 0xE77D6589,
    0x6018CBFF, 0x93CF3752, 0x9B6EA235, 0x504A50E8,
    0x464EA180, 0x86AFBE5E, 0xCC2D6AB0, 0xAB91707B,
    0x1DB4D579, 0xF9FAFD24, 0x2B28CC54, 0xCDCFD6B3,
    0x68A30978, 0x43A6DFD7, 0xC81DD98E, 0xA6C2FD31,
    0x0FD07543, 0xAFB400CC, 0x5AF11A03, 0x2647A909,
    0x24791387, 0x5CFB4802, 0x88CE4D29, 0x353F5F5E,
    0x7038F851, 0xF1F1C0AF, 0x78EC6335, 0xF2201AD1,
    0xDF403561, 0x4462DFC7, 0xE22C5044, 0x9C829EA3,
    0x43FD6EAE, 0x7A42B3A7, 0x5BFAAAEC, 0x3E046853,
    0x5789D266, 0xE1219370, 0xB2C420F8, 0x3218BD4E,
    0x84590D94, 0xD51D3A8C, 0xA3AB3D24, 0x2A339E3D,
    0xFEE67A23, 0xAF844391, 0x17465609, 0xA99AD0A1,
    0x05CA597B, 0x6024A656, 0x0BF05203, 0x8F559DDC,
    0x894A1911, 0x909F21B4, 0x6A7B63CE, 0xE28DD7E7,
    0x4178AA3D, 0x4346A7AA, 0xA1845E4C, 0x166735F4,
    0x639CA159, 0x58940419, 0x4E4F177A, 0xD17959B2,
    0x12AA6FFD, 0x1D39A8BE, 0x7667F5AC, 0xED0CE165,
    0xF1658FD8, 0x28B04E02, 0x1FA480CF, 0xD3FB6FEF,
    0xED336CCB, 0x9EE3CA39, 0x9F224202, 0x2D12D6E8,
    0xFAAC50CE, 0xFA1E98AE, 0x61498532, 0x03678CC0,
    0x9E85EFD7, 0x3069CE1A, 0xF115D008, 0x4553AA9F,
    0x3194BE09, 0xB4A9367D, 0x0A9DFEEC, 0x7CA002D6,
    0x8E53A875, 0x965E8183, 0x14D79DAC, 0x0192B555
};

static const uint32_t sbox2[256] = {
    0xA94BC384, 0xF7A81CAE, 0xAB84ECD4, 0x00DEF340,
    0x8E2329B8, 0x23AF3A22, 0x23C241FA, 0xAED8729E,
    0x2E59357F, 0xC3ED78AB, 0x687724BB, 0x7663886F,
    0x1669AA35, 0x5966EAC1, 0xD574C543, 0xDBC3F2FF,
    0x4DD44303, 0xCD4F8D01, 0x0CBF1D6F, 0xA8169D59,
    0x87841E00, 0x3C515AD4, 0x708784D6, 0x13EB675F,
    0x57592B96, 0x07836744, 0x3E721D90, 0x26DAA84F,
    0x253A4E4D, 0xE4FA37D5, 0x9C0830E4, 0xD7F20466,
    0xD41745BD, 0x1275129B, 0x33D0F724, 0xE234C68A,
    0x4CA1F260, 0x2BB0B2B6, 0xBD543A87, 0x4ABD3789,
    0x87A84A81, 0x948104EB, 0xA9AAC3EA, 0xBAC5B4FE,
    0xD4479EB6, 0xC4108568, 0xE144693B, 0x5760C117,
    0x48A9A1A6, 0xA987B887, 0xDF7C74E0, 0xBC0682D7,
    0xEDB7705D, 0x57BFFEAA, 0x8A0BD4F1, 0x1A98D448,
    0xEA4615C9, 0x99E0CBD6, 0x780E39A3, 0xADBCD406,
    0x84DA1362, 0x7A0E984B, 0xBED853E6, 0xD05D610B,
    0x9CAC6A28, 0x1682ACDF, 0x889F605F, 0x9EE2FEBA,
    0xDB556C92, 0x86818021, 0x3CC5BEA1, 0x75A934C6,
    0x95574478, 0x31A92B9B, 0xBFE3E92B, 0xB28067AE,
    0xD862D848, 0x0732A22D, 0x840EF879, 0x79FFA920,
    0x0124C8BB, 0x26C75B69, 0xC3DAAAC5, 0x6E71F2E9,
    0x9FD4AFA6, 0x474D0702, 0x8B6AD73E, 0xF5714E20,
    0xE608A352, 0x2BF644F8, 0x4DF9A8BC, 0xB71EAD7E,
    0x6335F5FB, 0x0A271CE3, 0xD2B552BB, 0x3834A0C3,
    0x341C5908, 0x0674A87B, 0x8C87C0F1, 0xFF0842FC,
    0x48C46BDB, 0x30826DF8, 0x8B82CE8E, 0x0235C905,
    0xDE4844C3, 0x296DF078, 0xEFAA6FEA, 0x6CB98D67,
    0x6E959632, 0xD5D3732F, 0x68D95F19, 0x43FC0148,
    0xF808C7B1, 0xD45DBD5D, 0x5DD1B83B, 0x8BA824FD,
    0xC0449E98, 0xB743CC56, 0x41FADDAC, 0x141E9B1C,
    0x8B937233, 0x9B59DCA7, 0xF1C871AD, 0x6C678B4D,
    0x46617752, 0xAAE49354, 0xCABE8156, 0x6D0AC54C,
    0x680CA74C, 0x5CD82B3F, 0xA1C72A59, 0x336EFB54,
    0xD3B1A748, 0xF4EB40D5, 0x0ADB36CF, 0x59FA1CE0,
    0x2C694FF9, 0x5CE2F81A, 0x469B9E34, 0xCE74A493,
    0x08B55111, 0xEDED517C, 0x1695D6FE, 0xE37C7EC7,
    0x57827B93, 0x0E02A748, 0x6E4A9C0F, 0x4D840764,
    0x9DFFC45C, 0x891D29D7, 0xF9AD0D52, 0x3F663F69,
    0xD00A91B9, 0x615E2398, 0xEDBBC423, 0x09397968,
    0xE42D6B68, 0x24C7EFB1, 0x384D472C, 0x3F0CE39F,
    0xD02E9787, 0xC326F415, 0x9E135320, 0x150CB9E2,
    0xED94AFC7, 0x236EAB0F, 0x596807A0, 0x0BD61C36,
    0xA29E8F57, 0x0D8099A5, 0x520200EA, 0xD11FF96C,
    0x5FF47467, 0x575C0B39, 0x0FC89690, 0xB1FBACE8,
    0x7A957D16, 0xB54D9F76, 0x21DC77FB, 0x6DE85CF5,
    0xBFE7AEE9, 0xC49571A9, 0x7F1DE4DA, 0x29E03484,
    0x786BA455, 0xC26E2109, 0x4A0215F4, 0x44BFF99C,
    0x711A2414, 0xFDE9CDD0, 0xDCE15B77, 0x66D37887,
    0xF006CB92, 0x27429119, 0xF37B9784, 0x9BE182D9,
    0xF21B8C34, 0x732CAD2D, 0xAF8A6A60, 0x33A5D3AF,
    0x633E2688, 0x5EAB5FD1, 0x23E6017A, 0xAC27A7CF,
    0xF0FC5A0E, 0xCC857A5D, 0x20FB7B56, 0x3241F4CD,
    0xE132B8F7, 0x4BB37056, 0xDA1D5F94, 0x76E08321,
    0xE1936A9C, 0x876C99C3, 0x2B8A5877, 0xEB6E3836,
    0x9ED8A201, 0xB49B5122, 0xB1199638, 0xA0A4AF2B,
    0x15F50A42, 0x775F3759, 0x41291099, 0xB6131D94,
    0x9A563075, 0x224D1EB1, 0x12BB0FA2, 0xFF9BFC8C,
    0x58237F23, 0x98EF2A15, 0xD6BCCF8A, 0xB340DC66,
    0x0D7743F0, 0x13372812, 0x6279F82B, 0x4E45E519,
    0x98B4BE06, 0x71375BAE, 0x2173ED47, 0x14148267,
    0xB7AB85B5, 0xA875E314, 0x1372F18D, 0xFD105270,
    0xB83F161F, 0x5C175260, 0x44FFD49F, 0xD428C4F6,
    0x2C2002FC, 0xF2797BAF, 0xA3B20A4E, 0xB9BF1A89,
    0xE4ABA5E2, 0xC912C58D, 0x96516F9A, 0x51561E77
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __DRAGON_H_ */
