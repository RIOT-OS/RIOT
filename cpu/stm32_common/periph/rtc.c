/*
 * Copyright (C) 2015 Lari Lehtomäki
 *               2016 Laksh Bhatia
 *               2016-2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 * @file
 * @brief       Low-level RTC driver implementation
 *
 * @author      Lari Lehtomäki <lari@lehtomaki.fi>
 * @author      Laksh Bhatia <bhatialaksh3@gmail.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "periph/rtc.h"
#include "periph_conf.h"

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32L1)

/* guard file in case no RTC device was specified */
#if RTC_NUMOF

#define RTC_WRITE_PROTECTION_KEY1   (0xCA)
#define RTC_WRITE_PROTECTION_KEY2   (0x53)
#define RTC_SYNC_PRESCALER          (0xff)  /**< prescaler for 32.768 kHz oscillator */
#define RTC_ASYNC_PRESCALER         (0x7f)  /**< prescaler for 32.768 kHz oscillator */

#define MCU_YEAR_OFFSET              (100)  /**< struct tm counts years since 1900
                                                but RTC has only two-digit year
                                                hence the offset of 100 years. */

typedef struct {
    rtc_alarm_cb_t cb;          /**< callback called from RTC interrupt */
    void *arg;                  /**< argument passed to the callback */
} rtc_state_t;

static rtc_state_t rtc_callback;

static uint8_t byte2bcd(uint8_t value);

/**
 * @brief Initializes the RTC to use LSE (external 32.768 kHz oscillator) as a
 * clocl source. Verify that your board has this oscillator. If other clock
 * source is used, then also the prescaler constants should be changed.
 */
