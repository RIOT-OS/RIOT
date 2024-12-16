/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph
 * @defgroup    drivers_periph_sercom Low-level SERCOM driver implementation
 * @{
 *
 * @file
 * @brief       Low-level SERCOM driver implementation
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef SERCOM_INTERNAL_H
#define SERCOM_INTERNAL_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#if DOXYGEN
#  define       SERCOM_NUMOF NUM /**< Number of SERCOM instances */
#else
#  if defined(SERCOM7)
#    define     SERCOM_NUMOF 8
#  elif defined(SERCOM6)
#    define     SERCOM_NUMOF 7
#  elif defined(SERCOM5)
#    define     SERCOM_NUMOF 6
#  elif defined(SERCOM4)
#    define     SERCOM_NUMOF 5
#  elif defined(SERCOM3)
#    define     SERCOM_NUMOF 4
#  elif defined(SERCOM2)
#    define     SERCOM_NUMOF 3
#  elif defined(SERCOM1)
#    define     SERCOM_NUMOF 2
#  elif defined(SERCOM0)
#    define     SERCOM_NUMOF 1
#  else
#    error "No SERCOMs present according to vendor header files"
#  endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* SERCOM_INTERNAL_H */
/** @} */
