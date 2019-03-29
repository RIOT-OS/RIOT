/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_samd5x
 * @{
 *
 * @file
 * @brief           Wrapper include file for including the specific
 *                  SAMD5x/SAME5x vendor header
 *
 * @author          Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef SAMD5X_H
#define SAMD5X_H

#ifdef __cplusplus
extern "C" {
#endif

/* Workaround redefinition of LITTLE_ENDIAN macro (part1) */
#ifdef LITTLE_ENDIAN
#define __TMP_LITTLE_ENDIAN     LITTLE_ENDIAN
#undef LITTLE_ENDIAN
#endif

#if defined(CPU_MODEL_SAMD51G18A)
  #include "vendor/samd51/include/samd51g18a.h"
#elif defined(CPU_MODEL_SAMD51G19A)
  #include "vendor/samd51/include/samd51g19a.h"
#elif defined(CPU_MODEL_SAMD51J18A)
  #include "vendor/samd51/include/samd51j18a.h"
#elif defined(CPU_MODEL_SAMD51J19A)
  #include "vendor/samd51/include/samd51j19a.h"
#elif defined(CPU_MODEL_SAMD51J20A)
  #include "vendor/samd51/include/samd51j20a.h"
#elif defined(CPU_MODEL_SAMD51N19A)
  #include "vendor/samd51/include/samd51n19a.h"
#elif defined(CPU_MODEL_SAMD51N20A)
  #include "vendor/samd51/include/samd51n20a.h"
#elif defined(CPU_MODEL_SAMD51P19A)
  #include "vendor/samd51/include/samd51p19a.h"
#elif defined(CPU_MODEL_SAMD51P20A)
  #include "vendor/samd51/include/samd51p20a.h"

#elif defined(CPU_MODEL_SAME54N19A)
  #include "vendor/same54/include/same54n19a.h"
#elif defined(CPU_MODEL_SAME54N20A)
  #include "vendor/same54/include/same54n20a.h"
#elif defined(CPU_MODEL_SAME54P19A)
  #include "vendor/same54/include/same54p19a.h"
#elif defined(CPU_MODEL_SAME54P20A)
  #include "vendor/same54/include/same54p20a.h"

#else
  #error "Unsupported SAMD5x/SAME5x variant."
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

#endif /* SAMD5x_H */
/** @} */