void rtc_init(void)
{
    /* Enable write access to RTC registers */
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
    PWR->CR |= PWR_CR_DBP;

#if defined(CPU_FAM_STM32L1)
    if (!(RCC->CSR & RCC_CSR_RTCEN)) {
#else
    if (!(RCC->BDCR & RCC_BDCR_RTCEN)) {
#endif
        rtc_poweron();
    }

    /* Unlock RTC write protection */
    RTC->WPR = RTC_WRITE_PROTECTION_KEY1;
    RTC->WPR = RTC_WRITE_PROTECTION_KEY2;

    /* Enter RTC Init mode */
    RTC->ISR = 0;
    RTC->ISR |= RTC_ISR_INIT;
    while ((RTC->ISR & RTC_ISR_INITF) == 0) {}

    /* Set 24-h clock */
    RTC->CR &= ~RTC_CR_FMT;
    /* Timestamps enabled */
    RTC->CR |= RTC_CR_TSE;

    /* Configure the RTC PRER */
    RTC->PRER = RTC_SYNC_PRESCALER;
    RTC->PRER |= (RTC_ASYNC_PRESCALER << 16);

    /* Exit RTC init mode */
    RTC->ISR &= (uint32_t) ~RTC_ISR_INIT;

    /* Enable RTC write protection */
    RTC->WPR = 0xff;
}

int rtc_set_time(struct tm *time)
{
    /* Enable write access to RTC registers */
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
    PWR->CR |= PWR_CR_DBP;

    /* Unlock RTC write protection */
    RTC->WPR = RTC_WRITE_PROTECTION_KEY1;
    RTC->WPR = RTC_WRITE_PROTECTION_KEY2;

    /* Enter RTC Init mode */
    RTC->ISR |= RTC_ISR_INIT;
    while ((RTC->ISR & RTC_ISR_INITF) == 0) {}

    /* Set 24-h clock */
    RTC->CR &= ~RTC_CR_FMT;

    RTC->DR = ((((uint32_t)byte2bcd(time->tm_year - MCU_YEAR_OFFSET) << 16) & (RTC_DR_YT | RTC_DR_YU)) |
               (((uint32_t)byte2bcd(time->tm_mon + 1) <<  8) & (RTC_DR_MT | RTC_DR_MU)) |
               (((uint32_t)byte2bcd(time->tm_mday) <<  0) & (RTC_DR_DT | RTC_DR_DU)));

    RTC->TR = ((((uint32_t)byte2bcd(time->tm_hour) << 16) & (RTC_TR_HT | RTC_TR_HU)) |
               (((uint32_t)byte2bcd(time->tm_min) <<  8) & (RTC_TR_MNT | RTC_TR_MNU)) |
               (((uint32_t)byte2bcd(time->tm_sec) <<  0) & (RTC_TR_ST | RTC_TR_SU)));

    /* Exit RTC init mode */
    RTC->ISR &= (uint32_t) ~RTC_ISR_INIT;
    /* Enable RTC write protection */
    RTC->WPR = 0xFF;
    return 0;
}

int rtc_get_time(struct tm *time)
{
    time->tm_year = MCU_YEAR_OFFSET;
    time->tm_year += (((RTC->DR & RTC_DR_YT)  >> 20) * 10) + ((RTC->DR & RTC_DR_YU)  >> 16);
    time->tm_mon  = (((RTC->DR & RTC_DR_MT)  >> 12) * 10) + ((RTC->DR & RTC_DR_MU)  >>  8) - 1;
    time->tm_mday = (((RTC->DR & RTC_DR_DT)  >>  4) * 10) + ((RTC->DR & RTC_DR_DU)  >>  0);
    time->tm_hour = (((RTC->TR & RTC_TR_HT)  >> 20) * 10) + ((RTC->TR & RTC_TR_HU)  >> 16);
    if (RTC->TR & RTC_TR_PM) {
        /* 12PM is noon */
        if (time->tm_hour != 12) {
            time->tm_hour += 12;
        }
    }
    else if ((RTC->CR & RTC_CR_FMT) && time->tm_hour == 12) {
        /* 12AM is midnight */
        time->tm_hour -= 12;
    }
    time->tm_min  = (((RTC->TR & RTC_TR_MNT) >> 12) * 10) + ((RTC->TR & RTC_TR_MNU) >>  8);
    time->tm_sec  = (((RTC->TR & RTC_TR_ST)  >>  4) * 10) + ((RTC->TR & RTC_TR_SU)  >>  0);
    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* Enable write access to RTC registers */
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
    PWR->CR |= PWR_CR_DBP;

    /* Unlock RTC write protection */
    RTC->WPR = RTC_WRITE_PROTECTION_KEY1;
    RTC->WPR = RTC_WRITE_PROTECTION_KEY2;

    /* Enter RTC Init mode */
    RTC->ISR |= RTC_ISR_INIT;
    while ((RTC->ISR & RTC_ISR_INITF) == 0) {}

    RTC->CR &= ~(RTC_CR_ALRAE);
    while ((RTC->ISR & RTC_ISR_ALRAWF) == 0) {}

    RTC->ALRMAR &= ~(RTC_ALRMAR_MSK1 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4);
    RTC->ALRMAR = ((((uint32_t)byte2bcd(time->tm_mday) << 24) & (RTC_ALRMAR_DT | RTC_ALRMAR_DU)) |
                   (((uint32_t)byte2bcd(time->tm_hour) << 16) & (RTC_ALRMAR_HT | RTC_ALRMAR_HU)) |
                   (((uint32_t)byte2bcd(time->tm_min) <<  8) & (RTC_ALRMAR_MNT | RTC_ALRMAR_MNU)) |
                   (((uint32_t)byte2bcd(time->tm_sec) <<  0) & (RTC_ALRMAR_ST | RTC_ALRMAR_SU)));
    /* Enable Alarm A */
    RTC->CR |= RTC_CR_ALRAE;
    RTC->CR |= RTC_CR_ALRAIE;
    RTC->ISR &= ~(RTC_ISR_ALRAF);

    /* Exit RTC init mode */
    RTC->ISR &= (uint32_t) ~RTC_ISR_INIT;
    /* Enable RTC write protection */
    RTC->WPR = 0xFF;

    EXTI->IMR  |= EXTI_IMR_MR17;
    EXTI->RTSR |= EXTI_RTSR_TR17;
#if defined(CPU_FAM_STM32F0)
    NVIC_SetPriority(RTC_IRQn, 10);
    NVIC_EnableIRQ(RTC_IRQn);
#else
    NVIC_SetPriority(RTC_Alarm_IRQn, 10);
    NVIC_EnableIRQ(RTC_Alarm_IRQn);
#endif

    rtc_callback.cb = cb;
    rtc_callback.arg = arg;

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    time->tm_year = MCU_YEAR_OFFSET;
    time->tm_year += (((RTC->DR     & RTC_DR_YT)      >> 20) * 10) + ((RTC->DR & RTC_DR_YU)          >> 16);
    time->tm_mon  = (((RTC->DR     & RTC_DR_MT)      >> 12) * 10) + ((RTC->DR & RTC_DR_MU)          >>  8) - 1;
    time->tm_mday = (((RTC->ALRMAR & RTC_ALRMAR_DT)  >> 28) * 10) + ((RTC->ALRMAR & RTC_ALRMAR_DU)  >> 24);
    time->tm_hour = (((RTC->ALRMAR & RTC_ALRMAR_HT)  >> 20) * 10) + ((RTC->ALRMAR & RTC_ALRMAR_HU)  >> 16);
    if ((RTC->ALRMAR & RTC_ALRMAR_PM) && (RTC->CR & RTC_CR_FMT)) {
        time->tm_hour += 12;
    }
    time->tm_min  = (((RTC->ALRMAR & RTC_ALRMAR_MNT) >> 12) * 10) + ((RTC->ALRMAR & RTC_ALRMAR_MNU) >>  8);
    time->tm_sec  = (((RTC->ALRMAR & RTC_ALRMAR_ST)  >>  4) * 10) + ((RTC->ALRMAR & RTC_ALRMAR_SU)  >>  0);
    return 0;
}

void rtc_clear_alarm(void)
{
    /* Disable Alarm A */
    RTC->CR &= RTC_CR_ALRAE;
    RTC->CR &= RTC_CR_ALRAIE;

    rtc_callback.cb = NULL;
    rtc_callback.arg = NULL;
}

void rtc_poweron(void)
{
#if defined(CPU_FAM_STM32L1)
    /* Reset RTC domain */
    RCC->CSR |= RCC_CSR_RTCRST;
    RCC->CSR &= ~(RCC_CSR_RTCRST);

    /* Enable the LSE clock (external 32.768 kHz oscillator) */
    RCC->CSR &= ~(RCC_CSR_LSEON);
    RCC->CSR &= ~(RCC_CSR_LSEBYP);
    RCC->CSR |= RCC_CSR_LSEON;
    while ( (RCC->CSR & RCC_CSR_LSERDY) == 0 ) {}

    /* Switch RTC to LSE clock source */
    RCC->CSR &= ~(RCC_CSR_RTCSEL);
    RCC->CSR |= RCC_CSR_RTCSEL_LSE;

    /* Enable the RTC */
    RCC->CSR |= RCC_CSR_RTCEN;
#else
    /* Reset RTC domain */
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~(RCC_BDCR_BDRST);

    /* Enable the LSE clock (external 32.768 kHz oscillator) */
    RCC->BDCR &= ~(RCC_BDCR_LSEON);
    RCC->BDCR &= ~(RCC_BDCR_LSEBYP);
    RCC->BDCR |= RCC_BDCR_LSEON;
    while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0) {}

    /* Switch RTC to LSE clock source */
    RCC->BDCR &= ~(RCC_BDCR_RTCSEL);
    RCC->BDCR |= RCC_BDCR_RTCSEL_0;

    /* Enable the RTC */
    RCC->BDCR |= RCC_BDCR_RTCEN;
#endif
}

