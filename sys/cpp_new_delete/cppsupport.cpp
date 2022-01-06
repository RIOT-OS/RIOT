/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup cpp_new_delete
 * @{
 *
 * @file
 * @brief C++ runtime support functions
 *
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stdlib.h>

/**
 * @brief DSO handle
 *
 * This symbol is used by dynamic shared objects to identify them, but it is
 * somehow pulled in as a dependency by the compiler-generated global (static)
 * constructor code.
 */
void *__dso_handle __attribute__((weak)) = NULL;

/** @} */
