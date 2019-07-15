/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32l4
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32L4 specific interrupt vectors */
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_aes1(void);
WEAK_DEFAULT void isr_aes2(void);
WEAK_DEFAULT void isr_c2sev_pwr_c2h(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_crs(void);
WEAK_DEFAULT void isr_dma1_channel1(void);
WEAK_DEFAULT void isr_dma1_channel2(void);
WEAK_DEFAULT void isr_dma1_channel3(void);
WEAK_DEFAULT void isr_dma1_channel4(void);
WEAK_DEFAULT void isr_dma1_channel5(void);
WEAK_DEFAULT void isr_dma1_channel6(void);
WEAK_DEFAULT void isr_dma1_channel7(void);
WEAK_DEFAULT void isr_dma2_channel1(void);
WEAK_DEFAULT void isr_dma2_channel2(void);
WEAK_DEFAULT void isr_dma2_channel3(void);
WEAK_DEFAULT void isr_dma2_channel4(void);
WEAK_DEFAULT void isr_dma2_channel5(void);
WEAK_DEFAULT void isr_dma2_channel6(void);
WEAK_DEFAULT void isr_dma2_channel7(void);
WEAK_DEFAULT void isr_dmamux1_ovr(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_hsem(void);
WEAK_DEFAULT void isr_ipcc_c1_rx(void);
WEAK_DEFAULT void isr_ipcc_c1_tx(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_lptim1(void);
WEAK_DEFAULT void isr_lptim2(void);
WEAK_DEFAULT void isr_lpuart1(void);
WEAK_DEFAULT void isr_pka(void);
WEAK_DEFAULT void isr_pvd_pvm(void);
WEAK_DEFAULT void isr_quadspi(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_sai1(void);
WEAK_DEFAULT void isr_sotf_pwr_ble_802_rfphase(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_tamp_stamp(void);
WEAK_DEFAULT void isr_tim1_brk(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim1_up_tim16(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tsc(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usb_hp(void);
WEAK_DEFAULT void isr_usb_lp(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [WWDG_IRQn                          ] = isr_wwdg,                     /* [ 0] Window WatchDog Interrupt */
    [PVD_PVM_IRQn                       ] = isr_pvd_pvm,                  /* [ 1] PVD and PVM detector */
    [TAMP_STAMP_LSECSS_IRQn             ] = isr_tamp_stamp,               /* [ 2] RTC Tamper and TimeStamp Interrupts and LSECSS Interrupts */
    [RTC_WKUP_IRQn                      ] = isr_rtc_wkup,                 /* [ 3] RTC Wakeup Interrupt  */
    [FLASH_IRQn                         ] = isr_flash,                    /* [ 4] FLASH global Interrupt */
    [RCC_IRQn                           ] = isr_rcc,                      /* [ 5] RCC global Interrupt */
    [EXTI0_IRQn                         ] = isr_exti,                     /* [ 6] EXTI Line0 Interrupt */
    [EXTI1_IRQn                         ] = isr_exti,                     /* [ 7] EXTI Line1 Interrupt */
    [EXTI2_IRQn                         ] = isr_exti,                     /* [ 8] EXTI Line2 Interrupt */
    [EXTI3_IRQn                         ] = isr_exti,                     /* [ 9] EXTI Line3 Interrupt */
    [EXTI4_IRQn                         ] = isr_exti,                     /* [10] EXTI Line4 Interrupt */
    [DMA1_Channel1_IRQn                 ] = isr_dma1_channel1,            /* [11] DMA1 Channel 1 global Interrupt */
    [DMA1_Channel2_IRQn                 ] = isr_dma1_channel2,            /* [12] DMA1 Channel 2 global Interrupt */
    [DMA1_Channel3_IRQn                 ] = isr_dma1_channel3,            /* [13] DMA1 Channel 3 global Interrupt */
    [DMA1_Channel4_IRQn                 ] = isr_dma1_channel4,            /* [14] DMA1 Channel 4 global Interrupt */
    [DMA1_Channel5_IRQn                 ] = isr_dma1_channel5,            /* [15] DMA1 Channel 5 global Interrupt */
    [DMA1_Channel6_IRQn                 ] = isr_dma1_channel6,            /* [16] DMA1 Channel 6 global Interrupt */
    [DMA1_Channel7_IRQn                 ] = isr_dma1_channel7,            /* [17] DMA1 Channel 7 global Interrupt */
    [ADC1_IRQn                          ] = isr_adc1,                     /* [18] ADC1 Interrup */
    [USB_HP_IRQn                        ] = isr_usb_hp,                   /* [19] USB High Priority Interrupt */
    [USB_LP_IRQn                        ] = isr_usb_lp,                   /* [20] USB Low Priority Interrupt (including USB wakeup) */
    [C2SEV_PWR_C2H_IRQn                 ] = isr_c2sev_pwr_c2h,            /* [21] CPU2 SEV Interrupt */
    [COMP_IRQn                          ] = isr_comp,                     /* [62] COMP1 and COMP2 Interrupts */
    [EXTI9_5_IRQn                       ] = isr_exti,                     /* [23] External Line[9:5] Interrupts */
    [TIM1_BRK_IRQn                      ] = isr_tim1_brk,                 /* [24] TIM1 Break interrupt  */
    [TIM1_UP_TIM16_IRQn                 ] = isr_tim1_up_tim16,            /* [25] TIM1 Update Interrupt and TIM16 global interrupt */
    [TIM1_CC_IRQn                       ] = isr_tim1_cc,                  /* [27] TIM1 Capture Compare Interrupt */
    [TIM2_IRQn                          ] = isr_tim2,                     /* [28] TIM2 global Interrupt */
    [PKA_IRQn                           ] = isr_pka,                      /* [29] PKA Interrup */
    [I2C1_EV_IRQn                       ] = isr_i2c1_ev,                  /* [30] I2C1 Event Interrupt */
    [I2C1_ER_IRQn                       ] = isr_i2c1_er,                  /* [31] I2C1 Error Interrupt */
    [I2C3_EV_IRQn                       ] = isr_i2c3_ev,                  /* [32] I2C3 Event Interrupt */
    [I2C3_ER_IRQn                       ] = isr_i2c3_er,                  /* [33] I2C3 Error Interrupt */
    [SPI1_IRQn                          ] = isr_spi1,                     /* [34] SPI1 global Interrupt */
    [SPI2_IRQn                          ] = isr_spi2,                     /* [35] SPI2 global Interrupt */
    [USART1_IRQn                        ] = isr_usart1,                   /* [36] USART1 global Interrupt */
    [LPUART1_IRQn                       ] = isr_lpuart1,                  /* [37] LP UART1 interrupt */
    [SAI1_IRQn                          ] = isr_sai1,                     /* [38] Serial Audio Interface 1 global interrupt */
    [TSC_IRQn                           ] = isr_tsc,                      /* [39] Touch Sense Controller global interrupt */
    [EXTI15_10_IRQn                     ] = isr_exti,                     /* [40] External Line[15:10] Interrupts */
    [RTC_Alarm_IRQn                     ] = isr_rtc_alarm,                /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    [CRS_IRQn                           ] = isr_crs,                      /* [42] CRS global interrupt */
    [PWR_SOTF_BLEACT_802ACT_RFPHASE_IRQn] = isr_sotf_pwr_ble_802_rfphase, /* [43] PWR switching on the fly interrupt
                                                                                  PWR end of BLE activity interrupt
                                                                                  PWR end of 802.15.4 (Zigbee) activity interrupt
                                                                                  PWR end of critical radio phase interrupt */
    [IPCC_C1_RX_IRQn                    ] = isr_ipcc_c1_rx,               /* [44] IPCC RX Occupied Interrupt */
    [IPCC_C1_TX_IRQn                    ] = isr_ipcc_c1_tx,               /* [45] HIPCC TX Free Interrupt */
    [HSEM_IRQn                          ] = isr_hsem,                     /* [46] HSEM Interrupt */
    [LPTIM1_IRQn                        ] = isr_lptim1,                   /* [47] LP TIM1 interrupt */
    [LPTIM2_IRQn                        ] = isr_lptim2,                   /* [48] LP TIM2 interrupt */
    [LCD_IRQn                           ] = isr_lcd,                      /* [49] LCD global interrupt */
    [QUADSPI_IRQn                       ] = isr_quadspi,                  /* [50] Quad SPI global interrupt */
    [AES1_IRQn                          ] = isr_aes1,                     /* [51] AES1 Interrup */
    [AES2_IRQn                          ] = isr_aes2,                     /* [52] AES2 Interrupt */
    [RNG_IRQn                           ] = isr_rng,                      /* [53] RNG global interrupt */
    [FPU_IRQn                           ] = isr_fpu,                      /* [54] FPU global interrupt */
    [DMA2_Channel1_IRQn                 ] = isr_dma2_channel1,            /* [55] DMA2 Channel 1 global Interrupt */
    [DMA2_Channel2_IRQn                 ] = isr_dma2_channel2,            /* [56] DMA2 Channel 2 global Interrupt */
    [DMA2_Channel3_IRQn                 ] = isr_dma2_channel3,            /* [57] DMA2 Channel 3 global Interrupt */
    [DMA2_Channel4_IRQn                 ] = isr_dma2_channel4,            /* [58] DMA2 Channel 4 global Interrupt */
    [DMA2_Channel5_IRQn                 ] = isr_dma2_channel5,            /* [69] DMA2 Channel 5 global Interrupt */
    [DMA2_Channel6_IRQn                 ] = isr_dma2_channel6,            /* [60] DMA2 Channel 6 global interrupt */
    [DMA2_Channel7_IRQn                 ] = isr_dma2_channel7,            /* [61] DMA2 Channel 7 global interrupt */
    [DMAMUX1_OVR_IRQn                   ] = isr_dmamux1_ovr,              /* [62] DMAMUX1 overrun global interrupt */
};
