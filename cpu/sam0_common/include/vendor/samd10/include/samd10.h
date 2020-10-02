/**
 * \file
 *
 * \brief Top header file for SAMD10
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

#ifndef _SAMD10_
#define _SAMD10_

/**
 * \defgroup SAMD10_definitions SAMD10 Device Definitions
 * \brief SAMD10 CMSIS Definitions.
 */

#if   defined(__SAMD10C13A__) || defined(__ATSAMD10C13A__)
  #include "samd10c13a.h"
#elif defined(__SAMD10C14A__) || defined(__ATSAMD10C14A__)
  #include "samd10c14a.h"
#elif defined(__SAMD10D13AM__) || defined(__ATSAMD10D13AM__)
  #include "samd10d13am.h"
#elif defined(__SAMD10D13AS__) || defined(__ATSAMD10D13AS__)
  #include "samd10d13as.h"
#elif defined(__SAMD10D14AM__) || defined(__ATSAMD10D14AM__)
  #include "samd10d14am.h"
#elif defined(__SAMD10D14AS__) || defined(__ATSAMD10D14AS__)
  #include "samd10d14as.h"
#elif defined(__SAMD10D14AU__) || defined(__ATSAMD10D14AU__)
  #include "samd10d14au.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAMD10_ */
