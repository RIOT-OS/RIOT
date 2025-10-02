/*
 * SPDX-FileCopyrightText: 2015 Lari Lehtomäki
 * SPDX-FileCopyrightText: 2016 Laksh Bhatia
 * SPDX-FileCopyrightText: 2016-2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @{
 * @file
 * @brief       Low-level RTC driver implementation
 *
 * @author      Lari Lehtomäki <lari@lehtomaki.fi>
 * @author      Laksh Bhatia <bhatialaksh3@gmail.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "stmclk.h"
#include "periph/rtc.h"

/* map some CPU specific register names */
#if defined (CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define EN_REG              (RCC->CSR)
#define EN_BIT              (RCC_CSR_RTCEN)
#define CLKSEL_MASK         (RCC_CSR_RTCSEL)
#define CLKSEL_LSE          (RCC_CSR_RTCSEL_LSE)
#define CLKSEL_LSI          (RCC_CSR_RTCSEL_LSI)
#else
#define EN_REG              (RCC->BDCR)
#define EN_BIT              (RCC_BDCR_RTCEN)
#define CLKSEL_MASK         (RCC_BDCR_RTCSEL_0 | RCC_BDCR_RTCSEL_1)
#define CLKSEL_LSE          (RCC_BDCR_RTCSEL_0)
#define CLKSEL_LSI          (RCC_BDCR_RTCSEL_1)
#endif

/* map some EXTI register names */
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4)
#define EXTI_REG_RTSR       (EXTI->RTSR1)
#define EXTI_REG_FTSR       (EXTI->FTSR1)
#define EXTI_REG_PR         (EXTI->PR1)
#define EXTI_REG_IMR        (EXTI->IMR1)
#elif defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32U5)
#define EXTI_REG_RTSR       (EXTI->RTSR1)
#define EXTI_REG_FTSR       (EXTI->FTSR1)
#define EXTI_REG_PR         (EXTI->RPR1)
#define EXTI_REG_IMR        (EXTI->IMR1)
#elif defined(CPU_FAM_STM32L5)
#define EXTI_REG_IMR        (EXTI->IMR1)
#else
#define EXTI_REG_RTSR       (EXTI->RTSR)
#define EXTI_REG_FTSR       (EXTI->FTSR)
#define EXTI_REG_PR         (EXTI->PR)
#define EXTI_REG_IMR        (EXTI->IMR)
#endif

/* map some RTC register names and bitfield */
#if defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0)
#define RTC_REG_ISR         RTC->ICSR
#define RTC_REG_SR          RTC->SR
#define RTC_REG_SCR         RTC->SCR
#define RTC_ISR_RSF         RTC_ICSR_RSF
#define RTC_ISR_INIT        RTC_ICSR_INIT
#define RTC_ISR_INITF       RTC_ICSR_INITF
#define RTC_ISR_ALRAWF      RTC_ICSR_ALRAWF
#define RTC_ISR_ALRAF       RTC_SR_ALRAF
#elif defined(CPU_FAM_STM32L5)
#define RTC_REG_ISR         RTC->ICSR
#define RTC_REG_SR          RTC->SR
#define RTC_REG_SCR         RTC->SCR
#define RTC_ISR_RSF         RTC_ICSR_RSF
#define RTC_ISR_INIT        RTC_ICSR_INIT
#define RTC_ISR_INITF       RTC_ICSR_INITF
#elif defined(CPU_FAM_STM32U5)
#define RTC_REG_ISR         RTC->ICSR
#define RTC_REG_SR          RTC->SR
#define RTC_REG_SCR         RTC->SCR
#define RTC_ISR_RSF         RTC_ICSR_RSF
#define RTC_ISR_INIT        RTC_ICSR_INIT
#define RTC_ISR_INITF       RTC_ICSR_INITF
#define RTC_ISR_ALRAF       RTC_SR_ALRAF
#else
#define RTC_REG_ISR         RTC->ISR
#endif

/* interrupt line name mapping */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5)
#define IRQN                (RTC_IRQn)
#define ISR_NAME            isr_rtc
#elif defined(CPU_FAM_STM32G0)
#define IRQN                (RTC_TAMP_IRQn)
#define ISR_NAME            isr_rtc_tamp
#else
#define IRQN                (RTC_Alarm_IRQn)
#define ISR_NAME            isr_rtc_alarm
#endif

