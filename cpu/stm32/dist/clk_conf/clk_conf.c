/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       Compute clock constants for STM32F[2|4|7] CPUs
 *
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "clk_conf.h"

#define ENABLE_DEBUG 0
#if ENABLE_DEBUG
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...)
#endif

/**
 * @brief Check if N/P pair is valid
 *
 * Check if N/P (alternatively N/Q or N/R) pair is valid with given @p vco_in and
 * @p pll_out
 *
 * @param[in] n
 * @param[in] p
 * @param[in] vco_in
 * @param[in] pll_out
 *
 * @return 1 if pair is valid, 0 otherwise
 */
static int is_n_ok(const pll_cfg_t *cfg, unsigned n, unsigned p,
                   unsigned vco_in, unsigned pll_out)
{
    if (n >= cfg->min_n && n <= cfg->max_n &&
            vco_in * n >= cfg->min_vco_output && vco_in * n <= cfg->max_vco_output &&
            vco_in * n / p == pll_out) {
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * @brief Compute PLL factors
 *
 * @param[in] pll_in      PLL input frequency
 * @param[in] pll_p_out   PLL P output frequency (0 if P is not needed)
 * @param[in] pll_q_out   PLL Q output frequency (0 if Q is not needed)
 * @param[in] pll_r_out   PLL R output frequency (0 if R is not needed)
 * @param[in,out] m       M factor, can be preset (0, if it has to be calculated)
 * @param[out] n          N factor
 * @param[out] p          P factor
 * @param[out] q          Q factor
 * @param[out] r          R factor
 *
 * @return -1 if no P,N pair can be computed with given @p pll_in and @p pll_p_out
 * @return 1 if no Q can be computed, M, N and P are valid
 * @return 2 if no R can be computed, M, M and P are valid
 * @return 3 if no Q nor R can be computed, M, M and P are valid
 * @return 0 if M, N, P, Q, R are valid
 */
static int compute_pll(const pll_cfg_t *cfg, unsigned pll_in,
                       unsigned pll_p_out, unsigned pll_q_out, unsigned pll_r_out,
                       unsigned *m, unsigned *n,
                       unsigned *p, unsigned *q, unsigned *r)
{
    (void)pll_r_out;
    (void)r;

    int res = 0;
    unsigned vco_in;

    if (*m == 0) {
        unsigned found_m = 0;
        unsigned found_n;
        unsigned found_p;
        unsigned found_q;
        unsigned found_r;
        unsigned found_res;
        *m = cfg->min_m;
        while (*m <= cfg->max_m && (res = compute_pll(cfg, pll_in, pll_p_out,
                                                      pll_q_out, pll_r_out,
                                                      m, n, p, q, r)) != 0) {
            if (res > 0 && !found_m) {
                found_m = *m;
                found_n = *n;
                found_p = *p;
                found_q = *q;
                found_r = *r;
                found_res = res;
            }
            *m += cfg->inc_m;
        }
        if (res == 0) {
            return 0;
        }
        if (found_m) {
            *m = found_m;
            *n = found_n;
            *p = found_p;
            *q = found_q;
            *r = found_r;
            return found_res;
        }
        else {
            return -1;
        }
    }
    else {
        vco_in = pll_in / *m;
        DEBUG("M=%u, vco_in=%u\n", *m, vco_in);
    }

    if (*m < cfg->min_m || *m > cfg->max_m ||
            vco_in < cfg->min_vco_input || vco_in > cfg->max_vco_input) {
        DEBUG("Invalid M=%u\n", *m);
        return -1;
    }

    if (pll_p_out) {
        DEBUG("Computing P for freq=%u\n", pll_p_out);
        for (*p = cfg->max_p; *p >= cfg->min_p; *p -= cfg->inc_p) {
            *n = *p * pll_p_out / vco_in;
            DEBUG("Trying P=%u: N=%u\n", *p, *n);
            if (is_n_ok(cfg, *n, *p, vco_in, pll_p_out)) {
                DEBUG("Found M=%u, N=%u, P=%u\n", *m, *n, *p);
                break;
            }
        }
        if (*p < cfg->min_p) {
            *p += cfg->inc_p;
        }
        if (!is_n_ok(cfg, *n, *p, vco_in, pll_p_out)) {
            return -1;
        }
    }

    if (pll_q_out) {
        DEBUG("Computing Q for freq=%u\n", pll_q_out);
        for (*q = cfg->max_q; *q >= cfg->min_q; *q -= cfg->inc_q) {
            if (!pll_p_out) {
                *n = *q * pll_q_out / vco_in;
            }
            DEBUG("Trying Q=%u: N=%u\n", *q, *n);
            if (is_n_ok(cfg, *n, *q, vco_in, pll_q_out)) {
                DEBUG("Found M=%u, N=%u, Q=%u\n", *m, *n, *q);
                break;
            }
        }
        if (*q < cfg->min_q) {
            *q += cfg->inc_q;
        }
        if (!is_n_ok(cfg, *n, *q, vco_in, pll_q_out)) {
            *q = 0;
            res |= 1;
        }
    }

    /* todo, compute r */

    return res;
}

static void usage(char **argv)
{
    fprintf(stderr, "usage: %s <cpu_model> <coreclock> <hse_freq> <lse> [pll_i2s_src] "
                    "[pll_i2s_q_out] [pll_sai_q_out]\n", argv[0]);
}

#define HSI 0
#define HSE 1

int main(int argc, char **argv)
{
    int char_offset = 0;
    const unsigned int* stm32_model_p = stm32_f_model;
    const clk_cfg_t* stm32_clk_cfg_p = stm32_f_clk_cfg;
    int model_max = MODEL_F_MAX;
    if (argc < 2) {
        usage(argv);
        return 1;
    }

    if (strlen(argv[1]) < 9
            || !isdigit(argv[1][6])
            || !isdigit(argv[1][7])
            || !isdigit(argv[1][8])
            || ((argv[1][5] != 'f') && (argv[1][5] != 'F')
                /* && (argv[1][5] != 'l') && (argv[1][5] != 'L') */)) {
        if (strlen(argv[1]) < 10
            || !isdigit(argv[1][7])
            || !isdigit(argv[1][8])
            || !isdigit(argv[1][9])
            || ((argv[1][5] != 'm') && (argv[1][5] != 'M'))
            || ((argv[1][6] != 'p') && (argv[1][5] != 'p'))
            ) {
            fprintf(stderr, "Invalid model : %s\n", argv[1]);
            return 1;
        }

        char_offset = 1;
        stm32_model_p = stm32_model_mp;
        stm32_clk_cfg_p = stm32_mp_clk_cfg;
        model_max = MODEL_MP_MAX;
    }

    int model = atoi(argv[1] + 6 + char_offset);
    int i;
    for (i = 0; i < model_max; i++) {
        if (stm32_model_p[i] == model) {
            break;
        }
    }
    if (i == model_max) {
        fprintf(stderr, "Unsupported CPU model %s\n", argv[1]);
        return 1;
    }

    const clk_cfg_t *cfg = &stm32_clk_cfg_p[i];

    /* print help for given cpu */
    if (argc < 5) {
        usage(argv);
        fprintf(stderr, "Max values for stm32f%03d:\n", model);
        fprintf(stderr, "  Max coreclock: %u Hz\n"
                        "  Max APB1:      %u Hz\n"
                        "  Max APB2:      %u Hz\n",
                cfg->max_coreclock, cfg->max_apb1, cfg->max_apb2);
        fprintf(stderr, "Additional PLLs:\n"
                        "  PLL I2S: %d\n"
                        "  PLL SAI: %d\n"
                        "  Alternate 48MHz source: ",
                cfg->has_pll_i2s, cfg->has_pll_sai);
        if (cfg->has_alt_48MHz & ALT_48MHZ_I2S) {
            fprintf(stderr, "PLL I2S\n");
        }
        else if (cfg->has_alt_48MHz & ALT_48MHZ_SAI) {
            fprintf(stderr, "PLL SAI\n");
        }
        else {
            fprintf(stderr, "None\n");
        }
        return 0;
    }

    /* parse command line arguments */
    unsigned coreclock = atoi(argv[2]);
    unsigned pll_in = atoi(argv[3]);
    int pll_src;
    if (pll_in == 0) {
        pll_in = cfg->hsi;
        pll_src = HSI;
    }
    else {
        pll_src = HSE;
    }

    unsigned is_lse = atoi(argv[4]) ? 1 : 0;

    unsigned pll_i2s_input = 0;
    if (argc > 5) {
        pll_i2s_input = atoi(argv[5]);
    }

    unsigned pll_i2s_p_out = 0;
    unsigned pll_i2s_q_out = 0;
    if (argc > 6) {
        pll_i2s_q_out = atoi(argv[6]);
    }

    unsigned pll_sai_p_out = 0;
    unsigned pll_sai_q_out = 0;
    if (argc > 7) {
        pll_sai_q_out = atoi(argv[7]);
    }

    if (cfg->max_coreclock && coreclock > cfg->max_coreclock) {
        fprintf(stderr, "Invalid coreclock (max=%u)\n", cfg->max_coreclock);
        return 1;
    }

    fprintf(stderr, "Computing settings for stm32f%03d CPU...\n", model);

    unsigned m = 0;
    unsigned n = 0;
    unsigned p = 0;
    unsigned q = 0;
    unsigned r = 0;

    unsigned m_i2s = 0;
    unsigned n_i2s = 0;
    unsigned p_i2s = 0;
    unsigned q_i2s = 0;
    unsigned r_i2s = 0;

    unsigned m_sai = 0;
    unsigned n_sai = 0;
    unsigned p_sai = 0;
    unsigned q_sai = 0;
    unsigned r_sai = 0;

    bool use_alt_48MHz = false;
    unsigned clock_48MHz = cfg->need_48MHz ? 48000000U : 0;
    if ((cfg->hsi_prediv) && (pll_src == HSI)) {
        m = cfg->hsi_prediv;
    }

    /* main PLL */
    /* try to match coreclock with P output and 48MHz for Q output (USB) */
    switch (compute_pll(&cfg->pll, pll_in, coreclock, clock_48MHz, 0,
                        &m, &n, &p, &q, &r)) {
    case -1:
        /* no config available */
        fprintf(stderr, "Unable to compute main PLL factors\n");
        return 1;
    case 1:
        /* Q not OK */
        fprintf(stderr, "Need to use an alternate 48MHz src...");
        if (cfg->has_pll_i2s && (cfg->has_alt_48MHz & ALT_48MHZ_I2S) == ALT_48MHZ_I2S) {
            puts("PLL I2S");
            use_alt_48MHz = true;
            if (pll_i2s_q_out != 0 && pll_i2s_q_out != 48000000U) {
                fprintf(stderr, "Invalid PLL I2S Q output freq: %u\n", pll_i2s_q_out);
                return 1;
            }
            pll_i2s_q_out = 48000000U;
        }
        else if (cfg->has_pll_sai && (cfg->has_alt_48MHz & ALT_48MHZ_SAI)) {
            fprintf(stderr, "PLL SAI...");
            use_alt_48MHz = true;
            if ((cfg->has_alt_48MHz & ALT_48MHZ_P) &&
                    (pll_sai_p_out == 0 || pll_sai_p_out == 48000000U)) {
                fprintf(stderr, "P\n");
                pll_sai_p_out = 48000000U;
            }
            else if (!(cfg->has_alt_48MHz & ALT_48MHZ_P) &&
                     (pll_sai_q_out == 0 || pll_sai_q_out == 48000000U)) {
                fprintf(stderr, "Q\n");
                pll_sai_q_out = 48000000U;
            }
            else {
                if (cfg->has_alt_48MHz & ALT_48MHZ_P) {
                    fprintf(stderr, "Invalid PLL SAI P output freq: %u\n", pll_sai_p_out);
                } else {
                    fprintf(stderr, "Invalid PLL SAI Q output freq: %u\n", pll_sai_q_out);
                }
                return 1;
            }
        }
        else {
            fprintf(stderr, "No other source available\n");
            return 1;
        }
        break;
    default:
        break;
    }

    /* PLL I2S */
    if (pll_i2s_p_out || pll_i2s_q_out) {
        unsigned *_m;
        unsigned _in;
        if (cfg->has_pll_i2s_m) {
            _m = &m_i2s;
        }
        else {
            _m = &m;
        }
        if (cfg->has_pll_i2s_alt_input && pll_i2s_input) {
            _in = pll_i2s_input;
        }
        else {
            _in = pll_in;
        }
        if (compute_pll(&cfg->pll, _in, pll_i2s_p_out, pll_i2s_q_out, 0,
                        _m, &n_i2s, &p_i2s, &q_i2s, &r_i2s) != 0) {
            fprintf(stderr, "Unable to compute 48MHz output using PLL I2S\n");
            return 1;
        }
    }

    /* PLL SAI */
    if (pll_sai_p_out || pll_sai_q_out) {
        if (compute_pll(&cfg->pll, pll_in, pll_sai_p_out, pll_sai_q_out, 0,
                        &m_sai, &n_sai, &p_sai, &q_sai, &r_sai) != 0) {
            puts("Unable to compute 48MHz output using PLL I2S");
            return 1;
        }
        if (!cfg->has_pll_sai_m && m != m_sai) {
            m = m_sai;
            DEBUG("Retry to compute main PLL with M=%u\n", m);
            if (compute_pll(&cfg->pll, pll_in, coreclock, clock_48MHz, 0,
                            &m, &n, &p, &q, &r) < 0) {
                fprintf(stderr, "Unable to compute 48MHz output using PLL I2S\n");
                return 1;
            }
        }
    }

    /* APB prescalers */
    unsigned apb1_pre;
    unsigned apb2_pre;
    unsigned apb3_pre;

    for (apb1_pre = 1; apb1_pre <= 16; apb1_pre <<= 1) {
        if (coreclock / apb1_pre <= cfg->max_apb1) {
            break;
        }
    }
    if (cfg->family != STM32F0) {
        for (apb2_pre = 1; apb2_pre <= 16; apb2_pre <<= 1) {
            if (coreclock / apb2_pre <= cfg->max_apb2) {
                break;
            }
        }
    }
    if (cfg->family == STM32MP1) {
        for (apb3_pre = 1; apb3_pre <= 16; apb3_pre <<= 1) {
            if (coreclock / apb3_pre <= cfg->max_apb3) {
                break;
            }
        }
    }

    /* Print constants */
    fprintf(stderr, "==============================================================\n");
    fprintf(stderr, "Please copy the following code into your board's periph_conf.h\n\n");

    printf("/**\n"
           " * @name    Clock settings\n"
           " *\n"
           " * @note    This is auto-generated from\n"
           " *          `cpu/stm32_common/dist/clk_conf/clk_conf.c`\n"
           " * @{\n"
           " */\n");
    printf("/* give the target core clock (HCLK) frequency [in Hz],\n"
           " * maximum: %uMHz */\n", cfg->max_coreclock / 1000000U);
    printf("#define CLOCK_CORECLOCK     (%uU)\n", coreclock);
    printf("/* 0: no external high speed crystal available\n"
           " * else: actual crystal frequency [in Hz] */\n"
           "#define CLOCK_HSE           (%uU)\n", pll_src ? pll_in : 0);
    printf("/* 0: no external low speed crystal available,\n"
           " * 1: external crystal available (always 32.768kHz) */\n"
           "#define CLOCK_LSE           (%uU)\n", is_lse);
    printf("/* peripheral clock setup */\n");

    if (cfg->family != STM32MP1) {
    printf("#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1\n"
           "#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)\n");
    }
    if (cfg->family == STM32F0) {
        printf("#define CLOCK_APB1_DIV      RCC_CFGR_PPRE_DIV%u      /* max %uMHz */\n"
               "#define CLOCK_APB1          (CLOCK_CORECLOCK / %u)\n",
               apb1_pre, cfg->max_apb1 / 1000000U, apb1_pre);
        printf("#define CLOCK_APB2          (CLOCK_APB1)\n");
    }
    else if (cfg->family == STM32MP1) {
        /* TODO: Set to 1 by default, conf_clk is not able to handle this parameter */
        printf("#define CLOCK_MCU_DIV       RCC_MCUDIVR_MCUDIV_1     /* max %uMHz */\n"
               "#define CLOCK_MCU           (CLOCK_CORECLOCK / 1)\n",
               cfg->max_coreclock / 1000000U);
        printf("#define CLOCK_APB1_DIV      RCC_APB1DIVR_APB1DIV_%u     /* max %uMHz */\n"
               "#define CLOCK_APB1          (CLOCK_CORECLOCK / %u)\n",
               apb1_pre, cfg->max_apb1 / 1000000U, apb1_pre);
        printf("#define CLOCK_APB2_DIV      RCC_APB2DIVR_APB2DIV_%u     /* max %uMHz */\n"
               "#define CLOCK_APB2          (CLOCK_CORECLOCK / %u)\n",
               apb2_pre, cfg->max_apb2 / 1000000U, apb2_pre);
        printf("#define CLOCK_APB3_DIV      RCC_APB3DIVR_APB3DIV_%u     /* max %uMHz */\n"
               "#define CLOCK_APB3          (CLOCK_CORECLOCK / %u)\n",
               apb3_pre, cfg->max_apb3 / 1000000U, apb3_pre);
    }
    else {
        printf("#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV%u     /* max %uMHz */\n"
               "#define CLOCK_APB1          (CLOCK_CORECLOCK / %u)\n",
               apb1_pre, cfg->max_apb1 / 1000000U, apb1_pre);
        printf("#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV%u     /* max %uMHz */\n"
               "#define CLOCK_APB2          (CLOCK_CORECLOCK / %u)\n",
               apb2_pre, cfg->max_apb2 / 1000000U, apb2_pre);
    }
    if (cfg->family == STM32F0 || cfg->family == STM32F1 || cfg->family == STM32F3) {
        printf("\n/* PLL factors */\n");
        printf("#define CLOCK_PLL_PREDIV     (%u)\n", m);
        printf("#define CLOCK_PLL_MUL        (%u)\n", n);
    }
    else {
        printf("\n/* Main PLL factors */\n");
        printf("#define CLOCK_PLL_M          (%u)\n", m);
        printf("#define CLOCK_PLL_N          (%u)\n", n);
        printf("#define CLOCK_PLL_P          (%u)\n", p);
        printf("#define CLOCK_PLL_Q          (%u)\n", q);
    }

    if (pll_i2s_p_out || pll_i2s_q_out) {
        printf("\n/* PLL I2S configuration */\n");
        printf("#define CLOCK_ENABLE_PLL_I2S (1)\n");
        if (cfg->has_pll_i2s_alt_input && pll_i2s_input) {
            printf("#define CLOCK_PLL_I2S_SRC    (RCC_PLLI2SCFGR_PLLI2SSRC)\n");
        }
        else {
            printf("#define CLOCK_PLL_I2S_SRC    (0)\n");
        }
        if (cfg->has_pll_i2s_m) {
            printf("#define CLOCK_PLL_I2S_M      (%u)\n", m_i2s);
        }
        printf("#define CLOCK_PLL_I2S_N      (%u)\n", n_i2s);
        printf("#define CLOCK_PLL_I2S_P      (%u)\n", p_i2s);
        printf("#define CLOCK_PLL_I2S_Q      (%u)\n", q_i2s);
    }

    if (pll_sai_p_out || pll_sai_q_out) {
        printf("\n/* PLL SAI configuration */\n");
        printf("#define CLOCK_ENABLE_PLL_SAI (1)\n");
        if (cfg->has_pll_sai_m) {
            printf("#define CLOCK_PLL_SAI_M      (%u)\n", m_sai);
        }
        printf("#define CLOCK_PLL_SAI_N      (%u)\n", n_sai);
        printf("#define CLOCK_PLL_SAI_P      (%u)\n", p_sai);
        printf("#define CLOCK_PLL_SAI_Q      (%u)\n", q_sai);
    }

    if (use_alt_48MHz) {
        printf("\n/* Use alternative source for 48MHz clock */\n");
        printf("#define CLOCK_USE_ALT_48MHZ  (1)\n");
    }
    printf("/** @} */\n");

    return 0;
}
