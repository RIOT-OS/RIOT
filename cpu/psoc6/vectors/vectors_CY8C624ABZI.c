#include <stdint.h>
#include "cpu.h"
#include "vectors_cortexm.h"

/* Your vendor IRQ header (defines IRQn names / numbers) */
#include "cy8c6248bzi_s2d44.h"

void dummy_handler(void);
void dummy_handler(void)
{
    dummy_handler_default();
}

/* -------------------------------------------------------------------------- */
/* CM0+ vectors (User + Internal SW IRQs)                                      */
/* -------------------------------------------------------------------------- */
#if defined(CPU_CORE_CORTEX_M0PLUS)

/* IRQs specific to CM0+ (User interrupts + Internal SW interrupts) */
WEAK_DEFAULT void NvicMux0_IRQHandler(void);
WEAK_DEFAULT void NvicMux1_IRQHandler(void);
WEAK_DEFAULT void NvicMux2_IRQHandler(void);
WEAK_DEFAULT void NvicMux3_IRQHandler(void);
WEAK_DEFAULT void NvicMux4_IRQHandler(void);
WEAK_DEFAULT void NvicMux5_IRQHandler(void);
WEAK_DEFAULT void NvicMux6_IRQHandler(void);
WEAK_DEFAULT void NvicMux7_IRQHandler(void);

WEAK_DEFAULT void Internal0_IRQHandler(void);
WEAK_DEFAULT void Internal1_IRQHandler(void);
WEAK_DEFAULT void Internal2_IRQHandler(void);
WEAK_DEFAULT void Internal3_IRQHandler(void);
WEAK_DEFAULT void Internal4_IRQHandler(void);
WEAK_DEFAULT void Internal5_IRQHandler(void);
WEAK_DEFAULT void Internal6_IRQHandler(void);
WEAK_DEFAULT void Internal7_IRQHandler(void);

/* CPU specific interrupt vector table (external IRQs only) */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    NvicMux0_IRQHandler,   /*  0 */
    NvicMux1_IRQHandler,   /*  1 */
    NvicMux2_IRQHandler,   /*  2 */
    NvicMux3_IRQHandler,   /*  3 */
    NvicMux4_IRQHandler,   /*  4 */
    NvicMux5_IRQHandler,   /*  5 */
    NvicMux6_IRQHandler,   /*  6 */
    NvicMux7_IRQHandler,   /*  7 */
    Internal0_IRQHandler,  /*  8 */
    Internal1_IRQHandler,  /*  9 */
    Internal2_IRQHandler,  /* 10 */
    Internal3_IRQHandler,  /* 11 */
    Internal4_IRQHandler,  /* 12 */
    Internal5_IRQHandler,  /* 13 */
    Internal6_IRQHandler,  /* 14 */
    Internal7_IRQHandler,  /* 15 */
};

#else
/* -------------------------------------------------------------------------- */
/* CM4 vectors (Peripheral IRQs 0..167)                                        */
/* -------------------------------------------------------------------------- */

