/*
 * Copyright (C) 2016 Unwired Devices [info@unwds.com]
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sx1276 SX1276
 * @ingroup     drivers_netdev
 * @brief       Semtech SX1276
 * @{
 * @file
 * @brief       SX1276 LoRa registers
 * @author      Eugene P. [ep@unwds.com]
 */

#ifndef SX1276_REGS_LORA_H
#define SX1276_REGS_LORA_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * SX1276 Internal registers Addresses
 */

/** Common settings */
#define SX1276_REG_LR_OPMODE                               0x01
#define SX1276_REG_LR_FRFMSB                               0x06
#define SX1276_REG_LR_FRFMID                               0x07
#define SX1276_REG_LR_FRFLSB                               0x08

/** Tx settings */
#define SX1276_REG_LR_PACONFIG                             0x09
#define SX1276_REG_LR_PARAMP                               0x0A
#define SX1276_REG_LR_OCP                                  0x0B

/** Rx settings */
#define SX1276_REG_LR_LNA                                  0x0C

/** LoRa registers */
#define SX1276_REG_LR_FIFOADDRPTR                          0x0D
#define SX1276_REG_LR_FIFOTXBASEADDR                       0x0E
#define SX1276_REG_LR_FIFORXBASEADDR                       0x0F
#define SX1276_REG_LR_FIFORXCURRENTADDR                    0x10
#define SX1276_REG_LR_IRQFLAGSMASK                         0x11
#define SX1276_REG_LR_IRQFLAGS                             0x12
#define SX1276_REG_LR_RXNBBYTES                            0x13
#define SX1276_REG_LR_RXHEADERCNTVALUEMSB                  0x14
#define SX1276_REG_LR_RXHEADERCNTVALUELSB                  0x15
#define SX1276_REG_LR_RXPACKETCNTVALUEMSB                  0x16
#define SX1276_REG_LR_RXPACKETCNTVALUELSB                  0x17
#define SX1276_REG_LR_MODEMSTAT                            0x18
#define SX1276_REG_LR_PKTSNRVALUE                          0x19
#define SX1276_REG_LR_PKTRSSIVALUE                         0x1A
#define SX1276_REG_LR_RSSIVALUE                            0x1B
#define SX1276_REG_LR_HOPCHANNEL                           0x1C
#define SX1276_REG_LR_MODEMCONFIG1                         0x1D
#define SX1276_REG_LR_MODEMCONFIG2                         0x1E
#define SX1276_REG_LR_SYMBTIMEOUTLSB                       0x1F
#define SX1276_REG_LR_PREAMBLEMSB                          0x20
#define SX1276_REG_LR_PREAMBLELSB                          0x21
#define SX1276_REG_LR_PAYLOADLENGTH                        0x22
#define SX1276_REG_LR_PAYLOADMAXLENGTH                     0x23
#define SX1276_REG_LR_HOPPERIOD                            0x24
#define SX1276_REG_LR_FIFORXBYTEADDR                       0x25
#define SX1276_REG_LR_MODEMCONFIG3                         0x26
#define SX1276_REG_LR_FEIMSB                               0x28
#define SX1276_REG_LR_FEIMID                               0x29
#define SX1276_REG_LR_FEILSB                               0x2A
#define SX1276_REG_LR_RSSIWIDEBAND                         0x2C
#define SX1276_REG_LR_TEST2F                               0x2F
#define SX1276_REG_LR_TEST30                               0x30
#define SX1276_REG_LR_DETECTOPTIMIZE                       0x31
#define SX1276_REG_LR_INVERTIQ                             0x33
#define SX1276_REG_LR_TEST36                               0x36
#define SX1276_REG_LR_DETECTIONTHRESHOLD                   0x37
#define SX1276_REG_LR_SYNCWORD                             0x39
#define SX1276_REG_LR_TEST3A                               0x3A
#define SX1276_REG_LR_INVERTIQ2                            0x3B

/** I/O settings */
#define SX1276_REG_LR_DIOMAPPING1                          0x40
#define SX1276_REG_LR_DIOMAPPING2                          0x41

