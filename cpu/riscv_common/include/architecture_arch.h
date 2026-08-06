/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_fe310
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
#  if __riscv_xlen == 64
#    define ARCHITECTURE_WORD_BITS  (64U)
#  else
#    define ARCHITECTURE_WORD_BITS  (32U)
#  endif
#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
