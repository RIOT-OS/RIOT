/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <string.h>

#include "modules.h"

/**
 * @defgroup     core_util_log Logging
 * @brief Logging
 * @{
 *
 * This header offers a bunch of "LOG_*" functions that, with the default
 * implementation, just use printf, but honour a verbosity level.
 *
 * If you want a logging unit name to be prefixed to the logs, define LOG_UNIT
 * in the source file before including this header.
 *
 * If desired, it is possible to implement a log module which then will be used
 * instead the default printf-based implementation.  In order to do so, the log
 * module has to
 *
 * 1. provide "log_module.h"
 * 2. have a name starting with "log_" *or* depend on the pseudo-module LOG,
 * 3. implement log_write()
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief       System logging header
 *
 * See "sys/log/log_printfnoformat" for an example.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

/**
 * @brief defined log levels
 *
 * These are the logging levels a user can choose.
 * The idea is to set LOG_LEVEL to one of these values in the application's Makefile.
 * That will restrict output of log statements to those with equal or lower log level.
 *
 * The default log level is LOG_INFO, which will print every message.
 *
 * The log function calls of filtered messages will be optimized out at compile
 * time, so a lower log level might result in smaller code size.
 */
typedef enum {
    /**
     * @brief Lowest log level, will output nothing
     */
    LOG_NONE = 0,
    /**
     * @brief Error log level, will print only critical,
     *        non-recoverable errors like hardware initialization failures
     */
    LOG_ERROR = 1,
    /**
     * @brief Warning log level, will print warning messages for
     *        temporary errors
     */
    LOG_WARNING = 2,
    /**
     * @brief Informational log level, will print purely
     *        informational messages like successful system bootup,
     *        network link state, ...
     */
    LOG_INFO = 3,
    /**
     * @brief Debug log level, printing developer stuff considered
     *        too verbose for production use
     */
    LOG_DEBUG = 4,

    LOG_ALL,
} log_level_t;


#if !defined(LOG_LEVEL)
/**
 * @brief Default log level define
 */
#  define LOG_LEVEL LOG_INFO
#endif

/* If the log unit is not provided, we assume the default created by the build system. */
#if !defined(LOG_UNIT)
#  if defined(LOG_UNIT_DEFAULT)
#    define LOG_UNIT LOG_UNIT_DEFAULT
#  else
#    define LOG_UNIT ""
#  endif
#endif

#define LOG_IMPL(write, level, unit, ...) \
    do { _LOG_PROLOGUE \
        if (_CAN_LOG_H(level, unit)) { \
            write(level, (unit), __VA_ARGS__); \
        } \
    } while (0U) _LOG_EPILOGUE

#define LOG_WITH_UNIT(level, unit, ...)       LOG_IMPL(log_write,    level, unit,     __VA_ARGS__)
#define LOG_BEGIN_WITH_UNIT(level, unit, ...) LOG_IMPL(log_begin,    level, unit,     __VA_ARGS__)
#define LOG_CONT_WITH_UNIT(level, unit, ...)  LOG_IMPL(log_continue, level, unit,     __VA_ARGS__)
#define LOG_END_WITH_UNIT(level, unit, ...)   LOG_IMPL(log_end,      level, unit,     __VA_ARGS__)

#define LOG(level, ...)                       LOG_WITH_UNIT(         level, LOG_UNIT, __VA_ARGS__)
#define LOG_BEGIN(level, ...)                 LOG_BEGIN_WITH_UNIT(   level, LOG_UNIT, __VA_ARGS__)
#define LOG_CONT(level, ...)                  LOG_CONT_WITH_UNIT(    level, LOG_UNIT, __VA_ARGS__)
#define LOG_END(level, ...)                   LOG_END_WITH_UNIT(     level, LOG_UNIT, __VA_ARGS__)

#define LOG_ERROR(...)   LOG(LOG_ERROR,   __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOG_WARNING, __VA_ARGS__)
#define LOG_INFO(...)    LOG(LOG_INFO,    __VA_ARGS__)
#define LOG_DEBUG(...)   LOG(LOG_DEBUG,   __VA_ARGS__)

/** @} */ /* section */

/** @cond */ /* hide */

#if defined(MODULE_LOG_DYNAMIC_CONTROL)
#  include "log_dynamic.h"
#else
#  define log_dynamic_allows(unit, level) 0
#endif