/** Version */
#define SX1276_REG_LR_VERSION                              0x42

/** Additional settings */
#define SX1276_REG_LR_PLLHOP                               0x44
#define SX1276_REG_LR_TCXO                                 0x4B
#define SX1276_REG_LR_PADAC                                0x4D
#define SX1276_REG_LR_FORMERTEMP                           0x5B
#define SX1276_REG_LR_BITRATEFRAC                          0x5D
#define SX1276_REG_LR_AGCREF                               0x61
#define SX1276_REG_LR_AGCTHRESH1                           0x62
#define SX1276_REG_LR_AGCTHRESH2                           0x63
#define SX1276_REG_LR_AGCTHRESH3                           0x64
#define SX1276_REG_LR_PLL                                  0x70

/**
 * SX1276 LoRa bits control definition
 */

/**
 * RegFifo
 */
#define SX1276_REG_LR_FIFO                                 0x00

/**
 * RegOpMode
 */
#define SX1276_RF_LORA_OPMODE_LONGRANGEMODE_MASK              0x7F
#define SX1276_RF_LORA_OPMODE_LONGRANGEMODE_OFF               0x00 /**< Default */
#define SX1276_RF_LORA_OPMODE_LONGRANGEMODE_ON                0x80

#define SX1276_RF_LORA_OPMODE_ACCESSSHAREDSX1276_REG_MASK            0xBF
#define SX1276_RF_LORA_OPMODE_ACCESSSHAREDSX1276_REG_ENABLE          0x40
#define SX1276_RF_LORA_OPMODE_ACCESSSHAREDSX1276_REG_DISABLE         0x00 /**< Default */

#define SX1276_RF_LORA_OPMODE_FREQMODE_ACCESS_MASK            0xF7
#define SX1276_RF_LORA_OPMODE_FREQMODE_ACCESS_LF              0x08 /**< Default */
#define SX1276_RF_LORA_OPMODE_FREQMODE_ACCESS_HF              0x00

#define SX1276_RF_LORA_OPMODE_MASK                            0xF8
#define SX1276_RF_LORA_OPMODE_SLEEP                           0x00
#define SX1276_RF_LORA_OPMODE_STANDBY                         0x01 /**< Default */
#define SX1276_RF_LORA_OPMODE_SYNTHESIZER_TX                  0x02
#define SX1276_RF_LORA_OPMODE_TRANSMITTER                     0x03
#define SX1276_RF_LORA_OPMODE_SYNTHESIZER_RX                  0x04
#define SX1276_RF_LORA_OPMODE_RECEIVER                        0x05

/** LoRa specific modes */
#define SX1276_RF_LORA_OPMODE_RECEIVER_SINGLE                 0x06
#define SX1276_RF_LORA_OPMODE_CAD                             0x07

/**
 * RegFrf (MHz)
 */
#define SX1276_RF_LORA_FRFMSB_434_MHZ                         0x6C /**< Default */
#define SX1276_RF_LORA_FRFMID_434_MHZ                         0x80 /**< Default */
#define SX1276_RF_LORA_FRFLSB_434_MHZ                         0x00 /**< Default */

/**
 * RegPaConfig
 */
#define SX1276_RF_LORA_PACONFIG_PASELECT_MASK                 0x7F
#define SX1276_RF_LORA_PACONFIG_PASELECT_PABOOST              0x80
#define SX1276_RF_LORA_PACONFIG_PASELECT_RFO                  0x00 /**< Default */

#define SX1276_RF_LORA_PACONFIG_MAX_POWER_MASK                0x8F

#define SX1276_RF_LORA_PACONFIG_OUTPUTPOWER_MASK              0xF0

/**
 * RegPaRamp
 */
#define SX1276_RF_LORA_PARAMP_TXBANDFORCE_MASK                0xEF
#define SX1276_RF_LORA_PARAMP_TXBANDFORCE_BAND_SEL            0x10
#define SX1276_RF_LORA_PARAMP_TXBANDFORCE_AUTO                0x00 /**< Default */

