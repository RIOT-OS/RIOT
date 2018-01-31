/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc1200
 * @{
 *
 * @file
 * @brief       Driver internal constants for CC1200 chip configuration
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
 */

#ifndef CC1200_DEFINES_H
#define CC1200_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Variable packet length PKTCTRL0 bit configuration
 *
 * If variable packet length is configured in PKTCTRL0 the
 * first byte after the synch word determines the packet length.
 */
#define VARIABLE_PKTLEN     (0x01)

/**
 * @name    Bitmasks for reading out status register values
 * @{
 */

/**
 * @brief   Bitmask (=10000000) for reading CRC_OK.
 *
 * If CRC_OK == 1: CRC for received data OK (or CRC disabled).
 * If CRC_OK == 0: CRC error in received data.
 */
#define CRC_OK              (0x80)
/**
 * @brief  Bitmask (=01111111) for reading LQI_EST.
 *
 * The Link Quality Indicator estimates how easily a received signal can be demodulated.
 */
#define LQI_EST                 (0x7F)
#define I_RSSI                  (0x00)  /**< Index 0 contains RSSI information (from optionally appended packet status bytes). */
#define I_LQI                   (0x01)  /**< Index 1 contains LQI & CRC_OK information (from optionally appended packet status bytes). */
#define MARC_STATE              (0x1F)  /**< Bitmask (=00011111) for reading MARC_STATE in MARCSTATE status register. */
#define PKTSTATUS_CS            (0x40)  /**< Bitmask (=01000000) for reading CS (Carrier Sense) in PKTSTATUS status register. */
#define PKTSTATUS_PQT_REACHED   (0x20)  /**< Bitmask (=00100000) for reading PQT_REACHED (Preamble Quality reached) in PKTSTATUS status register. */
#define PKTSTATUS_CCA           (0x10)  /**< Bitmask (=00010000) for reading CCA (clear channel assessment) in PKTSTATUS status register. */
#define PKTSTATUS_SFD           (0x08)  /**< Bitmask (=00001000) for reading SFD (Sync word found) in PKTSTATUS status register. */
#define PKTSTATUS_GDO2          (0x04)  /**< Bitmask (=00000100) for reading GDO2 (current value on GDO2 pin) in PKTSTATUS status register. */
#define PKTSTATUS_GDO1          (0x02)  /**< Bitmask (=00000010) for reading GDO1 (current value on GDO1 pin) in PKTSTATUS status register. */
#define PKTSTATUS_GDO0          (0x01)  /**< Bitmask (=00000001) for reading GDO0 (current value on GDO0 pin) in PKTSTATUS status register. */
#define TXFIFO_UNDERFLOW        (0x80)  /**< Bitmask (=10000000) for reading TXFIFO_UNDERFLOW in TXBYTES status register. */
#define BYTES_IN_TXFIFO         (0x7F)  /**< Bitmask (=01111111) for reading NUM_TXBYTES in TXBYTES status register. */
#define RXFIFO_OVERFLOW         (0x80)  /**< Bitmask (=10000000) for reading RXFIFO_OVERFLOW in RXBYTES status register. */
#define BYTES_IN_RXFIFO         (0x7F)  /**< Bitmask (=01111111) for reading NUM_RXBYTES in RXBYTES status register. */
/** @} */

/**
 * @name    Bitmasks for reading out configuration register values
 * @{
 */
#define PKT_LENGTH_CONFIG   (0x03)      /**< Bitmask (=00000011) for reading LENGTH_CONFIG in PKTCTRL0 configuration register. */
/** @} */

/**
 * @name    Definitions to support burst/single access
 * @{
 */
#define CC1200_WRITE_BURST  (0x40)  /**< Offset for burst write. */
#define CC1200_READ_SINGLE  (0x80)  /**< Offset for read single byte. */
#define CC1200_READ_BURST   (0xC0)  /**< Offset for read burst. */
#define CC1200_NOBYTE       (0xFF)  /**< No command (for reading). */
/** @} */


/**
 * @name CC1200 Defines
 */
#define CC1200_CONST_TX_POWER_MAX       14                                  /**< Max output power */
#define CC1200_RF_CFG_CHAN_CENTER_F0           863125                       /**< Base frequency in kHz */
#define CC1200_RF_CFG_CHAN_SPACING             200000                       /**< Channel spacing in Hz */
#define CC1200_RF_CFG_MIN_CHANNEL              0                            /**< The minimum channel */
#define CC1200_RF_CFG_MAX_CHANNEL              33                           /**< The maximum channel */
#define CC1200_RF_CFG_MAX_TXPOWER              CC1200_CONST_TX_POWER_MAX    /**< The maximum output power in dBm */
#define CC1200_RF_CFG_CCA_THRESHOLD            (-91)                        /**< The carrier sense level used for CCA in dBm */
#define CC1200_RF_CFG_RSSI_OFFSET              (-81)                        /**< The RSSI offset in dBm */

/**
 * @name   CC1200 Register
 * @{
 */
#define CC1200_IOCFG3                  (0x0000) /**< GPIO3 IO Pin Configuration */
#define CC1200_IOCFG2                  (0x0001) /**< GPIO2 IO Pin Configuration */
#define CC1200_IOCFG1                  (0x0002) /**< GPIO1 IO Pin Configuration */
#define CC1200_IOCFG0                  (0x0003) /**< GPIO0 IO Pin Configuration */
#define CC1200_SYNC3                   (0x0004) /**< Sync Word Configuration [31:24] */
#define CC1200_SYNC2                   (0x0005) /**< Sync Word Configuration [23:16] */
#define CC1200_SYNC1                   (0x0006) /**< Sync Word Configuration [15:8] */
#define CC1200_SYNC0                   (0x0007) /**< Sync Word Configuration [7:0] */
#define CC1200_SYNC_CFG1               (0x0008) /**< Sync Word Detection Configuration Reg. 1 */
#define CC1200_SYNC_CFG0               (0x0009) /**< Sync Word Detection Configuration Reg. 0 */
#define CC1200_DEVIATION_M             (0x000A) /**< Frequency Deviation Configuration */
#define CC1200_MODCFG_DEV_E            (0x000B) /**< Modulation Format and Frequency Deviation Configur.. */
#define CC1200_DCFILT_CFG              (0x000C) /**< Digital DC Removal Configuration */
#define CC1200_PREAMBLE_CFG1           (0x000D) /**< Preamble Length Configuration Reg. 1 */
#define CC1200_PREAMBLE_CFG0           (0x000E) /**< Preamble Detection Configuration Reg. 0 */
#define CC1200_IQIC                    (0x000F) /**< Digital Image Channel Compensation Configuration */
#define CC1200_CHAN_BW                 (0x0010) /**< Channel Filter Configuration */
#define CC1200_MDMCFG1                 (0x0011) /**< General Modem Parameter Configuration Reg. 1 */
#define CC1200_MDMCFG0                 (0x0012) /**< General Modem Parameter Configuration Reg. 0 */
#define CC1200_SYMBOL_RATE2            (0x0013) /**< Symbol Rate Configuration Exponent and Mantissa [1.. */
#define CC1200_SYMBOL_RATE1            (0x0014) /**< Symbol Rate Configuration Mantissa [15:8] */
#define CC1200_SYMBOL_RATE0            (0x0015) /**< Symbol Rate Configuration Mantissa [7:0] */
#define CC1200_AGC_REF                 (0x0016) /**< AGC Reference Level Configuration */
#define CC1200_AGC_CS_THR              (0x0017) /**< Carrier Sense Threshold Configuration */
#define CC1200_AGC_GAIN_ADJUST         (0x0018) /**< RSSI Offset Configuration */
#define CC1200_AGC_CFG3                (0x0019) /**< Automatic Gain Control Configuration Reg. 3 */
#define CC1200_AGC_CFG2                (0x001A) /**< Automatic Gain Control Configuration Reg. 2 */
#define CC1200_AGC_CFG1                (0x001B) /**< Automatic Gain Control Configuration Reg. 1 */
#define CC1200_AGC_CFG0                (0x001C) /**< Automatic Gain Control Configuration Reg. 0 */
#define CC1200_FIFO_CFG                (0x001D) /**< FIFO Configuration */
#define CC1200_DEV_ADDR                (0x001E) /**< Device Address Configuration */
#define CC1200_SETTLING_CFG            (0x001F) /**< Frequency Synthesizer Calibration and Settling Con.. */
#define CC1200_FS_CFG                  (0x0020) /**< Frequency Synthesizer Configuration */
#define CC1200_WOR_CFG1                (0x0021) /**< eWOR Configuration Reg. 1 */
#define CC1200_WOR_CFG0                (0x0022) /**< eWOR Configuration Reg. 0 */
#define CC1200_WOR_EVENT0_MSB          (0x0023) /**< Event 0 Configuration MSB */
#define CC1200_WOR_EVENT0_LSB          (0x0024) /**< Event 0 Configuration LSB */
#define CC1200_RXDCM_TIME              (0x0025) /**< RX Duty Cycle Mode Configuration */
#define CC1200_PKT_CFG2                (0x0026) /**< Packet Configuration Reg. 2 */
#define CC1200_PKT_CFG1                (0x0027) /**< Packet Configuration Reg. 1 */
#define CC1200_PKT_CFG0                (0x0028) /**< Packet Configuration Reg. 0 */
#define CC1200_RFEND_CFG1              (0x0029) /**< RFEND Configuration Reg. 1 */
#define CC1200_RFEND_CFG0              (0x002A) /**< RFEND Configuration Reg. 0 */
#define CC1200_PA_CFG1                 (0x002B) /**< Power Amplifier Configuration Reg. 1 */
#define CC1200_PA_CFG0                 (0x002C) /**< Power Amplifier Configuration Reg. 0 */
#define CC1200_ASK_CFG                 (0x002D) /**< ASK Configuration */
#define CC1200_PKT_LEN                 (0x002E) /**< Packet Length Configuration */
/** @} */

/**
 * @name    Extended Configuration Register for CC1200
 * @{
 */
#define CC1200_IF_MIX_CFG              (0x2F00) /**< IF Mix Configuration */
#define CC1200_FREQOFF_CFG             (0x2F01) /**< Frequency Offset Correction Configuration */
#define CC1200_TOC_CFG                 (0x2F02) /**< Timing Offset Correction Configuration */
#define CC1200_MARC_SPARE              (0x2F03) /**< MARC Spare */
#define CC1200_ECG_CFG                 (0x2F04) /**< External Clock Frequency Configuration */
#define CC1200_MDMCFG2                 (0x2F05) /**< General Modem Parameter Configuration Reg. 2 */
#define CC1200_EXT_CTRL                (0x2F06) /**< External Control Configuration */
#define CC1200_RCCAL_FINE              (0x2F07) /**< RC Oscillator Calibration Fine */
#define CC1200_RCCAL_COARSE            (0x2F08) /**< RC Oscillator Calibration Coarse */
#define CC1200_RCCAL_OFFSET            (0x2F09) /**< RC Oscillator Calibration Clock Offset */
#define CC1200_FREQOFF1                (0x2F0A) /**< Frequency Offset MSB */
#define CC1200_FREQOFF0                (0x2F0B) /**< Frequency Offset LSB */
#define CC1200_FREQ2                   (0x2F0C) /**< Frequency Configuration [23:16] */
#define CC1200_FREQ1                   (0x2F0D) /**< Frequency Configuration [15:8] */
#define CC1200_FREQ0                   (0x2F0E) /**< Frequency Configuration [7:0] */
#define CC1200_IF_ADC2                 (0x2F0F) /**< Analog to Digital Converter Configuration Reg. 2 */
#define CC1200_IF_ADC1                 (0x2F10) /**< Analog to Digital Converter Configuration Reg. 1 */
#define CC1200_IF_ADC0                 (0x2F11) /**< Analog to Digital Converter Configuration Reg. 0 */
#define CC1200_FS_DIG1                 (0x2F12) /**< Frequency Synthesizer Digital Reg. 1 */
#define CC1200_FS_DIG0                 (0x2F13) /**< Frequency Synthesizer Digital Reg. 0 */
#define CC1200_FS_CAL3                 (0x2F14) /**< Frequency Synthesizer Calibration Reg. 3 */
#define CC1200_FS_CAL2                 (0x2F15) /**< Frequency Synthesizer Calibration Reg. 2 */
#define CC1200_FS_CAL1                 (0x2F16) /**< Frequency Synthesizer Calibration Reg. 1 */
#define CC1200_FS_CAL0                 (0x2F17) /**< Frequency Synthesizer Calibration Reg. 0 */
#define CC1200_FS_CHP                  (0x2F18) /**< Frequency Synthesizer Charge Pump Configuration */
#define CC1200_FS_DIVTWO               (0x2F19) /**< Frequency Synthesizer Divide by 2 */
#define CC1200_FS_DSM1                 (0x2F1A) /**< FS Digital Synthesizer Module Configuration Reg. 1 */
#define CC1200_FS_DSM0                 (0x2F1B) /**< FS Digital Synthesizer Module Configuration Reg. 0 */
#define CC1200_FS_DVC1                 (0x2F1C) /**< Frequency Synthesizer Divider Chain Configuration .. */
#define CC1200_FS_DVC0                 (0x2F1D) /**< Frequency Synthesizer Divider Chain Configuration .. */
#define CC1200_FS_LBI                  (0x2F1E) /**< Frequency Synthesizer Local Bias Configuration */
#define CC1200_FS_PFD                  (0x2F1F) /**< Frequency Synthesizer Phase Frequency Detector Con.. */
#define CC1200_FS_PRE                  (0x2F20) /**< Frequency Synthesizer Prescaler Configuration */
#define CC1200_FS_REG_DIV_CML          (0x2F21) /**< Frequency Synthesizer Divider Regulator Configurat.. */
#define CC1200_FS_SPARE                (0x2F22) /**< Frequency Synthesizer Spare */
#define CC1200_FS_VCO4                 (0x2F23) /**< FS Voltage Controlled Oscillator Configuration Reg.. */
#define CC1200_FS_VCO3                 (0x2F24) /**< FS Voltage Controlled Oscillator Configuration Reg.. */
#define CC1200_FS_VCO2                 (0x2F25) /**< FS Voltage Controlled Oscillator Configuration Reg.. */
#define CC1200_FS_VCO1                 (0x2F26) /**< FS Voltage Controlled Oscillator Configuration Reg.. */
#define CC1200_FS_VCO0                 (0x2F27) /**< FS Voltage Controlled Oscillator Configuration Reg.. */
#define CC1200_GBIAS6                  (0x2F28) /**< Global Bias Configuration Reg. 6 */
#define CC1200_GBIAS5                  (0x2F29) /**< Global Bias Configuration Reg. 5 */
#define CC1200_GBIAS4                  (0x2F2A) /**< Global Bias Configuration Reg. 4 */
#define CC1200_GBIAS3                  (0x2F2B) /**< Global Bias Configuration Reg. 3 */
#define CC1200_GBIAS2                  (0x2F2C) /**< Global Bias Configuration Reg. 2 */
#define CC1200_GBIAS1                  (0x2F2D) /**< Global Bias Configuration Reg. 1 */
#define CC1200_GBIAS0                  (0x2F2E) /**< Global Bias Configuration Reg. 0 */
#define CC1200_IFAMP                   (0x2F2F) /**< Intermediate Frequency Amplifier Configuration */
#define CC1200_LNA                     (0x2F30) /**< Low Noise Amplifier Configuration */
#define CC1200_RXMIX                   (0x2F31) /**< RX Mixer Configuration */
#define CC1200_XOSC5                   (0x2F32) /**< Crystal Oscillator Configuration Reg. 5 */
#define CC1200_XOSC4                   (0x2F33) /**< Crystal Oscillator Configuration Reg. 4 */
#define CC1200_XOSC3                   (0x2F34) /**< Crystal Oscillator Configuration Reg. 3 */
#define CC1200_XOSC2                   (0x2F35) /**< Crystal Oscillator Configuration Reg. 2 */
#define CC1200_XOSC1                   (0x2F36) /**< Crystal Oscillator Configuration Reg. 1 */
#define CC1200_XOSC0                   (0x2F37) /**< Crystal Oscillator Configuration Reg. 0 */
#define CC1200_ANALOG_SPARE            (0x2F38) /**< Analog Spare */
#define CC1200_PA_CFG3                 (0x2F39) /**< Power Amplifier Configuration Reg. 3 */
#define CC1200_WOR_TIME1               (0x2F64) /**< eWOR Timer Counter Value MSB */
#define CC1200_WOR_TIME0               (0x2F65) /**< eWOR Timer Counter Value LSB */
#define CC1200_WOR_CAPTURE1            (0x2F66) /**< eWOR Timer Capture Value MSB */
#define CC1200_WOR_CAPTURE0            (0x2F67) /**< eWOR Timer Capture Value LSB */
#define CC1200_BIST                    (0x2F68) /**< MARC Built-In Self-Test */
#define CC1200_DCFILTOFFSET_I1         (0x2F69) /**< DC Filter Offset I MSB */
#define CC1200_DCFILTOFFSET_I0         (0x2F6A) /**< DC Filter Offset I LSB */
#define CC1200_DCFILTOFFSET_Q1         (0x2F6B) /**< DC Filter Offset Q MSB */
#define CC1200_DCFILTOFFSET_Q0         (0x2F6C) /**< DC Filter Offset Q LSB */
#define CC1200_IQIE_I1                 (0x2F6D) /**< IQ Imbalance Value I MSB */
#define CC1200_IQIE_I0                 (0x2F6E) /**< IQ Imbalance Value I LSB */
#define CC1200_IQIE_Q1                 (0x2F6F) /**< IQ Imbalance Value Q MSB */
#define CC1200_IQIE_Q0                 (0x2F70) /**< IQ Imbalance Value Q LSB */
#define CC1200_RSSI1                   (0x2F71) /**< Received Signal Strength Indicator Reg. 1 */
#define CC1200_RSSI0                   (0x2F72) /**< Received Signal Strength Indicator Reg.0 */
#define CC1200_LQI_VAL                 (0x2F74) /**< Link Quality Indicator Value */
#define CC1200_PQT_SYNC_ERR            (0x2F75) /**< Preamble and Sync Word Error */
#define CC1200_DEM_STATUS              (0x2F76) /**< Demodulator Status */
#define CC1200_FREQOFF_EST1            (0x2F77) /**< Frequency Offset Estimate MSB */
#define CC1200_FREQOFF_EST0            (0x2F78) /**< Frequency Offset Estimate LSB */
#define CC1200_AGC_GAIN3               (0x2F79) /**< Automatic Gain Control Reg. 3 */
#define CC1200_AGC_GAIN2               (0x2F7A) /**< Automatic Gain Control Reg. 2 */
#define CC1200_AGC_GAIN1               (0x2F7B) /**< Automatic Gain Control Reg. 1 */
#define CC1200_AGC_GAIN0               (0x2F7C) /**< Automatic Gain Control Reg. 0 */
#define CC1200_CFM_RX_DATA_OUT         (0x2F7D) /**< Custom Frequency Modulation RX Data */
#define CC1200_CFM_TX_DATA_IN          (0x2F7E) /**< Custom Frequency Modulation TX Data */
#define CC1200_ASK_SOFT_RX_DATA        (0x2F7F) /**< ASK Soft Decision Output */
#define CC1200_RNDGEN                  (0x2F80) /**< Random Number Generator Value */
#define CC1200_MAGN2                   (0x2F81) /**< Signal Magnitude after CORDIC [16] */
#define CC1200_MAGN1                   (0x2F82) /**< Signal Magnitude after CORDIC [15:8] */
#define CC1200_MAGN0                   (0x2F83) /**< Signal Magnitude after CORDIC [7:0] */
#define CC1200_ANG1                    (0x2F84) /**< Signal Angular after CORDIC [9:8] */
#define CC1200_ANG0                    (0x2F85) /**< Signal Angular after CORDIC [7:0] */
#define CC1200_CHFILT_I2               (0x2F86) /**< Channel Filter Data Real Part [16] */
#define CC1200_CHFILT_I1               (0x2F87) /**< Channel Filter Data Real Part [15:8] */
#define CC1200_CHFILT_I0               (0x2F88) /**< Channel Filter Data Real Part [7:0] */
#define CC1200_CHFILT_Q2               (0x2F89) /**< Channel Filter Data Imaginary Part [16] */
#define CC1200_CHFILT_Q1               (0x2F8A) /**< Channel Filter Data Imaginary Part [15:8] */
#define CC1200_CHFILT_Q0               (0x2F8B) /**< Channel Filter Data Imaginary Part [7:0] */
#define CC1200_GPIO_STATUS             (0x2F8C) /**< General Purpose Input/Output Status */
#define CC1200_FSCAL_CTRL              (0x2F8D) /**< Frequency Synthesizer Calibration Control */
#define CC1200_PHASE_ADJUST            (0x2F8E) /**< Frequency Synthesizer Phase Adjust */
#define CC1200_PARTNUMBER              (0x2F8F) /**< Part Number */
#define CC1200_PARTVERSION             (0x2F90) /**< Part Revision */
#define CC1200_SERIAL_STATUS           (0x2F91) /**< Serial Status */
#define CC1200_MODEM_STATUS1           (0x2F92) /**< Modem Status Reg. 1 */
#define CC1200_MODEM_STATUS0           (0x2F93) /**< Modem Status Reg. 0 */
#define CC1200_MARC_STATUS1            (0x2F94) /**< MARC Status Reg. 1 */
#define CC1200_MARC_STATUS0            (0x2F95) /**< MARC Status Reg. 0 */
#define CC1200_PA_IFAMP_TEST           (0x2F96) /**< Power Amplifier Intermediate Frequency Amplifier T.. */
#define CC1200_FSRF_TEST               (0x2F97) /**< Frequency Synthesizer Test */
#define CC1200_PRE_TEST                (0x2F98) /**< Frequency Synthesizer Prescaler Test */
#define CC1200_PRE_OVR                 (0x2F99) /**< Frequency Synthesizer Prescaler Override */
#define CC1200_ADC_TEST                (0x2F9A) /**< Analog to Digital Converter Test */
#define CC1200_DVC_TEST                (0x2F9B) /**< Digital Divider Chain Test */
#define CC1200_ATEST                   (0x2F9C) /**< Analog Test */
#define CC1200_ATEST_LVDS              (0x2F9D) /**< Analog Test LVDS */
#define CC1200_ATEST_MODE              (0x2F9E) /**< Analog Test Mode */
#define CC1200_XOSC_TEST1              (0x2F9F) /**< Crystal Oscillator Test Reg. 1 */
#define CC1200_XOSC_TEST0              (0x2FA0) /**< Crystal Oscillator Test Reg. 0 */
#define CC1200_AES                     (0x2FA1) /**< AES */
#define CC1200_MDM_TEST                (0x2FA2) /**< MODEM Test */
#define CC1200_RXFIRST                 (0x2FD2) /**< RX FIFO Pointer First Entry */
#define CC1200_TXFIRST                 (0x2FD3) /**< TX FIFO Pointer First Entry */
#define CC1200_RXLAST                  (0x2FD4) /**< RX FIFO Pointer Last Entry */
#define CC1200_TXLAST                  (0x2FD5) /**< TX FIFO Pointer Last Entry */
#define CC1200_NUM_TXBYTES             (0x2FD6) /**< TX FIFO Status */
#define CC1200_NUM_RXBYTES             (0x2FD7) /**< RX FIFO Status */
#define CC1200_FIFO_NUM_TXBYTES        (0x2FD8) /**< TX FIFO Status */
#define CC1200_FIFO_NUM_RXBYTES        (0x2FD9) /**< RX FIFO Status */
#define CC1200_RXFIFO_PRE_BUF          (0x2FDA) /**< RX FIFO Status */
#define CC1200_AES_KEY15               (0x2FE0) /**< Advanced Encryption Standard Key [127:120] */
#define CC1200_AES_KEY14               (0x2FE1) /**< Advanced Encryption Standard Key [119:112] */
#define CC1200_AES_KEY13               (0x2FE2) /**< Advanced Encryption Standard Key [111:104] */
#define CC1200_AES_KEY12               (0x2FE3) /**< Advanced Encryption Standard Key [103:96] */
#define CC1200_AES_KEY11               (0x2FE4) /**< Advanced Encryption Standard Key [95:88] */
#define CC1200_AES_KEY10               (0x2FE5) /**< Advanced Encryption Standard Key [87:80] */
#define CC1200_AES_KEY9                (0x2FE6) /**< Advanced Encryption Standard Key [79:72] */
#define CC1200_AES_KEY8                (0x2FE7) /**< Advanced Encryption Standard Key [71:64] */
#define CC1200_AES_KEY7                (0x2FE8) /**< Advanced Encryption Standard Key [63:56] */
#define CC1200_AES_KEY6                (0x2FE9) /**< Advanced Encryption Standard Key [55:48] */
#define CC1200_AES_KEY5                (0x2FEA) /**< Advanced Encryption Standard Key [47:40] */
#define CC1200_AES_KEY4                (0x2FEB) /**< Advanced Encryption Standard Key [39:32] */
#define CC1200_AES_KEY3                (0x2FEC) /**< Advanced Encryption Standard Key [31:24] */
#define CC1200_AES_KEY2                (0x2FED) /**< Advanced Encryption Standard Key [23:16] */
#define CC1200_AES_KEY1                (0x2FEE) /**< Advanced Encryption Standard Key [15:8] */
#define CC1200_AES_KEY0                (0x2FEF) /**< Advanced Encryption Standard Key [7:0] */
#define CC1200_AES_BUFFER15            (0x2FF0) /**< Advanced Encryption Standard Buffer [127:120] */
#define CC1200_AES_BUFFER14            (0x2FF1) /**< Advanced Encryption Standard Buffer [119:112] */
#define CC1200_AES_BUFFER13            (0x2FF2) /**< Advanced Encryption Standard Buffer [111:104] */
#define CC1200_AES_BUFFER12            (0x2FF3) /**< Advanced Encryption Standard Buffer [103:93] */
#define CC1200_AES_BUFFER11            (0x2FF4) /**< Advanced Encryption Standard Buffer [95:88] */
#define CC1200_AES_BUFFER10            (0x2FF5) /**< Advanced Encryption Standard Buffer [87:80] */
#define CC1200_AES_BUFFER9             (0x2FF6) /**< Advanced Encryption Standard Buffer [79:72] */
#define CC1200_AES_BUFFER8             (0x2FF7) /**< Advanced Encryption Standard Buffer [71:64] */
#define CC1200_AES_BUFFER7             (0x2FF8) /**< Advanced Encryption Standard Buffer [63:56] */
#define CC1200_AES_BUFFER6             (0x2FF9) /**< Advanced Encryption Standard Buffer [55:48] */
#define CC1200_AES_BUFFER5             (0x2FFA) /**< Advanced Encryption Standard Buffer [47:40] */
#define CC1200_AES_BUFFER4             (0x2FFB) /**< Advanced Encryption Standard Buffer [39:32] */
#define CC1200_AES_BUFFER3             (0x2FFC) /**< Advanced Encryption Standard Buffer [31:24] */
#define CC1200_AES_BUFFER2             (0x2FFD) /**< Advanced Encryption Standard Buffer [23:16] */
#define CC1200_AES_BUFFER1             (0x2FFE) /**< Advanced Encryption Standard Buffer [15:8] */
#define CC1200_AES_BUFFER0             (0x2FFF) /**< Advanced Encryption Standard Buffer [7:0] */
/** @} */

/**
 * @name    Strobe commands (14x)
 * @{
 */
#define CC1200_SRES         (0x30)      /**< Reset chip. */
/**
 * @brief   Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
 *
 * If in RX/TX: Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
 */
#define CC1200_SFSTXON      (0x31)
#define CC1200_SXOFF        (0x32)      /**< Turn off crystal oscillator. */
#define CC1200_SCAL         (0x33)      /**< Calibrate frequency synthesizer and turn it off (enables quick start). */
#define CC1200_SRX          (0x34)      /**< Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1. */
/**
 * In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
 * If in RX state and CCA is enabled: Only go to TX if channel is clear.
 */
#define CC1200_STX          (0x35)
#define CC1200_SIDLE        (0x36)      /**< Exit RX / TX, turn off frequency synthesizer and exit WOR mode if applicable. */
#define CC1200_SAFC         (0x37)      /**< Perform AFC adjustment of the frequency synthesizer */
#define CC1200_SWOR         (0x38)      /**< Start automatic RX polling sequence (Wake-on-Radio) */
#define CC1200_SPWD         (0x39)      /**< Enter power down mode when CSn goes high. */
#define CC1200_SFRX         (0x3A)      /**< Flush the RX FIFO buffer (CC1200 should be in IDLE state). */
#define CC1200_SFTX         (0x3B)      /**< Flush the TX FIFO buffer (CC1200 should be in IDLE state). */
#define CC1200_SWORRST      (0x3C)      /**< Reset real time clock. */
#define CC1200_SNOP         (0x3D)      /**< No operation. May be used to pad strobe commands to two bytes for simpler software. */
#define CC1200_EXT_REG_ACC  (0x2F)      /**< When accessing the extended register space, 0x2F has to be sent first */
/** @} */

/**
 * @name    Status registers (12x)
 * @{
 */
#define CC1200_PARTNUM      (0x30)      /**< Part number of CC1200. */
#define CC1200_VERSION      (0x31)      /**< Current version number. */
#define CC1200_FREQEST      (0x32)      /**< Frequency Offset Estimate. */
#define CC1200_LQI          (0x33)      /**< Demodulator estimate for Link Quality. */
#define CC1200_RSSI         (0x34)      /**< Received signal strength indication. */
#define CC1200_MARCSTATE    (0x2F73)    /**< MARC State */
#define CC1200_WORTIME1     (0x36)      /**< High byte of WOR timer. */
#define CC1200_WORTIME0     (0x37)      /**< Low byte of WOR timer. */
#define CC1200_PKTSTATUS    (0x38)      /**< Current GDOx status and packet status. */
#define CC1200_VCO_VC_DAC   (0x39)      /**< Current setting from PLL calibration module. */


#define CC1200_TXBYTES      CC1200_NUM_TXBYTES      /**< Underflow and number of bytes in the TX FIFO. */
#define CC1200_RXBYTES      CC1200_NUM_RXBYTES      /**< Overflow and number of bytes in the RX FIFO. */

/** @} */

/**
 * @name    Multi byte registers
 * @{
 */
/**
 * @brief   Register for eight user selected output power settings.
 *
 * 3-bit FREND0.PA_POWER value selects the PATABLE entry to use.
 */
#define CC1200_PATABLE      (0x3E)
#define CC1200_TXFIFO       (0x3F)      /**< TX FIFO: Write operations write to the TX FIFO (SB: +0x00; BURST: +0x40) */
#define CC1200_RXFIFO       (0x3F)      /**< RX FIFO: Read operations read from the RX FIFO (SB: +0x80; BURST: +0xC0) */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif
