/*
 * Copyright (C) 2012, [Your Company Here] All Rights Reserved.
 * IT IS EXPECTED THAT THIS TEXT BE REPLACED WITH THE BOARD SOFTWARE
 * PROVIDER'S COPYRIGHT INFORMATION. THIS TEXT WILL BE DISPLAYED AT
 * THE TOP OF ALL SOURCE FILES GENERATED FOR THIS BOARD DESIGN.
*/

// File: uart2_iomux_config.c

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

// Function to configure IOMUXC for uart2 module.
void uart2_iomux_config(void)
{
    // Config uart2.UART2_RX_DATA to pad EIM_DATA27(E25)
    // HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27_WR(0x00000004);
    // HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_WR(0x0001B0B0);
    // HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27(0x020E0170)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [3:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: eim signal: EIM_DATA27
    //     ALT1 (1) - Select instance: ipu1 signal: IPU1_DI1_PIN13
    //     ALT2 (2) - Select instance: ipu1 signal: IPU1_CSI0_DATA00
    //     ALT3 (3) - Select instance: ipu1 signal: IPU1_CSI1_DATA13
    //     ALT4 (4) - Select instance: uart2 signal: UART2_RX_DATA
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO27
    //     ALT6 (6) - Select instance: ipu1 signal: IPU1_SISG3
    //     ALT7 (7) - Select instance: ipu1 signal: IPU1_DISP1_DATA23
    //     ALT8 (8) - Select instance: epdc signal: EPDC_SDOE
    HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27_WR(
        BF_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27_SION_V(DISABLED) |
        BF_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27_MUX_MODE_V(ALT4));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27(0x020E0540)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_WR(
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_HYS_V(ENABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_PUS_V(100K_OHM_PU) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_PUE_V(PULL) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_PKE_V(ENABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_ODE_V(DISABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_SPEED_V(100MHZ) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_DSE_V(40_OHM) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_SRE_V(SLOW));
    // Pad EIM_DATA27 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT(0x020E0904)
    //   DAISY [2:0] - Input Select (DAISY) Field Reset: EIM_DATA26_ALT4
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA26_ALT4 (0) - Select signal uart2 UART2_TX_DATA as input from pad EIM_DATA26(ALT4).
    //     EIM_DATA27_ALT4 (1) - Select signal uart2 UART2_RX_DATA as input from pad EIM_DATA27(ALT4).
    //     GPIO07_ALT4 (2) - Select signal uart2 UART2_TX_DATA as input from pad GPIO07(ALT4).
    //     GPIO08_ALT4 (3) - Select signal uart2 UART2_RX_DATA as input from pad GPIO08(ALT4).
    //     SD3_DATA4_ALT1 (4) - Select signal uart2 UART2_RX_DATA as input from pad SD3_DATA4(ALT1).
    //     SD3_DATA5_ALT1 (5) - Select signal uart2 UART2_TX_DATA as input from pad SD3_DATA5(ALT1).
    //     SD4_DATA4_ALT2 (6) - Select signal uart2 UART2_RX_DATA as input from pad SD4_DATA4(ALT2).
    //     SD4_DATA7_ALT2 (7) - Select signal uart2 UART2_TX_DATA as input from pad SD4_DATA7(ALT2).
    HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(
        BF_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_DAISY_V(EIM_DATA26_ALT4));

    // Config uart2.UART2_TX_DATA to pad EIM_DATA26(E24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA26_WR(0x00000004);
    // HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_WR(0x0001B0B0);
    // HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_EIM_DATA26(0x020E016C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [3:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: eim signal: EIM_DATA26
    //     ALT1 (1) - Select instance: ipu1 signal: IPU1_DI1_PIN11
    //     ALT2 (2) - Select instance: ipu1 signal: IPU1_CSI0_DATA01
    //     ALT3 (3) - Select instance: ipu1 signal: IPU1_CSI1_DATA14
    //     ALT4 (4) - Select instance: uart2 signal: UART2_TX_DATA
    //     ALT5 (5) - Select instance: gpio3 signal: GPIO3_IO26
    //     ALT6 (6) - Select instance: ipu1 signal: IPU1_SISG2
    //     ALT7 (7) - Select instance: ipu1 signal: IPU1_DISP1_DATA22
    //     ALT8 (8) - Select instance: epdc signal: EPDC_SDOED
    HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA26_WR(
        BF_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA26_SION_V(DISABLED) |
        BF_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA26_MUX_MODE_V(ALT4));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26(0x020E053C)
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
    HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_WR(
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_HYS_V(ENABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_PUS_V(100K_OHM_PU) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_PUE_V(PULL) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_PKE_V(ENABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_ODE_V(DISABLED) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_SPEED_V(100MHZ) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_DSE_V(40_OHM) |
        BF_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_SRE_V(SLOW));
    // Pad EIM_DATA26 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT(0x020E0904)
    //   DAISY [2:0] - Input Select (DAISY) Field Reset: EIM_DATA26_ALT4
    //                 Selecting Pads Involved in Daisy Chain.
    //     EIM_DATA26_ALT4 (0) - Select signal uart2 UART2_TX_DATA as input from pad EIM_DATA26(ALT4).
    //     EIM_DATA27_ALT4 (1) - Select signal uart2 UART2_RX_DATA as input from pad EIM_DATA27(ALT4).
    //     GPIO07_ALT4 (2) - Select signal uart2 UART2_TX_DATA as input from pad GPIO07(ALT4).
    //     GPIO08_ALT4 (3) - Select signal uart2 UART2_RX_DATA as input from pad GPIO08(ALT4).
    //     SD3_DATA4_ALT1 (4) - Select signal uart2 UART2_RX_DATA as input from pad SD3_DATA4(ALT1).
    //     SD3_DATA5_ALT1 (5) - Select signal uart2 UART2_TX_DATA as input from pad SD3_DATA5(ALT1).
    //     SD4_DATA4_ALT2 (6) - Select signal uart2 UART2_RX_DATA as input from pad SD4_DATA4(ALT2).
    //     SD4_DATA7_ALT2 (7) - Select signal uart2 UART2_TX_DATA as input from pad SD4_DATA7(ALT2).
    HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(
        BF_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_DAISY_V(EIM_DATA26_ALT4));
}