#define SX1276_RF_LORA_PARAMP_MASK                            0xF0
#define SX1276_RF_LORA_PARAMP_3400_US                         0x00
#define SX1276_RF_LORA_PARAMP_2000_US                         0x01
#define SX1276_RF_LORA_PARAMP_1000_US                         0x02
#define SX1276_RF_LORA_PARAMP_0500_US                         0x03
#define SX1276_RF_LORA_PARAMP_0250_US                         0x04
#define SX1276_RF_LORA_PARAMP_0125_US                         0x05
#define SX1276_RF_LORA_PARAMP_0100_US                         0x06
#define SX1276_RF_LORA_PARAMP_0062_US                         0x07
#define SX1276_RF_LORA_PARAMP_0050_US                         0x08
#define SX1276_RF_LORA_PARAMP_0040_US                         0x09 /**< Default */
#define SX1276_RF_LORA_PARAMP_0031_US                         0x0A
#define SX1276_RF_LORA_PARAMP_0025_US                         0x0B
#define SX1276_RF_LORA_PARAMP_0020_US                         0x0C
#define SX1276_RF_LORA_PARAMP_0015_US                         0x0D
#define SX1276_RF_LORA_PARAMP_0012_US                         0x0E
#define SX1276_RF_LORA_PARAMP_0010_US                         0x0F

/**
 * RegOcp
 */
#define SX1276_RF_LORA_OCP_MASK                               0xDF
#define SX1276_RF_LORA_OCP_ON                                 0x20 /**< Default */
#define SX1276_RF_LORA_OCP_OFF                                0x00

#define SX1276_RF_LORA_OCP_TRIM_MASK                          0xE0
#define SX1276_RF_LORA_OCP_TRIM_045_MA                        0x00
#define SX1276_RF_LORA_OCP_TRIM_050_MA                        0x01
#define SX1276_RF_LORA_OCP_TRIM_055_MA                        0x02
#define SX1276_RF_LORA_OCP_TRIM_060_MA                        0x03
#define SX1276_RF_LORA_OCP_TRIM_065_MA                        0x04
#define SX1276_RF_LORA_OCP_TRIM_070_MA                        0x05
#define SX1276_RF_LORA_OCP_TRIM_075_MA                        0x06
#define SX1276_RF_LORA_OCP_TRIM_080_MA                        0x07
#define SX1276_RF_LORA_OCP_TRIM_085_MA                        0x08
#define SX1276_RF_LORA_OCP_TRIM_090_MA                        0x09
#define SX1276_RF_LORA_OCP_TRIM_095_MA                        0x0A
#define SX1276_RF_LORA_OCP_TRIM_100_MA                        0x0B  /**< Default */
#define SX1276_RF_LORA_OCP_TRIM_105_MA                        0x0C
#define SX1276_RF_LORA_OCP_TRIM_110_MA                        0x0D
#define SX1276_RF_LORA_OCP_TRIM_115_MA                        0x0E
#define SX1276_RF_LORA_OCP_TRIM_120_MA                        0x0F
#define SX1276_RF_LORA_OCP_TRIM_130_MA                        0x10
#define SX1276_RF_LORA_OCP_TRIM_140_MA                        0x11
#define SX1276_RF_LORA_OCP_TRIM_150_MA                        0x12
#define SX1276_RF_LORA_OCP_TRIM_160_MA                        0x13
#define SX1276_RF_LORA_OCP_TRIM_170_MA                        0x14
#define SX1276_RF_LORA_OCP_TRIM_180_MA                        0x15
#define SX1276_RF_LORA_OCP_TRIM_190_MA                        0x16
#define SX1276_RF_LORA_OCP_TRIM_200_MA                        0x17
#define SX1276_RF_LORA_OCP_TRIM_210_MA                        0x18
#define SX1276_RF_LORA_OCP_TRIM_220_MA                        0x19
#define SX1276_RF_LORA_OCP_TRIM_230_MA                        0x1A
#define SX1276_RF_LORA_OCP_TRIM_240_MA                        0x1B

/**
 * RegLna
 */
