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

#if   defined(__SAML21E18A__) || defined(__ATSAML21E18A__)
  #include "saml21e18a.h"
#elif defined(__SAML21G18A__) || defined(__ATSAML21G18A__)
  #include "saml21g18a.h"
#elif defined(__SAML21J18A__) || defined(__ATSAML21J18A__)
  #include "saml21j18a.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAML21_ */
