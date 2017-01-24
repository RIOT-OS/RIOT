/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f303vg
 * @{
 *
 * @file        cpu-conf.h
 * @brief       CPU specific kernel configuration for the STM32F303VC MCU.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CPU_CONF
#define __CPU_CONF

/**
 * @name kernel configuration (shouldn't this go into the cpu.h?)
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (4096)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (512)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (512)
/** @} */

#endif /* __CPU_CONF */
/** @} */

