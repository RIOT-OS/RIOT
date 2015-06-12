/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam3s
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 */

#ifndef CPU_PERIPH_H_
#define CPU_PERIPH_H_

#include "sam3s.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 48 pin versions (A) with 34 GPIOs */
#if defined(__SAM3S1A__) || defined(__SAM3S2A__) || defined(__SAM3S4A__)
#define PIOA_NUMOF       (21)
#define PIOB_NUMOF       (13)
#define PIOC_NUMOF       (0)
/* 64 pin versions (B) with 47 GPIOs */
#elif defined(__SAM3S1B__) || defined(__SAM3S2B__) || defined(__SAM3S4B__)
#define PIOA_NUMOF       (32)
#define PIOB_NUMOF       (15)
#define PIOC_NUMOF       (0)
/* 100 pin versions (C) with 79 GPIOs */
#elif defined(__SAM3S1C__) || defined(__SAM3S2C__) || defined(__SAM3S4C__)
#define PIOA_NUMOF       (32)
#define PIOB_NUMOF       (15)
#define PIOC_NUMOF       (32)
#else
#warning "No parallel I/O controller configuration!"
#endif

/**
 * @brief   Define mandatory GPIO type for SAM3S MCUs
 * @{
 */
#define HAVE_GPIO_T
typedef uint8_t gpio_t;
/** @} */

/**
 * @brief   Mandatory function for defining a GPIO pin
 * @{
 */
#define GPIO(x, y)       ((32 * x) + y)

/**
 * @brief   Available PIO controllers on SAM3S
 */
enum {
    PA = 0,                 /**< Parallel I/O Controller A (PIOA) */
    PB = 1,                 /**< Parallel I/O Controller B (PIOB) */
#if defined(ID_PIOC)        /* only on C versions */
    PC = 2,                 /**< Parallel I/O Controller C (PIOC) */
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_H_ */
/** @} */
