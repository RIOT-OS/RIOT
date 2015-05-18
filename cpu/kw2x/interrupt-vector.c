/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kw2x
 * @{
 *
 * @file
 * @brief       Interrupt vector definition for MKW2XDXXX MCUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdint.h>
#include "cpu-conf.h"
#include "fault_handlers.h"

/**
 * memory markers as defined in the linker script
 */
extern uint32_t _estack;

extern void reset_handler(void);

void dummy_handler(void)
{
    isr_unhandled();
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__((weak, alias("dummy_handler")));

/* MKW22D512 specific interrupt vector */
void isr_dma0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma1(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma2(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma3(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma4(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma5(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma6(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma7(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma8(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma9(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_dma10(void)                __attribute__((weak, alias("dummy_handler")));
void isr_dma11(void)                __attribute__((weak, alias("dummy_handler")));
void isr_dma12(void)                __attribute__((weak, alias("dummy_handler")));
void isr_dma13(void)                __attribute__((weak, alias("dummy_handler")));
void isr_dma14(void)                __attribute__((weak, alias("dummy_handler")));
void isr_dma15(void)                __attribute__((weak, alias("dummy_handler")));
void isr_dma_error(void)            __attribute__((weak, alias("dummy_handler")));
void isr_mcm(void)                  __attribute__((weak, alias("dummy_handler")));
void isr_ftfl(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_ftfl_collision(void)       __attribute__((weak, alias("dummy_handler")));
void isr_pmc(void)                  __attribute__((weak, alias("dummy_handler")));
void isr_llwu(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_wdog_ewm(void)             __attribute__((weak, alias("dummy_handler")));
void isr_rng(void)                  __attribute__((weak, alias("dummy_handler")));
void isr_i2c0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_i2c1(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_spi0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_spi1(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_i2s0_tx(void)              __attribute__((weak, alias("dummy_handler")));
void isr_i2s0_rx(void)              __attribute__((weak, alias("dummy_handler")));
void isr_uart0_rx_tx(void)          __attribute__((weak, alias("dummy_handler")));
void isr_uart0_err(void)            __attribute__((weak, alias("dummy_handler")));
void isr_uart1_rx_tx(void)          __attribute__((weak, alias("dummy_handler")));
void isr_uart1_err(void)            __attribute__((weak, alias("dummy_handler")));
void isr_uart2_rx_tx(void)          __attribute__((weak, alias("dummy_handler")));
void isr_uart2_err(void)            __attribute__((weak, alias("dummy_handler")));
void isr_adc0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_cmp0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_cmp1(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_ftm0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_ftm1(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_ftm2(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_cmt(void)                  __attribute__((weak, alias("dummy_handler")));
void isr_rtc(void)                  __attribute__((weak, alias("dummy_handler")));
void isr_rtc_seconds(void)          __attribute__((weak, alias("dummy_handler")));
void isr_pit0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_pit1(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_pit2(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_pit3(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_pdb0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_usb0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_usbdcd(void)               __attribute__((weak, alias("dummy_handler")));
void isr_dac0(void)                 __attribute__((weak, alias("dummy_handler")));
void isr_mcg(void)                  __attribute__((weak, alias("dummy_handler")));
void isr_lptmr0(void)               __attribute__((weak, alias("dummy_handler")));
void isr_porta(void)                __attribute__((weak, alias("dummy_handler")));
void isr_portb(void)                __attribute__((weak, alias("dummy_handler")));
void isr_portc(void)                __attribute__((weak, alias("dummy_handler")));
void isr_portd(void)                __attribute__((weak, alias("dummy_handler")));
void isr_porte(void)                __attribute__((weak, alias("dummy_handler")));
void isr_swi(void)                  __attribute__((weak, alias("dummy_handler")));


/* interrupt vector table */
__attribute__((section(".vector_table")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void *)(&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    (void *) reset_handler,         /* entry point of the program */
    (void *) isr_nmi,               /* non maskable interrupt handler */
    (void *) isr_hard_fault,        /* if you end up here its not good */
    (void *) isr_mem_manage,        /* memory controller interrupt */
    (void *) isr_bus_fault,         /* also not good to end up here */
    (void *) isr_usage_fault,       /* autsch */
    (void *)(0UL),                  /* Reserved */
    (void *)(0UL),                  /* Reserved */
    (void *)(0UL),                  /* Reserved */
    (void *)(0UL),                  /* Reserved */
    (void *) isr_svc,               /* system call interrupt */
    (void *) isr_debug_mon,         /* debug interrupt */
    (void *)(0UL),                  /* Reserved */
    (void *) isr_pendsv,            /* pendSV interrupt, used for task switching in RIOT */
    (void *) isr_systick,           /* SysTick interrupt, not used in RIOT */
    /* MKW22D512 specific peripheral handlers */
    (void *) isr_dma0,              /* DMA channel 0 transfer complete */
    (void *) isr_dma1,              /* DMA channel 1 transfer complete */
    (void *) isr_dma2,              /* DMA channel 2 transfer complete */
    (void *) isr_dma3,              /* DMA channel 3 transfer complete */
    (void *) isr_dma4,              /* DMA channel 4 transfer complete */
    (void *) isr_dma5,              /* DMA channel 5 transfer complete */
    (void *) isr_dma6,              /* DMA channel 6 transfer complete */
    (void *) isr_dma7,              /* DMA channel 7 transfer complete */
    (void *) isr_dma8,              /* DMA channel 8 transfer complete */
    (void *) isr_dma9,              /* DMA channel 9 transfer complete */
    (void *) isr_dma10,             /* DMA channel 10 transfer complete */
    (void *) isr_dma11,             /* DMA channel 11 transfer complete */
    (void *) isr_dma12,             /* DMA channel 12 transfer complete */
    (void *) isr_dma13,             /* DMA channel 13 transfer complete */
    (void *) isr_dma14,             /* DMA channel 14 transfer complete */
    (void *) isr_dma15,             /* DMA channel 15 transfer complete */
    (void *) isr_dma_error,         /* DMA channel 0 - 15 error */
    (void *) isr_mcm,               /* MCM normal interrupt */
    (void *) isr_ftfl,              /* FTFL command complete */
    (void *) isr_ftfl_collision,    /* FTFL read collision */
    (void *) isr_pmc,               /* PMC controller low-voltage detect low-voltage warning */
    (void *) isr_llwu,              /* Low leakage wakeup */
    (void *) isr_wdog_ewm,          /* Single interrupt vector for  WDOG and EWM */
    (void *) isr_rng,               /* Randon number generator */
    (void *) isr_i2c0,              /* Inter-integrated circuit 0 */
    (void *) isr_i2c1,              /* Inter-integrated circuit 1 */
    (void *) isr_spi0,              /* Serial peripheral Interface 0 */
    (void *) isr_spi1,              /* Serial peripheral Interface 1 */
    (void *) isr_i2s0_tx,           /* Integrated interchip sound 0 transmit interrupt */
    (void *) isr_i2s0_rx,           /* Integrated interchip sound 0 receive interrupt */
    (void *) dummy_handler,         /* Reserved interrupt */
    (void *) isr_uart0_rx_tx,       /* UART0 receive/transmit interrupt */
    (void *) isr_uart0_err,         /* UART0 error interrupt */
    (void *) isr_uart1_rx_tx,       /* UART1 receive/transmit interrupt */
    (void *) isr_uart1_err,         /* UART1 error interrupt */
    (void *) isr_uart2_rx_tx,       /* UART2 receive/transmit interrupt */
    (void *) isr_uart2_err,         /* UART2 error interrupt */
    (void *) dummy_handler,         /* Reserved interrupt */
    (void *) dummy_handler,         /* Reserved interrupt */
    (void *) isr_adc0,              /* Analog-to-digital converter 0 */
    (void *) isr_cmp0,              /* Comparator 0 */
    (void *) isr_cmp1,              /* Comparator 1 */
    (void *) isr_ftm0,              /* FlexTimer module 0 fault overflow and channels interrupt */
    (void *) isr_ftm1,              /* FlexTimer module 1 fault overflow and channels interrupt */
    (void *) isr_ftm2,              /* FlexTimer module 2 fault overflow and channels interrupt */
    (void *) isr_cmt,               /* Carrier modulator transmitter */
    (void *) isr_rtc,               /* Real time clock */
    (void *) isr_rtc_seconds,       /* Real time clock seconds */
    (void *) isr_pit0,              /* Periodic interrupt timer channel 0 */
    (void *) isr_pit1,              /* Periodic interrupt timer channel 1 */
    (void *) isr_pit2,              /* Periodic interrupt timer channel 2 */
    (void *) isr_pit3,              /* Periodic interrupt timer channel 3 */
    (void *) isr_pdb0,              /* Programmable delay block */
    (void *) isr_usb0,              /* USB OTG interrupt */
    (void *) isr_usbdcd,            /* USB charger detect */
    (void *) dummy_handler,         /* Reserved interrupt */
    (void *) isr_dac0,              /* Digital-to-analog converter 0 */
    (void *) isr_mcg,               /* Multipurpose clock generator */
    (void *) isr_lptmr0,            /* Low power timer interrupt */
    (void *) isr_porta,             /* Port A pin detect interrupt */
    (void *) isr_portb,             /* Port B pin detect interrupt */
    (void *) isr_portc,             /* Port C pin detect interrupt */
    (void *) isr_portd,             /* Port D pin detect interrupt */
    (void *) isr_porte,             /* Port E pin detect interrupt */
    (void *) isr_swi,               /* Software interrupt */
    (void *) dummy_handler,         /* reserved 81 */
    (void *) dummy_handler,         /* reserved 82 */
    (void *) dummy_handler,         /* reserved 83 */
    (void *) dummy_handler,         /* reserved 84 */
    (void *) dummy_handler,         /* reserved 85 */
    (void *) dummy_handler,         /* reserved 86 */
    (void *) dummy_handler,         /* reserved 87 */
    (void *) dummy_handler,         /* reserved 88 */
    (void *) dummy_handler,         /* reserved 89 */
    (void *) dummy_handler,         /* reserved 90 */
    (void *) dummy_handler,         /* reserved 91 */
    (void *) dummy_handler,         /* reserved 92 */
    (void *) dummy_handler,         /* reserved 93 */
    (void *) dummy_handler,         /* reserved 94 */
    (void *) dummy_handler,         /* reserved 95 */
    (void *) dummy_handler,         /* reserved 96 */
    (void *) dummy_handler,         /* reserved 97 */
    (void *) dummy_handler,         /* reserved 98 */
    (void *) dummy_handler,         /* reserved 99 */
    (void *) dummy_handler,         /* reserved 100 */
    (void *) dummy_handler,         /* reserved 101 */
    (void *) dummy_handler,         /* reserved 102 */
    (void *) dummy_handler,         /* reserved 103 */
    (void *) dummy_handler,         /* reserved 104 */
    (void *) dummy_handler,         /* reserved 105 */
    (void *) dummy_handler,         /* reserved 106 */
    (void *) dummy_handler,         /* reserved 107 */
    (void *) dummy_handler,         /* reserved 108 */
    (void *) dummy_handler,         /* reserved 109 */
    (void *) dummy_handler,         /* reserved 110 */
    (void *) dummy_handler,         /* reserved 111 */
    (void *) dummy_handler,         /* reserved 112 */
    (void *) dummy_handler,         /* reserved 113 */
    (void *) dummy_handler,         /* reserved 114 */
    (void *) dummy_handler,         /* reserved 115 */
    (void *) dummy_handler,         /* reserved 116 */
    (void *) dummy_handler,         /* reserved 117 */
    (void *) dummy_handler,         /* reserved 118 */
    (void *) dummy_handler,         /* reserved 119 */
    (void *) dummy_handler,         /* reserved 120 */
    (void *) dummy_handler,         /* reserved 121 */
    (void *) dummy_handler,         /* reserved 122 */
    (void *) dummy_handler,         /* reserved 123 */
    (void *) dummy_handler,         /* reserved 124 */
    (void *) dummy_handler,         /* reserved 125 */
    (void *) dummy_handler,         /* reserved 126 */
    (void *) dummy_handler,         /* reserved 127 */
    (void *) dummy_handler,         /* reserved 128 */
    (void *) dummy_handler,         /* reserved 129 */
    (void *) dummy_handler,         /* reserved 130 */
    (void *) dummy_handler,         /* reserved 131 */
    (void *) dummy_handler,         /* reserved 132 */
    (void *) dummy_handler,         /* reserved 133 */
    (void *) dummy_handler,         /* reserved 134 */
    (void *) dummy_handler,         /* reserved 135 */
    (void *) dummy_handler,         /* reserved 136 */
    (void *) dummy_handler,         /* reserved 137 */
    (void *) dummy_handler,         /* reserved 138 */
    (void *) dummy_handler,         /* reserved 139 */
    (void *) dummy_handler,         /* reserved 140 */
    (void *) dummy_handler,         /* reserved 141 */
    (void *) dummy_handler,         /* reserved 142 */
    (void *) dummy_handler,         /* reserved 143 */
    (void *) dummy_handler,         /* reserved 144 */
    (void *) dummy_handler,         /* reserved 145 */
    (void *) dummy_handler,         /* reserved 146 */
    (void *) dummy_handler,         /* reserved 147 */
    (void *) dummy_handler,         /* reserved 148 */
    (void *) dummy_handler,         /* reserved 149 */
    (void *) dummy_handler,         /* reserved 150 */
    (void *) dummy_handler,         /* reserved 151 */
    (void *) dummy_handler,         /* reserved 152 */
    (void *) dummy_handler,         /* reserved 153 */
    (void *) dummy_handler,         /* reserved 154 */
    (void *) dummy_handler,         /* reserved 155 */
    (void *) dummy_handler,         /* reserved 156 */
    (void *) dummy_handler,         /* reserved 157 */
    (void *) dummy_handler,         /* reserved 158 */
    (void *) dummy_handler,         /* reserved 159 */
    (void *) dummy_handler,         /* reserved 160 */
    (void *) dummy_handler,         /* reserved 161 */
    (void *) dummy_handler,         /* reserved 162 */
    (void *) dummy_handler,         /* reserved 163 */
    (void *) dummy_handler,         /* reserved 164 */
    (void *) dummy_handler,         /* reserved 165 */
    (void *) dummy_handler,         /* reserved 166 */
    (void *) dummy_handler,         /* reserved 167 */
    (void *) dummy_handler,         /* reserved 168 */
    (void *) dummy_handler,         /* reserved 169 */
    (void *) dummy_handler,         /* reserved 170 */
    (void *) dummy_handler,         /* reserved 171 */
    (void *) dummy_handler,         /* reserved 172 */
    (void *) dummy_handler,         /* reserved 173 */
    (void *) dummy_handler,         /* reserved 174 */
    (void *) dummy_handler,         /* reserved 175 */
    (void *) dummy_handler,         /* reserved 176 */
    (void *) dummy_handler,         /* reserved 177 */
    (void *) dummy_handler,         /* reserved 178 */
    (void *) dummy_handler,         /* reserved 179 */
    (void *) dummy_handler,         /* reserved 180 */
    (void *) dummy_handler,         /* reserved 181 */
    (void *) dummy_handler,         /* reserved 182 */
    (void *) dummy_handler,         /* reserved 183 */
    (void *) dummy_handler,         /* reserved 184 */
    (void *) dummy_handler,         /* reserved 185 */
    (void *) dummy_handler,         /* reserved 186 */
    (void *) dummy_handler,         /* reserved 187 */
    (void *) dummy_handler,         /* reserved 188 */
    (void *) dummy_handler,         /* reserved 189 */
    (void *) dummy_handler,         /* reserved 190 */
    (void *) dummy_handler,         /* reserved 191 */
    (void *) dummy_handler,         /* reserved 192 */
    (void *) dummy_handler,         /* reserved 193 */
    (void *) dummy_handler,         /* reserved 194 */
    (void *) dummy_handler,         /* reserved 195 */
    (void *) dummy_handler,         /* reserved 196 */
    (void *) dummy_handler,         /* reserved 197 */
    (void *) dummy_handler,         /* reserved 198 */
    (void *) dummy_handler,         /* reserved 199 */
    (void *) dummy_handler,         /* reserved 200 */
    (void *) dummy_handler,         /* reserved 201 */
    (void *) dummy_handler,         /* reserved 202 */
    (void *) dummy_handler,         /* reserved 203 */
    (void *) dummy_handler,         /* reserved 204 */
    (void *) dummy_handler,         /* reserved 205 */
    (void *) dummy_handler,         /* reserved 206 */
    (void *) dummy_handler,         /* reserved 207 */
    (void *) dummy_handler,         /* reserved 208 */
    (void *) dummy_handler,         /* reserved 209 */
    (void *) dummy_handler,         /* reserved 210 */
    (void *) dummy_handler,         /* reserved 211 */
    (void *) dummy_handler,         /* reserved 212 */
    (void *) dummy_handler,         /* reserved 213 */
    (void *) dummy_handler,         /* reserved 214 */
    (void *) dummy_handler,         /* reserved 215 */
    (void *) dummy_handler,         /* reserved 216 */
    (void *) dummy_handler,         /* reserved 217 */
    (void *) dummy_handler,         /* reserved 218 */
    (void *) dummy_handler,         /* reserved 219 */
    (void *) dummy_handler,         /* reserved 220 */
    (void *) dummy_handler,         /* reserved 221 */
    (void *) dummy_handler,         /* reserved 222 */
    (void *) dummy_handler,         /* reserved 223 */
    (void *) dummy_handler,         /* reserved 224 */
    (void *) dummy_handler,         /* reserved 225 */
    (void *) dummy_handler,         /* reserved 226 */
    (void *) dummy_handler,         /* reserved 227 */
    (void *) dummy_handler,         /* reserved 228 */
    (void *) dummy_handler,         /* reserved 229 */
    (void *) dummy_handler,         /* reserved 230 */
    (void *) dummy_handler,         /* reserved 231 */
    (void *) dummy_handler,         /* reserved 232 */
    (void *) dummy_handler,         /* reserved 233 */
    (void *) dummy_handler,         /* reserved 234 */
    (void *) dummy_handler,         /* reserved 235 */
    (void *) dummy_handler,         /* reserved 236 */
    (void *) dummy_handler,         /* reserved 237 */
    (void *) dummy_handler,         /* reserved 238 */
    (void *) dummy_handler,         /* reserved 239 */
    (void *) dummy_handler,         /* reserved 240 */
    (void *) dummy_handler,         /* reserved 241 */
    (void *) dummy_handler,         /* reserved 242 */
    (void *) dummy_handler,         /* reserved 243 */
    (void *) dummy_handler,         /* reserved 244 */
    (void *) dummy_handler,         /* reserved 245 */
    (void *) dummy_handler,         /* reserved 246 */
    (void *) dummy_handler,         /* reserved 247 */
    (void *) dummy_handler,         /* reserved 248 */
    (void *) dummy_handler,         /* reserved 249 */
    (void *) dummy_handler,         /* reserved 250 */
    (void *) dummy_handler,         /* reserved 251 */
    (void *) dummy_handler,         /* reserved 252 */
    (void *) dummy_handler,         /* reserved 253 */
    (void *) dummy_handler,         /* reserved 254 */
    (void *) dummy_handler,         /* reserved 255 */
};
