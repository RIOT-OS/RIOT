/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_progress_bar
 * @{
 *
 * @file
 * @brief       A simple CLI progress bar
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdlib.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Progress bar maximum characters length
 */
#ifndef CONFIG_PROGRESS_BAR_LENGTH
#define CONFIG_PROGRESS_BAR_LENGTH                     (25U)
#endif

/**
 * @brief   Progress bar character
 */
#ifndef CONFIG_PROGRESS_BAR_FULL_CHARACTER
#define CONFIG_PROGRESS_BAR_FULL_CHARACTER             "█"
#endif

/**
 * @brief   Progress bar empty character
 */
#ifndef CONFIG_PROGRESS_BAR_EMPTY_CHARACTER
#define CONFIG_PROGRESS_BAR_EMPTY_CHARACTER            " "
#endif

/**
 * @brief   Character displayed on the left of the progress bar
 */
#ifndef CONFIG_PROGRESS_BAR_PREFIX_CHARACTER
#define CONFIG_PROGRESS_BAR_PREFIX_CHARACTER           "|"
#endif

/**
 * @brief   Character displayed on the left of the progress bar
 */
#ifndef CONFIG_PROGRESS_BAR_SUFFIX_CHARACTER
#define CONFIG_PROGRESS_BAR_SUFFIX_CHARACTER           "|"
#endif

/**
 * @brief   Progress bar prefix max length
 */
#ifndef CONFIG_PROGRESS_BAR_PREFIX_MAX_LENGTH
#define CONFIG_PROGRESS_BAR_PREFIX_MAX_LENGTH          (32U)
#endif

/**
 * @brief   Progress bar suffix max length
 */
#ifndef CONFIG_PROGRESS_BAR_SUFFIX_MAX_LENGTH
#define CONFIG_PROGRESS_BAR_SUFFIX_MAX_LENGTH          (32U)
#endif

/**
 * @brief   Progress bar descriptor
 */
typedef struct {
    /**  Current value of the progress bar. Must be between 0 and 100 (included) */
    uint8_t value;
    /** Prefix displayed on the left of the progress bar */
    char prefix[CONFIG_PROGRESS_BAR_PREFIX_MAX_LENGTH];
    /** Suffix displayed on the right of the progress bar */
    char suffix[CONFIG_PROGRESS_BAR_SUFFIX_MAX_LENGTH];
} progress_bar_t;

/**
 * @brief   Print a progress bar in the terminal
 *
 * @param[in] prefix                String displayed on the left of the progress bar
 * @param[in] suffix                String displayed on the right of the progress bar
 * @param[in] value                 Value of the progress bar
 */
void progress_bar_print(char *prefix, char *suffix, uint8_t value);

/**
 * @brief   Update the progress bar display in the terminal
 *
 * @param[in] progress_bar          Pointer to the progress bar descriptor
 */
void progress_bar_update(progress_bar_t *progress_bar);

/**
 * @brief   Prepare the output for displaying multiple progress bars.
 *
 * This function is just adding enough empty lines to give enough space to
 * print the list of progress bars.
 *
 * This function must be called only once and before starting the progress bar
 * list updates with.
 *
 * @param[in] len                   The length of the progress bar array
 */
void progress_bar_prepare_multi(uint8_t len);

/**
 * @brief   Update all progress bar displays of the given progress bars list
 *
 * @param[in]   progress_bar_list   An array of progress bars
 * @param[in]   len                 The length of the progress bar array
 */
void progress_bar_update_multi(progress_bar_t *progress_bar_list, uint8_t len);

#ifdef __cplusplus
}
#endif

/** @} */
