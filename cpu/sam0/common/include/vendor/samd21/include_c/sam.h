/**
 * \file
 *
 * \brief Top level header file
 *
 * Copyright (c) 2019 Microchip Technology Inc.
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

#if   defined(__SAMD21J15B__) || defined(__ATSAMD21J15B__)
  #include "samd21j15b.h"
#elif defined(__SAMD21E16CU__) || defined(__ATSAMD21E16CU__)
  #include "samd21e16cu.h"
#elif defined(__SAMD21G15L__) || defined(__ATSAMD21G15L__)
  #include "samd21g15l.h"
#elif defined(__SAMD21J16B__) || defined(__ATSAMD21J16B__)
  #include "samd21j16b.h"
#elif defined(__SAMD21E15B__) || defined(__ATSAMD21E15B__)
  #include "samd21e15b.h"
#elif defined(__SAMD21E15L__) || defined(__ATSAMD21E15L__)
  #include "samd21e15l.h"
#elif defined(__SAMD21E15CU__) || defined(__ATSAMD21E15CU__)
  #include "samd21e15cu.h"
#elif defined(__SAMD21E16L__) || defined(__ATSAMD21E16L__)
  #include "samd21e16l.h"
#elif defined(__SAMD21G15B__) || defined(__ATSAMD21G15B__)
  #include "samd21g15b.h"
#elif defined(__SAMD21G16L__) || defined(__ATSAMD21G16L__)
  #include "samd21g16l.h"
#elif defined(__SAMD21G16B__) || defined(__ATSAMD21G16B__)
  #include "samd21g16b.h"
#elif defined(__SAMD21E16B__) || defined(__ATSAMD21E16B__)
  #include "samd21e16b.h"
#else
  #error Library does not support the specified device
#endif

#endif /* _SAM_ */

