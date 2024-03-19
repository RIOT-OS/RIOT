/*
* Copyright (C) 2022 Jens Wetterich <jens@wetterich-net.de>
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

/**
* @ingroup     pkg_etl
* @brief
* @{
*
* @file
* @brief       Common settings for etl
* @details     This activates the usage of the STL if available and/or the usage of
               workarounds for missing NAN macros.
*              This is configured by compiler defines the in Makefile.include
* @author      Jens Wetterich <jens@wetterich-net.de>
*/
#ifndef ETL_PROFILE_H
#define ETL_PROFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FEATURE_LIBSTDCPP
/**
 * @internal
 * @brief Deactivate the usage of the stdlib in ETL
 */
#define ETL_NO_STL
#endif

#ifdef NO_CPP_NAN_SUPPORT
/**
 * @internal
 * @brief Activate the ETL workarounds when NAN macros are not supported
 */
#define ETL_NO_CPP_NAN_SUPPORT
#endif

/**
 * @internal
 * @brief Activate the checking of bounds in push and pop operations in ETL containers
 */
#define ETL_CHECK_PUSH_POP

#ifdef __cplusplus
}
#endif
#endif /* ETL_PROFILE_H */
/** @} */
