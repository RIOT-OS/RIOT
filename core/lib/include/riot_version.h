/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2017 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_internal
 * @{
 *
 * @file
 * @brief       Common macros and compiler attributes/pragmas configuration
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef RIOT_VERSION_H
#define RIOT_VERSION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def         RIOT_VERSION_NUM(major, minor, patch, extra)
 * @brief       Generates a 64 bit variable of a release version.
 *              Comparisons to this only apply to released branches
 *
 *              To define @p extra add a file `EXTRAVERSION` to the RIOT root
 *              with the content
 *
 *                  RIOT_EXTRAVERSION = <extra>
 *
 *              with `<extra>` being the number of your local version.
 *              This can be useful if you are maintaining a downstream
 *              release to base further work on.
 *
 * @warning     This is only intended to be used with external boards or
 *              modules.
 *              In-tree code must not make use of this macro.
 *
 * @param[in]   major   Mayor version of the release
 * @param[in]   minor   Minor version of the release
 * @param[in]   patch   Patch level of the release
 * @param[in]   extra   Extra version, user defined
 *
 * @returns     A machine readable version variable
 */
#define RIOT_VERSION_NUM(major, minor, patch, extra)   \
    (((0ULL + major) << 48) + ((0ULL + minor) << 32) + \
     ((0ULL + patch) << 16) + (extra))

#ifdef __cplusplus
}
#endif

#endif /* RIOT_VERSION_H */
/** @} */
