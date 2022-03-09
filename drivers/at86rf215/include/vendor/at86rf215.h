/**
 * @file at86rf215.h
 *
 * @brief File contains register and command defines specific for RF215.
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 */

/*
 * Copyright (c) 2015-2018, Microchip Technology Inc All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef RF215_H
#define RF215_H

/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/** Parameter definitions */

/** Typical timing values. */
/* TRX Parameter: SPI_T2 */
#define RST_PULSE_WIDTH_NS                                 (5)

/** Subregister Type Definitions (MASK, SHIFT) */

/** Sub-registers of Register @ref IRQS */
/** Bit Mask for Sub-Register IRQS.WAKEUP */
#define IRQS_WAKEUP_MASK                0x01
/** Bit Offset for Sub-Register IRQS.WAKEUP */
#define IRQS_WAKEUP_SHIFT               0

/** Bit Mask for Sub-Register IRQS.TRXRDY */
#define IRQS_TRXRDY_MASK                0x02
/** Bit Offset for Sub-Register IRQS.TRXRDY */
#define IRQS_TRXRDY_SHIFT               1

/** Bit Mask for Sub-Register IRQS.EDC */
#define IRQS_EDC_MASK                   0x04
/** Bit Offset for Sub-Register IRQS.EDC */
#define IRQS_EDC_SHIFT                  2

/** Bit Mask for Sub-Register IRQS.BATLOW */
#define IRQS_BATLOW_MASK                0x08
/** Bit Offset for Sub-Register IRQS.BATLOW */
#define IRQS_BATLOW_SHIFT               3

/** Bit Mask for Sub-Register IRQS.TRXERR */
#define IRQS_TRXERR_MASK                0x10
/** Bit Offset for Sub-Register IRQS.TRXERR */
#define IRQS_TRXERR_SHIFT               4

/** Bit Mask for Sub-Register IRQS.IQIFSF */
#define IRQS_IQIFSF_MASK                0x20
/** Bit Offset for Sub-Register IRQS.IQIFSF */
#define IRQS_IQIFSF_SHIFT               5

/** Sub-registers of Register @ref IRQS */
/** Bit Mask for Sub-Register IRQS.RXFS */
#define IRQS_RXFS_MASK                  0x01
/** Bit Offset for Sub-Register IRQS.RXFS */
#define IRQS_RXFS_SHIFT                 0

/** Bit Mask for Sub-Register IRQS.RXFE */
#define IRQS_RXFE_MASK                  0x02
/** Bit Offset for Sub-Register IRQS.RXFE */
#define IRQS_RXFE_SHIFT                 1

/** Bit Mask for Sub-Register IRQS.RXAM */
#define IRQS_RXAM_MASK                  0x04
/** Bit Offset for Sub-Register IRQS.RXAM */
#define IRQS_RXAM_SHIFT                 2

/** Bit Mask for Sub-Register IRQS.RXEM */
#define IRQS_RXEM_MASK                  0x08
/** Bit Offset for Sub-Register IRQS.RXEM */
#define IRQS_RXEM_SHIFT                 3

/** Bit Mask for Sub-Register IRQS.TXFE */
#define IRQS_TXFE_MASK                  0x10
/** Bit Offset for Sub-Register IRQS.TXFE */
#define IRQS_TXFE_SHIFT                 4

/** Bit Mask for Sub-Register IRQS.AGCH */
#define IRQS_AGCH_MASK                  0x20
/** Bit Offset for Sub-Register IRQS.AGCH */
#define IRQS_AGCH_SHIFT                 5

/** Bit Mask for Sub-Register IRQS.AGCR */
#define IRQS_AGCR_MASK                  0x40
/** Bit Offset for Sub-Register IRQS.AGCR */
#define IRQS_AGCR_SHIFT                 6

/** Bit Mask for Sub-Register IRQS.FBLI */
#define IRQS_FBLI_MASK                  0x80
/** Bit Offset for Sub-Register IRQS.FBLI */
#define IRQS_FBLI_SHIFT                 7

/** Sub-registers of Register @ref RST */
/** Bit Mask for Sub-Register RST.CMD */
#define RST_CMD_MASK                    0x07
/** Bit Offset for Sub-Register RST.CMD */
#define RST_CMD_SHIFT                   0

/** Sub-registers of Register @ref CFG */
/** Bit Mask for Sub-Register CFG.DRV */
#define CFG_DRV_MASK                    0x03
/** Bit Offset for Sub-Register CFG.DRV */
#define CFG_DRV_SHIFT                   0

/** Bit Mask for Sub-Register CFG.IRQP */
#define CFG_IRQP_MASK                   0x04
/** Bit Offset for Sub-Register CFG.IRQP */
#define CFG_IRQP_SHIFT                  2

/** Bit Mask for Sub-Register CFG.IRQMM */
#define CFG_IRQMM_MASK                  0x08
/** Bit Offset for Sub-Register CFG.IRQMM */
#define CFG_IRQMM_SHIFT                 3

/** Sub-registers of Register @ref CLKO */
/** Bit Mask for Sub-Register CLKO.OS */
#define CLKO_OS_MASK                    0x07
/** Bit Offset for Sub-Register CLKO.OS */
#define CLKO_OS_SHIFT                   0

/** Bit Mask for Sub-Register CLKO.DRV */
#define CLKO_DRV_MASK                   0x18
/** Bit Offset for Sub-Register CLKO.DRV */
#define CLKO_DRV_SHIFT                  3

/** Sub-registers of Register @ref BMDVC */
/** Bit Mask for Sub-Register BMDVC.BMVTH */
#define BMDVC_BMVTH_MASK                0x0F
/** Bit Offset for Sub-Register BMDVC.BMVTH */
#define BMDVC_BMVTH_SHIFT               0

/** Bit Mask for Sub-Register BMDVC.BMHR */
#define BMDVC_BMHR_MASK                 0x10
/** Bit Offset for Sub-Register BMDVC.BMHR */
#define BMDVC_BMHR_SHIFT                4

/** Bit Mask for Sub-Register BMDVC.BMS */
#define BMDVC_BMS_MASK                  0x20
/** Bit Offset for Sub-Register BMDVC.BMS */
#define BMDVC_BMS_SHIFT                 5

/** Sub-registers of Register @ref XOC */
/** Bit Mask for Sub-Register XOC.TRIM */
#define XOC_TRIM_MASK                   0x0F
/** Bit Offset for Sub-Register XOC.TRIM */
#define XOC_TRIM_SHIFT                  0

/** Bit Mask for Sub-Register XOC.FS */
#define XOC_FS_MASK                     0x10
/** Bit Offset for Sub-Register XOC.FS */
#define XOC_FS_SHIFT                    4

/** Sub-registers of Register @ref IQIFC0 */
/** Bit Mask for Sub-Register IQIFC0.EEC */
#define IQIFC0_EEC_MASK                 0x01
/** Bit Offset for Sub-Register IQIFC0.EEC */
#define IQIFC0_EEC_SHIFT                0

/** Bit Mask for Sub-Register IQIFC0.CMV1V2 */
#define IQIFC0_CMV1V2_MASK              0x02
/** Bit Offset for Sub-Register IQIFC0.CMV1V2 */
#define IQIFC0_CMV1V2_SHIFT             1

/** Bit Mask for Sub-Register IQIFC0.CMV */
#define IQIFC0_CMV_MASK                 0x0C
/** Bit Offset for Sub-Register IQIFC0.CMV */
#define IQIFC0_CMV_SHIFT                2

/** Bit Mask for Sub-Register IQIFC0.DRV */
#define IQIFC0_DRV_MASK                 0x30
/** Bit Offset for Sub-Register IQIFC0.DRV */
#define IQIFC0_DRV_SHIFT                4

/** Bit Mask for Sub-Register IQIFC0.SF */
#define IQIFC0_SF_MASK                  0x40
/** Bit Offset for Sub-Register IQIFC0.SF */
#define IQIFC0_SF_SHIFT                 6

/** Bit Mask for Sub-Register IQIFC0.EXTLB */
#define IQIFC0_EXTLB_MASK               0x80
/** Bit Offset for Sub-Register IQIFC0.EXTLB */
#define IQIFC0_EXTLB_SHIFT              7

/** Sub-registers of Register @ref IQIFC1 */
/** Bit Mask for Sub-Register IQIFC1.SKEWDRV */
#define IQIFC1_SKEWDRV_MASK             0x03
/** Bit Offset for Sub-Register IQIFC1.SKEWDRV */
#define IQIFC1_SKEWDRV_SHIFT            0

/** Bit Mask for Sub-Register IQIFC1.CHPM */
#define IQIFC1_CHPM_MASK                0x70
/** Bit Offset for Sub-Register IQIFC1.CHPM */
#define IQIFC1_CHPM_SHIFT               4

/** Bit Mask for Sub-Register IQIFC1.FAILSF */
#define IQIFC1_FAILSF_MASK              0x80
/** Bit Offset for Sub-Register IQIFC1.FAILSF */
#define IQIFC1_FAILSF_SHIFT             7

/** Sub-registers of Register @ref IQIFC2 */
/** Bit Mask for Sub-Register IQIFC2.SYNC */
#define IQIFC2_SYNC_MASK                0x80
/** Bit Offset for Sub-Register IQIFC2.SYNC */
#define IQIFC2_SYNC_SHIFT               7

/** Sub-registers of Register @ref PN */
/** Bit Mask for Sub-Register PN.PN */
#define PN_PN_MASK                      0xFF
/** Bit Offset for Sub-Register PN.PN */
#define PN_PN_SHIFT                     0

/** Sub-registers of Register @ref VN */
/** Bit Mask for Sub-Register VN.VN */
#define VN_VN_MASK                      0xFF
/** Bit Offset for Sub-Register VN.VN */
#define VN_VN_SHIFT                     0

/** Sub-registers of Register @ref IRQM */
/** Bit Mask for Sub-Register IRQM.WAKEUP */
#define IRQM_WAKEUP_MASK                0x01
/** Bit Offset for Sub-Register IRQM.WAKEUP */
#define IRQM_WAKEUP_SHIFT               0

/** Bit Mask for Sub-Register IRQM.TRXRDY */
#define IRQM_TRXRDY_MASK                0x02
/** Bit Offset for Sub-Register IRQM.TRXRDY */
#define IRQM_TRXRDY_SHIFT               1

/** Bit Mask for Sub-Register IRQM.EDC */
#define IRQM_EDC_MASK                   0x04
/** Bit Offset for Sub-Register IRQM.EDC */
#define IRQM_EDC_SHIFT                  2

/** Bit Mask for Sub-Register IRQM.BATLOW */
#define IRQM_BATLOW_MASK                0x08
/** Bit Offset for Sub-Register IRQM.BATLOW */
#define IRQM_BATLOW_SHIFT               3

/** Bit Mask for Sub-Register IRQM.TRXERR */
#define IRQM_TRXERR_MASK                0x10
/** Bit Offset for Sub-Register IRQM.TRXERR */
#define IRQM_TRXERR_SHIFT               4

/** Bit Mask for Sub-Register IRQM.IQIFSF */
#define IRQM_IQIFSF_MASK                0x20
/** Bit Offset for Sub-Register IRQM.IQIFSF */
#define IRQM_IQIFSF_SHIFT               5

/** Sub-registers of Register @ref AUXS */
/** Bit Mask for Sub-Register AUXS.PAVC */
#define AUXS_PAVC_MASK                  0x03
/** Bit Offset for Sub-Register AUXS.PAVC */
#define AUXS_PAVC_SHIFT                 0

/** Bit Mask for Sub-Register AUXS.AVS */
#define AUXS_AVS_MASK                   0x04
/** Bit Offset for Sub-Register AUXS.AVS */
#define AUXS_AVS_SHIFT                  2

/** Bit Mask for Sub-Register AUXS.AVEN */
#define AUXS_AVEN_MASK                  0x08
/** Bit Offset for Sub-Register AUXS.AVEN */
#define AUXS_AVEN_SHIFT                 3

/** Bit Mask for Sub-Register AUXS.AVEXT */
#define AUXS_AVEXT_MASK                 0x10
/** Bit Offset for Sub-Register AUXS.AVEXT */
#define AUXS_AVEXT_SHIFT                4

/** Bit Mask for Sub-Register AUXS.AGCMAP */
#define AUXS_AGCMAP_MASK                0x60
/** Bit Offset for Sub-Register AUXS.AGCMAP */
#define AUXS_AGCMAP_SHIFT               5

/** Bit Mask for Sub-Register AUXS.EXTLNABYP */
#define AUXS_EXTLNABYP_MASK             0x80
/** Bit Offset for Sub-Register AUXS.EXTLNABYP */
#define AUXS_EXTLNABYP_SHIFT            7

/** Sub-registers of Register @ref STATE */
/** Bit Mask for Sub-Register STATE.STATE */
#define STATE_STATE_MASK                0x07
/** Bit Offset for Sub-Register STATE.STATE */
#define STATE_STATE_SHIFT               0

/** Sub-registers of Register @ref CMD */
/** Bit Mask for Sub-Register CMD.CMD */
#define CMD_CMD_MASK                    0x07
/** Bit Offset for Sub-Register CMD.CMD */
#define CMD_CMD_SHIFT                   0

/** Sub-registers of Register @ref CS */
/** Bit Mask for Sub-Register CS.CS */
#define CS_CS_MASK                      0xFF
/** Bit Offset for Sub-Register CS.CS */
#define CS_CS_SHIFT                     0

/** Sub-registers of Register @ref CCF0L */
/** Bit Mask for Sub-Register CCF0L.CCF0L */
#define CCF0L_CCF0L_MASK                0xFF
/** Bit Offset for Sub-Register CCF0L.CCF0L */
#define CCF0L_CCF0L_SHIFT               0

/** Sub-registers of Register @ref CCF0H */
/** Bit Mask for Sub-Register CCF0H.CCF0H */
#define CCF0H_CCF0H_MASK                0xFF
/** Bit Offset for Sub-Register CCF0H.CCF0H */
#define CCF0H_CCF0H_SHIFT               0

/** Sub-registers of Register @ref CNL */
/** Bit Mask for Sub-Register CNL.CNL */
#define CNL_CNL_MASK                    0xFF
/** Bit Offset for Sub-Register CNL.CNL */
#define CNL_CNL_SHIFT                   0

/** Sub-registers of Register @ref CNM */
/** Bit Mask for Sub-Register CNM.CNH */
#define CNM_CNH_MASK                    0x01
/** Bit Offset for Sub-Register CNM.CNH */
#define CNM_CNH_SHIFT                   0

/** Bit Mask for Sub-Register CNM.CM */
#define CNM_CM_MASK                     0xC0
/** Bit Offset for Sub-Register CNM.CM */
#define CNM_CM_SHIFT                    6

/** Sub-registers of Register @ref RXBWC */
/** Bit Mask for Sub-Register RXBWC.BW */
#define RXBWC_BW_MASK                   0x0F
/** Bit Offset for Sub-Register RXBWC.BW */
#define RXBWC_BW_SHIFT                  0

/** Bit Mask for Sub-Register RXBWC.IFS */
#define RXBWC_IFS_MASK                  0x10
/** Bit Offset for Sub-Register RXBWC.IFS */
#define RXBWC_IFS_SHIFT                 4

/** Bit Mask for Sub-Register RXBWC.IFI */
#define RXBWC_IFI_MASK                  0x20
/** Bit Offset for Sub-Register RXBWC.IFI */
#define RXBWC_IFI_SHIFT                 5

/** Sub-registers of Register @ref RXDFE */
/** Bit Mask for Sub-Register RXDFE.SR */
#define RXDFE_SR_MASK                   0x0F
/** Bit Offset for Sub-Register RXDFE.SR */
#define RXDFE_SR_SHIFT                  0

/** Bit Mask for Sub-Register RXDFE.RCUT */
#define RXDFE_RCUT_MASK                 0xE0
/** Bit Offset for Sub-Register RXDFE.RCUT */
#define RXDFE_RCUT_SHIFT                5

/** Sub-registers of Register @ref AGCC */
/** Bit Mask for Sub-Register AGCC.EN */
#define AGCC_EN_MASK                    0x01
/** Bit Offset for Sub-Register AGCC.EN */
#define AGCC_EN_SHIFT                   0

/** Bit Mask for Sub-Register AGCC.FRZC */
#define AGCC_FRZC_MASK                  0x02
/** Bit Offset for Sub-Register AGCC.FRZC */
#define AGCC_FRZC_SHIFT                 1

/** Bit Mask for Sub-Register AGCC.FRZS */
#define AGCC_FRZS_MASK                  0x04
/** Bit Offset for Sub-Register AGCC.FRZS */
#define AGCC_FRZS_SHIFT                 2

/** Bit Mask for Sub-Register AGCC.RST */
#define AGCC_RST_MASK                   0x08
/** Bit Offset for Sub-Register AGCC.RST */
#define AGCC_RST_SHIFT                  3

/** Bit Mask for Sub-Register AGCC.AVGS */
#define AGCC_AVGS_MASK                  0x30
/** Bit Offset for Sub-Register AGCC.AVGS */
#define AGCC_AVGS_SHIFT                 4

/** Bit Mask for Sub-Register AGCC.AGCI */
#define AGCC_AGCI_MASK                  0x40
/** Bit Offset for Sub-Register AGCC.AGCI */
#define AGCC_AGCI_SHIFT                 6

/** Sub-registers of Register @ref AGCS */
/** Bit Mask for Sub-Register AGCS.GCW */
#define AGCS_GCW_MASK                   0x1F
/** Bit Offset for Sub-Register AGCS.GCW */
#define AGCS_GCW_SHIFT                  0

/** Bit Mask for Sub-Register AGCS.TGT */
#define AGCS_TGT_MASK                   0xE0
/** Bit Offset for Sub-Register AGCS.TGT */
#define AGCS_TGT_SHIFT                  5

/** Sub-registers of Register @ref RSSI */
/** Bit Mask for Sub-Register RSSI.RSSI */
#define RSSI_RSSI_MASK                  0xFF
/** Bit Offset for Sub-Register RSSI.RSSI */
#define RSSI_RSSI_SHIFT                 0

/** Sub-registers of Register @ref EDC */
/** Bit Mask for Sub-Register EDC.EDM */
#define EDC_EDM_MASK                    0x03
/** Bit Offset for Sub-Register EDC.EDM */
#define EDC_EDM_SHIFT                   0

/** Sub-registers of Register @ref EDD */
/** Bit Mask for Sub-Register EDD.DTB */
#define EDD_DTB_MASK                    0x03
/** Bit Offset for Sub-Register EDD.DTB */
#define EDD_DTB_SHIFT                   0

/** Bit Mask for Sub-Register EDD.DF */
#define EDD_DF_MASK                     0xFC
/** Bit Offset for Sub-Register EDD.DF */
#define EDD_DF_SHIFT                    2

/** Sub-registers of Register @ref EDV */
/** Bit Mask for Sub-Register EDV.EDV */
#define EDV_EDV_MASK                    0xFF
/** Bit Offset for Sub-Register EDV.EDV */
#define EDV_EDV_SHIFT                   0

/** Sub-registers of Register @ref RNDV */
/** Bit Mask for Sub-Register RNDV.RNDV */
#define RNDV_RNDV_MASK                  0xFF
/** Bit Offset for Sub-Register RNDV.RNDV */
#define RNDV_RNDV_SHIFT                 0

/** Sub-registers of Register @ref TXCUTC */
/** Bit Mask for Sub-Register TXCUTC.LPFCUT */
#define TXCUTC_LPFCUT_MASK              0x0F
/** Bit Offset for Sub-Register TXCUTC.LPFCUT */
#define TXCUTC_LPFCUT_SHIFT             0

/** Bit Mask for Sub-Register TXCUTC.PARAMP */
#define TXCUTC_PARAMP_MASK              0xC0
/** Bit Offset for Sub-Register TXCUTC.PARAMP */
#define TXCUTC_PARAMP_SHIFT             6

/** Sub-registers of Register @ref TXDFE */
/** Bit Mask for Sub-Register TXDFE.SR */
#define TXDFE_SR_MASK                   0x0F
/** Bit Offset for Sub-Register TXDFE.SR */
#define TXDFE_SR_SHIFT                  0

/** Bit Mask for Sub-Register TXDFE.DM */
#define TXDFE_DM_MASK                   0x10
/** Bit Offset for Sub-Register TXDFE.DM */
#define TXDFE_DM_SHIFT                  4

/** Bit Mask for Sub-Register TXDFE.RCUT */
#define TXDFE_RCUT_MASK                 0xE0
/** Bit Offset for Sub-Register TXDFE.RCUT */
#define TXDFE_RCUT_SHIFT                5

/** Sub-registers of Register @ref PAC */
/** Bit Mask for Sub-Register PAC.TXPWR */
#define PAC_TXPWR_MASK                  0x1F
/** Bit Offset for Sub-Register PAC.TXPWR */
#define PAC_TXPWR_SHIFT                 0

/** Bit Mask for Sub-Register PAC.PACUR */
#define PAC_PACUR_MASK                  0x60
/** Bit Offset for Sub-Register PAC.PACUR */
#define PAC_PACUR_SHIFT                 5

/** Sub-registers of Register @ref PADFE */
/** Bit Mask for Sub-Register PADFE.PADFE */
#define PADFE_PADFE_MASK                0xC0
/** Bit Offset for Sub-Register PADFE.PADFE */
#define PADFE_PADFE_SHIFT               6

/** Sub-registers of Register @ref PLL */
/** Bit Mask for Sub-Register PLL.LS */
#define PLL_LS_MASK                     0x02
/** Bit Offset for Sub-Register PLL.LS */
#define PLL_LS_SHIFT                    1

/** Bit Mask for Sub-Register PLL.LBW */
#define PLL_LBW_MASK                    0x30
/** Bit Offset for Sub-Register PLL.LBW */
#define PLL_LBW_SHIFT                   4

/** Sub-registers of Register @ref PLLCF */
/** Bit Mask for Sub-Register PLLCF.CF */
#define PLLCF_CF_MASK                   0x3F
/** Bit Offset for Sub-Register PLLCF.CF */
#define PLLCF_CF_SHIFT                  0

/** Sub-registers of Register @ref TXCI */
/** Bit Mask for Sub-Register TXCI.DCOI */
#define TXCI_DCOI_MASK                  0x3F
/** Bit Offset for Sub-Register TXCI.DCOI */
#define TXCI_DCOI_SHIFT                 0

/** Sub-registers of Register @ref TXCQ */
/** Bit Mask for Sub-Register TXCQ.DCOQ */
#define TXCQ_DCOQ_MASK                  0x3F
/** Bit Offset for Sub-Register TXCQ.DCOQ */
#define TXCQ_DCOQ_SHIFT                 0

/** Sub-registers of Register @ref TXDACI */
/** Bit Mask for Sub-Register TXDACI.TXDACID */
#define TXDACI_TXDACID_MASK             0x7F
/** Bit Offset for Sub-Register TXDACI.TXDACID */
#define TXDACI_TXDACID_SHIFT            0

/** Bit Mask for Sub-Register TXDACI.ENTXDACID */
#define TXDACI_ENTXDACID_MASK           0x80
/** Bit Offset for Sub-Register TXDACI.ENTXDACID */
#define TXDACI_ENTXDACID_SHIFT          7

/** Sub-registers of Register @ref TXDACQ */
/** Bit Mask for Sub-Register TXDACQ.TXDACQD */
#define TXDACQ_TXDACQD_MASK             0x7F
/** Bit Offset for Sub-Register TXDACQ.TXDACQD */
#define TXDACQ_TXDACQD_SHIFT            0

/** Bit Mask for Sub-Register TXDACQ.ENTXDACQD */
#define TXDACQ_ENTXDACQD_MASK           0x80
/** Bit Offset for Sub-Register TXDACQ.ENTXDACQD */
#define TXDACQ_ENTXDACQD_SHIFT          7

/** Sub-registers of Register @ref IRQM */
/** Bit Mask for Sub-Register IRQM.RXFS */
#define IRQM_RXFS_MASK                  0x01
/** Bit Offset for Sub-Register IRQM.RXFS */
#define IRQM_RXFS_SHIFT                 0

/** Bit Mask for Sub-Register IRQM.RXFE */
#define IRQM_RXFE_MASK                  0x02
/** Bit Offset for Sub-Register IRQM.RXFE */
#define IRQM_RXFE_SHIFT                 1

/** Bit Mask for Sub-Register IRQM.RXAM */
#define IRQM_RXAM_MASK                  0x04
/** Bit Offset for Sub-Register IRQM.RXAM */
#define IRQM_RXAM_SHIFT                 2

/** Bit Mask for Sub-Register IRQM.RXEM */
#define IRQM_RXEM_MASK                  0x08
/** Bit Offset for Sub-Register IRQM.RXEM */
#define IRQM_RXEM_SHIFT                 3

/** Bit Mask for Sub-Register IRQM.TXFE */
#define IRQM_TXFE_MASK                  0x10
/** Bit Offset for Sub-Register IRQM.TXFE */
#define IRQM_TXFE_SHIFT                 4

/** Bit Mask for Sub-Register IRQM.AGCH */
#define IRQM_AGCH_MASK                  0x20
/** Bit Offset for Sub-Register IRQM.AGCH */
#define IRQM_AGCH_SHIFT                 5

/** Bit Mask for Sub-Register IRQM.AGCR */
#define IRQM_AGCR_MASK                  0x40
/** Bit Offset for Sub-Register IRQM.AGCR */
#define IRQM_AGCR_SHIFT                 6

/** Bit Mask for Sub-Register IRQM.FBLI */
#define IRQM_FBLI_MASK                  0x80
/** Bit Offset for Sub-Register IRQM.FBLI */
#define IRQM_FBLI_SHIFT                 7

/** Sub-registers of Register @ref PC */
/** Bit Mask for Sub-Register PC.PT */
#define PC_PT_MASK                      0x03
/** Bit Offset for Sub-Register PC.PT */
#define PC_PT_SHIFT                     0

/** Bit Mask for Sub-Register PC.BBEN */
#define PC_BBEN_MASK                    0x04
/** Bit Offset for Sub-Register PC.BBEN */
#define PC_BBEN_SHIFT                   2

/** Bit Mask for Sub-Register PC.FCST */
#define PC_FCST_MASK                    0x08
/** Bit Offset for Sub-Register PC.FCST */
#define PC_FCST_SHIFT                   3

/** Bit Mask for Sub-Register PC.TXAFCS */
#define PC_TXAFCS_MASK                  0x10
/** Bit Offset for Sub-Register PC.TXAFCS */
#define PC_TXAFCS_SHIFT                 4

/** Bit Mask for Sub-Register PC.FCSOK */
#define PC_FCSOK_MASK                   0x20
/** Bit Offset for Sub-Register PC.FCSOK */
#define PC_FCSOK_SHIFT                  5

/** Bit Mask for Sub-Register PC.FCSFE */
#define PC_FCSFE_MASK                   0x40
/** Bit Offset for Sub-Register PC.FCSFE */
#define PC_FCSFE_SHIFT                  6

/** Bit Mask for Sub-Register PC.CTX */
#define PC_CTX_MASK                     0x80
/** Bit Offset for Sub-Register PC.CTX */
#define PC_CTX_SHIFT                    7

/** Sub-registers of Register @ref PS */
/** Bit Mask for Sub-Register PS.TXUR */
#define PS_TXUR_MASK                    0x01
/** Bit Offset for Sub-Register PS.TXUR */
#define PS_TXUR_SHIFT                   0

/** Sub-registers of Register @ref RXFLL */
/** Bit Mask for Sub-Register RXFLL.RXFLL */
#define RXFLL_RXFLL_MASK                0xFF
/** Bit Offset for Sub-Register RXFLL.RXFLL */
#define RXFLL_RXFLL_SHIFT               0

/** Sub-registers of Register @ref RXFLH */
/** Bit Mask for Sub-Register RXFLH.RXFLH */
#define RXFLH_RXFLH_MASK                0x07
/** Bit Offset for Sub-Register RXFLH.RXFLH */
#define RXFLH_RXFLH_SHIFT               0

/** Sub-registers of Register @ref TXFLL */
/** Bit Mask for Sub-Register TXFLL.TXFLL */
#define TXFLL_TXFLL_MASK                0xFF
/** Bit Offset for Sub-Register TXFLL.TXFLL */
#define TXFLL_TXFLL_SHIFT               0

/** Sub-registers of Register @ref TXFLH */
/** Bit Mask for Sub-Register TXFLH.TXFLH */
#define TXFLH_TXFLH_MASK                0x07
/** Bit Offset for Sub-Register TXFLH.TXFLH */
#define TXFLH_TXFLH_SHIFT               0

/** Sub-registers of Register @ref FBLL */
/** Bit Mask for Sub-Register FBLL.FBLL */
#define FBLL_FBLL_MASK                  0xFF
/** Bit Offset for Sub-Register FBLL.FBLL */
#define FBLL_FBLL_SHIFT                 0

/** Sub-registers of Register @ref FBLH */
/** Bit Mask for Sub-Register FBLH.FBLH */
#define FBLH_FBLH_MASK                  0x07
/** Bit Offset for Sub-Register FBLH.FBLH */
#define FBLH_FBLH_SHIFT                 0

/** Sub-registers of Register @ref FBLIL */
/** Bit Mask for Sub-Register FBLIL.FBLIL */
#define FBLIL_FBLIL_MASK                0xFF
/** Bit Offset for Sub-Register FBLIL.FBLIL */
#define FBLIL_FBLIL_SHIFT               0

/** Sub-registers of Register @ref FBLIH */
/** Bit Mask for Sub-Register FBLIH.FBLIH */
#define FBLIH_FBLIH_MASK                0x07
/** Bit Offset for Sub-Register FBLIH.FBLIH */
#define FBLIH_FBLIH_SHIFT               0

/** Sub-registers of Register @ref OFDMPHRTX */
/** Bit Mask for Sub-Register OFDMPHRTX.MCS */
#define OFDMPHRTX_MCS_MASK              0x07
/** Bit Offset for Sub-Register OFDMPHRTX.MCS */
#define OFDMPHRTX_MCS_SHIFT             0

/** Bit Mask for Sub-Register OFDMPHRTX.RB5 */
#define OFDMPHRTX_RB5_MASK              0x10
/** Bit Offset for Sub-Register OFDMPHRTX.RB5 */
#define OFDMPHRTX_RB5_SHIFT             4

/** Bit Mask for Sub-Register OFDMPHRTX.RB17 */
#define OFDMPHRTX_RB17_MASK             0x20
/** Bit Offset for Sub-Register OFDMPHRTX.RB17 */
#define OFDMPHRTX_RB17_SHIFT            5

/** Bit Mask for Sub-Register OFDMPHRTX.RB18 */
#define OFDMPHRTX_RB18_MASK             0x40
/** Bit Offset for Sub-Register OFDMPHRTX.RB18 */
#define OFDMPHRTX_RB18_SHIFT            6

/** Bit Mask for Sub-Register OFDMPHRTX.RB21 */
#define OFDMPHRTX_RB21_MASK             0x80
/** Bit Offset for Sub-Register OFDMPHRTX.RB21 */
#define OFDMPHRTX_RB21_SHIFT            7

/** Sub-registers of Register @ref OFDMPHRRX */
/** Bit Mask for Sub-Register OFDMPHRRX.MCS */
#define OFDMPHRRX_MCS_MASK              0x07
/** Bit Offset for Sub-Register OFDMPHRRX.MCS */
#define OFDMPHRRX_MCS_SHIFT             0

/** Bit Mask for Sub-Register OFDMPHRRX.SPC */
#define OFDMPHRRX_SPC_MASK              0x08
/** Bit Offset for Sub-Register OFDMPHRRX.SPC */
#define OFDMPHRRX_SPC_SHIFT             3

/** Bit Mask for Sub-Register OFDMPHRRX.RB5 */
#define OFDMPHRRX_RB5_MASK              0x10
/** Bit Offset for Sub-Register OFDMPHRRX.RB5 */
#define OFDMPHRRX_RB5_SHIFT             4

/** Bit Mask for Sub-Register OFDMPHRRX.RB17 */
#define OFDMPHRRX_RB17_MASK             0x20
/** Bit Offset for Sub-Register OFDMPHRRX.RB17 */
#define OFDMPHRRX_RB17_SHIFT            5

/** Bit Mask for Sub-Register OFDMPHRRX.RB18 */
#define OFDMPHRRX_RB18_MASK             0x40
/** Bit Offset for Sub-Register OFDMPHRRX.RB18 */
#define OFDMPHRRX_RB18_SHIFT            6

/** Bit Mask for Sub-Register OFDMPHRRX.RB21 */
#define OFDMPHRRX_RB21_MASK             0x80
/** Bit Offset for Sub-Register OFDMPHRRX.RB21 */
#define OFDMPHRRX_RB21_SHIFT            7

/** Sub-registers of Register @ref OFDMC */
/** Bit Mask for Sub-Register OFDMC.OPT */
#define OFDMC_OPT_MASK                  0x03
/** Bit Offset for Sub-Register OFDMC.OPT */
#define OFDMC_OPT_SHIFT                 0

/** Bit Mask for Sub-Register OFDMC.POI */
#define OFDMC_POI_MASK                  0x04
/** Bit Offset for Sub-Register OFDMC.POI */
#define OFDMC_POI_SHIFT                 2

/** Bit Mask for Sub-Register OFDMC.LFO */
#define OFDMC_LFO_MASK                  0x08
/** Bit Offset for Sub-Register OFDMC.LFO */
#define OFDMC_LFO_SHIFT                 3

/** Bit Mask for Sub-Register OFDMC.SSTX */
#define OFDMC_SSTX_MASK                 0x30
/** Bit Offset for Sub-Register OFDMC.SSTX */
#define OFDMC_SSTX_SHIFT                4

/** Bit Mask for Sub-Register OFDMC.SSRX */
#define OFDMC_SSRX_MASK                 0xC0
/** Bit Offset for Sub-Register OFDMC.SSRX */
#define OFDMC_SSRX_SHIFT                6

/** Sub-registers of Register @ref OFDMSW */
/** Bit Mask for Sub-Register OFDMSW.RXO */
#define OFDMSW_RXO_MASK                 0x10
/** Bit Offset for Sub-Register OFDMSW.RXO */
#define OFDMSW_RXO_SHIFT                4

/** Bit Mask for Sub-Register OFDMSW.PDT */
#define OFDMSW_PDT_MASK                 0xE0
/** Bit Offset for Sub-Register OFDMSW.PDT */
#define OFDMSW_PDT_SHIFT                5

/** Sub-registers of Register @ref OQPSKC0 */
/** Bit Mask for Sub-Register OQPSKC0.FCHIP */
#define OQPSKC0_FCHIP_MASK              0x03
/** Bit Offset for Sub-Register OQPSKC0.FCHIP */
#define OQPSKC0_FCHIP_SHIFT             0

