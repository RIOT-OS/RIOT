/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef __SILICON_LABS_EM_RTCC_UTILS_H__
#define __SILICON_LABS_EM_RTCC_UTILS_H__

#include "em_device.h"
#if defined( RTCC_COUNT ) && ( RTCC_COUNT == 1 )

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @brief
 *   Conversion macros for RTCC_DATE and RTCC_TIME registers.
 *
 * @note
 *   Values are expected to be in a valid within the date and time domain.
 *
 * @{
 ******************************************************************************/
#define RTCC_BCD2Second(x)     (((((x) & _RTCC_TIME_SECT_MASK) >> _RTCC_TIME_SECT_SHIFT) * 10) + (((x) & _RTCC_TIME_SECU_MASK) >> _RTCC_TIME_SECU_SHIFT))
#define RTCC_BCD2Minute(x)     (((((x) & _RTCC_TIME_MINT_MASK) >> _RTCC_TIME_MINT_SHIFT) * 10) + (((x) & _RTCC_TIME_MINU_MASK) >> _RTCC_TIME_MINU_SHIFT))
#define RTCC_BCD2Hour(x)       (((((x) & _RTCC_TIME_HOURT_MASK) >> _RTCC_TIME_HOURT_SHIFT) * 10) + (((x) & _RTCC_TIME_HOURU_MASK) >> _RTCC_TIME_HOURU_SHIFT))

#define RTCC_BCD2DayOfMonth(x) (((((x) & _RTCC_DATE_DAYOMT_MASK) >> _RTCC_DATE_DAYOMT_SHIFT) * 10) + (((x) & _RTCC_DATE_DAYOMU_MASK) >> _RTCC_DATE_DAYOMU_SHIFT))
#define RTCC_BCD2Month(x)      (((((x) & _RTCC_DATE_MONTHT_MASK) >> _RTCC_DATE_MONTHT_SHIFT) * 10) + (((x) & _RTCC_DATE_MONTHU_MASK) >> _RTCC_DATE_MONTHU_SHIFT))
#define RTCC_BCD2Year(x)       (((((x) & _RTCC_DATE_YEART_MASK) >> _RTCC_DATE_YEART_SHIFT) * 10) + (((x) & _RTCC_DATE_YEARU_MASK) >> _RTCC_DATE_YEARU_SHIFT))
#define RTCC_BCD2DayOfWeek(x)  ((((x) & _RTCC_DATE_DAYOW_MASK) >> _RTCC_DATE_DAYOW_SHIFT))

#define RTCC_Second2BCD(x)     (((((x) / 10) << _RTCC_TIME_SECT_SHIFT) | (((x) % 10) << _RTCC_TIME_SECU_SHIFT)) & (_RTCC_TIME_SECT_MASK | _RTCC_TIME_SECU_MASK))
#define RTCC_Minute2BCD(x)     (((((x) / 10) << _RTCC_TIME_MINT_SHIFT) | (((x) % 10) << _RTCC_TIME_MINU_SHIFT)) & (_RTCC_TIME_MINT_MASK | _RTCC_TIME_MINU_MASK))
#define RTCC_Hour2BCD(x)       (((((x) / 10) << _RTCC_TIME_HOURT_SHIFT) | (((x) % 10) << _RTCC_TIME_HOURU_SHIFT)) & (_RTCC_TIME_HOURT_MASK | _RTCC_TIME_HOURU_MASK))

#define RTCC_DayOfMonth2BCD(x) (((((x) / 10) << _RTCC_DATE_DAYOMT_SHIFT) | (((x) % 10) << _RTCC_DATE_DAYOMU_SHIFT)) & (_RTCC_DATE_DAYOMT_MASK | _RTCC_DATE_DAYOMU_MASK))
#define RTCC_Month2BCD(x)      (((((x) / 10) << _RTCC_DATE_MONTHT_SHIFT) | (((x) % 10) << _RTCC_DATE_MONTHU_SHIFT)) & (_RTCC_DATE_MONTHT_MASK | _RTCC_DATE_MONTHU_MASK))
#define RTCC_Year2BCD(x)       (((((x) / 10) << _RTCC_DATE_YEART_SHIFT) | (((x) % 10) << _RTCC_DATE_YEARU_SHIFT)) & (_RTCC_DATE_YEART_MASK | _RTCC_DATE_YEARU_MASK))
#define RTCC_DayOfWeek2BCD(x)  ((((x) % 10) << _RTCC_DATE_DAYOW_SHIFT) & _RTCC_DATE_DAYOW_MASK)
/** @} */

/***************************************************************************//**
 * @brief
 *   Conversion macros for RTCC_CC_DATE and RTCC_CC_TIME registers.
 *
 * @note
 *   Values are expected to be in a valid within the date and time domain.
 *
 * @{
 ******************************************************************************/
#define RTCC_Channel_BCD2Second(x) (((((x) & _RTCC_CC_TIME_SECT_MASK) >> _RTCC_CC_TIME_SECT_SHIFT) * 10) + (((x) & _RTCC_CC_TIME_SECU_MASK) >> _RTCC_CC_TIME_SECU_SHIFT))
#define RTCC_Channel_BCD2Minute(x) (((((x) & _RTCC_CC_TIME_MINT_MASK) >> _RTCC_CC_TIME_MINT_SHIFT) * 10) + (((x) & _RTCC_CC_TIME_MINU_MASK) >> _RTCC_CC_TIME_MINU_SHIFT))
#define RTCC_Channel_BCD2Hour(x)   (((((x) & _RTCC_CC_TIME_HOURT_MASK) >> _RTCC_CC_TIME_HOURT_SHIFT) * 10) + (((x) & _RTCC_CC_TIME_HOURU_MASK) >> _RTCC_CC_TIME_HOURU_SHIFT))

#define RTCC_Channel_BCD2Day(x)    (((((x) & _RTCC_CC_DATE_DAYT_MASK) >> _RTCC_CC_DATE_DAYT_SHIFT) * 10) + (((x) & _RTCC_CC_DATE_DAYU_MASK) >> _RTCC_CC_DATE_DAYU_SHIFT))
#define RTCC_Channel_BCD2Month(x)  (((((x) & _RTCC_CC_DATE_MONTHT_MASK) >> _RTCC_CC_DATE_MONTHT_SHIFT) * 10) + (((x) & _RTCC_CC_DATE_MONTHU_MASK) >> _RTCC_CC_DATE_MONTHU_SHIFT))

#define RTCC_Channel_Second2BCD(x) (((((x) / 10) << _RTCC_CC_TIME_SECT_SHIFT) | (((x) % 10) << _RTCC_CC_TIME_SECU_SHIFT)) & (_RTCC_CC_TIME_SECT_MASK | _RTCC_CC_TIME_SECU_MASK))
#define RTCC_Channel_Minute2BCD(x) (((((x) / 10) << _RTCC_CC_TIME_MINT_SHIFT) | (((x) % 10) << _RTCC_CC_TIME_MINU_SHIFT)) & (_RTCC_CC_TIME_MINT_MASK | _RTCC_CC_TIME_MINU_MASK))
#define RTCC_Channel_Hour2BCD(x)   (((((x) / 10) << _RTCC_CC_TIME_HOURT_SHIFT) | (((x) % 10) << _RTCC_CC_TIME_HOURU_SHIFT)) & (_RTCC_CC_TIME_HOURT_MASK | _RTCC_CC_TIME_HOURU_MASK))

#define RTCC_Channel_Day2BCD(x)    (((((x) / 10) << _RTCC_CC_DATE_DAYT_SHIFT) | (((x) % 10) << _RTCC_CC_DATE_DAYU_SHIFT)) & (_RTCC_CC_DATE_DAYT_MASK | _RTCC_CC_DATE_DAYU_MASK))
#define RTCC_Channel_Month2BCD(x)  (((((x) / 10) << _RTCC_CC_DATE_MONTHT_SHIFT) | (((x) % 10) << _RTCC_CC_DATE_MONTHU_SHIFT)) & (_RTCC_CC_DATE_MONTHT_MASK | _RTCC_CC_DATE_MONTHU_MASK))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* defined( RTCC_COUNT ) && ( RTCC_COUNT == 1 ) */
#endif /* __SILICON_LABS_EM_RTCC_UTILS_H__ */
