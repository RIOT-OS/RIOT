/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <stdint.h>
#include <inttypes.h>

#include "cpu.h"
#include "periph/gating.h"
#include "periph/adc.h"
#include "periph_conf.h"

/* These refer to addresses and values to be written according to
 * the Errata notices ADC_AI.003 & ADC_AI.004 of the XMC1100 */
#define ERRATA_AI003   (0x40010500)
#define ERRATA_AI003_V (1UL)
#define ERRATA_AI004_1 (0x480340E0)
#define ERRATA_AI004_2 (0x480340E4)
#define ERRATA_AI004_V (0x80008000)

/* "Sample Time Control for Standard Conversions": Number of
 * additional clock cycles to be added to the minimum sample phase of
 * 2 analog clock cycles: Coding and resulting sample time see Table
 * 15-3. */
#ifndef ADC_STCS
#define ADC_STCS (17)           /* 32 additional clock cycles */
#endif

/* guard file in case no ADC device is defined */
#if ADC_NUMOF

int adc_init(adc_t dev, adc_precision_t precision)
{
    uint16_t cms;

    adc_poweron(dev);

    switch (precision) {
        case ADC_RES_8BIT:
            cms = (2 << VADC_GLOBICLASS_CMS_Pos);
            break;
        case ADC_RES_10BIT:
            cms = (1 << VADC_GLOBICLASS_CMS_Pos);
            break;
        case ADC_RES_12BIT:
            cms = 0;
            break;
        default:
            return -1;
    }

    /* set precision on device */
    VADC->GLOBICLASS[0] = cms | ADC_STCS;

    return 0;
}

int adc_sample(adc_t dev, int channel)
{
    if (channel > 7) {
        return -1;
    }

    /* device ready? */
    if (!(SHS0->SHSCFG & SHS_SHSCFG_ANRDY_Msk)) {
        return -2;
    }

    /* get precision configuration */
    uint8_t cms  = ((VADC->GLOBICLASS[dev] & VADC_GLOBICLASS_CMS_Msk)
                    >> VADC_GLOBICLASS_CMS_Pos);

    /* clear both */
    VADC->BRSSEL[0] = 0;
    VADC->BRSSEL[1] = 0;

    /* select channel */
    VADC->BRSSEL[dev] = (1 << channel);

    /* initiate load */
    VADC->BRSMR = ((1 << VADC_BRSMR_ENGT_Pos) |
                   (1 << VADC_BRSMR_LDEV_Pos));

    /* wait */
    while (!(VADC->GLOBEFLAG & VADC_GLOBEFLAG_REVGLB_Msk)) ;

    unsigned int result = VADC->GLOBRES;

    VADC->GLOBEFLAG = ((1 << VADC_GLOBEFLAG_REVGLBCLR_Pos) |
                       (1 << VADC_GLOBEFLAG_SEVGLBCLR_Pos));

    if (((result & VADC_GLOBRES_CHNR_Msk) >>
         VADC_GLOBRES_CHNR_Pos) == channel) {
        return (result & VADC_GLOBRES_RESULT_Msk) >> (2 * cms);
    }
    else {
        return -1;
    }
}

void adc_poweron(adc_t dev)
{
    (void)dev;

    if (SCU_CLK->CGATSTAT0 & SCU_CLK_CGATSTAT0_VADC_Msk) {
        GATING_CLEAR(VADC);
    }

    VADC->CLC = 0;

    /* An initial startup calibration is required once after a
     * reset. First, the converter must be enabled (ANOFF = 0B) */
    SHS0->SHSCFG = (1 << SHS_SHSCFG_SCWC_Pos);

    /* See ADC_AI.003: The analog section of ADC is not fully
     * functioning when it is enabled by bit SHSCFG.ANOFF.
     * Workaround: To enable the analog section of the ADC, write
     * value 00000001H to register address 40010500H in addition to
     * the setup as mentioned above.  */
    *((uint32_t *)ERRATA_AI003) = ERRATA_AI003_V;

    /* then the startup calibration can be initiated by setting bit
     * SUCAL in register GLOBCFG. Conversions may be started after the
     * initial calibration sequence. This is indicated by
     * parameter SHS0_SHSCFG.STATE which can be polled by software.

     * See ADC_AI.004: In the AA step, the ADC startup calibration via
     * GLOBCFG.SUCAL is incomplete.  Workaround: The automatic post
     * calibration function need to be disabled during calibration and
     * a software triggered re-calibration sequence is proposed.  The
     * proposed startup calibration sequence is:

     * Set bit GLOBCFG.SUCAL, GLOBCFG.DPCAL0 and GLOBCFG.DPCAL1
     * to 1 */
    VADC->GLOBCFG = ((1 << VADC_GLOBCFG_SUCAL_Pos) |
                     (1 << VADC_GLOBCFG_DPCAL0_Pos) |
                     (1 << VADC_GLOBCFG_DPCAL1_Pos));

    /* Wait for the startup calibration to complete by polling bit
     * SHSCFG.STATE */
    while (SHS0->SHSCFG & SHS_SHSCFG_STATE_Msk) ;

    /* Write value 80008000H to register address 480340E0H and 480340E4H */
    *((uint32_t *)ERRATA_AI004_1) = ERRATA_AI004_V;
    *((uint32_t *)ERRATA_AI004_2) = ERRATA_AI004_V;
}

void adc_poweroff(adc_t dev)
{
    /* Off request: stop the module clock */
    VADC->CLC = 1;

    /* ANOFF = 1: Converter is permanently off */
    SHS0->SHSCFG = ((1 << SHS_SHSCFG_ANOFF_Pos) |
                    (1 << SHS_SHSCFG_SCWC_Pos));

    /* gate */
    GATING_SET(VADC);
}

int adc_map(adc_t dev, int value, int min, int max)
{
    return -1;
}

float adc_mapf(adc_t dev, int value, float min, float max)
{
    return -1.0f;
}

#endif /* ADC_NUMOF */