/** Bit Mask for Sub-Register OQPSKC0.MOD */
#define OQPSKC0_MOD_MASK                0x08
/** Bit Offset for Sub-Register OQPSKC0.MOD */
#define OQPSKC0_MOD_SHIFT               3

/** Bit Mask for Sub-Register OQPSKC0.DM */
#define OQPSKC0_DM_MASK                 0x10
/** Bit Offset for Sub-Register OQPSKC0.DM */
#define OQPSKC0_DM_SHIFT                4

/** Sub-registers of Register @ref OQPSKC1 */
/** Bit Mask for Sub-Register OQPSKC1.PDT0 */
#define OQPSKC1_PDT0_MASK               0x07
/** Bit Offset for Sub-Register OQPSKC1.PDT0 */
#define OQPSKC1_PDT0_SHIFT              0

/** Bit Mask for Sub-Register OQPSKC1.PDT1 */
#define OQPSKC1_PDT1_MASK               0x38
/** Bit Offset for Sub-Register OQPSKC1.PDT1 */
#define OQPSKC1_PDT1_SHIFT              3

/** Bit Mask for Sub-Register OQPSKC1.RXOLEG */
#define OQPSKC1_RXOLEG_MASK             0x40
/** Bit Offset for Sub-Register OQPSKC1.RXOLEG */
#define OQPSKC1_RXOLEG_SHIFT            6

/** Bit Mask for Sub-Register OQPSKC1.RXO */
#define OQPSKC1_RXO_MASK                0x80
/** Bit Offset for Sub-Register OQPSKC1.RXO */
#define OQPSKC1_RXO_SHIFT               7

/** Sub-registers of Register @ref OQPSKC2 */
/** Bit Mask for Sub-Register OQPSKC2.RXM */
#define OQPSKC2_RXM_MASK                0x03
/** Bit Offset for Sub-Register OQPSKC2.RXM */
#define OQPSKC2_RXM_SHIFT               0

/** Bit Mask for Sub-Register OQPSKC2.FCSTLEG */
#define OQPSKC2_FCSTLEG_MASK            0x04
/** Bit Offset for Sub-Register OQPSKC2.FCSTLEG */
#define OQPSKC2_FCSTLEG_SHIFT           2

/** Bit Mask for Sub-Register OQPSKC2.ENPROP */
#define OQPSKC2_ENPROP_MASK             0x08
/** Bit Offset for Sub-Register OQPSKC2.ENPROP */
#define OQPSKC2_ENPROP_SHIFT            3

/** Bit Mask for Sub-Register OQPSKC2.RPC */
#define OQPSKC2_RPC_MASK                0x10
/** Bit Offset for Sub-Register OQPSKC2.RPC */
#define OQPSKC2_RPC_SHIFT               4

/** Bit Mask for Sub-Register OQPSKC2.SPC */
#define OQPSKC2_SPC_MASK                0x20
/** Bit Offset for Sub-Register OQPSKC2.SPC */
#define OQPSKC2_SPC_SHIFT               5

/** Sub-registers of Register @ref OQPSKC3 */
/** Bit Mask for Sub-Register OQPSKC3.NSFD */
#define OQPSKC3_NSFD_MASK               0x0C
/** Bit Offset for Sub-Register OQPSKC3.NSFD */
#define OQPSKC3_NSFD_SHIFT              2

/** Bit Mask for Sub-Register OQPSKC3.HRLEG */
#define OQPSKC3_HRLEG_MASK              0x20
/** Bit Offset for Sub-Register OQPSKC3.HRLEG */
#define OQPSKC3_HRLEG_SHIFT             5

/** Sub-registers of Register @ref OQPSKPHRTX */
/** Bit Mask for Sub-Register OQPSKPHRTX.LEG */
#define OQPSKPHRTX_LEG_MASK             0x01
/** Bit Offset for Sub-Register OQPSKPHRTX.LEG */
#define OQPSKPHRTX_LEG_SHIFT            0

/** Bit Mask for Sub-Register OQPSKPHRTX.MOD */
#define OQPSKPHRTX_MOD_MASK             0x0E
/** Bit Offset for Sub-Register OQPSKPHRTX.MOD */
#define OQPSKPHRTX_MOD_SHIFT            1

/** Bit Mask for Sub-Register OQPSKPHRTX.RB0 */
#define OQPSKPHRTX_RB0_MASK             0x10
/** Bit Offset for Sub-Register OQPSKPHRTX.RB0 */
#define OQPSKPHRTX_RB0_SHIFT            4

/** Bit Mask for Sub-Register OQPSKPHRTX.PPDUT */
#define OQPSKPHRTX_PPDUT_MASK           0x20
/** Bit Offset for Sub-Register OQPSKPHRTX.PPDUT */
#define OQPSKPHRTX_PPDUT_SHIFT          5

/** Sub-registers of Register @ref OQPSKPHRRX */
/** Bit Mask for Sub-Register OQPSKPHRRX.LEG */
#define OQPSKPHRRX_LEG_MASK             0x01
/** Bit Offset for Sub-Register OQPSKPHRRX.LEG */
#define OQPSKPHRRX_LEG_SHIFT            0

/** Bit Mask for Sub-Register OQPSKPHRRX.MOD */
#define OQPSKPHRRX_MOD_MASK             0x0E
/** Bit Offset for Sub-Register OQPSKPHRRX.MOD */
#define OQPSKPHRRX_MOD_SHIFT            1

/** Bit Mask for Sub-Register OQPSKPHRRX.RB0 */
#define OQPSKPHRRX_RB0_MASK             0x10
/** Bit Offset for Sub-Register OQPSKPHRRX.RB0 */
#define OQPSKPHRRX_RB0_SHIFT            4

/** Bit Mask for Sub-Register OQPSKPHRRX.PPDUT */
#define OQPSKPHRRX_PPDUT_MASK           0x20
/** Bit Offset for Sub-Register OQPSKPHRRX.PPDUT */
#define OQPSKPHRRX_PPDUT_SHIFT          5

/** Sub-registers of Register @ref AFC0 */
/** Bit Mask for Sub-Register AFC0.AFEN0 */
#define AFC0_AFEN0_MASK                 0x01
/** Bit Offset for Sub-Register AFC0.AFEN0 */
#define AFC0_AFEN0_SHIFT                0

/** Bit Mask for Sub-Register AFC0.AFEN1 */
#define AFC0_AFEN1_MASK                 0x02
/** Bit Offset for Sub-Register AFC0.AFEN1 */
#define AFC0_AFEN1_SHIFT                1

/** Bit Mask for Sub-Register AFC0.AFEN2 */
#define AFC0_AFEN2_MASK                 0x04
/** Bit Offset for Sub-Register AFC0.AFEN2 */
#define AFC0_AFEN2_SHIFT                2

/** Bit Mask for Sub-Register AFC0.AFEN3 */
#define AFC0_AFEN3_MASK                 0x08
/** Bit Offset for Sub-Register AFC0.AFEN3 */
#define AFC0_AFEN3_SHIFT                3

/** Bit Mask for Sub-Register AFC0.PM */
#define AFC0_PM_MASK                    0x10
/** Bit Offset for Sub-Register AFC0.PM */
#define AFC0_PM_SHIFT                   4

/** Sub-registers of Register @ref AFC1 */
/** Bit Mask for Sub-Register AFC1.PANC0 */
#define AFC1_PANC0_MASK                 0x01
/** Bit Offset for Sub-Register AFC1.PANC0 */
#define AFC1_PANC0_SHIFT                0

/** Bit Mask for Sub-Register AFC1.PANC1 */
#define AFC1_PANC1_MASK                 0x02
/** Bit Offset for Sub-Register AFC1.PANC1 */
#define AFC1_PANC1_SHIFT                1

/** Bit Mask for Sub-Register AFC1.PANC2 */
#define AFC1_PANC2_MASK                 0x04
/** Bit Offset for Sub-Register AFC1.PANC2 */
#define AFC1_PANC2_SHIFT                2

/** Bit Mask for Sub-Register AFC1.PANC3 */
#define AFC1_PANC3_MASK                 0x08
/** Bit Offset for Sub-Register AFC1.PANC3 */
#define AFC1_PANC3_SHIFT                3

/** Bit Mask for Sub-Register AFC1.MRFT0 */
#define AFC1_MRFT0_MASK                 0x10
/** Bit Offset for Sub-Register AFC1.MRFT0 */
#define AFC1_MRFT0_SHIFT                4

/** Bit Mask for Sub-Register AFC1.MRFT1 */
#define AFC1_MRFT1_MASK                 0x20
/** Bit Offset for Sub-Register AFC1.MRFT1 */
#define AFC1_MRFT1_SHIFT                5

/** Bit Mask for Sub-Register AFC1.MRFT2 */
#define AFC1_MRFT2_MASK                 0x40
/** Bit Offset for Sub-Register AFC1.MRFT2 */
#define AFC1_MRFT2_SHIFT                6

/** Bit Mask for Sub-Register AFC1.MRFT3 */
#define AFC1_MRFT3_MASK                 0x80
/** Bit Offset for Sub-Register AFC1.MRFT3 */
#define AFC1_MRFT3_SHIFT                7

/** Sub-registers of Register @ref AFFTM */
/** Bit Mask for Sub-Register AFFTM.AFFTM */
#define AFFTM_AFFTM_MASK                0xFF
/** Bit Offset for Sub-Register AFFTM.AFFTM */
#define AFFTM_AFFTM_SHIFT               0

/** Sub-registers of Register @ref AFFVM */
/** Bit Mask for Sub-Register AFFVM.AFFVM */
#define AFFVM_AFFVM_MASK                0x0F
/** Bit Offset for Sub-Register AFFVM.AFFVM */
#define AFFVM_AFFVM_SHIFT               0

/** Sub-registers of Register @ref AFS */
/** Bit Mask for Sub-Register AFS.AM0 */
#define AFS_AM0_MASK                    0x01
/** Bit Offset for Sub-Register AFS.AM0 */
#define AFS_AM0_SHIFT                   0

/** Bit Mask for Sub-Register AFS.AM1 */
#define AFS_AM1_MASK                    0x02
/** Bit Offset for Sub-Register AFS.AM1 */
#define AFS_AM1_SHIFT                   1

/** Bit Mask for Sub-Register AFS.AM2 */
#define AFS_AM2_MASK                    0x04
/** Bit Offset for Sub-Register AFS.AM2 */
#define AFS_AM2_SHIFT                   2

/** Bit Mask for Sub-Register AFS.AM3 */
#define AFS_AM3_MASK                    0x08
/** Bit Offset for Sub-Register AFS.AM3 */
#define AFS_AM3_SHIFT                   3

/** Bit Mask for Sub-Register AFS.EM */
#define AFS_EM_MASK                     0x10
/** Bit Offset for Sub-Register AFS.EM */
#define AFS_EM_SHIFT                    4

/** Sub-registers of Register @ref MACEA0 */
/** Bit Mask for Sub-Register MACEA0.MACEA0 */
#define MACEA0_MACEA0_MASK              0xFF
/** Bit Offset for Sub-Register MACEA0.MACEA0 */
#define MACEA0_MACEA0_SHIFT             0

/** Sub-registers of Register @ref MACEA1 */
/** Bit Mask for Sub-Register MACEA1.MACEA1 */
#define MACEA1_MACEA1_MASK              0xFF
/** Bit Offset for Sub-Register MACEA1.MACEA1 */
#define MACEA1_MACEA1_SHIFT             0

/** Sub-registers of Register @ref MACEA2 */
/** Bit Mask for Sub-Register MACEA2.MACEA2 */
#define MACEA2_MACEA2_MASK              0xFF
/** Bit Offset for Sub-Register MACEA2.MACEA2 */
#define MACEA2_MACEA2_SHIFT             0

/** Sub-registers of Register @ref MACEA3 */
/** Bit Mask for Sub-Register MACEA3.MACEA3 */
#define MACEA3_MACEA3_MASK              0xFF
/** Bit Offset for Sub-Register MACEA3.MACEA3 */
#define MACEA3_MACEA3_SHIFT             0

/** Sub-registers of Register @ref MACEA4 */
/** Bit Mask for Sub-Register MACEA4.MACEA4 */
#define MACEA4_MACEA4_MASK              0xFF
/** Bit Offset for Sub-Register MACEA4.MACEA4 */
#define MACEA4_MACEA4_SHIFT             0

/** Sub-registers of Register @ref MACEA5 */
/** Bit Mask for Sub-Register MACEA5.MACEA5 */
#define MACEA5_MACEA5_MASK              0xFF
/** Bit Offset for Sub-Register MACEA5.MACEA5 */
#define MACEA5_MACEA5_SHIFT             0

/** Sub-registers of Register @ref MACEA6 */
/** Bit Mask for Sub-Register MACEA6.MACEA6 */
#define MACEA6_MACEA6_MASK              0xFF
/** Bit Offset for Sub-Register MACEA6.MACEA6 */
#define MACEA6_MACEA6_SHIFT             0

/** Sub-registers of Register @ref MACEA7 */
/** Bit Mask for Sub-Register MACEA7.MACEA7 */
#define MACEA7_MACEA7_MASK              0xFF
/** Bit Offset for Sub-Register MACEA7.MACEA7 */
#define MACEA7_MACEA7_SHIFT             0

/** Sub-registers of Register @ref MACPID0F0 */
/** Bit Mask for Sub-Register MACPID0F0.MACPID0F0 */
#define MACPID0F0_MACPID0F0_MASK        0xFF
/** Bit Offset for Sub-Register MACPID0F0.MACPID0F0 */
#define MACPID0F0_MACPID0F0_SHIFT       0

/** Sub-registers of Register @ref MACPID1F0 */
/** Bit Mask for Sub-Register MACPID1F0.MACPID1F0 */
#define MACPID1F0_MACPID1F0_MASK        0xFF
/** Bit Offset for Sub-Register MACPID1F0.MACPID1F0 */
#define MACPID1F0_MACPID1F0_SHIFT       0

/** Sub-registers of Register @ref MACSHA0F0 */
/** Bit Mask for Sub-Register MACSHA0F0.MACSHA0F0 */
#define MACSHA0F0_MACSHA0F0_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA0F0.MACSHA0F0 */
#define MACSHA0F0_MACSHA0F0_SHIFT       0

/** Sub-registers of Register @ref MACSHA1F0 */
/** Bit Mask for Sub-Register MACSHA1F0.MACSHA1F0 */
#define MACSHA1F0_MACSHA1F0_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA1F0.MACSHA1F0 */
#define MACSHA1F0_MACSHA1F0_SHIFT       0

/** Sub-registers of Register @ref MACPID0F1 */
/** Bit Mask for Sub-Register MACPID0F1.MACPID0F1 */
#define MACPID0F1_MACPID0F1_MASK        0xFF
/** Bit Offset for Sub-Register MACPID0F1.MACPID0F1 */
#define MACPID0F1_MACPID0F1_SHIFT       0

/** Sub-registers of Register @ref MACPID1F1 */
/** Bit Mask for Sub-Register MACPID1F1.MACPID1F1 */
#define MACPID1F1_MACPID1F1_MASK        0xFF
/** Bit Offset for Sub-Register MACPID1F1.MACPID1F1 */
#define MACPID1F1_MACPID1F1_SHIFT       0

/** Sub-registers of Register @ref MACSHA0F1 */
/** Bit Mask for Sub-Register MACSHA0F1.MACSHA0F1 */
#define MACSHA0F1_MACSHA0F1_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA0F1.MACSHA0F1 */
#define MACSHA0F1_MACSHA0F1_SHIFT       0

/** Sub-registers of Register @ref MACSHA1F1 */
/** Bit Mask for Sub-Register MACSHA1F1.MACSHA1F1 */
#define MACSHA1F1_MACSHA1F1_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA1F1.MACSHA1F1 */
#define MACSHA1F1_MACSHA1F1_SHIFT       0

/** Sub-registers of Register @ref MACPID0F2 */
/** Bit Mask for Sub-Register MACPID0F2.MACPID0F2 */
#define MACPID0F2_MACPID0F2_MASK        0xFF
/** Bit Offset for Sub-Register MACPID0F2.MACPID0F2 */
#define MACPID0F2_MACPID0F2_SHIFT       0

/** Sub-registers of Register @ref MACPID1F2 */
/** Bit Mask for Sub-Register MACPID1F2.MACPID1F2 */
#define MACPID1F2_MACPID1F2_MASK        0xFF
/** Bit Offset for Sub-Register MACPID1F2.MACPID1F2 */
#define MACPID1F2_MACPID1F2_SHIFT       0

/** Sub-registers of Register @ref MACSHA0F2 */
/** Bit Mask for Sub-Register MACSHA0F2.MACSHA0F2 */
#define MACSHA0F2_MACSHA0F2_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA0F2.MACSHA0F2 */
#define MACSHA0F2_MACSHA0F2_SHIFT       0

/** Sub-registers of Register @ref MACSHA1F2 */
/** Bit Mask for Sub-Register MACSHA1F2.MACSHA1F2 */
#define MACSHA1F2_MACSHA1F2_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA1F2.MACSHA1F2 */
#define MACSHA1F2_MACSHA1F2_SHIFT       0

/** Sub-registers of Register @ref MACPID0F3 */
/** Bit Mask for Sub-Register MACPID0F3.MACPID0F3 */
#define MACPID0F3_MACPID0F3_MASK        0xFF
/** Bit Offset for Sub-Register MACPID0F3.MACPID0F3 */
#define MACPID0F3_MACPID0F3_SHIFT       0

/** Sub-registers of Register @ref MACPID1F3 */
/** Bit Mask for Sub-Register MACPID1F3.MACPID1F3 */
#define MACPID1F3_MACPID1F3_MASK        0xFF
/** Bit Offset for Sub-Register MACPID1F3.MACPID1F3 */
#define MACPID1F3_MACPID1F3_SHIFT       0

/** Sub-registers of Register @ref MACSHA0F3 */
/** Bit Mask for Sub-Register MACSHA0F3.MACSHA0F3 */
#define MACSHA0F3_MACSHA0F3_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA0F3.MACSHA0F3 */
#define MACSHA0F3_MACSHA0F3_SHIFT       0

/** Sub-registers of Register @ref MACSHA1F3 */
/** Bit Mask for Sub-Register MACSHA1F3.MACSHA1F3 */
#define MACSHA1F3_MACSHA1F3_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA1F3.MACSHA1F3 */
#define MACSHA1F3_MACSHA1F3_SHIFT       0

/** Sub-registers of Register @ref AMCS */
/** Bit Mask for Sub-Register AMCS.TX2RX */
#define AMCS_TX2RX_MASK                 0x01
/** Bit Offset for Sub-Register AMCS.TX2RX */
#define AMCS_TX2RX_SHIFT                0

/** Bit Mask for Sub-Register AMCS.CCATX */
#define AMCS_CCATX_MASK                 0x02
/** Bit Offset for Sub-Register AMCS.CCATX */
#define AMCS_CCATX_SHIFT                1

/** Bit Mask for Sub-Register AMCS.CCAED */
#define AMCS_CCAED_MASK                 0x04
/** Bit Offset for Sub-Register AMCS.CCAED */
#define AMCS_CCAED_SHIFT                2

/** Bit Mask for Sub-Register AMCS.AACK */
#define AMCS_AACK_MASK                  0x08
/** Bit Offset for Sub-Register AMCS.AACK */
#define AMCS_AACK_SHIFT                 3

/** Bit Mask for Sub-Register AMCS.AACKS */
#define AMCS_AACKS_MASK                 0x10
/** Bit Offset for Sub-Register AMCS.AACKS */
#define AMCS_AACKS_SHIFT                4

/** Bit Mask for Sub-Register AMCS.AACKDR */
#define AMCS_AACKDR_MASK                0x20
/** Bit Offset for Sub-Register AMCS.AACKDR */
#define AMCS_AACKDR_SHIFT               5

/** Bit Mask for Sub-Register AMCS.AACKFA */
#define AMCS_AACKFA_MASK                0x40
/** Bit Offset for Sub-Register AMCS.AACKFA */
#define AMCS_AACKFA_SHIFT               6

/** Bit Mask for Sub-Register AMCS.AACKFT */
#define AMCS_AACKFT_MASK                0x80
/** Bit Offset for Sub-Register AMCS.AACKFT */
#define AMCS_AACKFT_SHIFT               7

/** Sub-registers of Register @ref AMEDT */
/** Bit Mask for Sub-Register AMEDT.AMEDT */
#define AMEDT_AMEDT_MASK                0xFF
/** Bit Offset for Sub-Register AMEDT.AMEDT */
#define AMEDT_AMEDT_SHIFT               0

/** Sub-registers of Register @ref AMAACKPD */
/** Bit Mask for Sub-Register AMAACKPD.PD0 */
#define AMAACKPD_PD0_MASK               0x01
/** Bit Offset for Sub-Register AMAACKPD.PD0 */
#define AMAACKPD_PD0_SHIFT              0

/** Bit Mask for Sub-Register AMAACKPD.PD1 */
#define AMAACKPD_PD1_MASK               0x02
/** Bit Offset for Sub-Register AMAACKPD.PD1 */
#define AMAACKPD_PD1_SHIFT              1

/** Bit Mask for Sub-Register AMAACKPD.PD2 */
#define AMAACKPD_PD2_MASK               0x04
/** Bit Offset for Sub-Register AMAACKPD.PD2 */
#define AMAACKPD_PD2_SHIFT              2

/** Bit Mask for Sub-Register AMAACKPD.PD3 */
#define AMAACKPD_PD3_MASK               0x08
/** Bit Offset for Sub-Register AMAACKPD.PD3 */
#define AMAACKPD_PD3_SHIFT              3

/** Sub-registers of Register @ref AMAACKTL */
/** Bit Mask for Sub-Register AMAACKTL.AMAACKTL */
#define AMAACKTL_AMAACKTL_MASK          0xFF
/** Bit Offset for Sub-Register AMAACKTL.AMAACKTL */
#define AMAACKTL_AMAACKTL_SHIFT         0

/** Sub-registers of Register @ref AMAACKTH */
/** Bit Mask for Sub-Register AMAACKTH.AMAACKTH */
#define AMAACKTH_AMAACKTH_MASK          0x07
/** Bit Offset for Sub-Register AMAACKTH.AMAACKTH */
#define AMAACKTH_AMAACKTH_SHIFT         0

/** Sub-registers of Register @ref FSKC0 */
/** Bit Mask for Sub-Register FSKC0.MORD */
#define FSKC0_MORD_MASK                 0x01
/** Bit Offset for Sub-Register FSKC0.MORD */
#define FSKC0_MORD_SHIFT                0

/** Bit Mask for Sub-Register FSKC0.MIDX */
#define FSKC0_MIDX_MASK                 0x0E
/** Bit Offset for Sub-Register FSKC0.MIDX */
#define FSKC0_MIDX_SHIFT                1

/** Bit Mask for Sub-Register FSKC0.MIDXS */
#define FSKC0_MIDXS_MASK                0x30
/** Bit Offset for Sub-Register FSKC0.MIDXS */
#define FSKC0_MIDXS_SHIFT               4

/** Bit Mask for Sub-Register FSKC0.BT */
#define FSKC0_BT_MASK                   0xC0
/** Bit Offset for Sub-Register FSKC0.BT */
#define FSKC0_BT_SHIFT                  6

/** Sub-registers of Register @ref FSKC1 */
/** Bit Mask for Sub-Register FSKC1.SRATE */
#define FSKC1_SRATE_MASK                0x0F
/** Bit Offset for Sub-Register FSKC1.SRATE */
#define FSKC1_SRATE_SHIFT               0

/** Bit Mask for Sub-Register FSKC1.FI */
#define FSKC1_FI_MASK                   0x20
/** Bit Offset for Sub-Register FSKC1.FI */
#define FSKC1_FI_SHIFT                  5

/** Bit Mask for Sub-Register FSKC1.FSKPLH */
#define FSKC1_FSKPLH_MASK               0xC0
/** Bit Offset for Sub-Register FSKC1.FSKPLH */
#define FSKC1_FSKPLH_SHIFT              6

/** Sub-registers of Register @ref FSKC2 */
/** Bit Mask for Sub-Register FSKC2.FECIE */
#define FSKC2_FECIE_MASK                0x01
/** Bit Offset for Sub-Register FSKC2.FECIE */
#define FSKC2_FECIE_SHIFT               0

/** Bit Mask for Sub-Register FSKC2.FECS */
#define FSKC2_FECS_MASK                 0x02
/** Bit Offset for Sub-Register FSKC2.FECS */
#define FSKC2_FECS_SHIFT                1

/** Bit Mask for Sub-Register FSKC2.PRI */
#define FSKC2_PRI_MASK                  0x04
/** Bit Offset for Sub-Register FSKC2.PRI */
#define FSKC2_PRI_SHIFT                 2

/** Bit Mask for Sub-Register FSKC2.MSE */
#define FSKC2_MSE_MASK                  0x08
/** Bit Offset for Sub-Register FSKC2.MSE */
#define FSKC2_MSE_SHIFT                 3

/** Bit Mask for Sub-Register FSKC2.RXPTO */
#define FSKC2_RXPTO_MASK                0x10
/** Bit Offset for Sub-Register FSKC2.RXPTO */
#define FSKC2_RXPTO_SHIFT               4

/** Bit Mask for Sub-Register FSKC2.RXO */
#define FSKC2_RXO_MASK                  0x60
/** Bit Offset for Sub-Register FSKC2.RXO */
#define FSKC2_RXO_SHIFT                 5

/** Bit Mask for Sub-Register FSKC2.PDTM */
#define FSKC2_PDTM_MASK                 0x80
/** Bit Offset for Sub-Register FSKC2.PDTM */
#define FSKC2_PDTM_SHIFT                7

/** Sub-registers of Register @ref FSKC3 */
/** Bit Mask for Sub-Register FSKC3.PDT */
#define FSKC3_PDT_MASK                  0x0F
/** Bit Offset for Sub-Register FSKC3.PDT */
#define FSKC3_PDT_SHIFT                 0

/** Bit Mask for Sub-Register FSKC3.SFDT */
#define FSKC3_SFDT_MASK                 0xF0
/** Bit Offset for Sub-Register FSKC3.SFDT */
#define FSKC3_SFDT_SHIFT                4

/** Sub-registers of Register @ref FSKC4 */
/** Bit Mask for Sub-Register FSKC4.CSFD0 */
#define FSKC4_CSFD0_MASK                0x03
/** Bit Offset for Sub-Register FSKC4.CSFD0 */
#define FSKC4_CSFD0_SHIFT               0

/** Bit Mask for Sub-Register FSKC4.CSFD1 */
#define FSKC4_CSFD1_MASK                0x0C
/** Bit Offset for Sub-Register FSKC4.CSFD1 */
#define FSKC4_CSFD1_SHIFT               2

/** Bit Mask for Sub-Register FSKC4.RAWRBIT */
#define FSKC4_RAWRBIT_MASK              0x10
/** Bit Offset for Sub-Register FSKC4.RAWRBIT */
#define FSKC4_RAWRBIT_SHIFT             4

/** Bit Mask for Sub-Register FSKC4.SFD32 */
#define FSKC4_SFD32_MASK                0x20
/** Bit Offset for Sub-Register FSKC4.SFD32 */
#define FSKC4_SFD32_SHIFT               5

/** Bit Mask for Sub-Register FSKC4.SFDQ */
#define FSKC4_SFDQ_MASK                 0x40
/** Bit Offset for Sub-Register FSKC4.SFDQ */
#define FSKC4_SFDQ_SHIFT                6

/** Sub-registers of Register @ref FSKPLL */
/** Bit Mask for Sub-Register FSKPLL.FSKPLL */
#define FSKPLL_FSKPLL_MASK              0xFF
/** Bit Offset for Sub-Register FSKPLL.FSKPLL */
#define FSKPLL_FSKPLL_SHIFT             0

/** Sub-registers of Register @ref FSKSFD0L */
/** Bit Mask for Sub-Register FSKSFD0L.FSKSFD0L */
#define FSKSFD0L_FSKSFD0L_MASK          0xFF
/** Bit Offset for Sub-Register FSKSFD0L.FSKSFD0L */
#define FSKSFD0L_FSKSFD0L_SHIFT         0

/** Sub-registers of Register @ref FSKSFD0H */
/** Bit Mask for Sub-Register FSKSFD0H.FSKSFD0H */
#define FSKSFD0H_FSKSFD0H_MASK          0xFF
/** Bit Offset for Sub-Register FSKSFD0H.FSKSFD0H */
#define FSKSFD0H_FSKSFD0H_SHIFT         0

/** Sub-registers of Register @ref FSKSFD1L */
/** Bit Mask for Sub-Register FSKSFD1L.FSKSFD1L */
#define FSKSFD1L_FSKSFD1L_MASK          0xFF
/** Bit Offset for Sub-Register FSKSFD1L.FSKSFD1L */
#define FSKSFD1L_FSKSFD1L_SHIFT         0

/** Sub-registers of Register @ref FSKSFD1H */
/** Bit Mask for Sub-Register FSKSFD1H.FSKSFD1H */
#define FSKSFD1H_FSKSFD1H_MASK          0xFF
/** Bit Offset for Sub-Register FSKSFD1H.FSKSFD1H */
#define FSKSFD1H_FSKSFD1H_SHIFT         0

/** Sub-registers of Register @ref FSKPHRTX */
/** Bit Mask for Sub-Register FSKPHRTX.RB1 */
#define FSKPHRTX_RB1_MASK               0x01
/** Bit Offset for Sub-Register FSKPHRTX.RB1 */
#define FSKPHRTX_RB1_SHIFT              0

/** Bit Mask for Sub-Register FSKPHRTX.RB2 */
#define FSKPHRTX_RB2_MASK               0x02
/** Bit Offset for Sub-Register FSKPHRTX.RB2 */
#define FSKPHRTX_RB2_SHIFT              1

/** Bit Mask for Sub-Register FSKPHRTX.DW */
#define FSKPHRTX_DW_MASK                0x04
/** Bit Offset for Sub-Register FSKPHRTX.DW */
#define FSKPHRTX_DW_SHIFT               2

/** Bit Mask for Sub-Register FSKPHRTX.SFD */
#define FSKPHRTX_SFD_MASK               0x08
/** Bit Offset for Sub-Register FSKPHRTX.SFD */
#define FSKPHRTX_SFD_SHIFT              3

/** Sub-registers of Register @ref FSKPHRRX */
/** Bit Mask for Sub-Register FSKPHRRX.RB1 */
#define FSKPHRRX_RB1_MASK               0x01
/** Bit Offset for Sub-Register FSKPHRRX.RB1 */
#define FSKPHRRX_RB1_SHIFT              0

/** Bit Mask for Sub-Register FSKPHRRX.RB2 */
#define FSKPHRRX_RB2_MASK               0x02
/** Bit Offset for Sub-Register FSKPHRRX.RB2 */
#define FSKPHRRX_RB2_SHIFT              1

/** Bit Mask for Sub-Register FSKPHRRX.DW */
#define FSKPHRRX_DW_MASK                0x04
/** Bit Offset for Sub-Register FSKPHRRX.DW */
#define FSKPHRRX_DW_SHIFT               2

/** Bit Mask for Sub-Register FSKPHRRX.SFD */
#define FSKPHRRX_SFD_MASK               0x08
/** Bit Offset for Sub-Register FSKPHRRX.SFD */
#define FSKPHRRX_SFD_SHIFT              3

/** Bit Mask for Sub-Register FSKPHRRX.MS */
#define FSKPHRRX_MS_MASK                0x40
/** Bit Offset for Sub-Register FSKPHRRX.MS */
#define FSKPHRRX_MS_SHIFT               6

/** Bit Mask for Sub-Register FSKPHRRX.FCST */
#define FSKPHRRX_FCST_MASK              0x80
/** Bit Offset for Sub-Register FSKPHRRX.FCST */
#define FSKPHRRX_FCST_SHIFT             7

/** Sub-registers of Register @ref FSKRPC */
/** Bit Mask for Sub-Register FSKRPC.BASET */
#define FSKRPC_BASET_MASK               0x07
/** Bit Offset for Sub-Register FSKRPC.BASET */
#define FSKRPC_BASET_SHIFT              0

/** Bit Mask for Sub-Register FSKRPC.EN */
#define FSKRPC_EN_MASK                  0x08
/** Bit Offset for Sub-Register FSKRPC.EN */
#define FSKRPC_EN_SHIFT                 3

/** Sub-registers of Register @ref FSKRPCONT */
/** Bit Mask for Sub-Register FSKRPCONT.FSKRPCONT */
#define FSKRPCONT_FSKRPCONT_MASK        0xFF
/** Bit Offset for Sub-Register FSKRPCONT.FSKRPCONT */
#define FSKRPCONT_FSKRPCONT_SHIFT       0

/** Sub-registers of Register @ref FSKRPCOFFT */
/** Bit Mask for Sub-Register FSKRPCOFFT.FSKRPCOFFT */
#define FSKRPCOFFT_FSKRPCOFFT_MASK      0xFF
/** Bit Offset for Sub-Register FSKRPCOFFT.FSKRPCOFFT */
#define FSKRPCOFFT_FSKRPCOFFT_SHIFT     0

/** Sub-registers of Register @ref FSKRRXFLL */
/** Bit Mask for Sub-Register FSKRRXFLL.FSKRRXFLL */
#define FSKRRXFLL_FSKRRXFLL_MASK        0xFF
/** Bit Offset for Sub-Register FSKRRXFLL.FSKRRXFLL */
#define FSKRRXFLL_FSKRRXFLL_SHIFT       0

/** Sub-registers of Register @ref FSKRRXFLH */
/** Bit Mask for Sub-Register FSKRRXFLH.FSKRRXFLH */
#define FSKRRXFLH_FSKRRXFLH_MASK        0x07
/** Bit Offset for Sub-Register FSKRRXFLH.FSKRRXFLH */
#define FSKRRXFLH_FSKRRXFLH_SHIFT       0

/** Sub-registers of Register @ref FSKDM */
/** Bit Mask for Sub-Register FSKDM.EN */
#define FSKDM_EN_MASK                   0x01
/** Bit Offset for Sub-Register FSKDM.EN */
#define FSKDM_EN_SHIFT                  0

/** Bit Mask for Sub-Register FSKDM.PE */
#define FSKDM_PE_MASK                   0x02
/** Bit Offset for Sub-Register FSKDM.PE */
#define FSKDM_PE_SHIFT                  1

/** Sub-registers of Register @ref FSKPE0 */
/** Bit Mask for Sub-Register FSKPE0.FSKPE0 */
#define FSKPE0_FSKPE0_MASK              0xFF
/** Bit Offset for Sub-Register FSKPE0.FSKPE0 */
#define FSKPE0_FSKPE0_SHIFT             0

