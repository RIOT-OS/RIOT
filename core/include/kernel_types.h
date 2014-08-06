#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include <stdint.h>
#include <inttypes.h>

/**
 * @def KERNEL_PID_UNDEF
 * @brief Identifier to detect an invalid PID
 */
#define KERNEL_PID_UNDEF -1

/**
 * Macro for printing formatter
 */
#define PRIkernel_pid PRIi16

/**
 * @brief Unique process identifier
 *
 */
typedef int16_t kernel_pid_t;

#endif /* KERNEL_TYPES_H */
