/*
 * SPDX-FileCopyrightText: 2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_kinetis
 * @brief           CPU specific implementations for the NXP Kinetis K series of
 *                  Cortex-M MCUs
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#if (KINETIS_FAMILY == 2)
#if (KINETIS_SUBFAMILY == 2)
/* Kinetis K22 */
#if defined(CPU_MODEL_MK22FX512VLH12) || \
    defined(CPU_MODEL_MK22FN1M0VLH12) || \
    defined(CPU_MODEL_MK22FX512VLK12) || \
    defined(CPU_MODEL_MK22FN1M0VLK12) || \
    defined(CPU_MODEL_MK22FX512VLL12) || \
    defined(CPU_MODEL_MK22FN1M0VLL12) || \
    defined(CPU_MODEL_MK22FX512VLQ12) || \
    defined(CPU_MODEL_MK22FN1M0VLQ12) || \
    defined(CPU_MODEL_MK22FX512VMC12) || \
    defined(CPU_MODEL_MK22FN1M0VMC12) || \
    defined(CPU_MODEL_MK22FX512VMD12) || \
    defined(CPU_MODEL_MK22FN1M0VMD12)
#include "vendor/MK22F12.h"
#elif defined(CPU_MODEL_MK22FN128VDC10) || \
    defined(CPU_MODEL_MK22FN128VLH10) || \
    defined(CPU_MODEL_MK22FN128VLL10) || \
    defined(CPU_MODEL_MK22FN128VMP10)
#include "vendor/MK22F12810.h"
#elif defined(CPU_MODEL_MK22FN128CAH12) || \
    defined(CPU_MODEL_MK22FN256CAH12) || \
    defined(CPU_MODEL_MK22FN256VDC12) || \
    defined(CPU_MODEL_MK22FN256VLH12) || \
    defined(CPU_MODEL_MK22FN256VLL12) || \
    defined(CPU_MODEL_MK22FN256VMP12)
#include "vendor/MK22F25612.h"
#elif defined(CPU_MODEL_MK22FN512CAP12) || \
    defined(CPU_MODEL_MK22FN512VDC12) || \
    defined(CPU_MODEL_MK22FN512VFX12) || \
    defined(CPU_MODEL_MK22FN512VLH12) || \
    defined(CPU_MODEL_MK22FN512VLL12) || \
    defined(CPU_MODEL_MK22FN512VMP12)
#include "vendor/MK22F51212.h"
#elif defined(CPU_MODEL_MK22FN1M0AVLH12) || \
    defined(CPU_MODEL_MK22FN1M0AVLK12) || \
    defined(CPU_MODEL_MK22FN1M0AVLL12) || \
    defined(CPU_MODEL_MK22FN1M0AVLQ12) || \
    defined(CPU_MODEL_MK22FN1M0AVMC12) || \
    defined(CPU_MODEL_MK22FN1M0AVMD12) || \
    defined(CPU_MODEL_MK22FX512AVLH12) || \
    defined(CPU_MODEL_MK22FX512AVLK12) || \
    defined(CPU_MODEL_MK22FX512AVLL12) || \
    defined(CPU_MODEL_MK22FX512AVLQ12) || \
    defined(CPU_MODEL_MK22FX512AVMC12) || \
    defined(CPU_MODEL_MK22FX512AVMD12)
#include "vendor/MK22FA12.h"
#endif
#elif (KINETIS_SUBFAMILY == 0)
#if defined(CPU_MODEL_MK20DX64VLH7) || \
    defined(CPU_MODEL_MK20DX128VLH7) || \
    defined(CPU_MODEL_MK20DX256VLH7) || \
    defined(CPU_MODEL_MK20DX64VLK7) || \
    defined(CPU_MODEL_MK20DX128VLK7) || \
    defined(CPU_MODEL_MK20DX256VLK7) || \
    defined(CPU_MODEL_MK20DX128VLL7) || \
    defined(CPU_MODEL_MK20DX256VLL7) || \
    defined(CPU_MODEL_MK20DX64VMB7) || \
    defined(CPU_MODEL_MK20DX128VMB7) || \
    defined(CPU_MODEL_MK20DX256VMB7) || \
    defined(CPU_MODEL_MK20DX128VML7) || \
    defined(CPU_MODEL_MK20DX256VML7)
#include "vendor/MK20D7.h"
#endif
#endif /* (KINETIS_SUBFAMILY == y) */
#elif (KINETIS_FAMILY == 6)
#if (KINETIS_SUBFAMILY == 0)
#if defined(CPU_MODEL_MK60DN256VLL10) || \
    defined(CPU_MODEL_MK60DN256VLQ10) || \
    defined(CPU_MODEL_MK60DN256VMC10) || \
    defined(CPU_MODEL_MK60DN256VMD10) || \
    defined(CPU_MODEL_MK60DN512VLL10) || \
    defined(CPU_MODEL_MK60DN512VLQ10) || \
    defined(CPU_MODEL_MK60DN512VMC10) || \
    defined(CPU_MODEL_MK60DN512VMD10) || \
    defined(CPU_MODEL_MK60DX256VLL10) || \
    defined(CPU_MODEL_MK60DX256VLQ10) || \
    defined(CPU_MODEL_MK60DX256VMC10) || \
    defined(CPU_MODEL_MK60DX256VMD10)
#include "vendor/MK60D10.h"
#endif
/**
 * @brief This CPU provides an additional ADC clock divider as CFG1[ADICLK]=1
 */
#define KINETIS_HAVE_ADICLK_BUS_DIV_2 1

#elif (KINETIS_SUBFAMILY == 4)
#if defined(CPU_MODEL_MK64FN1M0CAJ12) || \
    defined(CPU_MODEL_MK64FN1M0VDC12) || \
    defined(CPU_MODEL_MK64FN1M0VLL12) || \
    defined(CPU_MODEL_MK64FN1M0VLQ12) || \
    defined(CPU_MODEL_MK64FN1M0VMD12) || \
    defined(CPU_MODEL_MK64FX512VDC12) || \
    defined(CPU_MODEL_MK64FX512VLL12) || \
    defined(CPU_MODEL_MK64FX512VLQ12) || \
    defined(CPU_MODEL_MK64FX512VMD12)
#include "vendor/MK64F12.h"

/**
 * @name Hardware random number generator module configuration
 *
 *       For K64F SCG3 or SCG6 can be used depending on if the
 *       peripheral is accessed through AIPS-lite0 or AIPS-lite1.
 *       For K64F RNGA is only mapped to SCG6.
 * @{
 */
#define HWRNG_CLK_REG           (SIM->SCGC6)
#define HWRNG_CLK_REG_SHIFT     (SIM_SCGC6_RNGA_SHIFT)
/** @} */

#endif
#endif /* (KINETIS_SUBFAMILY == y) */
#endif /* (KINETIS_FAMILY == x) */

/**
 * @name   Flashpage configuration
 * @{
 */
#define FLASHPAGE_SIZE      (4096U)
#define FLASHPAGE_NUMOF     ((KINETIS_ROMSIZE * 1024) / FLASHPAGE_SIZE)

/* The minimum block size which can be written is 8B (Phrase). However, the
 * erase block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_BLOCK_PHRASE              (8U)
#define FLASHPAGE_WRITE_BLOCK_SIZE          FLASHPAGE_BLOCK_PHRASE
/* Writing should be always 8 bytes aligned */
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT     FLASHPAGE_BLOCK_PHRASE
/* Section erase and programming must be 16 bytes aligned */
#define FLASHPAGE_BLOCK_SECTION_ALIGNMENT   (16U)
/** @} */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

/** @} */
