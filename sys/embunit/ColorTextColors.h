/*
 * Copyright (C) 2015 Janos Kutscherauer <noshky@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author Janos Kutscherauer <noshky@gmail.com>
 */
#ifndef EMBUNIT_COLORTEXTCOLORS_H
#define EMBUNIT_COLORTEXTCOLORS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Terminal color definitions
 * C<color>:    text color
 *
 * CDEFAULT:    default text color
 *
 * BG<color>:   background color
 *
 * BGDEFAULT:   default background color
 *
 * S<style>:    text style
 *
 * SDEFAULT:    default text style
 * @{
 */
#define CRED        "\033[31m"
#define CGREEN      "\033[32m"
#define CDEFAULT    "\033[39m"
#define BGRED       "\033[41m"
#define BGGREEN     "\033[42m"
#define BGDEFAULT   "\033[49m"
#define SBOLD       "\033[1m"
#define SDEFAULT    "\033[21m"
#define LINEFILL    "\033[K"

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif/* EMBUNIT_COLORTEXTCOLORS_H */
/** @} */
