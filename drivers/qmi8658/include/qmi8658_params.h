/*
 * Copyright (C) 2026 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_qmi8658
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

#include "board.h"
#include "qmi8658.h"
#include "qmi8658_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef QMI8658_PARAM_PARAM1
#define QMI8658_PARAM_PARAM1
#endif

#ifndef QMI8658_PARAMS
#define QMI8658_PARAMS
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const qmi8658_params_t qmi8658_params[] =
{
    QMI8658_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
