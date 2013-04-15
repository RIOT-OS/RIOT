/**
 * Native CPU configuration
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */
#ifndef CPUCONF_H_
#define CPUCONF_H_
#include <signal.h>

/* TODO: choose more sensibly? */
#define KERNEL_CONF_STACKSIZE_DEFAULT   (16384)
#define KERNEL_CONF_STACKSIZE_IDLE      (4096)
#define NATIVE_ISR_STACKSIZE            (16384)
#define TRANSCEIVER_STACK_SIZE          (16384)

/* for cc110x_ng */
#define RX_BUF_SIZE (10)
#define TRANSCEIVER_BUFFER_SIZE (3)

#endif /* CPUCONF_H_ */
