/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp8266_olimex-mod
 * @brief       Board specific definitions for
 *              Olimex MOD-WIFI-ESP8266-DEV boards.
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    On-board LED configuration and control definitions
 * @{
 */
#define LED0_PIN    GPIO1   /**< GPIO1 is used as LED Pin */
#define LED0_ACTIVE (0)     /**< LED is low active */

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

/** @} */
