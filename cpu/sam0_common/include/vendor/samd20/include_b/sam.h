/**
 * \file
 *
 * \brief Top level header file
 *
 * Copyright (c) 2020 Microchip Technology Inc.
 *
 * \license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \license_stop
 *
 */

#ifndef _SAM_
#define _SAM_

#if   defined(__SAMD20G16B__) || defined(__ATSAMD20G16B__)
  #include "samd20g16b.h"
#elif defined(__SAMD20G14B__) || defined(__ATSAMD20G14B__)
  #include "samd20g14b.h"
#elif defined(__SAMD20E14B__) || defined(__ATSAMD20E14B__)
  #include "samd20e14b.h"
#elif defined(__SAMD20J16B__) || defined(__ATSAMD20J16B__)
  #include "samd20j16b.h"
#elif defined(__SAMD20E16B__) || defined(__ATSAMD20E16B__)
  #include "samd20e16b.h"
#elif defined(__SAMD20G15B__) || defined(__ATSAMD20G15B__)
  #include "samd20g15b.h"
#elif defined(__SAMD20E15B__) || defined(__ATSAMD20E15B__)
  #include "samd20e15b.h"
#elif defined(__SAMD20E16BU__) || defined(__ATSAMD20E16BU__)
  #include "samd20e16bu.h"
#elif defined(__SAMD20J15B__) || defined(__ATSAMD20J15B__)
  #include "samd20j15b.h"
#elif defined(__SAMD20J14B__) || defined(__ATSAMD20J14B__)
  #include "samd20j14b.h"
#elif defined(__SAMD20E15BU__) || defined(__ATSAMD20E15BU__)
  #include "samd20e15bu.h"
#else
  #error Library does not support the specified device
#endif

#endif /* _SAM_ */

