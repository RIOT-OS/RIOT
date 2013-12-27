/**
 *
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @file
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 *
 */

#ifndef CPUCONF_H_
#define CPUCONF_H_

#define FEUERWARE_CONF_CPU_NAME			"stm32f407vgt6"

/**
 * @name CPU capabilities
 * @{
 */
#define FEUERWARE_CPU_LPC2387					1
#define FEUERWARE_CONF_CORE_SUPPORTS_TIME		1
/** @} */

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

#define KERNEL_CONF_STACKSIZE_IDLE		(512)
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

/** @} */
#endif /* CPUCONF_H_ */
