/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "reboot.h"
#include "cpu.h"
#include "log.h"

void reboot(void)
{
    LOG_INFO("RIOT rebooting...\n");
    NVIC_SystemReset();
}
