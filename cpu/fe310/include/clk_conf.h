/*
 * Copyright (C) 2019-2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file
 * @brief       Clock configuration for the FE310 cpu
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_CONF_H
#define CLK_CONF_H

#include "macros/units.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Core Clock configuration
 * @{
 */
#ifndef CONFIG_USE_CLOCK_HFXOSC_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HFXOSC) || \
    IS_ACTIVE(CONFIG_USE_CLOCK_HFROSC_PLL) || \
    IS_ACTIVE(CONFIG_USE_CLOCK_HFROSC)
#define CONFIG_USE_CLOCK_HFXOSC_PLL         0
#else
#define CONFIG_USE_CLOCK_HFXOSC_PLL         1       /* Use PLL clocked by HFXOSC by default */
#endif
#endif /* CONFIG_USE_CLOCK_HFXOSC_PLL */

#ifndef CONFIG_USE_CLOCK_HFXOSC
#define CONFIG_USE_CLOCK_HFXOSC             0
#endif /* CONFIG_USE_CLOCK_HFXOSC */

#ifndef CONFIG_USE_CLOCK_HFROSC_PLL
#define CONFIG_USE_CLOCK_HFROSC_PLL         0
#endif /* CONFIG_USE_CLOCK_HFROSC_PLL */

#ifndef CONFIG_USE_CLOCK_HFROSC
#define CONFIG_USE_CLOCK_HFROSC             0
#endif /* CONFIG_USE_CLOCK_HFROSC */

#if CONFIG_USE_CLOCK_HFXOSC_PLL && \
    (CONFIG_USE_CLOCK_HFROSC_PLL || CONFIG_USE_CLOCK_HFROSC || \
     CONFIG_USE_CLOCK_HFXOSC)
#error "Cannot use HFXOSC_PLL with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HFXOSC && \
    (CONFIG_USE_CLOCK_HFROSC_PLL || CONFIG_USE_CLOCK_HFROSC || \
     CONFIG_USE_CLOCK_HFXOSC_PLL)
#error "Cannot use HFXOSC with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HFROSC_PLL && \
    (CONFIG_USE_CLOCK_HFXOSC_PLL || CONFIG_USE_CLOCK_HFXOSC || \
     CONFIG_USE_CLOCK_HFROSC)
#error "Cannot use HFROSC_PLL with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HFROSC && \
    (CONFIG_USE_CLOCK_HFXOSC_PLL || CONFIG_USE_CLOCK_HFXOSC || \
     CONFIG_USE_CLOCK_HFROSC_PLL)
#error "Cannot use HFROSC with other clock configurations"
#endif

#define CONFIG_CLOCK_PLL_R                  (1)     /* Divide input clock by 2, mandatory with HFXOSC */
#ifndef CONFIG_CLOCK_PLL_F
#define CONFIG_CLOCK_PLL_F                  (39)    /* Multiply REFR by 80, e.g 2 * (39 + 1) */
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#define CONFIG_CLOCK_PLL_Q                  (1)     /* Divide VCO by 2, e.g 2^1 */
#endif

#if CONFIG_USE_CLOCK_HFXOSC_PLL
#define CLOCK_PLL_INPUT_CLOCK               MHZ(16)
#define CLOCK_PLL_REFR                      (CLOCK_PLL_INPUT_CLOCK / \
                                             (CONFIG_CLOCK_PLL_R + 1))
#define CLOCK_PLL_VCO                       (CLOCK_PLL_REFR * \
                                             (2 * (CONFIG_CLOCK_PLL_F + 1)))
#define CLOCK_PLL_OUT                       (CLOCK_PLL_VCO / \
                                             (1 << CONFIG_CLOCK_PLL_Q))
#define CLOCK_CORECLOCK                     (CLOCK_PLL_OUT) /* 320000000Hz with the values used above */

/* Check PLL settings */
#if CLOCK_PLL_REFR != MHZ(8)
#error "Only R=2 can be used when using HFXOSC"
#endif
#if (CLOCK_PLL_VCO < MHZ(384)) || (CLOCK_PLL_VCO > MHZ(768))
#error \
    "VCO frequency must be in the range [384MHz - 768MHz], check the CLOCK_PLL_F value"
#endif
#if (CLOCK_PLL_OUT < MHZ(48)) || (CLOCK_PLL_OUT > MHZ(384))
#error \
    "PLL output frequency must be in the range [48MHz - 384MHz], check the CLOCK_PLL_Q value"
#endif

#elif CONFIG_USE_CLOCK_HFXOSC
#define CLOCK_CORECLOCK                     MHZ(16)
#endif

/*
   When using HFROSC input clock, the core clock cannot be computed from settings,
   in this case, coreclk() returns the configured CPU frequency.
 */
#ifndef CONFIG_CLOCK_DESIRED_FREQUENCY
#define CONFIG_CLOCK_DESIRED_FREQUENCY     MHZ(320)
#endif

#ifndef CONFIG_CLOCK_HFROSC_TRIM
#define CONFIG_CLOCK_HFROSC_TRIM           (6)      /* ~72000000Hz input freq */
#endif
#ifndef CONFIG_CLOCK_HFROSC_DIV
#define CONFIG_CLOCK_HFROSC_DIV            (1)      /* Divide by 2 */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CLK_CONF_H */
