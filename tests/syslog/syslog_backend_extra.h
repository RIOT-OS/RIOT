/*
 * Copyright (C) 2020 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 *
 * @brief       Syslog backend extra definitions
 *
 * @{
 * @file
 *
 * @author      Vincent Dupont <vincent.dupont@continental-its.com>
 */

#ifndef SYSLOG_BACKEND_EXTRA_H
#define SYSLOG_BACKEND_EXTRA_H

#include "syslog_backend.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Extra custom backend
 */
extern syslog_backend_t extra_backend;

/**
 * @brief   Extra custom backend address for backend array
 */
#define SYSLOG_BACKEND_EXTRA &extra_backend

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* SYSLOG_BACKEND_EXTRA_H */
