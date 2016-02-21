/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_kinetis_common_port Kinetis PORT
 * @ingroup     cpu_kinetis_common
 * @brief       Driver for Freescale Kinetis PORT module
 *
 * @{

 * @file
 * @brief       Interface definition for the Kinetis PORT driver.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef KINETIS_COMMON_PORT_H
#define KINETIS_COMMON_PORT_H

#include "cpu.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @note    This is meant for internal use in Kinetis peripheral drivers only
 *
 * @param[in] dev       pin to configure
 * @param[in] pushpull  pull functionality configuration
 * @param[in] af        alternate function to use
 */
int port_init(gpio_t dev, gpio_pp_t pushpull, port_af_t af);

#ifdef __cplusplus
}
#endif

#endif /* KINETIS_COMMON_PORT_H */
/** @} */
