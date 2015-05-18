/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "arch/reboot_arch.h"
#include "cpu.h"

int reboot_arch(int mode)
{
	// Perform a software reset request.  This
    // will cause the microcontroller to reset; no further
    // code will be executed.
	SysCtlReset();

	// The microcontroller should have reset, so this should
	// never be reached.  Just in case, return -1 otherwise.
	return -1;
}
