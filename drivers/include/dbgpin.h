/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_dbgpin Direct pin control for debugging/profiling
 * @ingroup     sys
 *
 * This module gives the possibility to directly control a number of GPIO pins
 * without the least possible overhead for debugging and profiling purposes. For
 * this, the module provides a number of macros for setting, clearing, and
 * toggling some defined pins: `DBGPIN[\d]_[SET|CLR|TGL]`, for example
 * `DBGPIN0_SET` or `DBGPIN1_TGL`.
 *
 * To use this module, you have to do two things:
 * 1. the module has to be selected in your project: `USEMODULE += dbgpin`
 * 2. you have to define the pins you want to use for debugging, e.g.:
 *    `CFLAGS += -DDBGPIN0=GPIO_PIN\(0,15\) -DDBGPIN1=GPIO_PIN\(1,24\)`
 *
 * It is of course up to you to add these settings to your application Makefile
 * for the duration of debugging/profiling or to set them through environment
 * variables in your host systems shell.
 *
 * The initialization function is automatically called as early as possible
 * during the system initialization so the debug pins can be used already during
 * e.g. auto_init and so on.
 *
 * @{
 * @file
 * @brief       Module for direct pin control for debugging/profiling purposes
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef DBGPIN_H
#define DBGPIN_H

#include "dbgpin_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize all configured debug pins
 */
void dbgpin_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DBGPIN_H */
/** @} */

