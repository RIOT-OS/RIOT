/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup   sys_entropy_source_zero Zero Entropy Source
 * @ingroup    sys_entropy_source
 * @brief      Zero Entropy Source for testing.
 *
 * This module produces zeros only and should be used for testing purposes only.
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#ifndef ENTROPY_SOURCE_ZERO_ENTROPY_H
#define ENTROPY_SOURCE_ZERO_ENTROPY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stddef.h>

/**
 * @ingroup    sys_entropy_source_config
 * @{
 */

/**
 * @brief   Min. Entropy value for zero entropy module.
 *
 * H_min=0.9 bit/sample * 2^16 fake for testing!
 */
#if !defined(CONFIG_KCONFIG_USEMODULE_ENTROPY_SOURCE_ZERO_ENTROPY) || defined(DOXYGEN)
#ifndef CONFIG_ENTROPY_SOURCE_ZERO_ENTROPY_HMIN
#define CONFIG_ENTROPY_SOURCE_ZERO_HMIN         (58982)
#endif

/**
 * @brief   Enable health test by default.
 *
 * Testing is the only purpose of this module.
 */
#ifndef CONFIG_ENTROPY_SOURCE_ZERO_HEALTH_TEST
#define CONFIG_ENTROPY_SOURCE_ZERO_HEALTH_TEST  1
#endif

/**
 * @brief   Disable conditioning by default.
 *
 * Conditioning is useless for zeros only. The von Neumann extractor would
 * never finish and wait for the stop criterion given by
 * @ref CONFIG_ENTROPY_SOURCE_NEUMANN_ABORT.
 */
#ifndef CONFIG_ENTROPY_SOURCE_ZERO_COND
#define CONFIG_ENTROPY_SOURCE_ZERO_COND         0
#endif
#endif /* !CONFIG_KCONFIG_USEMODULE_ENTROPY_SOURCE_ZERO_ENTROPY || DOXYGEN */
/** @} */

/**
 * @brief   Initializes test structures, if tests are enabled.
 *
 * @return          ENTROPY_SOURCE_OK always
 */
int entropy_source_zero_init(void);

/**
 * @brief   Generates zeros.
 *
 * @param[out] buf   pointer to write zeros to
 * @param[in]  len   number of bytes to generate
 *
 * @return          ENTROPY_SOURCE_OK on success
 * @return          negative @ref entropy_source_error_t code on error
 */
int entropy_source_zero_get(uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ENTROPY_SOURCE_ZERO_ENTROPY_H */
