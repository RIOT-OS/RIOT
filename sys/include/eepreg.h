/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_eepreg EEPROM registration
 * @ingroup     sys
 * @brief       eepreg provides a facility to easily manage the locations of
 *              data stored in EEPROM.
 *
 * @{
 *
 * @file
 * @brief       eepreg interface definitions
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef EEPREG_H
#define EEPREG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_EEPREG_SIZE_T
/**
 * @brief   type for EEPROM locations and sizes
 */
typedef int eepreg_t;
#endif

/**
 * @brief   Find the location of num entry in registry
 *
 * This uses eepreg_sizes to calculate the start of the allocated space, then
 * checks to make sure that that its name from eepreg_names is at the beginning
 * of that space, then returns the location just after the end of the name. The
 * size is the actual size, not including the name.
 *
 * @param[in] num   number of the entry in the registry
 *
 * @return          location in eeprom on success
 * @return          < 0 on error
 */
eepreg_t eepreg_addr(unsigned num);

/**
 * @brief   Check to ensure that the name is written at the correct location
 *
 * @param[in] num   number of the entry in the registry
 *
 * @return          0 on success
 * @return          < 0 on error
 */
int eepreg_check_name(unsigned num);

/**
 * @brief   Write the name at the correct location in EEPROM
 *
 * WARNING: this will overwrite anything previously written to that area.
 *
 * @param[in] num   number of the entry in the registry
 *
 * @return          0 on success
 * @return          < 0 on error
 */
int eepreg_write_name(unsigned num);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* EEPREG_H */
