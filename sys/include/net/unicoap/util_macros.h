/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
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
