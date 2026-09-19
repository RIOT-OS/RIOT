/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_imx8-m4
 * @{
 *
 * @file        vectors.c
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* IMX8MQ-M4 specific interrupt vector */
WEAK_DEFAULT void isr_pm(void);
WEAK_DEFAULT void isr_gpr(void);
WEAK_DEFAULT void isr_dap(void);
WEAK_DEFAULT void isr_smda1(void);
WEAK_DEFAULT void isr_gpu(void);
WEAK_DEFAULT void isr_snvs(void);
WEAK_DEFAULT void isr_lcdif(void);
WEAK_DEFAULT void isr_spdif1(void);
WEAK_DEFAULT void isr_h264(void);
WEAK_DEFAULT void isr_vpudma(void);
WEAK_DEFAULT void isr_qos(void);
WEAK_DEFAULT void isr_wdog3(void);
WEAK_DEFAULT void isr_hs_cp1(void);
WEAK_DEFAULT void isr_apbhdma(void);
WEAK_DEFAULT void isr_spdif2(void);
WEAK_DEFAULT void isr_bch(void);
WEAK_DEFAULT void isr_gpmi(void);
WEAK_DEFAULT void isr_hdmi0(void);
WEAK_DEFAULT void isr_hdmi1(void);
WEAK_DEFAULT void isr_hdmi2(void);
WEAK_DEFAULT void isr_snvs_conso(void);
WEAK_DEFAULT void isr_snvs_secure(void);
WEAK_DEFAULT void isr_csu(void);
WEAK_DEFAULT void isr_usdhc1(void);
WEAK_DEFAULT void isr_usdhc2(void);
WEAK_DEFAULT void isr_ddc(void);
WEAK_DEFAULT void isr_dtrc(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_uart2(void);
WEAK_DEFAULT void isr_uart3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_vp9(void);
WEAK_DEFAULT void isr_ecspi1(void);
WEAK_DEFAULT void isr_ecspi2(void);
WEAK_DEFAULT void isr_ecspi3(void);
WEAK_DEFAULT void isr_mip_dsi(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_i2c3(void);
WEAK_DEFAULT void isr_i2c4(void);
WEAK_DEFAULT void isr_rdc(void);
WEAK_DEFAULT void isr_usb1(void);
WEAK_DEFAULT void isr_usb2(void);
WEAK_DEFAULT void isr_csi1(void);
WEAK_DEFAULT void isr_csi2(void);
WEAK_DEFAULT void isr_mipi_csi1(void);
WEAK_DEFAULT void isr_mipi_csi2(void);
WEAK_DEFAULT void isr_gpt6(void);
WEAK_DEFAULT void isr_sctr0(void);
WEAK_DEFAULT void isr_sctr1(void);
WEAK_DEFAULT void isr_tempmon(void);
WEAK_DEFAULT void isr_i2s3(void);
WEAK_DEFAULT void isr_gpt5(void);
WEAK_DEFAULT void isr_gpt4(void);
WEAK_DEFAULT void isr_gpt3(void);
WEAK_DEFAULT void isr_gpt2(void);
WEAK_DEFAULT void isr_gpt1(void);
WEAK_DEFAULT void isr_gpio7(void);
WEAK_DEFAULT void isr_gpio6(void);
WEAK_DEFAULT void isr_gpio5(void);
WEAK_DEFAULT void isr_gpio4(void);
WEAK_DEFAULT void isr_gpio3(void);
WEAK_DEFAULT void isr_gpio2(void);
WEAK_DEFAULT void isr_gpio1(void);
WEAK_DEFAULT void isr_gpio0(void);
WEAK_DEFAULT void isr_gpio1_l(void);
WEAK_DEFAULT void isr_gpio1_h(void);
WEAK_DEFAULT void isr_gpio2_l(void);
WEAK_DEFAULT void isr_gpio2_h(void);
WEAK_DEFAULT void isr_gpio3_l(void);
WEAK_DEFAULT void isr_gpio3_h(void);
WEAK_DEFAULT void isr_gpio4_l(void);
WEAK_DEFAULT void isr_gpio4_h(void);
WEAK_DEFAULT void isr_gpio5_l(void);
WEAK_DEFAULT void isr_gpio5_h(void);
WEAK_DEFAULT void isr_pcie_ctrl2_0(void);
WEAK_DEFAULT void isr_pcie_ctrl2_1(void);
WEAK_DEFAULT void isr_pcie_ctrl2_2(void);
WEAK_DEFAULT void isr_pcie_ctrl2_3(void);
WEAK_DEFAULT void isr_wdog1(void);
WEAK_DEFAULT void isr_wdog2(void);
WEAK_DEFAULT void isr_pcie_ctrl2(void);
WEAK_DEFAULT void isr_pwm1(void);
WEAK_DEFAULT void isr_pwm2(void);
WEAK_DEFAULT void isr_pwm3(void);
WEAK_DEFAULT void isr_pwm4(void);
WEAK_DEFAULT void isr_ccm1(void);
WEAK_DEFAULT void isr_ccm2(void);
WEAK_DEFAULT void isr_gpc(void);
WEAK_DEFAULT void isr_mu_a53(void);
WEAK_DEFAULT void isr_src(void);
WEAK_DEFAULT void isr_i2s_5_6(void);
WEAK_DEFAULT void isr_rtic(void);
WEAK_DEFAULT void isr_cpu_perf(void);
WEAK_DEFAULT void isr_cpu_cti(void);
WEAK_DEFAULT void isr_src_combined(void);
WEAK_DEFAULT void isr_i2s1(void);
WEAK_DEFAULT void isr_i2s2(void);
WEAK_DEFAULT void isr_mu_m4(void);
WEAK_DEFAULT void isr_ddr_perf(void);
WEAK_DEFAULT void isr_ddr(void);
WEAK_DEFAULT void isr_i2s4(void);
WEAK_DEFAULT void isr_cpu_err_axi(void);
WEAK_DEFAULT void isr_cpu_err_l2ram(void);
WEAK_DEFAULT void isr_sdma2(void);
WEAK_DEFAULT void isr_reserved_120(void);
WEAK_DEFAULT void isr_caam0(void);
WEAK_DEFAULT void isr_caam1(void);
WEAK_DEFAULT void isr_qspi(void);
WEAK_DEFAULT void isr_tzasc(void);
WEAK_DEFAULT void isr_reserved_125(void);
WEAK_DEFAULT void isr_reserved_126(void);
WEAK_DEFAULT void isr_reserved_127(void);
WEAK_DEFAULT void isr_perfmon1(void);
WEAK_DEFAULT void isr_perfmon2(void);
WEAK_DEFAULT void isr_caam2(void);
WEAK_DEFAULT void isr_caam_err(void);
WEAK_DEFAULT void isr_hs_cp0(void);
WEAK_DEFAULT void isr_hevc(void);
WEAK_DEFAULT void isr_enet_tx_done1(void);
WEAK_DEFAULT void isr_enet_tx_done2(void);
WEAK_DEFAULT void isr_enet(void);
WEAK_DEFAULT void isr_enet_1588(void);
WEAK_DEFAULT void isr_pcie_ctrl1_0(void);
WEAK_DEFAULT void isr_pcie_ctrl1_1(void);
WEAK_DEFAULT void isr_pcie_ctrl1_2(void);
WEAK_DEFAULT void isr_pcie_ctrl1_3(void);
WEAK_DEFAULT void isr_reserved_142(void);
WEAK_DEFAULT void isr_pcie_ctrl1(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    (void*) isr_gpr,
    (void*) isr_dap,
    (void*) isr_smda1,
    (void*) isr_gpu,
    (void*) isr_snvs,
    (void*) isr_lcdif,
    (void*) isr_spdif1,
    (void*) isr_h264,
    (void*) isr_vpudma,
    (void*) isr_qos,
    (void*) isr_wdog3,
    (void*) isr_hs_cp1,
    (void*) isr_apbhdma,
    (void*) isr_spdif2,
    (void*) isr_bch,
    (void*) isr_gpmi,
    (void*) isr_hdmi0,
    (void*) isr_hdmi1,
    (void*) isr_hdmi2,
    (void*) isr_snvs_conso,
    (void*) isr_snvs_secure,
    (void*) isr_csu,
    (void*) isr_usdhc1,
    (void*) isr_usdhc2,
    (void*) isr_ddc,
    (void*) isr_dtrc,
    (void*) isr_uart1,
    (void*) isr_uart2,
    (void*) isr_uart3,
    (void*) isr_uart4,
    (void*) isr_vp9,
    (void*) isr_ecspi1,
    (void*) isr_ecspi2,
    (void*) isr_ecspi3,
    (void*) isr_mip_dsi,
    (void*) isr_i2c1,
    (void*) isr_i2c2,
    (void*) isr_i2c3,
    (void*) isr_i2c4,
    (void*) isr_rdc,
    (void*) isr_usb1,
    (void*) isr_usb2,
    (void*) isr_csi1,
    (void*) isr_csi2,
    (void*) isr_mipi_csi1,
    (void*) isr_mipi_csi2,
    (void*) isr_gpt6,
    (void*) isr_sctr0,
    (void*) isr_sctr1,
    (void*) isr_tempmon,
    (void*) isr_i2s3,
    (void*) isr_gpt5,
    (void*) isr_gpt4,
    (void*) isr_gpt3,
    (void*) isr_gpt2,
    (void*) isr_gpt1,
    (void*) isr_gpio7,
    (void*) isr_gpio6,
    (void*) isr_gpio5,
    (void*) isr_gpio4,
    (void*) isr_gpio3,
    (void*) isr_gpio2,
    (void*) isr_gpio1,
    (void*) isr_gpio0,
    (void*) isr_gpio1_l,
    (void*) isr_gpio1_h,
    (void*) isr_gpio2_l,
    (void*) isr_gpio2_h,
    (void*) isr_gpio3_l,
    (void*) isr_gpio3_h,
    (void*) isr_gpio4_l,
    (void*) isr_gpio4_h,
    (void*) isr_gpio5_l,
    (void*) isr_gpio5_h,
    (void*) isr_pcie_ctrl2_0,
    (void*) isr_pcie_ctrl2_1,
    (void*) isr_pcie_ctrl2_2,
    (void*) isr_pcie_ctrl2_3,
    (void*) isr_wdog1,
    (void*) isr_wdog2,
    (void*) isr_pcie_ctrl2,
    (void*) isr_pwm1,
    (void*) isr_pwm2,
    (void*) isr_pwm3,
    (void*) isr_pwm4,
    (void*) isr_ccm1,
    (void*) isr_ccm2,
    (void*) isr_gpc,
    (void*) isr_mu_a53,
    (void*) isr_src,
    (void*) isr_i2s_5_6,
    (void*) isr_rtic,
    (void*) isr_cpu_perf,
    (void*) isr_cpu_cti,
    (void*) isr_src_combined,
    (void*) isr_i2s1,
    (void*) isr_i2s2,
    (void*) isr_mu_m4,
    (void*) isr_ddr_perf,
    (void*) isr_ddr,
    (void*) isr_i2s4,
    (void*) isr_cpu_err_axi,
    (void*) isr_cpu_err_l2ram,
    (void*) isr_sdma2,
    (void*) isr_reserved_120,
    (void*) isr_caam0,
    (void*) isr_caam1,
    (void*) isr_qspi,
    (void*) isr_tzasc,
    (void*) isr_reserved_125,
    (void*) isr_reserved_126,
    (void*) isr_reserved_127,
    (void*) isr_perfmon1,
    (void*) isr_perfmon2,
    (void*) isr_caam2,
    (void*) isr_caam_err,
    (void*) isr_hs_cp0,
    (void*) isr_hevc,
    (void*) isr_enet_tx_done1,
    (void*) isr_enet_tx_done2,
    (void*) isr_enet,
    (void*) isr_enet_1588,
    (void*) isr_pcie_ctrl1_0,
    (void*) isr_pcie_ctrl1_1,
    (void*) isr_pcie_ctrl1_2,
    (void*) isr_pcie_ctrl1_3,
    (void*) isr_reserved_142,
    (void*) isr_pcie_ctrl1
};
