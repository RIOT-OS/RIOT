/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k64f
 * @{
 *
 * @file
 * @brief       Interrupt vector definition for K64F MCUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"
#include "wdog.h"

/**
 * memory markers as defined in the linker script
 */
extern uint32_t _estack;

void pre_startup (void)
{
    /* disable the WDOG */
    wdog_disable();
}

void dummy_handler(void)
{
    dummy_handler_default();
}

/* Cortex-M specific interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* K64F specific interrupt vector */
WEAK_DEFAULT void isr_dma0(void);
WEAK_DEFAULT void isr_dma1(void);
WEAK_DEFAULT void isr_dma2(void);
WEAK_DEFAULT void isr_dma3(void);
WEAK_DEFAULT void isr_dma4(void);
WEAK_DEFAULT void isr_dma5(void);
WEAK_DEFAULT void isr_dma6(void);
WEAK_DEFAULT void isr_dma7(void);
WEAK_DEFAULT void isr_dma8(void);
WEAK_DEFAULT void isr_dma9(void);
WEAK_DEFAULT void isr_dma10(void);
WEAK_DEFAULT void isr_dma11(void);
WEAK_DEFAULT void isr_dma12(void);
WEAK_DEFAULT void isr_dma13(void);
WEAK_DEFAULT void isr_dma14(void);
WEAK_DEFAULT void isr_dma15(void);
WEAK_DEFAULT void isr_dma_error(void);
WEAK_DEFAULT void isr_mcm(void);
WEAK_DEFAULT void isr_ftfl(void);
WEAK_DEFAULT void isr_ftfl_collision(void);
WEAK_DEFAULT void isr_pmc(void);
WEAK_DEFAULT void isr_llwu(void);
WEAK_DEFAULT void isr_wdog_ewm(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_spi0(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_i2s0_tx(void);
WEAK_DEFAULT void isr_i2s0_rx(void);
WEAK_DEFAULT void isr_uart0_lon(void);
WEAK_DEFAULT void isr_uart0_rx_tx(void);
WEAK_DEFAULT void isr_uart0_err(void);
WEAK_DEFAULT void isr_uart1_rx_tx(void);
WEAK_DEFAULT void isr_uart1_err(void);
WEAK_DEFAULT void isr_uart2_rx_tx(void);
WEAK_DEFAULT void isr_uart2_err(void);
WEAK_DEFAULT void isr_uart3_rx_tx(void);
WEAK_DEFAULT void isr_uart3_err(void);
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_cmp0(void);
WEAK_DEFAULT void isr_cmp1(void);
WEAK_DEFAULT void isr_ftm0(void);
WEAK_DEFAULT void isr_ftm1(void);
WEAK_DEFAULT void isr_ftm2(void);
WEAK_DEFAULT void isr_cmt(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_rtc_seconds(void);
WEAK_DEFAULT void isr_pit0(void);
WEAK_DEFAULT void isr_pit1(void);
WEAK_DEFAULT void isr_pit2(void);
WEAK_DEFAULT void isr_pit3(void);
WEAK_DEFAULT void isr_pdb0(void);
WEAK_DEFAULT void isr_usb0(void);
WEAK_DEFAULT void isr_usbdcd(void);
WEAK_DEFAULT void isr_dac0(void);
WEAK_DEFAULT void isr_mcg(void);
WEAK_DEFAULT void isr_lptmr0(void);
WEAK_DEFAULT void isr_porta(void);
WEAK_DEFAULT void isr_portb(void);
WEAK_DEFAULT void isr_portc(void);
WEAK_DEFAULT void isr_portd(void);
WEAK_DEFAULT void isr_porte(void);
WEAK_DEFAULT void isr_swi(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_uart4_rx_tx(void);
WEAK_DEFAULT void isr_uart4_err(void);
WEAK_DEFAULT void isr_uart5_rx_tx(void);
WEAK_DEFAULT void isr_uart5_err(void);
WEAK_DEFAULT void isr_cmp2(void);
WEAK_DEFAULT void isr_ftm3(void);
WEAK_DEFAULT void isr_dac1(void);
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_can0_mb(void);
WEAK_DEFAULT void isr_can0_bus_off(void);
WEAK_DEFAULT void isr_can0_error(void);
WEAK_DEFAULT void isr_can0_tx_warning(void);
WEAK_DEFAULT void isr_can0_rx_warning(void);
WEAK_DEFAULT void isr_can0_wake_up(void);
WEAK_DEFAULT void isr_sdhc(void);
WEAK_DEFAULT void isr_enet_1588_timer(void);
WEAK_DEFAULT void isr_enet_transmit(void);
WEAK_DEFAULT void isr_enet_receive(void);
WEAK_DEFAULT void isr_enet_error(void);

/* interrupt vector table */
__attribute__((used, section(".vector_table")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void *)(&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) mem_manage_default,     /* memory manage exception */
    (void*) bus_fault_default,      /* bus fault exception */
    (void*) usage_fault_default,    /* usage fault exception */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) debug_mon_default,      /* debug monitor exception */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* K64F specific peripheral handlers */
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
    (void *) isr_uart0_lon,         /* UART0 LON interrupt */
    (void *) isr_uart0_rx_tx,       /* UART0 receive/transmit interrupt */
    (void *) isr_uart0_err,         /* UART0 error interrupt */
    (void *) isr_uart1_rx_tx,       /* UART1 receive/transmit interrupt */
    (void *) isr_uart1_err,         /* UART1 error interrupt */
    (void *) isr_uart2_rx_tx,       /* UART2 receive/transmit interrupt */
    (void *) isr_uart2_err,         /* UART2 error interrupt */
    (void *) isr_uart3_rx_tx,       /* UART3 receive/transmit interrupt */
    (void *) isr_uart3_err,         /* UART3 error interrupt */
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
    (void *) isr_spi2,              /* Serial peripheral Interface 2 */
    (void *) isr_uart4_rx_tx,       /* UART4 receive/transmit interrupt */
    (void *) isr_uart4_err,         /* UART4 error interrupt */
    (void *) isr_uart5_rx_tx,       /* UART5 receive/transmit interrupt */
    (void *) isr_uart5_err,         /* UART5 error interrupt */
    (void *) isr_cmp2,              /* Comparator 2 */
    (void *) isr_ftm3,              /* FlexTimer module 3 fault overflow and channels interrupt */
    (void *) isr_dac1,              /* Digital-to-analog converter 1 */
    (void *) isr_adc1,              /* Analog-to-digital converter 1 */
    (void *) isr_i2c2,              /* Inter-integrated circuit 2 */
    (void *) isr_can0_mb,           /* CAN0 OR'd message buffers interrupt */
    (void *) isr_can0_bus_off,      /* CAN0 bus off interrupt */
    (void *) isr_can0_error,        /* CAN0 error interrupt */
    (void *) isr_can0_tx_warning,   /* CAN0 tx warning interrupt */
    (void *) isr_can0_rx_warning,   /* CAN0 rx warning interrupt */
    (void *) isr_can0_wake_up,      /* CAN0 wake up interrupt */
    (void *) isr_sdhc,              /* SDHC interrupt */
    (void *) isr_enet_1588_timer,   /* Ethernet MAC IEEE 1588 Timer interrupt */
    (void *) isr_enet_transmit,     /* Ethernet MAC transmit interrupt */
    (void *) isr_enet_receive,      /* Ethernet MAC receive interrupt */
    (void *) isr_enet_error,        /* Ethernet MAC error interrupt */
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