/** Sub-registers of Register @ref FSKPE1 */
/** Bit Mask for Sub-Register FSKPE1.FSKPE1 */
#define FSKPE1_FSKPE1_MASK              0xFF
/** Bit Offset for Sub-Register FSKPE1.FSKPE1 */
#define FSKPE1_FSKPE1_SHIFT             0

/** Sub-registers of Register @ref FSKPE2 */
/** Bit Mask for Sub-Register FSKPE2.FSKPE2 */
#define FSKPE2_FSKPE2_MASK              0xFF
/** Bit Offset for Sub-Register FSKPE2.FSKPE2 */
#define FSKPE2_FSKPE2_SHIFT             0

/** Sub-registers of Register @ref PMUC */
/** Bit Mask for Sub-Register PMUC.EN */
#define PMUC_EN_MASK                    0x01
/** Bit Offset for Sub-Register PMUC.EN */
#define PMUC_EN_SHIFT                   0

/** Bit Mask for Sub-Register PMUC.AVG */
#define PMUC_AVG_MASK                   0x02
/** Bit Offset for Sub-Register PMUC.AVG */
#define PMUC_AVG_SHIFT                  1

/** Bit Mask for Sub-Register PMUC.SYNC */
#define PMUC_SYNC_MASK                  0x1C
/** Bit Offset for Sub-Register PMUC.SYNC */
#define PMUC_SYNC_SHIFT                 2

/** Bit Mask for Sub-Register PMUC.FED */
#define PMUC_FED_MASK                   0x20
/** Bit Offset for Sub-Register PMUC.FED */
#define PMUC_FED_SHIFT                  5

/** Bit Mask for Sub-Register PMUC.IQSEL */
#define PMUC_IQSEL_MASK                 0x40
/** Bit Offset for Sub-Register PMUC.IQSEL */
#define PMUC_IQSEL_SHIFT                6

/** Bit Mask for Sub-Register PMUC.CCFTS */
#define PMUC_CCFTS_MASK                 0x80
/** Bit Offset for Sub-Register PMUC.CCFTS */
#define PMUC_CCFTS_SHIFT                7

/** Sub-registers of Register @ref PMUVAL */
/** Bit Mask for Sub-Register PMUVAL.PMUVAL */
#define PMUVAL_PMUVAL_MASK              0xFF
/** Bit Offset for Sub-Register PMUVAL.PMUVAL */
#define PMUVAL_PMUVAL_SHIFT             0

/** Sub-registers of Register @ref PMUQF */
/** Bit Mask for Sub-Register PMUQF.PMUQF */
#define PMUQF_PMUQF_MASK                0xFF
/** Bit Offset for Sub-Register PMUQF.PMUQF */
#define PMUQF_PMUQF_SHIFT               0

/** Sub-registers of Register @ref PMUI */
/** Bit Mask for Sub-Register PMUI.PMUI */
#define PMUI_PMUI_MASK                  0xFF
/** Bit Offset for Sub-Register PMUI.PMUI */
#define PMUI_PMUI_SHIFT                 0

/** Sub-registers of Register @ref PMUQ */
/** Bit Mask for Sub-Register PMUQ.PMUQ */
#define PMUQ_PMUQ_MASK                  0xFF
/** Bit Offset for Sub-Register PMUQ.PMUQ */
#define PMUQ_PMUQ_SHIFT                 0

/** Sub-registers of Register @ref CNTC */
/** Bit Mask for Sub-Register CNTC.EN */
#define CNTC_EN_MASK                    0x01
/** Bit Offset for Sub-Register CNTC.EN */
#define CNTC_EN_SHIFT                   0

/** Bit Mask for Sub-Register CNTC.RSTRXS */
#define CNTC_RSTRXS_MASK                0x02
/** Bit Offset for Sub-Register CNTC.RSTRXS */
#define CNTC_RSTRXS_SHIFT               1

/** Bit Mask for Sub-Register CNTC.RSTTXS */
#define CNTC_RSTTXS_MASK                0x04
/** Bit Offset for Sub-Register CNTC.RSTTXS */
#define CNTC_RSTTXS_SHIFT               2

/** Bit Mask for Sub-Register CNTC.CAPRXS */
#define CNTC_CAPRXS_MASK                0x08
/** Bit Offset for Sub-Register CNTC.CAPRXS */
#define CNTC_CAPRXS_SHIFT               3

/** Bit Mask for Sub-Register CNTC.CAPTXS */
#define CNTC_CAPTXS_MASK                0x10
/** Bit Offset for Sub-Register CNTC.CAPTXS */
#define CNTC_CAPTXS_SHIFT               4

/** Sub-registers of Register @ref CNT0 */
/** Bit Mask for Sub-Register CNT0.CNT0 */
#define CNT0_CNT0_MASK                  0xFF
/** Bit Offset for Sub-Register CNT0.CNT0 */
#define CNT0_CNT0_SHIFT                 0

/** Sub-registers of Register @ref CNT1 */
/** Bit Mask for Sub-Register CNT1.CNT1 */
#define CNT1_CNT1_MASK                  0xFF
/** Bit Offset for Sub-Register CNT1.CNT1 */
#define CNT1_CNT1_SHIFT                 0

/** Sub-registers of Register @ref CNT2 */
/** Bit Mask for Sub-Register CNT2.CNT2 */
#define CNT2_CNT2_MASK                  0xFF
/** Bit Offset for Sub-Register CNT2.CNT2 */
#define CNT2_CNT2_SHIFT                 0

/** Sub-registers of Register @ref CNT3 */
/** Bit Mask for Sub-Register CNT3.CNT3 */
#define CNT3_CNT3_MASK                  0xFF
/** Bit Offset for Sub-Register CNT3.CNT3 */
#define CNT3_CNT3_SHIFT                 0

/** Sub-registers of Register @ref FBRXS */
/** Bit Mask for Sub-Register FBRXS.FBRXS */
#define FBRXS_FBRXS_MASK                0xFF
/** Bit Offset for Sub-Register FBRXS.FBRXS */
#define FBRXS_FBRXS_SHIFT               0

/** Sub-registers of Register @ref FBRXE */
/** Bit Mask for Sub-Register FBRXE.FBRXE */
#define FBRXE_FBRXE_MASK                0xFF
/** Bit Offset for Sub-Register FBRXE.FBRXE */
#define FBRXE_FBRXE_SHIFT               0

/** Sub-registers of Register @ref FBTXS */
/** Bit Mask for Sub-Register FBTXS.FBTXS */
#define FBTXS_FBTXS_MASK                0xFF
/** Bit Offset for Sub-Register FBTXS.FBTXS */
#define FBTXS_FBTXS_SHIFT               0

/** Sub-registers of Register @ref FBTXE */
/** Bit Mask for Sub-Register FBTXE.FBTXE */
#define FBTXE_FBTXE_MASK                0xFF
/** Bit Offset for Sub-Register FBTXE.FBTXE */
#define FBTXE_FBTXE_SHIFT               0

/** Register group base addresses */

/** Base address for RF09_RFIRQS09 register set */
#define BASE_ADDR_RF09_RFIRQS09         (0x0000)
/** Base address for RF24_RFIRQS24 register set */
#define BASE_ADDR_RF24_RFIRQS24         (0x0001)
/** Base address for BBC0_CORE0_IRQS register set */
#define BASE_ADDR_BBC0_CORE0_IRQS       (0x0002)
/** Base address for BBC1_CORE1_IRQS register set */
#define BASE_ADDR_BBC1_CORE1_IRQS       (0x0003)
/** Base address for RF_RFCOMMON register set */
#define BASE_ADDR_RF_RFCOMMON           (0x0005)
/** Base address for RFT_RFTESTCOMMON register set */
#define BASE_ADDR_RFT_RFTESTCOMMON      (0x0005)
/** Base address for RF09_RF09 register set */
#define BASE_ADDR_RF09_RF09             (0x0100)
/** Base address for RFT09_RFT09 register set */
#define BASE_ADDR_RFT09_RFT09           (0x0100)
/** Base address for RF24_RF24 register set */
#define BASE_ADDR_RF24_RF24             (0x0200)
/** Base address for RFT24_RFT24 register set */
#define BASE_ADDR_RFT24_RFT24           (0x0200)
/** Base address for BBC0_CORE0 register set */
#define BASE_ADDR_BBC0_CORE0            (0x0300)
/** Base address for BBC1_CORE1 register set */
#define BASE_ADDR_BBC1_CORE1            (0x0400)
/** Base address for BBC0_FB0 register set */
#define BASE_ADDR_BBC0_FB0              (0x2000)
/** Base address for BBC1_FB1 register set */
#define BASE_ADDR_BBC1_FB1              (0x3000)

/** Register addresses */

/** Address for register RF09_IRQS */
#define RG_RF09_IRQS                    (0x00)
/** Sub-registers of Register @ref RG_RF09_IRQS */
/** Access parameters for sub-register WAKEUP in register @ref RG_RF09_IRQS */
#define SR_RF09_IRQS_WAKEUP             RG_RF09_IRQS, IRQS_WAKEUP_MASK,	\
	IRQS_WAKEUP_SHIFT
/** Access parameters for sub-register TRXRDY in register @ref RG_RF09_IRQS */
#define SR_RF09_IRQS_TRXRDY             RG_RF09_IRQS, IRQS_TRXRDY_MASK,	\
	IRQS_TRXRDY_SHIFT
/** Access parameters for sub-register EDC in register @ref RG_RF09_IRQS */
#define SR_RF09_IRQS_EDC                RG_RF09_IRQS, IRQS_EDC_MASK, \
	IRQS_EDC_SHIFT
/** Access parameters for sub-register BATLOW in register @ref RG_RF09_IRQS */
#define SR_RF09_IRQS_BATLOW             RG_RF09_IRQS, IRQS_BATLOW_MASK,	\
	IRQS_BATLOW_SHIFT
/** Access parameters for sub-register TRXERR in register @ref RG_RF09_IRQS */
#define SR_RF09_IRQS_TRXERR             RG_RF09_IRQS, IRQS_TRXERR_MASK,	\
	IRQS_TRXERR_SHIFT
/** Access parameters for sub-register IQIFSF in register @ref RG_RF09_IRQS */
#define SR_RF09_IRQS_IQIFSF             RG_RF09_IRQS, IRQS_IQIFSF_MASK,	\
	IRQS_IQIFSF_SHIFT

/** Address for register RF24_IRQS */
#define RG_RF24_IRQS                    (0x01)
/** Sub-registers of Register @ref RG_RF24_IRQS */
/** Access parameters for sub-register WAKEUP in register @ref RG_RF24_IRQS */
#define SR_RF24_IRQS_WAKEUP             RG_RF24_IRQS, IRQS_WAKEUP_MASK,	\
	IRQS_WAKEUP_SHIFT
/** Access parameters for sub-register TRXRDY in register @ref RG_RF24_IRQS */
#define SR_RF24_IRQS_TRXRDY             RG_RF24_IRQS, IRQS_TRXRDY_MASK,	\
	IRQS_TRXRDY_SHIFT
/** Access parameters for sub-register EDC in register @ref RG_RF24_IRQS */
#define SR_RF24_IRQS_EDC                RG_RF24_IRQS, IRQS_EDC_MASK, \
	IRQS_EDC_SHIFT
/** Access parameters for sub-register BATLOW in register @ref RG_RF24_IRQS */
#define SR_RF24_IRQS_BATLOW             RG_RF24_IRQS, IRQS_BATLOW_MASK,	\
	IRQS_BATLOW_SHIFT
/** Access parameters for sub-register TRXERR in register @ref RG_RF24_IRQS */
#define SR_RF24_IRQS_TRXERR             RG_RF24_IRQS, IRQS_TRXERR_MASK,	\
	IRQS_TRXERR_SHIFT
/** Access parameters for sub-register IQIFSF in register @ref RG_RF24_IRQS */
#define SR_RF24_IRQS_IQIFSF             RG_RF24_IRQS, IRQS_IQIFSF_MASK,	\
	IRQS_IQIFSF_SHIFT

/** Address for register BBC0_IRQS */
#define RG_BBC0_IRQS                    (0x02)
/** Sub-registers of Register @ref RG_BBC0_IRQS */
/** Access parameters for sub-register RXFS in register @ref RG_BBC0_IRQS */
#define SR_BBC0_IRQS_RXFS               RG_BBC0_IRQS, IRQS_RXFS_MASK, \
	IRQS_RXFS_SHIFT
/** Access parameters for sub-register RXFE in register @ref RG_BBC0_IRQS */
#define SR_BBC0_IRQS_RXFE               RG_BBC0_IRQS, IRQS_RXFE_MASK, \
	IRQS_RXFE_SHIFT
/** Access parameters for sub-register RXAM in register @ref RG_BBC0_IRQS */
#define SR_BBC0_IRQS_RXAM               RG_BBC0_IRQS, IRQS_RXAM_MASK, \
	IRQS_RXAM_SHIFT
/** Access parameters for sub-register RXEM in register @ref RG_BBC0_IRQS */
#define SR_BBC0_IRQS_RXEM               RG_BBC0_IRQS, IRQS_RXEM_MASK, \
	IRQS_RXEM_SHIFT
/** Access parameters for sub-register TXFE in register @ref RG_BBC0_IRQS */
#define SR_BBC0_IRQS_TXFE               RG_BBC0_IRQS, IRQS_TXFE_MASK, \
	IRQS_TXFE_SHIFT
/** Access parameters for sub-register AGCH in register @ref RG_BBC0_IRQS */
#define SR_BBC0_IRQS_AGCH               RG_BBC0_IRQS, IRQS_AGCH_MASK, \
	IRQS_AGCH_SHIFT
/** Access parameters for sub-register AGCR in register @ref RG_BBC0_IRQS */
#define SR_BBC0_IRQS_AGCR               RG_BBC0_IRQS, IRQS_AGCR_MASK, \
	IRQS_AGCR_SHIFT
/** Access parameters for sub-register FBLI in register @ref RG_BBC0_IRQS */
#define SR_BBC0_IRQS_FBLI               RG_BBC0_IRQS, IRQS_FBLI_MASK, \
	IRQS_FBLI_SHIFT

/** Address for register BBC1_IRQS */
#define RG_BBC1_IRQS                    (0x03)
/** Sub-registers of Register @ref RG_BBC1_IRQS */
/** Access parameters for sub-register RXFS in register @ref RG_BBC1_IRQS */
#define SR_BBC1_IRQS_RXFS               RG_BBC1_IRQS, IRQS_RXFS_MASK, \
	IRQS_RXFS_SHIFT
/** Access parameters for sub-register RXFE in register @ref RG_BBC1_IRQS */
#define SR_BBC1_IRQS_RXFE               RG_BBC1_IRQS, IRQS_RXFE_MASK, \
	IRQS_RXFE_SHIFT
/** Access parameters for sub-register RXAM in register @ref RG_BBC1_IRQS */
#define SR_BBC1_IRQS_RXAM               RG_BBC1_IRQS, IRQS_RXAM_MASK, \
	IRQS_RXAM_SHIFT
/** Access parameters for sub-register RXEM in register @ref RG_BBC1_IRQS */
#define SR_BBC1_IRQS_RXEM               RG_BBC1_IRQS, IRQS_RXEM_MASK, \
	IRQS_RXEM_SHIFT
/** Access parameters for sub-register TXFE in register @ref RG_BBC1_IRQS */
#define SR_BBC1_IRQS_TXFE               RG_BBC1_IRQS, IRQS_TXFE_MASK, \
	IRQS_TXFE_SHIFT
/** Access parameters for sub-register AGCH in register @ref RG_BBC1_IRQS */
#define SR_BBC1_IRQS_AGCH               RG_BBC1_IRQS, IRQS_AGCH_MASK, \
	IRQS_AGCH_SHIFT
/** Access parameters for sub-register AGCR in register @ref RG_BBC1_IRQS */
#define SR_BBC1_IRQS_AGCR               RG_BBC1_IRQS, IRQS_AGCR_MASK, \
	IRQS_AGCR_SHIFT
/** Access parameters for sub-register FBLI in register @ref RG_BBC1_IRQS */
#define SR_BBC1_IRQS_FBLI               RG_BBC1_IRQS, IRQS_FBLI_MASK, \
	IRQS_FBLI_SHIFT

/** Address for register RF_RST */
#define RG_RF_RST                       (0x05)
/** Sub-registers of Register @ref RG_RF_RST */
/** Access parameters for sub-register CMD in register @ref RG_RF_RST */
#define SR_RF_RST_CMD                   RG_RF_RST, RST_CMD_MASK, RST_CMD_SHIFT

/** Address for register RF_CFG */
#define RG_RF_CFG                       (0x06)
/** Sub-registers of Register @ref RG_RF_CFG */
/** Access parameters for sub-register DRV in register @ref RG_RF_CFG */
#define SR_RF_CFG_DRV                   RG_RF_CFG, CFG_DRV_MASK, CFG_DRV_SHIFT
/** Constant DRV2 for sub-register @ref SR_DRV in register RF_CFG */
#define RF_DRV2                         (0x0)
/** Constant DRV4 for sub-register @ref SR_DRV in register RF_CFG */
#define RF_DRV4                         (0x1)
/** Constant DRV6 for sub-register @ref SR_DRV in register RF_CFG */
#define RF_DRV6                         (0x2)
/** Constant DRV8 for sub-register @ref SR_DRV in register RF_CFG */
#define RF_DRV8                         (0x3)
/** Access parameters for sub-register IRQP in register @ref RG_RF_CFG */
#define SR_RF_CFG_IRQP                  RG_RF_CFG, CFG_IRQP_MASK, CFG_IRQP_SHIFT
/** Access parameters for sub-register IRQMM in register @ref RG_RF_CFG */
#define SR_RF_CFG_IRQMM                 RG_RF_CFG, CFG_IRQMM_MASK, \
	CFG_IRQMM_SHIFT

/** Address for register RF_CLKO */
#define RG_RF_CLKO                      (0x07)
/** Sub-registers of Register @ref RG_RF_CLKO */
/** Access parameters for sub-register OS in register @ref RG_RF_CLKO */
#define SR_RF_CLKO_OS                   RG_RF_CLKO, CLKO_OS_MASK, CLKO_OS_SHIFT
/** Access parameters for sub-register DRV in register @ref RG_RF_CLKO */
#define SR_RF_CLKO_DRV                  RG_RF_CLKO, CLKO_DRV_MASK, \
	CLKO_DRV_SHIFT
/** Constant DRVCLKO2 for sub-register @ref SR_DRV in register RF_CLKO */
#define RF_DRVCLKO2                     (0x0)
/** Constant DRVCLKO4 for sub-register @ref SR_DRV in register RF_CLKO */
#define RF_DRVCLKO4                     (0x1)
/** Constant DRVCLKO6 for sub-register @ref SR_DRV in register RF_CLKO */
#define RF_DRVCLKO6                     (0x2)
/** Constant DRVCLKO8 for sub-register @ref SR_DRV in register RF_CLKO */
#define RF_DRVCLKO8                     (0x3)

/** Address for register RF_BMDVC */
#define RG_RF_BMDVC                     (0x08)
/** Sub-registers of Register @ref RG_RF_BMDVC */
/** Access parameters for sub-register BMVTH in register @ref RG_RF_BMDVC */
#define SR_RF_BMDVC_BMVTH               RG_RF_BMDVC, BMDVC_BMVTH_MASK, \
	BMDVC_BMVTH_SHIFT
/** Access parameters for sub-register BMHR in register @ref RG_RF_BMDVC */
#define SR_RF_BMDVC_BMHR                RG_RF_BMDVC, BMDVC_BMHR_MASK, \
	BMDVC_BMHR_SHIFT
/** Access parameters for sub-register BMS in register @ref RG_RF_BMDVC */
#define SR_RF_BMDVC_BMS                 RG_RF_BMDVC, BMDVC_BMS_MASK, \
	BMDVC_BMS_SHIFT

/** Address for register RF_XOC */
#define RG_RF_XOC                       (0x09)
/** Sub-registers of Register @ref RG_RF_XOC */
/** Access parameters for sub-register TRIM in register @ref RG_RF_XOC */
#define SR_RF_XOC_TRIM                  RG_RF_XOC, XOC_TRIM_MASK, XOC_TRIM_SHIFT
/** Access parameters for sub-register FS in register @ref RG_RF_XOC */
#define SR_RF_XOC_FS                    RG_RF_XOC, XOC_FS_MASK, XOC_FS_SHIFT

/** Address for register RF_IQIFC0 */
#define RG_RF_IQIFC0                    (0x0A)
/** Sub-registers of Register @ref RG_RF_IQIFC0 */
/** Access parameters for sub-register EEC in register @ref RG_RF_IQIFC0 */
#define SR_RF_IQIFC0_EEC                RG_RF_IQIFC0, IQIFC0_EEC_MASK, \
	IQIFC0_EEC_SHIFT
/** Access parameters for sub-register CMV1V2 in register @ref RG_RF_IQIFC0 */
#define SR_RF_IQIFC0_CMV1V2             RG_RF_IQIFC0, IQIFC0_CMV1V2_MASK, \
	IQIFC0_CMV1V2_SHIFT
/** Access parameters for sub-register CMV in register @ref RG_RF_IQIFC0 */
#define SR_RF_IQIFC0_CMV                RG_RF_IQIFC0, IQIFC0_CMV_MASK, \
	IQIFC0_CMV_SHIFT
/** Access parameters for sub-register DRV in register @ref RG_RF_IQIFC0 */
#define SR_RF_IQIFC0_DRV                RG_RF_IQIFC0, IQIFC0_DRV_MASK, \
	IQIFC0_DRV_SHIFT
/** Access parameters for sub-register SF in register @ref RG_RF_IQIFC0 */
#define SR_RF_IQIFC0_SF                 RG_RF_IQIFC0, IQIFC0_SF_MASK, \
	IQIFC0_SF_SHIFT
/** Access parameters for sub-register EXTLB in register @ref RG_RF_IQIFC0 */
#define SR_RF_IQIFC0_EXTLB              RG_RF_IQIFC0, IQIFC0_EXTLB_MASK, \
	IQIFC0_EXTLB_SHIFT

/** Address for register RF_IQIFC1 */
#define RG_RF_IQIFC1                    (0x0B)
/** Sub-registers of Register @ref RG_RF_IQIFC1 */
/** Access parameters for sub-register SKEWDRV in register @ref RG_RF_IQIFC1 */
#define SR_RF_IQIFC1_SKEWDRV            RG_RF_IQIFC1, IQIFC1_SKEWDRV_MASK, \
	IQIFC1_SKEWDRV_SHIFT
/** Access parameters for sub-register CHPM in register @ref RG_RF_IQIFC1 */
#define SR_RF_IQIFC1_CHPM               RG_RF_IQIFC1, IQIFC1_CHPM_MASK,	\
	IQIFC1_CHPM_SHIFT
/** Constant MODE_BBRF for sub-register @ref SR_CHPM in register RF_IQIFC1 */
#define RF_MODE_BBRF                    (0x0)
/** Constant MODE_BBRF09 for sub-register @ref SR_CHPM in register RF_IQIFC1 */
#define RF_MODE_BBRF09                  (0x4)
/** Constant MODE_BBRF24 for sub-register @ref SR_CHPM in register RF_IQIFC1 */
#define RF_MODE_BBRF24                  (0x5)
/** Constant MODE_RF for sub-register @ref SR_CHPM in register RF_IQIFC1 */
#define RF_MODE_RF                      (0x1)
/** Access parameters for sub-register FAILSF in register @ref RG_RF_IQIFC1 */
#define SR_RF_IQIFC1_FAILSF             RG_RF_IQIFC1, IQIFC1_FAILSF_MASK, \
	IQIFC1_FAILSF_SHIFT

/** Address for register RF_IQIFC2 */
#define RG_RF_IQIFC2                    (0x0C)
/** Sub-registers of Register @ref RG_RF_IQIFC2 */
/** Access parameters for sub-register SYNC in register @ref RG_RF_IQIFC2 */
#define SR_RF_IQIFC2_SYNC               RG_RF_IQIFC2, IQIFC2_SYNC_MASK,	\
	IQIFC2_SYNC_SHIFT

/** Address for register RF_PN */
#define RG_RF_PN                        (0x0D)
/** Sub-registers of Register @ref RG_RF_PN */
/** Access parameters for sub-register PN in register @ref RG_RF_PN */
#define SR_RF_PN_PN                     RG_RF_PN, PN_PN_MASK, PN_PN_SHIFT

/** Address for register RF_VN */
#define RG_RF_VN                        (0x0E)
/** Sub-registers of Register @ref RG_RF_VN */
/** Access parameters for sub-register VN in register @ref RG_RF_VN */
#define SR_RF_VN_VN                     RG_RF_VN, VN_VN_MASK, VN_VN_SHIFT

/** Address for register RF09_IRQM */
#define RG_RF09_IRQM                    (0x100)
/** Sub-registers of Register @ref RG_RF09_IRQM */
/** Access parameters for sub-register WAKEUP in register @ref RG_RF09_IRQM */
#define SR_RF09_IRQM_WAKEUP             RG_RF09_IRQM, IRQM_WAKEUP_MASK,	\
	IRQM_WAKEUP_SHIFT
/** Access parameters for sub-register TRXRDY in register @ref RG_RF09_IRQM */
#define SR_RF09_IRQM_TRXRDY             RG_RF09_IRQM, IRQM_TRXRDY_MASK,	\
	IRQM_TRXRDY_SHIFT
/** Access parameters for sub-register EDC in register @ref RG_RF09_IRQM */
#define SR_RF09_IRQM_EDC                RG_RF09_IRQM, IRQM_EDC_MASK, \
	IRQM_EDC_SHIFT
/** Access parameters for sub-register BATLOW in register @ref RG_RF09_IRQM */
#define SR_RF09_IRQM_BATLOW             RG_RF09_IRQM, IRQM_BATLOW_MASK,	\
	IRQM_BATLOW_SHIFT
/** Access parameters for sub-register TRXERR in register @ref RG_RF09_IRQM */
#define SR_RF09_IRQM_TRXERR             RG_RF09_IRQM, IRQM_TRXERR_MASK,	\
	IRQM_TRXERR_SHIFT
/** Access parameters for sub-register IQIFSF in register @ref RG_RF09_IRQM */
#define SR_RF09_IRQM_IQIFSF             RG_RF09_IRQM, IRQM_IQIFSF_MASK,	\
	IRQM_IQIFSF_SHIFT

/** Address for register RF09_AUXS */
#define RG_RF09_AUXS                    (0x101)
/** Sub-registers of Register @ref RG_RF09_AUXS */
/** Access parameters for sub-register PAVC in register @ref RG_RF09_AUXS */
#define SR_RF09_AUXS_PAVC               RG_RF09_AUXS, AUXS_PAVC_MASK, \
	AUXS_PAVC_SHIFT
/** Access parameters for sub-register AVS in register @ref RG_RF09_AUXS */
#define SR_RF09_AUXS_AVS                RG_RF09_AUXS, AUXS_AVS_MASK, \
	AUXS_AVS_SHIFT
/** Access parameters for sub-register AVEN in register @ref RG_RF09_AUXS */
#define SR_RF09_AUXS_AVEN               RG_RF09_AUXS, AUXS_AVEN_MASK, \
	AUXS_AVEN_SHIFT
/** Access parameters for sub-register AVEXT in register @ref RG_RF09_AUXS */
#define SR_RF09_AUXS_AVEXT              RG_RF09_AUXS, AUXS_AVEXT_MASK, \
	AUXS_AVEXT_SHIFT
/** Access parameters for sub-register AGCMAP in register @ref RG_RF09_AUXS */
#define SR_RF09_AUXS_AGCMAP             RG_RF09_AUXS, AUXS_AGCMAP_MASK,	\
	AUXS_AGCMAP_SHIFT

/** Access parameters for sub-register EXTLNABYP in register @ref RG_RF09_AUXS
**/
#define SR_RF09_AUXS_EXTLNABYP          RG_RF09_AUXS, AUXS_EXTLNABYP_MASK, \
	AUXS_EXTLNABYP_SHIFT

/** Address for register RF09_STATE */
#define RG_RF09_STATE                   (0x102)
/** Sub-registers of Register @ref RG_RF09_STATE */
/** Access parameters for sub-register STATE in register @ref RG_RF09_STATE */
#define SR_RF09_STATE_STATE             RG_RF09_STATE, STATE_STATE_MASK, \
	STATE_STATE_SHIFT
/** Constant TRANSITION for sub-register @ref SR_STATE in register RF09_STATE */
#define RF_TRANSITION                   (0x6)

/** Address for register RF09_CMD */
#define RG_RF09_CMD                     (0x103)
/** Sub-registers of Register @ref RG_RF09_CMD */
/** Access parameters for sub-register CMD in register @ref RG_RF09_CMD */
#define SR_RF09_CMD_CMD                 RG_RF09_CMD, CMD_CMD_MASK, CMD_CMD_SHIFT

/** Address for register RF09_CS */
#define RG_RF09_CS                      (0x104)
/** Sub-registers of Register @ref RG_RF09_CS */
/** Access parameters for sub-register CS in register @ref RG_RF09_CS */
#define SR_RF09_CS_CS                   RG_RF09_CS, CS_CS_MASK, CS_CS_SHIFT

/** Address for register RF09_CCF0L */
#define RG_RF09_CCF0L                   (0x105)
/** Sub-registers of Register @ref RG_RF09_CCF0L */
/** Access parameters for sub-register CCF0L in register @ref RG_RF09_CCF0L */
#define SR_RF09_CCF0L_CCF0L             RG_RF09_CCF0L, CCF0L_CCF0L_MASK, \
	CCF0L_CCF0L_SHIFT

/** Address for register RF09_CCF0H */
#define RG_RF09_CCF0H                   (0x106)
/** Sub-registers of Register @ref RG_RF09_CCF0H */
/** Access parameters for sub-register CCF0H in register @ref RG_RF09_CCF0H */
#define SR_RF09_CCF0H_CCF0H             RG_RF09_CCF0H, CCF0H_CCF0H_MASK, \
	CCF0H_CCF0H_SHIFT

/** Address for register RF09_CNL */
#define RG_RF09_CNL                     (0x107)
/** Sub-registers of Register @ref RG_RF09_CNL */
/** Access parameters for sub-register CNL in register @ref RG_RF09_CNL */
#define SR_RF09_CNL_CNL                 RG_RF09_CNL, CNL_CNL_MASK, CNL_CNL_SHIFT

/** Address for register RF09_CNM */
#define RG_RF09_CNM                     (0x108)
/** Sub-registers of Register @ref RG_RF09_CNM */
/** Access parameters for sub-register CNH in register @ref RG_RF09_CNM */
#define SR_RF09_CNM_CNH                 RG_RF09_CNM, CNM_CNH_MASK, CNM_CNH_SHIFT
/** Access parameters for sub-register CM in register @ref RG_RF09_CNM */
#define SR_RF09_CNM_CM                  RG_RF09_CNM, CNM_CM_MASK, CNM_CM_SHIFT

/** Address for register RF09_RXBWC */
#define RG_RF09_RXBWC                   (0x109)
/** Sub-registers of Register @ref RG_RF09_RXBWC */
/** Access parameters for sub-register BW in register @ref RG_RF09_RXBWC */
#define SR_RF09_RXBWC_BW                RG_RF09_RXBWC, RXBWC_BW_MASK, \
	RXBWC_BW_SHIFT

/** Constant BW1000KHZ_IF1000KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW1000KHZ_IF1000KHZ          (0x8)

/** Constant BW1250KHZ_IF2000KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW1250KHZ_IF2000KHZ          (0x9)

/** Constant BW1600KHZ_IF2000KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW1600KHZ_IF2000KHZ          (0xA)

/** Constant BW160KHZ_IF250KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW160KHZ_IF250KHZ            (0x0)

/** Constant BW2000KHZ_IF2000KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW2000KHZ_IF2000KHZ          (0xB)

/** Constant BW200KHZ_IF250KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW200KHZ_IF250KHZ            (0x1)

/** Constant BW250KHZ_IF250KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW250KHZ_IF250KHZ            (0x2)

/** Constant BW320KHZ_IF500KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW320KHZ_IF500KHZ            (0x3)

/** Constant BW400KHZ_IF500KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW400KHZ_IF500KHZ            (0x4)

/** Constant BW500KHZ_IF500KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW500KHZ_IF500KHZ            (0x5)

/** Constant BW630KHZ_IF1000KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW630KHZ_IF1000KHZ           (0x6)

/** Constant BW800KHZ_IF1000KHZ for sub-register @ref SR_BW in register
 * RF09_RXBWC */
#define RF_BW800KHZ_IF1000KHZ           (0x7)
/** Access parameters for sub-register IFS in register @ref RG_RF09_RXBWC */
#define SR_RF09_RXBWC_IFS               RG_RF09_RXBWC, RXBWC_IFS_MASK, \
	RXBWC_IFS_SHIFT
/** Access parameters for sub-register IFI in register @ref RG_RF09_RXBWC */
#define SR_RF09_RXBWC_IFI               RG_RF09_RXBWC, RXBWC_IFI_MASK, \
	RXBWC_IFI_SHIFT

/** Address for register RF09_RXDFE */
#define RG_RF09_RXDFE                   (0x10A)
/** Sub-registers of Register @ref RG_RF09_RXDFE */
/** Access parameters for sub-register SR in register @ref RG_RF09_RXDFE */
#define SR_RF09_RXDFE_SR                RG_RF09_RXDFE, RXDFE_SR_MASK, \
	RXDFE_SR_SHIFT
/** Access parameters for sub-register RCUT in register @ref RG_RF09_RXDFE */
#define SR_RF09_RXDFE_RCUT              RG_RF09_RXDFE, RXDFE_RCUT_MASK,	\
	RXDFE_RCUT_SHIFT

/** Address for register RF09_AGCC */
#define RG_RF09_AGCC                    (0x10B)
/** Sub-registers of Register @ref RG_RF09_AGCC */
/** Access parameters for sub-register EN in register @ref RG_RF09_AGCC */
#define SR_RF09_AGCC_EN                 RG_RF09_AGCC, AGCC_EN_MASK, \
	AGCC_EN_SHIFT
/** Access parameters for sub-register FRZC in register @ref RG_RF09_AGCC */
#define SR_RF09_AGCC_FRZC               RG_RF09_AGCC, AGCC_FRZC_MASK, \
	AGCC_FRZC_SHIFT
/** Access parameters for sub-register FRZS in register @ref RG_RF09_AGCC */
#define SR_RF09_AGCC_FRZS               RG_RF09_AGCC, AGCC_FRZS_MASK, \
	AGCC_FRZS_SHIFT
/** Access parameters for sub-register RST in register @ref RG_RF09_AGCC */
#define SR_RF09_AGCC_RST                RG_RF09_AGCC, AGCC_RST_MASK, \
	AGCC_RST_SHIFT
