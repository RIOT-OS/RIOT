/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     msp430
 * @{
 *
 * @file
 * @brief       stdio.h wrapper for MSP430
 * @see http://www.cplusplus.com/reference/cstdio/
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se
 */

#ifndef RIOT_MSP430_STDIO_H_
#define RIOT_MSP430_STDIO_H_

/*
 * msp430-libc does not provide getchar in stdio.h, but newlib does.
 * As a workaround, we declare getchar separately and include the system stdio.h
 */

int getchar(void);

#include_next <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* RIOT_MSP430_STDIO_H_ */
