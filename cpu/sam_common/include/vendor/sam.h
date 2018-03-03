/*
 * Copyright (C)    2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam_common
 * @{
 *
 * @file
 * @brief           Wrapper include file for specific SAM vendor headers
 *
 * @author          Sebastian Meiling <s@mlng.net>
 */

#ifndef SAM_H
#define SAM_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CPU_FAM_SAM3) && !defined(PERIPH_COUNT_IRQn)
/**
 * @brief Number of periphical IRQs for SAM3 CPUs
 */
#define PERIPH_COUNT_IRQn       (45U)
#endif

#if   defined(CPU_MODEL_SAM3A4C)
  #include "vendor/sam3x/include/sam3a4c.h"
#elif defined(CPU_MODEL_SAM3A8C)
  #include "vendor/sam3x/include/sam3a8c.h"
#elif defined(CPU_MODEL_SAM3X4C)
  #include "vendor/sam3x/include/sam3x4c.h"
#elif defined(CPU_MODEL_SAM3X4E)
  #include "vendor/sam3x/include/sam3x4e.h"
#elif defined(CPU_MODEL_SAM3X8C)
  #include "vendor/sam3x/include/sam3x8c.h"
#elif defined(CPU_MODEL_SAM3X8E)
  #include "vendor/sam3x/include/sam3x8e.h"
#elif defined(CPU_MODEL_SAM3X8H)
  #include "vendor/sam3x/include/sam3x8h.h"

#elif defined(CPU_MODEL_SAM4S2A)
  #include "vendor/sam4s/include/sam4s2a.h"
#elif defined(CPU_MODEL_SAM4S2B)
  #include "vendor/sam4s/include/sam4s2b.h"
#elif defined(CPU_MODEL_SAM4S2C)
  #include "vendor/sam4s/include/sam4s2c.h"
#elif defined(CPU_MODEL_SAM4S4A)
  #include "vendor/sam4s/include/sam4s4a.h"
#elif defined(CPU_MODEL_SAM4S4B)
  #include "vendor/sam4s/include/sam4s4b.h"
#elif defined(CPU_MODEL_SAM4S4C)
  #include "vendor/sam4s/include/sam4s4c.h"
#elif defined(CPU_MODEL_SAM4S8B)
  #include "vendor/sam4s/include/sam4s8b.h"
#elif defined(CPU_MODEL_SAM4S8C)
  #include "vendor/sam4s/include/sam4s8c.h"
#elif defined(CPU_MODEL_SAM4S16B)
  #include "vendor/sam4s/include/sam4s16b.h"
#elif defined(CPU_MODEL_SAM4S16C)
  #include "vendor/sam4s/include/sam4s16c.h"
#elif defined(CPU_MODEL_SAM4SA16B)
  #include "vendor/sam4s/include/sam4sa16b.h"
#elif defined(CPU_MODEL_SAM4SA16C)
  #include "vendor/sam4s/include/sam4sa16c.h"
#elif defined(CPU_MODEL_SAM4SD16B)
  #include "vendor/sam4s/include/sam4sd16b.h"
#elif defined(CPU_MODEL_SAM4SD16C)
  #include "vendor/sam4s/include/sam4sd16c.h"
#elif defined(CPU_MODEL_SAM4SD32B)
  #include "vendor/sam4s/include/sam4sd32b.h"
#elif defined(CPU_MODEL_SAM4SD32C)
  #include "vendor/sam4s/include/sam4sd32c.h"

#else
  #error "Unsupported Atmel SAM variant."
#endif

#ifdef __cplusplus
}
#endif

#endif /* SAM0_H */
/** @} */