/* Declare all CM4 peripheral IRQ handlers as weak aliases to dummy_handler */
WEAK_DEFAULT void ioss_interrupts_gpio_0_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_1_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_2_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_3_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_4_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_5_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_6_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_7_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_8_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_9_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_10_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_11_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_12_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_13_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupts_gpio_14_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupt_gpio_IRQHandler(void);
WEAK_DEFAULT void ioss_interrupt_vdd_IRQHandler(void);
WEAK_DEFAULT void lpcomp_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_8_interrupt_IRQHandler(void);
WEAK_DEFAULT void srss_interrupt_mcwdt_0_IRQHandler(void);
WEAK_DEFAULT void srss_interrupt_mcwdt_1_IRQHandler(void);
WEAK_DEFAULT void srss_interrupt_backup_IRQHandler(void);
WEAK_DEFAULT void srss_interrupt_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_0_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_1_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_2_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_3_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_4_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_5_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_6_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_7_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_8_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_9_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_10_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_11_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_12_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_13_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_14_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_ipc_15_IRQHandler(void);
WEAK_DEFAULT void scb_0_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_1_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_2_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_3_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_4_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_5_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_6_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_7_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_9_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_10_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_11_interrupt_IRQHandler(void);
WEAK_DEFAULT void scb_12_interrupt_IRQHandler(void);
WEAK_DEFAULT void csd_interrupt_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dmac_0_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dmac_1_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dmac_2_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dmac_3_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_0_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_1_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_2_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_3_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_4_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_5_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_6_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_7_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_8_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_9_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_10_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_11_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_12_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_13_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_14_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_15_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_16_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_17_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_18_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_19_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_20_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_21_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_22_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_23_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_24_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_25_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_26_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_27_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw0_28_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_0_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_1_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_2_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_3_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_4_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_5_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_6_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_7_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_8_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_9_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_10_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_11_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_12_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_13_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_14_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_15_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_16_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_17_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_18_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_19_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_20_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_21_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_22_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_23_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_24_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_25_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_26_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_27_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_dw1_28_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_fault_0_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_fault_1_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupt_crypto_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupt_fm_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_cm4_fp_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_cm0_cti_0_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_cm0_cti_1_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_cm4_cti_0_IRQHandler(void);
WEAK_DEFAULT void cpuss_interrupts_cm4_cti_1_IRQHandler(void);
WEAK_DEFAULT void tcpwm_0_interrupts_0_IRQHandler(void);
WEAK_DEFAULT void tcpwm_0_interrupts_1_IRQHandler(void);
WEAK_DEFAULT void tcpwm_0_interrupts_2_IRQHandler(void);
WEAK_DEFAULT void tcpwm_0_interrupts_3_IRQHandler(void);
WEAK_DEFAULT void tcpwm_0_interrupts_4_IRQHandler(void);
WEAK_DEFAULT void tcpwm_0_interrupts_5_IRQHandler(void);
WEAK_DEFAULT void tcpwm_0_interrupts_6_IRQHandler(void);
WEAK_DEFAULT void tcpwm_0_interrupts_7_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_0_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_1_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_2_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_3_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_4_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_5_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_6_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_7_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_8_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_9_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_10_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_11_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_12_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_13_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_14_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_15_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_16_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_17_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_18_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_19_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_20_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_21_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_22_IRQHandler(void);
WEAK_DEFAULT void tcpwm_1_interrupts_23_IRQHandler(void);
WEAK_DEFAULT void pass_interrupt_sar_IRQHandler(void);
WEAK_DEFAULT void audioss_0_interrupt_i2s_IRQHandler(void);
WEAK_DEFAULT void audioss_0_interrupt_pdm_IRQHandler(void);
WEAK_DEFAULT void audioss_1_interrupt_i2s_IRQHandler(void);
WEAK_DEFAULT void profile_interrupt_IRQHandler(void);
WEAK_DEFAULT void smif_interrupt_IRQHandler(void);
WEAK_DEFAULT void usb_interrupt_hi_IRQHandler(void);
WEAK_DEFAULT void usb_interrupt_med_IRQHandler(void);
WEAK_DEFAULT void usb_interrupt_lo_IRQHandler(void);
WEAK_DEFAULT void sdhc_0_interrupt_wakeup_IRQHandler(void);
WEAK_DEFAULT void sdhc_0_interrupt_general_IRQHandler(void);
WEAK_DEFAULT void sdhc_1_interrupt_wakeup_IRQHandler(void);
WEAK_DEFAULT void sdhc_1_interrupt_general_IRQHandler(void);

