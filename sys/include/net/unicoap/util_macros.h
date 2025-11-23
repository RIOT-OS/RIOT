/*
 * SPDX-FileCopyrightText: 2024-2025 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include "modules.h"
#include "macros/utils.h"

/**
 * @file
 * @brief   Utility macros for `unicoap`
 * @ingroup net_unicoap
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#ifndef DOXYGEN
#  define UNICOAP_CODE_CLASS_DETAIL_FORMAT "%u.%02u"

#  define _CONCAT3(a, b, c)                      CONCAT3(a, b, c)
#endif

#ifdef __cplusplus
extern "C" {}
#endif
