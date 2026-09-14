/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
