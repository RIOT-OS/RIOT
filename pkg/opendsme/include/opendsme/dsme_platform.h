/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_opendsme
 *
 * @{
 *
 * @file
 *
 * @author      José I. Álamos <jose.alamos@haw-hamburg.de>
 */

#include <assert.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT(x) assert(x)         /**< openDSME assert macro implementation */
#define DSME_ASSERT(x) assert(x)    /**< openDSME DSME assert macro implementation */
#define DSME_SIM_ASSERT(x)          /**< not used */

#define LOG_INFO(x)                 /**< not used */
#define LOG_INFO_PURE(x)            /**< not used */
#define LOG_INFO_PREFIX             /**< not used */
#define LOG_ERROR(x)                /**< not used */

#define HEXOUT std::hex             /**< openDSME HEXOUT macro implementation */
#define DECOUT std::dec             /**< openDSME DECout macro implementation */

#define LOG_ENDL std::endl          /**< openDSME log endline implementation */

#define LOG_DEBUG(x)                /**< not used */
#define LOG_DEBUG_PURE(x)           /**< not used */
#define LOG_DEBUG_PREFIX            /**< not used */

#ifdef __cplusplus
}
#endif

/** @} */
