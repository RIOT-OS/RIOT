/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_msp430_common
 * @{
 *
 * @file
 * @brief       stdlib.h wrapper for MSP430
 * @see http://www.cplusplus.com/reference/cstdlib/
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se
 */

#ifndef STDLIB_H
#define STDLIB_H

/*
 * The MSP430 toolchain does not provide malloc, free, calloc etc. in stdlib.h.
 * As a workaround, we include the system stdlib.h here and then add malloc.
 */

#include_next <stdlib.h>

#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* STDLIB_H */
/** @} */