/** Access parameters for sub-register AVGS in register @ref RG_RF09_AGCC */
#define SR_RF09_AGCC_AVGS               RG_RF09_AGCC, AGCC_AVGS_MASK, \
	AGCC_AVGS_SHIFT
/** Access parameters for sub-register AGCI in register @ref RG_RF09_AGCC */
#define SR_RF09_AGCC_AGCI               RG_RF09_AGCC, AGCC_AGCI_MASK, \
	AGCC_AGCI_SHIFT

/** Address for register RF09_AGCS */
#define RG_RF09_AGCS                    (0x10C)
/** Sub-registers of Register @ref RG_RF09_AGCS */
/** Access parameters for sub-register GCW in register @ref RG_RF09_AGCS */
#define SR_RF09_AGCS_GCW                RG_RF09_AGCS, AGCS_GCW_MASK, \
	AGCS_GCW_SHIFT
/** Access parameters for sub-register TGT in register @ref RG_RF09_AGCS */
#define SR_RF09_AGCS_TGT                RG_RF09_AGCS, AGCS_TGT_MASK, \
	AGCS_TGT_SHIFT

/** Address for register RF09_RSSI */
#define RG_RF09_RSSI                    (0x10D)
/** Sub-registers of Register @ref RG_RF09_RSSI */
/** Access parameters for sub-register RSSI in register @ref RG_RF09_RSSI */
#define SR_RF09_RSSI_RSSI               RG_RF09_RSSI, RSSI_RSSI_MASK, \
	RSSI_RSSI_SHIFT

/** Address for register RF09_EDC */
#define RG_RF09_EDC                     (0x10E)
/** Sub-registers of Register @ref RG_RF09_EDC */
/** Access parameters for sub-register EDM in register @ref RG_RF09_EDC */
#define SR_RF09_EDC_EDM                 RG_RF09_EDC, EDC_EDM_MASK, EDC_EDM_SHIFT
/** Constant EDAUTO for sub-register @ref SR_EDM in register RF09_EDC */
#define RF_EDAUTO                       (0x0)
/** Constant EDCONT for sub-register @ref SR_EDM in register RF09_EDC */
#define RF_EDCONT                       (0x2)
/** Constant EDOFF for sub-register @ref SR_EDM in register RF09_EDC */
#define RF_EDOFF                        (0x3)
/** Constant EDSINGLE for sub-register @ref SR_EDM in register RF09_EDC */
#define RF_EDSINGLE                     (0x1)

/** Address for register RF09_EDD */
#define RG_RF09_EDD                     (0x10F)
/** Sub-registers of Register @ref RG_RF09_EDD */
/** Access parameters for sub-register DTB in register @ref RG_RF09_EDD */
#define SR_RF09_EDD_DTB                 RG_RF09_EDD, EDD_DTB_MASK, EDD_DTB_SHIFT
/** Access parameters for sub-register DF in register @ref RG_RF09_EDD */
#define SR_RF09_EDD_DF                  RG_RF09_EDD, EDD_DF_MASK, EDD_DF_SHIFT

/** Address for register RF09_EDV */
#define RG_RF09_EDV                     (0x110)
/** Sub-registers of Register @ref RG_RF09_EDV */
/** Access parameters for sub-register EDV in register @ref RG_RF09_EDV */
#define SR_RF09_EDV_EDV                 RG_RF09_EDV, EDV_EDV_MASK, EDV_EDV_SHIFT

/** Address for register RF09_RNDV */
#define RG_RF09_RNDV                    (0x111)
/** Sub-registers of Register @ref RG_RF09_RNDV */
/** Access parameters for sub-register RNDV in register @ref RG_RF09_RNDV */
#define SR_RF09_RNDV_RNDV               RG_RF09_RNDV, RNDV_RNDV_MASK, \
	RNDV_RNDV_SHIFT

/** Address for register RF09_TXCUTC */
#define RG_RF09_TXCUTC                  (0x112)
/** Sub-registers of Register @ref RG_RF09_TXCUTC */
/** Access parameters for sub-register LPFCUT in register @ref RG_RF09_TXCUTC */
#define SR_RF09_TXCUTC_LPFCUT           RG_RF09_TXCUTC, TXCUTC_LPFCUT_MASK, \
	TXCUTC_LPFCUT_SHIFT

/** Constant FLC1000KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC1000KHZ                   (0xB)

/** Constant FLC100KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC100KHZ                    (0x1)

/** Constant FLC125KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC125KHZ                    (0x2)

/** Constant FLC160KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC160KHZ                    (0x3)

/** Constant FLC200KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC200KHZ                    (0x4)

/** Constant FLC250KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC250KHZ                    (0x5)

/** Constant FLC315KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC315KHZ                    (0x6)

/** Constant FLC400KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC400KHZ                    (0x7)

/** Constant FLC500KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC500KHZ                    (0x8)

/** Constant FLC625KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC625KHZ                    (0x9)

/** Constant FLC800KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC
**/
#define RF_FLC800KHZ                    (0xA)
/** Constant FLC80KHZ for sub-register @ref SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC80KHZ                     (0x0)
/** Access parameters for sub-register PARAMP in register @ref RG_RF09_TXCUTC */
#define SR_RF09_TXCUTC_PARAMP           RG_RF09_TXCUTC, TXCUTC_PARAMP_MASK, \
	TXCUTC_PARAMP_SHIFT

/** Constant PARAMP16U for sub-register @ref SR_PARAMP in register RF09_TXCUTC
**/
#define RF_PARAMP16U                    (0x2)

/** Constant PARAMP32U for sub-register @ref SR_PARAMP in register RF09_TXCUTC
**/
#define RF_PARAMP32U                    (0x3)
/** Constant PARAMP4U for sub-register @ref SR_PARAMP in register RF09_TXCUTC */
#define RF_PARAMP4U                     (0x0)
/** Constant PARAMP8U for sub-register @ref SR_PARAMP in register RF09_TXCUTC */
#define RF_PARAMP8U                     (0x1)

/** Address for register RF09_TXDFE */
#define RG_RF09_TXDFE                   (0x113)
/** Sub-registers of Register @ref RG_RF09_TXDFE */
/** Access parameters for sub-register SR in register @ref RG_RF09_TXDFE */
#define SR_RF09_TXDFE_SR                RG_RF09_TXDFE, TXDFE_SR_MASK, \
	TXDFE_SR_SHIFT
/** Access parameters for sub-register DM in register @ref RG_RF09_TXDFE */
#define SR_RF09_TXDFE_DM                RG_RF09_TXDFE, TXDFE_DM_MASK, \
	TXDFE_DM_SHIFT
/** Access parameters for sub-register RCUT in register @ref RG_RF09_TXDFE */
#define SR_RF09_TXDFE_RCUT              RG_RF09_TXDFE, TXDFE_RCUT_MASK,	\
	TXDFE_RCUT_SHIFT

/** Address for register RF09_PAC */
#define RG_RF09_PAC                     (0x114)
/** Sub-registers of Register @ref RG_RF09_PAC */
/** Access parameters for sub-register TXPWR in register @ref RG_RF09_PAC */
#define SR_RF09_PAC_TXPWR               RG_RF09_PAC, PAC_TXPWR_MASK, \
	PAC_TXPWR_SHIFT
/** Access parameters for sub-register PACUR in register @ref RG_RF09_PAC */
#define SR_RF09_PAC_PACUR               RG_RF09_PAC, PAC_PACUR_MASK, \
	PAC_PACUR_SHIFT

/** Address for register RF09_PADFE */
#define RG_RF09_PADFE                   (0x116)
/** Sub-registers of Register @ref RG_RF09_PADFE */
/** Access parameters for sub-register PADFE in register @ref RG_RF09_PADFE */
#define SR_RF09_PADFE_PADFE             RG_RF09_PADFE, PADFE_PADFE_MASK, \
	PADFE_PADFE_SHIFT
/** Constant FEMODE0 for sub-register @ref SR_PADFE in register RF09_PADFE */
#define RF_FEMODE0                      (0x0)
/** Constant FEMODE1 for sub-register @ref SR_PADFE in register RF09_PADFE */
#define RF_FEMODE1                      (0x1)
/** Constant FEMODE2 for sub-register @ref SR_PADFE in register RF09_PADFE */
#define RF_FEMODE2                      (0x2)
/** Constant FEMODE3 for sub-register @ref SR_PADFE in register RF09_PADFE */
#define RF_FEMODE3                      (0x3)

/** Address for register RF09_PLL */
#define RG_RF09_PLL                     (0x121)
/** Sub-registers of Register @ref RG_RF09_PLL */
/** Access parameters for sub-register LS in register @ref RG_RF09_PLL */
#define SR_RF09_PLL_LS                  RG_RF09_PLL, PLL_LS_MASK, PLL_LS_SHIFT
/** Access parameters for sub-register LBW in register @ref RG_RF09_PLL */
#define SR_RF09_PLL_LBW                 RG_RF09_PLL, PLL_LBW_MASK, PLL_LBW_SHIFT

/** Address for register RF09_PLLCF */
#define RG_RF09_PLLCF                   (0x122)
/** Sub-registers of Register @ref RG_RF09_PLLCF */
/** Access parameters for sub-register CF in register @ref RG_RF09_PLLCF */
#define SR_RF09_PLLCF_CF                RG_RF09_PLLCF, PLLCF_CF_MASK, \
	PLLCF_CF_SHIFT

/** Address for register RF09_TXCI */
#define RG_RF09_TXCI                    (0x125)
/** Sub-registers of Register @ref RG_RF09_TXCI */
/** Access parameters for sub-register DCOI in register @ref RG_RF09_TXCI */
#define SR_RF09_TXCI_DCOI               RG_RF09_TXCI, TXCI_DCOI_MASK, \
	TXCI_DCOI_SHIFT

/** Address for register RF09_TXCQ */
#define RG_RF09_TXCQ                    (0x126)
/** Sub-registers of Register @ref RG_RF09_TXCQ */
/** Access parameters for sub-register DCOQ in register @ref RG_RF09_TXCQ */
#define SR_RF09_TXCQ_DCOQ               RG_RF09_TXCQ, TXCQ_DCOQ_MASK, \
	TXCQ_DCOQ_SHIFT

/** Address for register RF09_TXDACI */
#define RG_RF09_TXDACI                  (0x127)
/** Sub-registers of Register @ref RG_RF09_TXDACI */

/** Access parameters for sub-register TXDACID in register @ref RG_RF09_TXDACI
**/
#define SR_RF09_TXDACI_TXDACID          RG_RF09_TXDACI, TXDACI_TXDACID_MASK, \
	TXDACI_TXDACID_SHIFT

/** Access parameters for sub-register ENTXDACID in register @ref RG_RF09_TXDACI
**/
#define SR_RF09_TXDACI_ENTXDACID        RG_RF09_TXDACI, TXDACI_ENTXDACID_MASK, \
	TXDACI_ENTXDACID_SHIFT

/** Address for register RF09_TXDACQ */
#define RG_RF09_TXDACQ                  (0x128)
/** Sub-registers of Register @ref RG_RF09_TXDACQ */

/** Access parameters for sub-register TXDACQD in register @ref RG_RF09_TXDACQ
**/
#define SR_RF09_TXDACQ_TXDACQD          RG_RF09_TXDACQ, TXDACQ_TXDACQD_MASK, \
	TXDACQ_TXDACQD_SHIFT

/** Access parameters for sub-register ENTXDACQD in register @ref RG_RF09_TXDACQ
**/
#define SR_RF09_TXDACQ_ENTXDACQD        RG_RF09_TXDACQ, TXDACQ_ENTXDACQD_MASK, \
	TXDACQ_ENTXDACQD_SHIFT

/** Address for register RF24_IRQM */
#define RG_RF24_IRQM                    (0x200)
/** Sub-registers of Register @ref RG_RF24_IRQM */
/** Access parameters for sub-register WAKEUP in register @ref RG_RF24_IRQM */
#define SR_RF24_IRQM_WAKEUP             RG_RF24_IRQM, IRQM_WAKEUP_MASK,	\
	IRQM_WAKEUP_SHIFT
/** Access parameters for sub-register TRXRDY in register @ref RG_RF24_IRQM */
#define SR_RF24_IRQM_TRXRDY             RG_RF24_IRQM, IRQM_TRXRDY_MASK,	\
	IRQM_TRXRDY_SHIFT
/** Access parameters for sub-register EDC in register @ref RG_RF24_IRQM */
#define SR_RF24_IRQM_EDC                RG_RF24_IRQM, IRQM_EDC_MASK, \
	IRQM_EDC_SHIFT
/** Access parameters for sub-register BATLOW in register @ref RG_RF24_IRQM */
#define SR_RF24_IRQM_BATLOW             RG_RF24_IRQM, IRQM_BATLOW_MASK,	\
	IRQM_BATLOW_SHIFT
/** Access parameters for sub-register TRXERR in register @ref RG_RF24_IRQM */
#define SR_RF24_IRQM_TRXERR             RG_RF24_IRQM, IRQM_TRXERR_MASK,	\
	IRQM_TRXERR_SHIFT
/** Access parameters for sub-register IQIFSF in register @ref RG_RF24_IRQM */
#define SR_RF24_IRQM_IQIFSF             RG_RF24_IRQM, IRQM_IQIFSF_MASK,	\
	IRQM_IQIFSF_SHIFT

/** Address for register BBC0_FBRXS */
#define RG_BBC0_FBRXS                   (0x2000)
/** Sub-registers of Register @ref RG_BBC0_FBRXS */
/** Access parameters for sub-register FBRXS in register @ref RG_BBC0_FBRXS */
#define SR_BBC0_FBRXS_FBRXS             RG_BBC0_FBRXS, FBRXS_FBRXS_MASK, \
	FBRXS_FBRXS_SHIFT

/** Address for register RF24_AUXS */
#define RG_RF24_AUXS                    (0x201)
/** Sub-registers of Register @ref RG_RF24_AUXS */
/** Access parameters for sub-register PAVC in register @ref RG_RF24_AUXS */
#define SR_RF24_AUXS_PAVC               RG_RF24_AUXS, AUXS_PAVC_MASK, \
	AUXS_PAVC_SHIFT
/** Access parameters for sub-register AVS in register @ref RG_RF24_AUXS */
#define SR_RF24_AUXS_AVS                RG_RF24_AUXS, AUXS_AVS_MASK, \
	AUXS_AVS_SHIFT
/** Access parameters for sub-register AVEN in register @ref RG_RF24_AUXS */
#define SR_RF24_AUXS_AVEN               RG_RF24_AUXS, AUXS_AVEN_MASK, \
	AUXS_AVEN_SHIFT
/** Access parameters for sub-register AVEXT in register @ref RG_RF24_AUXS */
#define SR_RF24_AUXS_AVEXT              RG_RF24_AUXS, AUXS_AVEXT_MASK, \
	AUXS_AVEXT_SHIFT
/** Access parameters for sub-register AGCMAP in register @ref RG_RF24_AUXS */
#define SR_RF24_AUXS_AGCMAP             RG_RF24_AUXS, AUXS_AGCMAP_MASK,	\
	AUXS_AGCMAP_SHIFT

/** Access parameters for sub-register EXTLNABYP in register @ref RG_RF24_AUXS
**/
#define SR_RF24_AUXS_EXTLNABYP          RG_RF24_AUXS, AUXS_EXTLNABYP_MASK, \
	AUXS_EXTLNABYP_SHIFT

/** Address for register RF24_STATE */
#define RG_RF24_STATE                   (0x202)
/** Sub-registers of Register @ref RG_RF24_STATE */
/** Access parameters for sub-register STATE in register @ref RG_RF24_STATE */
#define SR_RF24_STATE_STATE             RG_RF24_STATE, STATE_STATE_MASK, \
	STATE_STATE_SHIFT

/** Address for register RF24_CMD */
#define RG_RF24_CMD                     (0x203)
/** Sub-registers of Register @ref RG_RF24_CMD */
/** Access parameters for sub-register CMD in register @ref RG_RF24_CMD */
#define SR_RF24_CMD_CMD                 RG_RF24_CMD, CMD_CMD_MASK, CMD_CMD_SHIFT

/** Address for register RF24_CS */
#define RG_RF24_CS                      (0x204)
/** Sub-registers of Register @ref RG_RF24_CS */
/** Access parameters for sub-register CS in register @ref RG_RF24_CS */
#define SR_RF24_CS_CS                   RG_RF24_CS, CS_CS_MASK, CS_CS_SHIFT

/** Address for register RF24_CCF0L */
#define RG_RF24_CCF0L                   (0x205)
/** Sub-registers of Register @ref RG_RF24_CCF0L */
/** Access parameters for sub-register CCF0L in register @ref RG_RF24_CCF0L */
#define SR_RF24_CCF0L_CCF0L             RG_RF24_CCF0L, CCF0L_CCF0L_MASK, \
	CCF0L_CCF0L_SHIFT

/** Address for register RF24_CCF0H */
#define RG_RF24_CCF0H                   (0x206)
/** Sub-registers of Register @ref RG_RF24_CCF0H */
/** Access parameters for sub-register CCF0H in register @ref RG_RF24_CCF0H */
#define SR_RF24_CCF0H_CCF0H             RG_RF24_CCF0H, CCF0H_CCF0H_MASK, \
	CCF0H_CCF0H_SHIFT

/** Address for register RF24_CNL */
#define RG_RF24_CNL                     (0x207)
/** Sub-registers of Register @ref RG_RF24_CNL */
/** Access parameters for sub-register CNL in register @ref RG_RF24_CNL */
#define SR_RF24_CNL_CNL                 RG_RF24_CNL, CNL_CNL_MASK, CNL_CNL_SHIFT

/** Address for register RF24_CNM */
#define RG_RF24_CNM                     (0x208)
/** Sub-registers of Register @ref RG_RF24_CNM */
/** Access parameters for sub-register CNH in register @ref RG_RF24_CNM */
#define SR_RF24_CNM_CNH                 RG_RF24_CNM, CNM_CNH_MASK, CNM_CNH_SHIFT
/** Access parameters for sub-register CM in register @ref RG_RF24_CNM */
#define SR_RF24_CNM_CM                  RG_RF24_CNM, CNM_CM_MASK, CNM_CM_SHIFT

/** Address for register RF24_RXBWC */
#define RG_RF24_RXBWC                   (0x209)
/** Sub-registers of Register @ref RG_RF24_RXBWC */
/** Access parameters for sub-register BW in register @ref RG_RF24_RXBWC */
#define SR_RF24_RXBWC_BW                RG_RF24_RXBWC, RXBWC_BW_MASK, \
	RXBWC_BW_SHIFT
/** Access parameters for sub-register IFS in register @ref RG_RF24_RXBWC */
#define SR_RF24_RXBWC_IFS               RG_RF24_RXBWC, RXBWC_IFS_MASK, \
	RXBWC_IFS_SHIFT
/** Access parameters for sub-register IFI in register @ref RG_RF24_RXBWC */
#define SR_RF24_RXBWC_IFI               RG_RF24_RXBWC, RXBWC_IFI_MASK, \
	RXBWC_IFI_SHIFT

/** Address for register RF24_RXDFE */
#define RG_RF24_RXDFE                   (0x20A)
/** Sub-registers of Register @ref RG_RF24_RXDFE */
/** Access parameters for sub-register SR in register @ref RG_RF24_RXDFE */
#define SR_RF24_RXDFE_SR                RG_RF24_RXDFE, RXDFE_SR_MASK, \
	RXDFE_SR_SHIFT
/** Access parameters for sub-register RCUT in register @ref RG_RF24_RXDFE */
#define SR_RF24_RXDFE_RCUT              RG_RF24_RXDFE, RXDFE_RCUT_MASK,	\
	RXDFE_RCUT_SHIFT

/** Address for register RF24_AGCC */
#define RG_RF24_AGCC                    (0x20B)
/** Sub-registers of Register @ref RG_RF24_AGCC */
/** Access parameters for sub-register EN in register @ref RG_RF24_AGCC */
#define SR_RF24_AGCC_EN                 RG_RF24_AGCC, AGCC_EN_MASK, \
	AGCC_EN_SHIFT
/** Access parameters for sub-register FRZC in register @ref RG_RF24_AGCC */
#define SR_RF24_AGCC_FRZC               RG_RF24_AGCC, AGCC_FRZC_MASK, \
	AGCC_FRZC_SHIFT
/** Access parameters for sub-register FRZS in register @ref RG_RF24_AGCC */
#define SR_RF24_AGCC_FRZS               RG_RF24_AGCC, AGCC_FRZS_MASK, \
	AGCC_FRZS_SHIFT
/** Access parameters for sub-register RST in register @ref RG_RF24_AGCC */
#define SR_RF24_AGCC_RST                RG_RF24_AGCC, AGCC_RST_MASK, \
	AGCC_RST_SHIFT
/** Access parameters for sub-register AVGS in register @ref RG_RF24_AGCC */
#define SR_RF24_AGCC_AVGS               RG_RF24_AGCC, AGCC_AVGS_MASK, \
	AGCC_AVGS_SHIFT
/** Access parameters for sub-register AGCI in register @ref RG_RF24_AGCC */
#define SR_RF24_AGCC_AGCI               RG_RF24_AGCC, AGCC_AGCI_MASK, \
	AGCC_AGCI_SHIFT

/** Address for register RF24_AGCS */
#define RG_RF24_AGCS                    (0x20C)
/** Sub-registers of Register @ref RG_RF24_AGCS */
/** Access parameters for sub-register GCW in register @ref RG_RF24_AGCS */
#define SR_RF24_AGCS_GCW                RG_RF24_AGCS, AGCS_GCW_MASK, \
	AGCS_GCW_SHIFT
/** Access parameters for sub-register TGT in register @ref RG_RF24_AGCS */
#define SR_RF24_AGCS_TGT                RG_RF24_AGCS, AGCS_TGT_MASK, \
	AGCS_TGT_SHIFT

/** Address for register RF24_RSSI */
#define RG_RF24_RSSI                    (0x20D)
/** Sub-registers of Register @ref RG_RF24_RSSI */
/** Access parameters for sub-register RSSI in register @ref RG_RF24_RSSI */
#define SR_RF24_RSSI_RSSI               RG_RF24_RSSI, RSSI_RSSI_MASK, \
	RSSI_RSSI_SHIFT

/** Address for register RF24_EDC */
#define RG_RF24_EDC                     (0x20E)
/** Sub-registers of Register @ref RG_RF24_EDC */
/** Access parameters for sub-register EDM in register @ref RG_RF24_EDC */
#define SR_RF24_EDC_EDM                 RG_RF24_EDC, EDC_EDM_MASK, EDC_EDM_SHIFT

/** Address for register RF24_EDD */
#define RG_RF24_EDD                     (0x20F)
/** Sub-registers of Register @ref RG_RF24_EDD */
/** Access parameters for sub-register DTB in register @ref RG_RF24_EDD */
#define SR_RF24_EDD_DTB                 RG_RF24_EDD, EDD_DTB_MASK, EDD_DTB_SHIFT
/** Access parameters for sub-register DF in register @ref RG_RF24_EDD */
#define SR_RF24_EDD_DF                  RG_RF24_EDD, EDD_DF_MASK, EDD_DF_SHIFT

/** Address for register RF24_EDV */
#define RG_RF24_EDV                     (0x210)
/** Sub-registers of Register @ref RG_RF24_EDV */
/** Access parameters for sub-register EDV in register @ref RG_RF24_EDV */
#define SR_RF24_EDV_EDV                 RG_RF24_EDV, EDV_EDV_MASK, EDV_EDV_SHIFT

/** Address for register RF24_RNDV */
#define RG_RF24_RNDV                    (0x211)
/** Sub-registers of Register @ref RG_RF24_RNDV */
/** Access parameters for sub-register RNDV in register @ref RG_RF24_RNDV */
#define SR_RF24_RNDV_RNDV               RG_RF24_RNDV, RNDV_RNDV_MASK, \
	RNDV_RNDV_SHIFT

/** Address for register RF24_TXCUTC */
#define RG_RF24_TXCUTC                  (0x212)
/** Sub-registers of Register @ref RG_RF24_TXCUTC */
/** Access parameters for sub-register LPFCUT in register @ref RG_RF24_TXCUTC */
#define SR_RF24_TXCUTC_LPFCUT           RG_RF24_TXCUTC, TXCUTC_LPFCUT_MASK, \
	TXCUTC_LPFCUT_SHIFT
/** Access parameters for sub-register PARAMP in register @ref RG_RF24_TXCUTC */
#define SR_RF24_TXCUTC_PARAMP           RG_RF24_TXCUTC, TXCUTC_PARAMP_MASK, \
	TXCUTC_PARAMP_SHIFT

/** Address for register RF24_TXDFE */
#define RG_RF24_TXDFE                   (0x213)
/** Sub-registers of Register @ref RG_RF24_TXDFE */
/** Access parameters for sub-register SR in register @ref RG_RF24_TXDFE */
#define SR_RF24_TXDFE_SR                RG_RF24_TXDFE, TXDFE_SR_MASK, \
	TXDFE_SR_SHIFT
/** Access parameters for sub-register DM in register @ref RG_RF24_TXDFE */
#define SR_RF24_TXDFE_DM                RG_RF24_TXDFE, TXDFE_DM_MASK, \
	TXDFE_DM_SHIFT
/** Access parameters for sub-register RCUT in register @ref RG_RF24_TXDFE */
#define SR_RF24_TXDFE_RCUT              RG_RF24_TXDFE, TXDFE_RCUT_MASK,	\
	TXDFE_RCUT_SHIFT

/** Address for register RF24_PAC */
#define RG_RF24_PAC                     (0x214)
/** Sub-registers of Register @ref RG_RF24_PAC */
/** Access parameters for sub-register TXPWR in register @ref RG_RF24_PAC */
#define SR_RF24_PAC_TXPWR               RG_RF24_PAC, PAC_TXPWR_MASK, \
	PAC_TXPWR_SHIFT
/** Access parameters for sub-register PACUR in register @ref RG_RF24_PAC */
#define SR_RF24_PAC_PACUR               RG_RF24_PAC, PAC_PACUR_MASK, \
	PAC_PACUR_SHIFT

/** Address for register RF24_PADFE */
#define RG_RF24_PADFE                   (0x216)
/** Sub-registers of Register @ref RG_RF24_PADFE */
/** Access parameters for sub-register PADFE in register @ref RG_RF24_PADFE */
#define SR_RF24_PADFE_PADFE             RG_RF24_PADFE, PADFE_PADFE_MASK, \
	PADFE_PADFE_SHIFT

/** Address for register RF24_PLL */
#define RG_RF24_PLL                     (0x221)
/** Sub-registers of Register @ref RG_RF24_PLL */
/** Access parameters for sub-register LS in register @ref RG_RF24_PLL */
#define SR_RF24_PLL_LS                  RG_RF24_PLL, PLL_LS_MASK, PLL_LS_SHIFT
/** Access parameters for sub-register LBW in register @ref RG_RF24_PLL */
#define SR_RF24_PLL_LBW                 RG_RF24_PLL, PLL_LBW_MASK, PLL_LBW_SHIFT

/** Address for register RF24_PLLCF */
#define RG_RF24_PLLCF                   (0x222)
/** Sub-registers of Register @ref RG_RF24_PLLCF */
/** Access parameters for sub-register CF in register @ref RG_RF24_PLLCF */
#define SR_RF24_PLLCF_CF                RG_RF24_PLLCF, PLLCF_CF_MASK, \
	PLLCF_CF_SHIFT

/** Address for register RF24_TXCI */
#define RG_RF24_TXCI                    (0x225)
/** Sub-registers of Register @ref RG_RF24_TXCI */
/** Access parameters for sub-register DCOI in register @ref RG_RF24_TXCI */
#define SR_RF24_TXCI_DCOI               RG_RF24_TXCI, TXCI_DCOI_MASK, \
	TXCI_DCOI_SHIFT

/** Address for register RF24_TXCQ */
#define RG_RF24_TXCQ                    (0x226)
/** Sub-registers of Register @ref RG_RF24_TXCQ */
/** Access parameters for sub-register DCOQ in register @ref RG_RF24_TXCQ */
#define SR_RF24_TXCQ_DCOQ               RG_RF24_TXCQ, TXCQ_DCOQ_MASK, \
	TXCQ_DCOQ_SHIFT

/** Address for register RF24_TXDACI */
#define RG_RF24_TXDACI                  (0x227)
/** Sub-registers of Register @ref RG_RF24_TXDACI */

/** Access parameters for sub-register TXDACID in register @ref RG_RF24_TXDACI
**/
#define SR_RF24_TXDACI_TXDACID          RG_RF24_TXDACI, TXDACI_TXDACID_MASK, \
	TXDACI_TXDACID_SHIFT

/** Access parameters for sub-register ENTXDACID in register @ref RG_RF24_TXDACI
**/
#define SR_RF24_TXDACI_ENTXDACID        RG_RF24_TXDACI, TXDACI_ENTXDACID_MASK, \
	TXDACI_ENTXDACID_SHIFT

/** Address for register RF24_TXDACQ */
#define RG_RF24_TXDACQ                  (0x228)
/** Sub-registers of Register @ref RG_RF24_TXDACQ */

/** Access parameters for sub-register TXDACQD in register @ref RG_RF24_TXDACQ
**/
#define SR_RF24_TXDACQ_TXDACQD          RG_RF24_TXDACQ, TXDACQ_TXDACQD_MASK, \
	TXDACQ_TXDACQD_SHIFT

/** Access parameters for sub-register ENTXDACQD in register @ref RG_RF24_TXDACQ
**/
#define SR_RF24_TXDACQ_ENTXDACQD        RG_RF24_TXDACQ, TXDACQ_ENTXDACQD_MASK, \
	TXDACQ_ENTXDACQD_SHIFT

/** Address for register BBC0_FBRXE */
#define RG_BBC0_FBRXE                   (0x27FE)
/** Sub-registers of Register @ref RG_BBC0_FBRXE */
/** Access parameters for sub-register FBRXE in register @ref RG_BBC0_FBRXE */
#define SR_BBC0_FBRXE_FBRXE             RG_BBC0_FBRXE, FBRXE_FBRXE_MASK, \
	FBRXE_FBRXE_SHIFT

/** Address for register BBC0_FBTXS */
#define RG_BBC0_FBTXS                   (0x2800)
/** Sub-registers of Register @ref RG_BBC0_FBTXS */
/** Access parameters for sub-register FBTXS in register @ref RG_BBC0_FBTXS */
#define SR_BBC0_FBTXS_FBTXS             RG_BBC0_FBTXS, FBTXS_FBTXS_MASK, \
	FBTXS_FBTXS_SHIFT

/** Address for register BBC0_FBTXE */
#define RG_BBC0_FBTXE                   (0x2FFE)
/** Sub-registers of Register @ref RG_BBC0_FBTXE */
/** Access parameters for sub-register FBTXE in register @ref RG_BBC0_FBTXE */
#define SR_BBC0_FBTXE_FBTXE             RG_BBC0_FBTXE, FBTXE_FBTXE_MASK, \
	FBTXE_FBTXE_SHIFT

/** Address for register BBC0_IRQM */
#define RG_BBC0_IRQM                    (0x300)
/** Sub-registers of Register @ref RG_BBC0_IRQM */
/** Access parameters for sub-register RXFS in register @ref RG_BBC0_IRQM */
#define SR_BBC0_IRQM_RXFS               RG_BBC0_IRQM, IRQM_RXFS_MASK, \
	IRQM_RXFS_SHIFT
/** Access parameters for sub-register RXFE in register @ref RG_BBC0_IRQM */
#define SR_BBC0_IRQM_RXFE               RG_BBC0_IRQM, IRQM_RXFE_MASK, \
	IRQM_RXFE_SHIFT
/** Access parameters for sub-register RXAM in register @ref RG_BBC0_IRQM */
#define SR_BBC0_IRQM_RXAM               RG_BBC0_IRQM, IRQM_RXAM_MASK, \
	IRQM_RXAM_SHIFT
/** Access parameters for sub-register RXEM in register @ref RG_BBC0_IRQM */
#define SR_BBC0_IRQM_RXEM               RG_BBC0_IRQM, IRQM_RXEM_MASK, \
	IRQM_RXEM_SHIFT
/** Access parameters for sub-register TXFE in register @ref RG_BBC0_IRQM */
#define SR_BBC0_IRQM_TXFE               RG_BBC0_IRQM, IRQM_TXFE_MASK, \
	IRQM_TXFE_SHIFT
/** Access parameters for sub-register AGCH in register @ref RG_BBC0_IRQM */
#define SR_BBC0_IRQM_AGCH               RG_BBC0_IRQM, IRQM_AGCH_MASK, \
	IRQM_AGCH_SHIFT
/** Access parameters for sub-register AGCR in register @ref RG_BBC0_IRQM */
#define SR_BBC0_IRQM_AGCR               RG_BBC0_IRQM, IRQM_AGCR_MASK, \
	IRQM_AGCR_SHIFT
/** Access parameters for sub-register FBLI in register @ref RG_BBC0_IRQM */
#define SR_BBC0_IRQM_FBLI               RG_BBC0_IRQM, IRQM_FBLI_MASK, \
	IRQM_FBLI_SHIFT

/** Address for register BBC1_FBRXS */
#define RG_BBC1_FBRXS                   (0x3000)
/** Sub-registers of Register @ref RG_BBC1_FBRXS */
/** Access parameters for sub-register FBRXS in register @ref RG_BBC1_FBRXS */
#define SR_BBC1_FBRXS_FBRXS             RG_BBC1_FBRXS, FBRXS_FBRXS_MASK, \
	FBRXS_FBRXS_SHIFT

/** Address for register BBC0_PC */
#define RG_BBC0_PC                      (0x301)
/** Sub-registers of Register @ref RG_BBC0_PC */
/** Access parameters for sub-register PT in register @ref RG_BBC0_PC */
#define SR_BBC0_PC_PT                   RG_BBC0_PC, PC_PT_MASK, PC_PT_SHIFT
/** Constant MRFSK for sub-register @ref SR_PT in register BBC0_PC */
#define BB_MRFSK                        (0x1)
/** Constant MROFDM for sub-register @ref SR_PT in register BBC0_PC */
#define BB_MROFDM                       (0x2)
/** Constant MROQPSK for sub-register @ref SR_PT in register BBC0_PC */
#define BB_MROQPSK                      (0x3)
/** Constant PHYOFF for sub-register @ref SR_PT in register BBC0_PC */
#define BB_PHYOFF                       (0x0)
/** Access parameters for sub-register BBEN in register @ref RG_BBC0_PC */
#define SR_BBC0_PC_BBEN                 RG_BBC0_PC, PC_BBEN_MASK, PC_BBEN_SHIFT
/** Access parameters for sub-register FCST in register @ref RG_BBC0_PC */
#define SR_BBC0_PC_FCST                 RG_BBC0_PC, PC_FCST_MASK, PC_FCST_SHIFT
/** Access parameters for sub-register TXAFCS in register @ref RG_BBC0_PC */
#define SR_BBC0_PC_TXAFCS               RG_BBC0_PC, PC_TXAFCS_MASK, \
	PC_TXAFCS_SHIFT