#if defined(LOG_SELECTIVE)
#include <stdbool.h>
/* If LOG is present in make, switch to new logic: LOG invocations used to always have an effect
 * if the level matched, now they only do if contained in LOG. DEBUG invocations used to only
 * have an effect when ENABLE_DEBUG was turned on, now they do too if contained in LOG 
 * New behavior:
 *   - LOG() iff level and unit matches
 *   - DEBUG() iff (ENABLE_DEBUG or [level and unit matches, just like LOG])
 */
#  define _CAN_LOG_H(level, unit) _LOG_PREDICATE(unit, level)
#  define _CAN_DEBUG_H(level, unit) (ENABLE_DEBUG || _CAN_LOG_H(level, unit))

/*   There are three tiers of selective logging: */
#  if defined(_LOG_SELECTIVE_ALL)
/*   1. LOG="ALL", i.e., allow logging from all units if compatible with LOG_LEVEL */
#    define _LOG_PREDICATE(unit, level) (log_level_t)(level) <= (log_level_t)(LOG_LEVEL)

#  elif defined(_LOG_SELECTIVE_RULES)
/* 2. LOG="core.irq ztimer", i.e., enable log units by prefix pattern */
/*   This is the rule builder R from _LOG_SELECTIVE_RULES in Makefile.include. */
/**
 * @brief Selecting logging rule builder
 *
 * Expands to an expression that checks if the given @p rule_unit_pattern is a
 * prefix of the requested @p unit and if the requested @p level is less than or
 * equal to the log level this rule allows logging for, based on selective
 * logging rules supplied via the `LOG` make variable. 
 *
 * @note This macro corresponds to each rule in the `LOG` make variable. E.g.,
 * `LOG="ztimer=ERROR coap tinydtls=DEBUG" creates two log rules:
 * `_LOG_RULE(u,l, "ztimer", LOG_ERROR)`,
 * `_LOG_RULE(u,l, "coap", LOG_LEVEL)`, and
 * `_LOG_RULE(u,l, "tinydtls", LOG_DEBUG).
 *
 * A log rule consists of a log unit pattern and a maximum log level.
 * Each rule allows logging from all units matching the log unit prefix pattern
 * up to and including the maximum log level of the respective rule.
 * As all log rules are or'd, the most inclusive rule applies.
 * E.g., if you call LOG_INFO from the "ztimer.core" log unit, there are two
 * `_LOG_RULE(u,l,"ztimer",LOG_ERROR)` and 
 * `_LOG_RULE(u,l,"ztimer.core", LOG_INFO)` rules, the latter one finally allows
 * logging the given message.
 *
 * @param unit The log unit as string literal for which to check if logging is allowed at @p level
 * @param level The log level to check if allowed to be logged at from @p unit
 * @param rule_unit_pattern A log unit prefix or full unit as a string literal
 * @param rule_max_level The maximum log level this rule allows logging for from units matching
 *                       @p rule_unit_pattern
 *
 * @warning @p rule_unit_pattern and @p rule_max_level MUST be string literal,
 *          @p unit and @p level may be expressions or literals.
 *
 * @returns C boolean expression with `||` logical or operator prepended
 */
#    define _LOG_RULE(unit, level, rule_unit_pattern, rule_max_level) || \
        ((log_level_t)(level) <= (log_level_t)(rule_max_level) \
        && __builtin_strncmp((rule_unit_pattern), (unit), sizeof(rule_unit_pattern) - 1) == 0)

/**
 * @brief Selective logging predicate expression builder
 *
 * Expands to an expression that returns a boolean value determining
 * whether log messages are allowed to be emitted from the given log @p unit
 * at the given log @p level, based on selective logging rules supplied
 * via the `LOG` make variable. 
 *
 * @param unit Log unit, may be expression or string literal
 * @param level Log level, may be expression or string literal
 *
 * @returns C boolean expression
 */
#    define _LOG_SELECTIVE_ALLOWS_AS_EXPR(unit, level) \
        (0 _LOG_SELECTIVE_RULES(_LOG_RULE, unit, level))

