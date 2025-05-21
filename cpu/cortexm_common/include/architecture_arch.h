/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Architecture details
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

/* Doc is provided centrally in architecture.h, hide this from Doxygen */
#ifndef DOXYGEN
#define ARCHITECTURE_WORD_BITS          (32U)
#define ARCHITECTURE_BREAKPOINT(value)  __BKPT(value)
#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
