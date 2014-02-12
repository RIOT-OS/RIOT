/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file        attributes.h
 * @brief       Compiler attributes/pragmas configuration
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef ATTRIBUTES_H_
#define ATTRIBUTES_H_

#ifdef __GNUC__
#define NORETURN  __attribute__((noreturn))
#else
#define NORETURN
#endif

#ifdef __GNUC__
#define CONST  __attribute__((const))
#else
#define CONST
#endif

#ifdef __GNUC__
#define PURE  __attribute__((pure))
#else
#define PURE
#endif

/** @} */
#endif /* ATTRIBUTES_H_ */

