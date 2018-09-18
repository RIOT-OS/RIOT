/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tsl4531x
 * @{
 *
 * @file
 * @brief       SAUL interface for TSL4531 Luminosity sensor.
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#ifndef TSL4531x_SAUL_H
#define TSL4531x_SAUL_H

#include "saul.h"

/** SAUL-compatible structure for illuminance sensor
 */
const saul_driver_t tsl4531x_saul_driver;

#endif /* TSL4531x_SAUL_H */

