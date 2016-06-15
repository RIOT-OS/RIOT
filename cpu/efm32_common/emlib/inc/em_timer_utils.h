#ifndef __SILICON_LABS_EM_TIMER_UTILS_H__
#define __SILICON_LABS_EM_TIMER_UTILS_H__

#include "em_device.h"
#if defined(TIMER_COUNT) && (TIMER_COUNT > 0)

#include "em_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t TIMER_Channel2Route(uint8_t ch);

uint32_t TIMER_Prescaler2Div(TIMER_Prescale_TypeDef prescaler);

TIMER_Prescale_TypeDef TIMER_PrescalerCalc(uint32_t freq, uint32_t freq_timer);

#ifdef __cplusplus
}
#endif

#endif /* defined(TIMER_COUNT) && (TIMER_COUNT > 0) */
#endif /* __SILICON_LABS_EM_TIMER_UTILS_H__ */