/* EXTI bitfield mapping */
#if defined(CPU_FAM_STM32L4)
#define EXTI_IMR_BIT        (EXTI_IMR1_IM18)
#define EXTI_FTSR_BIT       (EXTI_FTSR1_FT18)
#define EXTI_RTSR_BIT       (EXTI_RTSR1_RT18)
#define EXTI_PR_BIT         (EXTI_PR1_PIF18)
#elif defined(CPU_FAM_STM32L5)
#define EXTI_IMR_BIT        (EXTI_IMR1_IM17)
#elif defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4)
#define EXTI_IMR_BIT        (EXTI_IMR1_IM17)
#define EXTI_FTSR_BIT       (EXTI_FTSR1_FT17)
#define EXTI_RTSR_BIT       (EXTI_RTSR1_RT17)
#define EXTI_PR_BIT         (EXTI_PR1_PIF17)
#elif defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32U5)
#define EXTI_IMR_BIT        (EXTI_IMR1_IM11)
#define EXTI_FTSR_BIT       (EXTI_FTSR1_FT11)
#define EXTI_RTSR_BIT       (EXTI_RTSR1_RT11)
#define EXTI_PR_BIT         (EXTI_RPR1_RPIF11)
#else
#if defined(CPU_FAM_STM32L0)
#define EXTI_IMR_BIT        (EXTI_IMR_IM17)
#else
#define EXTI_IMR_BIT        (EXTI_IMR_MR17)
#endif
#define EXTI_FTSR_BIT       (EXTI_FTSR_TR17)
#define EXTI_RTSR_BIT       (EXTI_RTSR_TR17)
#define EXTI_PR_BIT         (EXTI_PR_PR17)
#endif

/* write protection values */
#define WPK1                (0xCA)
#define WPK2                (0x53)

/* define TR, DR, and ALRMAR position and masks */
#define TR_H_MASK           (RTC_TR_HU | RTC_TR_HT)
#define TR_M_MASK           (RTC_TR_MNU | RTC_TR_MNT)
#define TR_S_MASK           (RTC_TR_SU | RTC_TR_ST)
#define DR_Y_MASK           (RTC_DR_YU | RTC_DR_YT)
#define DR_M_MASK           (RTC_DR_MU | RTC_DR_MT)
#define DR_D_MASK           (RTC_DR_DU | RTC_DR_DT)
#define ALRM_D_MASK         (RTC_ALRMAR_DU | RTC_ALRMAR_DT)
#define ALRM_H_MASK         (RTC_ALRMAR_HU | RTC_ALRMAR_HT)
#define ALRM_M_MASK         (RTC_ALRMAR_MNU | RTC_ALRMAR_MNT)
#define ALRM_S_MASK         (RTC_ALRMAR_SU | RTC_ALRMAR_ST)
#ifndef RTC_DR_YU_Pos
#define RTC_DR_YU_Pos       (16U)
#endif
#ifndef RTC_DR_MU_Pos
#define RTC_DR_MU_Pos       (8U)
#endif
#ifndef RTC_DR_DU_Pos
#define RTC_DR_DU_Pos       (0U)
#endif
#ifndef RTC_TR_HU_Pos
#define RTC_TR_HU_Pos       (16U)
#endif
#ifndef RTC_TR_MNU_Pos
#define RTC_TR_MNU_Pos      (8U)
#endif
#ifndef RTC_TR_SU_Pos
#define RTC_TR_SU_Pos       (0U)
#endif
#ifndef RTC_ALRMAR_DU_Pos
#define RTC_ALRMAR_DU_Pos   (24U)
#endif
#ifndef RTC_ALRMAR_HU_Pos
#define RTC_ALRMAR_HU_Pos   (16U)
#endif
#ifndef RTC_ALRMAR_MNU_Pos
#define RTC_ALRMAR_MNU_Pos  (8U)
#endif
#ifndef RTC_ALRMAR_SU_Pos
#define RTC_ALRMAR_SU_Pos   (0U)
#endif

/* figure out sync and async prescaler */
#if IS_ACTIVE(CONFIG_BOARD_HAS_LSE)
#define PRE_SYNC            (255)
#define PRE_ASYNC           (127)
#elif (CLOCK_LSI == 40000)
#define PRE_SYNC            (319)
#define PRE_ASYNC           (124)
#elif (CLOCK_LSI == 37000)
#define PRE_SYNC            (295)
#define PRE_ASYNC           (124)
#elif (CLOCK_LSI == 32000)
#define PRE_SYNC            (249)
#define PRE_ASYNC           (127)
#else
#error "rtc: unable to determine RTC SYNC and ASYNC prescalers from LSI value"
#endif

/* struct tm counts years since 1900 but RTC has only two-digit year, hence the offset */
#define YEAR_OFFSET         (RIOT_EPOCH - 1900)

/* Use a magic number to determine the initial RTC source. This will be used
   to know if a reset of the RTC is required at initialization. */
#if IS_ACTIVE(CONFIG_BOARD_HAS_LSE)
#define MAGIC_CLCK_NUMBER       (0x1970)
#else
#define MAGIC_CLCK_NUMBER       (0x1971)
#endif

