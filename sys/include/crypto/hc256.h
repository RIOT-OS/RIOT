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
 * @file        hc256.h
 * @brief       Headers for the implementation of the HC-256 stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Hongjun Wu
 *
 */

#ifndef __HC256_H_
#define __HC256_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The macro definitions
 */
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFF)
#define ROTL32(v, n) (U32V((v) << (n)) | ((v) >> (32 - (n))))
#define ROTR32(v, n) ROTL32(v, 32 - (n))
#define U32TO32_LITTLE(v) (v)

/**
 * @brief The HC-256 state context
 */
typedef struct {
    uint32_t T[2048];
    uint32_t X[16];
    uint32_t Y[16];
    uint32_t counter2048;
    uint32_t key[8];
    uint32_t iv[8];
    uint32_t keysize;
    uint32_t ivsize;
} hc256_ctx;

/* h1 function */
#define h1(ctx, x, y){				\
     uint32_t B0,B1,B2,B3;			\
     uint32_t t0,t1,t2;				\
     B0 = (uint8_t) (x);			\
     t0 = (ctx->T[1024 + B0]);			\
     B1 = (uint8_t) ((x) >> 8);			\
     t1 = t0+(ctx->T[1024 + 256 + B1]);		\
     B2 = (uint8_t) ((x) >> 16);		\
     t2 = t1+(ctx->T[1024 + 512 + B2]);		\
     B3 = (uint8_t)((x) >> 24);			\
     (y) =  t2 + ((ctx)->T[1024 + 768 + B3]);	\
}

/* h2 function */
#define h2(ctx, x, y){			\
     uint32_t B0,B1,B2,B3;		\
     uint32_t t0,t1,t2;			\
     B0 = (uint8_t) (x);		\
     t0 = (ctx->T[B0]);			\
     B1 = (uint8_t) ((x) >> 8);		\
     t1 = t0 + (ctx->T[256 + B1]);	\
     B2 = (uint8_t) ((x) >> 16);	\
     t2 = t1 + (ctx->T[512 + B2]);	\
     B3 = (uint8_t)((x) >> 24);		\
     (y) =  t2 + ((ctx)->T[768 + B3]);	\
}

/* one step of HC-256, update P and generate 32 bits keystream */
#define step_P(ctx,u,v,a,b,c,d,m){					\
     uint32_t tem0,tem1,tem2,tem3;					\
     tem0 = ROTR32((ctx->T[(v)]),23);					\
     tem1 = ROTR32((ctx->X[(c)]),10);					\
     tem2 = ((ctx->T[(v)]) ^ (ctx->X[(c)])) & 0x3ff;			\
     (ctx->T[(u)]) += (ctx->X[(b)])+(tem0^tem1)+(ctx->T[1024+tem2]);	\
     (ctx->X[(a)]) = (ctx->T[(u)]);					\
     h1((ctx),(ctx->X[(d)]),tem3);					\
     (m) = tem3 ^ (ctx->T[(u)]);					\
}

/* one step of HC-256, update Q and generate 32 bits keystream */
#define step_Q(ctx,u,v,a,b,c,d,m){				\
     uint32_t tem0,tem1,tem2,tem3;				\
     tem0 = ROTR32((ctx->T[(v)]),23);				\
     tem1 = ROTR32((ctx->Y[(c)]),10);				\
     tem2 = ((ctx->T[(v)]) ^ (ctx->Y[(c)])) & 0x3ff;		\
     (ctx->T[(u)]) += (ctx->Y[(b)])+(tem0^tem1)+(ctx->T[tem2]);	\
     (ctx->Y[(a)]) = (ctx->T[(u)]);				\
     h2((ctx),(ctx->Y[(d)]),tem3);				\
     (m) = tem3 ^ (ctx->T[(u)]);				\
}

#define f1(x)  (ROTR32((x),7) ^ ROTR32((x),18) ^ ((x) >> 3))
#define f2(x)  (ROTR32((x),17) ^ ROTR32((x),19) ^ ((x) >> 10))

/* update table P */
#define update_P(ctx,u,v,a,b,c){					\
     uint32_t tem0,tem1,tem2;						\
     tem0 = ROTR32((ctx->T[(v)]),23);					\
     tem1 = ROTR32((ctx->X[(c)]),10);					\
     tem2 = ((ctx->T[(v)]) ^ (ctx->X[(c)])) & 0x3ff;			\
     (ctx->T[(u)]) += (ctx->X[(b)])+(tem0^tem1)+(ctx->T[1024+tem2]);	\
     (ctx->X[(a)]) = (ctx->T[(u)]);					\
}

/* update table Q */
#define update_Q(ctx,u,v,a,b,c){				\
     uint32_t tem0,tem1,tem2;					\
     tem0 = ROTR32((ctx->T[(v)]),23);				\
     tem1 = ROTR32((ctx->Y[(c)]),10);				\
     tem2 = ((ctx->T[(v)]) ^ (ctx->Y[(c)])) & 0x3ff;		\
     (ctx->T[(u)]) += (ctx->Y[(b)])+(tem0^tem1)+(ctx->T[tem2]);	\
     (ctx->Y[(a)]) = (ctx->T[(u)]);				\
}

/**
 * @brief  HC-256 Key setup
 *
 * @param[in]   ctx         HC-256 context
 * @param[in]   key         Key to setup the ctx
 * @param[in]   keysize     Size of the key
 * @param[in]   ivsize      Size of the IV
 */
void hc256_keysetup(hc256_ctx *ctx, const uint8_t *key, uint32_t keysize, uint32_t ivsize);

/**
 * @brief  HC-256 IV setup
 *
 * @param[in]   ctx         HC-256 context
 * @param[in]   iv          IV to setup the ctx
 */
void hc256_ivsetup(hc256_ctx *ctx, const uint8_t *iv);

/**
 * @brief  Encrypt or decrypt "msglen" number of message bytes
 *
 * @param[in]   ctx         HC-256 context
 * @param[in]   input       (Plain/Cipher)text for (en/de)crypting
 * @param[out]  output      (Cipher/Plain)text for (en/de)crypting
 * @param[in]   msglen      Number of bytes to (en/de)crypt
 */
void hc256_process_bytes(hc256_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __HC256_H_ */
