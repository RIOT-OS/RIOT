/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 */

#ifndef CPU_H_
#define CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <inttypes.h>

#include "irq.h"

/* This file must exist else RIOT won't compile */

static inline void cpu_init_early(void)
{
}

static inline void cpu_print_last_instruction(void)
{
}

void board_init(void);

#ifdef __cplusplus
}
#endif

#endif
