/**
 * @defgroup    drivers_adc ADC
 * @ingroup     drivers
 * @brief       Generic interface for ADC drivers
 * @{
 *
 * @file        adc.h
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize ADC.
 */
void adc_init(void);

/**
 * @brief   Read ADC value of selected channel.
 *
 * Valid channel numbers are from 0 to 2.
 *
 * @return  ADC value of selected channel.
 */
uint16_t adc_read(uint8_t channel);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ADC_H */
