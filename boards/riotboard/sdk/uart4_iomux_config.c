/*
 * Copyright (C) 2012, [Your Company Here] All Rights Reserved.
 * IT IS EXPECTED THAT THIS TEXT BE REPLACED WITH THE BOARD SOFTWARE
 * PROVIDER'S COPYRIGHT INFORMATION. THIS TEXT WILL BE DISPLAYED AT
 * THE TOP OF ALL SOURCE FILES GENERATED FOR THIS BOARD DESIGN.
*/

// File: uart4_iomux_config.c

/* ------------------------------------------------------------------------------
 * <auto-generated>
 *     This code was generated by a tool.
 *     Runtime Version:3.4.0.3
 *
 *     Changes to this file may cause incorrect behavior and will be lost if
 *     the code is regenerated.
 * </auto-generated>
 * ------------------------------------------------------------------------------
*/

#include "iomux_config.h"
#include "registers/regsiomuxc.h"

// Function to configure IOMUXC for uart4 module.
void uart4_iomux_config(void)
{
    // Config uart4.UART4_RX_DATA to pad CSI0_DATA13(L1)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA13_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_WR(0x0001B0B0);
    // HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA13(0x020E0058)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: ipu1 signal: IPU1_CSI0_DATA13
    //     ALT1 (1) - Select instance: eim signal: EIM_DATA09
    //     ALT3 (3) - Select instance: uart4 signal: UART4_RX_DATA
    //     ALT5 (5) - Select instance: gpio5 signal: GPIO5_IO31
    //     ALT7 (7) - Select instance: arm signal: ARM_TRACE10
    HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA13_WR(
        BF_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA13_SION_V(DISABLED) |
        BF_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA13_MUX_MODE_V(ALT3));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13(0x020E036C)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     RESERVED0 (0) - Reserved
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_WR(
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_HYS_V(ENABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_PUS_V(100K_OHM_PU) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_PUE_V(PULL) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_PKE_V(ENABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_ODE_V(DISABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_SPEED_V(100MHZ) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_DSE_V(40_OHM) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_SRE_V(SLOW));
    // Pad CSI0_DATA13 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT(0x020E0914)
    //   DAISY [1:0] - Input Select (DAISY) Field Reset: CSI0_DATA12_ALT3
    //                 Selecting Pads Involved in Daisy Chain.
    //     CSI0_DATA12_ALT3 (0) - Select signal uart4 UART4_TX_DATA as input from pad CSI0_DATA12(ALT3).
    //     CSI0_DATA13_ALT3 (1) - Select signal uart4 UART4_RX_DATA as input from pad CSI0_DATA13(ALT3).
    //     KEY_COL0_ALT4 (2) - Select signal uart4 UART4_TX_DATA as input from pad KEY_COL0(ALT4).
    //     KEY_ROW0_ALT4 (3) - Select signal uart4 UART4_RX_DATA as input from pad KEY_ROW0(ALT4).
    HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_WR(
        BF_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_DAISY_V(CSI0_DATA12_ALT3));

    // Config uart4.UART4_TX_DATA to pad CSI0_DATA12(M2)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA12_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_WR(0x0001B0B0);
    // HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA12(0x020E0054)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: ipu1 signal: IPU1_CSI0_DATA12
    //     ALT1 (1) - Select instance: eim signal: EIM_DATA08
    //     ALT3 (3) - Select instance: uart4 signal: UART4_TX_DATA
    //     ALT5 (5) - Select instance: gpio5 signal: GPIO5_IO30
    //     ALT7 (7) - Select instance: arm signal: ARM_TRACE09
    HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA12_WR(
        BF_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA12_SION_V(DISABLED) |
        BF_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA12_MUX_MODE_V(ALT3));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12(0x020E0368)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     RESERVED0 (0) - Reserved
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_WR(
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_HYS_V(ENABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_PUS_V(100K_OHM_PU) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_PUE_V(PULL) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_PKE_V(ENABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_ODE_V(DISABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_SPEED_V(100MHZ) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_DSE_V(40_OHM) |
        BF_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_SRE_V(SLOW));
    // Pad CSI0_DATA12 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT(0x020E0914)
    //   DAISY [1:0] - Input Select (DAISY) Field Reset: CSI0_DATA12_ALT3
    //                 Selecting Pads Involved in Daisy Chain.
    //     CSI0_DATA12_ALT3 (0) - Select signal uart4 UART4_TX_DATA as input from pad CSI0_DATA12(ALT3).
    //     CSI0_DATA13_ALT3 (1) - Select signal uart4 UART4_RX_DATA as input from pad CSI0_DATA13(ALT3).
    //     KEY_COL0_ALT4 (2) - Select signal uart4 UART4_TX_DATA as input from pad KEY_COL0(ALT4).
    //     KEY_ROW0_ALT4 (3) - Select signal uart4 UART4_RX_DATA as input from pad KEY_ROW0(ALT4).
    HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_WR(
        BF_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_DAISY_V(CSI0_DATA12_ALT3));
}
