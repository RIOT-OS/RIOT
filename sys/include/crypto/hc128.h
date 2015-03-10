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
 * @brief       Headers for the implementation of the HC-128 stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Hongjun Wu
 *
 */

#ifndef __HC128_H_
#define __HC128_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Bit arithmetic macros
 * @{
 */
#define U32V(v) ((uint32_t)(v) & 0xFFFFFFFF)
#define ROTL32(v, n) (U32V((v) << (n)) | ((v) >> (32 - (n))))
#define ROTR32(v, n) ROTL32(v, 32 - (n))
#define U32TO32_LITTLE(v) (v)
/** @} */

/**
 * @brief The HC-128 state context
 */
typedef struct {
    uint32_t T[1024]; /**< T array*/
    uint32_t X[16]; /**< X array */
    uint32_t Y[16]; /**< Y array */
    uint32_t counter1024; /**< Counter variable */
    uint32_t key[8]; /**< Key */
    uint32_t iv[8]; /**< IV */
    uint32_t keysize; /**< Key size */
    uint32_t ivsize; /**< IV size */
} hc128_ctx;


/**
 * @name Algorithm macros
 * @{
 */
/* h1 function */
#define h1(ctx, x, y) {			\
     uint32_t B0,B2,t0;			\
     B0 = (uint8_t) (x);		\
     t0 = (ctx->T[512+B0]);		\
     B2 = (uint8_t) ((x) >> 16);	\
     y = t0 + (ctx->T[512+256+B2]);	\
}

/* h2 function */
#define h2(ctx, x, y) {			\
     uint32_t B0,B2,t0;			\
     B0 = (uint8_t) (x);		\
     t0 = (ctx->T[B0]);			\
     B2 = (uint8_t) ((x) >> 16);	\
     y = t0 + (ctx->T[256+B2]);		\
}

/* one step of HC-128, update P and generate 32 bits keystream */
#define step_P(ctx,u,v,a,b,c,d,n){		\
     uint32_t tem0,tem1,tem2,tem3;		\
     h1((ctx),(ctx->X[(d)]),tem3);		\
     tem0 = ROTR32((ctx->T[(v)]),23);		\
     tem1 = ROTR32((ctx->X[(c)]),10);		\
     tem2 = ROTR32((ctx->X[(b)]),8);		\
     (ctx->T[(u)]) += tem2+(tem0 ^ tem1);	\
     (ctx->X[(a)]) = (ctx->T[(u)]);		\
     (n) = tem3 ^ (ctx->T[(u)]);		\
}

/* one step of HC-128, update Q and generate 32 bits keystream */
#define step_Q(ctx,u,v,a,b,c,d,n){		\
     uint32_t tem0,tem1,tem2,tem3;		\
     h2((ctx),(ctx->Y[(d)]),tem3);		\
     tem0 = ROTR32((ctx->T[(v)]),(32-23));	\
     tem1 = ROTR32((ctx->Y[(c)]),(32-10));	\
     tem2 = ROTR32((ctx->Y[(b)]),(32-8));	\
     (ctx->T[(u)]) += tem2 + (tem0 ^ tem1);	\
     (ctx->Y[(a)]) = (ctx->T[(u)]);		\
     (n) = tem3 ^ (ctx->T[(u)]);		\
}

#define f1(x)  (ROTR32((x),7) ^ ROTR32((x),18) ^ ((x) >> 3))
#define f2(x)  (ROTR32((x),17) ^ ROTR32((x),19) ^ ((x) >> 10))

/* update table P */
#define update_P(ctx,u,v,a,b,c,d){				\
     uint32_t tem0,tem1,tem2,tem3;				\
     tem0 = ROTR32((ctx->T[(v)]),23);				\
     tem1 = ROTR32((ctx->X[(c)]),10);				\
     tem2 = ROTR32((ctx->X[(b)]),8);				\
     h1((ctx),(ctx->X[(d)]),tem3);				\
     (ctx->T[(u)]) = ((ctx->T[(u)]) + tem2+(tem0^tem1)) ^ tem3; \
     (ctx->X[(a)]) = (ctx->T[(u)]);				\
}

/* update table Q */
#define update_Q(ctx,u,v,a,b,c,d){				\
     uint32_t tem0,tem1,tem2,tem3;				\
     tem0 = ROTR32((ctx->T[(v)]),(32-23));			\
     tem1 = ROTR32((ctx->Y[(c)]),(32-10));			\
     tem2 = ROTR32((ctx->Y[(b)]),(32-8));			\
     h2((ctx),(ctx->Y[(d)]),tem3);				\
     (ctx->T[(u)]) = ((ctx->T[(u)]) + tem2+(tem0^tem1)) ^ tem3;	\
     (ctx->Y[(a)]) = (ctx->T[(u)]);				\
}
/** @} */

/**
 * @brief  HC-128 Key setup
 *
 * @param[in]   ctx         HC-128 context
 * @param[in]   key         Key to setup the ctx
 * @param[in]   keysize     Size of the key
 * @param[in]   ivsize      Size of the IV
 */
void hc128_keysetup(hc128_ctx *ctx, const uint8_t *key, uint32_t keysize, uint32_t ivsize);

/**
 * @brief  HC-128 IV setup
 *
 * @param[in]   ctx         HC-128 context
 * @param[in]   iv          IV to setup the ctx
 */
void hc128_ivsetup(hc128_ctx *ctx, const uint8_t *iv);

/**
 * @brief  Encrypt or decrypt "msglen" number of message bytes
 *
 * @param[in]   ctx         HC-128 context
 * @param[in]   input       (Plain/Cipher)text for (en/de)crypting
 * @param[out]  output      (Cipher/Plain)text for (en/de)crypting
 * @param[in]   msglen      Number of bytes to (en/de)crypt
 */
void hc128_process_bytes(hc128_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __HC128_H_ */