#define SX1276_RF_LORA_LNA_GAIN_MASK                          0x1F
#define SX1276_RF_LORA_LNA_GAIN_G1                            0x20 /**< Default */
#define SX1276_RF_LORA_LNA_GAIN_G2                            0x40
#define SX1276_RF_LORA_LNA_GAIN_G3                            0x60
#define SX1276_RF_LORA_LNA_GAIN_G4                            0x80
#define SX1276_RF_LORA_LNA_GAIN_G5                            0xA0
#define SX1276_RF_LORA_LNA_GAIN_G6                            0xC0

#define SX1276_RF_LORA_LNA_BOOST_LF_MASK                      0xE7
#define SX1276_RF_LORA_LNA_BOOST_LF_DEFAULT                   0x00 /**< Default */

#define SX1276_RF_LORA_LNA_BOOST_HF_MASK                      0xFC
#define SX1276_RF_LORA_LNA_BOOST_HF_OFF                       0x00 /**< Default */
#define SX1276_RF_LORA_LNA_BOOST_HF_ON                        0x03

/**
 * RegFifoAddrPtr
 */
#define SX1276_RF_LORA_FIFOADDRPTR                            0x00 /**< Default */

/**
 * RegFifoTxBaseAddr
 */
#define SX1276_RF_LORA_FIFOTXBASEADDR                         0x80 /**< Default */

/**
 * RegFifoTxBaseAddr
 */
#define SX1276_RF_LORA_FIFORXBASEADDR                         0x00 /**< Default */

/**
 * RegFifoRxCurrentAddr (Read Only)
 */

/**
 * RegIrqFlagsMask
 */
#define SX1276_RF_LORA_IRQFLAGS_RXTIMEOUT_MASK                0x80
#define SX1276_RF_LORA_IRQFLAGS_RXDONE_MASK                   0x40
#define SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR_MASK          0x20
#define SX1276_RF_LORA_IRQFLAGS_VALIDHEADER_MASK              0x10
#define SX1276_RF_LORA_IRQFLAGS_TXDONE_MASK                   0x08
#define SX1276_RF_LORA_IRQFLAGS_CADDONE_MASK                  0x04
#define SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL_MASK       0x02
#define SX1276_RF_LORA_IRQFLAGS_CADDETECTED_MASK              0x01

/**
 * RegIrqFlags
 */
#define SX1276_RF_LORA_IRQFLAGS_RXTIMEOUT                     0x80
#define SX1276_RF_LORA_IRQFLAGS_RXDONE                        0x40
#define SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR               0x20
#define SX1276_RF_LORA_IRQFLAGS_VALIDHEADER                   0x10
#define SX1276_RF_LORA_IRQFLAGS_TXDONE                        0x08
#define SX1276_RF_LORA_IRQFLAGS_CADDONE                       0x04
#define SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02
#define SX1276_RF_LORA_IRQFLAGS_CADDETECTED                   0x01

/**
 * RegFifoRxNbBytes (Read Only)
 */

/**
 * RegRxHeaderCntValueMsb (Read Only)
 */

/**
 * RegRxHeaderCntValueLsb (Read Only)
 */

/**
 * RegRxPacketCntValueMsb (Read Only)
 */

/**
 * RegRxPacketCntValueLsb (Read Only)
 */

/**
 * RegModemStat (Read Only)
 */
#define SX1276_RF_LORA_MODEMSTAT_RX_CR_MASK                   0x1F
#define SX1276_RF_LORA_MODEMSTAT_MODEM_STATUS_MASK            0xE0

/**
 * RegPktSnrValue (Read Only)
 */

/**
 * RegPktRssiValue (Read Only)
 */

/**
 * RegRssiValue (Read Only)
 */

/**
 * RegHopChannel (Read Only)
 */
#define SX1276_RF_LORA_HOPCHANNEL_PLL_LOCK_TIMEOUT_MASK       0x7F
#define SX1276_RF_LORA_HOPCHANNEL_PLL_LOCK_FAIL               0x80
#define SX1276_RF_LORA_HOPCHANNEL_PLL_LOCK_SUCCEED            0x00 /**< Default */

