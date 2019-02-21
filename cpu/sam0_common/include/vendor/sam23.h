/*
 * Copyright (C) 2018 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_saml1x
 * @{
 *
 * @file
 * @brief           Wrapper include file for including the specific
 *                  SAML10/SAML11 vendor header
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef SAM23_H
#define SAM23_H

#ifdef __cplusplus
extern "C" {
#endif

/* Workaround redefinition of LITTLE_ENDIAN macro (part1) */
#ifdef LITTLE_ENDIAN
#define __TMP_LITTLE_ENDIAN     LITTLE_ENDIAN
#undef LITTLE_ENDIAN
#endif

#if   defined(CPU_MODEL_SAML10D14A)
  #include "vendor/saml10/include/saml10d14a.h"
#elif defined(CPU_MODEL_SAML10D15A)
  #include "vendor/saml10/include/saml10d15a.h"
#elif defined(CPU_MODEL_SAML10D16A)
  #include "vendor/saml10/include/saml10d16a.h"
#elif defined(CPU_MODEL_SAML10E14A)
  #include "vendor/saml10/include/saml10e14a.h"
#elif defined(CPU_MODEL_SAML10E15A)
  #include "vendor/saml10/include/saml10e15a.h"
#elif defined(CPU_MODEL_SAML10E16A)
  #include "vendor/saml10/include/saml10e16a.h"

#elif   defined(CPU_MODEL_SAML11D14A)
  #include "vendor/saml11/include/saml11d14a.h"
#elif defined(CPU_MODEL_SAML11D15A)
  #include "vendor/saml11/include/saml11d15a.h"
#elif defined(CPU_MODEL_SAML11D16A)
  #include "vendor/saml11/include/saml11d16a.h"
#elif defined(CPU_MODEL_SAML11E14A)
  #include "vendor/saml11/include/saml11e14a.h"
#elif defined(CPU_MODEL_SAML11E15A)
  #include "vendor/saml11/include/saml11e15a.h"
#elif defined(CPU_MODEL_SAML11E16A)
  #include "vendor/saml11/include/saml11e16a.h"


#else
  #error "Unsupported SAM23 variant."
#endif

/* Workaround redefinition of LITTLE_ENDIAN macro (part2) */
#ifdef LITTLE_ENDIAN
#undef LITTLE_ENDIAN
#endif

#ifdef __TMP_LITTLE_ENDIAN
#define LITTLE_ENDIAN       __TMP_LITTLE_ENDIAN
#endif

#ifdef __cplusplus
}
#endif

#endif /* SAM23_H */
/** @} */
