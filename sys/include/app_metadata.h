/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_app_metadata app_metadata
 * @ingroup     sys
 * @brief       Module for the application metadata
 * @{
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief   Application Shell Format is an optional application metadata
 *          parameter intended to help coordinate any specific formats that
 *          are being used.
 *
 * @details An example is if the application is following a specific format,
 *          say semantics defined in a RDM or schema, that could be specified
 *          by adding APP_SHELL_FMT="RDM001_v1", that would inform anything
 *          using the shell that the formatting should follow what is dictated.
 * @note    This define is only for documentation, to use the APP_SHELL_FMT
 *          add it to the application makefile with APP_SHELL_FMT=your_value
 *          or when calling make such as `APP_SHELL_FMT=your_value make flash`
 */
#define APP_SHELL_FMT
#endif

/**
 * @brief   Prints the application metadata in json.
 *
 * @details Examples of application metadata are BOARD, OS_VERSION, APP_NAME...
 */
void app_metadata_print_json(void);

#ifdef __cplusplus
}
#endif

/** @} */