#define SX1276_RF_LORA_HOPCHANNEL_CRCONPAYLOAD_MASK           0xBF
#define SX1276_RF_LORA_HOPCHANNEL_CRCONPAYLOAD_ON             0x40
#define SX1276_RF_LORA_HOPCHANNEL_CRCONPAYLOAD_OFF            0x00 /**< Default */

#define SX1276_RF_LORA_HOPCHANNEL_CHANNEL_MASK                0x3F

/**
 * RegModemConfig1
 */
#define SX1276_RF_LORA_MODEMCONFIG1_BW_MASK                   0x0F
#define SX1276_RF_LORA_MODEMCONFIG1_BW_7_81_KHZ               0x00
#define SX1276_RF_LORA_MODEMCONFIG1_BW_10_41_KHZ              0x10
#define SX1276_RF_LORA_MODEMCONFIG1_BW_15_62_KHZ              0x20
#define SX1276_RF_LORA_MODEMCONFIG1_BW_20_83_KHZ              0x30
#define SX1276_RF_LORA_MODEMCONFIG1_BW_31_25_KHZ              0x40
#define SX1276_RF_LORA_MODEMCONFIG1_BW_41_66_KHZ              0x50
#define SX1276_RF_LORA_MODEMCONFIG1_BW_62_50_KHZ              0x60
#define SX1276_RF_LORA_MODEMCONFIG1_BW_125_KHZ                0x70 /**< Default */
#define SX1276_RF_LORA_MODEMCONFIG1_BW_250_KHZ                0x80
#define SX1276_RF_LORA_MODEMCONFIG1_BW_500_KHZ                0x90

#define SX1276_RF_LORA_MODEMCONFIG1_CODINGRATE_MASK           0xF1
#define SX1276_RF_LORA_MODEMCONFIG1_CODINGRATE_4_5            0x02
#define SX1276_RF_LORA_MODEMCONFIG1_CODINGRATE_4_6            0x04 /**< Default */
#define SX1276_RF_LORA_MODEMCONFIG1_CODINGRATE_4_7            0x06
#define SX1276_RF_LORA_MODEMCONFIG1_CODINGRATE_4_8            0x08

#define SX1276_RF_LORA_MODEMCONFIG1_IMPLICITHEADER_MASK       0xFE
#define SX1276_RF_LORA_MODEMCONFIG1_IMPLICITHEADER_ON         0x01
#define SX1276_RF_LORA_MODEMCONFIG1_IMPLICITHEADER_OFF        0x00 /**< Default */

/**
 * RegModemConfig2
 */
#define SX1276_RF_LORA_MODEMCONFIG2_SF_MASK                   0x0F
#define SX1276_RF_LORA_MODEMCONFIG2_SF_6                      0x60
#define SX1276_RF_LORA_MODEMCONFIG2_SF_7                      0x70 /**< Default */
#define SX1276_RF_LORA_MODEMCONFIG2_SF_8                      0x80
#define SX1276_RF_LORA_MODEMCONFIG2_SF_9                      0x90
#define SX1276_RF_LORA_MODEMCONFIG2_SF_10                     0xA0
#define SX1276_RF_LORA_MODEMCONFIG2_SF_11                     0xB0
#define SX1276_RF_LORA_MODEMCONFIG2_SF_12                     0xC0

#define SX1276_RF_LORA_MODEMCONFIG2_TXCONTINUOUSMODE_MASK     0xF7
#define SX1276_RF_LORA_MODEMCONFIG2_TXCONTINUOUSMODE_ON       0x08
#define SX1276_RF_LORA_MODEMCONFIG2_TXCONTINUOUSMODE_OFF      0x00

#define SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_MASK         0xFB
#define SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_ON           0x04
#define SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_OFF          0x00 /**< Default */

#define SX1276_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK       0xFC
#define SX1276_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB            0x00 /**< Default */

/**
 * RegSymbTimeoutLsb
 */
#define SX1276_RF_LORA_SYMBTIMEOUTLSB_SYMBTIMEOUT             0x64 /**< Default */

/**
 * RegPreambleLengthMsb
 */