static struct {
    rtc_alarm_cb_t cb;          /**< callback called from RTC interrupt */
    void *arg;                  /**< argument passed to the callback */
} isr_ctx;

static uint32_t val2bcd(int val, int shift, uint32_t mask)
{
    uint32_t bcdhigh = 0;

    while (val >= 10) {
        bcdhigh++;
        val -= 10;
    }

    return ((((bcdhigh << 4) | val) << shift) & mask);
}

static int bcd2val(uint32_t val, int shift, uint32_t mask)
{
    int tmp = (int)((val & mask) >> shift);
    return (((tmp >> 4) * 10) + (tmp & 0x0f));
}

void rtc_unlock(void)
{
    /* enable backup clock domain */
    stmclk_dbp_unlock();
    /* unlock RTC */
    RTC->WPR = WPK1;
    RTC->WPR = WPK2;
    /* enter RTC init mode */
    RTC_REG_ISR |= RTC_ISR_INIT;
    while (!(RTC_REG_ISR & RTC_ISR_INITF)) {}
}

void rtc_lock(void)
{
    /* exit RTC init mode */
    RTC_REG_ISR &= ~RTC_ISR_INIT;
    while (RTC_REG_ISR & RTC_ISR_INITF) {}
    /* lock RTC device */
    RTC->WPR = 0xff;
    /* disable backup clock domain */
    stmclk_dbp_lock();
}

void rtc_init(void)
{
    stmclk_dbp_unlock();
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    /* Compare the stored magic number with the current one. If it's different
       it means the clock source has changed and thus a RTC reset is
       required. */
    if (RTC->BKP0R != MAGIC_CLCK_NUMBER) {
        RCC->CSR |= RCC_CSR_RTCRST;
        RCC->CSR &= ~RCC_CSR_RTCRST;
        RTC->BKP0R = MAGIC_CLCK_NUMBER; /* Store the new magic number */
    }
#endif
    stmclk_dbp_lock();

    /* enable low frequency clock */
    stmclk_enable_lfclk();

    /* select input clock and enable the RTC */
    stmclk_dbp_unlock();
#if defined(CPU_FAM_STM32L5)
    periph_clk_en(APB1, RCC_APB1ENR1_RTCAPBEN);
#elif defined(CPU_FAM_STM32G0)
    periph_clk_en(APB1, RCC_APBENR1_RTCAPBEN);
#elif defined(CPU_FAM_STM32U5)
    periph_clk_en(APB3, RCC_APB3ENR_RTCAPBEN);
#endif
    EN_REG &= ~(CLKSEL_MASK);
#if IS_ACTIVE(CONFIG_BOARD_HAS_LSE)
    EN_REG |= (CLKSEL_LSE | EN_BIT);
#else
    EN_REG |= (CLKSEL_LSI | EN_BIT);
#endif

    rtc_unlock();
    /* reset configuration */
    RTC->CR = 0;
    RTC_REG_ISR = RTC_ISR_INIT;
    /* configure prescaler (RTC PRER) */
    RTC->PRER = (PRE_SYNC | (PRE_ASYNC << 16));
    rtc_lock();

    /* configure the EXTI channel, as RTC interrupts are routed through it.
     * Needs to be configured to trigger on rising edges. */
    EXTI_REG_IMR  |= EXTI_IMR_BIT;
#if !defined(CPU_FAM_STM32L5)
    EXTI_REG_FTSR &= ~(EXTI_FTSR_BIT);
    EXTI_REG_RTSR |= EXTI_RTSR_BIT;
    EXTI_REG_PR   = EXTI_PR_BIT;
#endif
    /* enable global RTC interrupt */
    NVIC_EnableIRQ(IRQN);
}

int rtc_set_time(struct tm *time)
{
    /* normalize input */
    rtc_tm_normalize(time);

    rtc_unlock();

    RTC->DR = (val2bcd((time->tm_year - YEAR_OFFSET), RTC_DR_YU_Pos, DR_Y_MASK) |
               val2bcd(time->tm_mon + 1,  RTC_DR_MU_Pos, DR_M_MASK) |
               val2bcd(time->tm_mday, RTC_DR_DU_Pos, DR_D_MASK));
    RTC->TR = (val2bcd(time->tm_hour, RTC_TR_HU_Pos, TR_H_MASK) |
               val2bcd(time->tm_min,  RTC_TR_MNU_Pos, TR_M_MASK) |
               val2bcd(time->tm_sec,  RTC_TR_SU_Pos, TR_S_MASK));
    rtc_lock();
    while (!(RTC_REG_ISR & RTC_ISR_RSF)) {}

    return 0;
}