/** Access parameters for sub-register FCSOK in register @ref RG_BBC0_PC */
#define SR_BBC0_PC_FCSOK                RG_BBC0_PC, PC_FCSOK_MASK, \
	PC_FCSOK_SHIFT
/** Access parameters for sub-register FCSFE in register @ref RG_BBC0_PC */
#define SR_BBC0_PC_FCSFE                RG_BBC0_PC, PC_FCSFE_MASK, \
	PC_FCSFE_SHIFT
/** Access parameters for sub-register CTX in register @ref RG_BBC0_PC */
#define SR_BBC0_PC_CTX                  RG_BBC0_PC, PC_CTX_MASK, PC_CTX_SHIFT

/** Address for register BBC0_PS */
#define RG_BBC0_PS                      (0x302)
/** Sub-registers of Register @ref RG_BBC0_PS */
/** Access parameters for sub-register TXUR in register @ref RG_BBC0_PS */
#define SR_BBC0_PS_TXUR                 RG_BBC0_PS, PS_TXUR_MASK, PS_TXUR_SHIFT

/** Address for register BBC0_RXFLL */
#define RG_BBC0_RXFLL                   (0x304)
/** Sub-registers of Register @ref RG_BBC0_RXFLL */
/** Access parameters for sub-register RXFLL in register @ref RG_BBC0_RXFLL */
#define SR_BBC0_RXFLL_RXFLL             RG_BBC0_RXFLL, RXFLL_RXFLL_MASK, \
	RXFLL_RXFLL_SHIFT

/** Address for register BBC0_RXFLH */
#define RG_BBC0_RXFLH                   (0x305)
/** Sub-registers of Register @ref RG_BBC0_RXFLH */
/** Access parameters for sub-register RXFLH in register @ref RG_BBC0_RXFLH */
#define SR_BBC0_RXFLH_RXFLH             RG_BBC0_RXFLH, RXFLH_RXFLH_MASK, \
	RXFLH_RXFLH_SHIFT

/** Address for register BBC0_TXFLL */
#define RG_BBC0_TXFLL                   (0x306)
/** Sub-registers of Register @ref RG_BBC0_TXFLL */
/** Access parameters for sub-register TXFLL in register @ref RG_BBC0_TXFLL */
#define SR_BBC0_TXFLL_TXFLL             RG_BBC0_TXFLL, TXFLL_TXFLL_MASK, \
	TXFLL_TXFLL_SHIFT

/** Address for register BBC0_TXFLH */
#define RG_BBC0_TXFLH                   (0x307)
/** Sub-registers of Register @ref RG_BBC0_TXFLH */
/** Access parameters for sub-register TXFLH in register @ref RG_BBC0_TXFLH */
#define SR_BBC0_TXFLH_TXFLH             RG_BBC0_TXFLH, TXFLH_TXFLH_MASK, \
	TXFLH_TXFLH_SHIFT

/** Address for register BBC0_FBLL */
#define RG_BBC0_FBLL                    (0x308)
/** Sub-registers of Register @ref RG_BBC0_FBLL */
/** Access parameters for sub-register FBLL in register @ref RG_BBC0_FBLL */
#define SR_BBC0_FBLL_FBLL               RG_BBC0_FBLL, FBLL_FBLL_MASK, \
	FBLL_FBLL_SHIFT

/** Address for register BBC0_FBLH */
#define RG_BBC0_FBLH                    (0x309)
/** Sub-registers of Register @ref RG_BBC0_FBLH */
/** Access parameters for sub-register FBLH in register @ref RG_BBC0_FBLH */
#define SR_BBC0_FBLH_FBLH               RG_BBC0_FBLH, FBLH_FBLH_MASK, \
	FBLH_FBLH_SHIFT

/** Address for register BBC0_FBLIL */
#define RG_BBC0_FBLIL                   (0x30A)
/** Sub-registers of Register @ref RG_BBC0_FBLIL */
/** Access parameters for sub-register FBLIL in register @ref RG_BBC0_FBLIL */
#define SR_BBC0_FBLIL_FBLIL             RG_BBC0_FBLIL, FBLIL_FBLIL_MASK, \
	FBLIL_FBLIL_SHIFT

/** Address for register BBC0_FBLIH */
#define RG_BBC0_FBLIH                   (0x30B)
/** Sub-registers of Register @ref RG_BBC0_FBLIH */
/** Access parameters for sub-register FBLIH in register @ref RG_BBC0_FBLIH */
#define SR_BBC0_FBLIH_FBLIH             RG_BBC0_FBLIH, FBLIH_FBLIH_MASK, \
	FBLIH_FBLIH_SHIFT

/** Address for register BBC0_OFDMPHRTX */
#define RG_BBC0_OFDMPHRTX               (0x30C)
/** Sub-registers of Register @ref RG_BBC0_OFDMPHRTX */
/** Access parameters for sub-register MCS in register @ref RG_BBC0_OFDMPHRTX */
#define SR_BBC0_OFDMPHRTX_MCS           RG_BBC0_OFDMPHRTX, OFDMPHRTX_MCS_MASK, \
	OFDMPHRTX_MCS_SHIFT
/** Access parameters for sub-register RB5 in register @ref RG_BBC0_OFDMPHRTX */
#define SR_BBC0_OFDMPHRTX_RB5           RG_BBC0_OFDMPHRTX, OFDMPHRTX_RB5_MASK, \
	OFDMPHRTX_RB5_SHIFT

/** Access parameters for sub-register RB17 in register @ref RG_BBC0_OFDMPHRTX
**/
#define SR_BBC0_OFDMPHRTX_RB17          RG_BBC0_OFDMPHRTX, OFDMPHRTX_RB17_MASK,	\
	OFDMPHRTX_RB17_SHIFT

/** Access parameters for sub-register RB18 in register @ref RG_BBC0_OFDMPHRTX
**/
#define SR_BBC0_OFDMPHRTX_RB18          RG_BBC0_OFDMPHRTX, OFDMPHRTX_RB18_MASK,	\
	OFDMPHRTX_RB18_SHIFT

/** Access parameters for sub-register RB21 in register @ref RG_BBC0_OFDMPHRTX
**/
#define SR_BBC0_OFDMPHRTX_RB21          RG_BBC0_OFDMPHRTX, OFDMPHRTX_RB21_MASK,	\
	OFDMPHRTX_RB21_SHIFT

/** Address for register BBC0_OFDMPHRRX */
#define RG_BBC0_OFDMPHRRX               (0x30D)
/** Sub-registers of Register @ref RG_BBC0_OFDMPHRRX */
/** Access parameters for sub-register MCS in register @ref RG_BBC0_OFDMPHRRX */
#define SR_BBC0_OFDMPHRRX_MCS           RG_BBC0_OFDMPHRRX, OFDMPHRRX_MCS_MASK, \
	OFDMPHRRX_MCS_SHIFT
/** Access parameters for sub-register SPC in register @ref RG_BBC0_OFDMPHRRX */
#define SR_BBC0_OFDMPHRRX_SPC           RG_BBC0_OFDMPHRRX, OFDMPHRRX_SPC_MASK, \
	OFDMPHRRX_SPC_SHIFT
/** Access parameters for sub-register RB5 in register @ref RG_BBC0_OFDMPHRRX */
#define SR_BBC0_OFDMPHRRX_RB5           RG_BBC0_OFDMPHRRX, OFDMPHRRX_RB5_MASK, \
	OFDMPHRRX_RB5_SHIFT

/** Access parameters for sub-register RB17 in register @ref RG_BBC0_OFDMPHRRX
**/
#define SR_BBC0_OFDMPHRRX_RB17          RG_BBC0_OFDMPHRRX, OFDMPHRRX_RB17_MASK,	\
	OFDMPHRRX_RB17_SHIFT

/** Access parameters for sub-register RB18 in register @ref RG_BBC0_OFDMPHRRX
**/
#define SR_BBC0_OFDMPHRRX_RB18          RG_BBC0_OFDMPHRRX, OFDMPHRRX_RB18_MASK,	\
	OFDMPHRRX_RB18_SHIFT

/** Access parameters for sub-register RB21 in register @ref RG_BBC0_OFDMPHRRX
**/
#define SR_BBC0_OFDMPHRRX_RB21          RG_BBC0_OFDMPHRRX, OFDMPHRRX_RB21_MASK,	\
	OFDMPHRRX_RB21_SHIFT

/** Address for register BBC0_OFDMC */
#define RG_BBC0_OFDMC                   (0x30E)
/** Sub-registers of Register @ref RG_BBC0_OFDMC */
/** Access parameters for sub-register OPT in register @ref RG_BBC0_OFDMC */
#define SR_BBC0_OFDMC_OPT               RG_BBC0_OFDMC, OFDMC_OPT_MASK, \
	OFDMC_OPT_SHIFT
/** Access parameters for sub-register POI in register @ref RG_BBC0_OFDMC */
#define SR_BBC0_OFDMC_POI               RG_BBC0_OFDMC, OFDMC_POI_MASK, \
	OFDMC_POI_SHIFT
/** Access parameters for sub-register LFO in register @ref RG_BBC0_OFDMC */
#define SR_BBC0_OFDMC_LFO               RG_BBC0_OFDMC, OFDMC_LFO_MASK, \
	OFDMC_LFO_SHIFT
/** Access parameters for sub-register SSTX in register @ref RG_BBC0_OFDMC */
#define SR_BBC0_OFDMC_SSTX              RG_BBC0_OFDMC, OFDMC_SSTX_MASK,	\
	OFDMC_SSTX_SHIFT
/** Access parameters for sub-register SSRX in register @ref RG_BBC0_OFDMC */
#define SR_BBC0_OFDMC_SSRX              RG_BBC0_OFDMC, OFDMC_SSRX_MASK,	\
	OFDMC_SSRX_SHIFT

/** Address for register BBC0_OFDMSW */
#define RG_BBC0_OFDMSW                  (0x30F)
/** Sub-registers of Register @ref RG_BBC0_OFDMSW */
/** Access parameters for sub-register RXO in register @ref RG_BBC0_OFDMSW */
#define SR_BBC0_OFDMSW_RXO              RG_BBC0_OFDMSW, OFDMSW_RXO_MASK, \
	OFDMSW_RXO_SHIFT
/** Access parameters for sub-register PDT in register @ref RG_BBC0_OFDMSW */
#define SR_BBC0_OFDMSW_PDT              RG_BBC0_OFDMSW, OFDMSW_PDT_MASK, \
	OFDMSW_PDT_SHIFT

/** Address for register BBC0_OQPSKC0 */
#define RG_BBC0_OQPSKC0                 (0x310)
/** Sub-registers of Register @ref RG_BBC0_OQPSKC0 */
/** Access parameters for sub-register FCHIP in register @ref RG_BBC0_OQPSKC0 */
#define SR_BBC0_OQPSKC0_FCHIP           RG_BBC0_OQPSKC0, OQPSKC0_FCHIP_MASK, \
	OQPSKC0_FCHIP_SHIFT
/** Constant FCHIP100 for sub-register @ref SR_FCHIP in register BBC0_OQPSKC0 */
#define BB_FCHIP100                     (0x0)

/** Constant FCHIP1000 for sub-register @ref SR_FCHIP in register BBC0_OQPSKC0
**/
#define BB_FCHIP1000                    (0x2)
/** Constant FCHIP200 for sub-register @ref SR_FCHIP in register BBC0_OQPSKC0 */
#define BB_FCHIP200                     (0x1)

/** Constant FCHIP2000 for sub-register @ref SR_FCHIP in register BBC0_OQPSKC0
**/
#define BB_FCHIP2000                    (0x3)
/** Access parameters for sub-register MOD in register @ref RG_BBC0_OQPSKC0 */
#define SR_BBC0_OQPSKC0_MOD             RG_BBC0_OQPSKC0, OQPSKC0_MOD_MASK, \
	OQPSKC0_MOD_SHIFT
/** Constant RC08 for sub-register @ref SR_MOD in register BBC0_OQPSKC0 */
#define BB_RC08                         (0x0)
/** Constant RRC08 for sub-register @ref SR_MOD in register BBC0_OQPSKC0 */
#define BB_RRC08                        (0x1)
/** Access parameters for sub-register DM in register @ref RG_BBC0_OQPSKC0 */
#define SR_BBC0_OQPSKC0_DM              RG_BBC0_OQPSKC0, OQPSKC0_DM_MASK, \
	OQPSKC0_DM_SHIFT

/** Address for register BBC0_OQPSKC1 */
#define RG_BBC0_OQPSKC1                 (0x311)
/** Sub-registers of Register @ref RG_BBC0_OQPSKC1 */
/** Access parameters for sub-register PDT0 in register @ref RG_BBC0_OQPSKC1 */
#define SR_BBC0_OQPSKC1_PDT0            RG_BBC0_OQPSKC1, OQPSKC1_PDT0_MASK, \
	OQPSKC1_PDT0_SHIFT
/** Access parameters for sub-register PDT1 in register @ref RG_BBC0_OQPSKC1 */
#define SR_BBC0_OQPSKC1_PDT1            RG_BBC0_OQPSKC1, OQPSKC1_PDT1_MASK, \
	OQPSKC1_PDT1_SHIFT

/** Access parameters for sub-register RXOLEG in register @ref RG_BBC0_OQPSKC1
**/
#define SR_BBC0_OQPSKC1_RXOLEG          RG_BBC0_OQPSKC1, OQPSKC1_RXOLEG_MASK, \
	OQPSKC1_RXOLEG_SHIFT
/** Access parameters for sub-register RXO in register @ref RG_BBC0_OQPSKC1 */
#define SR_BBC0_OQPSKC1_RXO             RG_BBC0_OQPSKC1, OQPSKC1_RXO_MASK, \
	OQPSKC1_RXO_SHIFT

/** Address for register BBC0_OQPSKC2 */
#define RG_BBC0_OQPSKC2                 (0x312)
/** Sub-registers of Register @ref RG_BBC0_OQPSKC2 */
/** Access parameters for sub-register RXM in register @ref RG_BBC0_OQPSKC2 */
#define SR_BBC0_OQPSKC2_RXM             RG_BBC0_OQPSKC2, OQPSKC2_RXM_MASK, \
	OQPSKC2_RXM_SHIFT

/** Access parameters for sub-register FCSTLEG in register @ref RG_BBC0_OQPSKC2
**/
#define SR_BBC0_OQPSKC2_FCSTLEG         RG_BBC0_OQPSKC2, OQPSKC2_FCSTLEG_MASK, \
	OQPSKC2_FCSTLEG_SHIFT

/** Access parameters for sub-register ENPROP in register @ref RG_BBC0_OQPSKC2
**/
#define SR_BBC0_OQPSKC2_ENPROP          RG_BBC0_OQPSKC2, OQPSKC2_ENPROP_MASK, \
	OQPSKC2_ENPROP_SHIFT
/** Access parameters for sub-register RPC in register @ref RG_BBC0_OQPSKC2 */
#define SR_BBC0_OQPSKC2_RPC             RG_BBC0_OQPSKC2, OQPSKC2_RPC_MASK, \
	OQPSKC2_RPC_SHIFT
/** Access parameters for sub-register SPC in register @ref RG_BBC0_OQPSKC2 */
#define SR_BBC0_OQPSKC2_SPC             RG_BBC0_OQPSKC2, OQPSKC2_SPC_MASK, \
	OQPSKC2_SPC_SHIFT

/** Address for register BBC0_OQPSKC3 */
#define RG_BBC0_OQPSKC3                 (0x313)
/** Sub-registers of Register @ref RG_BBC0_OQPSKC3 */
/** Access parameters for sub-register NSFD in register @ref RG_BBC0_OQPSKC3 */
#define SR_BBC0_OQPSKC3_NSFD            RG_BBC0_OQPSKC3, OQPSKC3_NSFD_MASK, \
	OQPSKC3_NSFD_SHIFT
/** Access parameters for sub-register HRLEG in register @ref RG_BBC0_OQPSKC3 */
#define SR_BBC0_OQPSKC3_HRLEG           RG_BBC0_OQPSKC3, OQPSKC3_HRLEG_MASK, \
	OQPSKC3_HRLEG_SHIFT

/** Address for register BBC0_OQPSKPHRTX */
#define RG_BBC0_OQPSKPHRTX              (0x314)
/** Sub-registers of Register @ref RG_BBC0_OQPSKPHRTX */

/** Access parameters for sub-register LEG in register @ref RG_BBC0_OQPSKPHRTX
**/
#define SR_BBC0_OQPSKPHRTX_LEG          RG_BBC0_OQPSKPHRTX, OQPSKPHRTX_LEG_MASK, \
	OQPSKPHRTX_LEG_SHIFT

/** Access parameters for sub-register MOD in register @ref RG_BBC0_OQPSKPHRTX
**/
#define SR_BBC0_OQPSKPHRTX_MOD          RG_BBC0_OQPSKPHRTX, OQPSKPHRTX_MOD_MASK, \
	OQPSKPHRTX_MOD_SHIFT

/** Access parameters for sub-register RB0 in register @ref RG_BBC0_OQPSKPHRTX
**/
#define SR_BBC0_OQPSKPHRTX_RB0          RG_BBC0_OQPSKPHRTX, OQPSKPHRTX_RB0_MASK, \
	OQPSKPHRTX_RB0_SHIFT

/** Access parameters for sub-register PPDUT in register @ref RG_BBC0_OQPSKPHRTX
**/
#define SR_BBC0_OQPSKPHRTX_PPDUT        RG_BBC0_OQPSKPHRTX, \
	OQPSKPHRTX_PPDUT_MASK, OQPSKPHRTX_PPDUT_SHIFT

/** Address for register BBC0_OQPSKPHRRX */
#define RG_BBC0_OQPSKPHRRX              (0x315)
/** Sub-registers of Register @ref RG_BBC0_OQPSKPHRRX */

/** Access parameters for sub-register LEG in register @ref RG_BBC0_OQPSKPHRRX
**/
#define SR_BBC0_OQPSKPHRRX_LEG          RG_BBC0_OQPSKPHRRX, OQPSKPHRRX_LEG_MASK, \
	OQPSKPHRRX_LEG_SHIFT

/** Access parameters for sub-register MOD in register @ref RG_BBC0_OQPSKPHRRX
**/
#define SR_BBC0_OQPSKPHRRX_MOD          RG_BBC0_OQPSKPHRRX, OQPSKPHRRX_MOD_MASK, \
	OQPSKPHRRX_MOD_SHIFT

/** Access parameters for sub-register RB0 in register @ref RG_BBC0_OQPSKPHRRX
**/
#define SR_BBC0_OQPSKPHRRX_RB0          RG_BBC0_OQPSKPHRRX, OQPSKPHRRX_RB0_MASK, \
	OQPSKPHRRX_RB0_SHIFT

/** Access parameters for sub-register PPDUT in register @ref RG_BBC0_OQPSKPHRRX
**/
#define SR_BBC0_OQPSKPHRRX_PPDUT        RG_BBC0_OQPSKPHRRX, \
	OQPSKPHRRX_PPDUT_MASK, OQPSKPHRRX_PPDUT_SHIFT

/** Address for register BBC0_AFC0 */
#define RG_BBC0_AFC0                    (0x320)
/** Sub-registers of Register @ref RG_BBC0_AFC0 */
/** Access parameters for sub-register AFEN0 in register @ref RG_BBC0_AFC0 */
#define SR_BBC0_AFC0_AFEN0              RG_BBC0_AFC0, AFC0_AFEN0_MASK, \
	AFC0_AFEN0_SHIFT
/** Access parameters for sub-register AFEN1 in register @ref RG_BBC0_AFC0 */
#define SR_BBC0_AFC0_AFEN1              RG_BBC0_AFC0, AFC0_AFEN1_MASK, \
	AFC0_AFEN1_SHIFT
/** Access parameters for sub-register AFEN2 in register @ref RG_BBC0_AFC0 */
#define SR_BBC0_AFC0_AFEN2              RG_BBC0_AFC0, AFC0_AFEN2_MASK, \
	AFC0_AFEN2_SHIFT
/** Access parameters for sub-register AFEN3 in register @ref RG_BBC0_AFC0 */
#define SR_BBC0_AFC0_AFEN3              RG_BBC0_AFC0, AFC0_AFEN3_MASK, \
	AFC0_AFEN3_SHIFT
/** Access parameters for sub-register PM in register @ref RG_BBC0_AFC0 */
#define SR_BBC0_AFC0_PM                 RG_BBC0_AFC0, AFC0_PM_MASK, \
	AFC0_PM_SHIFT

/** Address for register BBC0_AFC1 */
#define RG_BBC0_AFC1                    (0x321)
/** Sub-registers of Register @ref RG_BBC0_AFC1 */
/** Access parameters for sub-register PANC0 in register @ref RG_BBC0_AFC1 */
#define SR_BBC0_AFC1_PANC0              RG_BBC0_AFC1, AFC1_PANC0_MASK, \
	AFC1_PANC0_SHIFT
/** Access parameters for sub-register PANC1 in register @ref RG_BBC0_AFC1 */
#define SR_BBC0_AFC1_PANC1              RG_BBC0_AFC1, AFC1_PANC1_MASK, \
	AFC1_PANC1_SHIFT
/** Access parameters for sub-register PANC2 in register @ref RG_BBC0_AFC1 */
#define SR_BBC0_AFC1_PANC2              RG_BBC0_AFC1, AFC1_PANC2_MASK, \
	AFC1_PANC2_SHIFT
/** Access parameters for sub-register PANC3 in register @ref RG_BBC0_AFC1 */
#define SR_BBC0_AFC1_PANC3              RG_BBC0_AFC1, AFC1_PANC3_MASK, \
	AFC1_PANC3_SHIFT
/** Access parameters for sub-register MRFT0 in register @ref RG_BBC0_AFC1 */
#define SR_BBC0_AFC1_MRFT0              RG_BBC0_AFC1, AFC1_MRFT0_MASK, \
	AFC1_MRFT0_SHIFT
/** Access parameters for sub-register MRFT1 in register @ref RG_BBC0_AFC1 */
#define SR_BBC0_AFC1_MRFT1              RG_BBC0_AFC1, AFC1_MRFT1_MASK, \
	AFC1_MRFT1_SHIFT
/** Access parameters for sub-register MRFT2 in register @ref RG_BBC0_AFC1 */
#define SR_BBC0_AFC1_MRFT2              RG_BBC0_AFC1, AFC1_MRFT2_MASK, \
	AFC1_MRFT2_SHIFT
/** Access parameters for sub-register MRFT3 in register @ref RG_BBC0_AFC1 */
#define SR_BBC0_AFC1_MRFT3              RG_BBC0_AFC1, AFC1_MRFT3_MASK, \
	AFC1_MRFT3_SHIFT

/** Address for register BBC0_AFFTM */
#define RG_BBC0_AFFTM                   (0x322)
/** Sub-registers of Register @ref RG_BBC0_AFFTM */
/** Access parameters for sub-register AFFTM in register @ref RG_BBC0_AFFTM */
#define SR_BBC0_AFFTM_AFFTM             RG_BBC0_AFFTM, AFFTM_AFFTM_MASK, \
	AFFTM_AFFTM_SHIFT

/** Address for register BBC0_AFFVM */
#define RG_BBC0_AFFVM                   (0x323)
/** Sub-registers of Register @ref RG_BBC0_AFFVM */
/** Access parameters for sub-register AFFVM in register @ref RG_BBC0_AFFVM */
#define SR_BBC0_AFFVM_AFFVM             RG_BBC0_AFFVM, AFFVM_AFFVM_MASK, \
	AFFVM_AFFVM_SHIFT

/** Address for register BBC0_AFS */
#define RG_BBC0_AFS                     (0x324)
/** Sub-registers of Register @ref RG_BBC0_AFS */
/** Access parameters for sub-register AM0 in register @ref RG_BBC0_AFS */
#define SR_BBC0_AFS_AM0                 RG_BBC0_AFS, AFS_AM0_MASK, AFS_AM0_SHIFT
/** Access parameters for sub-register AM1 in register @ref RG_BBC0_AFS */
#define SR_BBC0_AFS_AM1                 RG_BBC0_AFS, AFS_AM1_MASK, AFS_AM1_SHIFT
/** Access parameters for sub-register AM2 in register @ref RG_BBC0_AFS */
#define SR_BBC0_AFS_AM2                 RG_BBC0_AFS, AFS_AM2_MASK, AFS_AM2_SHIFT
/** Access parameters for sub-register AM3 in register @ref RG_BBC0_AFS */
#define SR_BBC0_AFS_AM3                 RG_BBC0_AFS, AFS_AM3_MASK, AFS_AM3_SHIFT
/** Access parameters for sub-register EM in register @ref RG_BBC0_AFS */
#define SR_BBC0_AFS_EM                  RG_BBC0_AFS, AFS_EM_MASK, AFS_EM_SHIFT

/** Address for register BBC0_MACEA0 */
#define RG_BBC0_MACEA0                  (0x325)
/** Sub-registers of Register @ref RG_BBC0_MACEA0 */
/** Access parameters for sub-register MACEA0 in register @ref RG_BBC0_MACEA0 */
#define SR_BBC0_MACEA0_MACEA0           RG_BBC0_MACEA0, MACEA0_MACEA0_MASK, \
	MACEA0_MACEA0_SHIFT

/** Address for register BBC0_MACEA1 */
#define RG_BBC0_MACEA1                  (0x326)
/** Sub-registers of Register @ref RG_BBC0_MACEA1 */
/** Access parameters for sub-register MACEA1 in register @ref RG_BBC0_MACEA1 */
#define SR_BBC0_MACEA1_MACEA1           RG_BBC0_MACEA1, MACEA1_MACEA1_MASK, \
	MACEA1_MACEA1_SHIFT

/** Address for register BBC0_MACEA2 */
#define RG_BBC0_MACEA2                  (0x327)
/** Sub-registers of Register @ref RG_BBC0_MACEA2 */
/** Access parameters for sub-register MACEA2 in register @ref RG_BBC0_MACEA2 */
#define SR_BBC0_MACEA2_MACEA2           RG_BBC0_MACEA2, MACEA2_MACEA2_MASK, \
	MACEA2_MACEA2_SHIFT

/** Address for register BBC0_MACEA3 */
#define RG_BBC0_MACEA3                  (0x328)
/** Sub-registers of Register @ref RG_BBC0_MACEA3 */
/** Access parameters for sub-register MACEA3 in register @ref RG_BBC0_MACEA3 */
#define SR_BBC0_MACEA3_MACEA3           RG_BBC0_MACEA3, MACEA3_MACEA3_MASK, \
	MACEA3_MACEA3_SHIFT

/** Address for register BBC0_MACEA4 */
#define RG_BBC0_MACEA4                  (0x329)
/** Sub-registers of Register @ref RG_BBC0_MACEA4 */
/** Access parameters for sub-register MACEA4 in register @ref RG_BBC0_MACEA4 */
#define SR_BBC0_MACEA4_MACEA4           RG_BBC0_MACEA4, MACEA4_MACEA4_MASK, \
	MACEA4_MACEA4_SHIFT

/** Address for register BBC0_MACEA5 */
#define RG_BBC0_MACEA5                  (0x32A)
/** Sub-registers of Register @ref RG_BBC0_MACEA5 */
/** Access parameters for sub-register MACEA5 in register @ref RG_BBC0_MACEA5 */
#define SR_BBC0_MACEA5_MACEA5           RG_BBC0_MACEA5, MACEA5_MACEA5_MASK, \
	MACEA5_MACEA5_SHIFT

/** Address for register BBC0_MACEA6 */
#define RG_BBC0_MACEA6                  (0x32B)
/** Sub-registers of Register @ref RG_BBC0_MACEA6 */
/** Access parameters for sub-register MACEA6 in register @ref RG_BBC0_MACEA6 */
#define SR_BBC0_MACEA6_MACEA6           RG_BBC0_MACEA6, MACEA6_MACEA6_MASK, \
	MACEA6_MACEA6_SHIFT

/** Address for register BBC0_MACEA7 */
#define RG_BBC0_MACEA7                  (0x32C)
/** Sub-registers of Register @ref RG_BBC0_MACEA7 */
/** Access parameters for sub-register MACEA7 in register @ref RG_BBC0_MACEA7 */
#define SR_BBC0_MACEA7_MACEA7           RG_BBC0_MACEA7, MACEA7_MACEA7_MASK, \
	MACEA7_MACEA7_SHIFT

/** Address for register BBC0_MACPID0F0 */
#define RG_BBC0_MACPID0F0               (0x32D)
/** Sub-registers of Register @ref RG_BBC0_MACPID0F0 */

/** Access parameters for sub-register MACPID0F0 in register @ref
 * RG_BBC0_MACPID0F0 */
#define SR_BBC0_MACPID0F0_MACPID0F0     RG_BBC0_MACPID0F0, \
	MACPID0F0_MACPID0F0_MASK, MACPID0F0_MACPID0F0_SHIFT

/** Address for register BBC0_MACPID1F0 */
#define RG_BBC0_MACPID1F0               (0x32E)
/** Sub-registers of Register @ref RG_BBC0_MACPID1F0 */

/** Access parameters for sub-register MACPID1F0 in register @ref
 * RG_BBC0_MACPID1F0 */
#define SR_BBC0_MACPID1F0_MACPID1F0     RG_BBC0_MACPID1F0, \
	MACPID1F0_MACPID1F0_MASK, MACPID1F0_MACPID1F0_SHIFT

/** Address for register BBC0_MACSHA0F0 */
#define RG_BBC0_MACSHA0F0               (0x32F)
/** Sub-registers of Register @ref RG_BBC0_MACSHA0F0 */

/** Access parameters for sub-register MACSHA0F0 in register @ref
 * RG_BBC0_MACSHA0F0 */
#define SR_BBC0_MACSHA0F0_MACSHA0F0     RG_BBC0_MACSHA0F0, \
	MACSHA0F0_MACSHA0F0_MASK, MACSHA0F0_MACSHA0F0_SHIFT

/** Address for register BBC0_MACSHA1F0 */
#define RG_BBC0_MACSHA1F0               (0x330)
/** Sub-registers of Register @ref RG_BBC0_MACSHA1F0 */

/** Access parameters for sub-register MACSHA1F0 in register @ref
 * RG_BBC0_MACSHA1F0 */
#define SR_BBC0_MACSHA1F0_MACSHA1F0     RG_BBC0_MACSHA1F0, \
	MACSHA1F0_MACSHA1F0_MASK, MACSHA1F0_MACSHA1F0_SHIFT

/** Address for register BBC0_MACPID0F1 */
#define RG_BBC0_MACPID0F1               (0x331)
/** Sub-registers of Register @ref RG_BBC0_MACPID0F1 */

/** Access parameters for sub-register MACPID0F1 in register @ref
 * RG_BBC0_MACPID0F1 */
#define SR_BBC0_MACPID0F1_MACPID0F1     RG_BBC0_MACPID0F1, \
	MACPID0F1_MACPID0F1_MASK, MACPID0F1_MACPID0F1_SHIFT

/** Address for register BBC0_MACPID1F1 */
#define RG_BBC0_MACPID1F1               (0x332)
/** Sub-registers of Register @ref RG_BBC0_MACPID1F1 */

/** Access parameters for sub-register MACPID1F1 in register @ref
 * RG_BBC0_MACPID1F1 */
#define SR_BBC0_MACPID1F1_MACPID1F1     RG_BBC0_MACPID1F1, \
	MACPID1F1_MACPID1F1_MASK, MACPID1F1_MACPID1F1_SHIFT

/** Address for register BBC0_MACSHA0F1 */
#define RG_BBC0_MACSHA0F1               (0x333)
/** Sub-registers of Register @ref RG_BBC0_MACSHA0F1 */

/** Access parameters for sub-register MACSHA0F1 in register @ref
 * RG_BBC0_MACSHA0F1 */
#define SR_BBC0_MACSHA0F1_MACSHA0F1     RG_BBC0_MACSHA0F1, \
	MACSHA0F1_MACSHA0F1_MASK, MACSHA0F1_MACSHA0F1_SHIFT

/** Address for register BBC0_MACSHA1F1 */
#define RG_BBC0_MACSHA1F1               (0x334)
/** Sub-registers of Register @ref RG_BBC0_MACSHA1F1 */

/** Access parameters for sub-register MACSHA1F1 in register @ref
 * RG_BBC0_MACSHA1F1 */
#define SR_BBC0_MACSHA1F1_MACSHA1F1     RG_BBC0_MACSHA1F1, \
	MACSHA1F1_MACSHA1F1_MASK, MACSHA1F1_MACSHA1F1_SHIFT

/** Address for register BBC0_MACPID0F2 */
#define RG_BBC0_MACPID0F2               (0x335)
/** Sub-registers of Register @ref RG_BBC0_MACPID0F2 */

/** Access parameters for sub-register MACPID0F2 in register @ref
 * RG_BBC0_MACPID0F2 */
#define SR_BBC0_MACPID0F2_MACPID0F2     RG_BBC0_MACPID0F2, \
	MACPID0F2_MACPID0F2_MASK, MACPID0F2_MACPID0F2_SHIFT

/** Address for register BBC0_MACPID1F2 */
#define RG_BBC0_MACPID1F2               (0x336)
/** Sub-registers of Register @ref RG_BBC0_MACPID1F2 */

/** Access parameters for sub-register MACPID1F2 in register @ref
 * RG_BBC0_MACPID1F2 */
#define SR_BBC0_MACPID1F2_MACPID1F2     RG_BBC0_MACPID1F2, \
	MACPID1F2_MACPID1F2_MASK, MACPID1F2_MACPID1F2_SHIFT

/** Address for register BBC0_MACSHA0F2 */
#define RG_BBC0_MACSHA0F2               (0x337)
/** Sub-registers of Register @ref RG_BBC0_MACSHA0F2 */

/** Access parameters for sub-register MACSHA0F2 in register @ref
 * RG_BBC0_MACSHA0F2 */
#define SR_BBC0_MACSHA0F2_MACSHA0F2     RG_BBC0_MACSHA0F2, \
	MACSHA0F2_MACSHA0F2_MASK, MACSHA0F2_MACSHA0F2_SHIFT

/** Address for register BBC0_MACSHA1F2 */
#define RG_BBC0_MACSHA1F2               (0x338)
/** Sub-registers of Register @ref RG_BBC0_MACSHA1F2 */

/** Access parameters for sub-register MACSHA1F2 in register @ref
 * RG_BBC0_MACSHA1F2 */