#define SX1276_RF_LORA_PREAMBLELENGTHMSB                      0x00 /**< Default */

/**
 * RegPreambleLengthLsb
 */
#define SX1276_RF_LORA_PREAMBLELENGTHLSB                      0x08 /**< Default */

/**
 * RegPayloadLength
 */
#define SX1276_RF_LORA_PAYLOADLENGTH                          0x0E /**< Default */

/**
 * RegPayloadMaxLength
 */
#define SX1276_RF_LORA_PAYLOADMAXLENGTH                       0xFF /**< Default */

/**
 * RegHopPeriod
 */
#define SX1276_RF_LORA_HOPPERIOD_FREQFOPPINGPERIOD            0x00 /**< Default */

/**
 * RegFifoRxByteAddr (Read Only)
 */

/**
 * RegModemConfig3
 */
#define SX1276_RF_LORA_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK  0xF7
#define SX1276_RF_LORA_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON    0x08
#define SX1276_RF_LORA_MODEMCONFIG3_LOWDATARATEOPTIMIZE_OFF   0x00 /**< Default */

#define SX1276_RF_LORA_MODEMCONFIG3_AGCAUTO_MASK              0xFB
#define SX1276_RF_LORA_MODEMCONFIG3_AGCAUTO_ON                0x04 /**< Default */
#define SX1276_RF_LORA_MODEMCONFIG3_AGCAUTO_OFF               0x00

/**
 * RegFeiMsb (Read Only)
 */

/**
 * RegFeiMid (Read Only)
 */

/**
 * RegFeiLsb (Read Only)
 */

/**
 * RegRssiWideband (Read Only)
 */

/**
 * RegDetectOptimize
 */
#define SX1276_RF_LORA_DETECTIONOPTIMIZE_MASK                 0xF8
#define SX1276_RF_LORA_DETECTIONOPTIMIZE_SF7_TO_SF12          0x03 /**< Default */
#define SX1276_RF_LORA_DETECTIONOPTIMIZE_SF6                  0x05

/**
 * RegInvertIQ
 */
#define SX1276_RF_LORA_INVERTIQ_RX_MASK                       0xBF
#define SX1276_RF_LORA_INVERTIQ_RX_OFF                        0x00
#define SX1276_RF_LORA_INVERTIQ_RX_ON                         0x40
#define SX1276_RF_LORA_INVERTIQ_TX_MASK                       0xFE
#define SX1276_RF_LORA_INVERTIQ_TX_OFF                        0x01
#define SX1276_RF_LORA_INVERTIQ_TX_ON                         0x00

/**
 * RegDetectionThreshold
 */
#define SX1276_RF_LORA_DETECTIONTHRESH_SF7_TO_SF12            0x0A /**< Default */
#define SX1276_RF_LORA_DETECTIONTHRESH_SF6                    0x0C

/**
 * RegInvertIQ2
 */
#define SX1276_RF_LORA_INVERTIQ2_ON                           0x19
#define SX1276_RF_LORA_INVERTIQ2_OFF                          0x1D

/**
 * RegDioMapping1
 */
#define SX1276_RF_LORA_DIOMAPPING1_DIO0_MASK                  0x3F
#define SX1276_RF_LORA_DIOMAPPING1_DIO0_00                    0x00  /**< Default */
#define SX1276_RF_LORA_DIOMAPPING1_DIO0_01                    0x40
#define SX1276_RF_LORA_DIOMAPPING1_DIO0_10                    0x80
#define SX1276_RF_LORA_DIOMAPPING1_DIO0_11                    0xC0

#define SX1276_RF_LORA_DIOMAPPING1_DIO1_MASK                  0xCF
#define SX1276_RF_LORA_DIOMAPPING1_DIO1_00                    0x00  /**< Default */
#define SX1276_RF_LORA_DIOMAPPING1_DIO1_01                    0x10
#define SX1276_RF_LORA_DIOMAPPING1_DIO1_10                    0x20
#define SX1276_RF_LORA_DIOMAPPING1_DIO1_11                    0x30

