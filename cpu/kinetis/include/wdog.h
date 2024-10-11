/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_kinetis_wdog Kinetis WDOG
 * @ingroup     cpu_kinetis
 * @brief       Driver for Freescale WDOG device.
 *              There are different implementations of the Watchdog devices.
 *              Currently, it is only possible to turn the Watchdog off.
 *              It supports two types: an advanced Watchdog, mostly found
 *              on Kinetis MCUs with Cortex-M4 inside and a simpler
 *              COP Watchdog.
 *
 *              ### WDOG Configuration Example (for periph_conf.h) ###
 *              The driver attempts to determine the type of the WDOG.
 *              Also you can force the use of advanced WDOG:
 *
 *                  #define KINETIS_WDOG_ADVANCED    1
 *
 *              Or the use of COP WDOG:
 *
 *                  #define KINETIS_WDOG_ADVANCED    0
 *
 * @{

 * @file
 * @brief       Interface definition for the Kinetis WDOG driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef WDOG_H
#define WDOG_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Disable hardware watchdog.
 */
void wdog_disable(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* WDOG_H */