#define SR_BBC0_MACSHA1F2_MACSHA1F2     RG_BBC0_MACSHA1F2, \
	MACSHA1F2_MACSHA1F2_MASK, MACSHA1F2_MACSHA1F2_SHIFT

/** Address for register BBC0_MACPID0F3 */
#define RG_BBC0_MACPID0F3               (0x339)
/** Sub-registers of Register @ref RG_BBC0_MACPID0F3 */

/** Access parameters for sub-register MACPID0F3 in register @ref
 * RG_BBC0_MACPID0F3 */
#define SR_BBC0_MACPID0F3_MACPID0F3     RG_BBC0_MACPID0F3, \
	MACPID0F3_MACPID0F3_MASK, MACPID0F3_MACPID0F3_SHIFT

/** Address for register BBC0_MACPID1F3 */
#define RG_BBC0_MACPID1F3               (0x33A)
/** Sub-registers of Register @ref RG_BBC0_MACPID1F3 */

/** Access parameters for sub-register MACPID1F3 in register @ref
 * RG_BBC0_MACPID1F3 */
#define SR_BBC0_MACPID1F3_MACPID1F3     RG_BBC0_MACPID1F3, \
	MACPID1F3_MACPID1F3_MASK, MACPID1F3_MACPID1F3_SHIFT

/** Address for register BBC0_MACSHA0F3 */
#define RG_BBC0_MACSHA0F3               (0x33B)
/** Sub-registers of Register @ref RG_BBC0_MACSHA0F3 */

/** Access parameters for sub-register MACSHA0F3 in register @ref
 * RG_BBC0_MACSHA0F3 */
#define SR_BBC0_MACSHA0F3_MACSHA0F3     RG_BBC0_MACSHA0F3, \
	MACSHA0F3_MACSHA0F3_MASK, MACSHA0F3_MACSHA0F3_SHIFT

/** Address for register BBC0_MACSHA1F3 */
#define RG_BBC0_MACSHA1F3               (0x33C)
/** Sub-registers of Register @ref RG_BBC0_MACSHA1F3 */

/** Access parameters for sub-register MACSHA1F3 in register @ref
 * RG_BBC0_MACSHA1F3 */
#define SR_BBC0_MACSHA1F3_MACSHA1F3     RG_BBC0_MACSHA1F3, \
	MACSHA1F3_MACSHA1F3_MASK, MACSHA1F3_MACSHA1F3_SHIFT

/** Address for register BBC0_AMCS */
#define RG_BBC0_AMCS                    (0x340)
/** Sub-registers of Register @ref RG_BBC0_AMCS */
/** Access parameters for sub-register TX2RX in register @ref RG_BBC0_AMCS */
#define SR_BBC0_AMCS_TX2RX              RG_BBC0_AMCS, AMCS_TX2RX_MASK, \
	AMCS_TX2RX_SHIFT
/** Access parameters for sub-register CCATX in register @ref RG_BBC0_AMCS */
#define SR_BBC0_AMCS_CCATX              RG_BBC0_AMCS, AMCS_CCATX_MASK, \
	AMCS_CCATX_SHIFT
/** Access parameters for sub-register CCAED in register @ref RG_BBC0_AMCS */
#define SR_BBC0_AMCS_CCAED              RG_BBC0_AMCS, AMCS_CCAED_MASK, \
	AMCS_CCAED_SHIFT
/** Constant CH_BUSY for sub-register @ref SR_CCAED in register BBC0_AMCS */
#define BB_CH_BUSY                      (0x1)
/** Constant CH_CLEAR for sub-register @ref SR_CCAED in register BBC0_AMCS */
#define BB_CH_CLEAR                     (0x0)
/** Access parameters for sub-register AACK in register @ref RG_BBC0_AMCS */
#define SR_BBC0_AMCS_AACK               RG_BBC0_AMCS, AMCS_AACK_MASK, \
	AMCS_AACK_SHIFT
/** Access parameters for sub-register AACKS in register @ref RG_BBC0_AMCS */
#define SR_BBC0_AMCS_AACKS              RG_BBC0_AMCS, AMCS_AACKS_MASK, \
	AMCS_AACKS_SHIFT
/** Access parameters for sub-register AACKDR in register @ref RG_BBC0_AMCS */
#define SR_BBC0_AMCS_AACKDR             RG_BBC0_AMCS, AMCS_AACKDR_MASK,	\
	AMCS_AACKDR_SHIFT
/** Access parameters for sub-register AACKFA in register @ref RG_BBC0_AMCS */
#define SR_BBC0_AMCS_AACKFA             RG_BBC0_AMCS, AMCS_AACKFA_MASK,	\
	AMCS_AACKFA_SHIFT
/** Access parameters for sub-register AACKFT in register @ref RG_BBC0_AMCS */
#define SR_BBC0_AMCS_AACKFT             RG_BBC0_AMCS, AMCS_AACKFT_MASK,	\
	AMCS_AACKFT_SHIFT

/** Address for register BBC0_AMEDT */
#define RG_BBC0_AMEDT                   (0x341)
/** Sub-registers of Register @ref RG_BBC0_AMEDT */
/** Access parameters for sub-register AMEDT in register @ref RG_BBC0_AMEDT */
#define SR_BBC0_AMEDT_AMEDT             RG_BBC0_AMEDT, AMEDT_AMEDT_MASK, \
	AMEDT_AMEDT_SHIFT

/** Address for register BBC0_AMAACKPD */
#define RG_BBC0_AMAACKPD                (0x342)
/** Sub-registers of Register @ref RG_BBC0_AMAACKPD */
/** Access parameters for sub-register PD0 in register @ref RG_BBC0_AMAACKPD */
#define SR_BBC0_AMAACKPD_PD0            RG_BBC0_AMAACKPD, AMAACKPD_PD0_MASK, \
	AMAACKPD_PD0_SHIFT
/** Access parameters for sub-register PD1 in register @ref RG_BBC0_AMAACKPD */
#define SR_BBC0_AMAACKPD_PD1            RG_BBC0_AMAACKPD, AMAACKPD_PD1_MASK, \
	AMAACKPD_PD1_SHIFT
/** Access parameters for sub-register PD2 in register @ref RG_BBC0_AMAACKPD */
#define SR_BBC0_AMAACKPD_PD2            RG_BBC0_AMAACKPD, AMAACKPD_PD2_MASK, \
	AMAACKPD_PD2_SHIFT
/** Access parameters for sub-register PD3 in register @ref RG_BBC0_AMAACKPD */
#define SR_BBC0_AMAACKPD_PD3            RG_BBC0_AMAACKPD, AMAACKPD_PD3_MASK, \
	AMAACKPD_PD3_SHIFT

/** Address for register BBC0_AMAACKTL */
#define RG_BBC0_AMAACKTL                (0x343)
/** Sub-registers of Register @ref RG_BBC0_AMAACKTL */

/** Access parameters for sub-register AMAACKTL in register @ref
 * RG_BBC0_AMAACKTL */
#define SR_BBC0_AMAACKTL_AMAACKTL       RG_BBC0_AMAACKTL, \
	AMAACKTL_AMAACKTL_MASK, AMAACKTL_AMAACKTL_SHIFT

/** Address for register BBC0_AMAACKTH */
#define RG_BBC0_AMAACKTH                (0x344)
/** Sub-registers of Register @ref RG_BBC0_AMAACKTH */

/** Access parameters for sub-register AMAACKTH in register @ref
 * RG_BBC0_AMAACKTH */
#define SR_BBC0_AMAACKTH_AMAACKTH       RG_BBC0_AMAACKTH, \
	AMAACKTH_AMAACKTH_MASK, AMAACKTH_AMAACKTH_SHIFT

/** Address for register BBC0_FSKC0 */
#define RG_BBC0_FSKC0                   (0x360)
/** Sub-registers of Register @ref RG_BBC0_FSKC0 */
/** Access parameters for sub-register MORD in register @ref RG_BBC0_FSKC0 */
#define SR_BBC0_FSKC0_MORD              RG_BBC0_FSKC0, FSKC0_MORD_MASK,	\
	FSKC0_MORD_SHIFT
/** Access parameters for sub-register MIDX in register @ref RG_BBC0_FSKC0 */
#define SR_BBC0_FSKC0_MIDX              RG_BBC0_FSKC0, FSKC0_MIDX_MASK,	\
	FSKC0_MIDX_SHIFT
/** Access parameters for sub-register MIDXS in register @ref RG_BBC0_FSKC0 */
#define SR_BBC0_FSKC0_MIDXS             RG_BBC0_FSKC0, FSKC0_MIDXS_MASK, \
	FSKC0_MIDXS_SHIFT
/** Access parameters for sub-register BT in register @ref RG_BBC0_FSKC0 */
#define SR_BBC0_FSKC0_BT                RG_BBC0_FSKC0, FSKC0_BT_MASK, \
	FSKC0_BT_SHIFT

/** Address for register BBC0_FSKC1 */
#define RG_BBC0_FSKC1                   (0x361)
/** Sub-registers of Register @ref RG_BBC0_FSKC1 */
/** Access parameters for sub-register SRATE in register @ref RG_BBC0_FSKC1 */
#define SR_BBC0_FSKC1_SRATE             RG_BBC0_FSKC1, FSKC1_SRATE_MASK, \
	FSKC1_SRATE_SHIFT
/** Access parameters for sub-register FI in register @ref RG_BBC0_FSKC1 */
#define SR_BBC0_FSKC1_FI                RG_BBC0_FSKC1, FSKC1_FI_MASK, \
	FSKC1_FI_SHIFT
/** Access parameters for sub-register FSKPLH in register @ref RG_BBC0_FSKC1 */
#define SR_BBC0_FSKC1_FSKPLH            RG_BBC0_FSKC1, FSKC1_FSKPLH_MASK, \
	FSKC1_FSKPLH_SHIFT

/** Address for register BBC0_FSKC2 */
#define RG_BBC0_FSKC2                   (0x362)
/** Sub-registers of Register @ref RG_BBC0_FSKC2 */
/** Access parameters for sub-register FECIE in register @ref RG_BBC0_FSKC2 */
#define SR_BBC0_FSKC2_FECIE             RG_BBC0_FSKC2, FSKC2_FECIE_MASK, \
	FSKC2_FECIE_SHIFT
/** Access parameters for sub-register FECS in register @ref RG_BBC0_FSKC2 */
#define SR_BBC0_FSKC2_FECS              RG_BBC0_FSKC2, FSKC2_FECS_MASK,	\
	FSKC2_FECS_SHIFT
/** Access parameters for sub-register PRI in register @ref RG_BBC0_FSKC2 */
#define SR_BBC0_FSKC2_PRI               RG_BBC0_FSKC2, FSKC2_PRI_MASK, \
	FSKC2_PRI_SHIFT
/** Access parameters for sub-register MSE in register @ref RG_BBC0_FSKC2 */
#define SR_BBC0_FSKC2_MSE               RG_BBC0_FSKC2, FSKC2_MSE_MASK, \
	FSKC2_MSE_SHIFT
/** Access parameters for sub-register RXPTO in register @ref RG_BBC0_FSKC2 */
#define SR_BBC0_FSKC2_RXPTO             RG_BBC0_FSKC2, FSKC2_RXPTO_MASK, \
	FSKC2_RXPTO_SHIFT
/** Access parameters for sub-register RXO in register @ref RG_BBC0_FSKC2 */
#define SR_BBC0_FSKC2_RXO               RG_BBC0_FSKC2, FSKC2_RXO_MASK, \
	FSKC2_RXO_SHIFT
/** Access parameters for sub-register PDTM in register @ref RG_BBC0_FSKC2 */
#define SR_BBC0_FSKC2_PDTM              RG_BBC0_FSKC2, FSKC2_PDTM_MASK,	\
	FSKC2_PDTM_SHIFT

/** Address for register BBC0_FSKC3 */
#define RG_BBC0_FSKC3                   (0x363)
/** Sub-registers of Register @ref RG_BBC0_FSKC3 */
/** Access parameters for sub-register PDT in register @ref RG_BBC0_FSKC3 */
#define SR_BBC0_FSKC3_PDT               RG_BBC0_FSKC3, FSKC3_PDT_MASK, \
	FSKC3_PDT_SHIFT
/** Access parameters for sub-register SFDT in register @ref RG_BBC0_FSKC3 */
#define SR_BBC0_FSKC3_SFDT              RG_BBC0_FSKC3, FSKC3_SFDT_MASK,	\
	FSKC3_SFDT_SHIFT

/** Address for register BBC0_FSKC4 */
#define RG_BBC0_FSKC4                   (0x364)
/** Sub-registers of Register @ref RG_BBC0_FSKC4 */
/** Access parameters for sub-register CSFD0 in register @ref RG_BBC0_FSKC4 */
#define SR_BBC0_FSKC4_CSFD0             RG_BBC0_FSKC4, FSKC4_CSFD0_MASK, \
	FSKC4_CSFD0_SHIFT
/** Access parameters for sub-register CSFD1 in register @ref RG_BBC0_FSKC4 */
#define SR_BBC0_FSKC4_CSFD1             RG_BBC0_FSKC4, FSKC4_CSFD1_MASK, \
	FSKC4_CSFD1_SHIFT
/** Access parameters for sub-register RAWRBIT in register @ref RG_BBC0_FSKC4 */
#define SR_BBC0_FSKC4_RAWRBIT           RG_BBC0_FSKC4, FSKC4_RAWRBIT_MASK, \
	FSKC4_RAWRBIT_SHIFT
/** Access parameters for sub-register SFD32 in register @ref RG_BBC0_FSKC4 */
#define SR_BBC0_FSKC4_SFD32             RG_BBC0_FSKC4, FSKC4_SFD32_MASK, \
	FSKC4_SFD32_SHIFT
/** Access parameters for sub-register SFDQ in register @ref RG_BBC0_FSKC4 */
#define SR_BBC0_FSKC4_SFDQ              RG_BBC0_FSKC4, FSKC4_SFDQ_MASK,	\
	FSKC4_SFDQ_SHIFT

/** Address for register BBC0_FSKPLL */
#define RG_BBC0_FSKPLL                  (0x365)
/** Sub-registers of Register @ref RG_BBC0_FSKPLL */
/** Access parameters for sub-register FSKPLL in register @ref RG_BBC0_FSKPLL */
#define SR_BBC0_FSKPLL_FSKPLL           RG_BBC0_FSKPLL, FSKPLL_FSKPLL_MASK, \
	FSKPLL_FSKPLL_SHIFT

/** Address for register BBC0_FSKSFD0L */
#define RG_BBC0_FSKSFD0L                (0x366)
/** Sub-registers of Register @ref RG_BBC0_FSKSFD0L */

/** Access parameters for sub-register FSKSFD0L in register @ref
 * RG_BBC0_FSKSFD0L */
#define SR_BBC0_FSKSFD0L_FSKSFD0L       RG_BBC0_FSKSFD0L, \
	FSKSFD0L_FSKSFD0L_MASK, FSKSFD0L_FSKSFD0L_SHIFT

/** Address for register BBC0_FSKSFD0H */
#define RG_BBC0_FSKSFD0H                (0x367)
/** Sub-registers of Register @ref RG_BBC0_FSKSFD0H */

/** Access parameters for sub-register FSKSFD0H in register @ref
 * RG_BBC0_FSKSFD0H */
#define SR_BBC0_FSKSFD0H_FSKSFD0H       RG_BBC0_FSKSFD0H, \
	FSKSFD0H_FSKSFD0H_MASK, FSKSFD0H_FSKSFD0H_SHIFT

/** Address for register BBC0_FSKSFD1L */
#define RG_BBC0_FSKSFD1L                (0x368)
/** Sub-registers of Register @ref RG_BBC0_FSKSFD1L */

/** Access parameters for sub-register FSKSFD1L in register @ref
 * RG_BBC0_FSKSFD1L */
#define SR_BBC0_FSKSFD1L_FSKSFD1L       RG_BBC0_FSKSFD1L, \
	FSKSFD1L_FSKSFD1L_MASK, FSKSFD1L_FSKSFD1L_SHIFT

/** Address for register BBC0_FSKSFD1H */
#define RG_BBC0_FSKSFD1H                (0x369)
/** Sub-registers of Register @ref RG_BBC0_FSKSFD1H */

/** Access parameters for sub-register FSKSFD1H in register @ref
 * RG_BBC0_FSKSFD1H */
#define SR_BBC0_FSKSFD1H_FSKSFD1H       RG_BBC0_FSKSFD1H, \
	FSKSFD1H_FSKSFD1H_MASK, FSKSFD1H_FSKSFD1H_SHIFT

/** Address for register BBC0_FSKPHRTX */
#define RG_BBC0_FSKPHRTX                (0x36A)
/** Sub-registers of Register @ref RG_BBC0_FSKPHRTX */
/** Access parameters for sub-register RB1 in register @ref RG_BBC0_FSKPHRTX */
#define SR_BBC0_FSKPHRTX_RB1            RG_BBC0_FSKPHRTX, FSKPHRTX_RB1_MASK, \
	FSKPHRTX_RB1_SHIFT
/** Access parameters for sub-register RB2 in register @ref RG_BBC0_FSKPHRTX */
#define SR_BBC0_FSKPHRTX_RB2            RG_BBC0_FSKPHRTX, FSKPHRTX_RB2_MASK, \
	FSKPHRTX_RB2_SHIFT
/** Access parameters for sub-register DW in register @ref RG_BBC0_FSKPHRTX */
#define SR_BBC0_FSKPHRTX_DW             RG_BBC0_FSKPHRTX, FSKPHRTX_DW_MASK, \
	FSKPHRTX_DW_SHIFT
/** Access parameters for sub-register SFD in register @ref RG_BBC0_FSKPHRTX */
#define SR_BBC0_FSKPHRTX_SFD            RG_BBC0_FSKPHRTX, FSKPHRTX_SFD_MASK, \
	FSKPHRTX_SFD_SHIFT

/** Address for register BBC0_FSKPHRRX */
#define RG_BBC0_FSKPHRRX                (0x36B)
/** Sub-registers of Register @ref RG_BBC0_FSKPHRRX */
/** Access parameters for sub-register RB1 in register @ref RG_BBC0_FSKPHRRX */
#define SR_BBC0_FSKPHRRX_RB1            RG_BBC0_FSKPHRRX, FSKPHRRX_RB1_MASK, \
	FSKPHRRX_RB1_SHIFT
/** Access parameters for sub-register RB2 in register @ref RG_BBC0_FSKPHRRX */
#define SR_BBC0_FSKPHRRX_RB2            RG_BBC0_FSKPHRRX, FSKPHRRX_RB2_MASK, \
	FSKPHRRX_RB2_SHIFT
/** Access parameters for sub-register DW in register @ref RG_BBC0_FSKPHRRX */
#define SR_BBC0_FSKPHRRX_DW             RG_BBC0_FSKPHRRX, FSKPHRRX_DW_MASK, \
	FSKPHRRX_DW_SHIFT
/** Access parameters for sub-register SFD in register @ref RG_BBC0_FSKPHRRX */
#define SR_BBC0_FSKPHRRX_SFD            RG_BBC0_FSKPHRRX, FSKPHRRX_SFD_MASK, \
	FSKPHRRX_SFD_SHIFT
/** Access parameters for sub-register MS in register @ref RG_BBC0_FSKPHRRX */
#define SR_BBC0_FSKPHRRX_MS             RG_BBC0_FSKPHRRX, FSKPHRRX_MS_MASK, \
	FSKPHRRX_MS_SHIFT
/** Access parameters for sub-register FCST in register @ref RG_BBC0_FSKPHRRX */
#define SR_BBC0_FSKPHRRX_FCST           RG_BBC0_FSKPHRRX, FSKPHRRX_FCST_MASK, \
	FSKPHRRX_FCST_SHIFT

/** Address for register BBC0_FSKRPC */
#define RG_BBC0_FSKRPC                  (0x36C)
/** Sub-registers of Register @ref RG_BBC0_FSKRPC */
/** Access parameters for sub-register BASET in register @ref RG_BBC0_FSKRPC */
#define SR_BBC0_FSKRPC_BASET            RG_BBC0_FSKRPC, FSKRPC_BASET_MASK, \
	FSKRPC_BASET_SHIFT
/** Access parameters for sub-register EN in register @ref RG_BBC0_FSKRPC */
#define SR_BBC0_FSKRPC_EN               RG_BBC0_FSKRPC, FSKRPC_EN_MASK,	\
	FSKRPC_EN_SHIFT

/** Address for register BBC0_FSKRPCONT */
#define RG_BBC0_FSKRPCONT               (0x36D)
/** Sub-registers of Register @ref RG_BBC0_FSKRPCONT */

/** Access parameters for sub-register FSKRPCONT in register @ref
 * RG_BBC0_FSKRPCONT */
#define SR_BBC0_FSKRPCONT_FSKRPCONT     RG_BBC0_FSKRPCONT, \
	FSKRPCONT_FSKRPCONT_MASK, FSKRPCONT_FSKRPCONT_SHIFT

/** Address for register BBC0_FSKRPCOFFT */
#define RG_BBC0_FSKRPCOFFT              (0x36E)
/** Sub-registers of Register @ref RG_BBC0_FSKRPCOFFT */

/** Access parameters for sub-register FSKRPCOFFT in register @ref
 * RG_BBC0_FSKRPCOFFT */
#define SR_BBC0_FSKRPCOFFT_FSKRPCOFFT   RG_BBC0_FSKRPCOFFT, \
	FSKRPCOFFT_FSKRPCOFFT_MASK, FSKRPCOFFT_FSKRPCOFFT_SHIFT

/** Address for register BBC0_FSKRRXFLL */
#define RG_BBC0_FSKRRXFLL               (0x370)
/** Sub-registers of Register @ref RG_BBC0_FSKRRXFLL */

/** Access parameters for sub-register FSKRRXFLL in register @ref
 * RG_BBC0_FSKRRXFLL */
#define SR_BBC0_FSKRRXFLL_FSKRRXFLL     RG_BBC0_FSKRRXFLL, \
	FSKRRXFLL_FSKRRXFLL_MASK, FSKRRXFLL_FSKRRXFLL_SHIFT

/** Address for register BBC0_FSKRRXFLH */
#define RG_BBC0_FSKRRXFLH               (0x371)
/** Sub-registers of Register @ref RG_BBC0_FSKRRXFLH */
/** Access parameters for sub-register FSKRRXFLH in register @ref
 *RG_BBC0_FSKRRXFLH */
#define SR_BBC0_FSKRRXFLH_FSKRRXFLH     RG_BBC0_FSKRRXFLH, \
	FSKRRXFLH_FSKRRXFLH_MASK, FSKRRXFLH_FSKRRXFLH_SHIFT

/** Address for register BBC0_FSKDM */
#define RG_BBC0_FSKDM                   (0x372)
/** Sub-registers of Register @ref RG_BBC0_FSKDM */
/** Access parameters for sub-register EN in register @ref RG_BBC0_FSKDM */
#define SR_BBC0_FSKDM_EN                RG_BBC0_FSKDM, FSKDM_EN_MASK, \
	FSKDM_EN_SHIFT
/** Access parameters for sub-register PE in register @ref RG_BBC0_FSKDM */
#define SR_BBC0_FSKDM_PE                RG_BBC0_FSKDM, FSKDM_PE_MASK, \
	FSKDM_PE_SHIFT

/** Address for register BBC0_FSKPE0 */
#define RG_BBC0_FSKPE0                  (0x373)
/** Sub-registers of Register @ref RG_BBC0_FSKPE0 */
/** Access parameters for sub-register FSKPE0 in register @ref RG_BBC0_FSKPE0 */
#define SR_BBC0_FSKPE0_FSKPE0           RG_BBC0_FSKPE0, FSKPE0_FSKPE0_MASK, \
	FSKPE0_FSKPE0_SHIFT

/** Address for register BBC0_FSKPE1 */
#define RG_BBC0_FSKPE1                  (0x374)
/** Sub-registers of Register @ref RG_BBC0_FSKPE1 */
/** Access parameters for sub-register FSKPE1 in register @ref RG_BBC0_FSKPE1 */
#define SR_BBC0_FSKPE1_FSKPE1           RG_BBC0_FSKPE1, FSKPE1_FSKPE1_MASK, \
	FSKPE1_FSKPE1_SHIFT

/** Address for register BBC0_FSKPE2 */
#define RG_BBC0_FSKPE2                  (0x375)
/** Sub-registers of Register @ref RG_BBC0_FSKPE2 */
/** Access parameters for sub-register FSKPE2 in register @ref RG_BBC0_FSKPE2 */
#define SR_BBC0_FSKPE2_FSKPE2           RG_BBC0_FSKPE2, FSKPE2_FSKPE2_MASK, \
	FSKPE2_FSKPE2_SHIFT

/** Address for register BBC1_FBRXE */
#define RG_BBC1_FBRXE                   (0x37FE)
/** Sub-registers of Register @ref RG_BBC1_FBRXE */
/** Access parameters for sub-register FBRXE in register @ref RG_BBC1_FBRXE */
#define SR_BBC1_FBRXE_FBRXE             RG_BBC1_FBRXE, FBRXE_FBRXE_MASK, \
	FBRXE_FBRXE_SHIFT

/** Address for register BBC0_PMUC */
#define RG_BBC0_PMUC                    (0x380)
/** Sub-registers of Register @ref RG_BBC0_PMUC */
/** Access parameters for sub-register EN in register @ref RG_BBC0_PMUC */
#define SR_BBC0_PMUC_EN                 RG_BBC0_PMUC, PMUC_EN_MASK, \
	PMUC_EN_SHIFT
/** Access parameters for sub-register AVG in register @ref RG_BBC0_PMUC */
#define SR_BBC0_PMUC_AVG                RG_BBC0_PMUC, PMUC_AVG_MASK, \
	PMUC_AVG_SHIFT
/** Access parameters for sub-register SYNC in register @ref RG_BBC0_PMUC */
#define SR_BBC0_PMUC_SYNC               RG_BBC0_PMUC, PMUC_SYNC_MASK, \
	PMUC_SYNC_SHIFT
/** Access parameters for sub-register FED in register @ref RG_BBC0_PMUC */
#define SR_BBC0_PMUC_FED                RG_BBC0_PMUC, PMUC_FED_MASK, \
	PMUC_FED_SHIFT
/** Access parameters for sub-register IQSEL in register @ref RG_BBC0_PMUC */
#define SR_BBC0_PMUC_IQSEL              RG_BBC0_PMUC, PMUC_IQSEL_MASK, \
	PMUC_IQSEL_SHIFT
/** Access parameters for sub-register CCFTS in register @ref RG_BBC0_PMUC */
#define SR_BBC0_PMUC_CCFTS              RG_BBC0_PMUC, PMUC_CCFTS_MASK, \
	PMUC_CCFTS_SHIFT

/** Address for register BBC1_FBTXS */
#define RG_BBC1_FBTXS                   (0x3800)
/** Sub-registers of Register @ref RG_BBC1_FBTXS */
/** Access parameters for sub-register FBTXS in register @ref RG_BBC1_FBTXS */
#define SR_BBC1_FBTXS_FBTXS             RG_BBC1_FBTXS, FBTXS_FBTXS_MASK, \
	FBTXS_FBTXS_SHIFT

/** Address for register BBC0_PMUVAL */
#define RG_BBC0_PMUVAL                  (0x381)
/** Sub-registers of Register @ref RG_BBC0_PMUVAL */
/** Access parameters for sub-register PMUVAL in register @ref RG_BBC0_PMUVAL */
#define SR_BBC0_PMUVAL_PMUVAL           RG_BBC0_PMUVAL, PMUVAL_PMUVAL_MASK, \
	PMUVAL_PMUVAL_SHIFT

/** Address for register BBC0_PMUQF */
#define RG_BBC0_PMUQF                   (0x382)
/** Sub-registers of Register @ref RG_BBC0_PMUQF */
/** Access parameters for sub-register PMUQF in register @ref RG_BBC0_PMUQF */
#define SR_BBC0_PMUQF_PMUQF             RG_BBC0_PMUQF, PMUQF_PMUQF_MASK, \
	PMUQF_PMUQF_SHIFT

/** Address for register BBC0_PMUI */
#define RG_BBC0_PMUI                    (0x383)
/** Sub-registers of Register @ref RG_BBC0_PMUI */
/** Access parameters for sub-register PMUI in register @ref RG_BBC0_PMUI */
#define SR_BBC0_PMUI_PMUI               RG_BBC0_PMUI, PMUI_PMUI_MASK, \
	PMUI_PMUI_SHIFT

/** Address for register BBC0_PMUQ */
#define RG_BBC0_PMUQ                    (0x384)
/** Sub-registers of Register @ref RG_BBC0_PMUQ */
/** Access parameters for sub-register PMUQ in register @ref RG_BBC0_PMUQ */
#define SR_BBC0_PMUQ_PMUQ               RG_BBC0_PMUQ, PMUQ_PMUQ_MASK, \
	PMUQ_PMUQ_SHIFT

/** Address for register BBC0_CNTC */
#define RG_BBC0_CNTC                    (0x390)
/** Sub-registers of Register @ref RG_BBC0_CNTC */
/** Access parameters for sub-register EN in register @ref RG_BBC0_CNTC */
#define SR_BBC0_CNTC_EN                 RG_BBC0_CNTC, CNTC_EN_MASK, \
	CNTC_EN_SHIFT
/** Access parameters for sub-register RSTRXS in register @ref RG_BBC0_CNTC */
#define SR_BBC0_CNTC_RSTRXS             RG_BBC0_CNTC, CNTC_RSTRXS_MASK,	\
	CNTC_RSTRXS_SHIFT
/** Access parameters for sub-register RSTTXS in register @ref RG_BBC0_CNTC */
#define SR_BBC0_CNTC_RSTTXS             RG_BBC0_CNTC, CNTC_RSTTXS_MASK,	\
	CNTC_RSTTXS_SHIFT
/** Access parameters for sub-register CAPRXS in register @ref RG_BBC0_CNTC */
#define SR_BBC0_CNTC_CAPRXS             RG_BBC0_CNTC, CNTC_CAPRXS_MASK,	\
	CNTC_CAPRXS_SHIFT
/** Access parameters for sub-register CAPTXS in register @ref RG_BBC0_CNTC */
#define SR_BBC0_CNTC_CAPTXS             RG_BBC0_CNTC, CNTC_CAPTXS_MASK,	\
	CNTC_CAPTXS_SHIFT

/** Address for register BBC0_CNT0 */
#define RG_BBC0_CNT0                    (0x391)
/** Sub-registers of Register @ref RG_BBC0_CNT0 */
/** Access parameters for sub-register CNT0 in register @ref RG_BBC0_CNT0 */
#define SR_BBC0_CNT0_CNT0               RG_BBC0_CNT0, CNT0_CNT0_MASK, \
	CNT0_CNT0_SHIFT

/** Address for register BBC0_CNT1 */
#define RG_BBC0_CNT1                    (0x392)
/** Sub-registers of Register @ref RG_BBC0_CNT1 */
/** Access parameters for sub-register CNT1 in register @ref RG_BBC0_CNT1 */
#define SR_BBC0_CNT1_CNT1               RG_BBC0_CNT1, CNT1_CNT1_MASK, \
	CNT1_CNT1_SHIFT

/** Address for register BBC0_CNT2 */
#define RG_BBC0_CNT2                    (0x393)
/** Sub-registers of Register @ref RG_BBC0_CNT2 */
/** Access parameters for sub-register CNT2 in register @ref RG_BBC0_CNT2 */
#define SR_BBC0_CNT2_CNT2               RG_BBC0_CNT2, CNT2_CNT2_MASK, \
	CNT2_CNT2_SHIFT

/** Address for register BBC0_CNT3 */
#define RG_BBC0_CNT3                    (0x394)
/** Sub-registers of Register @ref RG_BBC0_CNT3 */
/** Access parameters for sub-register CNT3 in register @ref RG_BBC0_CNT3 */
#define SR_BBC0_CNT3_CNT3               RG_BBC0_CNT3, CNT3_CNT3_MASK, \
	CNT3_CNT3_SHIFT

/** Address for register BBC1_FBTXE */
#define RG_BBC1_FBTXE                   (0x3FFE)
/** Sub-registers of Register @ref RG_BBC1_FBTXE */
/** Access parameters for sub-register FBTXE in register @ref RG_BBC1_FBTXE */
#define SR_BBC1_FBTXE_FBTXE             RG_BBC1_FBTXE, FBTXE_FBTXE_MASK, \
	FBTXE_FBTXE_SHIFT

/** Address for register BBC1_IRQM */
#define RG_BBC1_IRQM                    (0x400)
/** Sub-registers of Register @ref RG_BBC1_IRQM */
/** Access parameters for sub-register RXFS in register @ref RG_BBC1_IRQM */
#define SR_BBC1_IRQM_RXFS               RG_BBC1_IRQM, IRQM_RXFS_MASK, \
	IRQM_RXFS_SHIFT
/** Access parameters for sub-register RXFE in register @ref RG_BBC1_IRQM */
#define SR_BBC1_IRQM_RXFE               RG_BBC1_IRQM, IRQM_RXFE_MASK, \
	IRQM_RXFE_SHIFT
/** Access parameters for sub-register RXAM in register @ref RG_BBC1_IRQM */
#define SR_BBC1_IRQM_RXAM               RG_BBC1_IRQM, IRQM_RXAM_MASK, \
	IRQM_RXAM_SHIFT
/** Access parameters for sub-register RXEM in register @ref RG_BBC1_IRQM */
#define SR_BBC1_IRQM_RXEM               RG_BBC1_IRQM, IRQM_RXEM_MASK, \
	IRQM_RXEM_SHIFT
/** Access parameters for sub-register TXFE in register @ref RG_BBC1_IRQM */
#define SR_BBC1_IRQM_TXFE               RG_BBC1_IRQM, IRQM_TXFE_MASK, \
	IRQM_TXFE_SHIFT
/** Access parameters for sub-register AGCH in register @ref RG_BBC1_IRQM */
#define SR_BBC1_IRQM_AGCH               RG_BBC1_IRQM, IRQM_AGCH_MASK, \
	IRQM_AGCH_SHIFT
/** Access parameters for sub-register AGCR in register @ref RG_BBC1_IRQM */
#define SR_BBC1_IRQM_AGCR               RG_BBC1_IRQM, IRQM_AGCR_MASK, \
	IRQM_AGCR_SHIFT
/** Access parameters for sub-register FBLI in register @ref RG_BBC1_IRQM */
#define SR_BBC1_IRQM_FBLI               RG_BBC1_IRQM, IRQM_FBLI_MASK, \
	IRQM_FBLI_SHIFT

