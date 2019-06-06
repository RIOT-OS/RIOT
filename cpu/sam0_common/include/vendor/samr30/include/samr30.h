/**
 * \file
 *
 * \brief Top header file for SAMR30
 *
 * Copyright (c) 2016 Atmel Corporation,
 *                    a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMR30_
#define _SAMR30_

/**
 * \defgroup SAMR30_definitions SAMR30 Device Definitions
 * \brief SAMR30 CMSIS Definitions.
 */

#if   defined(__SAMR30E18A__) || defined(__ATSAMR30E18A__)
  #include "samr30e18a.h"
#elif defined(__SAMR30G18A__) || defined(__ATSAMR30G18A__)
  #include "samr30g18a.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAMR30_ */