void rtc_poweroff(void)
{
#if defined(CPU_FAM_STM32L1)
    /* Reset RTC domain */
    RCC->CSR |= RCC_CSR_RTCRST;
    RCC->CSR &= ~(RCC_CSR_RTCRST);
    /* Disable the RTC */
    RCC->CSR &= ~RCC_CSR_RTCEN;
    /* Disable LSE clock */
    RCC->CSR &= ~(RCC_CSR_LSEON);
#else
    /* Reset RTC domain */
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~(RCC_BDCR_BDRST);
    /* Disable the RTC */
    RCC->BDCR &= ~RCC_BDCR_RTCEN;
    /* Disable LSE clock */
    RCC->BDCR &= ~(RCC_BDCR_LSEON);
#endif
}

#if defined(CPU_FAM_STM32F0)
void isr_rtc(void)
#else
void isr_rtc_alarm(void)
#endif
{
    if (RTC->ISR & RTC_ISR_ALRAF) {
        if (rtc_callback.cb != NULL) {
            rtc_callback.cb(rtc_callback.arg);
        }
        RTC->ISR &= ~RTC_ISR_ALRAF;
    }
    EXTI->PR |= EXTI_PR_PR17;
    cortexm_isr_end();
}

/**
 * Convert a number from unsigned to BCD
 *
 * @param[in] value to be converted
 * @return BCD representation of the value
 */
static uint8_t byte2bcd(uint8_t value)
{
    uint8_t bcdhigh = 0;

    while (value >= 10) {
        bcdhigh++;
        value -= 10;
    }

    return  ((uint8_t)(bcdhigh << 4) | value);
}

#endif /* RTC_NUMOF */

#endif /* defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F2) || \
          defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32F4) || \
          defined(CPU_FAM_STM32L1) */
