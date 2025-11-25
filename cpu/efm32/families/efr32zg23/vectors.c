/*
 * Copyright (C) 2015-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efr32zg23
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void)
{
    dummy_handler_default();
}

/* Silicon Labs specific interrupt vector */
WEAK_DEFAULT void isr_smu_secure(void);
WEAK_DEFAULT void isr_smu_s_privileged(void);
WEAK_DEFAULT void isr_smu_ns_privileged(void);
WEAK_DEFAULT void isr_emu(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_timer2(void);
WEAK_DEFAULT void isr_timer3(void);
WEAK_DEFAULT void isr_timer4(void);
WEAK_DEFAULT void isr_usart0_rx(void);
WEAK_DEFAULT void isr_usart0_tx(void);
WEAK_DEFAULT void isr_eusart0_rx(void);
WEAK_DEFAULT void isr_eusart0_tx(void);
WEAK_DEFAULT void isr_eusart1_rx(void);
WEAK_DEFAULT void isr_eusart1_tx(void);
WEAK_DEFAULT void isr_eusart2_rx(void);
WEAK_DEFAULT void isr_eusart2_tx(void);
WEAK_DEFAULT void isr_icache0(void);
WEAK_DEFAULT void isr_burtc(void);
WEAK_DEFAULT void isr_letimer0(void);
WEAK_DEFAULT void isr_syscfg(void);
WEAK_DEFAULT void isr_mpahbram(void);
WEAK_DEFAULT void isr_ldma(void);
WEAK_DEFAULT void isr_lfxo(void);
WEAK_DEFAULT void isr_lfrco(void);
WEAK_DEFAULT void isr_ulfrco(void);
WEAK_DEFAULT void isr_gpio_odd(void);
WEAK_DEFAULT void isr_gpio_even(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_emudg(void);
WEAK_DEFAULT void isr_agc(void);
WEAK_DEFAULT void isr_bufc(void);
WEAK_DEFAULT void isr_frc_pri(void);
WEAK_DEFAULT void isr_frc(void);
WEAK_DEFAULT void isr_modem(void);
WEAK_DEFAULT void isr_protimer(void);
WEAK_DEFAULT void isr_rac_rsm(void);
WEAK_DEFAULT void isr_rac_seq(void);
WEAK_DEFAULT void isr_hostmailbox(void);
WEAK_DEFAULT void isr_synth(void);
WEAK_DEFAULT void isr_acmp0(void);
WEAK_DEFAULT void isr_acmp1(void);
WEAK_DEFAULT void isr_wdog0(void);
WEAK_DEFAULT void isr_wdog1(void);
WEAK_DEFAULT void isr_hfxo0(void);
WEAK_DEFAULT void isr_hfrco0(void);
WEAK_DEFAULT void isr_hfrcoem23(void);
WEAK_DEFAULT void isr_cmu(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_iadc(void);
WEAK_DEFAULT void isr_msc(void);
WEAK_DEFAULT void isr_dpll0(void);
WEAK_DEFAULT void isr_emuefp(void);
WEAK_DEFAULT void isr_dcdc(void);
WEAK_DEFAULT void isr_vdac(void);
WEAK_DEFAULT void isr_pcnt0(void);
WEAK_DEFAULT void isr_sw0(void);
WEAK_DEFAULT void isr_sw1(void);
WEAK_DEFAULT void isr_sw2(void);
WEAK_DEFAULT void isr_sw3(void);
WEAK_DEFAULT void isr_kernel0(void);
WEAK_DEFAULT void isr_kernel1(void);
WEAK_DEFAULT void isr_m33cti0(void);
WEAK_DEFAULT void isr_m33cti1(void);
WEAK_DEFAULT void isr_fpuexh(void);
WEAK_DEFAULT void isr_setamperhost(void);
WEAK_DEFAULT void isr_sembrx(void);
WEAK_DEFAULT void isr_sembtx(void);
WEAK_DEFAULT void isr_lesense(void);
WEAK_DEFAULT void isr_sysrtc_app(void);
WEAK_DEFAULT void isr_sysrtc_seq(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_keyscan(void);
WEAK_DEFAULT void isr_rfeca0(void);
WEAK_DEFAULT void isr_rfeca1(void);

/* interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_smu_secure,         /* SMU_SECURE */
    [ 1] = isr_smu_s_privileged,   /* SMU_S_PRIVILEGED */
    [ 2] = isr_smu_ns_privileged,  /* SMU_NS_PRIVILEGED */
    [ 3] = isr_emu,                /* EMU */
    [ 4] = isr_timer0,             /* TIMER0 */
    [ 5] = isr_timer1,             /* TIMER1 */
    [ 6] = isr_timer2,             /* TIMER2 */
    [ 7] = isr_timer3,             /* TIMER3 */
    [ 8] = isr_timer4,             /* TIMER4 */
    [ 9] = isr_usart0_rx,          /* USART0_RX */
    [10] = isr_usart0_tx,          /* USART0_TX */
    [11] = isr_eusart0_rx,         /* EUSART0_RX */
    [12] = isr_eusart0_tx,         /* EUSART0_TX */
    [13] = isr_eusart1_rx,         /* EUSART1_RX */
    [14] = isr_eusart1_tx,         /* EUSART1_TX */
    [15] = isr_eusart2_rx,         /* EUSART2_RX */
    [16] = isr_eusart2_tx,         /* EUSART2_TX */
    [17] = isr_icache0,            /* ICACHE0 */
    [18] = isr_burtc,              /* BURTC */
    [19] = isr_letimer0,           /* LETIMER0 */
    [20] = isr_syscfg,             /* SYSCFG */
    [21] = isr_mpahbram,           /* MPAHBRAM */
    [22] = isr_ldma,               /* LDMA */
    [23] = isr_lfxo,               /* LFXO */
    [24] = isr_lfrco,              /* LFRCO */
    [25] = isr_ulfrco,             /* ULFRCO */
    [26] = isr_gpio_odd,           /* GPIO_ODD */
    [27] = isr_gpio_even,          /* GPIO_EVEN */
    [28] = isr_i2c0,               /* I2C0 */
    [29] = isr_i2c1,               /* I2C1 */
    [30] = isr_emudg,              /* EMUDG */
    [31] = isr_agc,                /* AGC */
    [32] = isr_bufc,               /* BUFC */
    [33] = isr_frc_pri,            /* FRC_PRI */
    [34] = isr_frc,                /* FRC */
    [35] = isr_modem,              /* MODEM */
    [36] = isr_protimer,           /* PROTIMER */
    [37] = isr_rac_rsm,            /* RAC_RSM */
    [38] = isr_rac_seq,            /* RAC_SEQ */
    [39] = isr_hostmailbox,        /* HOSTMAILBOX */
    [40] = isr_synth,              /* SYNTH */
    [41] = isr_acmp0,              /* ACMP0 */
    [42] = isr_acmp1,              /* ACMP1 */
    [43] = isr_wdog0,              /* WDOG0 */
    [44] = isr_wdog1,              /* WDOG1 */
    [45] = isr_hfxo0,              /* HFXO0 */
    [46] = isr_hfrco0,             /* HFRCO0 */
    [47] = isr_hfrcoem23,          /* HFRCOEM23 */
    [48] = isr_cmu,                /* CMU */
    [49] = isr_aes,                /* AES */
    [50] = isr_iadc,               /* IADC */
    [51] = isr_msc,                /* MSC */
    [52] = isr_dpll0,              /* DPLL0 */
    [53] = isr_emuefp,             /* EMUEFP */
    [54] = isr_dcdc,               /* DCDC */
    [55] = isr_vdac,               /* VDAC */
    [56] = isr_pcnt0,              /* PCNT0 */
    [57] = isr_sw0,                /* SW0 */
    [58] = isr_sw1,                /* SW1 */
    [59] = isr_sw2,                /* SW2 */
    [60] = isr_sw3,                /* SW3 */
    [61] = isr_kernel0,            /* KERNEL0 */
    [62] = isr_kernel1,            /* KERNEL1 */
    [63] = isr_m33cti0,            /* M33CTI0 */
    [64] = isr_m33cti1,            /* M33CTI1 */
    [65] = isr_fpuexh,             /* FPUEXH */
    [66] = isr_setamperhost,       /* SETAMPERHOST */
    [67] = isr_sembrx,             /* SEMBRX */
    [68] = isr_sembtx,             /* SEMBTX */
    [69] = isr_lesense,            /* LESENSE */
    [70] = isr_sysrtc_app,         /* SYSRTC_APP */
    [71] = isr_sysrtc_seq,         /* SYSRTC_SEQ */
    [72] = isr_lcd,                /* LCD */
    [73] = isr_keyscan,            /* KEYSCAN */
    [74] = isr_rfeca0,             /* RFECA0 */
    [75] = isr_rfeca1,             /* RFECA1 */
};
