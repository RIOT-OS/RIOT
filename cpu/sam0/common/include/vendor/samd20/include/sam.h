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

#if   defined(__SAMD20G17__) || defined(__ATSAMD20G17__)
  #include "samd20g17.h"
#elif defined(__SAMD20J15__) || defined(__ATSAMD20J15__)
  #include "samd20j15.h"
#elif defined(__SAMD20E14__) || defined(__ATSAMD20E14__)
  #include "samd20e14.h"
#elif defined(__SAMD20G14__) || defined(__ATSAMD20G14__)
  #include "samd20g14.h"
#elif defined(__SAMD20G18__) || defined(__ATSAMD20G18__)
  #include "samd20g18.h"
#elif defined(__SAMD20J17__) || defined(__ATSAMD20J17__)
  #include "samd20j17.h"
#elif defined(__SAMD20E17__) || defined(__ATSAMD20E17__)
  #include "samd20e17.h"
#elif defined(__SAMD20G15__) || defined(__ATSAMD20G15__)
  #include "samd20g15.h"
#elif defined(__SAMD20E15__) || defined(__ATSAMD20E15__)
  #include "samd20e15.h"
#elif defined(__SAMD20E16__) || defined(__ATSAMD20E16__)
  #include "samd20e16.h"
#elif defined(__SAMD20E18__) || defined(__ATSAMD20E18__)
  #include "samd20e18.h"
#elif defined(__SAMD20J14__) || defined(__ATSAMD20J14__)
  #include "samd20j14.h"
#elif defined(__SAMD20G18U__) || defined(__ATSAMD20G18U__)
  #include "samd20g18u.h"
#elif defined(__SAMD20G16__) || defined(__ATSAMD20G16__)
  #include "samd20g16.h"
#elif defined(__SAMD20J18__) || defined(__ATSAMD20J18__)
  #include "samd20j18.h"
#elif defined(__SAMD20G17U__) || defined(__ATSAMD20G17U__)
  #include "samd20g17u.h"
#elif defined(__SAMD20J16__) || defined(__ATSAMD20J16__)
  #include "samd20j16.h"
#else
  #error Library does not support the specified device
#endif

#endif /* _SAM_ */