int rtc_get_time(struct tm *time)
{
    /* save current time */
    uint32_t tr = RTC->TR;
    uint32_t dr = RTC->DR;
    time->tm_year = bcd2val(dr, RTC_DR_YU_Pos, DR_Y_MASK) + YEAR_OFFSET;
    time->tm_mon  = bcd2val(dr, RTC_DR_MU_Pos, DR_M_MASK) - 1;
    time->tm_mday = bcd2val(dr, RTC_DR_DU_Pos, DR_D_MASK);
    time->tm_hour = bcd2val(tr, RTC_TR_HU_Pos, TR_H_MASK);
    time->tm_min  = bcd2val(tr, RTC_TR_MNU_Pos, TR_M_MASK);
    time->tm_sec  = bcd2val(tr, RTC_TR_SU_Pos, TR_S_MASK);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* normalize input */
    rtc_tm_normalize(time);

    /* disable existing alarm (if enabled) */
    rtc_clear_alarm();

    rtc_unlock();

    /* save callback and argument */
    isr_ctx.cb = cb;
    isr_ctx.arg = arg;

    /* set wakeup time */
    RTC->ALRMAR = (val2bcd(time->tm_mday, RTC_ALRMAR_DU_Pos, ALRM_D_MASK) |
                   val2bcd(time->tm_hour, RTC_ALRMAR_HU_Pos, ALRM_H_MASK) |
                   val2bcd(time->tm_min, RTC_ALRMAR_MNU_Pos, ALRM_M_MASK) |
                   val2bcd(time->tm_sec,  RTC_ALRMAR_SU_Pos, ALRM_S_MASK));

    /* Enable Alarm A */
#if !defined(CPU_FAM_STM32L5)
    RTC_REG_ISR &= ~(RTC_ISR_ALRAF);
#else
    RTC_REG_SCR = RTC_SCR_CALRAF;
#endif
    RTC->CR |= (RTC_CR_ALRAE | RTC_CR_ALRAIE);

    rtc_lock();

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    uint32_t dr = RTC->DR;
    uint32_t alrm = RTC->ALRMAR;

    time->tm_year = bcd2val(dr, RTC_DR_YU_Pos, DR_Y_MASK) + YEAR_OFFSET;
    time->tm_mon  = bcd2val(dr, RTC_DR_MU_Pos, DR_M_MASK) - 1;
    time->tm_mday = bcd2val(alrm, RTC_ALRMAR_DU_Pos, ALRM_D_MASK);
    time->tm_hour = bcd2val(alrm, RTC_ALRMAR_HU_Pos, ALRM_H_MASK);
    time->tm_min  = bcd2val(alrm, RTC_ALRMAR_MNU_Pos, ALRM_M_MASK);
    time->tm_sec  = bcd2val(alrm, RTC_ALRMAR_SU_Pos, ALRM_S_MASK);

    return 0;
}

void rtc_clear_alarm(void)
{
    rtc_unlock();

    RTC->CR &= ~(RTC_CR_ALRAE | RTC_CR_ALRAIE);

#if !defined(CPU_FAM_STM32L5) && !defined(CPU_FAM_STM32U5)
    while (!(RTC_REG_ISR & RTC_ISR_ALRAWF)) {}
#else
    RTC_REG_SCR = RTC_SCR_CALRAF;
#endif

    isr_ctx.cb = NULL;
    isr_ctx.arg = NULL;

    rtc_lock();
}

void rtc_poweron(void)
{
    stmclk_dbp_unlock();
    EN_REG |= EN_BIT;
    stmclk_dbp_lock();
}

void rtc_poweroff(void)
{
    stmclk_dbp_unlock();
    EN_REG &= ~EN_BIT;
    stmclk_dbp_lock();
}

void ISR_NAME(void)
{
#if !defined(CPU_FAM_STM32L5) && !defined(CPU_FAM_STM32G0) && !defined(CPU_FAM_STM32U5)
    if (RTC_REG_ISR & RTC_ISR_ALRAF) {
        if (isr_ctx.cb != NULL) {
            isr_ctx.cb(isr_ctx.arg);
        }
        RTC_REG_ISR &= ~RTC_ISR_ALRAF;
    }
    EXTI_REG_PR = EXTI_PR_BIT; /* only clear the associated bit */
#else
    if (RTC_REG_SR & RTC_SR_ALRAF) {
        if (isr_ctx.cb != NULL) {
            isr_ctx.cb(isr_ctx.arg);
        }
        /* RTC registers are write access protected, DBP bit must be set to enable access */
        stmclk_dbp_unlock();
        RTC_REG_SCR = RTC_SCR_CALRAF;
        /* Lock to avoid parasitic write access */
        stmclk_dbp_lock();
    }
#endif
    cortexm_isr_end();
}
