/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_nrf51822qfaa
 * @{
 *
 * @file            cpu-conf.h
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "nrf51.h"
#include "nrf51_bitfields.h"


/**
 * @name Kernel configuration
 *
 * TODO: measure and adjust for the cortex-m0
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (2500)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (512)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (512)
/** @} */


#endif /* __CPU_CONF_H */
/** @} */
