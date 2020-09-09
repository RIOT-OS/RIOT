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

#if   defined(__SAMD21E17D__) || defined(__ATSAMD21E17D__)
  #include "samd21e17d.h"
#elif defined(__SAMD21E17DU__) || defined(__ATSAMD21E17DU__)
  #include "samd21e17du.h"
#elif defined(__SAMD21G17D__) || defined(__ATSAMD21G17D__)
  #include "samd21g17d.h"
#elif defined(__SAMD21E17L__) || defined(__ATSAMD21E17L__)
  #include "samd21e17l.h"
#elif defined(__SAMD21G17L__) || defined(__ATSAMD21G17L__)
  #include "samd21g17l.h"
#elif defined(__SAMD21J17D__) || defined(__ATSAMD21J17D__)
  #include "samd21j17d.h"
#else
  #error Library does not support the specified device
#endif

#endif /* _SAM_ */

