/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup   sys_entropy_source_common Entropy Source Common
 * @ingroup    sys_entropy_source
 * @brief      Common definitions and functions for entropy sources
 *
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <inttypes.h>
#include <assert.h>

/**
 * @brief   Entropy source error codes.
 */
typedef enum {
    ENTROPY_SOURCE_OK               =  0,   /**< Success */
    ENTROPY_SOURCE_ERR_INIT         = -1,   /**< Source initialization error */
    ENTROPY_SOURCE_ERR_CONFIG       = -2,   /**< Source configuration error */
    ENTROPY_SOURCE_ERR_TEST_REP     = -3,   /**< Repetition count test error */
    ENTROPY_SOURCE_ERR_TEST_PROP    = -4,   /**< Adaptive proportion test error */
    ENTROPY_SOURCE_ERR_TEST_BOTH    = -5,   /**< Repetition count and Adaptive
                                             *   proportion test error */
    ENTROPY_SOURCE_ERR_COND         = -6,   /**< Conditioning error */
} entropy_source_error_t;

/**
 * @brief   Data structure for Repetition Count Test (NIST SP 800-90B 4.4.1).
 */
typedef struct {
    uint8_t old_sample;     /**< Preceding sample to compare for repetition */
    uint16_t cnt_rep;       /**< Counter to count repetition */
    uint8_t c_rep;          /**< Cutoff threshold */
} entropy_source_tests_rep_t;

/**
 * @brief   Data structure for Adaptive Proportion Test (NIST SP 800-90B 4.4.2).
 */
typedef struct {
    uint8_t old_sample;     /**< Preceding sample to compare for repetition */
    uint16_t cnt_prop;      /**< Counter to count proportion */
    uint16_t cnt_window;    /**< Counter to count window size */
    uint16_t c_prop;        /**< Cutoff threshold */
} entropy_source_tests_prop_t;

/**
 * @brief   Scale Min. Entropy to fixed point integer to avoid float. The
 *          entropy per sample (8 Byte) of a noise source can likely be smaller
 *          than 1 bit.
 */
#define ENTROPY_SOURCE_HMIN_SCALE(x)    ((x * (1UL << 16)))

/**
 * @brief   Scale internal fixed point Min. Entropy back to float. This macro is
 *          not required and only there for convenience.
 */
#define ENTROPY_SOURCE_HMIN_SCALE_BACK(x)    ((float)x / (1UL << 16))

/**
 * @defgroup   sys_entropy_source_config Entropy Source compile configurations
 * @ingroup    config
 * @{
 */
/**
 * @brief   Window size for Adaptive Proportion Test (NIST SP 800-90B 4.4.2).
 *
 * In (NIST SP 800-90B 4.4.2) a window size of 512 samples is recommended for
 * non-binary sources. Typically, RIOT use cases will not request as many
 * samples, thus, it might be worth considering a smaller window size so the
 * test is more likely to complete a cycle. It is noteworthy that a cutoff value
 * calculated by @ref entropy_source_test_prop_cutoff that is greater than the
 * window size may lead to undetected errors.
 */
#ifndef CONFIG_ENTROPY_SOURCE_TESTS_WIN
#define CONFIG_ENTROPY_SOURCE_TESTS_WIN     (512)
#endif

/**
 * @brief   Abort factor for von Neumann extractor. The algorithms runs as long
 *          as no bit changes appear in subsequent samples. This define adds a
 *          factor that aborts the procedure after (factor * requested length)
 *          samples.
 */
#ifndef CONFIG_ENTROPY_SOURCE_NEUMANN_ABORT
#define CONFIG_ENTROPY_SOURCE_NEUMANN_ABORT (5)
#endif
/** @} */

/**
 * @brief   Get one sample of the entropy source.
 *
 * This function is typically used by the entropy source internally. A
 * conditioning component might need an interface to request a variable number
 * of samples, e.g., depending on the contained amount of entropy.
 *
 * @param[out] sample  pointer to write sample to.
 *
 * @return             ENTROPY_SOURCE_OK on success
 * @return             negative @ref entropy_source_error_t code on error
 */
typedef int (*entropy_source_sample_func_t)(uint8_t *sample);

/**
 * @brief   Applies von Neumann unbiasing.
 *
 * This function requests as many samples needed to create \p len unbiased bytes
 * using \p func. The algorithm compares bits of consecutive samples. Only bit
 * changes will be considered for the output value. An abort criterium stops
 * sampling after (len * CONFIG_ENTROPY_SOURCE_NEUMANN_ABORT) iterations.
 *
 * @warning This function has a non-deterministic runtime.
 *
 * @param[in] func   pointer to @ref entropy_source_sample_func_t function
                     that returns samples
 * @param[out] out   pointer to write unbiased bytes to
 * @param[in]  len   number of bytes to generate
 *
 * @return          ENTROPY_SOURCE_OK on success
 * @return          negative @ref entropy_source_error_t code on error
 */
int entropy_source_neumann_unbias(entropy_source_sample_func_t func,
                                  uint8_t *out, size_t len);