/* CPU specific interrupt vector table (external IRQs only) */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* 0..14 */
    ioss_interrupts_gpio_0_IRQHandler,
    ioss_interrupts_gpio_1_IRQHandler,
    ioss_interrupts_gpio_2_IRQHandler,
    ioss_interrupts_gpio_3_IRQHandler,
    ioss_interrupts_gpio_4_IRQHandler,
    ioss_interrupts_gpio_5_IRQHandler,
    ioss_interrupts_gpio_6_IRQHandler,
    ioss_interrupts_gpio_7_IRQHandler,
    ioss_interrupts_gpio_8_IRQHandler,
    ioss_interrupts_gpio_9_IRQHandler,
    ioss_interrupts_gpio_10_IRQHandler,
    ioss_interrupts_gpio_11_IRQHandler,
    ioss_interrupts_gpio_12_IRQHandler,
    ioss_interrupts_gpio_13_IRQHandler,
    ioss_interrupts_gpio_14_IRQHandler,

    /* 15..38 */
    ioss_interrupt_gpio_IRQHandler,
    ioss_interrupt_vdd_IRQHandler,
    lpcomp_interrupt_IRQHandler,
    scb_8_interrupt_IRQHandler,
    srss_interrupt_mcwdt_0_IRQHandler,
    srss_interrupt_mcwdt_1_IRQHandler,
    srss_interrupt_backup_IRQHandler,
    srss_interrupt_IRQHandler,
    cpuss_interrupts_ipc_0_IRQHandler,
    cpuss_interrupts_ipc_1_IRQHandler,
    cpuss_interrupts_ipc_2_IRQHandler,
    cpuss_interrupts_ipc_3_IRQHandler,
    cpuss_interrupts_ipc_4_IRQHandler,
    cpuss_interrupts_ipc_5_IRQHandler,
    cpuss_interrupts_ipc_6_IRQHandler,
    cpuss_interrupts_ipc_7_IRQHandler,
    cpuss_interrupts_ipc_8_IRQHandler,
    cpuss_interrupts_ipc_9_IRQHandler,
    cpuss_interrupts_ipc_10_IRQHandler,
    cpuss_interrupts_ipc_11_IRQHandler,
    cpuss_interrupts_ipc_12_IRQHandler,
    cpuss_interrupts_ipc_13_IRQHandler,
    cpuss_interrupts_ipc_14_IRQHandler,
    cpuss_interrupts_ipc_15_IRQHandler,

    /* 39..51 */
    scb_0_interrupt_IRQHandler,
    scb_1_interrupt_IRQHandler,
    scb_2_interrupt_IRQHandler,
    scb_3_interrupt_IRQHandler,
    scb_4_interrupt_IRQHandler,
    scb_5_interrupt_IRQHandler,
    scb_6_interrupt_IRQHandler,
    scb_7_interrupt_IRQHandler,
    scb_9_interrupt_IRQHandler,
    scb_10_interrupt_IRQHandler,
    scb_11_interrupt_IRQHandler,
    scb_12_interrupt_IRQHandler,
    csd_interrupt_IRQHandler,

    /* 52..55 */
    cpuss_interrupts_dmac_0_IRQHandler,
    cpuss_interrupts_dmac_1_IRQHandler,
    cpuss_interrupts_dmac_2_IRQHandler,
    cpuss_interrupts_dmac_3_IRQHandler,

    /* 56..84 */
    cpuss_interrupts_dw0_0_IRQHandler,
    cpuss_interrupts_dw0_1_IRQHandler,
    cpuss_interrupts_dw0_2_IRQHandler,
    cpuss_interrupts_dw0_3_IRQHandler,
    cpuss_interrupts_dw0_4_IRQHandler,
    cpuss_interrupts_dw0_5_IRQHandler,
    cpuss_interrupts_dw0_6_IRQHandler,
    cpuss_interrupts_dw0_7_IRQHandler,
    cpuss_interrupts_dw0_8_IRQHandler,
    cpuss_interrupts_dw0_9_IRQHandler,
    cpuss_interrupts_dw0_10_IRQHandler,
    cpuss_interrupts_dw0_11_IRQHandler,
    cpuss_interrupts_dw0_12_IRQHandler,
    cpuss_interrupts_dw0_13_IRQHandler,
    cpuss_interrupts_dw0_14_IRQHandler,
    cpuss_interrupts_dw0_15_IRQHandler,
    cpuss_interrupts_dw0_16_IRQHandler,
    cpuss_interrupts_dw0_17_IRQHandler,
    cpuss_interrupts_dw0_18_IRQHandler,
    cpuss_interrupts_dw0_19_IRQHandler,
    cpuss_interrupts_dw0_20_IRQHandler,
    cpuss_interrupts_dw0_21_IRQHandler,
    cpuss_interrupts_dw0_22_IRQHandler,
    cpuss_interrupts_dw0_23_IRQHandler,
    cpuss_interrupts_dw0_24_IRQHandler,
    cpuss_interrupts_dw0_25_IRQHandler,
    cpuss_interrupts_dw0_26_IRQHandler,
    cpuss_interrupts_dw0_27_IRQHandler,
    cpuss_interrupts_dw0_28_IRQHandler,

    /* 85..113 */
    cpuss_interrupts_dw1_0_IRQHandler,
    cpuss_interrupts_dw1_1_IRQHandler,
    cpuss_interrupts_dw1_2_IRQHandler,
    cpuss_interrupts_dw1_3_IRQHandler,
    cpuss_interrupts_dw1_4_IRQHandler,
    cpuss_interrupts_dw1_5_IRQHandler,
    cpuss_interrupts_dw1_6_IRQHandler,
    cpuss_interrupts_dw1_7_IRQHandler,
    cpuss_interrupts_dw1_8_IRQHandler,
    cpuss_interrupts_dw1_9_IRQHandler,
    cpuss_interrupts_dw1_10_IRQHandler,
    cpuss_interrupts_dw1_11_IRQHandler,
    cpuss_interrupts_dw1_12_IRQHandler,
    cpuss_interrupts_dw1_13_IRQHandler,
    cpuss_interrupts_dw1_14_IRQHandler,
    cpuss_interrupts_dw1_15_IRQHandler,
    cpuss_interrupts_dw1_16_IRQHandler,
    cpuss_interrupts_dw1_17_IRQHandler,
    cpuss_interrupts_dw1_18_IRQHandler,
    cpuss_interrupts_dw1_19_IRQHandler,
    cpuss_interrupts_dw1_20_IRQHandler,
    cpuss_interrupts_dw1_21_IRQHandler,
    cpuss_interrupts_dw1_22_IRQHandler,
    cpuss_interrupts_dw1_23_IRQHandler,
    cpuss_interrupts_dw1_24_IRQHandler,
    cpuss_interrupts_dw1_25_IRQHandler,
    cpuss_interrupts_dw1_26_IRQHandler,
    cpuss_interrupts_dw1_27_IRQHandler,
    cpuss_interrupts_dw1_28_IRQHandler,

    /* 114..122 */
    cpuss_interrupts_fault_0_IRQHandler,
    cpuss_interrupts_fault_1_IRQHandler,
    cpuss_interrupt_crypto_IRQHandler,
    cpuss_interrupt_fm_IRQHandler,
    cpuss_interrupts_cm4_fp_IRQHandler,
    cpuss_interrupts_cm0_cti_0_IRQHandler,
    cpuss_interrupts_cm0_cti_1_IRQHandler,
    cpuss_interrupts_cm4_cti_0_IRQHandler,
    cpuss_interrupts_cm4_cti_1_IRQHandler,

    /* 123..130 */
    tcpwm_0_interrupts_0_IRQHandler,
    tcpwm_0_interrupts_1_IRQHandler,
    tcpwm_0_interrupts_2_IRQHandler,
    tcpwm_0_interrupts_3_IRQHandler,
    tcpwm_0_interrupts_4_IRQHandler,
    tcpwm_0_interrupts_5_IRQHandler,
    tcpwm_0_interrupts_6_IRQHandler,
    tcpwm_0_interrupts_7_IRQHandler,

    /* 131..154 */
    tcpwm_1_interrupts_0_IRQHandler,
    tcpwm_1_interrupts_1_IRQHandler,
    tcpwm_1_interrupts_2_IRQHandler,
    tcpwm_1_interrupts_3_IRQHandler,
    tcpwm_1_interrupts_4_IRQHandler,
    tcpwm_1_interrupts_5_IRQHandler,
    tcpwm_1_interrupts_6_IRQHandler,
    tcpwm_1_interrupts_7_IRQHandler,
    tcpwm_1_interrupts_8_IRQHandler,
    tcpwm_1_interrupts_9_IRQHandler,
    tcpwm_1_interrupts_10_IRQHandler,
    tcpwm_1_interrupts_11_IRQHandler,
    tcpwm_1_interrupts_12_IRQHandler,
    tcpwm_1_interrupts_13_IRQHandler,
    tcpwm_1_interrupts_14_IRQHandler,
    tcpwm_1_interrupts_15_IRQHandler,
    tcpwm_1_interrupts_16_IRQHandler,
    tcpwm_1_interrupts_17_IRQHandler,
    tcpwm_1_interrupts_18_IRQHandler,
    tcpwm_1_interrupts_19_IRQHandler,
    tcpwm_1_interrupts_20_IRQHandler,
    tcpwm_1_interrupts_21_IRQHandler,
    tcpwm_1_interrupts_22_IRQHandler,
    tcpwm_1_interrupts_23_IRQHandler,

    /* 155..167 */
    pass_interrupt_sar_IRQHandler,
    audioss_0_interrupt_i2s_IRQHandler,
    audioss_0_interrupt_pdm_IRQHandler,
    audioss_1_interrupt_i2s_IRQHandler,
    profile_interrupt_IRQHandler,
    smif_interrupt_IRQHandler,
    usb_interrupt_hi_IRQHandler,
    usb_interrupt_med_IRQHandler,
    usb_interrupt_lo_IRQHandler,
    sdhc_0_interrupt_wakeup_IRQHandler,
    sdhc_0_interrupt_general_IRQHandler,
    sdhc_1_interrupt_wakeup_IRQHandler,
    sdhc_1_interrupt_general_IRQHandler,
};

#endif /* CPU_CORE_CORTEX_M0PLUS */
