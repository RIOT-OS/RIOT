/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_eepreg
 *
 * @{
 *
 * @file
 * @brief       EEPROM registration example registry
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#ifndef EEPREG_CONF_H
#define EEPREG_CONF_H

#include "eepreg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Example size macros
 *
 * In practice, these would probably be defined in foo.h and bar.h.
 */
#define FOO_EEPREG_SIZE    (1U)
#define BAR_EEPREG_SIZE    (1U)

/**
 * @brief   Example eepreg entry number definitions
 */
enum {
    FOO_EEPREG_NUM,
    BAR_EEPREG_NUM,
};

/**
 * @brief   Example sizes array
 */
static const eepreg_t eepreg_sizes[] =
{
    FOO_EEPREG_SIZE,
    BAR_EEPREG_SIZE,
};

/**
 * @brief   Example eepreg names (used as magic numbers)
 */
static const char *eepreg_names[] =
{
    "FOO",
    "BAR",
};

#ifdef __cplusplus
}
#endif

#endif /* EEPREG_CONF_H */
/** @} */
