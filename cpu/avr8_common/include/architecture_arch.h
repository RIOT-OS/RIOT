/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_avr8_common
 * @{
 *
 * @file
 * @brief       Architecture details
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Doc is provided centrally in platform.h, hide this from Doxygen */
#ifndef DOXYGEN
#define ARCHITECTURE_WORD_BITS      (8U)

#define ARCHITECTURE_LARGE_TXT_PTR  1
typedef uint32_t   uinttxtptr_t;
#define PRIxTXTPTR PRIx32
#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
