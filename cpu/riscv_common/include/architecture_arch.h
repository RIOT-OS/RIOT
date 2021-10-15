/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_riscv_common
 * @{
 *
 * @file
 * @brief       Architecture details
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 */

#ifndef ARCHITECTURE_ARCH_H
#define ARCHITECTURE_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Doc is provided centrally in architecture.h, hide this from Doxygen */
#ifndef DOXYGEN
#ifdef CPU_MPFS
#define ARCHITECTURE_WORD_BITS      (64U)
#else
#define ARCHITECTURE_WORD_BITS      (32U)
#endif /* CPU_MPFS */
#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ARCHITECTURE_ARCH_H */
