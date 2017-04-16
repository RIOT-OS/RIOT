/*
* Copyright (C) 2015 Martin Landsmann
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

/**
* @ingroup  examples
* @{
*
* @brief    Definitions for our library hooks
* @author   Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
*/

#ifndef _LIBRARY_H
#define _LIBRARY_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
*    @breif A test function to be hooked in the `core`
*/
void HOOK_library_01(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _LIBRARY_H */
