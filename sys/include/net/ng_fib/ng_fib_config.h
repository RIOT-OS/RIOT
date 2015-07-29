/*
 * Copyright (C) 2015  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_fib
 * @brief       configurations for the FIB
 *
 * @{
 *
 * @file
 * @brief       configuration header for the FIB
 * @author      Martin Landsmann
 */

 #ifndef FIB_CONFIG_H_
 #define FIB_CONFIG_H_

 #ifdef __cplusplus
 extern "C" {
 #endif

 /**
  * @brief the used memory in bytes for one universal address entry
  */
 #define UNIVERSAL_ADDRESS_SIZE (16)

 /**
  * @brief Maximum number of universal address entries handled
  */
 #define UNIVERSAL_ADDRESS_MAX_ENTRIES (40)

 /**
  * @brief maximum number of FIB tables entries handled
  * @note the value should be UNIVERSAL_ADDRESS_MAX_ENTRIES/2 to be safe since,
  *       in the worst case, each FIB entry points to 2 unique universal addresses.
  */
 #define FIB_MAX_FIB_TABLE_ENTRIES (20)

 /**
  * @brief maximum number of handled routing protocols (RP)
  *        used to notify the saved kernel_pid_t on ureachable destination
  */
 #define FIB_MAX_REGISTERED_RP (5)

 /**
  * @brief message type for RP notifications on unreachable destination
  */
 #define FIB_MSG_RP_SIGNAL (0x99)

 /**
  * @brief indicator of a FIB entry lifetime that does not expire (2^32 - 1)
  */
 #define FIB_LIFETIME_NO_EXPIRE (0xFFFFFFFF)

 #ifdef __cplusplus
 }
 #endif

#endif /* FIB_CONFIG_H_ */
/** @} */
