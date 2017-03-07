/**
 * \file
 *
 * \brief Top header file for SAML21
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
#elif defined(__SAML21J18B__) || defined(__ATSAML21J18B__)
  #include "saml21j18b.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAML21_ */
