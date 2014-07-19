/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_imx6
 * @{
 *
 * @file        lpm_arch.c
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Leon George <l.george@fu-berlin.de>
 *
 * @}
 */

#include "arch/lpm_arch.h"

__attribute__((section(".noinit")))
static enum lpm_mode lpm;

void lpm_arch_init(void)
{
    // TODO
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    // TODO
    return 0;
}

enum lpm_mode lpm_arch_get(void)
{
    // TODO
    return 0;
}

void lpm_arch_awake(void)
{
    // TODO
}

void lpm_arch_begin_awake(void)
{
    // TODO
}

void lpm_arch_end_awake(void)
{
    // TODO
}

// TODO
enum lpm_mode lpm_set(enum lpm_mode target) {
  enum lpm_mode last_lpm = lpm;

  lpm = target;

  return last_lpm;
}
