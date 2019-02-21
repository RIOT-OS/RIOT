/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Common helper macros
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#ifndef COMMON_H
#define COMMON_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/** string representation of x */
#ifndef XTSTR
#define _XTSTR(x)    # x
#define XTSTR(x)    _XTSTR(x)
#endif
#endif

#if !defined(ICACHE_FLASH) || defined (DOXYGEN)

#ifndef ICACHE_RAM_ATTR
/** Places the code with this attribute in the IRAM. */
#define ICACHE_RAM_ATTR  __attribute__((section(".iram.text")))
#endif
#else
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif
#endif

/** Print out a message that function is not yet implementd */
#define NOT_YET_IMPLEMENTED()     LOG_INFO("%s not yet implemented\n", __func__)
/** Print out a message that function is not supported */
#define NOT_SUPPORTED()           LOG_INFO("%s not supported\n", __func__)


#if defined(ENABLE_DEBUG) || defined(DOXYGEN)
/**
  * @brief  Parameter check with return a value.
  *
  * If ENABLE_DEBUG is true, the macro checks a condition and returns with a value
  * if the condition is not fulfilled.
  * @param  cond    the condition
  * @param  err     the return value in the case the condition is not fulfilled.
  */
#define CHECK_PARAM_RET(cond,err) if (!(cond)) \
                                  { \
                                    DEBUG("%s\n", "parameter condition (" #cond ") not fulfilled"); \
                                    return err; \
                                  }

/**
 * @brief  Parameter check without return value.
 *
 * If ENABLE_DEBUG is true, the macro checks a condition and returns without a
 * value if the condition is not fulfilled.
 * @param  cond    the condition
 */
#define CHECK_PARAM(cond)         if (!(cond)) \
                                  { \
                                    DEBUG("%s\n", "parameter condition (" #cond ") not fulfilled"); \
                                    return; \
                                  }
#else
#define CHECK_PARAM_RET(cond,err) if (!(cond)) return err;
#define CHECK_PARAM(cond)         if (!(cond)) return;
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */
