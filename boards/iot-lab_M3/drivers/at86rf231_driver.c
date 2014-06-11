#include <stdio.h>
#include <stddef.h>

#include "cpu.h"
#include "sched.h"
#include "vtimer.h"

#include "periph/gpio.h"
#include "periph_conf.h"
#include "board.h"

#include "at86rf231.h"
#include "at86rf231_spi.h"
#include "at86rf231_spi1.h"

extern volatile unsigned int sched_context_switch_request;

/*
SPI1
  SCLK : PA5
  MISO : PA6
  MOSI : PA7
  CS : PA4

GPIO
  IRQ0 : PC4 : Frame buff empty indicator
  DIG2 : ? : RX Frame Time stamping XXX : NOT USED
  Reset : PC1 : active low, enable chip
  SLEEP : PA2 : control sleep, tx & rx state
*/

inline static void RESET_CLR(void)
{
    GPIOC->BRR = 1 << 1;
}
inline static void RESET_SET(void)
{
    GPIOC->BSRR = 1 << 1;
}
inline static void CSn_SET(void)
{
    GPIOA->BSRR = 1 << 4;
}
inline static void CSn_CLR(void)
{
    GPIOA->BRR = 1 << 4;
}
inline static void SLEEP_CLR(void)
{
    GPIOA->BRR = 1 << 2;
}

uint8_t at86rf231_get_status(void)
{
    return at86rf231_reg_read(AT86RF231_REG__TRX_STATUS)
           & AT86RF231_TRX_STATUS_MASK__TRX_STATUS;
}


extern void at86rf231_rx_irq(void);
static
void enable_exti_interrupt(void)
{
    // EXTI_InitTypeDef   EXTI_InitStructure;

    // EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    // EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    // EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    // EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    // EXTI_Init(&EXTI_InitStructure);
    gpio_init_int(GPIO_6, GPIO_NOPULL, GPIO_RISING, at86rf231_rx_irq);
}
static
void disable_exti_interrupt(void)
{
    // EXTI_InitTypeDef   EXTI_InitStructure;

    // EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    // EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    // EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    // EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    // EXTI_Init(&EXTI_InitStructure);
#warning not implemented yet
}


void at86rf231_gpio_spi_interrupts_init(void)
{
    /* SPI1 init */
    at86rf231_spi1_init();

    /* IRQ0 : PC4, INPUT and IRQ */
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    // GPIO_Init(GPIOC, &GPIO_InitStructure);
    gpio_init_in(GPIO_4, GPIO_NOPULL);

    /* Enable AFIO clock */
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* Connect EXTI4 Line to PC4 pin */
    // GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);

    /* Configure EXTI4 line */
    enable_exti_interrupt();

    /* Enable and set EXTI4 Interrupt to the lowest priority */
    // NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);

    /* Init GPIOs */

    /* CS & SLEEP */
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);
    gpio_init_out(GPIO_2, GPIO_NOPULL);
    gpio_init_out(GPIO_4, GPIO_NOPULL);

    /* RESET */
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    // GPIO_Init(GPIOC, &GPIO_InitStructure);
    gpio_init_out(GPIO_1, GPIO_NOPULL);
}

void at86rf231_reset(void)
{
    /* force reset */
    RESET_CLR();
    CSn_SET();
    SLEEP_CLR();

    vtimer_usleep(AT86RF231_TIMING__RESET);

    RESET_SET();

    /* Wait until TRX_OFF is entered */
    vtimer_usleep(AT86RF231_TIMING__RESET_TO_TRX_OFF);

    /* Send a FORCE TRX OFF command */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__FORCE_TRX_OFF);

    /* Wait until TRX_OFF state is entered from P_ON */
    vtimer_usleep(AT86RF231_TIMING__SLEEP_TO_TRX_OFF);

    /* busy wait for TRX_OFF state */
    uint8_t status;
    uint8_t max_wait = 100;   // TODO : move elsewhere, this is in 10us

    do {
        status = at86rf231_get_status();

        vtimer_usleep(10);

        if (!--max_wait) {
            printf("at86rf231 : ERROR : could not enter TRX_OFF mode");
            break;
        }
    }
    while ((status & AT86RF231_TRX_STATUS_MASK__TRX_STATUS) != AT86RF231_TRX_STATUS__TRX_OFF);
}

void at86rf231_spi_select(void)
{
    CSn_CLR();
}
void at86rf231_spi_unselect(void)
{
    CSn_SET();
}

void at86rf231_enable_interrupts(void)
{
    enable_exti_interrupt();
}
void at86rf231_disable_interrupts(void)
{
    disable_exti_interrupt();
}

// extern void at86rf231_rx_irq(void);
// __attribute__((naked))
// void EXTI4_IRQHandler(void)
// {
//     save_context();

//     if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
//         /* IRQ_3 (TRX_END), read Frame Buffer */
//         EXTI_ClearITPendingBit(EXTI_Line4);

//         at86rf231_rx_irq();

//         if (sched_context_switch_request) {
//             /* scheduler */
//             thread_yield();
//         }
//     }

//     restore_context();
// }