/**
 * @brief Logging predicate builder
 *
 * Expands to an expression that returns a boolean value determining
 * whether log messages are allowed to be emitted from the given log @p unit
 * at the given log @p level.
 *
 * This macro checks both if dynamic logging control (if used) allows
 * logging from the given unit at the given level, and otherwise if
 * selective logging rules specified using the `LOG` make variable
 * allow logging from the unit at this level.
 *
 * If the selective logging rules cannot be folded into a simple
 * constant expression (e.g., `true` or `false`, so the surrounding `if` can
 * be optimised away), perhaps due to @p unit
 * being a variable that the compiler cannot see and is hence unable
 * to optimise for in `strcmp` calls at optimisation time, the resulting
 * expression will call a non-inlined `_log_selective_allows`
 * function. This may happen on, e.g., the ESP platform, due to `TAG`
 * string variables used as @p unit at lower optimisation levels.
 * In these cases, we would litter the RIOT binary with non-folded
 * `strcmp` calls and boolean or clauses to check if we can log there.
 * To avoid this, this non-optimisable or clause is replaced by a call
 * to `_log_selective_allows`.
 *
 * @param unit Log unit, may be expression or string literal
 * @param level Log level, may be expression or string literal
 *
 * @returns C boolean expression
 */
#  define _LOG_PREDICATE(unit, level) \ 
       /* Check if dynamic log control allows logging from unit at requested level. */ \
       (log_dynamic_allows(unit, level) || \
       /* Fall back to selective log control using LOG. */ \
        (__builtin_constant_p(_LOG_SELECTIVE_ALLOWS_AS_EXPR(unit, level)) \
           /* Can check at compile time, so emit expression. */
           ? _LOG_SELECTIVE_ALLOWS_AS_EXPR(unit, level) \
           /* Unable to check at compile time, so emit function call. 
            * This may happen when the unit is variable instead of a literal. */
           : _log_selective_allows((unit), (log_level_t)(level))))

#  else
/*   3. LOG="", i.e., disable all log units, unless dynamic control allows it again. */
#    define _LOG_PREDICATE(unit, level) log_dynamic_allows(unit, level)
#  endif

#else /* defined(LOG_SELECTIVE) */
/* Old behavior:
 * - LOG() iff level matches
 * - DEBUG() iff ENABLE_DEBUG is on. */
#  define _CAN_LOG_H(level, unit) ((log_level_t)(level) <= (log_level_t)(LOG_LEVEL))
#  define _CAN_DEBUG_H(level, unit) ENABLE_DEBUG
#endif /* defined(LOG_SELECTIVE) */

#if defined(__clang__)
#  define _LOG_PROLOGUE \
 _Pragma("clang diagnostic push") \
 _Pragma("clang diagnostic ignored \"-Wtautological-compare\"")
#  define _LOG_EPILOGUE \
 _Pragma("clang diagnostic pop")
#else /* defined(__clang__) */
#  define _LOG_PROLOGUE
#  define _LOG_EPILOGUE
#endif /* defined(__clang__) */

#if defined(MODULE_LOG)
#  include "log_module.h"
/* A log module can share the printf format it is using internally if it merely
 * applies formatting before printing. The log format can then be used on
 * platforms that do not use printf directly, such as ROM/DRAM printing on ESP.
 *
 * Additionally, if the format just applies to the prefix of printed messages,
 * the log module may define `LOG_FORMAT_PRINT_PREFIX` and then does not
 * need to define `log_write`, `log_begin`, `log_continue`, `log_end`.
 */
#  if defined(LOG_FORMAT_PRINT_PREFIX)
#    if !defined(LOG_FORMAT_PRINT)
#      define LOG_FORMAT_PRINT fmt_print
#    endif
#    include "fmt.h"
#    define log_write(   level, unit, ...) LOG_FORMAT_PRINT(LOG_FORMAT(level, unit, __VA_ARGS__))
#    define log_begin(   level, unit, ...) LOG_FORMAT_PRINT(LOG_FORMAT(level, unit, __VA_ARGS__))
#    define log_continue(level, unit, ...) LOG_FORMAT_PRINT(__VA_ARGS__)
#    define log_end(     level, unit, ...) LOG_FORMAT_PRINT(__VA_ARGS__)
#  endif
#  if defined(log_write) && !defined(log_begin) && !defined(log_continue) && !defined(log_end)
#    define log_begin    log_write
#    define log_continue log_write
#    define log_end      log_write
#  endif
#else /* defined(MODULE_LOG) */
#  include "fmt.h"
#  define log_write(   level, unit, ...) fmt_print(__VA_ARGS__)
#  define log_begin(   level, unit, ...) fmt_print(__VA_ARGS__)
#  define log_continue(level, unit, ...) fmt_print(__VA_ARGS__)
#  define log_end(     level, unit, ...) fmt_print(__VA_ARGS__)
#endif /* defined(MODULE_LOG) */

/** @endcond */ /* show */

#ifdef __cplusplus
}
#endif

/** @} */
