/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_arduino
 * @{
 *
 * @file
 * @brief       Wrapper to access the definition of the Arduino 'SPI' interface
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef MODULE_PERIPH_SPI
#error "No SPI support on your board"
#endif

#ifdef __cplusplus
#include "spiport.hpp"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARDUINO_SPI_INTERFACE
#define ARDUINO_SPI_INTERFACE 0 /**< Number of the SPI dev to make available to Arduino code */
#endif

#ifdef __cplusplus
}
#endif

/** @} */