/** Address for register BBC1_PC */
#define RG_BBC1_PC                      (0x401)
/** Sub-registers of Register @ref RG_BBC1_PC */
/** Access parameters for sub-register PT in register @ref RG_BBC1_PC */
#define SR_BBC1_PC_PT                   RG_BBC1_PC, PC_PT_MASK, PC_PT_SHIFT
/** Access parameters for sub-register BBEN in register @ref RG_BBC1_PC */
#define SR_BBC1_PC_BBEN                 RG_BBC1_PC, PC_BBEN_MASK, PC_BBEN_SHIFT
/** Access parameters for sub-register FCST in register @ref RG_BBC1_PC */
#define SR_BBC1_PC_FCST                 RG_BBC1_PC, PC_FCST_MASK, PC_FCST_SHIFT
/** Access parameters for sub-register TXAFCS in register @ref RG_BBC1_PC */
#define SR_BBC1_PC_TXAFCS               RG_BBC1_PC, PC_TXAFCS_MASK, \
	PC_TXAFCS_SHIFT
/** Access parameters for sub-register FCSOK in register @ref RG_BBC1_PC */
#define SR_BBC1_PC_FCSOK                RG_BBC1_PC, PC_FCSOK_MASK, \
	PC_FCSOK_SHIFT
/** Access parameters for sub-register FCSFE in register @ref RG_BBC1_PC */
#define SR_BBC1_PC_FCSFE                RG_BBC1_PC, PC_FCSFE_MASK, \
	PC_FCSFE_SHIFT
/** Access parameters for sub-register CTX in register @ref RG_BBC1_PC */
#define SR_BBC1_PC_CTX                  RG_BBC1_PC, PC_CTX_MASK, PC_CTX_SHIFT

/** Address for register BBC1_PS */
#define RG_BBC1_PS                      (0x402)
/** Sub-registers of Register @ref RG_BBC1_PS */
/** Access parameters for sub-register TXUR in register @ref RG_BBC1_PS */
#define SR_BBC1_PS_TXUR                 RG_BBC1_PS, PS_TXUR_MASK, PS_TXUR_SHIFT

/** Address for register BBC1_RXFLL */
#define RG_BBC1_RXFLL                   (0x404)
/** Sub-registers of Register @ref RG_BBC1_RXFLL */
/** Access parameters for sub-register RXFLL in register @ref RG_BBC1_RXFLL */
#define SR_BBC1_RXFLL_RXFLL             RG_BBC1_RXFLL, RXFLL_RXFLL_MASK, \
	RXFLL_RXFLL_SHIFT

/** Address for register BBC1_RXFLH */
#define RG_BBC1_RXFLH                   (0x405)
/** Sub-registers of Register @ref RG_BBC1_RXFLH */
/** Access parameters for sub-register RXFLH in register @ref RG_BBC1_RXFLH */
#define SR_BBC1_RXFLH_RXFLH             RG_BBC1_RXFLH, RXFLH_RXFLH_MASK, \
	RXFLH_RXFLH_SHIFT

/** Address for register BBC1_TXFLL */
#define RG_BBC1_TXFLL                   (0x406)
/** Sub-registers of Register @ref RG_BBC1_TXFLL */
/** Access parameters for sub-register TXFLL in register @ref RG_BBC1_TXFLL */
#define SR_BBC1_TXFLL_TXFLL             RG_BBC1_TXFLL, TXFLL_TXFLL_MASK, \
	TXFLL_TXFLL_SHIFT

/** Address for register BBC1_TXFLH */
#define RG_BBC1_TXFLH                   (0x407)
/** Sub-registers of Register @ref RG_BBC1_TXFLH */
/** Access parameters for sub-register TXFLH in register @ref RG_BBC1_TXFLH */
#define SR_BBC1_TXFLH_TXFLH             RG_BBC1_TXFLH, TXFLH_TXFLH_MASK, \
	TXFLH_TXFLH_SHIFT

/** Address for register BBC1_FBLL */
#define RG_BBC1_FBLL                    (0x408)
/** Sub-registers of Register @ref RG_BBC1_FBLL */
/** Access parameters for sub-register FBLL in register @ref RG_BBC1_FBLL */
#define SR_BBC1_FBLL_FBLL               RG_BBC1_FBLL, FBLL_FBLL_MASK, \
	FBLL_FBLL_SHIFT

/** Address for register BBC1_FBLH */
#define RG_BBC1_FBLH                    (0x409)
/** Sub-registers of Register @ref RG_BBC1_FBLH */
/** Access parameters for sub-register FBLH in register @ref RG_BBC1_FBLH */
#define SR_BBC1_FBLH_FBLH               RG_BBC1_FBLH, FBLH_FBLH_MASK, \
	FBLH_FBLH_SHIFT

/** Address for register BBC1_FBLIL */
#define RG_BBC1_FBLIL                   (0x40A)
/** Sub-registers of Register @ref RG_BBC1_FBLIL */
/** Access parameters for sub-register FBLIL in register @ref RG_BBC1_FBLIL */
#define SR_BBC1_FBLIL_FBLIL             RG_BBC1_FBLIL, FBLIL_FBLIL_MASK, \
	FBLIL_FBLIL_SHIFT

/** Address for register BBC1_FBLIH */
#define RG_BBC1_FBLIH                   (0x40B)
/** Sub-registers of Register @ref RG_BBC1_FBLIH */
/** Access parameters for sub-register FBLIH in register @ref RG_BBC1_FBLIH */
#define SR_BBC1_FBLIH_FBLIH             RG_BBC1_FBLIH, FBLIH_FBLIH_MASK, \
	FBLIH_FBLIH_SHIFT

/** Address for register BBC1_OFDMPHRTX */
#define RG_BBC1_OFDMPHRTX               (0x40C)
/** Sub-registers of Register @ref RG_BBC1_OFDMPHRTX */
/** Access parameters for sub-register MCS in register @ref RG_BBC1_OFDMPHRTX */
#define SR_BBC1_OFDMPHRTX_MCS           RG_BBC1_OFDMPHRTX, OFDMPHRTX_MCS_MASK, \
	OFDMPHRTX_MCS_SHIFT
/** Access parameters for sub-register RB5 in register @ref RG_BBC1_OFDMPHRTX */
#define SR_BBC1_OFDMPHRTX_RB5           RG_BBC1_OFDMPHRTX, OFDMPHRTX_RB5_MASK, \
	OFDMPHRTX_RB5_SHIFT

/** Access parameters for sub-register RB17 in register @ref RG_BBC1_OFDMPHRTX
**/
#define SR_BBC1_OFDMPHRTX_RB17          RG_BBC1_OFDMPHRTX, OFDMPHRTX_RB17_MASK,	\
	OFDMPHRTX_RB17_SHIFT

/** Access parameters for sub-register RB18 in register @ref RG_BBC1_OFDMPHRTX
**/
#define SR_BBC1_OFDMPHRTX_RB18          RG_BBC1_OFDMPHRTX, OFDMPHRTX_RB18_MASK,	\
	OFDMPHRTX_RB18_SHIFT

/** Access parameters for sub-register RB21 in register @ref RG_BBC1_OFDMPHRTX
**/
#define SR_BBC1_OFDMPHRTX_RB21          RG_BBC1_OFDMPHRTX, OFDMPHRTX_RB21_MASK,	\
	OFDMPHRTX_RB21_SHIFT

/** Address for register BBC1_OFDMPHRRX */
#define RG_BBC1_OFDMPHRRX               (0x40D)
/** Sub-registers of Register @ref RG_BBC1_OFDMPHRRX */
/** Access parameters for sub-register MCS in register @ref RG_BBC1_OFDMPHRRX */
#define SR_BBC1_OFDMPHRRX_MCS           RG_BBC1_OFDMPHRRX, OFDMPHRRX_MCS_MASK, \
	OFDMPHRRX_MCS_SHIFT
/** Access parameters for sub-register SPC in register @ref RG_BBC1_OFDMPHRRX */
#define SR_BBC1_OFDMPHRRX_SPC           RG_BBC1_OFDMPHRRX, OFDMPHRRX_SPC_MASK, \
	OFDMPHRRX_SPC_SHIFT
/** Access parameters for sub-register RB5 in register @ref RG_BBC1_OFDMPHRRX */
#define SR_BBC1_OFDMPHRRX_RB5           RG_BBC1_OFDMPHRRX, OFDMPHRRX_RB5_MASK, \
	OFDMPHRRX_RB5_SHIFT

/** Access parameters for sub-register RB17 in register @ref RG_BBC1_OFDMPHRRX
**/
#define SR_BBC1_OFDMPHRRX_RB17          RG_BBC1_OFDMPHRRX, OFDMPHRRX_RB17_MASK,	\
	OFDMPHRRX_RB17_SHIFT

/** Access parameters for sub-register RB18 in register @ref RG_BBC1_OFDMPHRRX
**/
#define SR_BBC1_OFDMPHRRX_RB18          RG_BBC1_OFDMPHRRX, OFDMPHRRX_RB18_MASK,	\
	OFDMPHRRX_RB18_SHIFT

/** Access parameters for sub-register RB21 in register @ref RG_BBC1_OFDMPHRRX
**/
#define SR_BBC1_OFDMPHRRX_RB21          RG_BBC1_OFDMPHRRX, OFDMPHRRX_RB21_MASK,	\
	OFDMPHRRX_RB21_SHIFT

/** Address for register BBC1_OFDMC */
#define RG_BBC1_OFDMC                   (0x40E)
/** Sub-registers of Register @ref RG_BBC1_OFDMC */
/** Access parameters for sub-register OPT in register @ref RG_BBC1_OFDMC */
#define SR_BBC1_OFDMC_OPT               RG_BBC1_OFDMC, OFDMC_OPT_MASK, \
	OFDMC_OPT_SHIFT
/** Access parameters for sub-register POI in register @ref RG_BBC1_OFDMC */
#define SR_BBC1_OFDMC_POI               RG_BBC1_OFDMC, OFDMC_POI_MASK, \
	OFDMC_POI_SHIFT
/** Access parameters for sub-register LFO in register @ref RG_BBC1_OFDMC */
#define SR_BBC1_OFDMC_LFO               RG_BBC1_OFDMC, OFDMC_LFO_MASK, \
	OFDMC_LFO_SHIFT
/** Access parameters for sub-register SSTX in register @ref RG_BBC1_OFDMC */
#define SR_BBC1_OFDMC_SSTX              RG_BBC1_OFDMC, OFDMC_SSTX_MASK,	\
	OFDMC_SSTX_SHIFT
/** Access parameters for sub-register SSRX in register @ref RG_BBC1_OFDMC */
#define SR_BBC1_OFDMC_SSRX              RG_BBC1_OFDMC, OFDMC_SSRX_MASK,	\
	OFDMC_SSRX_SHIFT

/** Address for register BBC1_OFDMSW */
#define RG_BBC1_OFDMSW                  (0x40F)
/** Sub-registers of Register @ref RG_BBC1_OFDMSW */
/** Access parameters for sub-register RXO in register @ref RG_BBC1_OFDMSW */
#define SR_BBC1_OFDMSW_RXO              RG_BBC1_OFDMSW, OFDMSW_RXO_MASK, \
	OFDMSW_RXO_SHIFT
/** Access parameters for sub-register PDT in register @ref RG_BBC1_OFDMSW */
#define SR_BBC1_OFDMSW_PDT              RG_BBC1_OFDMSW, OFDMSW_PDT_MASK, \
	OFDMSW_PDT_SHIFT

/** Address for register BBC1_OQPSKC0 */
#define RG_BBC1_OQPSKC0                 (0x410)
/** Sub-registers of Register @ref RG_BBC1_OQPSKC0 */
/** Access parameters for sub-register FCHIP in register @ref RG_BBC1_OQPSKC0 */
#define SR_BBC1_OQPSKC0_FCHIP           RG_BBC1_OQPSKC0, OQPSKC0_FCHIP_MASK, \
	OQPSKC0_FCHIP_SHIFT
/** Access parameters for sub-register MOD in register @ref RG_BBC1_OQPSKC0 */
#define SR_BBC1_OQPSKC0_MOD             RG_BBC1_OQPSKC0, OQPSKC0_MOD_MASK, \
	OQPSKC0_MOD_SHIFT
/** Access parameters for sub-register DM in register @ref RG_BBC1_OQPSKC0 */
#define SR_BBC1_OQPSKC0_DM              RG_BBC1_OQPSKC0, OQPSKC0_DM_MASK, \
	OQPSKC0_DM_SHIFT

/** Address for register BBC1_OQPSKC1 */
#define RG_BBC1_OQPSKC1                 (0x411)
/** Sub-registers of Register @ref RG_BBC1_OQPSKC1 */
/** Access parameters for sub-register PDT0 in register @ref RG_BBC1_OQPSKC1 */
#define SR_BBC1_OQPSKC1_PDT0            RG_BBC1_OQPSKC1, OQPSKC1_PDT0_MASK, \
	OQPSKC1_PDT0_SHIFT
/** Access parameters for sub-register PDT1 in register @ref RG_BBC1_OQPSKC1 */
#define SR_BBC1_OQPSKC1_PDT1            RG_BBC1_OQPSKC1, OQPSKC1_PDT1_MASK, \
	OQPSKC1_PDT1_SHIFT

/** Access parameters for sub-register RXOLEG in register @ref RG_BBC1_OQPSKC1
**/
#define SR_BBC1_OQPSKC1_RXOLEG          RG_BBC1_OQPSKC1, OQPSKC1_RXOLEG_MASK, \
	OQPSKC1_RXOLEG_SHIFT
/** Access parameters for sub-register RXO in register @ref RG_BBC1_OQPSKC1 */
#define SR_BBC1_OQPSKC1_RXO             RG_BBC1_OQPSKC1, OQPSKC1_RXO_MASK, \
	OQPSKC1_RXO_SHIFT

/** Address for register BBC1_OQPSKC2 */
#define RG_BBC1_OQPSKC2                 (0x412)
/** Sub-registers of Register @ref RG_BBC1_OQPSKC2 */
/** Access parameters for sub-register RXM in register @ref RG_BBC1_OQPSKC2 */
#define SR_BBC1_OQPSKC2_RXM             RG_BBC1_OQPSKC2, OQPSKC2_RXM_MASK, \
	OQPSKC2_RXM_SHIFT

/** Access parameters for sub-register FCSTLEG in register @ref RG_BBC1_OQPSKC2
**/
#define SR_BBC1_OQPSKC2_FCSTLEG         RG_BBC1_OQPSKC2, OQPSKC2_FCSTLEG_MASK, \
	OQPSKC2_FCSTLEG_SHIFT

/** Access parameters for sub-register ENPROP in register @ref RG_BBC1_OQPSKC2
**/
#define SR_BBC1_OQPSKC2_ENPROP          RG_BBC1_OQPSKC2, OQPSKC2_ENPROP_MASK, \
	OQPSKC2_ENPROP_SHIFT
/** Access parameters for sub-register RPC in register @ref RG_BBC1_OQPSKC2 */
#define SR_BBC1_OQPSKC2_RPC             RG_BBC1_OQPSKC2, OQPSKC2_RPC_MASK, \
	OQPSKC2_RPC_SHIFT
/** Access parameters for sub-register SPC in register @ref RG_BBC1_OQPSKC2 */
#define SR_BBC1_OQPSKC2_SPC             RG_BBC1_OQPSKC2, OQPSKC2_SPC_MASK, \
	OQPSKC2_SPC_SHIFT

/** Address for register BBC1_OQPSKC3 */
#define RG_BBC1_OQPSKC3                 (0x413)
/** Sub-registers of Register @ref RG_BBC1_OQPSKC3 */
/** Access parameters for sub-register NSFD in register @ref RG_BBC1_OQPSKC3 */
#define SR_BBC1_OQPSKC3_NSFD            RG_BBC1_OQPSKC3, OQPSKC3_NSFD_MASK, \
	OQPSKC3_NSFD_SHIFT
/** Access parameters for sub-register HRLEG in register @ref RG_BBC1_OQPSKC3 */
#define SR_BBC1_OQPSKC3_HRLEG           RG_BBC1_OQPSKC3, OQPSKC3_HRLEG_MASK, \
	OQPSKC3_HRLEG_SHIFT

/** Address for register BBC1_OQPSKPHRTX */
#define RG_BBC1_OQPSKPHRTX              (0x414)
/** Sub-registers of Register @ref RG_BBC1_OQPSKPHRTX */

/** Access parameters for sub-register LEG in register @ref RG_BBC1_OQPSKPHRTX
**/
#define SR_BBC1_OQPSKPHRTX_LEG          RG_BBC1_OQPSKPHRTX, OQPSKPHRTX_LEG_MASK, \
	OQPSKPHRTX_LEG_SHIFT

/** Access parameters for sub-register MOD in register @ref RG_BBC1_OQPSKPHRTX
**/
#define SR_BBC1_OQPSKPHRTX_MOD          RG_BBC1_OQPSKPHRTX, OQPSKPHRTX_MOD_MASK, \
	OQPSKPHRTX_MOD_SHIFT

/** Access parameters for sub-register RB0 in register @ref RG_BBC1_OQPSKPHRTX
**/
#define SR_BBC1_OQPSKPHRTX_RB0          RG_BBC1_OQPSKPHRTX, OQPSKPHRTX_RB0_MASK, \
	OQPSKPHRTX_RB0_SHIFT

/** Access parameters for sub-register PPDUT in register @ref RG_BBC1_OQPSKPHRTX
**/
#define SR_BBC1_OQPSKPHRTX_PPDUT        RG_BBC1_OQPSKPHRTX, \
	OQPSKPHRTX_PPDUT_MASK, OQPSKPHRTX_PPDUT_SHIFT

/** Address for register BBC1_OQPSKPHRRX */
#define RG_BBC1_OQPSKPHRRX              (0x415)
/** Sub-registers of Register @ref RG_BBC1_OQPSKPHRRX */

/** Access parameters for sub-register LEG in register @ref RG_BBC1_OQPSKPHRRX
**/
#define SR_BBC1_OQPSKPHRRX_LEG          RG_BBC1_OQPSKPHRRX, OQPSKPHRRX_LEG_MASK, \
	OQPSKPHRRX_LEG_SHIFT

/** Access parameters for sub-register MOD in register @ref RG_BBC1_OQPSKPHRRX
**/
#define SR_BBC1_OQPSKPHRRX_MOD          RG_BBC1_OQPSKPHRRX, OQPSKPHRRX_MOD_MASK, \
	OQPSKPHRRX_MOD_SHIFT

/** Access parameters for sub-register RB0 in register @ref RG_BBC1_OQPSKPHRRX
**/
#define SR_BBC1_OQPSKPHRRX_RB0          RG_BBC1_OQPSKPHRRX, OQPSKPHRRX_RB0_MASK, \
	OQPSKPHRRX_RB0_SHIFT

/** Access parameters for sub-register PPDUT in register @ref RG_BBC1_OQPSKPHRRX
**/
#define SR_BBC1_OQPSKPHRRX_PPDUT        RG_BBC1_OQPSKPHRRX, \
	OQPSKPHRRX_PPDUT_MASK, OQPSKPHRRX_PPDUT_SHIFT

/** Address for register BBC1_AFC0 */
#define RG_BBC1_AFC0                    (0x420)
/** Sub-registers of Register @ref RG_BBC1_AFC0 */
/** Access parameters for sub-register AFEN0 in register @ref RG_BBC1_AFC0 */
#define SR_BBC1_AFC0_AFEN0              RG_BBC1_AFC0, AFC0_AFEN0_MASK, \
	AFC0_AFEN0_SHIFT
/** Access parameters for sub-register AFEN1 in register @ref RG_BBC1_AFC0 */
#define SR_BBC1_AFC0_AFEN1              RG_BBC1_AFC0, AFC0_AFEN1_MASK, \
	AFC0_AFEN1_SHIFT
/** Access parameters for sub-register AFEN2 in register @ref RG_BBC1_AFC0 */
#define SR_BBC1_AFC0_AFEN2              RG_BBC1_AFC0, AFC0_AFEN2_MASK, \
	AFC0_AFEN2_SHIFT
/** Access parameters for sub-register AFEN3 in register @ref RG_BBC1_AFC0 */
#define SR_BBC1_AFC0_AFEN3              RG_BBC1_AFC0, AFC0_AFEN3_MASK, \
	AFC0_AFEN3_SHIFT
/** Access parameters for sub-register PM in register @ref RG_BBC1_AFC0 */
#define SR_BBC1_AFC0_PM                 RG_BBC1_AFC0, AFC0_PM_MASK, \
	AFC0_PM_SHIFT

/** Address for register BBC1_AFC1 */
#define RG_BBC1_AFC1                    (0x421)
/** Sub-registers of Register @ref RG_BBC1_AFC1 */
/** Access parameters for sub-register PANC0 in register @ref RG_BBC1_AFC1 */
#define SR_BBC1_AFC1_PANC0              RG_BBC1_AFC1, AFC1_PANC0_MASK, \
	AFC1_PANC0_SHIFT
/** Access parameters for sub-register PANC1 in register @ref RG_BBC1_AFC1 */
#define SR_BBC1_AFC1_PANC1              RG_BBC1_AFC1, AFC1_PANC1_MASK, \
	AFC1_PANC1_SHIFT
/** Access parameters for sub-register PANC2 in register @ref RG_BBC1_AFC1 */
#define SR_BBC1_AFC1_PANC2              RG_BBC1_AFC1, AFC1_PANC2_MASK, \
	AFC1_PANC2_SHIFT
/** Access parameters for sub-register PANC3 in register @ref RG_BBC1_AFC1 */
#define SR_BBC1_AFC1_PANC3              RG_BBC1_AFC1, AFC1_PANC3_MASK, \
	AFC1_PANC3_SHIFT
/** Access parameters for sub-register MRFT0 in register @ref RG_BBC1_AFC1 */
#define SR_BBC1_AFC1_MRFT0              RG_BBC1_AFC1, AFC1_MRFT0_MASK, \
	AFC1_MRFT0_SHIFT
/** Access parameters for sub-register MRFT1 in register @ref RG_BBC1_AFC1 */
#define SR_BBC1_AFC1_MRFT1              RG_BBC1_AFC1, AFC1_MRFT1_MASK, \
	AFC1_MRFT1_SHIFT
/** Access parameters for sub-register MRFT2 in register @ref RG_BBC1_AFC1 */
#define SR_BBC1_AFC1_MRFT2              RG_BBC1_AFC1, AFC1_MRFT2_MASK, \
	AFC1_MRFT2_SHIFT
/** Access parameters for sub-register MRFT3 in register @ref RG_BBC1_AFC1 */
#define SR_BBC1_AFC1_MRFT3              RG_BBC1_AFC1, AFC1_MRFT3_MASK, \
	AFC1_MRFT3_SHIFT

/** Address for register BBC1_AFFTM */
#define RG_BBC1_AFFTM                   (0x422)
/** Sub-registers of Register @ref RG_BBC1_AFFTM */
/** Access parameters for sub-register AFFTM in register @ref RG_BBC1_AFFTM */
#define SR_BBC1_AFFTM_AFFTM             RG_BBC1_AFFTM, AFFTM_AFFTM_MASK, \
	AFFTM_AFFTM_SHIFT

/** Address for register BBC1_AFFVM */
#define RG_BBC1_AFFVM                   (0x423)
/** Sub-registers of Register @ref RG_BBC1_AFFVM */
/** Access parameters for sub-register AFFVM in register @ref RG_BBC1_AFFVM */
#define SR_BBC1_AFFVM_AFFVM             RG_BBC1_AFFVM, AFFVM_AFFVM_MASK, \
	AFFVM_AFFVM_SHIFT

/** Address for register BBC1_AFS */
#define RG_BBC1_AFS                     (0x424)
/** Sub-registers of Register @ref RG_BBC1_AFS */
/** Access parameters for sub-register AM0 in register @ref RG_BBC1_AFS */
#define SR_BBC1_AFS_AM0                 RG_BBC1_AFS, AFS_AM0_MASK, AFS_AM0_SHIFT
/** Access parameters for sub-register AM1 in register @ref RG_BBC1_AFS */
#define SR_BBC1_AFS_AM1                 RG_BBC1_AFS, AFS_AM1_MASK, AFS_AM1_SHIFT
/** Access parameters for sub-register AM2 in register @ref RG_BBC1_AFS */
#define SR_BBC1_AFS_AM2                 RG_BBC1_AFS, AFS_AM2_MASK, AFS_AM2_SHIFT
/** Access parameters for sub-register AM3 in register @ref RG_BBC1_AFS */
#define SR_BBC1_AFS_AM3                 RG_BBC1_AFS, AFS_AM3_MASK, AFS_AM3_SHIFT
/** Access parameters for sub-register EM in register @ref RG_BBC1_AFS */
#define SR_BBC1_AFS_EM                  RG_BBC1_AFS, AFS_EM_MASK, AFS_EM_SHIFT

/** Address for register BBC1_MACEA0 */
#define RG_BBC1_MACEA0                  (0x425)
/** Sub-registers of Register @ref RG_BBC1_MACEA0 */
/** Access parameters for sub-register MACEA0 in register @ref RG_BBC1_MACEA0 */
#define SR_BBC1_MACEA0_MACEA0           RG_BBC1_MACEA0, MACEA0_MACEA0_MASK, \
	MACEA0_MACEA0_SHIFT

/** Address for register BBC1_MACEA1 */
#define RG_BBC1_MACEA1                  (0x426)
/** Sub-registers of Register @ref RG_BBC1_MACEA1 */
/** Access parameters for sub-register MACEA1 in register @ref RG_BBC1_MACEA1 */
#define SR_BBC1_MACEA1_MACEA1           RG_BBC1_MACEA1, MACEA1_MACEA1_MASK, \
	MACEA1_MACEA1_SHIFT

/** Address for register BBC1_MACEA2 */
#define RG_BBC1_MACEA2                  (0x427)
/** Sub-registers of Register @ref RG_BBC1_MACEA2 */
/** Access parameters for sub-register MACEA2 in register @ref RG_BBC1_MACEA2 */
#define SR_BBC1_MACEA2_MACEA2           RG_BBC1_MACEA2, MACEA2_MACEA2_MASK, \
	MACEA2_MACEA2_SHIFT

/** Address for register BBC1_MACEA3 */
#define RG_BBC1_MACEA3                  (0x428)
/** Sub-registers of Register @ref RG_BBC1_MACEA3 */
/** Access parameters for sub-register MACEA3 in register @ref RG_BBC1_MACEA3 */
#define SR_BBC1_MACEA3_MACEA3           RG_BBC1_MACEA3, MACEA3_MACEA3_MASK, \
	MACEA3_MACEA3_SHIFT

/** Address for register BBC1_MACEA4 */
#define RG_BBC1_MACEA4                  (0x429)
/** Sub-registers of Register @ref RG_BBC1_MACEA4 */
/** Access parameters for sub-register MACEA4 in register @ref RG_BBC1_MACEA4 */
#define SR_BBC1_MACEA4_MACEA4           RG_BBC1_MACEA4, MACEA4_MACEA4_MASK, \
	MACEA4_MACEA4_SHIFT

/** Address for register BBC1_MACEA5 */
#define RG_BBC1_MACEA5                  (0x42A)
/** Sub-registers of Register @ref RG_BBC1_MACEA5 */
/** Access parameters for sub-register MACEA5 in register @ref RG_BBC1_MACEA5 */
#define SR_BBC1_MACEA5_MACEA5           RG_BBC1_MACEA5, MACEA5_MACEA5_MASK, \
	MACEA5_MACEA5_SHIFT

/** Address for register BBC1_MACEA6 */
#define RG_BBC1_MACEA6                  (0x42B)
/** Sub-registers of Register @ref RG_BBC1_MACEA6 */
/** Access parameters for sub-register MACEA6 in register @ref RG_BBC1_MACEA6 */
#define SR_BBC1_MACEA6_MACEA6           RG_BBC1_MACEA6, MACEA6_MACEA6_MASK, \
	MACEA6_MACEA6_SHIFT

/** Address for register BBC1_MACEA7 */
#define RG_BBC1_MACEA7                  (0x42C)
/** Sub-registers of Register @ref RG_BBC1_MACEA7 */
/** Access parameters for sub-register MACEA7 in register @ref RG_BBC1_MACEA7 */
#define SR_BBC1_MACEA7_MACEA7           RG_BBC1_MACEA7, MACEA7_MACEA7_MASK, \
	MACEA7_MACEA7_SHIFT

/** Address for register BBC1_MACPID0F0 */
#define RG_BBC1_MACPID0F0               (0x42D)
/** Sub-registers of Register @ref RG_BBC1_MACPID0F0 */

/** Access parameters for sub-register MACPID0F0 in register @ref
 * RG_BBC1_MACPID0F0 */
#define SR_BBC1_MACPID0F0_MACPID0F0     RG_BBC1_MACPID0F0, \
	MACPID0F0_MACPID0F0_MASK, MACPID0F0_MACPID0F0_SHIFT

/** Address for register BBC1_MACPID1F0 */
#define RG_BBC1_MACPID1F0               (0x42E)
/** Sub-registers of Register @ref RG_BBC1_MACPID1F0 */

/** Access parameters for sub-register MACPID1F0 in register @ref
 * RG_BBC1_MACPID1F0 */
#define SR_BBC1_MACPID1F0_MACPID1F0     RG_BBC1_MACPID1F0, \
	MACPID1F0_MACPID1F0_MASK, MACPID1F0_MACPID1F0_SHIFT

/** Address for register BBC1_MACSHA0F0 */
#define RG_BBC1_MACSHA0F0               (0x42F)
/** Sub-registers of Register @ref RG_BBC1_MACSHA0F0 */

/** Access parameters for sub-register MACSHA0F0 in register @ref
 * RG_BBC1_MACSHA0F0 */
#define SR_BBC1_MACSHA0F0_MACSHA0F0     RG_BBC1_MACSHA0F0, \
	MACSHA0F0_MACSHA0F0_MASK, MACSHA0F0_MACSHA0F0_SHIFT

/** Address for register BBC1_MACSHA1F0 */
#define RG_BBC1_MACSHA1F0               (0x430)
/** Sub-registers of Register @ref RG_BBC1_MACSHA1F0 */

/** Access parameters for sub-register MACSHA1F0 in register @ref
 * RG_BBC1_MACSHA1F0 */
#define SR_BBC1_MACSHA1F0_MACSHA1F0     RG_BBC1_MACSHA1F0, \
	MACSHA1F0_MACSHA1F0_MASK, MACSHA1F0_MACSHA1F0_SHIFT

/** Address for register BBC1_MACPID0F1 */
#define RG_BBC1_MACPID0F1               (0x431)
/** Sub-registers of Register @ref RG_BBC1_MACPID0F1 */

/** Access parameters for sub-register MACPID0F1 in register @ref
 * RG_BBC1_MACPID0F1 */
#define SR_BBC1_MACPID0F1_MACPID0F1     RG_BBC1_MACPID0F1, \
	MACPID0F1_MACPID0F1_MASK, MACPID0F1_MACPID0F1_SHIFT

/** Address for register BBC1_MACPID1F1 */
#define RG_BBC1_MACPID1F1               (0x432)
/** Sub-registers of Register @ref RG_BBC1_MACPID1F1 */

/** Access parameters for sub-register MACPID1F1 in register @ref
 * RG_BBC1_MACPID1F1 */
#define SR_BBC1_MACPID1F1_MACPID1F1     RG_BBC1_MACPID1F1, \
	MACPID1F1_MACPID1F1_MASK, MACPID1F1_MACPID1F1_SHIFT

/** Address for register BBC1_MACSHA0F1 */
#define RG_BBC1_MACSHA0F1               (0x433)
/** Sub-registers of Register @ref RG_BBC1_MACSHA0F1 */

/** Access parameters for sub-register MACSHA0F1 in register @ref
 * RG_BBC1_MACSHA0F1 */
#define SR_BBC1_MACSHA0F1_MACSHA0F1     RG_BBC1_MACSHA0F1, \
	MACSHA0F1_MACSHA0F1_MASK, MACSHA0F1_MACSHA0F1_SHIFT

/** Address for register BBC1_MACSHA1F1 */
#define RG_BBC1_MACSHA1F1               (0x434)
/** Sub-registers of Register @ref RG_BBC1_MACSHA1F1 */

/** Access parameters for sub-register MACSHA1F1 in register @ref
 * RG_BBC1_MACSHA1F1 */
#define SR_BBC1_MACSHA1F1_MACSHA1F1     RG_BBC1_MACSHA1F1, \
	MACSHA1F1_MACSHA1F1_MASK, MACSHA1F1_MACSHA1F1_SHIFT

/** Address for register BBC1_MACPID0F2 */
#define RG_BBC1_MACPID0F2               (0x435)
/** Sub-registers of Register @ref RG_BBC1_MACPID0F2 */

/** Access parameters for sub-register MACPID0F2 in register @ref
 * RG_BBC1_MACPID0F2 */
#define SR_BBC1_MACPID0F2_MACPID0F2     RG_BBC1_MACPID0F2, \
	MACPID0F2_MACPID0F2_MASK, MACPID0F2_MACPID0F2_SHIFT

/** Address for register BBC1_MACPID1F2 */
#define RG_BBC1_MACPID1F2               (0x436)
/** Sub-registers of Register @ref RG_BBC1_MACPID1F2 */

/** Access parameters for sub-register MACPID1F2 in register @ref
 * RG_BBC1_MACPID1F2 */
#define SR_BBC1_MACPID1F2_MACPID1F2     RG_BBC1_MACPID1F2, \
	MACPID1F2_MACPID1F2_MASK, MACPID1F2_MACPID1F2_SHIFT

/** Address for register BBC1_MACSHA0F2 */
#define RG_BBC1_MACSHA0F2               (0x437)
/** Sub-registers of Register @ref RG_BBC1_MACSHA0F2 */

/** Access parameters for sub-register MACSHA0F2 in register @ref
 * RG_BBC1_MACSHA0F2 */
#define SR_BBC1_MACSHA0F2_MACSHA0F2     RG_BBC1_MACSHA0F2, \
	MACSHA0F2_MACSHA0F2_MASK, MACSHA0F2_MACSHA0F2_SHIFT

/** Address for register BBC1_MACSHA1F2 */
#define RG_BBC1_MACSHA1F2               (0x438)
/** Sub-registers of Register @ref RG_BBC1_MACSHA1F2 */

/** Access parameters for sub-register MACSHA1F2 in register @ref
 * RG_BBC1_MACSHA1F2 */
#define SR_BBC1_MACSHA1F2_MACSHA1F2     RG_BBC1_MACSHA1F2, \
	MACSHA1F2_MACSHA1F2_MASK, MACSHA1F2_MACSHA1F2_SHIFT

/** Address for register BBC1_MACPID0F3 */
#define RG_BBC1_MACPID0F3               (0x439)
/** Sub-registers of Register @ref RG_BBC1_MACPID0F3 */

/** Access parameters for sub-register MACPID0F3 in register @ref
 * RG_BBC1_MACPID0F3 */
