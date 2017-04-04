#ifndef VECTORS_STM32F0_H
#define VECTORS_STM32F0_H

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
static void dummy_handler(void) {
    dummy_handler_default();
}

WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_ts(void);
WEAK_DEFAULT void isr_dma1_ch1(void);
WEAK_DEFAULT void isr_dma1_ch2_3(void);
WEAK_DEFAULT void isr_dma1_ch4_5(void);
WEAK_DEFAULT void isr_adc1_comp(void);
WEAK_DEFAULT void isr_tim1_brk_up_trg_com(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim14(void);
WEAK_DEFAULT void isr_tim15(void);
WEAK_DEFAULT void isr_tim16(void);
WEAK_DEFAULT void isr_tim17(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3_4(void);
WEAK_DEFAULT void isr_usart3_8(void);
WEAK_DEFAULT void isr_cec(void);
WEAK_DEFAULT void isr_usb(void);

#endif // VECTORS_STM32F0_H
