/**
 * \file
 *
 * \brief Top header file for SAML21
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAML21_
#define _SAML21_

/**
 * \defgroup SAML21_definitions SAML21 Device Definitions
 * \brief SAML21 CMSIS Definitions.
 */

#if   defined(__SAML21E15B__) || defined(__ATSAML21E15B__)
  #include "saml21e15b.h"
#elif defined(__SAML21E16B__) || defined(__ATSAML21E16B__)
  #include "saml21e16b.h"
#elif defined(__SAML21E17B__) || defined(__ATSAML21E17B__)
  #include "saml21e17b.h"
#elif defined(__SAML21E18B__) || defined(__ATSAML21E18B__)
  #include "saml21e18b.h"
#elif defined(__SAML21G16B__) || defined(__ATSAML21G16B__)
  #include "saml21g16b.h"
#elif defined(__SAML21G17B__) || defined(__ATSAML21G17B__)
  #include "saml21g17b.h"
#elif defined(__SAML21G18B__) || defined(__ATSAML21G18B__)
  #include "saml21g18b.h"
#elif defined(__SAML21J16B__) || defined(__ATSAML21J16B__)
  #include "saml21j16b.h"
#elif defined(__SAML21J17B__) || defined(__ATSAML21J17B__)
  #include "saml21j17b.h"
#elif defined(__SAML21J17BU__) || defined(__ATSAML21J17BU__)
  #include "saml21j17bu.h"
#elif defined(__SAML21J18B__) || defined(__ATSAML21J18B__)
  #include "saml21j18b.h"
#elif defined(__SAML21J18BU__) || defined(__ATSAML21J18BU__)
  #include "saml21j18bu.h"
#else
  #error Library does not support the specified device.
#endif

#include "compat_saml21b.h"

#endif /* _SAML21_ */