#define SR_BBC1_MACPID0F3_MACPID0F3     RG_BBC1_MACPID0F3, \
	MACPID0F3_MACPID0F3_MASK, MACPID0F3_MACPID0F3_SHIFT

/** Address for register BBC1_MACPID1F3 */
#define RG_BBC1_MACPID1F3               (0x43A)
/** Sub-registers of Register @ref RG_BBC1_MACPID1F3 */

/** Access parameters for sub-register MACPID1F3 in register @ref
 * RG_BBC1_MACPID1F3 */
#define SR_BBC1_MACPID1F3_MACPID1F3     RG_BBC1_MACPID1F3, \
	MACPID1F3_MACPID1F3_MASK, MACPID1F3_MACPID1F3_SHIFT

/** Address for register BBC1_MACSHA0F3 */
#define RG_BBC1_MACSHA0F3               (0x43B)
/** Sub-registers of Register @ref RG_BBC1_MACSHA0F3 */

/** Access parameters for sub-register MACSHA0F3 in register @ref
 * RG_BBC1_MACSHA0F3 */
#define SR_BBC1_MACSHA0F3_MACSHA0F3     RG_BBC1_MACSHA0F3, \
	MACSHA0F3_MACSHA0F3_MASK, MACSHA0F3_MACSHA0F3_SHIFT

/** Address for register BBC1_MACSHA1F3 */
#define RG_BBC1_MACSHA1F3               (0x43C)
/** Sub-registers of Register @ref RG_BBC1_MACSHA1F3 */

/** Access parameters for sub-register MACSHA1F3 in register @ref
 * RG_BBC1_MACSHA1F3 */
#define SR_BBC1_MACSHA1F3_MACSHA1F3     RG_BBC1_MACSHA1F3, \
	MACSHA1F3_MACSHA1F3_MASK, MACSHA1F3_MACSHA1F3_SHIFT

/** Address for register BBC1_AMCS */
#define RG_BBC1_AMCS                    (0x440)
/** Sub-registers of Register @ref RG_BBC1_AMCS */
/** Access parameters for sub-register TX2RX in register @ref RG_BBC1_AMCS */
#define SR_BBC1_AMCS_TX2RX              RG_BBC1_AMCS, AMCS_TX2RX_MASK, \
	AMCS_TX2RX_SHIFT
/** Access parameters for sub-register CCATX in register @ref RG_BBC1_AMCS */
#define SR_BBC1_AMCS_CCATX              RG_BBC1_AMCS, AMCS_CCATX_MASK, \
	AMCS_CCATX_SHIFT
/** Access parameters for sub-register CCAED in register @ref RG_BBC1_AMCS */
#define SR_BBC1_AMCS_CCAED              RG_BBC1_AMCS, AMCS_CCAED_MASK, \
	AMCS_CCAED_SHIFT
/** Access parameters for sub-register AACK in register @ref RG_BBC1_AMCS */
#define SR_BBC1_AMCS_AACK               RG_BBC1_AMCS, AMCS_AACK_MASK, \
	AMCS_AACK_SHIFT
/** Access parameters for sub-register AACKS in register @ref RG_BBC1_AMCS */
#define SR_BBC1_AMCS_AACKS              RG_BBC1_AMCS, AMCS_AACKS_MASK, \
	AMCS_AACKS_SHIFT
/** Access parameters for sub-register AACKDR in register @ref RG_BBC1_AMCS */
#define SR_BBC1_AMCS_AACKDR             RG_BBC1_AMCS, AMCS_AACKDR_MASK,	\
	AMCS_AACKDR_SHIFT
/** Access parameters for sub-register AACKFA in register @ref RG_BBC1_AMCS */
#define SR_BBC1_AMCS_AACKFA             RG_BBC1_AMCS, AMCS_AACKFA_MASK,	\
	AMCS_AACKFA_SHIFT
/** Access parameters for sub-register AACKFT in register @ref RG_BBC1_AMCS */
#define SR_BBC1_AMCS_AACKFT             RG_BBC1_AMCS, AMCS_AACKFT_MASK,	\
	AMCS_AACKFT_SHIFT

/** Address for register BBC1_AMEDT */
#define RG_BBC1_AMEDT                   (0x441)
/** Sub-registers of Register @ref RG_BBC1_AMEDT */
/** Access parameters for sub-register AMEDT in register @ref RG_BBC1_AMEDT */
#define SR_BBC1_AMEDT_AMEDT             RG_BBC1_AMEDT, AMEDT_AMEDT_MASK, \
	AMEDT_AMEDT_SHIFT

/** Address for register BBC1_AMAACKPD */
#define RG_BBC1_AMAACKPD                (0x442)
/** Sub-registers of Register @ref RG_BBC1_AMAACKPD */
/** Access parameters for sub-register PD0 in register @ref RG_BBC1_AMAACKPD */
#define SR_BBC1_AMAACKPD_PD0            RG_BBC1_AMAACKPD, AMAACKPD_PD0_MASK, \
	AMAACKPD_PD0_SHIFT
/** Access parameters for sub-register PD1 in register @ref RG_BBC1_AMAACKPD */
#define SR_BBC1_AMAACKPD_PD1            RG_BBC1_AMAACKPD, AMAACKPD_PD1_MASK, \
	AMAACKPD_PD1_SHIFT
/** Access parameters for sub-register PD2 in register @ref RG_BBC1_AMAACKPD */
#define SR_BBC1_AMAACKPD_PD2            RG_BBC1_AMAACKPD, AMAACKPD_PD2_MASK, \
	AMAACKPD_PD2_SHIFT
/** Access parameters for sub-register PD3 in register @ref RG_BBC1_AMAACKPD */
#define SR_BBC1_AMAACKPD_PD3            RG_BBC1_AMAACKPD, AMAACKPD_PD3_MASK, \
	AMAACKPD_PD3_SHIFT

/** Address for register BBC1_AMAACKTL */
#define RG_BBC1_AMAACKTL                (0x443)
/** Sub-registers of Register @ref RG_BBC1_AMAACKTL */

/** Access parameters for sub-register AMAACKTL in register @ref
 * RG_BBC1_AMAACKTL */
#define SR_BBC1_AMAACKTL_AMAACKTL       RG_BBC1_AMAACKTL, \
	AMAACKTL_AMAACKTL_MASK, AMAACKTL_AMAACKTL_SHIFT

/** Address for register BBC1_AMAACKTH */
#define RG_BBC1_AMAACKTH                (0x444)
/** Sub-registers of Register @ref RG_BBC1_AMAACKTH */

/** Access parameters for sub-register AMAACKTH in register @ref
 * RG_BBC1_AMAACKTH */
#define SR_BBC1_AMAACKTH_AMAACKTH       RG_BBC1_AMAACKTH, \
	AMAACKTH_AMAACKTH_MASK, AMAACKTH_AMAACKTH_SHIFT

/** Address for register BBC1_FSKC0 */
#define RG_BBC1_FSKC0                   (0x460)
/** Sub-registers of Register @ref RG_BBC1_FSKC0 */
/** Access parameters for sub-register MORD in register @ref RG_BBC1_FSKC0 */
#define SR_BBC1_FSKC0_MORD              RG_BBC1_FSKC0, FSKC0_MORD_MASK,	\
	FSKC0_MORD_SHIFT
/** Access parameters for sub-register MIDX in register @ref RG_BBC1_FSKC0 */
#define SR_BBC1_FSKC0_MIDX              RG_BBC1_FSKC0, FSKC0_MIDX_MASK,	\
	FSKC0_MIDX_SHIFT
/** Access parameters for sub-register MIDXS in register @ref RG_BBC1_FSKC0 */
#define SR_BBC1_FSKC0_MIDXS             RG_BBC1_FSKC0, FSKC0_MIDXS_MASK, \
	FSKC0_MIDXS_SHIFT
/** Access parameters for sub-register BT in register @ref RG_BBC1_FSKC0 */
#define SR_BBC1_FSKC0_BT                RG_BBC1_FSKC0, FSKC0_BT_MASK, \
	FSKC0_BT_SHIFT

/** Address for register BBC1_FSKC1 */
#define RG_BBC1_FSKC1                   (0x461)
/** Sub-registers of Register @ref RG_BBC1_FSKC1 */
/** Access parameters for sub-register SRATE in register @ref RG_BBC1_FSKC1 */
#define SR_BBC1_FSKC1_SRATE             RG_BBC1_FSKC1, FSKC1_SRATE_MASK, \
	FSKC1_SRATE_SHIFT
/** Access parameters for sub-register FI in register @ref RG_BBC1_FSKC1 */
#define SR_BBC1_FSKC1_FI                RG_BBC1_FSKC1, FSKC1_FI_MASK, \
	FSKC1_FI_SHIFT
/** Access parameters for sub-register FSKPLH in register @ref RG_BBC1_FSKC1 */
#define SR_BBC1_FSKC1_FSKPLH            RG_BBC1_FSKC1, FSKC1_FSKPLH_MASK, \
	FSKC1_FSKPLH_SHIFT

/** Address for register BBC1_FSKC2 */
#define RG_BBC1_FSKC2                   (0x462)
/** Sub-registers of Register @ref RG_BBC1_FSKC2 */
/** Access parameters for sub-register FECIE in register @ref RG_BBC1_FSKC2 */
#define SR_BBC1_FSKC2_FECIE             RG_BBC1_FSKC2, FSKC2_FECIE_MASK, \
	FSKC2_FECIE_SHIFT
/** Access parameters for sub-register FECS in register @ref RG_BBC1_FSKC2 */
#define SR_BBC1_FSKC2_FECS              RG_BBC1_FSKC2, FSKC2_FECS_MASK,	\
	FSKC2_FECS_SHIFT
/** Access parameters for sub-register PRI in register @ref RG_BBC1_FSKC2 */
#define SR_BBC1_FSKC2_PRI               RG_BBC1_FSKC2, FSKC2_PRI_MASK, \
	FSKC2_PRI_SHIFT
/** Access parameters for sub-register MSE in register @ref RG_BBC1_FSKC2 */
#define SR_BBC1_FSKC2_MSE               RG_BBC1_FSKC2, FSKC2_MSE_MASK, \
	FSKC2_MSE_SHIFT
/** Access parameters for sub-register RXPTO in register @ref RG_BBC1_FSKC2 */
#define SR_BBC1_FSKC2_RXPTO             RG_BBC1_FSKC2, FSKC2_RXPTO_MASK, \
	FSKC2_RXPTO_SHIFT
/** Access parameters for sub-register RXO in register @ref RG_BBC1_FSKC2 */
#define SR_BBC1_FSKC2_RXO               RG_BBC1_FSKC2, FSKC2_RXO_MASK, \
	FSKC2_RXO_SHIFT
/** Access parameters for sub-register PDTM in register @ref RG_BBC1_FSKC2 */
#define SR_BBC1_FSKC2_PDTM              RG_BBC1_FSKC2, FSKC2_PDTM_MASK,	\
	FSKC2_PDTM_SHIFT

/** Address for register BBC1_FSKC3 */
#define RG_BBC1_FSKC3                   (0x463)
/** Sub-registers of Register @ref RG_BBC1_FSKC3 */
/** Access parameters for sub-register PDT in register @ref RG_BBC1_FSKC3 */
#define SR_BBC1_FSKC3_PDT               RG_BBC1_FSKC3, FSKC3_PDT_MASK, \
	FSKC3_PDT_SHIFT
/** Access parameters for sub-register SFDT in register @ref RG_BBC1_FSKC3 */
#define SR_BBC1_FSKC3_SFDT              RG_BBC1_FSKC3, FSKC3_SFDT_MASK,	\
	FSKC3_SFDT_SHIFT

/** Address for register BBC1_FSKC4 */
#define RG_BBC1_FSKC4                   (0x464)
/** Sub-registers of Register @ref RG_BBC1_FSKC4 */
/** Access parameters for sub-register CSFD0 in register @ref RG_BBC1_FSKC4 */
#define SR_BBC1_FSKC4_CSFD0             RG_BBC1_FSKC4, FSKC4_CSFD0_MASK, \
	FSKC4_CSFD0_SHIFT
/** Access parameters for sub-register CSFD1 in register @ref RG_BBC1_FSKC4 */
#define SR_BBC1_FSKC4_CSFD1             RG_BBC1_FSKC4, FSKC4_CSFD1_MASK, \
	FSKC4_CSFD1_SHIFT
/** Access parameters for sub-register RAWRBIT in register @ref RG_BBC1_FSKC4 */
#define SR_BBC1_FSKC4_RAWRBIT           RG_BBC1_FSKC4, FSKC4_RAWRBIT_MASK, \
	FSKC4_RAWRBIT_SHIFT
/** Access parameters for sub-register SFD32 in register @ref RG_BBC1_FSKC4 */
#define SR_BBC1_FSKC4_SFD32             RG_BBC1_FSKC4, FSKC4_SFD32_MASK, \
	FSKC4_SFD32_SHIFT
/** Access parameters for sub-register SFDQ in register @ref RG_BBC1_FSKC4 */
#define SR_BBC1_FSKC4_SFDQ              RG_BBC1_FSKC4, FSKC4_SFDQ_MASK,	\
	FSKC4_SFDQ_SHIFT

/** Address for register BBC1_FSKPLL */
#define RG_BBC1_FSKPLL                  (0x465)
/** Sub-registers of Register @ref RG_BBC1_FSKPLL */
/** Access parameters for sub-register FSKPLL in register @ref RG_BBC1_FSKPLL */
#define SR_BBC1_FSKPLL_FSKPLL           RG_BBC1_FSKPLL, FSKPLL_FSKPLL_MASK, \
	FSKPLL_FSKPLL_SHIFT

/** Address for register BBC1_FSKSFD0L */
#define RG_BBC1_FSKSFD0L                (0x466)
/** Sub-registers of Register @ref RG_BBC1_FSKSFD0L */

/** Access parameters for sub-register FSKSFD0L in register @ref
 * RG_BBC1_FSKSFD0L */
#define SR_BBC1_FSKSFD0L_FSKSFD0L       RG_BBC1_FSKSFD0L, \
	FSKSFD0L_FSKSFD0L_MASK, FSKSFD0L_FSKSFD0L_SHIFT

/** Address for register BBC1_FSKSFD0H */
#define RG_BBC1_FSKSFD0H                (0x467)
/** Sub-registers of Register @ref RG_BBC1_FSKSFD0H */

/** Access parameters for sub-register FSKSFD0H in register @ref
 * RG_BBC1_FSKSFD0H */
#define SR_BBC1_FSKSFD0H_FSKSFD0H       RG_BBC1_FSKSFD0H, \
	FSKSFD0H_FSKSFD0H_MASK, FSKSFD0H_FSKSFD0H_SHIFT

/** Address for register BBC1_FSKSFD1L */
#define RG_BBC1_FSKSFD1L                (0x468)
/** Sub-registers of Register @ref RG_BBC1_FSKSFD1L */

/** Access parameters for sub-register FSKSFD1L in register @ref
 * RG_BBC1_FSKSFD1L */
#define SR_BBC1_FSKSFD1L_FSKSFD1L       RG_BBC1_FSKSFD1L, \
	FSKSFD1L_FSKSFD1L_MASK, FSKSFD1L_FSKSFD1L_SHIFT

/** Address for register BBC1_FSKSFD1H */
#define RG_BBC1_FSKSFD1H                (0x469)
/** Sub-registers of Register @ref RG_BBC1_FSKSFD1H */

/** Access parameters for sub-register FSKSFD1H in register @ref
 * RG_BBC1_FSKSFD1H */
#define SR_BBC1_FSKSFD1H_FSKSFD1H       RG_BBC1_FSKSFD1H, \
	FSKSFD1H_FSKSFD1H_MASK, FSKSFD1H_FSKSFD1H_SHIFT

/** Address for register BBC1_FSKPHRTX */
#define RG_BBC1_FSKPHRTX                (0x46A)
/** Sub-registers of Register @ref RG_BBC1_FSKPHRTX */
/** Access parameters for sub-register RB1 in register @ref RG_BBC1_FSKPHRTX */
#define SR_BBC1_FSKPHRTX_RB1            RG_BBC1_FSKPHRTX, FSKPHRTX_RB1_MASK, \
	FSKPHRTX_RB1_SHIFT
/** Access parameters for sub-register RB2 in register @ref RG_BBC1_FSKPHRTX */
#define SR_BBC1_FSKPHRTX_RB2            RG_BBC1_FSKPHRTX, FSKPHRTX_RB2_MASK, \
	FSKPHRTX_RB2_SHIFT
/** Access parameters for sub-register DW in register @ref RG_BBC1_FSKPHRTX */
#define SR_BBC1_FSKPHRTX_DW             RG_BBC1_FSKPHRTX, FSKPHRTX_DW_MASK, \
	FSKPHRTX_DW_SHIFT
/** Access parameters for sub-register SFD in register @ref RG_BBC1_FSKPHRTX */
#define SR_BBC1_FSKPHRTX_SFD            RG_BBC1_FSKPHRTX, FSKPHRTX_SFD_MASK, \
	FSKPHRTX_SFD_SHIFT

/** Address for register BBC1_FSKPHRRX */
#define RG_BBC1_FSKPHRRX                (0x46B)
/** Sub-registers of Register @ref RG_BBC1_FSKPHRRX */
/** Access parameters for sub-register RB1 in register @ref RG_BBC1_FSKPHRRX */
#define SR_BBC1_FSKPHRRX_RB1            RG_BBC1_FSKPHRRX, FSKPHRRX_RB1_MASK, \
	FSKPHRRX_RB1_SHIFT
/** Access parameters for sub-register RB2 in register @ref RG_BBC1_FSKPHRRX */
#define SR_BBC1_FSKPHRRX_RB2            RG_BBC1_FSKPHRRX, FSKPHRRX_RB2_MASK, \
	FSKPHRRX_RB2_SHIFT
/** Access parameters for sub-register DW in register @ref RG_BBC1_FSKPHRRX */
#define SR_BBC1_FSKPHRRX_DW             RG_BBC1_FSKPHRRX, FSKPHRRX_DW_MASK, \
	FSKPHRRX_DW_SHIFT
/** Access parameters for sub-register SFD in register @ref RG_BBC1_FSKPHRRX */
#define SR_BBC1_FSKPHRRX_SFD            RG_BBC1_FSKPHRRX, FSKPHRRX_SFD_MASK, \
	FSKPHRRX_SFD_SHIFT
/** Access parameters for sub-register MS in register @ref RG_BBC1_FSKPHRRX */
#define SR_BBC1_FSKPHRRX_MS             RG_BBC1_FSKPHRRX, FSKPHRRX_MS_MASK, \
	FSKPHRRX_MS_SHIFT
/** Access parameters for sub-register FCST in register @ref RG_BBC1_FSKPHRRX */
#define SR_BBC1_FSKPHRRX_FCST           RG_BBC1_FSKPHRRX, FSKPHRRX_FCST_MASK, \
	FSKPHRRX_FCST_SHIFT

/** Address for register BBC1_FSKRPC */
#define RG_BBC1_FSKRPC                  (0x46C)
/** Sub-registers of Register @ref RG_BBC1_FSKRPC */
/** Access parameters for sub-register BASET in register @ref RG_BBC1_FSKRPC */
#define SR_BBC1_FSKRPC_BASET            RG_BBC1_FSKRPC, FSKRPC_BASET_MASK, \
	FSKRPC_BASET_SHIFT
/** Access parameters for sub-register EN in register @ref RG_BBC1_FSKRPC */
#define SR_BBC1_FSKRPC_EN               RG_BBC1_FSKRPC, FSKRPC_EN_MASK,	\
	FSKRPC_EN_SHIFT

/** Address for register BBC1_FSKRPCONT */
#define RG_BBC1_FSKRPCONT               (0x46D)
/** Sub-registers of Register @ref RG_BBC1_FSKRPCONT */

/** Access parameters for sub-register FSKRPCONT in register @ref
 * RG_BBC1_FSKRPCONT */
#define SR_BBC1_FSKRPCONT_FSKRPCONT     RG_BBC1_FSKRPCONT, \
	FSKRPCONT_FSKRPCONT_MASK, FSKRPCONT_FSKRPCONT_SHIFT

/** Address for register BBC1_FSKRPCOFFT */
#define RG_BBC1_FSKRPCOFFT              (0x46E)
/** Sub-registers of Register @ref RG_BBC1_FSKRPCOFFT */

/** Access parameters for sub-register FSKRPCOFFT in register @ref
 * RG_BBC1_FSKRPCOFFT */
#define SR_BBC1_FSKRPCOFFT_FSKRPCOFFT   RG_BBC1_FSKRPCOFFT, \
	FSKRPCOFFT_FSKRPCOFFT_MASK, FSKRPCOFFT_FSKRPCOFFT_SHIFT

/** Address for register BBC1_FSKRRXFLL */
#define RG_BBC1_FSKRRXFLL               (0x470)
/** Sub-registers of Register @ref RG_BBC1_FSKRRXFLL */

/** Access parameters for sub-register FSKRRXFLL in register @ref
 * RG_BBC1_FSKRRXFLL */
#define SR_BBC1_FSKRRXFLL_FSKRRXFLL     RG_BBC1_FSKRRXFLL, \
	FSKRRXFLL_FSKRRXFLL_MASK, FSKRRXFLL_FSKRRXFLL_SHIFT

/** Address for register BBC1_FSKRRXFLH */
#define RG_BBC1_FSKRRXFLH               (0x471)
/** Sub-registers of Register @ref RG_BBC1_FSKRRXFLH */
/** Access parameters for sub-register FSKRRXFLH in register @ref
 *RG_BBC1_FSKRRXFLH */
#define SR_BBC1_FSKRRXFLH_FSKRRXFLH     RG_BBC1_FSKRRXFLH, \
	FSKRRXFLH_FSKRRXFLH_MASK, FSKRRXFLH_FSKRRXFLH_SHIFT

/** Address for register BBC1_FSKDM */
#define RG_BBC1_FSKDM                   (0x472)
/** Sub-registers of Register @ref RG_BBC1_FSKDM */
/** Access parameters for sub-register EN in register @ref RG_BBC1_FSKDM */
#define SR_BBC1_FSKDM_EN                RG_BBC1_FSKDM, FSKDM_EN_MASK, \
	FSKDM_EN_SHIFT
/** Access parameters for sub-register PE in register @ref RG_BBC1_FSKDM */
#define SR_BBC1_FSKDM_PE                RG_BBC1_FSKDM, FSKDM_PE_MASK, \
	FSKDM_PE_SHIFT

/** Address for register BBC1_FSKPE0 */
#define RG_BBC1_FSKPE0                  (0x473)
/** Sub-registers of Register @ref RG_BBC1_FSKPE0 */
/** Access parameters for sub-register FSKPE0 in register @ref RG_BBC1_FSKPE0 */
#define SR_BBC1_FSKPE0_FSKPE0           RG_BBC1_FSKPE0, FSKPE0_FSKPE0_MASK, \
	FSKPE0_FSKPE0_SHIFT

/** Address for register BBC1_FSKPE1 */
#define RG_BBC1_FSKPE1                  (0x474)
/** Sub-registers of Register @ref RG_BBC1_FSKPE1 */
/** Access parameters for sub-register FSKPE1 in register @ref RG_BBC1_FSKPE1 */
#define SR_BBC1_FSKPE1_FSKPE1           RG_BBC1_FSKPE1, FSKPE1_FSKPE1_MASK, \
	FSKPE1_FSKPE1_SHIFT

/** Address for register BBC1_FSKPE2 */
#define RG_BBC1_FSKPE2                  (0x475)
/** Sub-registers of Register @ref RG_BBC1_FSKPE2 */
/** Access parameters for sub-register FSKPE2 in register @ref RG_BBC1_FSKPE2 */
#define SR_BBC1_FSKPE2_FSKPE2           RG_BBC1_FSKPE2, FSKPE2_FSKPE2_MASK, \
	FSKPE2_FSKPE2_SHIFT

/** Address for register BBC1_PMUC */
#define RG_BBC1_PMUC                    (0x480)
/** Sub-registers of Register @ref RG_BBC1_PMUC */
/** Access parameters for sub-register EN in register @ref RG_BBC1_PMUC */
#define SR_BBC1_PMUC_EN                 RG_BBC1_PMUC, PMUC_EN_MASK, \
	PMUC_EN_SHIFT
/** Access parameters for sub-register AVG in register @ref RG_BBC1_PMUC */
#define SR_BBC1_PMUC_AVG                RG_BBC1_PMUC, PMUC_AVG_MASK, \
	PMUC_AVG_SHIFT
/** Access parameters for sub-register SYNC in register @ref RG_BBC1_PMUC */
#define SR_BBC1_PMUC_SYNC               RG_BBC1_PMUC, PMUC_SYNC_MASK, \
	PMUC_SYNC_SHIFT
/** Access parameters for sub-register FED in register @ref RG_BBC1_PMUC */
#define SR_BBC1_PMUC_FED                RG_BBC1_PMUC, PMUC_FED_MASK, \
	PMUC_FED_SHIFT
/** Access parameters for sub-register IQSEL in register @ref RG_BBC1_PMUC */
#define SR_BBC1_PMUC_IQSEL              RG_BBC1_PMUC, PMUC_IQSEL_MASK, \
	PMUC_IQSEL_SHIFT
/** Access parameters for sub-register CCFTS in register @ref RG_BBC1_PMUC */
#define SR_BBC1_PMUC_CCFTS              RG_BBC1_PMUC, PMUC_CCFTS_MASK, \
	PMUC_CCFTS_SHIFT

/** Address for register BBC1_PMUVAL */
#define RG_BBC1_PMUVAL                  (0x481)
/** Sub-registers of Register @ref RG_BBC1_PMUVAL */
/** Access parameters for sub-register PMUVAL in register @ref RG_BBC1_PMUVAL */
#define SR_BBC1_PMUVAL_PMUVAL           RG_BBC1_PMUVAL, PMUVAL_PMUVAL_MASK, \
	PMUVAL_PMUVAL_SHIFT

/** Address for register BBC1_PMUQF */
#define RG_BBC1_PMUQF                   (0x482)
/** Sub-registers of Register @ref RG_BBC1_PMUQF */
/** Access parameters for sub-register PMUQF in register @ref RG_BBC1_PMUQF */
#define SR_BBC1_PMUQF_PMUQF             RG_BBC1_PMUQF, PMUQF_PMUQF_MASK, \
	PMUQF_PMUQF_SHIFT

/** Address for register BBC1_PMUI */
#define RG_BBC1_PMUI                    (0x483)
/** Sub-registers of Register @ref RG_BBC1_PMUI */
/** Access parameters for sub-register PMUI in register @ref RG_BBC1_PMUI */
#define SR_BBC1_PMUI_PMUI               RG_BBC1_PMUI, PMUI_PMUI_MASK, \
	PMUI_PMUI_SHIFT

/** Address for register BBC1_PMUQ */
#define RG_BBC1_PMUQ                    (0x484)
/** Sub-registers of Register @ref RG_BBC1_PMUQ */
/** Access parameters for sub-register PMUQ in register @ref RG_BBC1_PMUQ */
#define SR_BBC1_PMUQ_PMUQ               RG_BBC1_PMUQ, PMUQ_PMUQ_MASK, \
	PMUQ_PMUQ_SHIFT

/** Address for register BBC1_CNTC */
#define RG_BBC1_CNTC                    (0x490)
/** Sub-registers of Register @ref RG_BBC1_CNTC */
/** Access parameters for sub-register EN in register @ref RG_BBC1_CNTC */
#define SR_BBC1_CNTC_EN                 RG_BBC1_CNTC, CNTC_EN_MASK, \
	CNTC_EN_SHIFT
/** Access parameters for sub-register RSTRXS in register @ref RG_BBC1_CNTC */
#define SR_BBC1_CNTC_RSTRXS             RG_BBC1_CNTC, CNTC_RSTRXS_MASK,	\
	CNTC_RSTRXS_SHIFT
/** Access parameters for sub-register RSTTXS in register @ref RG_BBC1_CNTC */
#define SR_BBC1_CNTC_RSTTXS             RG_BBC1_CNTC, CNTC_RSTTXS_MASK,	\
	CNTC_RSTTXS_SHIFT
/** Access parameters for sub-register CAPRXS in register @ref RG_BBC1_CNTC */
#define SR_BBC1_CNTC_CAPRXS             RG_BBC1_CNTC, CNTC_CAPRXS_MASK,	\
	CNTC_CAPRXS_SHIFT
/** Access parameters for sub-register CAPTXS in register @ref RG_BBC1_CNTC */
#define SR_BBC1_CNTC_CAPTXS             RG_BBC1_CNTC, CNTC_CAPTXS_MASK,	\
	CNTC_CAPTXS_SHIFT

/** Address for register BBC1_CNT0 */
#define RG_BBC1_CNT0                    (0x491)
/** Sub-registers of Register @ref RG_BBC1_CNT0 */
/** Access parameters for sub-register CNT0 in register @ref RG_BBC1_CNT0 */
#define SR_BBC1_CNT0_CNT0               RG_BBC1_CNT0, CNT0_CNT0_MASK, \
	CNT0_CNT0_SHIFT

/** Address for register BBC1_CNT1 */
#define RG_BBC1_CNT1                    (0x492)
/** Sub-registers of Register @ref RG_BBC1_CNT1 */
/** Access parameters for sub-register CNT1 in register @ref RG_BBC1_CNT1 */
#define SR_BBC1_CNT1_CNT1               RG_BBC1_CNT1, CNT1_CNT1_MASK, \
	CNT1_CNT1_SHIFT

/** Address for register BBC1_CNT2 */
#define RG_BBC1_CNT2                    (0x493)
/** Sub-registers of Register @ref RG_BBC1_CNT2 */
/** Access parameters for sub-register CNT2 in register @ref RG_BBC1_CNT2 */
#define SR_BBC1_CNT2_CNT2               RG_BBC1_CNT2, CNT2_CNT2_MASK, \
	CNT2_CNT2_SHIFT

/** Address for register BBC1_CNT3 */
#define RG_BBC1_CNT3                    (0x494)
/** Sub-registers of Register @ref RG_BBC1_CNT3 */
/** Access parameters for sub-register CNT3 in register @ref RG_BBC1_CNT3 */
#define SR_BBC1_CNT3_CNT3               RG_BBC1_CNT3, CNT3_CNT3_MASK, \
	CNT3_CNT3_SHIFT

/** Enumerations */

/** Enumeration for RF commands and states used for trx command and state
 * registers */
typedef enum rf_cmd_state_tag {
	/** Constant RF_NOP for sub-register @ref SR_CMD */
	RF_NOP =                            (0x0),

	/** Constant RF_SLEEP for sub-register @ref SR_CMD */
	RF_SLEEP =                          (0x1),

	/** Constant RF_TRXOFF for sub-register @ref SR_CMD */
	RF_TRXOFF =                         (0x2),

	/** Constant RF_TXPREP for sub-register @ref SR_CMD */
	RF_TXPREP =                         (0x3),

	/** Constant RF_TX for sub-register @ref SR_CMD */
	RF_TX =                             (0x4),

	/** Constant RF_RX for sub-register @ref SR_CMD */
	RF_RX =                             (0x5),

	/** Constant RF_RESET for sub-register @ref SR_CMD */
	RF_RESET =                          (0x7)
} rf_cmd_state_t;

/** Enumeration for RF commands and states used for trx command and state
 * registers */
typedef enum rf_cmd_status_tag {
	/** Constant RF_TRXOFF for sub-register @ref SR_CMD */
	STATUS_RF_TRXOFF =                         (0x2),

	/** Constant RF_TXPREP for sub-register @ref SR_CMD */
	STATUS_RF_TXPREP =                         (0x3),

	/** Constant RF_TX for sub-register @ref SR_CMD */
	STATUS_RF_TX =                             (0x4),

	/** Constant RF_RX for sub-register @ref SR_CMD */
	STATUS_RF_RX =                             (0x5),

	/** Constant RF_RX for sub-register @ref SR_CMD */
	STATUS_RF_TRANSITION =                      (0x6),

	/** Constant RF_RESET for sub-register @ref SR_CMD */
	STATUS_RF_RESET =                          (0x7)
} rf_cmd_status_t;

/** Enumeration for RF IRQs used for IRQS and IRQM registers */
typedef enum rf_irq_tag {
	/** Constant RF_IRQ_IQIFSF for sub-register @ref SR_IQIFSF */
	RF_IRQ_IQIFSF =                     (0x20),

	/** Constant RF_IRQ_TRXERR for sub-register @ref SR_TRXERR */
	RF_IRQ_TRXERR =                     (0x10),

	/** Constant RF_IRQ_BATLOW for sub-register @ref SR_BATLOW */
	RF_IRQ_BATLOW =                     (0x08),

	/** Constant RF_IRQ_EDC for sub-register @ref SR_EDC */
	RF_IRQ_EDC =                        (0x04),

	/** Constant RF_IRQ_TRXRDY for sub-register @ref SR_TRXRDY */
	RF_IRQ_TRXRDY =                     (0x02),

	/** Constant RF_IRQ_WAKEUP for sub-register @ref SR_WAKEUP */
	RF_IRQ_WAKEUP =                     (0x01),

	/** No interrupt is indicated by IRQ_STATUS register */
	RF_IRQ_NO_IRQ =                     (0x00),

	/** All interrupts are indicated by IRQ_STATUS register */
	RF_IRQ_ALL_IRQ =                    (0x3F)
} rf_irq_t;

/** Enumeration for BB IRQs */
typedef enum bb_irq_tag {
	/** Constant BB_IRQ_FBLI for sub-register @ref SR_FBLI */
	BB_IRQ_FBLI =                       (0x80),

	/** Constant BB_IRQ_AGCR for sub-register @ref SR_AGCR */
	BB_IRQ_AGCR =                       (0x40),

	/** Constant BB_IRQ_AGCH for sub-register @ref SR_AGCH */
	BB_IRQ_AGCH =                       (0x20),

	/** Constant BB_IRQ_TXFE for sub-register @ref SR_TXFE */
	BB_IRQ_TXFE =                       (0x10),

	/** Constant BB_IRQ_RXEM for sub-register @ref SR_RXEM */
	BB_IRQ_RXEM =                       (0x08),

	/** Constant BB_IRQ_RXAM for sub-register @ref SR_RXAM */
	BB_IRQ_RXAM =                       (0x04),

	/** Constant BB_IRQ_RXFE for sub-register @ref SR_RXFE */
	BB_IRQ_RXFE =                       (0x02),

	/** Constant BB_IRQ_RXFS for sub-register @ref SR_RXFS */
	BB_IRQ_RXFS =                       (0x01),

	/** No interrupt is indicated by IRQ_STATUS register */
	BB_IRQ_NO_IRQ =                     (0x00),

	/** All interrupts are indicated by IRQ_STATUS register */
	BB_IRQ_ALL_IRQ =                    (0xFF)
} bb_irq_t;

#endif /* RF215_H */
