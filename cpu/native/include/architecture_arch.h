/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup cpu_native
 * @{
 */

/**
 * @file
 * @brief Architecture details
 * @author Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief raise SIGTRAP
 *
 *        We must not include signal.h directly into RIOT application namespace.
 */
void native_breakpoint(void);

/* Doc is provided centrally in architecture.h, hide this from Doxygen */
#ifndef DOXYGEN
#  if (__SIZEOF_POINTER__ == 8)
#    define ARCHITECTURE_WORD_BITS      (64U)
#  else
#    define ARCHITECTURE_WORD_BITS      (32U)
#  endif
#  define ARCHITECTURE_BREAKPOINT(v)  native_breakpoint()
#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