#define SX1276_RF_LORA_DIOMAPPING1_DIO2_MASK                  0xF3
#define SX1276_RF_LORA_DIOMAPPING1_DIO2_00                    0x00  /**< Default */
#define SX1276_RF_LORA_DIOMAPPING1_DIO2_01                    0x04
#define SX1276_RF_LORA_DIOMAPPING1_DIO2_10                    0x08
#define SX1276_RF_LORA_DIOMAPPING1_DIO2_11                    0x0C

#define SX1276_RF_LORA_DIOMAPPING1_DIO3_MASK                  0xFC
#define SX1276_RF_LORA_DIOMAPPING1_DIO3_00                    0x00  /**< Default */
#define SX1276_RF_LORA_DIOMAPPING1_DIO3_01                    0x01
#define SX1276_RF_LORA_DIOMAPPING1_DIO3_10                    0x02
#define SX1276_RF_LORA_DIOMAPPING1_DIO3_11                    0x03

/**
 * RegDioMapping2
 */
#define SX1276_RF_LORA_DIOMAPPING2_DIO4_MASK                  0x3F
#define SX1276_RF_LORA_DIOMAPPING2_DIO4_00                    0x00  /**< Default */
#define SX1276_RF_LORA_DIOMAPPING2_DIO4_01                    0x40
#define SX1276_RF_LORA_DIOMAPPING2_DIO4_10                    0x80
#define SX1276_RF_LORA_DIOMAPPING2_DIO4_11                    0xC0

#define SX1276_RF_LORA_DIOMAPPING2_DIO5_MASK                  0xCF
#define SX1276_RF_LORA_DIOMAPPING2_DIO5_00                    0x00  /**< Default */
#define SX1276_RF_LORA_DIOMAPPING2_DIO5_01                    0x10
#define SX1276_RF_LORA_DIOMAPPING2_DIO5_10                    0x20
#define SX1276_RF_LORA_DIOMAPPING2_DIO5_11                    0x30

#define SX1276_RF_LORA_DIOMAPPING2_MAP_MASK                   0xFE
#define SX1276_RF_LORA_DIOMAPPING2_MAP_PREAMBLEDETECT         0x01
#define SX1276_RF_LORA_DIOMAPPING2_MAP_RSSI                   0x00  /**< Default */

/**
 * RegVersion (Read Only)
 */

/**
 * RegPllHop
 */
#define SX1276_RF_LORA_PLLHOP_FASTHOP_MASK                    0x7F
#define SX1276_RF_LORA_PLLHOP_FASTHOP_ON                      0x80
#define SX1276_RF_LORA_PLLHOP_FASTHOP_OFF                     0x00 /**< Default */

/**
 * RegTcxo
 */
#define SX1276_RF_LORA_TCXO_TCXOINPUT_MASK                    0xEF
#define SX1276_RF_LORA_TCXO_TCXOINPUT_ON                      0x10
#define SX1276_RF_LORA_TCXO_TCXOINPUT_OFF                     0x00  /**< Default */

/**
 * RegPaDac
 */
#define SX1276_RF_LORA_PADAC_20DBM_MASK                       0xF8
#define SX1276_RF_LORA_PADAC_20DBM_ON                         0x07
#define SX1276_RF_LORA_PADAC_20DBM_OFF                        0x04  /**< Default */

/**
 * RegFormerTemp
 */

/**
 * RegBitrateFrac
 */
#define RF_BITRATEFRAC_MASK                         0xF0

/**
 * RegAgcRef
 */

/**
 * RegAgcThresh1
 */

/**
 * RegAgcThresh2
 */

/**
 * RegAgcThresh3
 */

/**
 * RegPll
 */
#define RF_PLL_BANDWIDTH_MASK                       0x3F
#define RF_PLL_BANDWIDTH_75                         0x00
#define RF_PLL_BANDWIDTH_150                        0x40
#define RF_PLL_BANDWIDTH_225                        0x80
#define RF_PLL_BANDWIDTH_300                        0xC0  /**< Default */


#ifdef __cplusplus
}
#endif

#endif /* SX1276_REGS_LORA_H */

/** @} */
