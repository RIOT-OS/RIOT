/*
 * cpu.h - mc1322x specific definitions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT
 */

#ifndef CPUCONF_H_
#define CPUCONF_H_

/**
 * @ingroup		conf
 * @ingroup		mc1322x
 *
 * @{
 */

/**
 * @file
 * @brief		MC1322X CPUconfiguration
 *
 * @author		Oleg Hahm
 */

/**
 * @name Stdlib configuration
 * @{
 */
#define __FOPEN_MAX__		4
#define __FILENAME_MAX__	12
/** @} */

/**
 * @name Kernel configuration
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (4096)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT	(512)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE		500
/** @} */

/**
 * @name Compiler specifics
 * @{
 */
#define CC_CONF_INLINE					inline
#define CC_CONF_USED					__attribute__((used))
#define CC_CONF_NONNULL(...)			__attribute__((nonnull(__VA_ARGS__)))
#define CC_CONF_WARN_UNUSED_RESULT		__attribute__((warn_unused_result))
/** @} */

#define TRANSCEIVER_BUFFER_SIZE (10)
#define RX_BUF_SIZE  (10)

/** @} */
#endif /* CPUCONF_H_ */