/**
 * @brief   Calculate cutoff value for Repetition Count Test (NIST SP 800-90B 4.4.1)
 *
 *~~~~
 * C = 1 + ( (-log2 a) / H)
 *~~~~
 *
 * C: Cutoff value.
 * H: Min. entropy of the source
 *    <a href="https://github.com/usnistgov/SP800-90B_EntropyAssessment">
 *    SP800-90B EntropyAssessment</a>.
 * a: Probability of type I error. We assume 2^(-20).
 *
 * @param[in] entropy_per_sample   Estimated min. entropy of one sample scaled
 *                                 by ENTROPY_SOURCE_HMIN_SCALE()
 *
 * @return                         Cutoff threshold
 */
static inline uint32_t entropy_source_test_rep_cutoff(uint32_t entropy_per_sample)
{
    return (1 + ((20 * 65536) / entropy_per_sample));
}

/**
 * @brief   Calculate cutoff value for Adaptive Proportion Test
 *          (NIST SP 800-90B 4.4.2)
 *
 * @param[in] entropy_per_sample   Estimated min. entropy of one sample scaled
 *                                 by ENTROPY_SOURCE_HMIN_SCALE()
 *
 * @return                         Cutoff value
 * @return                         ENTROPY_SOURCE_ERR_CONFIG if parameter
                                   invalid
 */
static inline int entropy_source_test_prop_cutoff(uint32_t entropy_per_sample)
{
    int ret;

    if (entropy_per_sample < 49152UL) {        /* 0.75 bit/sample */
        ret = 410;
    }
    else if (entropy_per_sample < 98304UL) {   /* 1.5 bit/sample */
        ret = 311;
    }
    else if (entropy_per_sample < 196608UL) {   /* 3 bit/sample */
        ret = 177;
    }
    else if (entropy_per_sample < 393216UL) {   /* 6 bit/sample */
        ret = 62;
    }
    else if (entropy_per_sample <= 524288UL) {  /* 8 bit/sample */
        ret = 13;
    }
    else {
        ret = ENTROPY_SOURCE_ERR_CONFIG;
    }

    return ret;
}

/**
 * @brief   Initialize structure for Repetition Count Test
 *
 * @param[in, out] state   Test structure of one entropy source.
 * @param[in]      c_rep   Cutoff value calculated by
 *                         @ref entropy_source_test_rep_cutoff.
 */
static inline void entropy_source_test_rep_init(
    entropy_source_tests_rep_t *state, uint16_t c_rep)
{
    assert(state != NULL);

    state->old_sample = 0;
    state->cnt_rep = 0;
    state->c_rep = c_rep;
}

/**
 * @brief   Initialize structure for Adaptive Proportion Test
 *
 * @param[in, out] state   Test structure of one entropy source.
 * @param[in]      c_prop  Cutoff value calculated by
 *                         @ref entropy_source_test_prop_cutoff.
 */
static inline void entropy_source_test_prop_init(
    entropy_source_tests_prop_t *state, uint16_t c_prop)
{
    assert(state != NULL);

    state->old_sample = 0;
    state->cnt_prop = 0;
    state->cnt_window = CONFIG_ENTROPY_SOURCE_TESTS_WIN;
    state->c_prop = c_prop;
}

/**
 * @brief   Performs Repetition Count Test (NIST SP 800-90B 4.4.1).
 *
 * This function will not block sampling. It only indicates detected errors.
 *
 * @param[in, out] state    Test structure of one entropy source.
 * @param[in]      sample   Current sample.
 *
 * @return          ENTROPY_SOURCE_OK on success
 * @return          ENTROPY_SOURCE_ERR_TEST_REP on detected weakness
 */
int entropy_source_test_rep(entropy_source_tests_rep_t *state, uint8_t sample);

/**
 * @brief   Performs Adaptive Proportion Test (NIST SP 800-90B 4.4.2).
 *
 * This function will not block the sampling. It only indicates detected errors.
 *
 * @param[in, out] state   Test structure of one entropy source.
 * @param[in]      sample  current sample.
 *
 * @return          ENTROPY_SOURCE_OK on success
 * @return          ENTROPY_SOURCE_ERR_TEST_PROP on detected weakness
 */
int entropy_source_test_prop(entropy_source_tests_prop_t *state,
                             uint8_t sample);

/**
 * @brief   Convenience function to perform @ref entropy_source_test_rep
 *          and @ref entropy_source_test_prop.
 *
 * This function will not block the sampling. It only indicates detected errors.
 *
 * @param[in, out] state_rep    Repetition Count test structure of one
 *                              entropy source.
 * @param[in, out] state_prop   Adaptive Proportion test structure of one
 *                              entropy source.
 * @param[in]      sample       Current sample.
 *
 * @return          ENTROPY_SOURCE_OK on success
 * @return          negative @ref entropy_source_error_t code on error
 */
static inline int entropy_source_test(entropy_source_tests_rep_t *state_rep,
                                      entropy_source_tests_prop_t *state_prop,
                                      uint8_t sample)
{
    int ret = ENTROPY_SOURCE_OK;

    if (entropy_source_test_rep(state_rep, sample) < 0) {
        ret = ENTROPY_SOURCE_ERR_TEST_REP;
    }
    if (entropy_source_test_prop(state_prop, sample) < 0) {
        /* If repetition count failed before, indicate that both tests failed */
        if (ret == ENTROPY_SOURCE_ERR_TEST_REP) {
            ret = ENTROPY_SOURCE_ERR_TEST_BOTH;
        }
        else {
            ret = ENTROPY_SOURCE_ERR_TEST_PROP;
        }
    }
    return ret;
}

#ifdef __cplusplus
}
#endif

/** @} */
